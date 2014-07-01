#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
===================================================================================================
    ���������
===================================================================================================
*/

const size_t DEFAULT_TEMP_BUFFER_SIZE = 256; //��������� ������ ������ ��� ��������� ������

/*
===================================================================================================
    �������
===================================================================================================    
*/

struct OpenGLException: virtual public xtl::exception {};

/*
===================================================================================================
    �������� ���������� ��������� OpenGL
===================================================================================================
*/

class ContextImpl: public xtl::reference_counter, private IContextListener
{
  public:
///�����������
    ContextImpl (ISwapChain* swap_chain, const ContextSettings& context_settings)
      : need_check_errors (context_settings.IsNeedCheckErrors ())
    {
      try
      {
          //������� ������������ ����

        memset (context_cache.data (), 0, context_cache.size () * sizeof (ContextCache::value_type));

          //�������� ���������

        context = ContextPtr (PlatformManager::CreateContext (swap_chain), false);

          //�������� �� ������� ���������

        context->AttachListener (this);

          //����� ��������� ���������

        context->MakeCurrent (swap_chain);

          //������������� ������� ����� ����� OpenGL 1.1

        gl_entries.Init (context->GetLibrary ());

          //���������� ������� ���������
          
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

        glDrawBuffer (0);
        glReadBuffer (0);

#endif

          //����������� ��������� ����������

        extensions_string = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));
        version_string    = reinterpret_cast<const char*> (glGetString (GL_VERSION));
        vendor_string     = reinterpret_cast<const char*> (glGetString (GL_VENDOR));
        renderer_string   = reinterpret_cast<const char*> (glGetString (GL_RENDERER));        

          //���������� ������������� ����������

        IPropertyList* swap_chain_properties = swap_chain->GetProperties ();        

        if (swap_chain_properties)
        {
          for (size_t i=0, count=swap_chain_properties->GetSize (); i<count; i++)
            if (!xtl::xstrcmp (swap_chain_properties->GetKey (i), "gl_extensions"))
            {
              const char* extensions = swap_chain_properties->GetValue (i);              

              if (*extensions)
              {
                extensions_string += ' ';
                extensions_string += extensions;
              }
              
              break;
            }
        }

          //��������� ������ ��������� ����������

        extensions.SetGroup (extensions_string.c_str (), true);

          //����������� ��������� ���������� � ������

        Version version = version_string.c_str ();

        struct VersionEntry
        {
          Version   version;
          Extension extension;
          
          VersionEntry (const char* version_id, const char* extension_id) : version (version_id), extension (extension_id) {}
        };        

        static VersionEntry version_entries [] = {
#ifndef OPENGL_ES2_SUPPORT
          VersionEntry ("1.1", "GL_VERSION_1_1"),
#else
          VersionEntry ("2.0", "GL_VERSION_2_0"),
#endif

#ifndef OPENGL_ES_SUPPORT
          VersionEntry ("1.2", "GL_VERSION_1_2"),
          VersionEntry ("1.3", "GL_VERSION_1_3"),
          VersionEntry ("1.4", "GL_VERSION_1_4"),
          VersionEntry ("1.5", "GL_VERSION_1_5"),
          VersionEntry ("2.0", "GL_VERSION_2_0"),
          VersionEntry ("2.1", "GL_VERSION_2_1"),
#endif
        };

        static const size_t version_entries_count = sizeof (version_entries) / sizeof (*version_entries);

        for (size_t i=0; i<version_entries_count; i++)
          if (version_entries [i].version < version)
            extensions.Set (version_entries [i].extension, true);

        const ExtensionSet& required_extensions = context_settings.RequiredExtensions ();
        ExtensionSet        enabled_extensions  = context_settings.EnabledExtensions ();

        enabled_extensions |= ExtensionSet::BugExtensions ();

        extensions &= enabled_extensions;

        for (size_t id=0, count=ExtensionSet::Size (); id<count; id++)
          if (required_extensions.Get (id) && !extensions.Get (id))
          {
            throw xtl::format_not_supported_exception ("", "Could not create new context. Reason: required extension '%s' not supported",
              get_extension_name (id));
          }          

        const Version &min_version = context_settings.MinVersion (),
                      &max_version = context_settings.MaxVersion ();

        if (min_version.IsValid () && version < min_version)
        {
          throw xtl::format_not_supported_exception ("", "Could not create new context. Reason: OpenGL version '%u.%u' not supported "
            "(version='%s')", min_version.major_version, min_version.minor_version, version_string.c_str ());
        }

        if (max_version.IsValid ())
        {
          for (size_t i=0; i<version_entries_count; i++)
          {
            if (max_version < version_entries [i].version)
              extensions.Set (version_entries [i].extension, false);
          }        
        }

