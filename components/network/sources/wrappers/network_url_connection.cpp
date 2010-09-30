#include "shared.h"

using namespace network;

namespace
{

/*
    ���������
*/

const char*  DEFAULT_URL              = "http://localhost/";  //URL �� ���������
const char*  DEFAULT_CONTENT_TYPE     = "text";               //��� �������� �� ���������
const char*  DEFAULT_CONTENT_ENCODING = "utf-8";              //��������� �������� �� ���������
const size_t BLOCK_SIZE               = 16384;                //������ ������ ����� �� ���������

/*
    ��������������� ���������
*/

///���� ������ ������
struct Block: public xtl::reference_counter
{
  char buffer [BLOCK_SIZE]; //����� ������
};

typedef xtl::intrusive_ptr<Block> BlockPtr;
typedef stl::list<BlockPtr>       BlockList;

///������ ������
class Buffer
{
  public:
    Buffer (syslib::Mutex& in_mutex)
      : mutex (in_mutex)
      , total_size (0)
      , offset (0)
      , put_finished (false)
    {
    }
    
///��������� ����� ������
    size_t Available ()
    {
      syslib::Lock lock (mutex);
      
      return total_size - offset;
    }
    
///����� ����� ������
    size_t TotalSize ()
    {
      syslib::Lock lock (mutex);
      
      return total_size;
    }
    
///����������� ��������� ������ � �����
    void FinishPut ()
    {
      syslib::Lock lock (mutex);
      
      put_finished = true;
      
      condition.NotifyAll ();
    }
    
///��������� ������ � �����
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
        size_t block_offset         = total_size % BLOCK_SIZE, //?????
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
    
///��������� ������ �� ������
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
          //�������� ������
          
        size_t available;
        
        while ((available = total_size - offset) == 0)
        {
          if (put_finished)
            return result;
            
          condition.Wait (mutex);
        }
        
          //��������� �����
          
        if (blocks.empty ())
          throw xtl::format_operation_exception ("", "Internal error: empty list");
        
        BlockPtr block = blocks.front ();
        
          //������ ��������
          
        size_t block_offset         = offset % BLOCK_SIZE,
               block_available_size = BLOCK_SIZE - block_offset,
               read_size            = size < block_available_size ? size : block_available_size;
               
        if (read_size > available)
          read_size = available;
        
          //������ ������
          
        memcpy (dst, block->buffer + block_offset, read_size);
        
          //���������� ������
          
        if (read_size == block_available_size)
          blocks.pop_front ();
        
          //���������� ����������
          
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
    syslib::Mutex&    mutex;        //����������
    syslib::Condition condition;    //������� ����� ������
    size_t            total_size;   //����� ����� ������
    size_t            offset;       //�������� ��� ������� ����� ������
    BlockList         blocks;       //����� ������
    bool              put_finished; //��������� ������ � ������ ���������
};

}

/*
    �������� ���������� URL ����������
*/

struct UrlConnection::Impl: public xtl::reference_counter, public IUrlStream::IListener
{
  syslib::Mutex             mutex;            //���������� ����������
  network::Url              url;              //URL �������
  stl::auto_ptr<IUrlStream> stream;           //����� URL ������
  stl::string               content_type;     //��� ��������
  stl::string               content_encoding; //���������
  size_t                    content_length;   //����� ��������
  Buffer                    send_buffer;      //����� �����
  Buffer                    recv_buffer;      //����� ��������
  bool                      send_closed;      //����� �������� ������ ������
  bool                      recv_closed;      //����� ����� ������ ������
  bool                      recv_headers;     //��������� ������ ��������

///������������
  Impl ()
    : url (DEFAULT_URL)
    , content_type (DEFAULT_CONTENT_TYPE)
    , content_encoding (DEFAULT_CONTENT_ENCODING)
    , content_length (0)
    , send_buffer (mutex)
    , recv_buffer (mutex)
    , send_closed (false)
    , recv_closed (false)
    , recv_headers (false)
  {
  }
  
