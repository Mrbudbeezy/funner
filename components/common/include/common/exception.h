#ifndef COMMONLIB_EXCEPTION_HEADER
#define COMMONLIB_EXCEPTION_HEADER

#include <exception>
#include <stddef.h>
#include <stdarg.h>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ArgumentExceptionTag;             //�������� �������� ��������� ����������� �������
struct ArgumentNullExceptionTag;         //������� �������� ���������
struct ArgumentOutOfRangeExceptionTag;   //�������� ��������� ������� �� ������� ���������� ��������
struct OperationExceptionTag;            //�������� ��������
struct NotImplementedExceptionTag;       //������� �� �����������
struct NotSupportedExceptionTag;         //������� �� ��������������
struct PlatformNotSupportedExceptionTag; //��������� �� ��������������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Exception: public std::exception
{
  public:
    Exception  (const char* message);
    Exception  (const char* source, const char* message);
    Exception  (const Exception&);
    ~Exception () throw ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Exception& operator = (const Exception&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Message () const throw ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� � ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Touch  (const char* format, ...);
    void VTouch (const char* format, va_list args);

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� std::exception
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* what () const throw ();
    
  private:
    void TouchImpl (const char* source);

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class BaseException,class Tag>
class DerivedException: public BaseException
{
  public:
    DerivedException (const char* message);
    DerivedException (const char* source, const char* message);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef DerivedException<Exception,ArgumentExceptionTag>                   ArgumentException;
typedef DerivedException<ArgumentException,ArgumentNullExceptionTag>       ArgumentNullException;
typedef DerivedException<ArgumentException,ArgumentOutOfRangeExceptionTag> ArgumentOutOfRangeException;
typedef DerivedException<Exception,OperationExceptionTag>                  OperationException;
typedef DerivedException<Exception,NotImplementedExceptionTag>             NotImplementedException;
typedef DerivedException<Exception,NotSupportedExceptionTag>               NotSupportedException;
typedef DerivedException<Exception,PlatformNotSupportedExceptionTag>       PlatformNotSupportedException;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� � ��������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Exception>
void raise (const char* source,const char* format,...);

template <class Exception>
void vraise (const char* source,const char* format,va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ������� ��� ������� ����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
void raise_invalid_argument   (const char* source,const char* param);
void raise_invalid_argument   (const char* source,const char* param,const char* value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,int value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,size_t value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,float value,const char* comment=NULL);
void raise_invalid_argument   (const char* source,const char* param,double value,const char* comment=NULL);
void raise_out_of_range        (const char* source,const char* param);
void raise_out_of_range        (const char* source,const char* param,int value,int first,int last);
void raise_out_of_range        (const char* source,const char* param,size_t value,size_t first,size_t last);
void raise_out_of_range        (const char* source,const char* param,float value,float first,float last);
void raise_out_of_range        (const char* source,const char* param,double value,double first,double last);
void raise_out_of_range        (const char* source,const char* param,size_t index,size_t max_count);
void raise_null_argument      (const char* source,const char* param);
void raise_not_implemented    (const char* source);
void raise_invalid_operation  (const char* source, const char* format, ...);
void vraise_invalid_operation (const char* source, const char* format, va_list list);
void raise_not_supported      (const char* source);
void raise_not_supported      (const char* source,const char* format,...);
void vraise_not_supported     (const char* source,const char* format,va_list list);

#include <common/detail/exception.inl>

}

#endif
