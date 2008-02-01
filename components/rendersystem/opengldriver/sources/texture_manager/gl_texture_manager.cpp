#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

struct TextureManagerExtensions
{
  bool has_ext_texture_rectangle;        //GL_EXT_texture_rectangle
  bool has_ext_texture3D;                //GL_EXT_texture3D
  bool has_ext_texture3D_extension;      //GL_EXT_texture3D
  bool has_ext_texture_compression_s3tc; //GL_EXT_texture_compression_s3tc
  bool has_ext_packed_depth_stencil;     //GL_EXT_packed_depth_stencil
  bool has_arb_texture_compression;      //GL_ARB_texture_compression
  bool has_arb_texture_cube_map;         //GL_ARB_texture_cubemap
  bool has_arb_texture_non_power_of_two; //GL_ARB_texture_non_power_of_two
  bool has_arb_depth_texture;            //GL_ARB_depth_texture
  
  TextureManagerExtensions (const ContextManager& manager)
  {
    static Extension EXT_texture_rectangle        = "GL_EXT_texture_rectangle",
                     EXT_texture3D                = "GL_EXT_texture3D",
                     EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc",
                     EXT_texture_cube_map         = "GL_EXT_texture_cube_map",
                     EXT_packed_depth_stencil     = "GL_EXT_packed_depth_stencil",
                     NV_texture_rectangle         = "GL_NV_texture_rectangle",
                     ARB_texture_cube_map         = "GL_ARB_texture_cube_map",
                     ARB_texture_compression      = "GL_ARB_texture_compression",
                     ARB_texture_non_power_of_two = "GL_ARB_texture_non_power_of_two",
                     ARB_depth_texture            = "GL_ARB_depth_texture",
                     Version_1_2                  = "GL_VERSION_1_2",
                     Version_1_3                  = "GL_VERSION_1_3",
                     Version_1_4                  = "GL_VERSION_1_4",
                     Version_2_0                  = "GL_VERSION_2_0";
      
    has_arb_texture_compression      = manager.IsSupported (ARB_texture_compression);
    has_ext_texture_rectangle        = manager.IsSupported (EXT_texture_rectangle) || manager.IsSupported (NV_texture_rectangle);
    has_ext_texture3D_extension      = manager.IsSupported (EXT_texture3D);
    has_ext_texture3D                = has_ext_texture3D_extension || manager.IsSupported (Version_1_2);
    has_ext_texture_compression_s3tc = (has_arb_texture_compression || manager.IsSupported (Version_1_3)) && 
                                       manager.IsSupported (EXT_texture_compression_s3tc);
    has_ext_packed_depth_stencil     = manager.IsSupported (EXT_packed_depth_stencil);  
    has_arb_texture_cube_map         = manager.IsSupported (ARB_texture_cube_map) || manager.IsSupported (EXT_texture_cube_map) ||
                                       manager.IsSupported (Version_1_3);
    has_arb_texture_non_power_of_two = manager.IsSupported (ARB_texture_non_power_of_two) || manager.IsSupported (Version_2_0);
    has_arb_depth_texture            = manager.IsSupported (ARB_depth_texture) || manager.IsSupported (Version_1_4);
  }
};

}

/*
    �������� ���������� TextureManager
*/

struct TextureManager::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*      CreateTexture (const TextureDesc&, TextureManager& texture_manager);    
    ISamplerState* CreateSamplerState (const SamplerDesc&);

  public:
    size_t max_texture_size;            //������������ ������ �������� ��� ����������
    size_t max_rectangle_texture_size;  //������������ ������ �������� �� ��������� �� ������� 2 ��� ����������
    size_t max_cube_map_texture_size;   //������������ ������ cubemap ��������
    size_t max_3d_texture_size;         //������������ ������ 3d ��������
};


/*
   �����������
*/

