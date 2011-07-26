#ifndef RENDER_MANAGER_MANAGER_HEADER
#define RENDER_MANAGER_MANAGER_HEADER

#include <render/dynamic_texture.h>
#include <render/entity.h>
#include <render/frame.h>
#include <render/material.h>
#include <render/texture.h>
#include <render/window.h>

namespace syslib
{

//forward declaration
class Window;

}

namespace render
{

//implementation forwards
class RenderManagerImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum LogLevel
{
  LogLevel_Info,  //�������������� ���������
  LogLevel_Debug, //���������� ���������
  
  LogLevel_Default = LogLevel_Debug
};

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
    render::Window CreateWindow (syslib::Window& window, common::PropertyMap& properties);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t               WindowsCount () const;
    const render::Window Window       (size_t index) const;
          render::Window Window       (size_t index);

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
    PrimitiveBuffers CreatePrimitiveBuffers (MeshBufferUsage lines_usage = MeshBufferUsage_Stream, MeshBufferUsage sprites_usage = MeshBufferUsage_Stream);    
    Frame            CreateFrame            ();
    Entity           CreateEntity           ();
    Texture          CreateTexture          (const char* name);
    Texture          CreateTexture          (const media::Image& image, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::Image& image, TextureDimension dimension, bool generate_mipmaps = true);
    Texture          CreateTexture          (const media::CompressedImage& image);
    Texture          CreateTexture          (const media::CompressedImage& image, TextureDimension dimension);
    Texture          CreateTexture2D        (size_t width, size_t height, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTexture3D        (size_t width, size_t height, size_t depth, PixelFormat format, bool generate_mipmaps = true);
    Texture          CreateTextureCubemap   (size_t size, PixelFormat format, bool generate_mipmaps = true);
    Material         CreateMaterial         ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������/����� ��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Texture   CreateSharedTexture   (const char* name);
    Material  CreateSharedMaterial  (const char* name);
    Primitive CreateSharedPrimitive (const char* name);
    bool      HasSharedTexture      (const char* name);
    bool      HasSharedMaterial     (const char* name);
    bool      HasSharedPrimitive    (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ������������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ShareTexture     (const char* name, const Texture& texture);
    void ShareMaterial    (const char* name, const Material& material);
    void SharePrimitive   (const char* name, const Primitive& primitive);
    void UnshareTexture   (const char* name);
    void UnshareMaterial  (const char* name);
    void UnsharePrimitive (const char* name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* resource_name);
    void LoadResource   (const media::rfx::MaterialLibrary&);
    void LoadResource   (const media::geometry::MeshLibrary&);
    void UnloadResource (const char* resource_name);
    void UnloadResource (const media::rfx::MaterialLibrary&);
    void UnloadResource (const media::geometry::MeshLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ �������
///  ����������: ������������ ��� �������� �������� �������� Entity ����� ������� �� ������ � ��
///  �����������, ��� ����� �������� � ����������� ������ � � ������ ��������. �������� ������ 
///  ������������� ��� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<IDynamicTexture* (const char* name, Entity& entity)> DynamicTextureCreator;

    void RegisterDynamicTexture       (const char* name_mask, const DynamicTextureCreator& creator);
    void UnregisterDynamicTexture     (const char* name_mask);
    void UnregisterAllDynamicTextures ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ����������� ��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetCacheTimeDelay  (size_t milliseconds);
    void   SetCacheFrameDelay (size_t frames_count);
    size_t CacheTimeDelay     () const;
    size_t CacheFrameDelay    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (RenderManager& manager, render::Window& window)> WindowEventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void             SetLogLevel (render::LogLevel level);
    render::LogLevel LogLevel    () const;

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

#endif
