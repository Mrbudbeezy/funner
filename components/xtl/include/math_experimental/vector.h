#ifndef MATHLIB_VECTOR_HEADER
#define MATHLIB_VECTOR_HEADER

#include <stddef.h>

#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

#undef min
#undef max




namespace math
{

//forward declaration
template <class Type, size_t SizeX, size_t SizeY> class matrix;

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� �� ���������
////////////////////////////////////////////////////////////////////////////////////////////
template <class Type, size_t Size> struct vec_base
{
  Type&       operator [] (size_t index)       { return x [index]; }
  const Type& operator [] (size_t index) const { return x [index]; }
 
  Type x [Size];
};

template <class Type> struct vec_base<Type, 2>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y;
};

template <class Type> struct vec_base<Type, 3>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y, z;
};

template <class Type> struct vec_base<Type, 4>
{
  Type&       operator [] (size_t index)       { return (&x) [index]; }
  const Type& operator [] (size_t index) const { return (&x) [index]; }
 
  Type x, y, z, w;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Type, size_t Size>
class vec: public vec_base<Type, Size>
{
  public:
    typedef vec_base<Type, Size> base;
    typedef Type                value_type;

    enum { size = Size };

////////////////////////////////////////////////////////////////////////////////////////////
///������������
////////////////////////////////////////////////////////////////////////////////////////////
    vec ();
    vec (const Type&);
    vec (const base&);

      //��� ������� - �� ������,  �� ��� ���� ����������
    vec (const Type&, const Type&, const Type& = 0, const Type& = 0); 
    vec (const vec<Type, Size-1>&, const Type&);

//////////////////////////////////////////////////////////////////////////////////////////////

  template <size_t Size1,  size_t Size2,  class Fn> vec (const vec<Type, Size1>v1, const vec<Type, Size2>v2, Fn fn);
  template <size_t Size1, class Fn>                 vec (const vec<Type, Size1>& v1, const Type& arg, Fn fn);
  template <size_t Size1, class Fn>                 vec (const vec<Type, Size1>& v1, Fn fn);
  template <class Fn>                               vec (const Type& v1, Fn fn);

//////////////////////////////////////////////////////////////////////////////////////////////////
   //��������� ������������. ������� ����� �������������� ����������

    template <class T1, class T2> vec (const T1&, const T2&, void (*eval)(vec&, const T1&, const T2&));

    template <size_t Size1>       vec (const vec<Type, Size1>&); 

////////////////////////////////////////////////////////////////////////////////////////////
///������� +, -,  ����� (length)
////////////////////////////////////////////////////////////////////////////////////////////
    const vec    operator +     () const;
    const vec    operator -     () const;
          Type   length         () const;
          Type   squared_length () const;
////////////////////////////////////////////////////////////////////////////////////////////
///������������
////////////////////////////////////////////////////////////////////////////////////////////
  template<class T> vec&   operator =  (const T&); 
                    vec&   operator =  (const base&);

////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
    vec&       operator += (const vec&);
    vec&       operator -= (const vec&);
    const vec  operator +  (const vec&)  const;
    const vec  operator -  (const vec&)  const;


    vec&       operator *= (const vec& );
    vec&       operator *= (const Type&);

    vec&       operator /= (const vec& );
    vec&       operator /= (const Type&);

    const vec  operator *  (const vec& ) const;
    const vec  operator *  (const Type&) const;

    const vec  operator /  (const vec& ) const;
    const vec  operator /  (const Type&) const;


////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ���������           
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const vec&) const;      
    bool operator != (const vec&) const;
    bool operator <  (const vec&) const;      
    bool operator >  (const vec&) const;
    bool operator <= (const vec&) const;      
    bool operator >= (const vec&) const;

    bool equal       (const vec&, const Type& eps); 


////////////////////////////////////////////////////////////////////////////////////////////
///C�������� ������������
////////////////////////////////////////////////////////////////////////////////////////////
    const Type operator &  (const vec&) const;


};

template <class Type> class vec<Type, 0> {};

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
  const vec<T,Size> operator *  (const T& a, const vec<T,Size>& v)    { return v * a; }


////////////////////////////////////////////////////////////////////////////////////////////
///��������������
////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const matrix<T, Size, 1>    to_matrix_column (const vec<T,Size>&);

template<class T,size_t Size>
const matrix<T, 1, Size>    to_matrix_row    (const vec<T,Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///������������
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const vec<T, Size> normalize (const vec<T, Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������
//////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const vec<T, Size> abs       (const vec<T, Size>&);

//////////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
//////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
const vec<T, 3> operator ^ (const vec<T, 3>& a, const vec<T, 3>& b);


//////////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������/��������
//////////////////////////////////////////////////////////////////////////////////////////////////////
    
template <class T,size_t Size>
const vec<T,Size> min (const vec<T,Size>&,const vec<T,Size>&); 

template <class T,size_t Size>
const vec<T,Size> max (const vec<T,Size>&,const vec<T,Size>&); 

//////////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����� ���������
//////////////////////////////////////////////////////////////////////////////////////////////////////
template <class T,size_t Size>
const T angle (const vec<T, Size>& a, const vec<T, Size>& b);


}
#endif
