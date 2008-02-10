#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

//�������������� ��������� ����������� �������
enum TextureManagerContextTable
{
//  TextureManagerContextTable_TexTarget0,
//  TextureManagerContextTable_TexTargetsNum = TextureManagerContextTable_Texture0 + DEVICE_SAMPLER_SLOTS_COUNT,

  TextureManagerContextTable_Num
};

/*
    �������� ���������� TextureManager
*/

struct TextureManager::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager, TextureManager& texture_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture*       CreateTexture      (const TextureDesc&);
    ISamplerState*  CreateSamplerState (const SamplerDesc&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetTexture (size_t sampler_slot, ITexture* texture);
    void           SetSampler (size_t sampler_slot, ISamplerState* state);
    ITexture*      GetTexture (size_t sampler_slot) const;
    ISamplerState* GetSampler (size_t sampler_slot) const;

  private:
    ITexture* CreateTexture1D      (const TextureDesc&);
    ITexture* CreateTexture2D      (const TextureDesc&);
    ITexture* CreateTexture3D      (const TextureDesc&);
    ITexture* CreateTextureCubemap (const TextureDesc&);

  private:
    typedef xtl::trackable_ptr<BindableTexture> TexturePtr;
    typedef xtl::trackable_ptr<SamplerState>    SamplerStatePtr;
    
    struct Sampler
    {
      TexturePtr      texture; //��������
      SamplerStatePtr state;   //��������� ��������
    };
    
    typedef xtl::array<Sampler, DEVICE_SAMPLER_SLOTS_COUNT> SamplerArray;

  public:
    TextureManager& texture_manager;            //�������� �������
    size_t          max_texture_size;           //������������ ������ �������� ��� ����������
    size_t          max_rectangle_texture_size; //������������ ������ �������� �� ��������� �� ������� 2 ��� ����������
    size_t          max_cube_map_texture_size;  //������������ ������ cubemap ��������
    size_t          max_3d_texture_size;        //������������ ������ 3d ��������
    size_t          texture_units_count;        //���������� ������ ���������������������
    SamplerArray    samplers;                   //������ ���������
    ExtensionsPtr   extensions;                 //�������������� ����������
};


/*
   �����������
*/

TextureManager::Impl::Impl (const ContextManager& context_manager, TextureManager& in_texture_manager)
  : ContextObject (context_manager),
    texture_manager (in_texture_manager),
    max_rectangle_texture_size (0),
    max_cube_map_texture_size (0),
    max_3d_texture_size (0),
    texture_units_count (1)
{
    //��������� �������� ���������

  MakeContextCurrent ();
  
    //��������� �������������� ����������
    
  extensions = ExtensionsPtr (new TextureExtensions (context_manager));

    //������ ������������ �������� ������� / ���������� ���������� ������
    
      //������� � TextureExtensions!!!!

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (extensions->has_ext_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (extensions->has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (extensions->has_ext_texture3d)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);
  if (extensions->has_arb_multitexture)      glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);

  if (texture_units_count > DEVICE_SAMPLER_SLOTS_COUNT)
    texture_units_count = DEVICE_SAMPLER_SLOTS_COUNT;

    //�������� ������
  
  CheckErrors ("render::low_level::opengl::TextureManager::Impl::Impl");
}

/*
   ������� ������� � ���������
*/

void TextureManager::Impl::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::Bind";
  
    //����� �������� ���������

  MakeContextCurrent ();

    //��������� ������� � ���������
    
/*  for (size_t i = 0; i < texture_units_count; i++)
  {
    BindableTexture* texture       = samplers [i].texture.get ();
    SamplerState*    sampler_state = samplers [i].state.get ();

    if (!texture || !state)
    {
      if (extensions->has_arb_multitexture) //�������!!!
      {
        if (glActiveTexture) glActiveTexture    (GL_TEXTURE0 + i);
        else                 glActiveTextureARB (GL_TEXTURE0 + i);
      }

      if (binded_tex_targets [i])
      {
        glDisable (binded_tex_targets [i]);
      
        binded_tex_targets [i] = 0;
      }
      
      continue;
    }
    
      //������!!!

    BindableTextureDesc tex_desc;

    texture->GetDesc (tex_desc);    

      //��������� �������� � �������� OpenGL      

    if (GetContextData (ContextDataTable_TextureManager, TextureManagerContextTable_Texture0 + i) != texture->GetId ())
    {
      if (extensions->has_arb_multitexture) //������!!!
      {
        if (glActiveTexture) glActiveTexture    (GL_TEXTURE0 + i);
        else                 glActiveTextureARB (GL_TEXTURE0 + i);
      }      

      glBindTexture (tex_desc.target, tex_desc.id);      

      SetContextData (ContextDataTable_TextureManager, TextureManagerContextTable_Texture0 + i, texture->GetId ());

      GLenum current_target = GetContextData (ContextDataTable_TextureManager, TextureManagerContextTable_TexTarget0 + i);

      if (current_target != tex_desc.target)
      {       
        if (current_target)
          glDisable (current_target);

        glEnable  (tex_desc.target);

        SetContextData (ContextDataTable_TextureManager, TextureManagerContextTable_TexTarget0 + i, tex_desc.target);
      }
    }

      //��������� ��������� �������������

      //��������� ������ ������������� � ����� �������� ������ ���������!!!!!!!

    if (texture->GetSamplerId () != sampler_state->GetId ())
    {
        //�������!!!!. ����� ������ glBindTexture!!!
      
      texture->SetSamplerId (sampler_state->GetId ());

      sampler_state->Bind (tex_desc.target);
    }
  }*/

    //�������� ������

  CheckErrors (METHOD_NAME);
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

