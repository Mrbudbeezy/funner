#include "shared.h"

using namespace network;

struct Socket::Impl: public xtl::reference_counter
{
};

/*
    ������������ / ���������� / ������������
*/

Socket::Socket ()
{
  throw xtl::make_not_implemented_exception ("network::Socket::Socket()");
}

Socket::Socket (const InetAddress& inet_address, unsigned short port, SocketProtocol protocol)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Socket(const SocketAddress&, unsigned short, SocketProtocol)");
}

Socket::Socket (const SocketAddress& socket_address, SocketProtocol protocol)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Socket(const SocketAddress&, SocketProtocol)");
}

Socket::Socket (const Socket& socket)
  : impl (socket.impl)
{
  addref (impl);
}

Socket::~Socket ()
{
  release (impl);
}

Socket& Socket::operator = (const Socket& socket)
{
  Socket (socket).Swap (*this);
  
  return *this;
}

/*
    �������� ������
*/

void Socket::Close ()
{
  throw xtl::make_not_implemented_exception ("network::Socket::Close");
}

/*
    ������ ������
*/

const SocketAddress& Socket::LocalAddress () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::LocalAddress");
}

const SocketAddress& Socket::RemoteAddress () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::RemoteAddress");
}

/*
    ��������
*/

SocketProtocol Socket::Protocol () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::Protocol");
}

/*
    �������� ������� �������� ������
*/

bool Socket::IsReceiveClosed () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsReceiveClosed");
}

bool Socket::IsSendClosed () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsSendClosed");
}

void Socket::CloseReceive ()
{
  throw xtl::make_not_implemented_exception ("network::Socket::CloseReceive");
}

void Socket::CloseSend ()
{
  throw xtl::make_not_implemented_exception ("network::Socket::CloseSend");
}

/*
    ����������
*/

void Socket::Bind (const SocketAddress& address)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Bind");
}

void Socket::Connect (const SocketAddress& address, size_t timeout)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Connect");
}

/*
    ���� ����������
*/

Socket Socket::Accept ()
{
  throw xtl::make_not_implemented_exception ("network::Socket::Accept");
}

/*
    ����� ������
*/

bool Socket::IsClosed () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsClosed");
}

bool Socket::IsConnected () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsConnected");
}

bool Socket::IsBound () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsBound");
}

bool Socket::IsKeepAlive () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsKeepAlive");
}

bool Socket::IsOobInline () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsOobInline");
}

bool Socket::IsReuseAddress () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsReuseAddress");
}

bool Socket::IsTcpNoDelay () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::IsTcpNoDelay");
}

void Socket::SetKeepAlive (bool state)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetKeepAlive");
}

void Socket::SetOobInline (bool state)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetOobInline");
}

void Socket::SetReuseAddress (bool state)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetReuseAddress");
}

void Socket::SetTcpNoDelay (bool state)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetTcpNoDelay");
}

/*
    ��������� ������
*/

void Socket::SetReceiveBufferSize (size_t size)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetReceiveBufferSize");
}

void Socket::SetSendBufferSize (size_t size)
{
  throw xtl::make_not_implemented_exception ("network::Socket::SetSendBufferSize");
}

size_t Socket::ReceiveBufferSize () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::ReceiveBufferSize");
}

size_t Socket::SendBufferSize () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::SendBufferSize");
}

/*
    ������ / ������ ������
*/

size_t Socket::Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Receive");
}

size_t Socket::Send (const void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  throw xtl::make_not_implemented_exception ("network::Socket::Send");
}

/*
    ���������� ���� ��������� ��� ������ ��� ����������
*/

size_t Socket::ReceiveAvailable () const
{
  throw xtl::make_not_implemented_exception ("network::Socket::ReceiveAvailable");
}

/*
    �����
*/

void Socket::Swap (Socket& socket)
{
  stl::swap (impl, socket.impl);
}

namespace network
{

void swap (Socket& socket1, Socket& socket2)
{
  socket1.Swap (socket2);
}

}
