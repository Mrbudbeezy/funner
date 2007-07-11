/*
    ���� ��� ��� ������������ �������������. �� ����������� ��� �������
*/

#ifndef DESIGN_FUNCTION_TRAITS_HEADER
#define DESIGN_FUNCTION_TRAITS_HEADER

#include <mpl/type_list.h>

namespace mpl
{

/*
    ������� ������������ ���������� ������� � ���� ����������� ��������
*/

//��� ������������ � �������� ���������� ���������� ���������, ���������� void
struct void_argument {};

//������ ����������� �� ������� ���������� � ������ ���������� �������
template <class Traits>
struct eclipsis_function_traits: public Traits
{
  enum { has_eclipsis = true };
};

//������� ������������ ��� �������� �� �������
template <class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
class signature_arguments_traits
{
  private:
    typedef typename type_node<Arg1,type_node<Arg2,type_node<Arg3,type_node<Arg4,type_node<Arg5,type_node<Arg6,type_node<Arg7,type_node<Arg8,type_node<Arg9,null_type> > > > > > > > > arguments_list;

  public:
    enum {
      is_ptrfun    = false,
      is_memfun    = false,
      has_eclipsis = false
    };

    typedef Ret result_type;

    template <size_t number> struct argument {
      typedef typename mpl::at<arguments_list,number-1>::type type;
    };
};

//������� ������������ ��� ���������� �� �������
template <class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
struct ptrfun_arguments_traits: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  enum { is_ptrfun = true };
};

//������� ������������ ��� ���������� �� �������-�����
template <class T,class Ret,class Arg1=void_argument,class Arg2=void_argument,class Arg3=void_argument,class Arg4=void_argument,class Arg5=void_argument,class Arg6=void_argument,class Arg7=void_argument,class Arg8=void_argument,class Arg9=void_argument>
struct memfun_arguments_traits: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  typedef T object_type;

  enum { is_memfun = true };
};

//������� ������������ ��������������� �������
template <class Fn>
struct function_traits
{
  enum {
    is_memfun    = false,
    is_ptrfun    = false,
    has_eclipsis = Fn::has_eclipsis
  };

  typedef typename Fn::result_type result_type;

  template <size_t number> struct argument {
    typedef typename Fn::template argument<number>::type type;
  };
};

/*
    ������������� ��� �������� �������
*/

//������������� ��� ��������� ��� ����������
template <class Ret>
struct function_traits<Ret ()>: public signature_arguments_traits<Ret> { };

//������������� ��� ��������� ��� ���������� � �����������
template <class Ret>
struct function_traits<Ret (...)>: public eclipsis_function_traits<signature_arguments_traits<Ret> > { };

//������������� ��� ��������� � 1-� ����������
template <class Arg1,class Ret>
struct function_traits<Ret (Arg1)>: public signature_arguments_traits<Ret,Arg1> { };

//������������� ��� ��������� � 1-� ���������� � �����������
template <class Arg1,class Ret>
struct function_traits<Ret (Arg1,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1> > { };

//������������� ��� ��������� � 2-�� �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (Arg1,Arg2)>: public signature_arguments_traits<Ret,Arg1,Arg2> { };

//������������� ��� ��������� � 2-�� ����������� � �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (Arg1,Arg2,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2> > { };

//������������� ��� ��������� � 3-�� �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� � 3-�� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� � 4-�� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� � 4-�� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� � 5-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� � 5-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� � 6-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� � 6-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� � 7-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� � 7-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� � 8-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� � 8-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� � 9-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� � 9-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<signature_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    ������������� ��� ���������� �� �������
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct function_traits<Ret (*)()>: public ptrfun_arguments_traits<Ret>  { };

//������������� ��� ��������� �� ������� ��� ���������� � �����������
template <class Ret>
struct function_traits<Ret (*)(...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret> > { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1,class Ret>
struct function_traits<Ret (*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//������������� ��� ��������� �� ������� � 1-� ���������� � �����������
template <class Arg1,class Ret>
struct function_traits<Ret (*)(Arg1,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1> > { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//������������� ��� ��������� �� ������� � 2-�� ����������� � �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2> > { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//������������� ��� ��������� �� ������� � 3-�� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//������������� ��� ��������� �� ������� � 4-�� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//������������� ��� ��������� �� ������� � 5-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//������������� ��� ��������� �� ������� � 6-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//������������� ��� ��������� �� ������� � 7-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//������������� ��� ��������� �� ������� � 8-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

//������������� ��� ��������� �� ������� � 9-� ����������� � �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    ������������� ��� ���������� �� �������-���� ������
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ T ��� ���������� � �����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret> > { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ���������� � �����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ���������� � �����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ���������� � �����������
template <class T,class Ret>
struct function_traits<Ret (T::*const volatile*)(...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret> > { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ���������� � �����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ���������� � �����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ���������� � �����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ���������� � �����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...)>: public eclipsis_function_traits<memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) const>: public eclipsis_function_traits<memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) volatile>: public eclipsis_function_traits<memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� ����������� � �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9,...) const volatile>: public eclipsis_function_traits<memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> > { };

/*
    ������������� ��� ����������� MSVC
*/

#ifdef _MSC_VER

/*
    ������������� ��� ���������� �� ������� � ������������� __fastcall
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct function_traits<Ret (__fastcall*)()>: public ptrfun_arguments_traits<Ret>  { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

/*
    ������������� ��� ���������� �� �������-���� ������ � ������������� __fastcall
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__fastcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

/*
    ������������� ��� ���������� �� ������� � ������������� __stdcall
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct function_traits<Ret (__stdcall*)()>: public ptrfun_arguments_traits<Ret>  { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1)>: public ptrfun_arguments_traits<Ret,Arg1>  { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2>  { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3>  { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4>  { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5>  { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6>  { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7>  { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8>  { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public ptrfun_arguments_traits<Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>  { };

/*
    ������������� ��� ���������� �� �������-���� ������ � ������������� __stdcall
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)()>: public memfun_arguments_traits<T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() const>: public memfun_arguments_traits<const T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() volatile>: public memfun_arguments_traits<volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)() const volatile>: public memfun_arguments_traits<const volatile T,Ret> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1)>: public memfun_arguments_traits<T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) const>: public memfun_arguments_traits<const T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1,class Arg2,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9)>: public memfun_arguments_traits<T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const>: public memfun_arguments_traits<const T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) volatile>: public memfun_arguments_traits<volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1,class Arg2,class Arg3,class Arg4,class Arg5,class Arg6,class Arg7,class Arg8,class Arg9,class Ret>
struct function_traits<Ret (__stdcall T::*const volatile*)(Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9) const volatile>: public memfun_arguments_traits<const volatile T,Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9> { };

#endif //_MSC_VER

}

namespace design
{
  namespace type_manip = mpl;
}

#endif //DESIGN_FUNCTION_TRAITS_HEADER
