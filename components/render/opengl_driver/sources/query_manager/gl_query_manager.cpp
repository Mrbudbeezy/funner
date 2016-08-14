#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    ��������� ��������� ��������
*/

class QueryManagerState: public IStageState
{
  public:
///�����������
    QueryManagerState (QueryManagerState* in_main_state = 0) : main_state (in_main_state) {}

///��������� ��������� ���������
    void SetPredication (IOpenGlPredicate* in_predicate, bool in_predicate_value)
    {
      predicate       = in_predicate;
      predicate_value = in_predicate_value;
    }

///��������� ��������� ���������
    IOpenGlPredicate* GetPredicate () { return predicate.get (); }

///��������� ������������� �������� ��������� ���������
    bool GetPredicateValue () { return predicate_value; }

///��������� ���������� ���������
    bool GetPredicateAsyncResult ()
    {
      if (!(predicate && predicate->IsResultAvailable ()))
        return true;

      return predicate->GetResult () == predicate_value;
    }

///������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }

///�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:
///����������� ���������
    void Copy (QueryManagerState& source, const StateBlockMask& mask)
    {
      if (mask.predication)
      {
        predicate       = source.GetPredicate ();
        predicate_value = source.GetPredicateValue ();
      }
    }

  private:
    typedef xtl::trackable_ptr<QueryManagerState> QueryManagerStatePtr;
    typedef xtl::trackable_ptr<IOpenGlPredicate>  PredicatePtr;

  private:
    QueryManagerStatePtr main_state;       //�������� ��������� ������
    PredicatePtr         predicate;        //��������
    bool                 predicate_value;  //������������ ��������
};

}

/*
    �������� ���������� ��������� ��������
*/

struct QueryManager::Impl: public ContextObject, public QueryManagerState
{
  public:
///�����������
    Impl (const ContextManager& manager) : ContextObject (manager)
    {
      SetContextCacheValue (CacheEntry_IsInQueryRanges, 0);
    }

///�������� ����������
    IPredicate* CreatePredicate ()
    {
        //�������� ������� ����������� ����������

      if (!GetCaps ().has_arb_occlusion_query)
        return new NullPredicate;

        //��������� �������� ���������

      MakeContextCurrent ();
      
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

        //��������� ���������� ����� � ���������� �������

      GLint query_counter_bits = 0;

      glGetQueryiv (GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_counter_bits);

      CheckErrors ("render::low_level::opengl::QueryManager::Impl::CreatePredicate");

        //� ������ ������������� ��������� ����������� ���������� - ������ ��������

      if (!query_counter_bits)
        return new NullPredicate;

        //�������� ���������

      return new AsyncPredicate (GetContextManager ());
#else
      return new NullPredicate;
#endif
    }
};

/*
   ����������� / ����������
*/

QueryManager::QueryManager (const ContextManager& manager)
  : impl (new Impl (manager))
  {}

QueryManager::~QueryManager ()
{
}

/*
    �������� ������� ��������� ������
*/

IStageState* QueryManager::CreateStageState ()
{
  return new QueryManagerState (static_cast<QueryManagerState*> (impl.get ()));
}

/*
    �������� ����������
*/

IPredicate* QueryManager::CreatePredicate ()
{
  return impl->CreatePredicate ();
}

/*
    ���������� ����������� ���������
*/

void QueryManager::SetPredication (IPredicate* predicate, bool predicate_value)
{
  static const char* METHOD_NAME = "render::low_level::opengl::QueryManager::SetPredication";

  ContextObject* casted_object = dynamic_cast<ContextObject*> (predicate);

  if (casted_object && !casted_object->IsCompatible (impl->GetContextManager ()))
    throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Argument 'predicate' is incompatible with target IDevice");

  IOpenGlPredicate* gl_predicate = dynamic_cast<IOpenGlPredicate*> (predicate);

  impl->SetPredication (gl_predicate, predicate_value);
}

IPredicate* QueryManager::GetPredicate ()
{
  return impl->GetPredicate ();
}

bool QueryManager::GetPredicateValue ()
{
  return impl->GetPredicateValue ();
}

/*
    ��������� ���������� ���������
*/

bool QueryManager::GetPredicateAsyncResult ()
{
  return impl->GetPredicateAsyncResult ();
}
