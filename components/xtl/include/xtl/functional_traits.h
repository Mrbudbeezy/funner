/*
    ���� ��� ��� ������������ �������������. �� ����������� ��� �������
*/

#ifndef XTL_FUNCTIONAL_TRAITS_HEADER
#define XTL_FUNCTIONAL_TRAITS_HEADER

#include <cstddef>

namespace xtl
{

//forward declarations
namespace mpl
{

template <class Head, class Tail>       struct type_node;
template <class TypeList, size_t Index> struct at;
                                        struct null_type;

}

namespace detail
{

/*
    ������� ������������ ��������������� �������
*/

template <class Fn> struct functional_traits
{
  enum {
    is_function  = false,
    is_memfun    = false,
    is_ptrfun    = false,
    has_eclipsis = false
  };
};

/*
    ������������� ��� ��������� ����� ��������������
*/

template <class Fn> struct functional_traits<const Fn>:          public functional_traits<Fn> {};
template <class Fn> struct functional_traits<volatile Fn>:       public functional_traits<Fn> {};
template <class Fn> struct functional_traits<const volatile Fn>: public functional_traits<Fn> {};

/*
    ������� ������������ ���������� ������� � ���� ������������� ��������
*/

//��� ������������ � �������� ���������� ���������� ���������, ���������� void
struct void_argument {};

//������ ����������� �� ������� ���������� � ������ ���������� �������
template <class Traits>
struct eclipsis_functional_traits: public Traits
{
  enum { has_eclipsis = true };
};

//������� ������������ ��� �������� �� �������
template <size_t ArgumentsCount, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
class signature_arguments_traits
{
  private:
    typedef typename mpl::type_node<Arg1, mpl::type_node<Arg2, mpl::type_node<Arg3, mpl::type_node<Arg4, mpl::type_node<Arg5, mpl::type_node<Arg6, mpl::type_node<Arg7, mpl::type_node<Arg8, mpl::type_node<Arg9, mpl::null_type> > > > > > > > > argument_list;

  public:
    enum {
      is_function     = true,
      is_ptrfun       = false,
      is_memfun       = false,
      has_eclipsis    = false,
      arguments_count = ArgumentsCount
    };

    typedef Ret result_type;

    template <size_t number> struct argument {
      typedef typename mpl::at<argument_list, number-1>::type type;
    };
};

//������� ������������ ��� ���������� �� �������
template <size_t ArgumentsCount, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
struct ptrfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret,Arg1,Arg2,Arg3,Arg4,Arg5,Arg6,Arg7,Arg8,Arg9>
{
  enum { is_ptrfun = true, is_function = false };
};

//������� ������������ ��� ���������� �� �������-�����
template <size_t ArgumentsCount, class T, class Ret, class Arg1=void_argument, class Arg2=void_argument, class Arg3=void_argument, class Arg4=void_argument, class Arg5=void_argument, class Arg6=void_argument, class Arg7=void_argument, class Arg8=void_argument, class Arg9=void_argument>
struct memfun_arguments_traits: public signature_arguments_traits<ArgumentsCount, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>
{
  typedef T object_type;

