#include "shared.h"

using namespace network;

//TODO: ��������� ������ ���������
//TODO: ������� select-based ���������
//TODO: ������������ threadpool

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' is used in base member initializer list
#endif

namespace
{

/*
    ���������
*/

const size_t DEFAULT_ASYNC_RECEIVE_BUFFER_RECEIVE_SIZE = 16384;
const size_t ASYNC_SENDING_QUEUE_MAX_SIZE              = 16;
const size_t ASYNC_TIMEOUT_IN_MILLISECONDS             = 1000;

/*
    ����
*/

typedef xtl::uninitialized_storage<char> Buffer;

/*
    ���������� ������������ �������
*/

///���� � �������
struct Block
{
  typedef xtl::com_ptr<Block> Pointer;

  xtl::reference_counter ref_count; //���������� ������ �� �����
  size_t                 size;      //������ ������
  char                   data [1];  //������

///�����������
  Block (size_t in_size, const void* in_data)
    : size (in_size)
  {
    memcpy (data, in_data, size);
  }

///�������� ������
  static Pointer Create (size_t size, const void* data)
  {
    Block* block = new (stl::allocate<char> (sizeof (Block) - sizeof (char) + size)) Block (size, data);

    return Pointer (block, false);
  }

///������� ������
  void AddRef () { ref_count.increment (); }

  void Release ()
  {
    if (ref_count.decrement ())
    {
      const size_t buf_size = size;

      stl::destroy (this);
      stl::deallocate ((void*)this, buf_size);
    }
  }
};

typedef Block::Pointer BlockPtr;

///������� � ������������ ������� �� ���������� �����
class SharedQueue
{
  public:
///�����������
    SharedQueue (size_t in_max_size)
      : max_size (in_max_size)
    {      
    }
      
///����������
    ~SharedQueue ()
    {
      mutex.Lock ();
    }

///��������� ������ � �������
    bool Push (const BlockPtr& block, size_t timeout_in_milliseconds)
    {
      syslib::Lock lock (mutex);

      size_t start_time = common::milliseconds ();

      while (blocks.size () >= max_size)
      {
        size_t delay = start_time + timeout_in_milliseconds - common::milliseconds ();

        if (delay > timeout_in_milliseconds || !delay)
          return false;

        full_condition.TryWait (mutex, delay);        
      }

      blocks.push_back (block);

      empty_condition.NotifyOne ();

      return true;
    }

///���������� ������ �� �������
    BlockPtr Pop (size_t timeout_in_milliseconds)
    {
      syslib::Lock lock (mutex);

      size_t start_time = common::milliseconds ();

      while (blocks.empty ())      
      {
        size_t delay = start_time + timeout_in_milliseconds - common::milliseconds ();

        if (delay > timeout_in_milliseconds || !delay)
          return BlockPtr ();

        empty_condition.TryWait (mutex, delay);
      }

      BlockPtr result = blocks.front ();

      blocks.pop_front ();

      full_condition.NotifyOne ();

      return result;
    }

  private:
    typedef stl::deque<BlockPtr> BlockQueue;

  private:
    syslib::Mutex     mutex;           //���������� ������� � ������ �������
    syslib::Condition full_condition;  //������� ��������� ��������� �������
    syslib::Condition empty_condition; //������� ��������� ��������� �������
    BlockQueue        blocks;          //�����
    size_t            max_size;        //������������ ������ �������
};

///����������� ��������� ������
class AsyncProcessor
{
  public:
///�����������
    AsyncProcessor (const char* name)
      : thread (name, xtl::bind (&AsyncProcessor::Run, this))
      , log (name)
      , stop_requested (false)
    {
    }

///����������
    ~AsyncProcessor ()
    {
      try
      {
        Stop ();
      }
      catch (...)
      {
      }
    }

///��������� ���������
    void Stop ()
    {
      stop_requested = true;

      thread.Join ();      
    }

    void StopRequest () { stop_requested = true; }

///��������� �� ���������
    bool IsStopRequested () const { return stop_requested; }

  private:
///��������� �������
    int Run ()
    {
      while (!stop_requested)
      {
        try
        {
          DoStep ();
        }
        catch (std::exception& e)
        {
          log.Printf ("%s\n    at network::AsyncProcessor::Run", e.what ());
        }
        catch (...)
        {
          log.Printf ("unknown exception\n    at network::AsyncProcessor::Run");
        }
      }

      return 0;
    }

///�������� ��������� �������
    virtual void DoStep () = 0;

