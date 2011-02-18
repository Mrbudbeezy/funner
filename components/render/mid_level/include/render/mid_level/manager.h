#ifndef RENDER_MID_LEVEL_MANAGER_HEADER
#define RENDER_MID_LEVEL_MANAGER_HEADER

///???�������������� �������� ���������� �� ���������� ����� entity

#include <render/mid_level/entity.h>
#include <render/mid_level/dynamic_resource.h>
#include <render/mid_level/material.h>
#include <render/mid_level/resource_library.h>
#include <render/mid_level/texture.h>
#include <render/mid_level/window.h>

namespace syslib
{

//forward declaration
class Window;

}

namespace render
{

namespace mid_level
{

//implementation forwards
class RenderManagerImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum RenderManagerWindowEvent
{
  RenderManagerWindowEvent_OnAdded,   //��������� ����� ���� ����������
  RenderManagerWindowEvent_OnRemoved, //������� ���� ����������
  
  RenderManagerWindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������� ����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ResourceInstanceMode
{
  ResourceInstanceMode_Shared, //������ ������������ ���������
  ResourceInstanceMode_Copy,   //������ ���������� ��� ��������

  ResourceInstanceMode_Default = ResourceInstanceMode_Shared
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManager
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderManager  ();
    RenderManager  (const RenderManager&);
    ~RenderManager ();
    
    RenderManager& operator = (const RenderManager&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    mid_level::Window CreateWindow (syslib::Window& window, common::PropertyMap& properties);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  WindowsCount () const;
    const mid_level::Window Window       (size_t index) const;
          mid_level::Window Window       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget CreateRenderBuffer       (size_t width, size_t height, PixelFormat format);
    RenderTarget CreateDepthStencilBuffer (size_t width, size_t height);
                                                         
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Primitive        CreatePrimitive        ();
    Primitive        CreatePrimitive        (const PrimitiveBuffers&);    
    Primitive        CreatePrimitive        (const char* name, ResourceInstanceMode mode = ResourceInstanceMode_Default);
    PrimitiveBuffers CreatePrimitiveBuffers (MeshBufferUsage lines_usage = MeshBufferUsage_Stream, MeshBufferUsage sprites_usage = MeshBufferUsage_Stream);
    Frame            CreateFrame            ();
    Texture          CreateTexture          (const media::Image& image, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::Image& image, TextureDimension dimension, bool generate_mipmaps = true);
    Texture          CreateTexture2D        (size_t width, size_t height, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTexture3D        (size_t width, size_t height, size_t depth, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTextureCubemap   (size_t size, PixelFormat format, bool generate_mipmaps = true);
    Material         CreateMaterial         ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��� ����������???
    ResourceLibrary Load (const char* resource_name);
    ResourceLibrary Load (const media::rfx::MaterialLibrary&);
    ResourceLibrary Load (const media::rfx::EffectLibrary&);
    ResourceLibrary Load (const media::geometry::MeshLibrary&);
    ResourceLibrary Load (const media::rfx::ShaderLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (RenderManager& manager, mid_level::Window& window)> WindowEventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (RenderManager&);
    
  private:
    RenderManager (RenderManagerImpl*);
    
  private:
    RenderManagerImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (RenderManager&, RenderManager&);

}

}

#endif
