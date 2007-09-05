///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////

template <class T> sphere<T>::sphere()
{
   sphere_center=vec_type(0,0,0);
   sphere_radius=0;
}

template <class T> sphere<T>::sphere (const vec_type& center, const element_type& radius)
{
   sphere_center=center;
   sphere_radius=radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ��ࠬ��஢
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
const typename sphere<T>::vec_type &sphere<T>::center () const
{
   return sphere_center;
}
/*template <class T>
const typename sphere<T>::vec_type& sphere<T>::center () const
{
   return center;
} */

template <class T>
typename sphere<T>::element_type sphere<T>::radius () const
{
   return sphere_radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///��⠭���� ��ࠬ��஢
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void typename sphere<T>::set_radius  (const element_type& r)
{
   sphere_radius=r;
}

template <class T>
void typename sphere<T>::set_center  (const vec_type& c)
{
   sphere_center=c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void typename sphere<T>::reset (const vec_type& center = vec_type (0), const element_type& radius = element_type (0))
{
   sphere_center=center;
   sphere_radius=radius;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����祭�� ����⨪ ��࠭�稢��饩 ����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
typename sphere<T>::element_type typename sphere<T>::volume () const //��������᪨� ���� ����
{
   return 4*M_PI*sphere_radius*sphere_radius*sphere_radius/3; //���� �� ��ᨢ�
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
bool sphere<T>::empty (const element_type& eps) const
//bool typename sphere<T>::empty (const typename sphere::element_type& eps = default_epsilon) //�஢�ઠ: r < eps
{
   return sphere_radius<eps;//true:false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �ਬ�⨢�� � ��࠭�稢��騩 ����
///  �᫨ ⥪�騩 ࠤ��� < 0 - ���뢠�� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
sphere<T>& sphere<T>::operator += (const vec_type& point)
{
   vec_type vec;
   if(sphere_radius==0)
   {
      sphere_center=vec;
      return *this;
   }
   if(contains(point))
      return *this;
   vec=(sphere_center+point)/2;
   sphere_center=vec+normalize(vec)*(sphere_radius/2);
   return *this;
}

template <class T>
sphere<T>& sphere<T>::operator += (const sphere<T>& sph)
{
   vec_type vec;
   if(sphere_radius==0)
   {
      sphere_radius=sph.radius();
      sphere_center=sph.centre();
      return *this;
   }
   if(contains(sph))
      return *this;
   vec=(sphere_center+sph.center())/2;
   sphere_center=vec+normalize(vec)*((sphere_radius+sph.radius)/2);
   return *this;
}

//sphere& template <class T> class sphere::operator += (const axis_aligned_box<T>&); //???
template <class T>
sphere<T> sphere<T>::operator +  (const vec_type& vec) const
{
   return shere(*this)+=vec;
}

template <class T>
sphere<T> sphere<T>::operator +  (const sphere<T>& sph) const
{
   return shere(*this)+=sph;
}
//sphere  template <class T> class sphere::operator +  (const axis_aligned_box&) const; //???

///////////////////////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ ����祭�� ��࠭�稢��饩 ���� � ࠧ���묨 �ਬ�⨢���
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
bool sphere<T>::intersects (const sphere<T>& sphere) const
{
   return length(sphere_center-sphere.center())<sphere_radius+sphere.radius()?true:false;
}
//bool template <class T> class sphere::intersects (const axis_aligned_box<T>&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�஢�ઠ: ᮤ�ন� �� ��࠭�稢���� ��� ࠧ���� �ਬ�⨢�
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
bool typename sphere<T>::contains (const vec_type& point) const
{
   return length(point-sphere_center)<sphere_radius?true:false;
}

template <class T>
bool typename sphere<T>::contains (const sphere& sphere) const
{
   return length(sphere_center-sphere.center())<(sphere_radius-2*sphere.radius())?true:false;   //�த� 㬭������ �� �㦭�
}
//bool template <class T> class sphere::contains (const axis_aligned_box<T>& box) const;





///////////////////////////////////////////////////////////////////////////////////////////////////
///�ࠢ�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
bool sphere<T>::equal (const sphere& sp, const element_type& eps = default_epsilon) const
{
   return ((abs(sphere_radius-sp.radius())<eps)&&(sphere_center==sp.center()))?true:false;
}

template <class T>
bool typename sphere<T>::operator == (const sphere<T>& sp) const
{
   return equal(sp);
}

template <class T>
bool typename sphere<T>::operator != (const sphere& sp) const
{
   return !equal(sp);
}
