#ifndef BOUND_VOLUMES_AABB_HEADER
#define BOUND_VOLUMES_AABB_HEADER

#include <mathlib.h>

namespace bound_volumes
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
const float default_epsilon = 1e-6f;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum box_corner
{
  box_corner_nxnynz,
  box_corner_pxnynz,
  box_corner_nxpynz,
  box_corner_pxpynz,
  box_corner_nxnypz,
  box_corner_pxnypz,
  box_corner_nxpypz,
  box_corner_pxpypz,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������������� � ������� ������������� ���� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct axis_aligned_box
{
  public:
    typedef T               element_type;
    typedef math::vec<T, 3> vec_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    axis_aligned_box () {}
    axis_aligned_box (const vec_type& vmin, const vec_type& vmax);
    axis_aligned_box (const element_type& min_x, const element_type& min_y, const element_type& min_z,
                      const element_type& max_x, const element_type& max_y, const element_type& max_z);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const vec_type& minimum () const;
    const vec_type& maximum () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void set_minimum (const vec_type&);
    void set_minimum (const element_type& x, const element_type& y, const element_type& z);
    void set_maximum (const vec_type&);
    void set_maximum (const element_type& x, const element_type& y, const element_type& z);
    void set_extents (const vec_type& vmin, const vec_type& vmax);
    void set_extents (const element_type& min_x, const element_type& min_y, const element_type& min_z,
                      const element_type& max_x, const element_type& max_y, const element_type& max_z);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void reset (const vec_type& = vec_type (element_type (0)));

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ��������������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    vec_type     center () const; //�����
    vec_type     size   () const; //������ ����������������
    element_type radius () const; //������ (�������� ����� ���������)
    element_type volume () const; //����� ��������������� ����������������
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty (const element_type& eps = default_epsilon) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    vec_type corner      (box_corner) const;
    void     get_corners (vec_type corners [8]) const; //� �������, ������������� � box_corner    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� � ����� / ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    axis_aligned_box& operator += (const vec_type&);
    axis_aligned_box& operator += (const axis_aligned_box&);
    axis_aligned_box  operator +  (const vec_type&) const;
    axis_aligned_box  operator +  (const axis_aligned_box&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    axis_aligned_box& operator *= (const math::matrix<T, 4>&);
    axis_aligned_box& operator *= (const math::quat<T>&);
    axis_aligned_box  operator *  (const math::matrix<T, 4>&) const;
    axis_aligned_box  operator *  (const math::quat<T>&) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ��������������� ���������������� � ���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool intersects (const axis_aligned_box&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������: �������� �� �������������� ��������������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool contains (const vec_type& point) const;
    bool contains (const axis_aligned_box& box) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool equal (const axis_aligned_box&, const element_type& eps = default_epsilon) const;

    bool operator == (const axis_aligned_box&) const;
    bool operator != (const axis_aligned_box&) const;

  private:
    vec_type min_extent, max_extent;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef axis_aligned_box<int>    aaboxi;
typedef axis_aligned_box<float>  aaboxf;
typedef axis_aligned_box<double> aaboxd;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> axis_aligned_box<T> operator * (const math::matrix<T, 4>&, const axis_aligned_box<T>&);
template <class T> axis_aligned_box<T> operator * (const math::quat<T>&, const axis_aligned_box<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ���������� ���� �������������� �����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> axis_aligned_box<T> intersection (const axis_aligned_box<T>&, const axis_aligned_box<T>&);

#include <bv/detail/axis_aligned_box.inl>

}

#endif
