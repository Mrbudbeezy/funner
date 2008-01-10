/*
    ����������
*/

inline trackable::~trackable ()
{
  try
  {
    sig ();
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    ���������� ������������ �������� �������
*/

inline connection trackable::connect_tracker (const function_type& fn)
{
  return sig.connect (fn);
}

inline connection trackable::connect_tracker (slot_type& s)
{
  return sig.connect (s);
}

template <class Fn>
inline void trackable::disconnect_tracker (Fn fn)
{
  sig.disconnect (fn);
}

inline void trackable::disconnect_all_trackers ()
{
  sig.disconnect_all ();
}

/*
    ���������� ������������
*/

inline size_t trackable::num_trackers () const
{
  return sig.num_slots ();
}