ITexture* TextureManager::Impl::CreateTexture1D (const TextureDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture1D";
  
    //������������� �������������� ����������
  
  TextureDesc desc = in_desc;
  
  desc.height = desc.layers = 1;
  
    //�������� ����������� �������� ��������

  if (desc.width > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_texture_size);

    //��������������� �������� �������� � ����������� �� �������������� ����������

  bool is_pot = is_power_of_two (desc.width);    

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new Texture1D (GetContextManager (), extensions, desc);

  if (extensions->has_ext_texture_rectangle)
  {
    if (desc.width > max_rectangle_texture_size)
      RaiseNotSupported (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, max_rectangle_texture_size);

    return new TextureNpot (GetContextManager (), extensions, desc);
  }

  return new ScaledTexture (GetContextManager (), texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture2D (const TextureDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture2D";
  
    //������������� �������������� ����������
  
  TextureDesc desc = in_desc;
  
  desc.layers = 1;  
  
    //�������� ����������� �������� ��������

  if (desc.width > max_texture_size || desc.height > max_texture_size)
    RaiseNotSupported (METHOD_NAME, "Can't create 2D texture %ux%u (maximum texture size is %u)", desc.width, desc.height, max_texture_size);

    //��������������� �������� �������� � ����������� �� �������������� ����������

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new Texture2D (GetContextManager (), extensions, desc);

  if (extensions->has_ext_texture_rectangle && !is_compressed (desc.format) && !desc.generate_mips_enable)
    return new TextureNpot (GetContextManager (), extensions, desc);

  return new ScaledTexture (GetContextManager (), texture_manager, desc);
}

ITexture* TextureManager::Impl::CreateTexture3D (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture3D";

    //�������� ����������� �������� ��������

  if (!extensions->has_ext_texture3d)
    RaiseNotSupported (METHOD_NAME, "3D textures not supported (GL_ext_texture_3d not supported)");
    
  if (desc.width > max_3d_texture_size || desc.height > max_3d_texture_size || desc.layers > max_3d_texture_size)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u (max_edge_size=%u)", desc.width, desc.height,
                       desc.layers, max_3d_texture_size);
  }

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height) && is_power_of_two (desc.layers);

  if (!is_pot && !extensions->has_arb_texture_non_power_of_two)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                       desc.width, desc.height, desc.layers, get_name (desc.format));
  }
  
    //�������� ��������

  return new Texture3D (GetContextManager (), extensions, desc);
}

ITexture* TextureManager::Impl::CreateTextureCubemap (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTextureCubemap";
  
    //�������� ����������� �������� ��������
  
  if (!extensions->has_arb_texture_cube_map)
    RaiseNotSupported (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");

  if (desc.width > max_cube_map_texture_size || desc.height > max_cube_map_texture_size)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create cubemap texture %ux%u (max_edge_size=%u)", desc.width, desc.height,
                       max_cube_map_texture_size);
  }
  
    //��������������� �������� �������� � ����������� �� �������������� ����������

  bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

  if (is_pot || extensions->has_arb_texture_non_power_of_two)
    return new TextureCubemap (GetContextManager (), extensions, desc);

  return new ScaledTexture (GetContextManager (), texture_manager, desc);  
}

//��������������� �������� �������  
ITexture* TextureManager::Impl::CreateTexture (const TextureDesc& desc)
{
  try
  {
    switch (desc.dimension)
    {
      case TextureDimension_1D:      return CreateTexture1D (desc);
      case TextureDimension_2D:      return CreateTexture2D (desc);
      case TextureDimension_3D:      return CreateTexture3D (desc);
      case TextureDimension_Cubemap: return CreateTextureCubemap (desc);
      default:
        RaiseInvalidArgument ("", "desc.dimension", desc.dimension);
        return 0;
    }
  }
  catch (common::Exception& e)
  {
    e.Touch ("render::low_level::opengl::TextureManager::Impl::CreateTexture");

    throw;
  }
}

ISamplerState* TextureManager::Impl::CreateSamplerState (const SamplerDesc& in_desc)
{
  return new SamplerState (GetContextManager (), extensions, in_desc);
}

/*
   ��������� ������� �������� � ��������
*/

void TextureManager::Impl::SetTexture (size_t sampler_slot, ITexture* texture)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set texture in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);
  
  samplers [sampler_slot].texture = cast_object<BindableTexture> (*this, texture, METHOD_NAME, "state");
}

void TextureManager::Impl::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::SetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't set sampler in unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  samplers [sampler_slot].state = cast_object<SamplerState> (*this, state, METHOD_NAME, "state");
}

ITexture* TextureManager::Impl::GetTexture (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetTexture";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get texture from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return samplers [sampler_slot].texture.get ();
}

ISamplerState* TextureManager::Impl::GetSampler (size_t sampler_slot) const
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::GetSampler";

  if (sampler_slot >= texture_units_count)
    RaiseNotSupported (METHOD_NAME, "Can't get sampler from unit %u (maximum texture units = %u)", sampler_slot, texture_units_count);

  return samplers [sampler_slot].state.get ();
}

/*
   ����������� / ����������
*/

TextureManager::TextureManager (const ContextManager& context_manager)
  : impl (new Impl (context_manager, *this))
  {}

TextureManager::~TextureManager ()
{
}

/*
   ������� ������� � ���������
*/

void TextureManager::Bind ()
{
  impl->Bind ();
}
    
/*
   �������� �������� � ��������
*/

ITexture* TextureManager::CreateTexture (const TextureDesc& tex_desc)
{
  return impl->CreateTexture (tex_desc);
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
  impl->SetTexture (sampler_slot, texture);
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  impl->SetSampler (sampler_slot, state);
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  return impl->GetTexture (sampler_slot);
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  return impl->GetSampler (sampler_slot);
}
