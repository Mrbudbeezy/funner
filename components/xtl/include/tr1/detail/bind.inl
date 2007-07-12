namespace detail
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, class Args>
inline T& eval_argument (T& x, Args&)
{
  return x;
}

template <class T, class Args>
inline T& eval_argument (const reference_wrapper<T>& x, Args&)
{
  return x.get ();
}

template <class T, class Args>
inline T& eval_argument (reference_wrapper<T>& x, Args&)
{
  return x.get ();
}

template <size_t I, class Args>
inline typename tuple_element<I-1, Args>::type eval_argument (const placeholders::argument<I>&, Args& args)
{
  return get<I-1> (args);
}

template <size_t I, class Args>
inline typename tuple_element<I-1, Args>::type eval_argument (placeholders::argument<I>&, Args& args)
{
  return get<I-1> (args);
}

template <class Ret, class Fn, class Args, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>::result_type
eval_argument (const binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>& fn, Args& args)
{
  return fn.eval (args);
}

template <class Ret, class Fn, class Args, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline typename binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>::result_type
eval_argument (binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9>& fn, Args& args)
{
  return fn.eval (args);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� ����������� ���������� ��������� bind-���������, �� ���������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> inline T& wrap_result (T& r)
{
  return r;
}

template <class T> inline const T& wrap_result (const T& r)
{
  return r;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ bind
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs&, Args&, mpl::integer_constant<0>)
{
  return funcall<Ret> (fn);
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<1>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<2>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<3>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<4>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<5>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<6>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<7>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<8>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)),
                           wrap_result (eval_argument (get<7> (binded_args), args)));
}

template <class Ret, class Fn, class BindedArgs, class Args>
inline Ret bind_call (Fn& fn, BindedArgs& binded_args, Args& args, mpl::integer_constant<9>)
{
  return funcall<Ret> (fn, wrap_result (eval_argument (get<0> (binded_args), args)),
                           wrap_result (eval_argument (get<1> (binded_args), args)),
                           wrap_result (eval_argument (get<2> (binded_args), args)),
                           wrap_result (eval_argument (get<3> (binded_args), args)),
                           wrap_result (eval_argument (get<4> (binded_args), args)),
                           wrap_result (eval_argument (get<5> (binded_args), args)),
                           wrap_result (eval_argument (get<6> (binded_args), args)),
                           wrap_result (eval_argument (get<7> (binded_args), args)),
                           wrap_result (eval_argument (get<8> (binded_args), args)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ reference_wrapper
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T& unwrap_function (T& r)
{
  return r;
}

template <class T> T& unwrap_function (const reference_wrapper<T>& r)
{
  return r.get ();
}

template <class T> T& unwrap_function (reference_wrapper<T>& r)
{
  return r.get ();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� / ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct bind_equals      { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a == b; } };
struct bind_not_equals  { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a != b; } };
struct bind_less        { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a < b; } };
struct bind_greater     { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a > b; } };
struct bind_not_less    { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a >= b; } };
struct bind_not_greater { template <class T1, class T2> bool operator () (const T1& a, const T2& b) const { return a <= b; } };
struct bind_logical_not { template <class T> bool operator () (const T& a) const { return !a; } };

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� ������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn> struct bind_result_of
{
  typedef Ret type;
};

