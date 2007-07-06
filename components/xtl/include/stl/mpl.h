/*
    C����� ����� � �� ��������� (�������� �� ���� ��������� Loki)
*/

#ifndef XTL_META_PROGRAMMING_LIBRARY_HEADER
#define XTL_META_PROGRAMMING_LIBRARY_HEADER

#include <stddef.h>

namespace xtl
{

namespace mpl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Head,class Tail>
struct type_node
{
  typedef Head                 head_type; //��� ��������� ��������
  typedef Tail                 tail_type; //��� ���������� ������
  typedef type_node<Head,Tail> type;      //��� ��������������� ������ (��������� ��� ������������ ���������)
};

//������������ ���, ������������ ��� ��������� ���������� ������
struct null_type {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1=null_type,class T2=null_type,class T3=null_type,class T4=null_type,class T5=null_type,
          class T6=null_type,class T7=null_type,class T8=null_type,class T9=null_type,class T10=null_type>
struct list: public type_node<T1,typename list<T2,T3,T4,T5,T6,T7,T8,T9,T10>::result>  {};

template <class T1> struct list<T1>: public type_node<T1,null_type> {};

/*
    �������� �������������� ��� ������� �����
      ������ �� �������������� �������� ���������� ��������� ����������� ����/�������� result
*/


template <class TypeNode>                 struct length;  //����������� ����� ������
template <class TypeNode,size_t index>    struct at;      //��������� index-�� �������� ������
template <class TypeNode,class T>         struct indexof; //��������� ������� �������� (������� ��������������)
template <class TypeNode,class T>         struct append;  //���������� ���� ��� ������ ����� � ������ �����
template <class TypeNode,class T>         struct erase_first; //�������� ������� ��������� �������� � ������
template <class TypeNode,class T>         struct erase;   //�������� ���� ��������� �������� � ������
template <class TypeNode>                 struct unique;  //�������� ����������
template <class TypeNode,class T,class R> struct replace_first; //������ ������� ��������� �������� T � ������ �� R
template <class TypeNode,class T,class R> struct replace; //������ ���� ��������� �������� T � ������ �� R
template <class TypeNode>                 struct reverse; //��������� ������� ���������� ��������� � ������

/*
    ���������������� �������������� ��� ������� �����
      �������� Transform ���������� ��������� ����������� ���� result
*/

//����������� ������ � ������ c ����������� � ������� �� ��� ��������� �������� Transform
template <class TypeNode,template <class T> class Transform> struct transform;

//��������������� �������������� ��� �������
template <class TypeNode,template <class CurrentResult,class T> class Transform,class StartResult=null_type>
struct accumulate;

/*
    ��������� �������� ��������� �� ��������� ������ �����, ���� �� ����������� �������������� ��� ����
*/

//��������� �������� � ������������� �������������
template <class TypeNode,template <class T> class Unit,class Root=null_type>
struct generate_hierarchy;

//��������� �������� � ��������� �������������
template <class TypeNode,template <class Base,class T> class Unit,class Root=null_type>
struct generate_linear_hierarchy;

/*
    ����������� �������
*/

//����� �� ���� ����� condition == true -> TrueType, false -> FalseType
template <bool condition, class TrueType, class FalseType> struct select;

//compile-time assertion
template <bool condition> struct compile_time_assert;
template <>               struct compile_time_assert<true> { enum { value = 1 }; };

#include <stl/detail/ext/typelist.inl> 

}

}

#endif
