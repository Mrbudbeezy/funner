#include <common/exception.h>
#include <common/strlib.h>

using namespace common;
using namespace stl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ������ Exception
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Exception::Impl
{
  string message;
  bool   stack_print;
  
  Impl () : stack_print (false) {}
};

/*
    Exception
*/

Exception::Exception (const char* _message)
{
  impl = new Impl;
  
  try
  {      
    impl->message = _message;    
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Exception::Exception (const char* source, const char* _message)
{
  impl = new Impl;
  
  try
  {      
    impl->message = _message;
    
    Touch (source);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Exception::Exception (const Exception& exception)
{
  impl = new Impl;
  
  try
  {
    *impl = *exception.impl;
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Exception::~Exception () throw ()
{
  delete impl;
}

Exception& Exception::operator = (const Exception& exception)
{
  if (this == &exception)
    return *this;

  *impl = *exception.impl;

  return *this;
}

const char* Exception::Message () const throw ()
{
  return impl->message.c_str ();
}

const char* Exception::what () const throw ()
{
  return impl->message.c_str ();
}

void Exception::TouchImpl (const char* source)
{
  if (!source || !*source)
    return;

/*  if (!impl->stack_print)
  {
    impl->message     += " at ";
    impl->stack_print  = true;
  }
  else
  {
//    impl->message += "<-";
    impl->message += '\n';
  }*/
  
  impl->message += "\n    at ";
  impl->message += source;
}

void Exception::VTouch (const char* format, va_list args)
{
  if (!format)
    return;

  TouchImpl (vformat (format, args).c_str ());
}
    
void Exception::Touch (const char* format, ...)
{
  va_list list;
  
  va_start (list, format);  
  VTouch   (format, list);
}    

/*
    VRaise
*/

void common::VRaise (const char* source,const char* format,va_list list,void (*raise)(const char* source,const char* message))
{
  raise (source,common::vformat (format,list).c_str ());
}
