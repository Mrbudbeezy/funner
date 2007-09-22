#ifndef XTL_ITERATOR_RANGE_HEADER
#define XTL_ITERATOR_RANGE_HEADER

#include <xtl/range_traits.h>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������, �������� 2-�� ����������� ������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FwdIter>
class iterator_range
{
  typedef bool (iterator_range::*unspecified_bool_type)() const;
  typedef stl::iterator_traits<FwdIter> iterator_traits;
  public:
    typedef typename iterator_traits::value_type      value_type;
    typedef typename iterator_traits::difference_type difference_type;
    typedef size_t                                    size_type;
    typedef FwdIter                                   iterator, const_iterator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
                              iterator_range ();
    template <class FwdIter2> iterator_range (FwdIter2 begin, FwdIter2 end);                
    template <class FwdRange> iterator_range (FwdRange&);
    template <class FwdRange> iterator_range (const FwdRange&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
                              iterator_range& operator = (const iterator_range&);
    template <class FwdIter1> iterator_range& operator = (const iterator_range<FwdIter1>&);
    template <class FwdRange> iterator_range& operator = (FwdRange&);
    template <class FwdRange> iterator_range& operator = (const FwdRange&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    iterator  begin () const; //������ ���������
    iterator  end   () const; //����� ���������
    size_type size  () const; //������ ���������
    bool      empty () const; //�������� �� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� � ���������� ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    value_type& front () const;
    value_type& back  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    operator unspecified_bool_type () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ (������ ��� ���������� ������������� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    value_type& operator [] (size_type index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool equal (const iterator_range&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class FwdIter2> bool operator == (const iterator_range<FwdIter2>&) const;
    template <class FwdIter2> bool operator != (const iterator_range<FwdIter2>&) const;
    template <class FwdIter2> bool operator <  (const iterator_range<FwdIter2>&) const;
    template <class FwdIter2> bool operator >  (const iterator_range<FwdIter2>&) const;
    template <class FwdIter2> bool operator <= (const iterator_range<FwdIter2>&) const;
    template <class FwdIter2> bool operator >= (const iterator_range<FwdIter2>&) const;
    template <class FwdRange> bool operator == (const FwdRange&) const;
    template <class FwdRange> bool operator != (const FwdRange&) const;
    template <class FwdRange> bool operator <  (const FwdRange&) const;
    template <class FwdRange> bool operator >  (const FwdRange&) const;
    template <class FwdRange> bool operator <= (const FwdRange&) const;
    template <class FwdRange> bool operator >= (const FwdRange&) const;

  private:
    iterator first, last;
    bool     singular;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FwdIter>
bool empty (const iterator_range<FwdIter>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FwdRange, class FwdIter> bool operator == (const FwdRange&, const iterator_range<FwdIter>&);
template <class FwdRange, class FwdIter> bool operator != (const FwdRange&, const iterator_range<FwdIter>&);
template <class FwdRange, class FwdIter> bool operator <  (const FwdRange&, const iterator_range<FwdIter>&);
template <class FwdRange, class FwdIter> bool operator >  (const FwdRange&, const iterator_range<FwdIter>&);
template <class FwdRange, class FwdIter> bool operator <= (const FwdRange&, const iterator_range<FwdIter>&);
template <class FwdRange, class FwdIter> bool operator >= (const FwdRange&, const iterator_range<FwdIter>&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class FwdIter>  iterator_range<FwdIter>                                       make_iterator_range (FwdIter begin, FwdIter end);
template <class FwdRange> iterator_range<typename range_iterator<FwdRange>::type>       make_iterator_range (FwdRange&);
template <class FwdRange> iterator_range<typename range_const_iterator<FwdRange>::type> make_iterator_range (const FwdRange&);

template <class Range> iterator_range<typename range_iterator<Range>::type>
make_iterator_range (Range&, typename range_difference<Range>::type advance_begin, typename range_difference<Range>::type advance_end);

template <class Range> iterator_range<typename range_const_iterator<Range>::type>
make_iterator_range (const Range&, typename range_difference<Range>::type advance_begin, typename range_difference<Range>::type advance_end);

template <class T> iterator_range<T*> make_iterator_range (size_t count, T* array);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Sequence, class FwdRange>
Sequence copy_range (const FwdRange&);

#include <xtl/detail/iterator_range.inl>

}

#endif
