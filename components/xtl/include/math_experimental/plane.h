#ifndef MATHLIB_PLANE_HEADER
#define MATHLIB_PLANE_HEADER

#include "forward.h"
#include "vector.h"
#include "functional.h"
#include <stddef.h>
#include <math.h>
#ifdef _MSC_VER
  #pragma pack (push,1)
#endif

namespace math
{

//���᪮���
template<class Type,size_t Size>
class plane
{
 public:

  typedef Type type;
  enum {size=Size};
  typedef vec<type,size> value_vec;

  enum Side {
   positive=1,
   negative=-1,
   zero=0
  };

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //����������
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  plane();//�� 㬮�砭��
  plane(const vec<type,Size>& normal_vec, const vec<type,Size>& control_pnt=vec<type,Size>(0));//�१ ��� � ��ଠ��� �����
  plane(const plane<type,Size>& pl);//����஢����

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //������ ���祭��
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  const value_vec& get_normal_vector () const {return normal_vector; }
  const value_vec& get_control_point () const {return control_point; }

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  //������ �������
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  bool operator == (const plane<Type,Size>& p) const;

  plane<Type,Size>& operator = (const plane<Type,Size>& p);

  plane<Type,Size>&       operator *= (const quat<Type>& q);
  const plane<Type,Size>  operator *   (const quat<Type>& q) const ;
  


 private:
  value_vec control_point;
  value_vec normal_vector;
};


///////////////////////////////////////////////////////////////////////////////////////////////////
//������� ���᪮�� 
///////////////////////////////////////////////////////////////////////////////////////////////////

//��१ �� �窨
template<class T,size_t Size>
const plane<T,Size> plane_by_3points (const vec<T,Size>& point1, const vec<T,Size>& point2, const vec<T,Size>& point3);

//��१ ��� ���ᥪ��騥�� ����
template<class T,size_t Size>
const plane<T,Size> plane_by_2crosslines(const vec<T,Size>& vec1, const vec<T,Size>& vec2, const vec<T,Size>& point);

//��१ ��� ��ࠫ����� ����
template<class T,size_t Size>
const plane<T,Size> plane_by_2parallel_lines(const vec<T,Size>& point1, const vec<T,Size>& point2, const vec<T,Size>& vec);

///////////////////////////////////////////////////////////////////////////////////////////////////
//�����ﭨ� �� �窨
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class T,size_t Size>
const T distance(const plane<T,Size>& pl, const vec<T,Size>& point, typename plane<T,Size>::Side& s);

template<class T,size_t Size>
const T distance(const plane<T,Size>& pl, const vec<T,Size>& point);

/*
	�᭮��� ⨯�
*/
typedef plane<float,3>              plane3f;
typedef plane<double,3>             plane3d;
typedef plane<int,3>                plane3i;
typedef plane<unsigned int,3>       plane3ui;
typedef plane<short,3>              plane3s;
typedef plane<unsigned short,3>     plane3us;
typedef plane<char,3>               plane3b;
typedef plane<unsigned char,3>      plane3ub;

typedef plane<float,4>              plane4f;
typedef plane<double,4>             plane4d;
typedef plane<int,4>                plane4i;
typedef plane<unsigned int,4>       plane4ui;
typedef plane<short,4>              plane4s;
typedef plane<unsigned short,4>     plane4us;
typedef plane<char,4>               plane4b;
typedef plane<unsigned char,4>      plane4ub;

#include "impl/plane.inl"

#ifdef _MSC_VER
  #pragma pack (pop)
#endif
}

#endif