  enum { is_memfun = true, is_function = false };
};

/*
    ������������� ��� �������� �������
*/

//������������� ��� ��������� ��� ����������
template <class Ret>
struct functional_traits<Ret ()>: public signature_arguments_traits<0, Ret> { };

//������������� ��� ��������� ��� ���������� � �����������
template <class Ret>
struct functional_traits<Ret (...)>: public eclipsis_functional_traits<signature_arguments_traits<0, Ret> > { };

//������������� ��� ��������� � 1-� ����������
template <class Arg1, class Ret>
struct functional_traits<Ret (Arg1)>: public signature_arguments_traits<1, Ret, Arg1> { };

//������������� ��� ��������� � 1-� ���������� � �����������
template <class Arg1, class Ret>
struct functional_traits<Ret (Arg1, ...)>: public eclipsis_functional_traits<signature_arguments_traits<1, Ret, Arg1> > { };

//������������� ��� ��������� � 2-�� �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (Arg1, Arg2)>: public signature_arguments_traits<2, Ret, Arg1, Arg2> { };

//������������� ��� ��������� � 2-�� ����������� � �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (Arg1, Arg2, ...)>: public eclipsis_functional_traits<signature_arguments_traits<2, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� � 3-�� �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3)>: public signature_arguments_traits<3, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� � 3-�� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<signature_arguments_traits<3, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� � 4-�� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4)>: public signature_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� � 4-�� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<signature_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� � 5-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5)>: public signature_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� � 5-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<signature_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� � 6-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public signature_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� � 6-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<signature_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� � 7-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public signature_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� � 7-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<signature_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� � 8-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public signature_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� � 8-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<signature_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� � 9-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public signature_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� � 9-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<signature_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

/*
    ������������� ��� ���������� �� �������-���� ������
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ T ��� ���������� � �����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...)>: public eclipsis_functional_traits<memfun_arguments_traits<0, T, Ret> > { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ���������� � �����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) const>: public eclipsis_functional_traits<memfun_arguments_traits<0, const T, Ret> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ���������� � �����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<0, volatile T, Ret> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ���������� � �����������
template <class T,class Ret>
struct functional_traits<Ret (T::*)(...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<0, const volatile T, Ret> > { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ���������� � �����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<1, T, Ret, Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ���������� � �����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<1, const T, Ret, Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ���������� � �����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<1, volatile T, Ret, Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ���������� � �����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (T::*)(Arg1, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<1, const volatile T, Ret, Arg1> > { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<2, T, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) const>: public eclipsis_functional_traits<memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) volatile>: public eclipsis_functional_traits<memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� ����������� � �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...) const volatile>: public eclipsis_functional_traits<memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

#if !defined(_MSC_VER) || defined(_WIN32_WCE)

/*
    ������������� ��� ���������� �� �������
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct functional_traits<Ret (*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//������������� ��� ��������� �� ������� ��� ���������� � �����������
template <class Ret>
struct functional_traits<Ret (*)(...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<0, Ret> > { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1, class Ret>
struct functional_traits<Ret (*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//������������� ��� ��������� �� ������� � 1-� ���������� � �����������
template <class Arg1, class Ret>
struct functional_traits<Ret (*)(Arg1, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<1, Ret, Arg1> > { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//������������� ��� ��������� �� ������� � 2-�� ����������� � �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<2, Ret, Arg1, Arg2> > { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//������������� ��� ��������� �� ������� � 3-�� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3> > { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//������������� ��� ��������� �� ������� � 4-�� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4> > { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//������������� ��� ��������� �� ������� � 5-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> > { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//������������� ��� ��������� �� ������� � 6-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> > { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//������������� ��� ��������� �� ������� � 7-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> > { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//������������� ��� ��������� �� ������� � 8-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> > { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

//������������� ��� ��������� �� ������� � 9-� ����������� � �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9, ...)>: public eclipsis_functional_traits<ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> > { };

#else //_MSC_VER

/*
    ������������� ��� ����������� MSVC
*/

/*
    ������������� ��� ���������� �� ������� � ������������� __fastcall
*/

#ifndef __cplusplus_cli

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct functional_traits<Ret (__fastcall*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    ������������� ��� ���������� �� �������-���� ������ � ������������� __fastcall
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__fastcall T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__fastcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

#endif

/*
    ������������� ��� ���������� �� ������� � ������������� __stdcall
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct functional_traits<Ret (__stdcall*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    ������������� ��� ���������� �� �������-���� ������ � ������������� __stdcall
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__stdcall T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__stdcall T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

/*
    ������������� ��� ���������� �� ������� � ������������� __cdecl
*/

//������������� ��� ��������� �� ������� ��� ����������
template <class Ret>
struct functional_traits<Ret (__cdecl*)()>: public ptrfun_arguments_traits<0, Ret>  { };

//������������� ��� ��������� �� ������� � 1-� ����������
template <class Arg1, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1)>: public ptrfun_arguments_traits<1, Ret, Arg1>  { };

//������������� ��� ��������� �� ������� � 2-�� �����������
template <class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2)>: public ptrfun_arguments_traits<2, Ret, Arg1, Arg2>  { };

//������������� ��� ��������� �� ������� � 3-�� �����������
template <class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3)>: public ptrfun_arguments_traits<3, Ret, Arg1, Arg2, Arg3>  { };

//������������� ��� ��������� �� ������� � 4-�� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4)>: public ptrfun_arguments_traits<4, Ret, Arg1, Arg2, Arg3, Arg4>  { };

//������������� ��� ��������� �� ������� � 5-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public ptrfun_arguments_traits<5, Ret, Arg1, Arg2, Arg3, Arg4, Arg5>  { };

