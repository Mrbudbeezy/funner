#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    ���� �������������
*/

typedef xtl::trackable_ptr<BindableTexture> TexturePtr;
typedef xtl::trackable_ptr<SamplerState>    SamplerStatePtr;

struct SamplerSlot
{
  TexturePtr      texture;       //��������
  SamplerStatePtr sampler_state; //��������� ��������
};

/*
    ��������� ��������� �������
*/

class TextureManagerState: public IStageState
{
  public:
      //������������
    TextureManagerState (TextureManagerState* in_main_state) : owner (0), main_state (in_main_state) {}
    TextureManagerState (ContextObject* in_owner) : owner (in_owner), main_state (0) {}    
    
      //��������� ������ �������������
    SamplerSlot* GetSlots () { return &samplers [0]; }
    
      //��������� ����� �������������
    SamplerSlot& GetSlot (size_t slot)
    {
      if (slot >= DEVICE_SAMPLER_SLOTS_COUNT)
        throw xtl::make_range_exception ("render::low_level::opengl::TextureManagerState::GetSlot", "slot", slot, DEVICE_SAMPLER_SLOTS_COUNT);
        
      return samplers [slot];
    }

      //��������� ��������
    void SetTexture (size_t slot, BindableTexture* in_texture)
    {
      TexturePtr& texture = GetSlot (slot).texture;
      
      if (texture == in_texture)
        return;
        
      texture = in_texture;
      
      UpdateNotify ();
    }

      //��������� ��������
    BindableTexture* GetTexture (size_t slot)
    {
      return GetSlot (slot).texture.get ();
    }
    
      //��������� ��������� �������������
    void SetSampler (size_t slot, SamplerState* in_state)
    {
      SamplerStatePtr& state = GetSlot (slot).sampler_state;
      
      if (state == in_state)
        return;
        
      state = in_state;
      
      UpdateNotify ();
    }
    
      //��������� ��������� �������������
    SamplerState* GetSampler (size_t slot)
    {
      return GetSlot (slot).sampler_state.get ();
    }

      //������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }

      //�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:
      //����������� ���������
    void Copy (const TextureManagerState& source, const StateBlockMask& mask)
    {
      bool need_rebind = false;
      
      for (size_t i=0; i<DEVICE_SAMPLER_SLOTS_COUNT; i++)
      {
        if (mask.ss_textures [i])
        {
          samplers [i].texture = source.samplers [i].texture;
          need_rebind          = true;
        }

        if (mask.ss_samplers [i])
        {
          samplers [i].sampler_state = source.samplers [i].sampler_state;
          need_rebind                = true;
        }
      }
      
      if (need_rebind)
        UpdateNotify ();
    }
    
      //���������� �� ���������
    void UpdateNotify ()
    {
      if (!owner)
        return;
        
      owner->GetContextManager ().StageRebindNotify (Stage_TextureManager);
    }
  
  private:
    typedef xtl::trackable_ptr<TextureManagerState>             TextureManagerStatePtr;
    typedef xtl::array<SamplerSlot, DEVICE_SAMPLER_SLOTS_COUNT> SamplerSlotArray;

  private:
    ContextObject*         owner;      //�������� ���������
    TextureManagerStatePtr main_state; //�������� ���������
    SamplerSlotArray       samplers;   //����� �������������
};

//�������� ������� �� ������� ������
bool is_power_of_two (size_t size)
{
  return ((size - 1) & size) == 0;
}

}

/*
    �������� ���������� TextureManager
*/

struct TextureManager::Impl: public ContextObject
{
  public:
      //�����������    
    Impl (const ContextManager& context_manager, TextureManager& in_texture_manager) :
      ContextObject (context_manager),
      texture_manager (in_texture_manager),
      state (this) {}
    
      //��������� ��������� ���������
    TextureManagerState& GetState () { return state; }

      //������� ������� � ���������
    void Bind ()
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::Bind";

        //��������� ���-����������

      size_t *common_cache             = &GetContextDataTable (Stage_Common)[0],
             *cache                    = &GetContextDataTable (Stage_TextureManager)[0],
             *current_texture_id       = cache + TextureManagerCache_TextureId0,
             *current_texture_target   = cache + TextureManagerCache_TextureTarget0,
             &current_enabled_textures = common_cache [CommonCache_EnabledTextures],
             &current_active_slot      = common_cache [CommonCache_ActiveTextureSlot],
             enabled_textures          = 0;

        //����� �������� ���������

      MakeContextCurrent ();  

        //��������� ������� � ���������

      BindableTextureDesc texture_desc;

      const ContextCaps& caps     = GetCaps ();
      SamplerSlot*       samplers = state.GetSlots ();      