          //����������� ����� ������ OpenGL

        stl::string bug_string;

        detect_opengl_bugs (bug_string);

        extensions.SetGroup (bug_string.c_str (), true);
        
        extensions_string += bug_string;

          //������������� ������� ������������ ���������

        context_caps.Init (extensions, context_settings.EnabledExtensions (), context_settings);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ContextImpl::ContextImpl");        
        throw;
      }
    }
       
///��������� ���������
    IContext& GetContext () { return *context; }

///��������� ����� ����� OpenGL 1.1
    const GlEntries& GetGlEntries () { return gl_entries; }

///��������� ������ ��������� ����������
    ExtensionSet& GetExtensions () { return extensions; } 
 
///��������� ���������-�������������� ������������ ���������
    const ContextCaps& GetCaps () { return context_caps; }
    
///����� �� ��������� ������
    bool IsNeedCheckErrors () { return need_check_errors; }

///��������� ���������� � ���������� OpenGL
    const char* GetExtensionsString () { return extensions_string.c_str (); }
    const char* GetVersionString    () { return version_string.c_str (); }
    const char* GetVendorString     () { return vendor_string.c_str (); }
    const char* GetRendererString   () { return renderer_string.c_str (); }

///��������� ���� ���������
    size_t* GetContextCache () { return &context_cache [0]; }

///��������� �������� ���������
    static ContextImpl* GetCurrentContext ()
    {
      return current_context;
    }

  private:
///�������� ���� �������
    void OnSetCurrent ()
    {
      current_context = this;
    } 

///�������� �������� ���� �������
    virtual void OnLostCurrent ()
    {
      current_context = 0;
    }

  private:
    typedef xtl::com_ptr<ISwapChain>           SwapChainPtr;
    typedef xtl::com_ptr<IContext>             ContextPtr;
    typedef xtl::array<size_t, CacheEntry_Num> ContextCache;
    
  private:
    ContextPtr   context;            //�������� OpenGL
    GlEntries    gl_entries;         //������� ����� ����� OpenGL 1.1
    ExtensionSet extensions;         //����� �������������� ����������
    stl::string  extensions_string;  //������ �������������� ����������
    stl::string  version_string;     //������ OpenGL
    stl::string  vendor_string;      //������������� ���������� OpenGL
    stl::string  renderer_string;    //��� ���������� ��������� OpenGL
    ContextCache context_cache;      //��� ��������� ���������
    ContextCaps  context_caps;       //��������� �������������� ����������� ���������
    bool         need_check_errors;  //����� �� ��������� ������

  private:
    static ContextImpl* current_context; //��������� �� ������� ��������
};

ContextImpl* ContextImpl::current_context = 0;

}

