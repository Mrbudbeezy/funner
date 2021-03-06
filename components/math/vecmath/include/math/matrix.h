#ifndef MATHLIB_VECMATH_MATRIX_HEADER
#define MATHLIB_VECMATH_MATRIX_HEADER

#include <math/vector.h>

#ifdef _MSC_VER  
  #pragma pack(push,1)
#endif

#ifdef minor
#undef minor
#endif

namespace math
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Матрица
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
class matrix
{
  public:
    typedef math::vector<T, Size>       vector;     //вектор строка
    typedef typename vector::value_type value_type; //тип элементов
    
    enum { size = Size };

///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    matrix ();
    matrix (const value_type& a);      //a будет записано на главной диагонали
    matrix (const value_type* values); //values - массив значений матрицы                                           

      //для использования оптимизации возвращаемого значения
    template <class T1, class Fn>                     matrix (const T1& arg, Fn fn, return_value_tag);                  //fn (arg, *this)
    template <class T1, class T2, class Fn>           matrix (const T1& arg1, const T2& arg2, Fn fn, return_value_tag); //fn (arg1, arg2, *this)
    template <class T1, class T2, class T3, class Fn> matrix (const T1& arg1, const T2& arg2, const T3& arg3, Fn fn, return_value_tag); //fn (arg1, arg2, arg3, *this)

///////////////////////////////////////////////////////////////////////////////////////////////////
///Индексирование
///////////////////////////////////////////////////////////////////////////////////////////////////
          vector& row         (unsigned int index);
    const vector& row         (unsigned int index) const;
          vector  column      (unsigned int index) const; //копия!
          vector& operator [] (unsigned int index);
    const vector& operator [] (unsigned int index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Унарные операции
///////////////////////////////////////////////////////////////////////////////////////////////////
    const matrix& operator + () const;
          matrix  operator - () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    matrix& operator = (const value_type&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Основные арифметические операции
///////////////////////////////////////////////////////////////////////////////////////////////////
    matrix& operator += (const matrix&);
    matrix& operator -= (const matrix&);
    matrix& operator *= (const value_type&);
    matrix& operator /= (const value_type&);
    matrix& operator *= (const matrix&);
    matrix  operator +  (const matrix&) const;
    matrix  operator -  (const matrix&) const;
    matrix  operator *  (const value_type&) const;
    matrix  operator /  (const value_type&) const; 
    matrix  operator *  (const matrix&) const; 

///////////////////////////////////////////////////////////////////////////////////////////////////
///Умножение на вектор (матрица строка - вектор столбец)
///////////////////////////////////////////////////////////////////////////////////////////////////
    vector                  operator * (const vector&) const;
    math::vector<T, Size-1> operator * (const math::vector<T, Size-1>&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Отношения между матрицами
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const matrix&) const;
    bool operator != (const matrix&) const;

  private:
    vector x [size];  
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Переопределения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef matrix<float, 2>  mat2f;
typedef matrix<float, 3>  mat3f;
typedef matrix<float, 4>  mat4f;
typedef matrix<double, 2> mat2d;
typedef matrix<double, 3> mat3d;
typedef matrix<double, 4> mat4d;
typedef matrix<int, 2>    mat2i;
typedef matrix<int, 3>    mat3i;
typedef matrix<int, 4>    mat4i;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Умножение на скаляр
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
matrix<T, Size> operator * (const T& a, const matrix<T, Size>& m);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Умножение вектора строки на матрицу столбец
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
vector<T, Size> operator * (const vector<T, Size>& v, const matrix<T, Size>& m);

template <class T, unsigned int Size>
vector<T, Size> operator * (const vector<T, Size>& v, const matrix<T, Size+1>& m);

template <class T, unsigned int Size>
vector<T, Size>& operator *= (vector<T, Size>& v, const matrix<T, Size>& m);

template <class T, unsigned int Size>
vector<T, Size>& operator *= (vector<T, Size>& v, const matrix<T, Size+1>& m);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие транспонированнной матрицы 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
matrix<T, Size> transpose (const matrix<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие обратной матрицы
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
matrix<T, Size> inverse (const matrix<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Взятие нормированной матрицы
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
matrix<T, Size> normalize (const matrix<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определитель (для квадратной матрицы)
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size> 
typename matrix<T, Size>::value_type det (const matrix<T, Size>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Минор
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size>
typename matrix<T, Size>::value_type minor (const matrix<T, Size>&, unsigned int, unsigned int);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение матриц
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T, unsigned int Size> 
bool equal (const matrix<T, Size>&, const matrix<T, Size>&, const T& eps);

#include <math/detail/matrix.inl>

}

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#endif
