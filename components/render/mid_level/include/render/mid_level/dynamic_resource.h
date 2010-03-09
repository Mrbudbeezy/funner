#ifndef RENDER_MID_LEVEL_DYNAMIC_RESOURCE_HEADER
#define RENDER_MID_LEVEL_DYNAMIC_RESOURCE_HEADER

#include <xtl/functional_fwd>

#include <render/mid_level/frame.h>

namespace render
{

namespace mid_level
{

//forward declaration
class RenderManager;
class Texture;
class Primitive;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Resource> class IDynamicResource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Resource GetResource (Frame& frame, Entity& entity) = 0;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IDynamicResource () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class Resource> class DynamicResourceManager
{
  public:
    typedef IDynamicResource<Resource>                                                 DynamicResource;
    typedef xtl::function<DynamicResource* (RenderManager& manager, const char* name)> DynamicResourceCreator;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Register      (const char* name, const DynamicResourceCreator& creator);
    static void Unregister    (const char* name);
    static void UnregisterAll ();
};

//implementation in library
typedef IDynamicResource<Texture>         DynamicTexture;
typedef IDynamicResource<Primitive>       DynamicPrimitive;
typedef DynamicResourceManager<Texture>   DynamicTextureManager;
typedef DynamicResourceManager<Primitive> DynamicPrimitiveManager;

}

}

#endif