/*
===================================================================================================
    �������� ���������� ContextManager
===================================================================================================
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  public:  
///�����������
    Impl (ISwapChain* swap_chain, const char* init_string)
      : context (swap_chain, init_string),
        temp_buffer (DEFAULT_TEMP_BUFFER_SIZE),
        current_swap_chain (0),
        check_gl_errors (true),
        need_change_context (true),
        on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
    {
        //������������� ������ ���������� �������

      for (size_t stage=0; stage<Stage_Num; stage++)
        need_stage_rebind [stage] = true;

        //��������� ��������

      if (!swap_chain)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::Impl::Impl", "swap_chain");

      adapter = swap_chain->GetAdapter ();
    }

///��������� ��������
    IAdapter* GetAdapter () { return adapter.get (); }

///��������� ������� ������� ������
    void SetSwapChain (ISwapChain* swap_chain)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::SetSwapChain";

      if (swap_chain == current_swap_chain)
        return;
     
      if (!swap_chain) //������������� ��������� ������� ������
        throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

        //����������� ������������ �������� ������� ������

      swap_chain->RegisterDestroyHandler (on_destroy_swap_chain);

        //���������� ������

      current_swap_chain  = swap_chain;
      need_change_context = true;

        //���������� � ������������� ���������� ��������� ������� ������� ���������

      StageRebindNotify (Stage_RenderTargetManager);
    }
    
///��������� �������� ���������
    void MakeContextCurrent (bool clear_errors)
    {
        //�������� ������������� ����� ����������
      
      if (ContextImpl::GetCurrentContext () == &context && !need_change_context)
        return;
      
      try
      {
          //��������� ���������

        context.GetContext ().MakeCurrent (current_swap_chain);
        
        need_change_context = false;

          //������� ������� ������

        if (clear_errors)
          ClearErrors ();          
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::ContextManager::MakeContextCurrent");

        throw;
      }
    }

///��������� �������� ��������� � ������� ������
    ISwapChain*  GetSwapChain () { return current_swap_chain; }
    ContextImpl& GetContext   () { return context; }

///��������� ������ ������ ��� ���������� ���������� ������
    void* GetTempBuffer (size_t size)
    {
      temp_buffer.resize (size, false);

      return temp_buffer.data ();
    }

///�������� ����������� ������� ������
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain)
    {
      if (!swap_chain)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*)", "swap_chain");
      
      return PlatformManager::CreatePBuffer (swap_chain);
    }
    
///�������� ����������� ������� ������
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
    {
      if (!swap_chain)
        throw xtl::make_null_argument_exception ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*,const SwapChainDesc&)", "swap_chain");
      
      return PlatformManager::CreatePBuffer (swap_chain, &desc);
    }    

///�������� ������������� ��������� � ������� ������
    bool IsCompatible (ISwapChain* swap_chain)
    {
      return context.GetContext ().IsCompatible (swap_chain);
    }

///����������������
    void LogPrint (const char* message)
    {
      log.Print (message);
    }

    void LogVPrintf (const char* message, va_list args)
    {
      log.VPrintf (message, args);
    }

///��������� / ���������� �������� ������
    void SetValidationState (bool state)
    {
      check_gl_errors = state;
    }
    
    bool GetValidationState () { return check_gl_errors && context.IsNeedCheckErrors (); }
    
//�������� ������ OpenGL
    void CheckErrors (const char* source)
    {
      if (!GetValidationState ())      
        return;

      if (!source)
        source = "render::low_level::ContextManager::Impl::CheckErrors";

      MakeContextCurrent (false);

      GLenum error = glGetError ();

      switch (error)
      {
        case GL_NO_ERROR:
          break;
        case GL_INVALID_ENUM:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid enum");
        case GL_INVALID_VALUE:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid value");
        case GL_INVALID_OPERATION:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid operation");
        case GL_STACK_OVERFLOW:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: stack overflow");
        case GL_STACK_UNDERFLOW:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: stack underflow");
        case GL_OUT_OF_MEMORY:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: out of memory");
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: invalid framebuffer operation");
        default:
          throw xtl::format_exception<OpenGLException> (source, "OpenGL error: code=0x%04x", error);
      }
    }

///������� ������� ������ OpenGL
    void ClearErrors ()
    {
      if (!check_gl_errors)
        return;

      while (glGetError () != GL_NO_ERROR);
    }
    
///���������� � ������������� ������� ������
    void StageRebindNotify (Stage stage)
    {
      if (stage < 0 || stage >= Stage_Num)
        throw xtl::make_argument_exception ("render::low_level::opengl::ContextManager::Impl::StageRebindNotify", "stage", stage);

      need_stage_rebind [stage] = true;
    }
    
///������� ������ ����������
    void ResetRebindNotifications ()
    {
      for (size_t stage=0; stage<Stage_Num; stage++)
        need_stage_rebind [stage] = false;
    }
    
///�������� ������������� ������� ������
    bool NeedStageRebind (Stage stage)
    {
      if (stage < 0 || stage >= Stage_Num)
        return false;

      return need_stage_rebind [stage];
    }  

  private:
///���������� �������� ������� ������
    void OnDestroySwapChain ()
    {
      current_swap_chain  = 0;
      need_change_context = true;
    }

  private:
    typedef xtl::com_ptr<IAdapter>           AdapterPtr;
    typedef xtl::uninitialized_storage<char> DataBuffer;

  private:
    Log                       log;                           //��������
    ContextImpl               context;                       //��������
    DataBuffer                temp_buffer;                   //����� ������ ��� ��������� ������
    LogHandler                log_handler;                   //���������� ����������������
    AdapterPtr                adapter;                       //������� ��������� ����������
    ISwapChain*               current_swap_chain;            //������� ������� ������
    bool                      check_gl_errors;               //����� �� ��������� ������ OpenGL
    bool                      need_change_context;           //���������� ������� ��������
    xtl::trackable::slot_type on_destroy_swap_chain;         //���������� �������� ������� ������
    bool                      need_stage_rebind [Stage_Num]; //�����, ������������ ������������� ���������� ������
};

/*
    ������������ / ���������� / ������������
*/

ContextManager::ContextManager (ISwapChain* swap_chain, const char* init_string)
  : impl (new Impl (swap_chain, init_string))
{
}

ContextManager::ContextManager (const ContextManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ContextManager::~ContextManager ()
{
  release (impl);
}

ContextManager& ContextManager::operator = (const ContextManager& manager)
{  
  addref (manager.impl);
  release (impl);

  impl = manager.impl;

  return *this;
}

/*
    �������� ������� ������ ����������� � ����������
*/

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain)
{
  return impl->CreateCompatibleSwapChain (swap_chain);
}

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
{
  return impl->CreateCompatibleSwapChain (swap_chain, desc);
}