  private:    
    syslib::Thread thread;         //���� ������� ������
    common::Log    log;            //����� ����������� ����������������
    volatile bool  stop_requested; //��������� ���������
};

///����������� ������� ������
class AsyncSender: public AsyncProcessor
{
  public:
///�����������
    AsyncSender (Socket& in_socket)
      : AsyncProcessor ("network.AsyncSender")
      , socket (in_socket)
      , queue (ASYNC_SENDING_QUEUE_MAX_SIZE)
    {
    }

///����������
    ~AsyncSender ()
    {
      try
      {
        Stop ();
      }
      catch (...)
      {
      }
    }

///������ �� ������� ������
    void SendRequest (const void* buffer, size_t size)
    {
      try
      { 
        while (!queue.Push (Block::Create (size, buffer), ASYNC_TIMEOUT_IN_MILLISECONDS) && !IsStopRequested ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::AsyncSender");
        throw;
      }
    }

  private:
///�������� ��������� �������
    void DoStep ()
    {
      BlockPtr block = queue.Pop (ASYNC_TIMEOUT_IN_MILLISECONDS);  

      if (!block)
        return;

      size_t size = block->size;
      char*  data = block->data;

      while (size)
      {
        if (IsStopRequested ())
          break;

        size_t send_size = socket.Send (data, size, ASYNC_TIMEOUT_IN_MILLISECONDS);        

        if (!send_size)
        {
          if (socket.IsSendClosed ())
            StopRequest ();

          return;
        }

        size -= send_size;
      }
    }

  private:    
    Socket&     socket; //�����
    SharedQueue queue;  //������� �������
};

///����������� ����� ������
class AsyncReceiver: public AsyncProcessor, public common::Lockable
{
  public:
///�����������
    AsyncReceiver (Socket& in_socket, const TcpClient::AsyncReceivingHandler& in_handler)
      : AsyncProcessor ("network.AsyncReceiver")
      , socket (in_socket)
      , handler (in_handler)
    {
      buffer.resize (DEFAULT_ASYNC_RECEIVE_BUFFER_RECEIVE_SIZE);
    }

///����������
    ~AsyncReceiver ()
    {
      try
      {
        Stop ();

        Lock ();
      }
      catch (...)
      {
      }
    }

  private:
///�������� ��������� �������
    void DoStep ()
    {
      size_t recv_size = socket.Receive (buffer.data (), buffer.size (), ASYNC_TIMEOUT_IN_MILLISECONDS);

      if (!recv_size)
      {
        if (socket.IsReceiveClosed ())
          StopRequest ();

        return;
      }

      common::Lock lock (*this);

      handler (buffer.data (), recv_size);
    }

  private:    
    Socket&                          socket;  //�����
    Buffer                           buffer;  //����� �������
    TcpClient::AsyncReceivingHandler handler; //���������� ��������� ������
};

}

/*
    �������� ���������� �������
*/

typedef xtl::signal<void (const void*, size_t)> AsyncDataReceivingSignal;

struct TcpClient::Impl: public xtl::reference_counter, public common::Lockable
{
  Socket                       socket;                    //�����
  Buffer                       recv_buffer;               //����� � ��������� �������
  char*                        recv_start;                //������ �������� ������
  char*                        recv_finish;               //����� �������� ������
  stl::auto_ptr<AsyncReceiver> async_receiver;            //����������� ���� ������
  stl::auto_ptr<AsyncSender>   async_sender;              //����������� ������� ������
  AsyncDataReceivingSignal     async_receive_data_signal; //������ ���������� � ��������� ������
  
///�����������
  Impl ()
  {  
    recv_start = recv_finish = recv_buffer.data ();
  }
  
///����������
  ~Impl ()
  {
    try
    {
      Close ();
    }
    catch (...)
    {      
    }
  }
  
///�������� ������
  void Close ()
  {
    common::Lock lock (*this);

    async_sender.reset ();
    async_receiver.reset ();

    socket.Close ();
    recv_buffer.resize (0);
    
    recv_start = recv_finish = recv_buffer.data ();
  }
  
///�������� ������
  size_t Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return 0;
      
    char*  pos       = (char*)buffer;
    size_t recv_size = 0;