  Impl (const network::Url& in_url, const char* params)
    : url (in_url)
    , content_length (0)
    , send_buffer (mutex)
    , recv_buffer (mutex)
    , send_closed (false)
    , recv_closed (false)
    , recv_headers (false)    
  {
    stream = stl::auto_ptr<IUrlStream> (UrlStreamManagerSingleton::Instance ()->CreateStream (url.ToString (), params, *this));
    
    if (!&*stream)
      throw xtl::format_operation_exception ("", "Internal error: Null stream returned by stream manager");
  }
  
///����������
  ~Impl ()
  {
    mutex.Lock (); //��� ������ Unlock, ��������� Mutex ����� �����
  }
  
///��������� ����������
  void ReceiveHeaders ()
  {
    syslib::Lock lock (mutex);
    
    if (recv_headers)
      return;
    
    content_type     = stream->GetContentType ();
    content_encoding = stream->GetContentEncoding ();
    content_length   = stream->GetContentLength ();
    recv_headers     = true;
  }
  
///��������� ������� ��������� ������
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
  
///����� ����� ������
  void FinishReceiveData ()
  {
    try
    {
      recv_buffer.FinishPut ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("network::UrlConnection::Impl::FinishReceiveData");
      throw;
    }
  }
  
///���� ������
  size_t Receive (void* buffer, size_t size)
  {
    try
    {
      {
        syslib::Lock lock (mutex);
      
        if (!&*stream)
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
  
///��������� ������� ������� ������
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
  
///������� ������
  size_t Send (const void* buffer, size_t size)
  {
    try
    {
      {
        syslib::Lock lock (mutex);    
      
        if (!&*stream)
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
    ������������ / ���������� / ������������
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
    ������������� �������
*/

const network::Url& UrlConnection::Url () const
{
  syslib::Lock lock (impl->mutex);

  return impl->url;
}

/*
    �������� ����������
*/

void UrlConnection::Close ()
{
  syslib::Lock lock (impl->mutex);
  
  UrlConnection ().Swap (*this);
}

bool UrlConnection::IsClosed () const
{
  syslib::Lock lock (impl->mutex);

  return &*impl->stream == 0;
}

/*
    �������� ������� �������� ������
*/

bool UrlConnection::IsReceiveClosed () const
{
  syslib::Lock lock (impl->mutex);

  return impl->recv_closed;
}

bool UrlConnection::IsSendClosed () const
{
  syslib::Lock lock (impl->mutex);

  return impl->send_closed;
}

void UrlConnection::CloseReceive ()
{
  syslib::Lock lock (impl->mutex);
  
  impl->recv_closed = true;
}

void UrlConnection::CloseSend ()
{
  syslib::Lock lock (impl->mutex);
  
  impl->send_closed = true;
  
  impl->send_buffer.FinishPut ();
}

/*
    ��������� �����������
*/

size_t UrlConnection::ContentLength () const
{
  impl->ReceiveHeaders ();

  return impl->content_length;
}

const char* UrlConnection::ContentEncoding () const
{
  impl->ReceiveHeaders ();

  return impl->content_encoding.c_str ();
}

const char* UrlConnection::ContentType () const
{
  impl->ReceiveHeaders ();

  return impl->content_type.c_str ();
}

/*
    ������ / ������ ������
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
    ������� ���� ����� / ��������
*/

//���������� ���� ��������� ��� ������ ��� ����������
size_t UrlConnection::ReceiveAvailable () const
{
  syslib::Lock lock (impl->mutex);

  return impl->recv_buffer.Available ();
}

//����� ����� ���������� ����
size_t UrlConnection::ReceivedDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->recv_buffer.TotalSize ();
}

//����� ����� ���������� ����
size_t UrlConnection::SendDataSize () const
{
  syslib::Lock lock (impl->mutex);

  return impl->send_buffer.TotalSize ();
}

/*
    �����
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
