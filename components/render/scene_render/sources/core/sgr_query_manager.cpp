#include "scene_render_shared.h"

using namespace render;

namespace
{

/*
    ���������� ������� ����������
*/

class RenderQueryImpl: public IRenderQuery, public xtl::reference_counter
{
  public:
///�����������
    RenderQueryImpl (RenderTarget& in_render_target) : render_target (in_render_target) {}

///����������
    void Update () { render_target.Update (); }

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  private:
    RenderTarget render_target; //���� ����������
};

}

/*
    �������� ���������� ����������� �������
*/

class QueryManager::QueryHandlerEntry: public xtl::reference_counter
{
  public:
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
    IRenderQuery* CreateQuery (RenderTarget& render_target, const char* query_string)
    {
      handler (render_target, query_string);

      return new RenderQueryImpl (render_target);
    }

  private:
    stl::string   query_string_mask; //����� ������ �������
    QueryFunction handler;           //���������� �������
};

/*
    ����������� / ����������
*/

QueryManager::QueryManager ()
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

  handlers.push_front (EntryPtr (new QueryHandlerEntry (query_string_mask, handler), false));
}

void QueryManager::UnregisterQueryHandler (const char* query_string_mask)
{
  if (!query_string_mask)
    return;
    
  for (QueryHandlerList::iterator iter=handlers.begin (), end=handlers.end (); iter!=end;)
    if (xtl::xstrcmp (query_string_mask, (*iter)->QueryStringMask ()))
    {
      QueryHandlerList::iterator next = iter;

      ++next;

      handlers.erase (iter);

      iter = next;
    }
    else ++iter;
}

void QueryManager::UnregisterAllQueryHandlers ()
{
  handlers.clear ();
}

/*
   �������� �������
*/

IRenderQuery* QueryManager::CreateQuery
 (mid_level::IRenderTarget* color_attachment,
  mid_level::IRenderTarget* depth_stencil_attachment,
  const char*               query_string,
  RenderTargetManager&      render_target_manager)
{
  try
  {
    if (!query_string)
      throw xtl::make_null_argument_exception ("", "query_string");
      
      //����� ����������� �����������
      
    for (QueryHandlerList::iterator iter=handlers.begin (), end=handlers.end (); iter!=end; ++iter)
    {
      if (!(*iter)->IsMatch (query_string))
        continue;

        //�������� ���� ����������

      RenderTarget render_target = render_target_manager.CreateRenderTarget (color_attachment, depth_stencil_attachment);

        //�������� �������

      return (*iter)->CreateQuery (render_target, query_string);
    }

      //�������� ������� �������

    RenderTarget default_render_target = render_target_manager.CreateRenderTarget (color_attachment, depth_stencil_attachment);

    return new RenderQueryImpl (default_render_target);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::QueryManager::CreateQuery");
    throw;
  }
}
