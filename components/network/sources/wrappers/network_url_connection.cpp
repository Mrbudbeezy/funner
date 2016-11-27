#include "shared.h"

using namespace network;

namespace
{

/*
    Константы
*/

const char*  DEFAULT_URL              = "http://localhost/";  //URL по умолчанию
const char*  DEFAULT_CONTENT_TYPE     = "text";               //тип контента по умолчанию
const char*  DEFAULT_CONTENT_ENCODING = "utf-8";              //кодировка контента по умолчанию
const size_t BLOCK_SIZE               = 16384;                //размер буфера блока по умолчанию

/*
    Вспомогательные структуры
*/

///Блок буфера данных
struct Block: public xtl::reference_counter
{
  char buffer [BLOCK_SIZE]; //буфер данных
};

typedef xtl::intrusive_ptr<Block> BlockPtr;
typedef stl::list<BlockPtr>       BlockList;

///Буфера данных
class Buffer
{
  public:
    Buffer ()
      : total_size (0)
      , offset (0)
      , put_finished (false)
    {
    }
    
///Доступный объём данных
    size_t Available ()
    {
      syslib::Lock lock (mutex);
      
      return total_size - offset;
    }
    
///Общий объём данных
    size_t TotalSize ()
    {
      syslib::Lock lock (mutex);
      
      return total_size;
    }
    
///Прекращение помещения данных в буфер
    void FinishPut ()
    {
      syslib::Lock lock (mutex);
      
      put_finished = true;
      
      condition.NotifyAll ();
    }
    
///Помещение данных в буфер
    void Put (const void* buffer, size_t size)
    {
      if (!buffer && size)
        throw xtl::make_null_argument_exception ("network::BufferPut", "buffer");
        
      syslib::Lock lock (mutex);
      
      if (put_finished)
        throw xtl::format_operation_exception ("network::Buffer::Put", "Buffer has being closed for data writing");

      const char* src = reinterpret_cast<const char*> (buffer);
        
      while (size)
      {
        size_t block_offset         = total_size % BLOCK_SIZE,
               available_block_size = BLOCK_SIZE - block_offset,
               write_size           = size < available_block_size ? size : available_block_size;
               
        BlockPtr block = block_offset == 0 ? BlockPtr (new Block, false) : blocks.back ();
        
        memcpy (block->buffer + block_offset, src, write_size);
              
        if (block != blocks.back ())
          blocks.push_back (block);
          
        size       -= write_size;
        src        += write_size;
        total_size += write_size;
      }
      
      condition.NotifyAll ();
    }
    
///Получение данных из буфера
    size_t Get (void* buffer, size_t size)
    {
      syslib::Lock lock (mutex);    
      
      if (!buffer && size)
        throw xtl::make_null_argument_exception ("network::Buffer::Get", "buffer");

      if (!size)
        return 0;
        
      size_t result = 0;
      
      char* dst = reinterpret_cast<char*> (buffer);

      while (size)
      {
          //ожидание данных
          
        size_t available;
        
        while ((available = total_size - offset) == 0)
        {
          if (put_finished)
            return result;
            
          condition.Wait (mutex);
        }
        
          //получение блока
          
        if (blocks.empty ())
          throw xtl::format_operation_exception ("", "Internal error: empty list");
        
        BlockPtr block = blocks.front ();
        
          //расчёт смещений
          
        size_t block_offset         = offset % BLOCK_SIZE,
               block_available_size = BLOCK_SIZE - block_offset,
               read_size            = size < block_available_size ? size : block_available_size;
               
        if (read_size > available)
          read_size = available;
        
          //чтение данных
          
        memcpy (dst, block->buffer + block_offset, read_size);
        
          //обновление списка
          
        if (read_size == block_available_size)
          blocks.pop_front ();
        
          //обновление указателей
          
        result += read_size;
        size   -= read_size;
        dst    += read_size;
        offset += read_size;
      }
      
      return result;
    }
  