TextureManager::Impl::Impl (const ContextManager& context_manager) 
  : ContextObject (context_manager),
    max_rectangle_texture_size (0),
    max_cube_map_texture_size (0),
    max_3d_texture_size (0)
{
    //��������� �������� ���������

  MakeContextCurrent ();
  
    //����������� �������������� ����������

  TextureManagerExtensions ext (context_manager);
  
    //������ ������������ �������� ��������

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (ext.has_ext_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (ext.has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (ext.has_ext_texture3D)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);

    //�������� ������
  
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   �������� �������� � ��������
*/

namespace
{

bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}

}
  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& tex_desc, TextureManager& texture_manager)
{
    //�������� ����� ��� ������ ������� �������� ����������� �������� �� ���������!!!!

  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture";
  
    //��������� �������� ���������

  MakeContextCurrent ();

  TextureManagerExtensions ext (GetContextManager ());    
    
  TextureDesc temp_desc = tex_desc; ////////????????

  switch (tex_desc.dimension)
  {
    case TextureDimension_1D: 
    {
      temp_desc.height = 1;
      temp_desc.layers = 1;
            
      bool is_pot = is_power_of_two (tex_desc.width);

      if (temp_desc.width > max_texture_size)
        RaiseNotSupported (METHOD_NAME, "Can't create 1d texture with width = %u (maximum texture size = %u)", tex_desc.width, max_texture_size);      

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new Texture1D (GetContextManager (), temp_desc);

      if (ext.has_ext_texture_rectangle) ///compressed?????
      {
        if (temp_desc.width > max_rectangle_texture_size)
          RaiseNotSupported (METHOD_NAME, "Can't create 1d texture with width = %u (maximum texture size = %u)", tex_desc.width, max_rectangle_texture_size);

        return new TextureNPOT (GetContextManager (), temp_desc);
      }

      return new ScaledTexture (texture_manager, temp_desc);
    }
    case TextureDimension_2D:
    {
      temp_desc.layers = 1;

      if ((temp_desc.width > max_texture_size) || (temp_desc.height > max_texture_size))
        RaiseNotSupported (METHOD_NAME, "Can't create 2d texture with width = %u height = %u (maximum texture size = %u)", tex_desc.width, tex_desc.height, max_texture_size);
      
      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height);

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new Texture2D (GetContextManager (), temp_desc);      

      if (ext.has_ext_texture_rectangle && !is_compressed_format (tex_desc.format))
      {
        if ((temp_desc.width > max_texture_size) || (temp_desc.height > max_rectangle_texture_size))
          RaiseNotSupported (METHOD_NAME, "Can't create 2d texture with width = %u height = %u (maximum texture size = %u)", tex_desc.width, tex_desc.height, max_rectangle_texture_size);
      
        return new TextureNPOT (GetContextManager (), temp_desc);
      }

      return new ScaledTexture (texture_manager, temp_desc);
    }
    case TextureDimension_3D: 
    {
        //�������� ������� ����������, ����������� ��� �������� ��������� ��������

      if (!ext.has_ext_texture3D) //��������� � Texture3D!!!
        RaiseNotSupported (METHOD_NAME, "3D textures not supported (GL_EXT_texture3D not supported)");
        
      if (tex_desc.width > max_3d_texture_size || tex_desc.height > max_3d_texture_size || tex_desc.layers > max_3d_texture_size)
      {
          //�������� ��������: ���� layers = 2^n, width , height - not
        
        RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u (max_edge_size=%u)", tex_desc.width, tex_desc.height,
                           tex_desc.layers, max_3d_texture_size);
      }

      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height) && is_power_of_two (tex_desc.layers);

      if (!is_pot && !ext.has_arb_texture_non_power_of_two)
      {
        RaiseNotSupported (METHOD_NAME, "Can't create texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                           tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));
      }

      return new Texture3D (GetContextManager (), tex_desc);
    }
    case TextureDimension_Cubemap:
    {
      if (!ext.has_arb_texture_cube_map) //��������� � TextureCubemap!!!
        RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");
        
      if (tex_desc.width > max_cube_map_texture_size || tex_desc.height > max_cube_map_texture_size)
      {
        RaiseNotSupported (METHOD_NAME, "Can't create cubemap texture %ux%u (max_edge_size=%u)", tex_desc.width, tex_desc.height,
                           max_cube_map_texture_size);
      }

      bool is_pot = is_power_of_two (tex_desc.width) && is_power_of_two (tex_desc.height);

      if (is_pot || ext.has_arb_texture_non_power_of_two)
        return new TextureCubemap (GetContextManager (), temp_desc);

      return new ScaledTexture (texture_manager, temp_desc);
    }
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.dimension", tex_desc.dimension);
      return 0;
  }
}

ISamplerState* TextureManager::Impl::CreateSamplerState (const SamplerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::Impl::CreateSamplerState");
  return 0;
}


/*
   ����������� / ����������
*/

TextureManager::TextureManager (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
  {}

TextureManager::~TextureManager ()
{
}
    
/*
   �������� �������� � ��������
*/

ITexture* TextureManager::CreateTexture (const TextureDesc& tex_desc)
{
  return impl->CreateTexture (tex_desc, *this);
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc& sampler_desc)
{
  return impl->CreateSamplerState (sampler_desc);
}

/*
   ��������� ������� �������� � ��������
*/

void TextureManager::SetTexture (size_t sampler_slot, ITexture* texture)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetTexture");
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::SetSampler");
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetTexture");
  return 0;
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  RaiseNotImplemented ("render::low_level::opengl::TextureManager::GetSampler");
  return 0;
}
