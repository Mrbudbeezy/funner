/*
    �������� ��������� ����� ������
*/

namespace detail
{

//��������������� ����� ����������� ���������� ������������ �� From � To
template <class From, class To> struct is_convertible_basic_impl
{
  typedef char no_type;
  struct  yes_type { char dummy [2]; };
  
  static no_type  test (...);
  static yes_type test (To);

  static From from;
  
#ifdef _MSC_VER
  #pragma warning (push)
  #pragma warning (disable : 4244)
#endif  

  enum { value = sizeof (yes_type) == sizeof (test (from)) };
  
#ifdef _MSC_VER
  #pragma warning (pop)
#endif  
};

//� ������� �� ����������� ������ ��������� ������� ������: ���������� � void, � � �������
template <class From, class To> struct is_convertible_impl
{
  typedef typename add_reference<From>::type reference_type;
  
  enum { value = (is_convertible_basic_impl<reference_type, To>::value || is_void<To>::value) && !is_array<To>::value };
};

//����������� ���������� � ������ �������� �������������� � ����������� ����� ������
template <bool trivial_from, bool trivial_to, bool abstract_to> struct is_convertible_impl_select
{
  template <class From, class To> struct rebind
  {
    typedef bool_constant<is_convertible_impl<From, To>::value> type;
  };
};

template <> struct is_convertible_impl_select<true, true, false>
{
  template <class From, class To> struct rebind
  {
    typedef true_type type;
  };
};

template <> struct is_convertible_impl_select<false, false, true>
{
   template <class From, class To> struct rebind
   {
     typedef false_type type;
   };
};

template <> struct is_convertible_impl_select<true, false, true>
{
   template <class From, class To> struct rebind
   {
     typedef false_type type;
   };
};

template <class From, class To> struct is_convertible_helper
{
  typedef is_convertible_impl_select<is_arithmetic<From>::value, is_arithmetic<To>::value, is_abstract<To>::value> selector;  
  typedef typename selector::template rebind<From, To>::type type;
};

}

//�������� �������� �� T � U ����� � ��� �� �����
template <class T, class U> struct is_same:       public false_type {};
template <class T>          struct is_same<T, T>: public true_type {};

//�������� �������� �� ������� �������������� �� From � To
template <class From, class To> struct is_convertible: public detail::is_convertible_helper<From, To>::type {};

//������������� ��� ���� void
template <class From> struct is_convertible<From, void>:                               public true_type {};
template <class From> struct is_convertible<From, const void>:                         public true_type {};
template <class From> struct is_convertible<From, volatile void>:                      public true_type {};
template <class From> struct is_convertible<From, const volatile void>:                public true_type {};
template <class To>   struct is_convertible<void, To>:                                 public false_type {};
template <class To>   struct is_convertible<const void, To>:                           public false_type {};
template <class To>   struct is_convertible<volatile void, To>:                        public false_type {};
template <class To>   struct is_convertible<const volatile void, To>:                  public false_type {};
template <>           struct is_convertible<void, void>:                               public true_type {};
template <>           struct is_convertible<void, const void>:                         public true_type {};
template <>           struct is_convertible<void, volatile void>:                      public true_type {};
template <>           struct is_convertible<void, const volatile void>:                public true_type {};
template <>           struct is_convertible<const void, void>:                         public true_type {};
template <>           struct is_convertible<const void, const void>:                   public true_type {};
template <>           struct is_convertible<const void, volatile void>:                public true_type {};
template <>           struct is_convertible<const void, const volatile void>:          public true_type {};
template <>           struct is_convertible<volatile void, void>:                      public true_type {};
template <>           struct is_convertible<volatile void, const void>:                public true_type {};
template <>           struct is_convertible<volatile void, volatile void>:             public true_type {};
template <>           struct is_convertible<volatile void, const volatile void>:       public true_type {};
template <>           struct is_convertible<const volatile void, void>:                public true_type {};
template <>           struct is_convertible<const volatile void, const void>:          public true_type {};
template <>           struct is_convertible<const volatile void, volatile void>:       public true_type {};
template <>           struct is_convertible<const volatile void, const volatile void>: public true_type {};

//�������� �������� �� Base ������� ��� Derived
//template <class Base, class Derived> struct is_base_of: public bool_constant<> {};
