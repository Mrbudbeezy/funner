#include "shared.h"

using namespace render;

namespace
{

typedef RenderTargetImpl::Pointer RenderTargetPtr;

/*
    ���������� ������� ����������
*/

class RenderQueryImpl: public IRenderQuery, public xtl::reference_counter
{
  public:
///�����������
    RenderQueryImpl (const RenderTargetPtr& in_render_target) : render_target (in_render_target) {}

///����������
    void Update () { render_target->Update (); }

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    RenderTargetPtr render_target; //���� ����������
};

/*
    ���������� �������
*/

class QueryHandlerEntry: public xtl::reference_counter
{
  public:
    typedef QueryManager::QueryFunction QueryFunction;
  
///�����������
    QueryHandlerEntry (const char* in_query_string_mask, const QueryFunction& in_handler)
      : query_string_mask (in_query_string_mask),
        handler (in_handler)
    {
    }
    
///����� ������ �������
    const char* QueryStringMask () { return query_string_mask.c_str (); }

///�������� ����������� ���������
    bool IsMatch (const char* query_string) { return common::wcmatch (query_string, query_string_mask.c_str ()); }
    
///�������� ������� �������
    IRenderQuery* CreateQuery (const RenderTargetPtr& render_target, const char* query_string)
    {
      ConstructableRenderTarget render_target_wrapper (*render_target);

      handler (render_target_wrapper, query_string);

      return new RenderQueryImpl (render_target);
    }

  private:
    stl::string   query_string_mask; //����� ������ �������
    QueryFunction handler;           //���������� �������
};

}

/*
    �������� ���������� ��������� ��������
*/

typedef xtl::intrusive_ptr<QueryHandlerEntry> EntryPtr;
typedef stl::list<EntryPtr>                   QueryHandlerList;

struct QueryManager::Impl
{
  QueryHandlerList handlers; //����������� ��������
};

/*
    ����������� / ����������
*/

QueryManager::QueryManager ()
  : impl (new Impl)
{
}

QueryManager::~QueryManager ()
{
}

/*
    ����������� ������� ��������� �������� ���������� (�������� ���������)
*/

void QueryManager::RegisterQueryHandler (const char* query_string_mask, const QueryFunction& handler)
{
  static const char* METHOD_NAME = "render::QueryManager::RegisterQueryHandler";  

  if (!query_string_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "query_string_mask");

  impl->handlers.push_front (EntryPtr (new QueryHandlerEntry (query_string_mask, handler), false));
}

void QueryManager::UnregisterQueryHandler (const char* query_string_mask)
{
  if (!query_string_mask)
    return;
    
  for (QueryHandlerList::iterator iter=impl->handlers.begin (), end=impl->handlers.end (); iter!=end;)
    if (xtl::xstrcmp (query_string_mask, (*iter)->QueryStringMask ()))
    {
      QueryHandlerList::iterator next = iter;

      ++next;

      impl->handlers.erase (iter);

      iter = next;
    }
    else ++iter;
}

void QueryManager::UnregisterAllQueryHandlers ()
{
  impl->handlers.clear ();
}

/*
   �������� �������
*/

IRenderQuery* QueryManager::CreateQuery
 (mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment,
  const char*               query_string,
  RenderManager&            render_manager)
{
  try
  {
    if (!query_string)
      throw xtl::make_null_argument_exception ("", "query_string");
      
      //����� ����������� �����������
      
    for (QueryHandlerList::iterator iter=impl->handlers.begin (), end=impl->handlers.end (); iter!=end; ++iter)
    {
      if (!(*iter)->IsMatch (query_string))
        continue;

        //�������� �������

      return (*iter)->CreateQuery (RenderTargetImpl::Create (render_manager, color_attachment, depth_stencil_attachment), query_string);
    }

      //�������� ������� �������

    return new RenderQueryImpl (RenderTargetImpl::Create (render_manager, color_attachment, depth_stencil_attachment));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::QueryManager::CreateQuery");
    throw;
  }
}