template <class Fn> struct bind_result_of<unspecified_result, Fn>
{
  typedef typename result_of<Fn>::type type;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��� bind-�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct binder
{
  typedef typename detail::make_tuple_mapper<T1, T2, T3, T4, T5, T6, T7, T8, T9>::type binded_arguments_type;
  typedef typename detail::access_traits<T1>::const_type arg1_type;
  typedef typename detail::access_traits<T2>::const_type arg2_type;
  typedef typename detail::access_traits<T3>::const_type arg3_type;
  typedef typename detail::access_traits<T4>::const_type arg4_type;
  typedef typename detail::access_traits<T5>::const_type arg5_type;
  typedef typename detail::access_traits<T6>::const_type arg6_type;
  typedef typename detail::access_traits<T7>::const_type arg7_type;
  typedef typename detail::access_traits<T8>::const_type arg8_type;
  typedef typename detail::access_traits<T9>::const_type arg9_type;
  public:
    typedef typename bind_result_of<Ret, Fn>::type result_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    binder (const Fn& in_fn) : fn (in_fn) {}   
    binder (const Fn& in_fn, arg1_type a1) : fn (in_fn), binded_arguments (a1) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2) : fn (in_fn), binded_arguments (a1, a2) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3) : fn (in_fn), binded_arguments (a1, a2, a3) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4) : fn (in_fn), binded_arguments (a1, a2, a3, a4) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7, a8) {}
    binder (const Fn& in_fn, arg1_type a1, arg2_type a2, arg3_type a3, arg4_type a4, arg5_type a5, arg6_type a6, arg7_type a7, arg8_type a8, arg9_type a9) : fn (in_fn), binded_arguments (a1, a2, a3, a4, a5, a6, a7, a8, a9) {}
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� bind-���������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    template <class Args> result_type eval (Args& args)
    {
      return bind_call<result_type> (unwrap_function (fn), binded_arguments, args, mpl::integer_constant<binded_arguments_type::size> ());
    }

    template <class Args> result_type eval (Args& args) const
    {
      return bind_call<result_type> (unwrap_function (fn), binded_arguments, args, mpl::integer_constant<binded_arguments_type::size> ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
     result_type operator () ()
     {
       return eval (tie ());
     }

     template <class A1> result_type operator () (A1& a1)
     {
       return eval (tie (a1));
     }

     template <class A1, class A2> result_type operator () (A1& a1, A2& a2)
     {
       return eval (tie (a1, a2));
     }

     template <class A1, class A2, class A3> result_type operator () (A1& a1, A2& a2, A3& a3)
     {
       return eval (tie (a1, a2, a3));
     }

     template <class A1, class A2, class A3, class A4> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4)
     {
       return eval (tie (a1, a2, a3, a4));
     }

     template <class A1, class A2, class A3, class A4, class A5> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
     {
       return eval (tie (a1, a2, a3, a4, a5));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
     {
       return eval (tie (a1, a2, a3, a4, a5, a6));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7)
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8)
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7, a8));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9)
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7, a8, a9));
     }
     
     result_type operator () () const
     {
       return eval (tie ());
     }

     template <class A1> result_type operator () (A1& a1) const
     {
       return eval (tie (a1));
     }

     template <class A1, class A2> result_type operator () (A1& a1, A2& a2) const
     {
       return eval (tie (a1, a2));
     }

     template <class A1, class A2, class A3> result_type operator () (A1& a1, A2& a2, A3& a3) const
     {
       return eval (tie (a1, a2, a3));
     }

     template <class A1, class A2, class A3, class A4> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4) const
     {
       return eval (tie (a1, a2, a3, a4));
     }

     template <class A1, class A2, class A3, class A4, class A5> result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5) const
     {
       return eval (tie (a1, a2, a3, a4, a5));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6) const
     {
       return eval (tie (a1, a2, a3, a4, a5, a6));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7) const
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8) const
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7, a8));
     }

     template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
     result_type operator () (A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7, A8& a8, A9& a9) const
     {
       return eval (tie (a1, a2, a3, a4, a5, a6, a7, a8, a9));
     }
     
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    binder<bool, bind_logical_not, binder> operator ! () const
    {
      return binder<bool, bind_logical_not, binder> (bind_logical_not (), *this);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class A> binder<bool, bind_equals, binder, A> operator == (A arg) const
    {
      return binder<bool, bind_equals, binder, A> (bind_equals (), *this, arg);
    }

    template <class A> binder<bool, bind_not_equals, binder, A> operator != (A arg) const
    {
      return binder<bool, bind_not_equals, binder, A> (bind_not_equals (), *this, arg);
    }

    template <class A> binder<bool, bind_less, binder, A> operator < (A arg) const
    {
      return binder<bool, bind_less, binder, A> (bind_less (), *this, arg);
    }

    template <class A> binder<bool, bind_greater, binder, A> operator > (A arg) const
    {
      return binder<bool, bind_greater, binder, A> (bind_greater (), *this, arg);
    }

    template <class A> binder<bool, bind_not_greater, binder, A> operator <= (A arg) const
    {
      return binder<bool, bind_not_greater, binder, A> (bind_not_greater (), *this, arg);
    }

    template <class A> binder<bool, bind_not_less, binder, A> operator >= (A arg) const
    {
      return binder<bool, bind_not_less, binder, A> (bind_not_less (), *this, arg);
    }

  private:    
    Fn                    fn;
    binded_arguments_type binded_arguments;
};

}

/*
    �������� bind-���������
*/

template <class Fn>
inline detail::binder<detail::unspecified_result, Fn> bind (Fn fn)
{
  return detail::binder<detail::unspecified_result, Fn> (fn);  
}

template <class Fn, class T1>
inline detail::binder<detail::unspecified_result, Fn, T1> bind (Fn fn, T1 arg1)
{
  return detail::binder<detail::unspecified_result, Fn, T1> (fn, arg1);
}

template <class Fn, class T1, class T2>
inline detail::binder<detail::unspecified_result, Fn, T1, T2> bind (Fn fn, T1 arg1, T2 arg2)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2> (fn, arg1, arg2);
}

template <class Fn, class T1, class T2, class T3>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3> (fn, arg1, arg2, arg3);
}

template <class Fn, class T1, class T2, class T3, class T4>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4> (fn, arg1, arg2, arg3, arg4);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5> (fn, arg1, arg2, arg3, arg4, arg5);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6> (fn, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return detail::binder<detail::unspecified_result, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

template <class Ret, class Fn>
inline detail::binder<Ret, Fn> bind (Fn fn)
{
  return detail::binder<Ret, Fn> (fn);  
}

template <class Ret, class Fn, class T1>
inline detail::binder<Ret, Fn, T1> bind (Fn fn, T1 arg1)
{
  return detail::binder<Ret, Fn, T1> (fn, arg1);
}

template <class Ret, class Fn, class T1, class T2>
inline detail::binder<Ret, Fn, T1, T2> bind (Fn fn, T1 arg1, T2 arg2)
{
  return detail::binder<Ret, Fn, T1, T2> (fn, arg1, arg2);
}

template <class Ret, class Fn, class T1, class T2, class T3>
inline detail::binder<Ret, Fn, T1, T2, T3> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3)
{
  return detail::binder<Ret, Fn, T1, T2, T3> (fn, arg1, arg2, arg3);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline detail::binder<Ret, Fn, T1, T2, T3, T4> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4> (fn, arg1, arg2, arg3, arg4);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5> (fn, arg1, arg2, arg3, arg4, arg5);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6> (fn, arg1, arg2, arg3, arg4, arg5, arg6);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> bind (Fn fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
{
  return detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> (fn, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
}

/*
    �������� ������� ����
*/

//�������� �� ��� �������������� ���������� ��������������� �������
template <class T>  struct is_placeholder: public mpl::false_type {};
template <size_t I> struct is_placeholder<placeholders::argument<I> >: public mpl::true_type {};

//�������� �� ��� ����������, ���������� ����� ������ bind
template <class T> struct is_bind_expression: public mpl::false_type {};
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
struct is_bind_expression<detail::binder<Ret, Fn, T1, T2, T3, T4, T5, T6, T7, T8, T9> >: public mpl::true_type {};
