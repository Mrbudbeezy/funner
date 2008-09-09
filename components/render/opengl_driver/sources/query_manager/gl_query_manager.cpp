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
    void SetPredication (AsyncPredicate* in_predicate, bool in_predicate_value)
    {
      predicate       = in_predicate;
      predicate_value = in_predicate_value;
    }

///��������� ��������� ���������
    AsyncPredicate* GetPredicate () { return predicate.get (); }

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
    typedef xtl::trackable_ptr<AsyncPredicate>    AsyncPredicatePtr;

  private:
    QueryManagerStatePtr main_state;       //�������� ��������� ������
    AsyncPredicatePtr    predicate;        //��������
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
      static const char* METHOD_NAME = "render::low_level::opengl::QueryManager::Impl::CreatePredicate";

        //�������� ������� ����������� ����������

      if (!GetCaps ().has_arb_occlusion_query)
        return new NullPredicate (GetContextManager ());
        
        //��������� �������� ���������

      MakeContextCurrent ();        

        //��������� ���������� ����� � ���������� �������

      int query_counter_bits = 0;

      glGetQueryiv (GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &query_counter_bits);     

      CheckErrors (METHOD_NAME);
      
        //� ������ ������������� ��������� ����������� ���������� - ������ ��������

      if (!query_counter_bits)
        return new NullPredicate (GetContextManager ());
        
        //�������� ���������

      return new AsyncPredicate (GetContextManager ());      
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
  return new QueryManagerState (static_cast<QueryManagerState*> (&*impl));
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
  AsyncPredicate* casted_predicate = cast_object<AsyncPredicate> (impl->GetContextManager (), predicate, "render::low_level::opengl::QueryManager::SetPredication", "predicate");

  impl->SetPredication (casted_predicate, predicate_value);
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