//������������� ��� ��������� �� ������� � 6-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public ptrfun_arguments_traits<6, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6>  { };

//������������� ��� ��������� �� ������� � 7-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public ptrfun_arguments_traits<7, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7>  { };

//������������� ��� ��������� �� ������� � 8-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public ptrfun_arguments_traits<8, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8>  { };

//������������� ��� ��������� �� ������� � 9-� �����������
template <class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public ptrfun_arguments_traits<9, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9>  { };

/*
    ������������� ��� ���������� �� �������-���� ������ � ������������� __cdecl
*/

//������������� ��� ��������� �� �������-���� ������ T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)()>: public memfun_arguments_traits<0, T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() const>: public memfun_arguments_traits<0, const T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() volatile>: public memfun_arguments_traits<0, volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T ��� ����������
template <class T,class Ret>
struct functional_traits<Ret (__cdecl T::*)() const volatile>: public memfun_arguments_traits<0, const volatile T, Ret> { };

//������������� ��� ��������� �� �������-���� ������ T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1)>: public memfun_arguments_traits<1, T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) const>: public memfun_arguments_traits<1, const T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) volatile>: public memfun_arguments_traits<1, volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 1-� ����������
template <class T,class Arg1, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1) const volatile>: public memfun_arguments_traits<1, const volatile T, Ret, Arg1> { };

//������������� ��� ��������� �� �������-���� ������ T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2)>: public memfun_arguments_traits<2, T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) const>: public memfun_arguments_traits<2, const T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) volatile>: public memfun_arguments_traits<2, volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 2-�� �����������
template <class T,class Arg1, class Arg2, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2) const volatile>: public memfun_arguments_traits<2, const volatile T, Ret, Arg1, Arg2> { };

//������������� ��� ��������� �� �������-���� ������ T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3)>: public memfun_arguments_traits<3, T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) const>: public memfun_arguments_traits<3, const T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) volatile>: public memfun_arguments_traits<3, volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 3-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3) const volatile>: public memfun_arguments_traits<3, const volatile T, Ret, Arg1, Arg2, Arg3> { };

//������������� ��� ��������� �� �������-���� ������ T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4)>: public memfun_arguments_traits<4, T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) const>: public memfun_arguments_traits<4, const T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) volatile>: public memfun_arguments_traits<4, volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 4-�� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4) const volatile>: public memfun_arguments_traits<4, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4> { };

//������������� ��� ��������� �� �������-���� ������ T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5)>: public memfun_arguments_traits<5, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const>: public memfun_arguments_traits<5, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) volatile>: public memfun_arguments_traits<5, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 5-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5) const volatile>: public memfun_arguments_traits<5, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5> { };

//������������� ��� ��������� �� �������-���� ������ T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6)>: public memfun_arguments_traits<6, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const>: public memfun_arguments_traits<6, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) volatile>: public memfun_arguments_traits<6, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 6-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6) const volatile>: public memfun_arguments_traits<6, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6> { };

//������������� ��� ��������� �� �������-���� ������ T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7)>: public memfun_arguments_traits<7, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const>: public memfun_arguments_traits<7, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) volatile>: public memfun_arguments_traits<7, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 7-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7) const volatile>: public memfun_arguments_traits<7, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7> { };

//������������� ��� ��������� �� �������-���� ������ T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8)>: public memfun_arguments_traits<8, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const>: public memfun_arguments_traits<8, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) volatile>: public memfun_arguments_traits<8, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 8-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8) const volatile>: public memfun_arguments_traits<8, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8> { };

//������������� ��� ��������� �� �������-���� ������ T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9)>: public memfun_arguments_traits<9, T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const>: public memfun_arguments_traits<9, const T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) volatile>: public memfun_arguments_traits<9, volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

//������������� ��� ��������� �� �������-���� ������ const volatile T � 9-� �����������
template <class T,class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9, class Ret>
struct functional_traits<Ret (__cdecl T::*)(Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9) const volatile>: public memfun_arguments_traits<9, const volatile T, Ret, Arg1, Arg2, Arg3, Arg4, Arg5, Arg6, Arg7, Arg8, Arg9> { };

#endif //_MSC_VER

}

using detail::functional_traits;

}

#endif //XTL_FUNCTIONAL_TRAITS_HEADER
