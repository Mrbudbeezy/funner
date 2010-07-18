#ifndef MATH_CURVES_BASIC_SPLINE_HEADER
#define MATH_CURVES_BASIC_SPLINE_HEADER

#include <cstddef>

#ifndef MATH_CURVES_SPLINE_NO_DETAILS
  #include <stl/algorithm>
  #include <stl/vector>
  #include <xtl/common_exceptions.h>
  #include <xtl/reference_counter.h>
  #include <math/matrix.h>
#endif

namespace math
{

//forward declarations
template <class T, unsigned int Size> class matrix;
template <class T, unsigned int Size> class vector;
template <class T>                    class quat;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>                    struct spline_scalar_type                   { typedef T type; };
template <class T, unsigned int Size> struct spline_scalar_type<vector<T, Size> > { typedef T type; };
template <class T, unsigned int Size> struct spline_scalar_type<matrix<T, Size> > { typedef T type; };
template <class T>                    struct spline_scalar_type<quat<T> >         { typedef T type; };

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct spline_key
{
  typedef T                                    value_type;
  typedef typename spline_scalar_type<T>::type scalar_type;
  typedef scalar_type                          time_type;

  time_type  time;      //����� ������������ ��������
  value_type value;     //��������

  spline_key ();
  spline_key (const time_type& time, const value_type& value);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� ��� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct spline_step_key: public spline_key<T>
{
  private:
    typedef spline_key<T> base;

  public:
    typedef typename base::time_type  time_type;
    typedef typename base::value_type value_type;

    spline_step_key ();
    spline_step_key (const time_type& time, const value_type& value);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������� � �������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct spline_linear_key: public spline_key<T>
{
  private:
    typedef spline_key<T> base;

  public:
    typedef typename base::time_type  time_type;
    typedef typename base::value_type value_type;

    spline_linear_key ();
    spline_linear_key (const time_type& time, const value_type& value);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� TCB �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct spline_tcb_key: public spline_key<T>
{
  private:
    typedef spline_key<T> base;

  public:
    typedef typename base::time_type   time_type;
    typedef typename base::value_type  value_type;
    typedef typename base::scalar_type scalar_type;

    scalar_type tension;    //������������� ������
    scalar_type continuity; //��������� �������������
    scalar_type bias;       //���������������� ������

    spline_tcb_key ();
    spline_tcb_key (const time_type& time, const value_type& value);
    spline_tcb_key (const time_type& time, const value_type& value, const scalar_type& tension, const scalar_type& continuity, const scalar_type& bias);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> struct spline_bezier_key: public spline_key<T>
{
  private:
    typedef spline_key<T> base;

  public:
    typedef typename base::time_type   time_type;
    typedef typename base::value_type  value_type;
    typedef typename base::scalar_type scalar_type;

    value_type inner_value; //�������� ��������
    value_type outer_value; //��������� ��������
  
    spline_bezier_key ();
    spline_bezier_key (const time_type& time, const value_type& value);
    spline_bezier_key (const time_type& time, const value_type& value, const value_type& inner_value, const value_type& outer_value);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum spline_wrap
{
  spline_wrap_clamp,  //���������
  spline_wrap_repeat, //����������
  spline_wrap_mirror, //���������� � ������������ ����������

  spline_wrap_default = spline_wrap_clamp
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key>
class basic_spline
{
  public:
    typedef Key                            key_type;
    typedef typename key_type::value_type  value_type, result_type;
    typedef typename key_type::scalar_type scalar_type;
    typedef typename key_type::time_type   time_type;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    basic_spline  ();
    basic_spline  (const basic_spline&);
    ~basic_spline ();

    basic_spline& operator = (const basic_spline&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    basic_spline clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        wrap       (spline_wrap begin_wrap, spline_wrap end_wrap);
    spline_wrap begin_wrap () const;
    spline_wrap end_wrap   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ / �������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t keys_count () const;
    bool   empty      () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   reserve  (size_t keys_count);
    size_t capacity () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool               closed         () const;
    void               set_closed_eps (const scalar_type& eps);
    const scalar_type& closed_eps     () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const time_type& min_time () const;
    const time_type& max_time () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� / ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const key_type& get_key (size_t index) const;
    void            set_key (size_t index, const key_type& key);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t add_keys (size_t keys_count, const key_type* keys);
    size_t add_key  (const key_type& key);
    size_t add_key  (const time_type& time, const value_type& value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void remove_key (size_t key_index);
    void clear      ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ ��� ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void sort ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� � ������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void       eval (const time_type& time, value_type& out_value) const;
    value_type eval (const time_type& time) const;

    value_type operator () (const time_type& time) const;
    void       operator () (const time_type& time, value_type& value) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void swap (basic_spline&);
    
  private:
    struct implementation;  
  
    basic_spline (implementation*);

  private:
    implementation* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Key>
void swap (basic_spline<Key>&, basic_spline<Key>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef basic_spline<spline_tcb_key<float> >                tcb_splinef;
typedef basic_spline<spline_tcb_key<vector<float, 2> > >    tcb_spline2f;
typedef basic_spline<spline_tcb_key<vector<float, 3> > >    tcb_spline3f;
typedef basic_spline<spline_tcb_key<vector<float, 4> > >    tcb_spline4f;
typedef basic_spline<spline_tcb_key<quat<float> > >         tcb_rotation_splinef;
typedef basic_spline<spline_bezier_key<float> >             bezier_splinef;
typedef basic_spline<spline_bezier_key<vector<float, 2> > > bezier_spline2f;
typedef basic_spline<spline_bezier_key<vector<float, 3> > > bezier_spline3f;
typedef basic_spline<spline_bezier_key<vector<float, 4> > > bezier_spline4f;
typedef basic_spline<spline_step_key<float> >               step_splinef;
typedef basic_spline<spline_step_key<vector<float, 2> > >   step_spline2f;
typedef basic_spline<spline_step_key<vector<float, 3> > >   step_spline3f;
typedef basic_spline<spline_step_key<vector<float, 4> > >   step_spline4f;
typedef basic_spline<spline_step_key<matrix<float, 4> > >   step_spline_mat4f;
typedef basic_spline<spline_linear_key<float> >             linear_splinef;
typedef basic_spline<spline_linear_key<vector<float, 2> > > linear_spline2f;
typedef basic_spline<spline_linear_key<vector<float, 3> > > linear_spline3f;
typedef basic_spline<spline_linear_key<vector<float, 4> > > linear_spline4f;
typedef basic_spline<spline_linear_key<matrix<float, 4> > > linear_spline_mat4f;

#ifndef MATH_CURVES_SPLINE_NO_DETAILS
  #include <math/detail/basic_spline.inl>
#endif

}

#endif
