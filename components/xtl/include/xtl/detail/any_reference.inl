/*
    ������������ / ����������
*/

inline any_reference::any_reference ()
  : data (0),
    data_type (&typeid (void))
{
}

template <class T>
inline any_reference::any_reference (T& referenced_object)
  : data ((void*)&referenced_object),
    data_type (&typeid (T))
{
}

/*
    ��� � ������
*/

inline const std::type_info& any_reference::type () const
{
  return *data_type;
}

template <class T>
inline T* any_reference::content () const
{
  if (&typeid (T) != data_type)
    return 0;
    
  return static_cast<T*> (data);
}

/*
    ���������� � ���������� ���� (���������� � ������ �������)
*/

template <class T>
inline T& any_reference::cast () const
{
  T* result = content<T> ();

  if (!result)
    throw bad_any_cast (bad_any_cast::bad_direct_cast, *data_type, typeid (T));

  return *result;
}

/*
    ���������� �����
*/

template <class T>
inline T* any_cast (any_reference* ref)
{
  return ref ? ref->template content<T> () : 0;
}

template <class T>
inline T& any_cast (any_reference& ref)
{
  return ref.template cast<T> ();
}
