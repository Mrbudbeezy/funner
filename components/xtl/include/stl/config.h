#ifndef __MYSTL_CONFIG__
#define __MYSTL_CONFIG__

namespace stl
{

//forward declarations
template <class T> class allocator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ��������� (����� ���� ��������������� �������������)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct default_allocator        { typedef allocator<T> allocator_type; };
template <class T> struct default_string_allocator { typedef allocator<T> allocator_type; };

}

#endif