  private:
    Buffer (const Buffer&); //no impl
    Buffer& operator = (const Buffer&); //no impl
  
  private:
    syslib::Mutex     mutex;        //блокировка
    syslib::Condition condition;    //событие приёма данных
    size_t            total_size;   //общий объём данных
    size_t            offset;       //смещение для первого блока данных
    BlockList         blocks;       //блоки данных
    bool              put_finished; //помещение данных в буфера завершено
};

}

/*
    Описание реализации URL соединения
*/

struct UrlConnection::Impl: public xtl::reference_counter, public IUrlStream::IListener
{
  syslib::Mutex             mutex;            //блокировка соединения
  network::Url              url;              //URL ресурса
  stl::auto_ptr<IUrlStream> stream;           //поток URL данных
  stl::string               status;           //статус
  stl::string               content_type;     //тип контента
  stl::string               content_encoding; //кодировка
  size_t                    content_length;   //длина контента
  Buffer                    send_buffer;      //буфер приёма
  Buffer                    recv_buffer;      //буфер передачи
  volatile bool             send_closed;      //канал отправки данных закрыт
  volatile bool             recv_closed;      //канал приёма данных закрыт
  volatile bool             recv_headers;     //заголовки потока получены

///Конструкторы
  Impl ()
    : url (DEFAULT_URL)
    , content_type (DEFAULT_CONTENT_TYPE)
    , content_encoding (DEFAULT_CONTENT_ENCODING)
    , content_length (0)
    , send_closed (false)
    , recv_closed (false)
    , recv_headers (false)
  {
  }
  
