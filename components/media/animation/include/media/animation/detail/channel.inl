namespace detail
{

///������� ����� ���������� ������� ������������� ������
class IEvaluatorBase : public xtl::reference_counter
{
  public:
    virtual const std::type_info& TrackType () = 0; //��� �����
    virtual const std::type_info& ValueType () = 0; //��� �������� ����������
    virtual float                 MinUnwrappedTime () = 0; //����������� ������������ �����
    virtual float                 MaxUnwrappedTime () = 0; //������������ ������������ �����
    virtual float                 MinTime () = 0; //����������� �����
    virtual float                 MaxTime () = 0; //������������ �����
    
    virtual ~IEvaluatorBase () {}
};

///��������� ��������� ������� ������������� ������
template <class T> class IEvaluator : public IEvaluatorBase
{
  public:
    virtual void Eval (float time, T& value) = 0;
    
    const std::type_info& ValueType ()
    {
      return typeid (T);
    }
};

template <class Key>
typename math::basic_spline<Key>::time_type get_min_unwrapped_time (const math::basic_spline<Key>& spline)
{
  if (spline.is_begin_clamped ())
    return spline.min_time ();

  return FLT_MIN;
}

template <class Key>
typename math::basic_spline<Key>::time_type get_max_unwrapped_time (const math::basic_spline<Key>& spline)
{
  if (spline.is_end_clamped ())
    return spline.max_time ();

  return FLT_MAX;
}

template <class Key>
typename math::basic_spline<Key>::time_type get_min_time (const math::basic_spline<Key>& spline)
{
  return spline.min_time ();
}

template <class Key>
typename math::basic_spline<Key>::time_type get_max_time (const math::basic_spline<Key>& spline)
{
  return spline.max_time ();
}

namespace adl_defaults
{

inline float get_min_unwrapped_time (xtl::default_cast_type)
{
  return FLT_MIN;
}

inline float get_max_unwrapped_time (xtl::default_cast_type)
{
  return FLT_MAX;
}

inline float get_min_time (xtl::default_cast_type)
{
  return FLT_MIN;
}

inline float get_max_time (xtl::default_cast_type)
{
  return FLT_MAX;
}

}

///���������� ������� ������������� ������
template <class Fn> class TrackImpl : public IEvaluator<typename TrackResultType<Fn>::Type> 
{
  public:
    typedef typename TrackResultType<Fn>::Type ValueType;
    
    TrackImpl (Fn in_fn)
      : fn (in_fn)
    {
    }

    void Eval (float time, ValueType& value)
    {
      fn (time, value);
    }
    
    Fn& Function ()
    {
      return fn;
    }

    const std::type_info& TrackType ()
    {
      return typeid (Fn);
    }
    
    float MinUnwrappedTime ()
    {
      using namespace adl_defaults;
      
      return get_min_unwrapped_time (fn);
    }
    
    float MaxUnwrappedTime ()
    {
      using namespace adl_defaults;
      
      return get_max_unwrapped_time (fn);
    }
    
    float MinTime ()
    {
      using namespace adl_defaults;
      
      return get_min_time (fn);
    }
    
    float MaxTime ()
    {
      using namespace adl_defaults;
      
      return get_max_time (fn);
    }    

  private:
    Fn fn;
};

}

/*
    class Evaluator<T>
*/

/*
   ������������ / ���������� / ������������
*/

template <class T>
Evaluator<T>::Evaluator (const detail::IEvaluator<T>& evaluator)
  : impl (const_cast<detail::IEvaluator<T>*> (&evaluator))
{
  addref (impl);
}

template <class T>
Evaluator<T>::Evaluator (const Evaluator& source)
  : impl (source.impl)
{
  addref (impl);
}

template <class T>
Evaluator<T>::~Evaluator ()
{
  release (impl);
}

template <class T>
Evaluator<T>& Evaluator<T>::operator = (const Evaluator<T>& source)
{
  Evaluator (source).Swap (*this);
  
  return *this;  
}

/*
    �����
*/

template <class T>
void Evaluator<T>::Swap (Evaluator& source)
{
  stl::swap (impl, source.impl);
}

template <class T>
void swap (Evaluator<T>& evaluator1, Evaluator<T>& evaluator2)
{
  evaluator1.Swap (evaluator2);
}

/*
   ���������� ��������
*/

namespace detail
{

template <class Ret> struct ResultValue
{
  Ret value;
  
  ResultValue () {}
};

}

template <class T>
typename Evaluator<T>::ValueType Evaluator<T>::operator () (float time) const
{
  detail::ResultValue<T> result;
  
  impl->Eval (time, result.value);
  
  return result.value;
}

template <class T>
void Evaluator<T>::operator () (float time, ValueType& value)
{
  impl->Eval (time, value);
}

/*
    class Channel
*/

/*
    ���������� �������� �� �������
*/

template <class Ret> Ret Channel::Eval (float time) const
{
  return Evaluator<Ret> () (time);
}

template <class Ret> void Channel::Eval (float time, Ret& result) const
{
  Evaluator<Ret> () (time, result);
}

/*
    ������
*/

template <class Fn> void Channel::SetTrack (const Fn& fn)
{
  SetTrackCore (new detail::TrackImpl<Fn> (fn));
}

template <class Fn> const Fn* Channel::Track () const
{
  if (&typeid (Fn) != &TrackType ())
    return 0;
  
  return &static_cast<detail::TrackImpl<Fn>*> (TrackCore ())->Function ();
}

template <class Fn> Fn* Channel::Track ()
{
  return const_cast<Fn*> (const_cast<const Channel&> (*this).Track<Fn> ());
}

/*
    ��������� ������� ���������� ��������
*/

template <class T> animation::Evaluator<T> Channel::Evaluator () const
{
  if (&typeid (T) != &ValueType ())
    RaiseTypeError ("media::animation::Channel::Evaluator", typeid (T));
    
  return media::animation::Evaluator<T> (*static_cast<detail::IEvaluator<T>*> (TrackCore ()));
}
