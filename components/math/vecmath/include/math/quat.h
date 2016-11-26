#ifndef MATHLIB_VECMATH_QUATERNION_HEADER
#define MATHLIB_VECMATH_QUATERNION_HEADER

#include <math/vector.h>

#ifdef _MSC_VER
  #pragma pack (push, 1)
#endif

namespace math
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///База кватерниона
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct quat_base { T x, y, z, w; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кватернион
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class quat: public quat_base<T>
{
  public:
    typedef T value_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    using quat_base<T>::x;
    using quat_base<T>::y;
    using quat_base<T>::z;
    using quat_base<T>::w;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    quat ();
    quat (const value_type& x, const value_type& y, const value_type& z, const value_type& w);
    quat (const value_type& a);
    quat (const quat& q);
    
        //для использования оптимизации возвращаемого значения
    template <class T1, class Fn>                     quat (const T1& arg, Fn fn, return_value_tag); //fn (arg, *this)
    template <class T1, class T2, class Fn>           quat (const T1& arg1, const T2& arg2, Fn fn, return_value_tag); //fn (arg1, arg2, *this)
    template <class T1, class T2, class T3, class Fn> quat (const T1& arg1, const T2& arg2, const T3& arg3, Fn fn, return_value_tag); //fn (arg1, arg2, arg3, *this)
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    quat& operator = (const value_type&);
    quat& operator = (const quat&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Индексирование
///////////////////////////////////////////////////////////////////////////////////////////////////    
          value_type& operator [] (unsigned int index);
    const value_type& operator [] (unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Унарные операции
///////////////////////////////////////////////////////////////////////////////////////////////////
    const quat& operator + () const;
          quat  operator - () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Бинарные операции
///////////////////////////////////////////////////////////////////////////////////////////////////
    quat& operator += (const quat&);
    quat& operator -= (const quat&);
    quat& operator *= (const quat&);
    quat& operator *= (const value_type&);
    quat& operator /= (const value_type&);
    quat  operator +  (const quat&) const;
    quat  operator -  (const quat&) const;
    quat  operator *  (const quat&) const;
    quat  operator *  (const value_type&) const;
    quat  operator /  (const value_type&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между кватернионами
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const quat&) const;
    bool operator != (const quat&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поворот вектора
///////////////////////////////////////////////////////////////////////////////////////////////////
   vector<value_type, 4> operator * (const vector<value_type, 4>&) const;
   vector<value_type, 3> operator * (const vector<value_type, 3>&) const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef quat<float>          quatf;
typedef quat<double>         quatd;
typedef quat<int>            quati;
typedef quat<unsigned int>   quatui;
typedef quat<short>          quats;
typedef quat<unsigned short> quatus;
typedef quat<char>           quatb;
typedef quat<unsigned char>  quatub;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Умножение на скаляр
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
quat<T> operator * (const typename quat<T>::value_type& a, const quat<T>& q);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поворот вектора
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
vector<typename quat<T>::value_type, Size> operator * (const vector<typename quat<T>::value_type, Size>&, const quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Длина
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
typename quat<T>::value_type length (const quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Норма (квадрат длины)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
typename quat<T>::value_type norm (const quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Нормирование (возвращает единичный кватернион)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
quat<T> normalize (const quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> 
bool equal (const quat<T>&, const quat<T>&, const T& eps);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Скалярное произведение
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
typename quat<T>::value_type inner (const quat<T>&, const quat<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обратный кватернион
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
quat<T> inverse (const quat<T>&);

#include <math/detail/quat.inl>

}

#ifdef _MSC_VER
  #pragma pack (pop)
#endif

#endif
