#ifndef __MATHLIB_QUATERNION__
#define __MATHLIB_QUATERNION__

#pragma pack (push,1)

namespace math
{

template <class type> struct quat_base
{
  type x,y,z,w;

  type&       operator [] (size_t index)       { return (&x) [index]; }
  const type& operator [] (size_t index) const { return (&x) [index]; }
};

/////////////////////////////////////////////////////////////////////////////////////////////
///����������
/////////////////////////////////////////////////////////////////////////////////////////////
template <class type> 
class quat: public quat_base<type>
{
  public:
    typedef type value_type;
  
    quat ();
    quat (const type& x,const type& y,const type& z,const type& w);
    quat (const type&);
    quat (const matrix<type,3>&);
    quat (const matrix<type,4>&);
    
        //��� ������������� ����������� ������������� ��������
    template <class T>                    quat (const T&,void (*eval)(quat&,const T&));
    template <class T1,class T2>          quat (const T1&,const T2&,void (*eval)(quat&,const T1&,const T2&));
    template <class T1,class T2,class T3> quat (const T1&,const T2&,const T3&,void (*eval)(quat&,const T1&,const T2&,const T3&));

////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
    const quat& operator + () const { return *this; }
    const quat  operator - () const;
    type        operator ~ () const;

////////////////////////////////////////////////////////////////////////////////////////////
///������������
////////////////////////////////////////////////////////////////////////////////////////////
    quat&   operator = (const matrix<type,3>&);
    quat&   operator = (const matrix<type,4>&);
    quat&   operator = (const type&);  
    quat&   operator = (const quat_base<type>&);

////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� ��������
////////////////////////////////////////////////////////////////////////////////////////////
    quat&       operator += (const quat&);
    quat&       operator -= (const quat&);
    quat&       operator *= (const quat&);
    quat&       operator *= (const type&);
    quat&       operator /= (const type&);
     
    const quat  operator +  (const quat& q) const;
    const quat  operator -  (const quat& q) const;
    const quat  operator *  (const quat& q) const;
    const quat  operator *  (const type& a) const;
    const quat  operator /  (const type& a) const;

    friend const quat operator * (const type& a,const quat& q)  { return q * a; }

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������������
////////////////////////////////////////////////////////////////////////////////////////////
    bool operator  ==   (const quat&) const;
    bool operator  !=   (const quat&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///������� �������
////////////////////////////////////////////////////////////////////////////////////////////
   const vec<type,4> operator * (const vec<type,4>&) const;
   const vec<type,3> operator * (const vec<type,3>&) const;

////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
////////////////////////////////////////////////////////////////////////////////////////////
   type operator & (const quat&);    
   
////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������� ������������� � ������ ������
////////////////////////////////////////////////////////////////////////////////////////////
    using quat_base<type>::x;
    using quat_base<type>::y;
    using quat_base<type>::z;
    using quat_base<type>::w;
};

#pragma pack (pop)

////////////////////////////////////////////////////////////////////////////////////////////
///�������
////////////////////////////////////////////////////////////////////////////////////////////

///����� (������)
template <class type>
type length (const quat<type>&);

///����� (������� �����)
template <class type>
type norm (const quat<type>&);

///���������� ��������� ����������
template <class type>
quat<type> normalize (const quat<type>&);

///���������
template <class type> 
bool equal (const quat<type>&,const quat<type>&,const type& eps);

///��������� ������������
template <class type>
type inner (const quat<type>&,const quat<type>&);

///�������� ����������
template <class type>
quat<type> invert (const quat<type>&);

}

#endif
