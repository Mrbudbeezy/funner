namespace detail
{

/*
    ����� ��� ��������: ���� �� ���������� �� A* � B*
*/

template <class From, class To> struct light_is_convertible
{
  typedef char                       no_type;
  typedef struct { char dummy [2]; } yes_type;

  static no_type  test (...);
  static yes_type test (const To*);

  enum { value = sizeof (yes_type) == sizeof (test ((From*)0)) };
};

/*
    �������� ������ ��������������� �������
*/

template <int value> struct funcall_tag {};

typedef funcall_tag<0> default_funcall;     //f (arg1, ..., argN)
typedef funcall_tag<1> memfun_ptr_funcall;  //((*arg1).*f)(arg2, ..., argN)
typedef funcall_tag<2> memfun_ref_funcall;  //(arg1.*)(arg2, ..., argN)
typedef funcall_tag<3> memptr_ptr_funcall;  //(*arg1).*f
typedef funcall_tag<4> memptr_ref_funcall;  //arg1.*f

template <class Fn, class T=void, bool is_memfunc_ptr=functional_traits<Fn>::is_memfun> struct funcall_selector
{
  typedef typename functional_traits<Fn>::object_type object_type;
  typedef typename mpl::select<light_is_convertible<T, object_type>::value, memfun_ref_funcall, memfun_ptr_funcall>::type type;
};

template <class Fn, class T> struct funcall_selector<Fn, T, false>
{
  typedef default_funcall type;
};

template <class Ret, class T1, class T2> struct funcall_selector<Ret T1::*, T2, false>
{
  typedef typename mpl::select<light_is_convertible<T2, T1>::value, memptr_ref_funcall, memptr_ptr_funcall>::type type;
};

template <class Fn> struct funcall_selector<Fn, void, true>;

/*
    ���������� ������� �������������� ��������
*/

//���������� ��� ��������������� ������� ��� ����������
template <class Ret, class Fn>
inline Ret funcall_dispatch (Fn& fn, default_funcall)
{
  return static_cast<Ret> (fn ());
}

//���������� ��� ��������������� ������� � 1-� ����������
template <class Ret, class Fn, class T1>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, default_funcall)
{
  return static_cast<Ret> (fn (arg1));
}

//���������� ��� ��������������� ������� � 2-�� �����������
template <class Ret, class Fn, class T1, class T2>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2));
}

//���������� ��� ��������������� ������� � 3-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3));
}

//���������� ��� ��������������� ������� � 4-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4));
}

//���������� ��� ��������������� ������� � 5-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4, arg5));
}

//���������� ��� ��������������� ������� � 6-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4, arg5, arg6));
}

//���������� ��� ��������������� ������� � 7-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7));
}

//���������� ��� ��������������� ������� � 8-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

//���������� ��� ��������������� ������� � 9-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall_dispatch (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, default_funcall)
{
  return static_cast<Ret> (fn (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

/*
    ���������� ������� ���������� �� �������-���� ������
*/

//���������� ��� ��������� �� �������-���� ������ � 1-� ����������
template <class Ret, class Fn, class T1>
inline Ret funcall_dispatch (Fn& fn, T1& object, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)());
}

template <class Ret, class Fn, class T1>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)());
}

//���������� ��� ��������� �� �������-���� ������ � 2-�� �����������
template <class Ret, class Fn, class T1, class T2>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2));
}

template <class Ret, class Fn, class T1, class T2>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2));
}

//���������� ��� ��������� �� �������-���� ������ � 3-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3));
}

template <class Ret, class Fn, class T1, class T2, class T3>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3));
}

//���������� ��� ��������� �� �������-���� ������ � 4-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4));
}

//���������� ��� ��������� �� �������-���� ������ � 5-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4, arg5));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4, arg5));
}

//���������� ��� ��������� �� �������-���� ������ � 6-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4, arg5, arg6));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6));
}

//���������� ��� ��������� �� �������-���� ������ � 7-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7));
}

//���������� ��� ��������� �� �������-���� ������ � 8-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8));
}

//���������� ��� ��������� �� �������-���� ������ � 9-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall_dispatch (Fn& fn, T1& object, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, memfun_ref_funcall)
{
  return static_cast<Ret> ((object.*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall_dispatch (Fn& fn, T1& object_ptr, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9, memfun_ptr_funcall)
{
  return static_cast<Ret> (((*object_ptr).*fn)(arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9));
}

/*
    ���������� ������� ���������� �� ������-���� ������
*/

//���������� ��� ��������� �� ������-���� ������ � 1-� ����������
template <class Ret, class R, class T, class T1>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 2-�� �����������
template <class Ret, class R, class T, class T1, class T2>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 3-�� �����������
template <class Ret, class R, class T, class T1, class T2, class T3>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 4-�� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 5-� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, T5&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 6-� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 7-� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 8-� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, T8&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, T8&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

//���������� ��� ��������� �� ������-���� ������ � 9-� �����������
template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall_dispatch (R T::*& ptr, T1& object, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, memptr_ref_funcall)
{
  return static_cast<Ret> (const_cast<T&> (object).*ptr);
}

template <class Ret, class R, class T, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall_dispatch (R T::*& ptr, T1& object_ptr, T2&, T3&, T4&, T5&, T6&, T7&, T8&, T9&, memptr_ptr_funcall)
{
  return static_cast<Ret> (const_cast<T&> (*object_ptr).*ptr);
}

}

/*
    ��������������� ������� �������������� ��������
*/

//���������� ��� ��������������� ������� ��� ����������
template <class Ret, class Fn>
inline Ret funcall (Fn& fn)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), typename detail::funcall_selector<Fn>::type ());
}

//���������� ��� ��������������� ������� � 1-� ����������
template <class Ret, class Fn, class T1>
inline Ret funcall (Fn& fn, T1& arg1)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 2-�� �����������
template <class Ret, class Fn, class T1, class T2>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 3-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 4-�� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 5-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, arg5, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 6-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, arg5, arg6, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 7-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, arg5, arg6, arg7, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 8-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, typename detail::funcall_selector<Fn, T1>::type ());
}

//���������� ��� ��������������� ������� � 9-� �����������
template <class Ret, class Fn, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret funcall (Fn& fn, T1& arg1, T2& arg2, T3& arg3, T4& arg4, T5& arg5, T6& arg6, T7& arg7, T8& arg8, T9& arg9)
{
  return detail::funcall_dispatch<Ret> (unwrap (fn), arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, typename detail::funcall_selector<Fn, T1>::type ());
}
