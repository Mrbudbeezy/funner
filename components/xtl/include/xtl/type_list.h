/*
    Cписки типов и их обработка (основано на коде билиотеки Loki)
*/

#ifndef XTL_TYPE_LIST_HEADER
#define XTL_TYPE_LIST_HEADER

#include <xtl/utility>

namespace xtl
{

namespace mpl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Узел списка типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Head,class Tail>
struct type_node
{
  typedef Head                 head_type; //тип головного элемента
  typedef Tail                 tail_type; //тип хвостового списка
  typedef type_node<Head,Tail> type;      //тип результирующего списка (необходим для однородности обработки)
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Рекурсивное определение списка типов
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T1=null_type,class T2=null_type,class T3=null_type,class T4=null_type,class T5=null_type,
          class T6=null_type,class T7=null_type,class T8=null_type,class T9=null_type,class T10=null_type>
struct list: public type_node<T1,typename list<T2,T3,T4,T5,T6,T7,T8,T9,T10>::type>  {};

template <class T1> struct list<T1>: public type_node<T1,null_type> {};

/*
    Основные преобразования над списком типов
      каждая из нижеприведеных операций возвращает результат посредством типа/значения type/value
*/


template <class TypeNode>                 struct length;  //определение длины списка
template <class TypeNode,size_t index>    struct at;      //получение index-го элемента списка
template <class TypeNode,class T>         struct indexof; //получение индекса элемента (первого встретившегося)
template <class TypeNode,class T>         struct append;  //добавление типа или списка типов к списку типов
template <class TypeNode,class T>         struct erase_first; //удаление первого вхождения элемента в список
template <class TypeNode,class T>         struct erase;   //удаление всех вхождений элемента в список
template <class TypeNode>                 struct unique;  //удаление дубликатов
template <class TypeNode,class T,class R> struct replace_first; //замена первого вхождения элемента T в списке на R
template <class TypeNode,class T,class R> struct replace; //замена всех вхождений элемента T в списке на R
template <class TypeNode>                 struct reverse; //обращение порядка следования элементов в списке

/*
    Пользовательские преобразования над списком типов
      операция Transform возвращает результат посредством типа type
*/

//отображение списка в список c применением к каждому из его элементов операции Transform
template <class TypeNode,template <class T> class Transform> struct transform;

//аккумулирование преобразований над списком
template <class TypeNode,template <class CurrentResult,class T> class Transform,class StartResult=null_type>
struct accumulate;

/*
    Генерация иерархии состоящей из элементов списка типов, либо из результатов преобразований над ними
*/

//генерация иерархии с множественным наследованием
template <class TypeNode,template <class T> class Unit,class Root=null_type>
struct generate_hierarchy;

//генерация иерархии с одиночным наследованием
template <class TypeNode,template <class Base,class T> class Unit,class Root=null_type>
struct generate_linear_hierarchy;

#include <xtl/detail/typelist.inl>

}

}

#endif
