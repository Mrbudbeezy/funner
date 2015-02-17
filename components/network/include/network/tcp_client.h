#ifndef NETWORK_TCP_CLIENT_HEADER
#define NETWORK_TCP_CLIENT_HEADER

#include <network/socket_address.h>

namespace network
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ TCP
///////////////////////////////////////////////////////////////////////////////////////////////////
class TcpClient
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TcpClient  ();
    TcpClient  (const SocketAddress& address, unsigned int timeout_in_milliseconds = 0, bool tcp_no_delay = false);
    TcpClient  (const InetAddress& address, unsigned short port, unsigned int timeout_in_milliseconds = 0, bool tcp_no_delay = false);
    TcpClient  (const TcpClient&);
    ~TcpClient ();

    TcpClient& operator = (const TcpClient&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Connect (const SocketAddress& address, unsigned int timeout_in_milliseconds = 0, bool tcp_no_delay = false);
    void Connect (const InetAddress& address, unsigned short port, unsigned int timeout_in_milliseconds = 0, bool tcp_no_delay = false);
    void Close   ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Send (const void* buffer, unsigned int size);
    void Send (const char* string);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Receive        (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds = 0);
    bool         ReceiveExactly (void* buffer, unsigned int size, unsigned int timeout_in_milliseconds = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� ��������� ��� ������ ��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int ReceiveAvailable () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetReceiveBufferSize (unsigned int size);
    void         SetSendBufferSize    (unsigned int size);
    unsigned int ReceiveBufferSize    () const;
    unsigned int SendBufferSize       () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsClosed      () const;      //������ �� �����
    bool IsConnected   () const;      //����������� �� �����
    bool IsTcpNoDelay  () const;      //���������� �� ���� TCP_NODELAY
    void SetTcpNoDelay (bool state);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsAsyncSendingEnabled   () const;
    bool IsAsyncReceivingEnabled () const;
    void SwitchToAsyncReceiving  () const;
    void SwitchToAsyncSending    () const;

    typedef xtl::function<void (const void* buffer, unsigned int size)> AsyncReceivingHandler;

    xtl::connection RegisterAsyncReceivingEventHandler (const AsyncReceivingHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (TcpClient&);
  
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (TcpClient&, TcpClient&);

}

#endif
