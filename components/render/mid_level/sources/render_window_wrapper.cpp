#include "shared.h"

using namespace render::mid_level;

Window::Window (WindowImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Window::Window (const Window& window)
  : impl (window.impl)
{
  addref (impl);
}

Window::~Window ()
{
  release (impl);
}

Window& Window::operator = (const Window& window)
{
  Window (window).Swap (*this);
  return *this;
}

void Window::SetName (const char* name)
{
  impl->SetName (name);
}

const char* Window::Name () const
{
  return impl->Name ();
}

RenderTarget Window::ColorBuffer () const
{
  return Wrappers::Wrap<RenderTarget> (impl->ColorBuffer ());
}

RenderTarget Window::DepthStencilBuffer () const
{
  return Wrappers::Wrap<RenderTarget> (impl->DepthStencilBuffer ());
}

size_t Window::Width () const
{
  return impl->Width ();
}

size_t Window::Height () const
{
  return impl->Height ();
}

xtl::connection Window::RegisterEventHandler (WindowEvent event, const EventHandler& handler) const
{
  return impl->RegisterEventHandler (event, handler);
}

void Window::Swap (Window& window)
{
  stl::swap (impl, window.impl);
}

namespace render
{

namespace mid_level
{

void swap (Window& window1, Window& window2)
{
  window1.Swap (window2);
}

}

}