    {
      common::Lock lock (*this);

      size_t received = recv_finish - recv_start;
      
      if (received)
      {
        size_t copy_size = stl::min (received, size);
        
        memcpy (pos, recv_start, copy_size);
        
        pos        += copy_size;
        recv_start += copy_size;
        recv_size  += copy_size;        
        size       -= copy_size;
      }
    }

    if (size)
      recv_size += socket.Receive (pos, size, timeout_in_milliseconds);     

    return recv_size;
  }
  
  bool ReceiveExactly (void* buffer, size_t size, size_t timeout_in_milliseconds)
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");
      
    if (!size)
      return true;

    char* pos = (char*)buffer;
    
    size_t start_time = common::milliseconds (),
           recv_size  = 0;

    try
    {
      while (size)
      {
        size_t time_spent = common::milliseconds () - start_time;

        if (time_spent > timeout_in_milliseconds)
          throw xtl::format_operation_exception ("", "Timeout in ReceiveExactly");

        size_t cur_recv_size = 0;

        try
        {
          cur_recv_size = Receive (pos, size, timeout_in_milliseconds - time_spent);
        }
        catch (...)
        {
        }
        
        if (!cur_recv_size && socket.IsReceiveClosed () && !socket.ReceiveAvailable ())
          throw xtl::format_operation_exception ("", "Connection closed by counterparty");
       
        recv_size += cur_recv_size;
        pos       += cur_recv_size;
        size      -= cur_recv_size;        
      }
      
      return true;
    }
    catch (...)
    {
      common::Lock lock (*this);

      if (recv_size && recv_start != recv_finish)
        throw xtl::format_operation_exception ("network::TcpClient::Impl::ReceiveExactly", "Internal error: recv_size && recv_start != recv_finish");

      if (recv_size)
      {
        if (recv_size > recv_buffer.size ())
          recv_buffer.resize (recv_size, false);
        
        recv_start  = recv_buffer.data ();
        recv_finish = recv_start + recv_size;
        
        memcpy (recv_start, pos - recv_size, recv_size);
      }

      return false;
    }
  }

///����������� ���������� ��������� ������
  void OnAsyncDataReceived (const void* data, size_t size)
  {
    common::Lock lock (*this);
  
    async_receive_data_signal (data, size);
  }
};

/*
    ������������ / ���������� / ������������
*/

TcpClient::TcpClient ()
  : impl (new Impl)
{
}

TcpClient::TcpClient (const SocketAddress& address, size_t timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (address, timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const SocketAddress&,size_t,bool)");
    throw;
  }
}

