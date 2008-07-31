#ifndef MATHLIB_PLANE_HEADER
#define MATHLIB_PLANE_HEADER

#include <math_experimental/forward.h>
#include "vector.h"
#include "functional.h"
#include <stddef.h>
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

namespace math
{

//��窠
template <class Type>
class point
{
 public:
  typedef Type type;
  
  enum {size=3};

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //����������
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  point();
  point(const type& x_, const type& y_, const type& z_=T(0));
  point(const point<type>& p);

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //�������� (��� ᮢ���⨬��� � �㭪樮������)
  ///////////////////////////////////////////////////////////////////////////////////////////////////

        type& operator [](size_t index)       {return (&x) [index]; }
  const	type& operator [](size_t index) const {return (&x) [index]; }

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //������ �������
  ///////////////////////////////////////////////////////////////////////////////////////////////////

  point<type>& move (const type& x_, const type& y_, const type& z_);
  point<type>& operator= (const point<type>& p);

 private:
  type x, y, z;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//�⨫���
///////////////////////////////////////////////////////////////////////////////////////////////////

//�������� �����
template<class T>
const vec<T,3> create_vector(const point<T>& p1,const point<T>& p2);

//���᪮���
template<class Type>
class plane
{
  public:

  typedef Type type;

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //����������
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  plane();//�� 㬮�砭��
  plane(const vec<type,3>& normal, const point<type>& control=point<type>(0,0,0));//�१ ��� � ��ଠ��� �����
  plane(const point<type>& p1, const point<type>& p2, const point<type>& p3);//�१ �� �窨
  plane(const vec<type,3>& v1, const vec<type,3>& v2, const point<type> p);//�१ ��� ���ᥪ��騥�� ����
  plane(const point<type>& p1, const point<type>& p2,const vec<type,3> v);//�१ ��� ��ࠫ����� ����
  plane(const plane<type>& p);//����஢����

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //������ ���祭��
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  const vec<type,3>& get_vector() const {return normal_vector; }
  const point<type>& get_point () const {return control_point; }

  private:
  point<type> control_point;
  vec<type,3> normal_vector;
};

#include "impl/point.inl"
#include "impl/plane.inl"

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