/*
    ����� ������� ������� ������
*/

void ContextManager::SetSwapChain (ISwapChain* swap_chain)
{
  impl->SetSwapChain (swap_chain);
}

ISwapChain* ContextManager::GetSwapChain () const
{
  return impl->GetSwapChain ();
}

/*
    ��������� �������� ���������
*/

void ContextManager::MakeContextCurrent () const
{
  impl->MakeContextCurrent (true);
}

bool ContextManager::TryMakeContextCurrent () const
{
  try
  {
    MakeContextCurrent ();
    
    return true;
  }
  catch (...)
  {
    return false;
  }
}

/*
    ������ � ����� �������� ���������
*/

const size_t* ContextManager::GetContextCache () const
{
  return impl->GetContext ().GetContextCache ();
}

size_t* ContextManager::GetContextCache ()
{
  return impl->GetContext ().GetContextCache ();
}

void ContextManager::SetContextCacheValue (size_t entry_id, size_t value)
{
  if (entry_id >= CacheEntry_Num)
    throw xtl::make_range_exception ("render::low_level::opengl::SetContextCacheValue", "entry_id", entry_id, size_t (CacheEntry_Num));
    
  GetContextCache ()[entry_id] = value;
}

size_t ContextManager::GetContextCacheValue (size_t entry_id) const
{
  if (entry_id >= CacheEntry_Num)
    throw xtl::make_range_exception ("render::low_level::opengl::GetContextCacheValue", "entry_id", entry_id, size_t (CacheEntry_Num));
    
  return GetContextCache ()[entry_id];
}

/*
   ��������� ������ ������ ��� ���������� ���������� ������
*/

void* ContextManager::GetTempBuffer (size_t size) const
{
  return impl->GetTempBuffer (size);
}

/*
    ��������� ���������� � ������� ���������� OpenGL
*/

const char* ContextManager::GetExtensions () const
{
  return impl->GetContext ().GetExtensionsString ();
}

const char* ContextManager::GetVersion () const
{
  return impl->GetContext ().GetVersionString ();
}

const char* ContextManager::GetVendor () const
{
  return impl->GetContext ().GetVendorString ();
}

const char* ContextManager::GetRenderer () const
{
  return impl->GetContext ().GetRendererString ();
}

/*
    �������
*/

IAdapter* ContextManager::GetAdapter () const
{
  return impl->GetAdapter ();
}

/*
    �������� ������������� ��������� � ������� ������
*/

bool ContextManager::IsCompatible (ISwapChain* swap_chain) const
{
  return impl->IsCompatible (swap_chain);
}

/*
    �������� ������������� ���������� ����������
*/

bool ContextManager::IsCompatible (const ContextManager& manager) const
{
  return impl == manager.impl;
}

/*
    ����������� ��������� ���������� ����������
*/

bool ContextManager::IsSupported (const Extension& extension) const
{
  return impl->GetContext ().GetExtensions ().Get (extension);
}

/*
   ��������� ��������� �������������� ������������ ���������
*/

const ContextCaps& ContextManager::GetCaps () const
{
  return impl->GetContext ().GetCaps ();
}

/*
    ����������������
*/

void ContextManager::LogPrintf (const char* format, ...) const
{
  va_list list;
  
  va_start   (list, format);  
  LogVPrintf (format, list);
}

void ContextManager::LogVPrintf (const char* format, va_list args) const
{
  impl->LogVPrintf (format, args);
}

/*
    �������� ������ OpenGL
*/

void ContextManager::SetValidationState (bool state)
{
  impl->SetValidationState (state);  
}

bool ContextManager::GetValidationState () const
{
  return impl->GetValidationState ();
}

void ContextManager::CheckErrors (const char* source) const
{
  impl->CheckErrors (source);
}

void ContextManager::RaiseError (const char* source) const
{
  if (!source)
    return;
    
  CheckErrors (source);
  
  throw xtl::format_operation_exception (source, "Invalid operation (internal error)");
}

void ContextManager::ClearErrors () const
{
  impl->ClearErrors ();
}

/*
    ���������� � ������������� ������� ������ / ������� ������ ���������� / �������� ������������� ������� �������
*/

void ContextManager::StageRebindNotify (Stage stage)
{
  impl->StageRebindNotify (stage);
}

void ContextManager::ResetRebindNotifications ()
{
  impl->ResetRebindNotifications ();
}

bool ContextManager::NeedStageRebind (Stage stage) const
{
  return impl->NeedStageRebind (stage);
}

namespace render
{

namespace low_level
{

namespace opengl
{

/*
    ��������� ������� ����� ����� ��� �������� ���������
*/

const GlEntries* get_gl_entries ()
{
  return &ContextImpl::GetCurrentContext ()->GetGlEntries ();
}

}

}

}
