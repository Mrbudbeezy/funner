#ifndef XTL_TYPE_TRAITS_CV_HEADER
#define XTL_TYPE_TRAITS_CV_HEADER

/*
    CV-�஢�ન � �८�ࠧ������
*/

//�஢�ઠ ����� �� T �����䨪��� const/volatile
template <class T>           struct is_const:                       public false_type {};
template <class T>           struct is_const<const T>:              public true_type {};
template <class T>           struct is_const<const volatile T>:     public true_type {};
template <class T, size_t N> struct is_const<const T [N]>:          public true_type {};
template <class T, size_t N> struct is_const<const volatile T [N]>: public true_type {};
template <class T>           struct is_const<const T []>:           public true_type {};
template <class T>           struct is_const<const volatile T []>:  public true_type {};

template <class T>           struct is_volatile:                       public false_type {};
template <class T>           struct is_volatile<volatile T>:           public true_type {};
template <class T>           struct is_volatile<const volatile T>:     public true_type {};
template <class T, size_t N> struct is_volatile<volatile T [N]>:       public true_type {};
template <class T, size_t N> struct is_volatile<const volatile T [N]>: public true_type {};
template <class T>           struct is_volatile<volatile T []>:        public true_type {};
template <class T>           struct is_volatile<const volatile T []>:  public true_type {};

//㤠����� �����䨪��� const
template <class T>           struct remove_const                       { typedef T          type; };
template <class T>           struct remove_const<T&>                   { typedef T&         type; };
template <class T>           struct remove_const<const T>              { typedef T          type; };
template <class T, size_t N> struct remove_const<T const [N]>          { typedef T          type [N]; };
template <class T, size_t N> struct remove_const<T const volatile [N]> { typedef T volatile type [N]; };
template <class T>           struct remove_const<T const []>           { typedef T          type []; };
template <class T>           struct remove_const<T const volatile []>  { typedef T volatile type []; };

//㤠����� �����䨪��� volatile
template <class T>           struct remove_volatile                       { typedef T       type; };
template <class T>           struct remove_volatile<T&>                   { typedef T&      type; };
template <class T>           struct remove_volatile<volatile T>           { typedef T       type; };
template <class T, size_t N> struct remove_volatile<T volatile [N]>       { typedef T       type [N]; };
template <class T, size_t N> struct remove_volatile<T const volatile [N]> { typedef T const type [N]; };
template <class T>           struct remove_volatile<T volatile []>        { typedef T       type []; };
template <class T>           struct remove_volatile<T const volatile []>  { typedef T const type []; };

//㤠����� �����䨪��஢ const+volatile
template <class T> struct remove_cv: public remove_const<typename remove_volatile<T>::type> {};

//���������� �����䨪��� const
template <class T> struct add_const     { typedef T const type; };
template <class T> struct add_const<T&> { typedef T& type; };

//���������� �����䨪��� volatile
template <class T> struct add_volatile     { typedef T volatile type; };
template <class T> struct add_volatile<T&> { typedef T& type; };

//���������� �����䨪��� const volatile
template <class T> struct add_cv     { typedef T const volatile type; };
template <class T> struct add_cv<T&> { typedef T& type; };

#endif