TcpClient::TcpClient (const InetAddress& address, unsigned short port, size_t timeout_in_milliseconds, bool tcp_no_delay)
  : impl (new Impl)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::TcpClient(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

TcpClient::TcpClient (const TcpClient& client)
  : impl (client.impl)
{
  addref (impl);
}

TcpClient::~TcpClient ()
{
  release (impl);
}

TcpClient& TcpClient::operator = (const TcpClient& client)
{
  TcpClient (client).Swap (*this);
  
  return *this;
}

/*
    ����������
*/

void TcpClient::Connect (const SocketAddress& address, size_t timeout_in_milliseconds, bool tcp_no_delay)
{
  try
  {
    if (!IsClosed ())
      Close ();

    common::Lock lock (*impl);
    
    Socket (SocketDomain_IpV4, SocketProtocol_Tcp).Swap (impl->socket);

    impl->socket.Connect (address, timeout_in_milliseconds);

    try
    {
      SetTcpNoDelay (tcp_no_delay);
    }
    catch (...)
    {
      Close ();
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

void TcpClient::Connect (const InetAddress& address, unsigned short port, size_t timeout_in_milliseconds, bool tcp_no_delay)
{
  try
  {
    Connect (SocketAddress (address, port), timeout_in_milliseconds, tcp_no_delay);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Connect(const InetAddress&,unsigned short,size_t,bool)");
    throw;
  }
}

void TcpClient::Close ()
{
  try
  {
    impl->Close ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Close");
    throw;
  }
}

/*
    �������� ������
*/

void TcpClient::Send (const void* buffer, size_t size)
{
  try
  {
    if (!buffer && size)
      throw xtl::make_null_argument_exception ("", "buffer");

    {
      common::Lock lock (*impl);

      if (impl->async_sender)
      {
        impl->async_sender->SendRequest (buffer, size);
        return;
      }
    }

    const char* data = (const char*)buffer;
      
    while (size)
    {
      size_t send_size = impl->socket.Send (data, size);

      size -= send_size;
      data += send_size;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Send(const void* size_t)");
    throw;
  }
}

void TcpClient::Send (const char* string)
{
  try
  {
    if (!string)
      throw xtl::make_null_argument_exception ("", "string");

    Send (string, strlen (string));
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Send(const char*)");
    throw;
  }
}

/*
    ���� ������
*/

size_t TcpClient::Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  try
  {
    {
      common::Lock lock (*impl);
    
      if (impl->async_receiver.get ())
        return 0;
    }

    return impl->Receive (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::Receive(void*,size_t,size_t)");
    throw;
  }
}

bool TcpClient::ReceiveExactly (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  try
  {
    {
      common::Lock lock (*impl);
    
      if (impl->async_receiver.get ())
        return false;
    }

    return impl->ReceiveExactly (buffer, size, timeout_in_milliseconds);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveExactly(void*,size_t,size_t)");
    throw;
  }
}

/*
    ���������� ���� ��������� ��� ������ ��� ����������
*/

size_t TcpClient::ReceiveAvailable () const
{
  try
  {
    common::Lock lock (*impl);

    return impl->recv_finish - impl->recv_start + impl->socket.ReceiveAvailable ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveAvailable");
    throw;
  }
}

/*
    ��������� ������
*/

void TcpClient::SetReceiveBufferSize (size_t size)
{
  try
  {
    impl->socket.SetReceiveBufferSize (size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetReceiveBufferSize");
    throw;
  }
}

void TcpClient::SetSendBufferSize (size_t size)
{
  try
  {
    impl->socket.SetSendBufferSize (size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetSendBufferSize");
    throw;
  }
}

size_t TcpClient::ReceiveBufferSize () const
{
  try
  {
    return impl->socket.ReceiveBufferSize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::ReceiveBufferSize");
    throw;
  }
}

size_t TcpClient::SendBufferSize () const
{
  try
  {
    return impl->socket.SendBufferSize ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SendBufferSize");
    throw;
  }
}

/*
    ����� ������
*/

bool TcpClient::IsClosed () const
{
  try
  {
    return impl->socket.IsClosed ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsClosed");
    throw;
  }
}

bool TcpClient::IsConnected () const
{
  try
  {
    return impl->socket.IsConnected ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsConnected");
    throw;
  }
}

bool TcpClient::IsTcpNoDelay () const
{
  try
  {
    return impl->socket.IsTcpNoDelay ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::IsTcpNoDelay");
    throw;
  }
}

void TcpClient::SetTcpNoDelay (bool state)
{
  try
  {
    impl->socket.SetTcpNoDelay (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("network::TcpClient::SetTcpNoDelay");
    throw;
  }
}

/*
    ����������� �����
*/

bool TcpClient::IsAsyncSendingEnabled () const
{
  common::Lock lock (*impl);

  return impl->async_sender;
}

bool TcpClient::IsAsyncReceivingEnabled () const
{
  common::Lock lock (*impl);

  return impl->async_receiver;
}

void TcpClient::SwitchToAsyncReceiving () const
{
  common::Lock lock (*impl);

  if (impl->async_receiver)
    return;

  size_t available = impl->recv_finish - impl->recv_start;

  if (available)
  {
    impl->async_receive_data_signal (impl->recv_start, available);

    impl->recv_start = impl->recv_finish = impl->recv_buffer.data ();
  }

  impl->async_receiver.reset (new AsyncReceiver (impl->socket, xtl::bind (&Impl::OnAsyncDataReceived, &*impl, _1, _2)));
}

void TcpClient::SwitchToAsyncSending () const
{
  common::Lock lock (*impl);

  if (impl->async_sender)
    return;

  impl->async_sender.reset (new AsyncSender (impl->socket));
}

xtl::connection TcpClient::RegisterAsyncReceivingEventHandler (const AsyncReceivingHandler& handler)
{
  common::Lock lock (*impl);

  return impl->async_receive_data_signal.connect (handler);
}

/*
    �����
*/

void TcpClient::Swap (TcpClient& client)
{
  stl::swap (impl, client.impl);
}

namespace network
{

void swap (TcpClient& client1, TcpClient& client2)
{
  client1.Swap (client2);
}

}
