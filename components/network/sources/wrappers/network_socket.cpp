#include "shared.h"

using namespace network;

typedef xtl::com_ptr<SocketImpl> SocketImplPtr;

struct Socket::Impl: public xtl::reference_counter
{
  SocketImplPtr socket;

  Impl () {}
  Impl (SocketImplPtr in_socket) : socket (in_socket) {}

  SocketImpl* CheckedHandle () const
  {
    if (!socket)
      throw xtl::format_operation_exception ("network::Socket::Impl::CheckedHandle", "Socket not opened");

    return socket.get ();
  }
};

/*
    ������������ / ���������� / ������������
*/

Socket::Socket ()
  : impl (new Impl)
{
}

Socket::Socket (SocketDomain socket_domain, SocketProtocol protocol)
{
  SocketImplPtr socket = SocketImplPtr (Platform::CreateSocket (socket_domain, protocol), false);

  impl = new Impl (socket);
}

Socket::Socket (SocketImpl* socket_impl)
  : impl (new Impl (socket_impl))
  {}

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
  Socket ().Swap (*this);
}

/*
    ������ ������
*/

const SocketAddress& Socket::LocalAddress () const
{
  return impl->CheckedHandle ()->LocalAddress ();
}

const SocketAddress& Socket::RemoteAddress () const
{
  return impl->CheckedHandle ()->RemoteAddress ();
}

/*
    ��������
*/

SocketProtocol Socket::Protocol () const
{
  return impl->CheckedHandle ()->Protocol ();
}

/*
    �������� ������� �������� ������
*/

bool Socket::IsReceiveClosed () const
{
  return impl->CheckedHandle ()->IsReceiveClosed ();
}

bool Socket::IsSendClosed () const
{
  return impl->CheckedHandle ()->IsSendClosed ();
}

void Socket::CloseReceive ()
{
  impl->CheckedHandle ()->CloseReceive ();
}

void Socket::CloseSend ()
{
  impl->CheckedHandle ()->CloseSend ();
}

/*
    ����������
*/

void Socket::Bind (const SocketAddress& address)
{
  impl->CheckedHandle ()->Bind (address);
}

void Socket::Connect (const SocketAddress& address, size_t timeout)
{
  impl->CheckedHandle ()->Connect (address, timeout);
}

/*
    ���� ����������
*/

void Socket::Listen ()
{
  impl->CheckedHandle ()->Listen ();
}

Socket Socket::Accept ()
{
  return Socket (impl->CheckedHandle ()->Accept ());
}

bool Socket::TryAccept (Socket& target_socket)
{
  SocketImpl* socket_impl = impl->CheckedHandle ()->TryAccept ();

  if (!impl)
    return false;

  Socket accepted_socket (socket_impl);

  target_socket.Swap (accepted_socket);

  return true;
}

/*
    ����� ������
*/

bool Socket::IsClosed () const
{
  return !impl->socket;
}

bool Socket::IsConnected () const
{
  return impl->CheckedHandle ()->IsConnected ();
}

bool Socket::IsBound () const
{
  return impl->CheckedHandle ()->IsBound ();
}

bool Socket::IsKeepAlive () const
{
  return impl->CheckedHandle ()->IsKeepAlive ();
}

bool Socket::IsOobInline () const
{
  return impl->CheckedHandle ()->IsOobInline ();
}

bool Socket::IsReuseAddress () const
{
  return impl->CheckedHandle ()->IsReuseAddress ();
}

bool Socket::IsTcpNoDelay () const
{
  return impl->CheckedHandle ()->IsTcpNoDelay ();
}

void Socket::SetKeepAlive (bool state)
{
  impl->CheckedHandle ()->SetKeepAlive (state);
}

void Socket::SetOobInline (bool state)
{
  impl->CheckedHandle ()->SetOobInline (state);
}

void Socket::SetReuseAddress (bool state)
{
  impl->CheckedHandle ()->SetReuseAddress (state);
}

void Socket::SetTcpNoDelay (bool state)
{
  impl->CheckedHandle ()->SetTcpNoDelay (state);
}

/*
    ��������� ������
*/

void Socket::SetReceiveBufferSize (size_t size)
{
  impl->CheckedHandle ()->SetReceiveBufferSize (size);
}

void Socket::SetSendBufferSize (size_t size)
{
  impl->CheckedHandle ()->SetSendBufferSize (size);
}

size_t Socket::ReceiveBufferSize () const
{
  return impl->CheckedHandle ()->ReceiveBufferSize ();
}

size_t Socket::SendBufferSize () const
{
  return impl->CheckedHandle ()->SendBufferSize ();
}

/*
    ������ / ������ ������
*/

size_t Socket::Receive (void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  return impl->CheckedHandle ()->Receive (buffer, size, timeout_in_milliseconds);
}

size_t Socket::Send (const void* buffer, size_t size, size_t timeout_in_milliseconds)
{
  return impl->CheckedHandle ()->Send (buffer, size, timeout_in_milliseconds);
}

/*
    ���������� ���� ��������� ��� ������ ��� ����������
*/

size_t Socket::ReceiveAvailable () const
{
  return impl->CheckedHandle ()->ReceiveAvailable ();
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