  Impl (const network::Url& in_url, const char* params)
    : url (in_url)
    , content_length (0)
    , send_closed (false)
    , recv_closed (false)
    , recv_headers (false)    
  {
    stream = stl::auto_ptr<IUrlStream> (UrlStreamManagerSingleton::Instance ()->CreateStream (url.ToString (), params, *this));
    
    if (!stream)
      throw xtl::format_operation_exception ("", "Internal error: Null stream returned by stream manager");
  }
  
///Деструктор
  ~Impl ()
  {
    mutex.Lock (); //без вызова Unlock, поскольку Mutex будет удалён

    stream.reset ();
  }
  
///Получение заголовков
  void ReceiveHeaders ()
  {
    try
    {
      if (recv_headers)
        return;
      
      syslib::Lock lock (mutex);

      content_type     = stream->GetContentType ();
      content_encoding = stream->GetContentEncoding ();
      content_length   = stream->GetContentLength ();
      recv_headers     = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("UrlConnection::Impl::ReceiveHeaders");
      throw;
    }
  }
  
///Обработка события получения данных
  void WriteReceivedData (const void* buffer, size_t size)
  {
    try
    {
      recv_buffer.Put (buffer, size);
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::WriteReceivedData");
      throw;
    }
  }
  
///Конец приёма данных
  void FinishReceiveData ()
  {
    try
    {
      recv_buffer.FinishPut ();

      recv_closed = true;
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::FinishReceiveData");
      throw;
    }
  }
  
///Приём данных
  size_t Receive (void* buffer, size_t size)
  {
    try
    {
      {
        syslib::Lock lock (mutex);
      
        if (!stream)
          throw xtl::format_operation_exception ("", "Can't receive data from closed URL stream");
      }
      
      return recv_buffer.Get (buffer, size);
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::Receive");
      throw;
    }
  }
  
///Обработка события отсылки данных
  size_t ReadSendData (void* buffer, size_t size)
  {
    try
    {
      return send_buffer.Get (buffer, size);
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::ReadSendData");
      throw;
    }
  }
  
///Отсылка данных
  size_t Send (const void* buffer, size_t size)
  {
    try
    {
      {
        syslib::Lock lock (mutex);    
      
        if (!stream)
          throw xtl::format_operation_exception ("", "Can't send data to closed URL stream");
      }

      send_buffer.Put (buffer, size); 

      return size;
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::Send");
      throw;
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

UrlConnection::UrlConnection ()
  : impl (new Impl)
{
}

UrlConnection::UrlConnection (const network::Url& url, const char* params)
{
  try
  {
    impl = new Impl (url, params);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::UrlConnection(const network::Url&,const char*)");
    throw;
  }
}

UrlConnection::UrlConnection (const UrlConnection& connection)
  : impl (connection.impl)
{
  syslib::Lock lock (impl->mutex);

  addref (impl);
}

UrlConnection::~UrlConnection ()
{
  release (impl);
}

UrlConnection& UrlConnection::operator = (const UrlConnection& connection)
{
  UrlConnection (connection).Swap (*this);

  return *this;
}

/*
    Идентификатор ресурса
*/

const network::Url& UrlConnection::Url () const
{
  syslib::Lock lock (impl->mutex);

  return impl->url;
}

/*
    Закрытие соединения
*/

void UrlConnection::Close ()
{
  syslib::Lock lock (impl->mutex);
  
  UrlConnection ().Swap (*this);
}

bool UrlConnection::IsClosed () const
{
  syslib::Lock lock (impl->mutex);

  return !impl->stream;
}

/*
    Закрытие каналов передачи данных
*/

bool UrlConnection::IsReceiveClosed () const
{
  return impl->recv_closed;
}

bool UrlConnection::IsSendClosed () const
{
  return impl->send_closed;
}

void UrlConnection::CloseReceive ()
{
  impl->recv_closed = true;
}

void UrlConnection::CloseSend ()
{
  impl->send_closed = true;
  
  impl->send_buffer.FinishPut ();
}

/*
    Параметры содержимого
*/

size_t UrlConnection::ContentLength () const
{  
  try
  {
    impl->ReceiveHeaders ();

    return impl->content_length;
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::ContentLength");
    throw;
  }    
}

const char* UrlConnection::ContentEncoding () const
{
  try
  {
    impl->ReceiveHeaders ();

    return impl->content_encoding.c_str ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::ContentEncoding");
    throw;
  }
}

const char* UrlConnection::ContentType () const
{
  try
  {
    impl->ReceiveHeaders ();

    return impl->content_type.c_str ();    
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::ContentType");
    throw;
  }    
}

const char* UrlConnection::Status () const
{
  try
  {
    syslib::Lock lock (impl->mutex);

    if (!impl->stream)
      return "Closed";
      
    impl->status = impl->stream->GetStatus ();

    return impl->status.c_str ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::Status");
    throw;
  }    
}

/*
    Чтение / запись данных
*/

size_t UrlConnection::Receive (void* buffer, size_t size)
{
  try
  {
    return impl->Receive (buffer, size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::Receive");
    throw;
  }  
}

size_t UrlConnection::Send (const void* buffer, size_t size)
{
  try
  {
    return impl->Send (buffer, size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::UrlConnection::Send");
    throw;
  }
}

/*
    Размеры окна приёма / передачи
*/

//количество байт доступных для чтения без блокировки
size_t UrlConnection::ReceiveAvailable () const
{
  syslib::Lock lock (impl->mutex);

  return impl->recv_buffer.Available ();
}

//общее число полученных байт
size_t UrlConnection::ReceivedDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->recv_buffer.TotalSize ();
}

//общее число переданных байт
size_t UrlConnection::SendDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->send_buffer.TotalSize ();
}

/*
    Обмен
*/

void UrlConnection::Swap (UrlConnection& connection)
{
  syslib::Lock lock1 (impl->mutex);
  syslib::Lock lock2 (connection.impl->mutex);

  stl::swap (impl, connection.impl);
}

namespace network
{

void swap (UrlConnection& connection1, UrlConnection& connection2)
{
  connection1.Swap (connection2);
}

}

/*
    HttpPostConnection
*/

HttpPostConnection::HttpPostConnection (const network::Url& url, size_t send_data_size)
  : UrlConnection (url, common::format ("method=post send_size=%u", send_data_size).c_str ())
{
}
