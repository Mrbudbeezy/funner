/*
    �������� ��������� �������-�����
*/

namespace detail
{ 

template <class T>
inline xtl::any make_invoker_argument (T& value)
{ 
  return make_ref_any (value);
}

inline char               make_invoker_argument (char value)               { return value; }
inline signed char        make_invoker_argument (signed char value)        { return value; }
inline unsigned char      make_invoker_argument (unsigned char value)      { return value; }
inline short              make_invoker_argument (short value)              { return value; }
inline unsigned short     make_invoker_argument (unsigned short value)     { return value; }
inline int                make_invoker_argument (int value)                { return value; }
inline unsigned int       make_invoker_argument (unsigned int value)       { return value; }
inline long               make_invoker_argument (long value)               { return value; }
inline unsigned long      make_invoker_argument (unsigned long value)      { return value; }
inline float              make_invoker_argument (float value)              { return value; }
inline const double&      make_invoker_argument (const double& value)      { return value; }
inline const long double& make_invoker_argument (const long double& value) { return value; }
inline const char*        make_invoker_argument (const char* string)       { return string; }
inline const char*        make_invoker_argument (char* string)             { return string; }

/*
    ���������� ���������� �� �����
*/

//����� ������
template <class T> struct argument_selector
{
  static T get (IStack& stack, int index)
  {
    return xtl::any_multicast<T> (stack.GetVariant (index));
  }
};

//���������� �������������� ���������
template <class T> struct int_argument_selector
{
  static T get (IStack& stack, int index) { return static_cast<T> (stack.GetInteger (index)); }
};

//���������� ������������� ���������
template <class T> struct float_argument_selector
{
  static T get (IStack& stack, int index) { return static_cast<T> (stack.GetFloat (index)); }
};

//���������� void-����������
struct raw_pointer_argument_selector
{
  static void* get (IStack& stack, int index) { return stack.GetPointer (index); }
};

//���������� �����
struct string_argument_selector
{
  static const char* get (IStack& stack, int index) { return stack.GetString (index); }
};

//������������� ������ ��������� ��� ��������� ����� ������
template <> struct argument_selector<char>:                 public int_argument_selector<char> {};
template <> struct argument_selector<signed char>:          public int_argument_selector<signed char> {};
template <> struct argument_selector<unsigned char>:        public int_argument_selector<unsigned char> {};
template <> struct argument_selector<short>:                public int_argument_selector<short> {};
template <> struct argument_selector<unsigned short>:       public int_argument_selector<unsigned short> {};
template <> struct argument_selector<int>:                  public int_argument_selector<int> {};
template <> struct argument_selector<unsigned int>:         public int_argument_selector<unsigned int> {};
template <> struct argument_selector<long>:                 public int_argument_selector<long> {};
template <> struct argument_selector<unsigned long>:        public int_argument_selector<unsigned long> {};
template <> struct argument_selector<float>:                public float_argument_selector<float> {};
template <> struct argument_selector<double>:               public float_argument_selector<double> {};
template <> struct argument_selector<long double>:          public float_argument_selector<long double> {};
template <> struct argument_selector<void*>:                public raw_pointer_argument_selector {};
template <> struct argument_selector<const void*>:          public raw_pointer_argument_selector {};
template <> struct argument_selector<const volatile void*>: public raw_pointer_argument_selector {};
template <> struct argument_selector<const char*>:          public string_argument_selector {};
template <> struct argument_selector<const volatile char*>: public string_argument_selector {};

template <class Traits, class Allocator>
struct argument_selector<stl::basic_string<char, Traits, Allocator> >: public string_argument_selector {};

//������� ������ ��������� �� �����
template <class T>
inline T get_argument (IStack& stack, int index)
{
  return argument_selector<T>::get (stack, index);
}

/*
    ��������� ��������� � ����
*/

template <class T> inline void push_argument (IStack& stack, const T& value)
{
  stack.Push (make_invoker_argument (value));
}

/*
    �������� ����������
*/

template <class FunctionalTraits, size_t I, bool is_memfun=FunctionalTraits::is_memfun>
struct functional_argument_traits
{
  typedef typename FunctionalTraits::template argument<I+1>::type argument_type;
  
  enum { argument_index = I };
};

template <class FunctionalTraits, size_t I>
struct functional_argument_traits<FunctionalTraits, I, true>
{
  typedef typename FunctionalTraits::template argument<I>::type argument_type;
  
  enum { argument_index = I };
};

template <class FunctionalTraits>
struct functional_argument_traits<FunctionalTraits, 0, true>
{
  typedef typename FunctionalTraits::object_type& argument_type;
  
  enum { argument_index = 0 };
};

template <class FunctionalTraits>
struct stack_argument_selector
{
  template <size_t I> struct traits: public functional_argument_traits<FunctionalTraits, I> {};  

  template <size_t I> static typename traits<I>::argument_type eval (IStack& stack)
  {
    return argument_selector<typename traits<I>::argument_type>::get (stack, traits<I>::argument_index);
  }
};

/*
    ������� �����
*/

template <class FnTraits, class Fn, class Ret=typename FnTraits::result_type>
struct invoker_impl
{
  enum {
    arguments_count = FnTraits::arguments_count + FnTraits::is_memfun,
    results_count   = 1
  };

  invoker_impl (const Fn& in_fn) : fn (in_fn) {}    

  void operator () (IStack& stack) const
  {
    push_argument (stack, xtl::apply<typename FnTraits::result_type, arguments_count> (fn, stack, stack_argument_selector<FnTraits> ()));
  }

  Fn fn;
};

template <class FnTraits, class Fn>
struct invoker_impl<FnTraits, Fn, void>
{
  enum {
    arguments_count = FnTraits::arguments_count + FnTraits::is_memfun,
    results_count   = 0
  };

  invoker_impl (const Fn& in_fn) : fn (in_fn) {}

  void operator () (IStack& stack) const
  {
    xtl::apply<void, arguments_count> (fn, stack, stack_argument_selector<FnTraits> ());
  }

  Fn fn;
};

}

/*
    �������� ����� ��� ������������� �������� � ��������� ����������
*/

template <class Signature, class Fn>
inline Invoker make_invoker (Fn fn)
{
  typedef detail::invoker_impl<xtl::functional_traits<Signature>, Fn> invoker_type;

  return Invoker (invoker_type (fn), invoker_type::arguments_count, invoker_type::results_count);
}

template <class Fn>
inline Invoker make_invoker (Fn fn)
{
  return make_invoker<Fn, Fn> (fn);
}
