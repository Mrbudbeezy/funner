/*
    ������������ / ����������
*/

template <class T, class Allocator>
inline uninitialized_storage<T, Allocator>::uninitialized_storage (const allocator_type& in_allocator)
  : allocator_type (in_allocator), start (0), finish (0), end_of_storage (0)
  {}
  
template <class T, class Allocator>
inline uninitialized_storage<T, Allocator>::uninitialized_storage (size_type size, const allocator_type& in_allocator)
  : allocator_type (in_allocator), start (0), finish (0), end_of_storage (0)
{
  resize (size);
}

template <class T, class Allocator>    
inline uninitialized_storage<T, Allocator>::uninitialized_storage (const uninitialized_storage& storage)
  : allocator_type (storage), start (0), finish (0), end_of_storage (0)
{
  resize (storage.size ());  
  memcpy (start, storage.start, sizeof (T) * (finish - start));
}
    
template <class T, class Allocator>    
inline uninitialized_storage<T, Allocator>::~uninitialized_storage ()
{
  if (start)
    this->deallocate (start, end_of_storage - start);
}

/*
    ������������
*/

template <class T, class Allocator>
inline uninitialized_storage<T, Allocator>& uninitialized_storage<T, Allocator>::operator = (const uninitialized_storage& storage)
{
  if (this != &storage)
    uninitialized_storage (storage).swap (*this);

  return *this;
}

/*
    ���������
*/

template <class T, class Allocator>
inline typename uninitialized_storage<T, Allocator>::allocator_type uninitialized_storage<T, Allocator>::get_allocator () const
{
  return static_cast<const allocator_type&> (*this)
}

/*
    ������ ������ / ����� ������ (���������� ��������� ������)
*/

template <class T, class Allocator>
inline typename uninitialized_storage<T, Allocator>::size_type uninitialized_storage<T, Allocator>::size () const
{
  return finish - start;
}

template <class T, class Allocator>
inline typename uninitialized_storage<T, Allocator>::size_type uninitialized_storage<T, Allocator>::capacity () const
{
  return end_of_storage - start;
}

/*
    �������� �� �������
*/

template <class T, class Allocator>
inline bool uninitialized_storage<T, Allocator>::empty () const
{
  return start == finish;
}

/*
    ������
*/

template <class T, class Allocator>
inline typename uninitialized_storage<T, Allocator>::const_pointer uninitialized_storage<T, Allocator>::data () const
{
  return start;
}

template <class T, class Allocator>
inline typename uninitialized_storage<T, Allocator>::pointer uninitialized_storage<T, Allocator>::data ()
{
  return start;
}

/*
    ��������� ������� ������ / �������������� ������
*/

template <class T, class Allocator>
inline void uninitialized_storage<T, Allocator>::resize (size_type new_size, bool need_copy)
{
  size_type size = finish - start;
  
  if (new_size == size)
    return;

  if (new_size > size)
    reserve (new_size, need_copy);

  finish = start + new_size;
}

template <class T, class Allocator>
inline void uninitialized_storage<T, Allocator>::reserve (size_type new_size, bool need_copy)
{
  size_type size = end_of_storage - start;

  if (new_size <= size)
    return;    
    
  pointer new_buffer = this->allocate (new_size);
  
  if (need_copy && start)
    memcpy (new_buffer, start, (finish - start) * sizeof (T));

  if (start)
    this->deallocate (start, end_of_storage - start);

  finish         = new_buffer + (finish - start);
  start          = new_buffer;  
  end_of_storage = new_buffer + new_size;
}

/*
    �����
*/

template <class T, class Allocator>
inline void uninitialized_storage<T, Allocator>::swap (uninitialized_storage& s)
{
  stl::swap (start, s.start);
  stl::swap (finish, s.finish);
  stl::swap (end_of_storage, s.end_of_storage);
}

template <class T, class Allocator>
inline void swap (uninitialized_storage<T, Allocator>& s1, uninitialized_storage<T, Allocator>& s2)
{
  s1.swap (s2);
}