      for (size_t i = 0; i < caps.texture_units_count; i++)
      {
        BindableTexture* texture        = samplers [i].texture.get ();
        SamplerState*    sampler_state  = samplers [i].sampler_state.get ();
        bool             is_active_slot = texture && sampler_state;          

        if (is_active_slot)
        {
          texture->GetDesc (texture_desc);
          
          enabled_textures |= 1 << i;
        }

        GLenum texture_target      = is_active_slot ? texture_desc.target : 0;
        bool   need_change_sampler = is_active_slot && texture->GetSamplerHash () != sampler_state->GetDescHash (),
               need_bind           = need_change_sampler || is_active_slot && current_texture_id [i] != texture->GetId (),
               need_change_mode    = current_texture_target [i] != texture_target,
               need_set_slot       = need_bind || need_change_mode;

          //��������� ��������� �����

        if (need_set_slot && current_active_slot != i && caps.glActiveTexture_fn)
        {
          caps.glActiveTexture_fn (GL_TEXTURE0 + i);

          current_active_slot = i;
        }
        
          //������� ��������
          
        if (need_bind)
          texture->Bind ();
          
          //��������� ��������
          
        if (need_change_sampler)
        {
            //�������� ������ ��������� ��� ����� � ��� �� �������� ��������� ���������
            
          for (size_t j=0; j<i; j++)
            if (samplers [j].texture == texture && samplers [j].sampler_state && samplers [j].sampler_state != sampler_state)
            {
              throw xtl::format_not_supported_exception (METHOD_NAME, "Sampler slot #%u conflicts with sampler slot #%u. "
                                 "Reason: different sampler states for same texture", j, i);
            }

          sampler_state->Bind (texture_target);

          texture->SetSamplerHash (sampler_state->GetDescHash ());
        }
        
          //��������� ������ ��������������� ��� �������� �����

        if (need_change_mode)
        {
          if (current_texture_target [i]) glDisable (current_texture_target [i]);
          if (texture_target)             glEnable  (texture_target);

          current_texture_target [i] = texture_target;
        }    
      }

        //�������� ������

      CheckErrors (METHOD_NAME);

        //���������� ���-����������

