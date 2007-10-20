/*
    �������� array
*/

/*
    ��������� ����������
*/
template <class T, size_t N>
inline typename array<T,N>::iterator array<T,N>::begin ()
{
  return elems;
}

template <class T, size_t N>
inline typename array<T,N>::const_iterator array<T,N>::begin () const 
{
  return elems;
}

template <class T, size_t N>
inline typename array<T,N>::iterator array<T,N>::end ()
{
  return elems+N;
}
template <class T, size_t N>
inline typename array<T,N>::const_iterator array<T,N>::end () const
{
  return elems+N;
}

/*
    ��������� ��������� ����������
*/
template <class T, size_t N>
inline typename array<T,N>::reverse_iterator array<T,N>::rbegin ()
{
  return reverse_iterator (end ());
}

template <class T, size_t N>
inline typename array<T,N>::const_reverse_iterator array<T,N>::rbegin() const
{
  return const_reverse_iterator (end ());
}

template <class T, size_t N>
inline typename array<T,N>::reverse_iterator array<T,N>::rend ()
{
  return reverse_iterator (begin ());
}

template <class T, size_t N>
inline typename array<T,N>::const_reverse_iterator array<T,N>::rend () const
{
  return const_reverse_iterator (begin ());
}
    
/*
    ������ � �������� (operator[])
*/
template <class T, size_t N>
inline typename array<T,N>::reference array<T,N>::operator[] (size_type i) 
{ 
  return elems [i];
}

template <class T, size_t N>
inline typename array<T,N>::const_reference array<T,N>::operator[](size_type i) const 
{     
  return elems [i]; 
}
    
/*
    ������ � �������� � ��������� ��������� (at())
*/
template <class T, size_t N>
inline typename array<T,N>::reference array<T,N>::at (size_type i)
{
  rangecheck (i);
  return elems [i];
}

template <class T, size_t N>
inline typename array<T,N>::const_reference array<T,N>::at (size_type i) const
{
  rangecheck (i);
  return elems [i];
}

/*
    ������ � ������� (front) � ���������� (back) ���������
*/
template <class T, size_t N>
inline typename array<T,N>::reference array<T,N>::front () 
{ 
  return elems [0]; 
}

template <class T, size_t N>
inline typename array<T,N>::const_reference array<T,N>::front () const 
{
  return elems [0];
}

template <class T, size_t N>
inline typename array<T,N>::reference array<T,N>::back () 
{ 
  return elems [N-1]; 
}

template <class T, size_t N>
inline typename array<T,N>::const_reference array<T,N>::back () const 
{ 
  return elems [N-1]; 
}

/*
    ���������� � �������
*/
template <class T, size_t N>
inline typename array<T,N>::size_type array<T,N>::size ()
{
  return N;
}

template <class T, size_t N>
inline typename bool array<T,N>::empty ()
{
  return false;
}

template <class T, size_t N>
inline typename array<T,N>::size_type array<T,N>::max_size ()
{
  return N;
}

/*
   ����� (�� �������� �����)
*/
template <class T, size_t N>
void array<T,N>::swap (array<T,N>& y)
{
  stl::swap_ranges (begin (), end (), y.begin ());
}

/*
    ������ ������ � ������ (������ ������)
*/
template <class T, size_t N>
const T* array<T,N>::data () const
{
  return elems;
}

template <class T, size_t N>
T* array<T,N>::data ()
{ 
  return elems;
}

/*
    ������������� ��� � ������� (������/������)
*/
template <class T, size_t N>
T* array<T,N>::c_array ()
{ 
  return elems;
}

/*
    ������������ c ��������������� ����
*/
template <class T, size_t N>
template <typename T2>
array<T,N>& array<T,N>::operator= (const array<T2,N>& rhs)
{
    stl::copy (rhs.begin (),rhs.end (), begin ());
    return *this;
}

/*
    ������������ ������ �������� ��� ���� ���������
*/
template <class T, size_t N>
void array<T,N>::assign (const T& value)
{
    stl::fill_n (begin (), size (), value);
}

/*
    �������� ���������
*/
template <class T, size_t N>
void array<T,N>::rangecheck (typename array<T,N>::size_type i)
{
  if (i >= size ())
  {
      throw stl::range_error ("array<>: index out of range");
  }
}
