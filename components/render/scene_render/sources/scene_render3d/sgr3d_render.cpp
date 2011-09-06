#include "shared.h"

using namespace render;
using namespace render::scene_render3d;

namespace
{

///�������� ��������
class RenderManagerRegistry
{
  public:
///����������� �������
    void RegisterRender (const RenderManager& manager, Render& render)
    {
      renders.insert_pair (&manager, &render);
    }

///������ ����������� �������
    void UnregisterRender (const RenderManager& manager)
    {
      renders.erase (&manager);
    }
    
///��������� �������
    RenderPtr GetRender (const RenderManager& manager)
    {
      RenderMap::iterator iter = renders.find (&manager);

      if (iter != renders.end ())
        return iter->second;

      return RenderPtr (new Render (manager), false);
    }

  private:
    typedef stl::hash_map<const RenderManager*, Render*> RenderMap;
    
  private:
    RenderMap renders; //����� ������������ ���������� ���������� �������� �����
};

typedef common::Singleton<RenderManagerRegistry> RenderManagerSingleton;

}

/*
    �������� ���������� �������
*/

struct Render::Impl
{
  RenderManager manager; //�������� ����������
  Log           log;     //����� ����������������  
  
///�����������
  Impl (const RenderManager& in_manager)
    : manager (in_manager)
  {
    log.Printf ("Render created");
  }
  
///����������
  ~Impl ()
  {
    log.Printf ("Render destroyed");    
  }
};

/*
    ����������� / ����������
*/

Render::Render (const RenderManager& manager)
{
  try
  {
    impl = new Impl (manager);
    
    RenderManagerSingleton::Instance ()->RegisterRender (manager, *this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::Render");
    throw;
  }
}

Render::~Render ()
{
  try
  {
    RenderManagerSingleton::Instance ()->UnregisterRender (impl->manager);
  }
  catch (...)
  {
    //���������� ���� ����������
  }
}
    
/*
    ��������� �������
*/

RenderPtr Render::GetRender (const RenderManager& manager)
{
  try
  {
    return RenderManagerSingleton::Instance ()->GetRender (manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene_render3d::Render::GetRender");
    throw;
  }
}