      current_enabled_textures = enabled_textures;
    }

      //�������� ��������
    ITexture* CreateTexture (const TextureDesc& desc)
    {
      switch (desc.dimension)
      {
        case TextureDimension_1D:      return CreateTexture1D (desc);
        case TextureDimension_2D:      return CreateTexture2D (desc);
        case TextureDimension_3D:      return CreateTexture3D (desc);
        case TextureDimension_Cubemap: return CreateTextureCubemap (desc);
        default:
          throw xtl::make_argument_exception ("", "desc.dimension", desc.dimension);
          return 0;
      }
    }
    
      //c������� ��������
    ISamplerState* TextureManager::Impl::CreateSamplerState (const SamplerDesc& desc)
    {
      return new SamplerState (GetContextManager (), desc);
    }
    
    /*
        ��������� ������� �������� � ��������    
    */
    
    void SetTexture (size_t sampler_slot, ITexture* texture)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::SetTexture";

      return state.SetTexture (CheckSamplerSlot (sampler_slot), cast_object<BindableTexture> (*this, texture, METHOD_NAME, "texture"));
    }

    void SetSampler (size_t sampler_slot, ISamplerState* sampler_state)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::SetSampler";

      return state.SetSampler (CheckSamplerSlot (sampler_slot), cast_object<SamplerState> (*this, sampler_state, METHOD_NAME, "state"));
    }

    ITexture* GetTexture (size_t sampler_slot)
    {
      return state.GetTexture (CheckSamplerSlot (sampler_slot));
    }

    ISamplerState* GetSampler (size_t sampler_slot)
    {
      return state.GetSampler (CheckSamplerSlot (sampler_slot));
    }

  private:
      //�������� ������ ����� �������������
    size_t CheckSamplerSlot (size_t sampler_slot)
    {
      const ContextCaps& caps = GetCaps ();

      if (sampler_slot >= caps.texture_units_count)
        throw xtl::format_not_supported_exception ("render::low_level::opengl::TextureManager::Impl::CheckSamplerSlot",
                           "Sampler slot #%u not supported (texture units count = %u)", sampler_slot, caps.texture_units_count);

      return sampler_slot;
    }
    
      //�������� ���������� ��������
    ITexture* CreateTexture1D (const TextureDesc& in_desc)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture1D";
      
        //������������� �������������� ����������
      
      TextureDesc desc = in_desc;
      
      desc.height = desc.layers = 1;
      
        //�������� ����������� �������� ��������

      const ContextCaps& caps = GetCaps ();

      if (desc.width > caps.max_texture_size)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, caps.max_texture_size);

        //��������������� �������� �������� � ����������� �� �������������� ����������

      bool is_pot = is_power_of_two (desc.width);    

      if (is_pot || caps.has_arb_texture_non_power_of_two)
        return new Texture1D (GetContextManager (), desc);

      if (caps.has_ext_texture_rectangle)
      {
        if (desc.width > caps.max_rectangle_texture_size)
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 1D texture with width %u (maximum texture size is %u)", desc.width, caps.max_rectangle_texture_size);

        return new TextureNpot (GetContextManager (), desc);
      }

      return new ScaledTexture (GetContextManager (), texture_manager, desc);
    }
    
      //�������� ��������� ��������
    ITexture* CreateTexture2D (const TextureDesc& in_desc)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture2D";
      
        //������������� �������������� ����������
      
      TextureDesc desc = in_desc;
      
      desc.layers = 1;  
      
        //�������� ����������� �������� ��������

      const ContextCaps& caps = GetCaps ();

      if (desc.width > caps.max_texture_size || desc.height > caps.max_texture_size)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 2D texture %ux%u (maximum texture size is %u)", desc.width, desc.height, caps.max_texture_size);

        //��������������� �������� �������� � ����������� �� �������������� ����������

      bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

      if (is_pot || caps.has_arb_texture_non_power_of_two)
        return new Texture2D (GetContextManager (), desc);

      if (caps.has_ext_texture_rectangle && !is_compressed (desc.format) && !desc.generate_mips_enable)
      {
        if (desc.width > caps.max_rectangle_texture_size || desc.height > caps.max_rectangle_texture_size)
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 2D texture %ux%u (maximum texture size is %u)", desc.width, desc.height, caps.max_rectangle_texture_size);

        return new TextureNpot (GetContextManager (), desc);
      }

      return new ScaledTexture (GetContextManager (), texture_manager, desc);
    }

      //�������� ��������� ��������
    ITexture* CreateTexture3D (const TextureDesc& desc)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTexture3D";

        //�������� ����������� �������� ��������

      const ContextCaps& caps = GetCaps ();

      if (!caps.has_ext_texture3d)
        throw xtl::format_not_supported_exception (METHOD_NAME, "3D textures not supported (GL_ext_texture_3d not supported)");
        
      if (desc.width > caps.max_3d_texture_size || desc.height > caps.max_3d_texture_size || desc.layers > caps.max_3d_texture_size)
      {
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 3D texture %ux%ux%u (max_edge_size=%u)", desc.width, desc.height,
                           desc.layers, caps.max_3d_texture_size);
      }

      bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height) && is_power_of_two (desc.layers);

      if (!is_pot && !caps.has_arb_texture_non_power_of_two)
      {
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 3D texture %ux%ux%u@%s (GL_ARB_texture_non_power_of_two & GL_VERSION_2_0 not supported)",
                           desc.width, desc.height, desc.layers, get_name (desc.format));
      }
      
        //�������� ��������

      return new Texture3D (GetContextManager (), desc);
    }

      //�������� ���������� ��������
    ITexture* CreateTextureCubemap (const TextureDesc& desc)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::TextureManager::Impl::CreateTextureCubemap";

        //�������� ����������� �������� ��������

      const ContextCaps& caps = GetCaps ();

      if (!caps.has_arb_texture_cube_map)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Cubemap textuers not supported. No 'ARB_texture_cubemap' extension");

      if (desc.width > caps.max_cube_map_texture_size || desc.height > caps.max_cube_map_texture_size)
      {
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create cubemap texture %ux%u (max_edge_size=%u)", desc.width, desc.height,
                           caps.max_cube_map_texture_size);
      }
      
        //��������������� �������� �������� � ����������� �� �������������� ����������

      bool is_pot = is_power_of_two (desc.width) && is_power_of_two (desc.height);

      if (is_pot || caps.has_arb_texture_non_power_of_two)
        return new TextureCubemap (GetContextManager (), desc);

      return new ScaledTexture (GetContextManager (), texture_manager, desc);  
    }    

  public:
    TextureManager&     texture_manager; //�������� �������
    TextureManagerState state;           //��������� ��������� �������
};

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
    �������� ������� ��������� ������
*/

IStageState* TextureManager::CreateStageState ()
{
  return new TextureManagerState (&impl->GetState ());
}

/*
   ������� ������� � ���������
*/

void TextureManager::Bind ()
{
  try
  {
    impl->Bind ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::Bind");
    throw;
  }
}
    
/*
   �������� �������� � ��������
*/

ITexture* TextureManager::CreateTexture (const TextureDesc& tex_desc)
{
  try
  {
    return impl->CreateTexture (tex_desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::CreateTexture");
    throw;
  }
}

ISamplerState* TextureManager::CreateSamplerState (const SamplerDesc& sampler_desc)
{
  try
  {
    return impl->CreateSamplerState (sampler_desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::CreateSamplerState");
    throw;
  }
}

/*
   ��������� ������� �������� � ��������
*/

void TextureManager::SetTexture (size_t sampler_slot, ITexture* texture)
{
  try
  {
    impl->SetTexture (sampler_slot, texture);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::SetTexture");
    throw;
  }
}

void TextureManager::SetSampler (size_t sampler_slot, ISamplerState* state)
{
  try
  {
    impl->SetSampler (sampler_slot, state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::SetSampler");
    throw;
  }
}

ITexture* TextureManager::GetTexture (size_t sampler_slot) const
{
  try
  {
    return impl->GetTexture (sampler_slot);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::GetTexture");
    throw;
  }
}

ISamplerState* TextureManager::GetSampler (size_t sampler_slot) const
{
  try
  {
    return impl->GetSampler (sampler_slot);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::TextureManager::GetSampler");
    throw;
  }  
}
