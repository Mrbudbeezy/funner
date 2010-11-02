#ifndef XTL_SIGNAL_HEADER
#define XTL_SIGNAL_HEADER

#include <xtl/tuple>
#include <xtl/function.h>
#include <xtl/connection.h>

namespace stl
{

struct bidirectional_iterator_tag;

}

namespace xtl
{

namespace detail
{

//implementation forwards
template <class Signature> class slot_impl;

}

//forwards declarations
template <class Signature, class Accumulator> class signal;
template <class Result>                       struct default_signal_accumulator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
class slot
{
  template <class Signature1, class Accumulator> friend class signal;
  public:
    typedef xtl::function<Signature> function_type;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    slot  ();
    slot  (const slot&);
    slot  (const function_type& fn);
    ~slot ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    slot& operator = (const slot&);
    slot& operator = (const function_type&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������-���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const function_type& function () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������, ��������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection connection () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    slot previos () const;
    slot next    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������� ����� / ������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool connected  () const;
    void disconnect ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void block   ();
    void unblock ();
    bool blocked () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (slot&);
    
  private:
    typedef detail::slot_impl<Signature> slot_impl;
  
    slot (const function_type&, slot_impl*);    

  private:
    slot_impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
void swap (slot<Signature>&, slot<Signature>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������������� ����������� ����� ����� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature>
class auto_slot: public slot<Signature>
{
  typedef slot<Signature> base;
  public:
    typedef typename base::function_type function_type;  
  
    auto_slot  () {}
    auto_slot  (const slot<Signature>&);
    auto_slot  (const function_type& fn);
    ~auto_slot ();

    auto_slot& operator = (const slot<Signature>&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Signature, class Accumulator = default_signal_accumulator<typename function<Signature>::result_type> >
class signal
{
  struct dummy { void nonnull () {} };
  typedef void (dummy::*safe_bool)();
  typedef function<Signature> function_type;
  public:
    typedef typename function_type::arg1_type      arg1_type;
    typedef typename function_type::arg2_type      arg2_type;
    typedef typename function_type::arg3_type      arg3_type;
    typedef typename function_type::arg4_type      arg4_type;
    typedef typename function_type::arg5_type      arg5_type;
    typedef typename function_type::arg6_type      arg6_type;
    typedef typename function_type::arg7_type      arg7_type;
    typedef typename function_type::arg8_type      arg8_type;
    typedef typename function_type::arg9_type      arg9_type;
    typedef Accumulator                            accumulator_type;
    typedef typename accumulator_type::result_type result_type;
    typedef slot<Signature>                        slot_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    signal  ();
    ~signal ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

    operator safe_bool () const { return empty () ? 0 : &dummy::nonnull; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ������������ ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
                        connection connect    (slot_type&);
                        connection connect    (const function_type&);
    template <class Fn> void       disconnect (Fn);

    void disconnect_all ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t num_slots () const; //�������� �����

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    result_type operator () () const;
    result_type operator () (arg1_type) const;
    result_type operator () (arg1_type, arg2_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type) const;
    result_type operator () (arg1_type, arg2_type, arg3_type, arg4_type, arg5_type, arg6_type, arg7_type, arg8_type, arg9_type) const;
    
  private:
    typedef detail::slot_impl<Signature> slot_impl;

    template <class Tuple> result_type invoke (const Tuple&) const;    

  private:
    signal (const signal&); //no impl
    signal& operator = (const signal&); //no impl

  private:
    slot_impl* first;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� ���������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace type_traits
{

template <class T> struct is_function;

template <class Signature, class Accumulator> struct is_function<signal<Signature, Accumulator> > { enum { value = true }; };

}

namespace detail
{

template <class T> struct functional_traits;

template <class Signature, class Accumulator> struct functional_traits<signal<Signature, Accumulator> >: public functional_traits<Signature>
{
  enum { is_function = false }; 
};

}

#include <xtl/detail/slot.inl>
#include <xtl/detail/signal.inl>

}

#endif
