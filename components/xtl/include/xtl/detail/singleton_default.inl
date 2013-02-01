/*
    ��������������� �����, ������������� ��������������� ������� �� main � ������ ������� � ���� singleton::instance
*/

namespace detail
{

template <class T, bool need_destroy> struct singleton_default_object_creator
{
  singleton_default_object_creator () { singleton_default<T, need_destroy>::instance (); }

  void do_nothing () const {}

  static singleton_default_object_creator creator;
};

template <class T, bool need_destroy> singleton_default_object_creator<T, need_destroy> singleton_default_object_creator<T, need_destroy>::creator;

/*
    ��������� ���������� �������
*/

template <class T, bool need_destroy> struct singleton_default_instance
{
  static T& get ()
  {
    static T instance;

    return instance;
  }
};

template <class T> struct singleton_default_instance<T, false>
{
  static T& get ()
  {
    union max_align
    {
      char        field1;
      short       field2;
      int         field3;
      long        field4;
      float       field5;
      double      field6;
      long double field7;
      void*       field8;
      char        value_buffer [sizeof (T)];
    };

#ifdef ARM
#ifdef __GNUC__
    #define XTL_SINGLETON_DEEFAULT_ALIGN __attribute__ ((aligned))
#elif defined (_MSC_VER)
    #define XTL_SINGLETON_DEEFAULT_ALIGN __declspec(align(32))
#else
#error Unknown compiler
#endif
#else
    #define XTL_SINGLETON_DEEFAULT_ALIGN
#endif
    
    struct initializer
    {
      max_align buffer XTL_SINGLETON_DEEFAULT_ALIGN;
      T*        instance XTL_SINGLETON_DEEFAULT_ALIGN;
      
      initializer () : instance (new (&buffer) T) {}
    };

    static initializer instance_holder;

    return *instance_holder.instance;
  }
};

}

template <class T, bool need_destroy>
T& singleton_default<T, need_destroy>::instance ()
{
  detail::singleton_default_object_creator<T, need_destroy>::creator.do_nothing ();

  return detail::singleton_default_instance<T, need_destroy>::get ();
}
