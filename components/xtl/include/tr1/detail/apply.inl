/*
    ���������� �������� � ������ ����������
*/

namespace detail
{

//�������� ���������� ���������� ������
template <size_t count> struct apply_selector {};

//������ ����������, ������������ �� ��������
template <class T> T& wrap_argument (T& x)
{
  return x;
}

template <class T> const T& wrap_argument (const T& x)
{
  return x;
}

//���������� ��� ��������������� ������� ��� ����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple&, apply_selector<0>)
{
  return funcall<Ret> (fn);
}

//���������� ��� ��������������� ������� c 1-� ����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<1>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)));
}

//���������� ��� ��������������� ������� c 2-�� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<2>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)));
}

//���������� ��� ��������������� ������� c 3-�� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<3>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)));
}

//���������� ��� ��������������� ������� c 4-�� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<4>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)));
}

//���������� ��� ��������������� ������� c 5-� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<5>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)),
                           wrap_argument (get<4> (args)));
}

//���������� ��� ��������������� ������� c 6-� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<6>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)),
                           wrap_argument (get<4> (args)), wrap_argument (get<5> (args)));
}

//���������� ��� ��������������� ������� c 7-� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<7>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)),
                           wrap_argument (get<4> (args)), wrap_argument (get<5> (args)), wrap_argument (get<6> (args)));
}

//���������� ��� ��������������� ������� c 8-� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<8>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)),
                           wrap_argument (get<4> (args)), wrap_argument (get<5> (args)), wrap_argument (get<6> (args)), wrap_argument (get<7> (args)));
}

//���������� ��� ��������������� ������� c 9-� �����������
template <class Ret, class Fn, class Tuple>
inline Ret apply_dispatch (Fn& fn, Tuple& args, apply_selector<9>)
{
  return funcall<Ret> (fn, wrap_argument (get<0> (args)), wrap_argument (get<1> (args)), wrap_argument (get<2> (args)), wrap_argument (get<3> (args)),
                           wrap_argument (get<4> (args)), wrap_argument (get<5> (args)), wrap_argument (get<6> (args)), wrap_argument (get<7> (args)),
                           wrap_argument (get<8> (args)));
}

}

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret apply (Fn& fn, const tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args)
{
  enum { args_count = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };

  return detail::apply_dispatch<Ret> (unwrap (fn), args, detail::apply_selector<args_count> ());
}

template <class Ret, class Fn, class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline Ret apply (Fn& fn, tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>& args)
{
  enum { args_count = tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>::size };

  return detail::apply_dispatch<Ret> (unwrap (fn), args, detail::apply_selector<args_count> ());
}
