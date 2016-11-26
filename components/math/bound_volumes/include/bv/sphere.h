#ifndef BOUND_VOLUMES_SPHERE_HEADER
#define BOUND_VOLUMES_SPHERE_HEADER

#include <cmath>

#include <math/matrix.h>
#include <math/quat.h>

namespace bound_volumes
{

//forward declarations
template <class T> class axis_aligned_box;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Ограничивающая сфера
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class sphere
{
  public:
    typedef T                  element_type;
    typedef math::vector<T, 3> vec_type;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы
///////////////////////////////////////////////////////////////////////////////////////////////////
    sphere ();
    sphere (const vec_type& center, const element_type& radius);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
    const vec_type& center () const;
    element_type    radius () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка параметров
///////////////////////////////////////////////////////////////////////////////////////////////////
    void set_radius  (const element_type&);
    void set_center  (const vec_type&);
    void set_center  (const element_type& x, const element_type& y, const element_type& z);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сброс объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
    void reset (const vec_type& center = vec_type (0), const element_type& radius = element_type (-1));

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка на пустоту
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool empty () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление примитивов в ограничивающий объём
///  Если текущий радиус < 0 - сбрасываем положение сферы
///////////////////////////////////////////////////////////////////////////////////////////////////
    sphere& operator += (const vec_type&);
    sphere& operator += (const sphere&);
    sphere& operator += (const axis_aligned_box<T>&);
    sphere  operator +  (const vec_type&) const;
    sphere  operator +  (const sphere&) const;
    sphere  operator +  (const axis_aligned_box<T>&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования ограничивающей сферы
///////////////////////////////////////////////////////////////////////////////////////////////////
    sphere& operator *= (const math::quat<T>&);
    sphere  operator *  (const math::quat<T>&) const;

    axis_aligned_box<T> operator * (const math::matrix<T, 4>&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сравнение
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const sphere&) const;
    bool operator != (const sphere&) const;

  private:
    vec_type     sphere_center;
    element_type sphere_radius;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определения типов
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef sphere<int>    spherei;
typedef sphere<float>  spheref;
typedef sphere<double> sphered;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объёма
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> T volume (const sphere<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Преобразования ограничивающей сферы
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> axis_aligned_box<T> operator * (const math::matrix<T, 4>&, const sphere<T>&);
template <class T> sphere<T>           operator * (const math::quat<T>&, const sphere<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка пересечения ограничивающей сферы с различными примитивами
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> bool intersects (const sphere<T>&, const sphere<T>&);
template <class T> bool intersects (const sphere<T>&, const axis_aligned_box<T>&);
template <class T> bool intersects (const sphere<T>&, const plane_list<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка: содержит ли ограничивающая сфера различные примитивы
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> bool contains (const sphere<T>&, const math::vector<T, 3>&);
template <class T> bool contains (const sphere<T>&, const sphere<T>&);
template <class T> bool contains (const sphere<T>&, const axis_aligned_box<T>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Проверка эквивалентности
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> bool equal (const sphere<T>&, const sphere<T>&, const T& eps);

#include <bv/detail/sphere.inl>

}

#endif
