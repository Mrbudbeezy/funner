#ifndef XTL_MESSAGE_EXCEPTION_HEADER
#define XTL_MESSAGE_EXCEPTION_HEADER

#include <xtl/exception.h>
#include <xtl/string_buffer.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����������, ���������� �� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class message_exception_base: virtual public exception
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    message_exception_base  (const char* message);
    message_exception_base  (const char* format, va_list args);    
    message_exception_base  (const char* source, const char* message);
    message_exception_base  (const char* source, const char* format, va_list args);
    ~message_exception_base () throw () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* what () const throw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void vtouch (const char* format, va_list args) throw ();
    
  private:
    string_buffer message;    
};

#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4250) //inherits via dominance
#endif

template <class BaseException>
class message_exception: public BaseException, public message_exception_base
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    message_exception (const char* message);
    message_exception (const char* format, va_list args);
    message_exception (const char* source, const char* message);
    message_exception (const char* source, const char* format, va_list args);
};

#ifdef _MSC_VER
  #pragma warning (pop)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� � ��������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Exception> message_exception<Exception> format_exception  (const char* source, const char* format, ...);
template <class Exception> message_exception<Exception> vformat_exception (const char* source, const char* format, va_list args);

#include <xtl/detail/message_exception.inl>

}

#endif
