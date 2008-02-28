#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
===================================================================================================
    �������
===================================================================================================    
*/

struct OpenGLExceptionTag;

typedef DerivedException<Exception, OpenGLExceptionTag> OpenGLException; //����������: ���������� ������ OpenGL

/*
===================================================================================================
    �������� ���������� ��������� OpenGL
===================================================================================================
*/

class ContextImpl: public xtl::reference_counter
{
  public:
    ContextImpl (ISwapChain* swap_chain) : context (swap_chain), master_swap_chain (swap_chain)
    {
      Init ();
    }

    ContextImpl (ISwapChain* swap_chain, const Context& shared_context) : context (swap_chain, shared_context), master_swap_chain (swap_chain)
    {
      Init ();
    }
    
      //��������� ��������� � ������� ������� ������
       Context&  GetContext () { return context; }
    ISwapChain*  GetMasterSwapChain () const { return master_swap_chain.get (); }
    
      //��������� ������ ��������� ����������
    ExtensionSet& GetExtensions () { return extensions; } 
 
      ///��������� ��������� �������������� ������������ ���������
    const ContextCaps& GetCaps () const
    {
      return context_caps;
    }
    
      //��������� ���������� � ���������� OpenGL
    const char* GetExtensionsString () const { return extensions_string.c_str (); }
    const char* GetVersionString    () const { return version_string.c_str (); }
    const char* GetVendorString     () const { return vendor_string.c_str (); }
    const char* GetRendererString   () const { return renderer_string.c_str (); }

      //��������� ������� ��������� ������ ���������
    ContextDataTable& GetContextDataTable (Stage table_id)
    {
      if (table_id < 0 || table_id >= Stage_Num)
        RaiseOutOfRange ("render::low_level::opengl::ContextImpl::GetContextDataTable", "table_id", table_id, 0, Stage_Num);

      return context_data_table [table_id];
    }
    
  private:

      //�������������
    void Init ()        
    {
      try
      {
          //����� ��������� ���������
        
        context.MakeCurrent (master_swap_chain.get ());
        
          //���������� ������� ���������

        glDrawBuffer (GL_NONE);
        glReadBuffer (GL_NONE);

          //����������� ��������� ����������        

        extensions_string = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));
        version_string    = reinterpret_cast<const char*> (glGetString (GL_VERSION));
        vendor_string     = reinterpret_cast<const char*> (glGetString (GL_VENDOR));
        renderer_string   = reinterpret_cast<const char*> (glGetString (GL_RENDERER));        

        const char* swap_chain_extension_string = Context::GetSwapChainExtensionString ();

        if (*swap_chain_extension_string)
        {
          extensions_string += ' ';
          extensions_string += swap_chain_extension_string;
        }
        
          //����������� ����� ������ OpenGL
          
        detect_opengl_bugs (extensions_string);
        
          //��������� ������ ��������� ����������

        extensions.SetGroup (extensions_string.c_str (), true);

          //����������� ��������� ������
          
        static Extension Version_1_1 ("GL_VERSION_1_1"), Version_1_2 ("GL_VERSION_1_2"), Version_1_3 ("GL_VERSION_1_3"),
                         Version_1_4 ("GL_VERSION_1_4"), Version_1_5 ("GL_VERSION_1_5"), Version_2_0 ("GL_VERSION_2_0"),
                         Version_2_1 ("GL_VERSION_2_1");

        extensions.Set (Version_1_1, GLEW_VERSION_1_1 != 0);
        extensions.Set (Version_1_2, GLEW_VERSION_1_2 != 0);
        extensions.Set (Version_1_3, GLEW_VERSION_1_3 != 0);
        extensions.Set (Version_1_4, GLEW_VERSION_1_4 != 0);
        extensions.Set (Version_1_5, GLEW_VERSION_1_5 != 0);
        extensions.Set (Version_2_0, GLEW_VERSION_2_0 != 0);
        extensions.Set (Version_2_1, GLEW_VERSION_2_1 != 0);        

          //������������� ��������� �������������� ������������ ���������
        context_caps.Init (extensions);
      }
      catch (common::Exception& exception)
      {
        exception.Touch ("render::low_level::opengl::ContextImpl::Init");
        
        throw;        
      }
    }

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;   //��������� �� ������� ������  
    
  private:
    Context          context;                        //�������� OpenGL
    SwapChainPtr     master_swap_chain;              //������� ������� ������, ��������� � ����������
    ExtensionSet     extensions;                     //����� �������������� ����������
    stl::string      extensions_string;              //������ �������������� ����������
    stl::string      version_string;                 //������ OpenGL
    stl::string      vendor_string;                  //������������� ���������� OpenGL
    stl::string      renderer_string;                //��� ���������� ��������� OpenGL
    ContextDataTable context_data_table [Stage_Num]; //������� ��������� ������ ���������
    ContextCaps      context_caps;                   //��������� �������������� ����������� ���������
};

}

/*
===================================================================================================
    �������� ���������� ContextManager
===================================================================================================
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  public:  
    Impl (const LogHandler& in_log_handler, const char* init_string) :
      log_handler (in_log_handler),
      current_context (0),
      current_draw_swap_chain (0),
      current_read_swap_chain (0),
      current_context_id (0),
      next_context_id (1),
      check_gl_errors (true),
      on_destroy_draw_swap_chain (xtl::bind (&Impl::RestoreContext, this)),
      on_destroy_read_swap_chain (xtl::bind (&Impl::RestoreContext, this))
    {
      if (!init_string)
        RaiseNullArgument ("render::low_level::opengl::ContextManager::ContextManager", "init_string");          
        
      enabled_extensions.Set (true);
      
      common::parse_init_string (init_string, xtl::bind (&Impl::AddInitProperty, this, _1, _2));
    }    
    
      //�������� ���������
    size_t CreateContext (ISwapChain* swap_chain)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateContext";         
      
      if (!swap_chain)
        RaiseNullArgument (METHOD_NAME, "swap_chain");        

      ContextImplPtr new_context;      

      if (context_map.empty ()) new_context = ContextImplPtr (new ContextImpl (swap_chain), false);
      else                      new_context = ContextImplPtr (new ContextImpl (swap_chain, context_map.begin ()->second->GetContext ()), false);
      
        //�������� ��������� ��������� ���������� � ������
      
      ExtensionSet& context_extensions = new_context->GetExtensions ();

      context_extensions &= enabled_extensions;
      
      for (size_t id=0, count=ExtensionSet::Size (); id<count; id++)
        if (required_extensions.Get (id) && !context_extensions.Get (id))
        {
          RaiseNotSupported (METHOD_NAME, "Could not create new context. Reason: required extension '%s' not supported",
            get_extension_name (id));
        }        
        
      if (!min_version.empty () && strcmp (new_context->GetVersionString (), min_version.c_str ()) < 0)
      {
        RaiseNotSupported (METHOD_NAME, "Could not create new context. Reason: OpenGL version '%s' not supported "
          "(version='%s')", min_version.c_str (), new_context->GetVersionString ());
      }      

      if (!max_version.empty ())
      {
        static const char*  version_string [] = {"1.1", "1.2", "1.3", "1.4", "1.5", "2.0", "2.1"};        
        static const size_t versions_count    = sizeof (version_string) / sizeof (*version_string);

        static Extension extension [] = {
          Extension ("GL_VERSION_1_1"),
          Extension ("GL_VERSION_1_2"),
          Extension ("GL_VERSION_1_3"),
          Extension ("GL_VERSION_1_4"),
          Extension ("GL_VERSION_1_5"),
          Extension ("GL_VERSION_2_0"),
          Extension ("GL_VERSION_2_1"),
        };

        for (size_t i=0; i<versions_count; i++)
        {
          if (strcmp (max_version.c_str (), version_string [i]) < 0)
            context_extensions.Set (extension [i], false);
        }
      }

        //����������� ���������

      size_t new_context_id = next_context_id;

      context_map.insert_pair (new_context_id, new_context);

      next_context_id++;

      return new_context_id;
    }
    
      //�������� ���������
    void DeleteContext (size_t context_id)
    {
      context_map.erase (context_id);

      if (context_id == current_context_id)
        RestoreContext ();
    }
    
      //��������� �������� ���������
    void SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::SetContext";
      
      if (context_id == current_context_id && draw_swap_chain == current_draw_swap_chain && read_swap_chain == current_read_swap_chain)
        return;
        
      if (!context_id && !draw_swap_chain)
        RaiseNullArgument (METHOD_NAME, "draw_swap_chain && context_id");
        
      if (!context_id) //����� ������������ ���������
      {
        try
        {
          context_id = GetCompatibleContextId (draw_swap_chain);
        }
        catch (common::Exception& exception)
        {
          exception.Touch (METHOD_NAME);
          
          throw;
        }
      }

        //����� ��������� �� ��� ��������������
        
      ContextMap::iterator iter = context_map.find (context_id);
      
      if (iter == context_map.end ())
        RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

      ContextImplPtr context = iter->second;
      
      if (!draw_swap_chain) //������������� ��������� ������� ������
        draw_swap_chain = context->GetMasterSwapChain ();
        
        //���� ������� ������ �� �������
        
      if (!read_swap_chain)
        read_swap_chain = draw_swap_chain;        

        //�������� ������������� ��������� � ��������� ������

      if (!context->GetContext ().IsCompatible (draw_swap_chain))
        RaiseInvalidArgument (METHOD_NAME, "draw_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

      if (draw_swap_chain != read_swap_chain && !context->GetContext ().IsCompatible (read_swap_chain))
        RaiseInvalidArgument (METHOD_NAME, "read_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

        //����������� ������������ �������� ������� ������

      draw_swap_chain->RegisterDestroyHandler (on_destroy_draw_swap_chain);
      read_swap_chain->RegisterDestroyHandler (on_destroy_read_swap_chain);

        //���������� ������

      current_context         = &*iter->second;
      current_draw_swap_chain = draw_swap_chain;
      current_read_swap_chain = read_swap_chain;
      current_context_id      = context_id;
    }
    
      //��������� �������� ���������
    void MakeContextCurrent (bool clear_errors)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::MakeContextCurrent";

      if (!current_context)
        RaiseInvalidOperation (METHOD_NAME, "Null active context");

      try
      {
        current_context->GetContext ().MakeCurrent (current_draw_swap_chain, current_read_swap_chain);
        
          //������� ������� ������
          
        if (clear_errors)
          ClearErrors ();
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);

        throw;
      }
    }    
    
      //��������� �������� ��������� � ������� ������
    ISwapChain*  GetDrawSwapChain () const { return current_draw_swap_chain; }
    ISwapChain*  GetReadSwapChain () const { return current_read_swap_chain; }
    size_t       GetContextId     () const { return current_context_id; }
    ContextImpl* GetContext       () const { return current_context; }
    
      //�������� ������� ������ ����������� � ����������
    ISwapChain* CreateCompatibleSwapChain (size_t context_id)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(size_t)";

        //����� ���������

      ContextMap::iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

      ContextImplPtr context = iter->second;
      
        //�������� ������� ������
        
      return SwapChainManager::CreatePBuffer (context->GetMasterSwapChain ());
    }
    
      //�������� ����������� ������� ������
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain)
    {
      if (!swap_chain)
        RaiseNullArgument ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*)", "swap_chain");
      
      return SwapChainManager::CreatePBuffer (swap_chain);
    }
    
      //�������� ����������� ������� ������
    ISwapChain* CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
    {
      if (!swap_chain)
        RaiseNullArgument ("render::low_level::opengl::ContextManager::CreateCompatibleSwapChain(ISwapChain*,const SwapChainDesc&)", "swap_chain");
      
      return SwapChainManager::CreatePBuffer (swap_chain, desc);
    }    
    
      //�������� ������������� ��������� � ������� ������
    bool IsCompatible (size_t context_id, ISwapChain* swap_chain)
    {
        //����� ���������

      ContextMap::iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        return false;

      return iter->second->GetContext ().IsCompatible (swap_chain);
    }
    
      //����������� ��������� ���������� ����������
    bool IsSupported (size_t context_id, const Extension& extension) const
    {
        //����� ���������

      ContextMap::const_iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        return false;
        
      return iter->second->GetExtensions ().Get (extension);
    }
      
    bool IsSupported (const Extension& extension) const
    {
      if (!current_context)
        return false;
        
      return current_context->GetExtensions ().Get (extension);
    }
    
      //����������������
    void LogPrint (const char* message)
    {
      try
      {
        log_handler (message);
      }
      catch (...)
      {
        //��������� ��� ����������
      }
    }
    
      //��������� / ���������� �������� ������
    void SetValidationState (bool state)
    {
      check_gl_errors = state;
    }
    
    bool GetValidationState () const { return check_gl_errors; }
    
      //�������� ������ OpenGL
    void CheckErrors (const char* source)
    {
      if (!check_gl_errors)
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
          Raise<OpenGLException> (source, "OpenGL error: invalid enum");
          break;
        case GL_INVALID_VALUE:
          Raise<OpenGLException> (source, "OpenGL error: invalid value");
          break;
        case GL_INVALID_OPERATION:
          Raise<OpenGLException> (source, "OpenGL error: invalid operation");
          break;
        case GL_STACK_OVERFLOW:
          Raise<OpenGLException> (source, "OpenGL error: stack overflow");
          break;
        case GL_STACK_UNDERFLOW:
          Raise<OpenGLException> (source, "OpenGL error: stack underflow");
          break;
        case GL_OUT_OF_MEMORY:
          Raise<OpenGLException> (source, "OpenGL error: out of memory");
          break;
        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
          Raise<OpenGLException> (source, "OpenGL error: invalid framebuffer operation");
          break;
        default:
          Raise<OpenGLException> (source, "OpenGL error: code=0x%04x", error);
          break;
      }      
    }
    
    void ClearErrors ()
    {
      if (!check_gl_errors)
        return;

      while (glGetError () != GL_NO_ERROR);
    }
    
  private:
      //���������� ��������
    void AddInitProperty (const char* name, const char* value)
    {
      if (!common::string_wrappers::stricmp (name, "require"))
      {
        required_extensions.SetGroup (value, true);
      }
      else if (!common::string_wrappers::stricmp (name, "disable"))
      {     
        enabled_extensions.SetGroup (value, false);
      }
      else if (!common::string_wrappers::stricmp (name, "enable"))
      {
        enabled_extensions.SetGroup (value, true);
      }
      else if (!common::string_wrappers::strnicmp (name, "gl_", 3))
      {
        if (!strcmp (value, "0"))
        {
          enabled_extensions.Set (name, false);
        }
        else if (!strcmp (value, "1"))
        {
          enabled_extensions.Set (name, true);
        }
        else if (!strcmp (value, "!"))
        {
          required_extensions.Set (name, true);
        }
        else if (!strcmp (value, "1!"))
        {
          enabled_extensions.Set (name, true);
          required_extensions.Set (name, true);
        }
      }
      else if (!common::string_wrappers::stricmp (name, "min_version"))
      {
        min_version = value;
      }
      else if (!common::string_wrappers::stricmp (name, "max_version"))
      {
        max_version = value;
      }
      else if (!stricmp (name, "check_gl_errors"))
      {
        check_gl_errors = atoi (value) != 0;
      }
    }
    
      //����� ���������, ������������ � ��������� �������� ������
    size_t GetCompatibleContextId (ISwapChain* swap_chain)
    {
        //�������� �������� �������� ����� �������� ���������, ���� ��� ��������
      
      if (current_context && current_context->GetContext ().IsCompatible (swap_chain))
        return current_context_id;
        
        //����� ��������� ����� ������������
        
      for (ContextMap::iterator iter=context_map.begin (), end=context_map.end (); iter != end; ++iter)
        if (iter->second->GetContext ().IsCompatible (swap_chain))
          return iter->first;
          
        //����������� �������� �� ������
        
      RaiseNotSupported ("render::low_level::opengl::ContextManager::Impl::GetCompatibleContextId",
        "No context compatible with swap_chain");

      return 0;
    }
    
      //������������ ��������� (����� �������� ��������� ��� ������� ������)
    void RestoreContext ()
    {
        //���� ������� �������� �� ����� - ������������� � �������� ������� ������� ������ �������� ������� ������ ���������
      
      ContextMap::iterator iter = context_map.find (current_context_id);

      if (iter != context_map.end ())
      {
        current_draw_swap_chain = current_read_swap_chain = current_context->GetMasterSwapChain ();

        return;
      }
      
        //���� ��� ����� ������� ��������
        
      if (context_map.empty ())
      {
          //���� ������� ��� ��������� - ������������ �� ��������

        current_draw_swap_chain = 0;
        current_read_swap_chain = 0;
        current_context         = 0;
        current_context_id      = 0;

        return;
      }

        //�������� � �������� �������� ������ �������� �� ������ ��������� ������ � ��� �������� �������� ������

      ContextImplPtr context = context_map.begin ()->second;

      current_context         = &*context;
      current_read_swap_chain = context->GetMasterSwapChain ();
      current_draw_swap_chain = current_read_swap_chain;
      current_context_id      = context_map.begin ()->first;
    }    
    
  private:
    typedef xtl::intrusive_ptr<ContextImpl>       ContextImplPtr;  //��������� �� ���������� ���������
    typedef stl::hash_map<size_t, ContextImplPtr> ContextMap;      //����� ������������ ��������� OpenGL � ������� ������
  
  private:
    LogHandler                log_handler;                //���������� ����������������
    ExtensionSet              required_extensions;        //����������, ������������� � ������ ������������
    ExtensionSet              enabled_extensions;         //����������, ����������� � ������������� � ������ ������������
    stl::string               min_version;                //����������� ��������� ������ OpenGL
    stl::string               max_version;                //������������ ����������� ������ OpenGL
    bool                      check_gl_errors;            //����� �� ��������� ������ OpenGL
    ContextMap                context_map;                //����� ����������� ������� ������ �� ��������
    ContextImpl*              current_context;            //������� ��������
    ISwapChain*               current_draw_swap_chain;    //������� ������ ��� ���������
    ISwapChain*               current_read_swap_chain;    //������� ������ ��� ������
    size_t                    current_context_id;         //������������� �������� ��������� (0 - �����������)
    size_t                    next_context_id;            //����� ���������� ������������ ���������
    xtl::trackable::slot_type on_destroy_draw_swap_chain; //���������� �������� ������� ������ ��� ��������� 
    xtl::trackable::slot_type on_destroy_read_swap_chain; //���������� �������� ������� ������ ��� ������  
};

/*
    ������������ / ���������� / ������������
*/

ContextManager::ContextManager (const LogHandler& log_handler, const char* init_string)
  : impl (new Impl (log_handler, init_string))
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
    �������� / �������� ���������
*/

size_t ContextManager::CreateContext (ISwapChain* swap_chain)
{
  return impl->CreateContext (swap_chain);
}

void ContextManager::DeleteContext (size_t context_id)
{
  impl->DeleteContext (context_id);
}

/*
    �������� ������� ������ ����������� � ����������
*/

ISwapChain* ContextManager::CreateCompatibleSwapChain (size_t context_id)
{
  return impl->CreateCompatibleSwapChain (context_id);
}

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain)
{
  return impl->CreateCompatibleSwapChain (swap_chain);
}

ISwapChain* ContextManager::CreateCompatibleSwapChain (ISwapChain* swap_chain, const SwapChainDesc& desc)
{
  return impl->CreateCompatibleSwapChain (swap_chain, desc);
}

/*
    ����� �������� ���������
*/

void ContextManager::SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
{
  impl->SetContext (context_id, draw_swap_chain, read_swap_chain);
}

ISwapChain* ContextManager::GetDrawSwapChain () const
{
  return impl->GetDrawSwapChain ();
}

ISwapChain* ContextManager::GetReadSwapChain () const
{
  return impl->GetReadSwapChain ();
}

size_t ContextManager::GetContextId () const
{
  return impl->GetContextId ();
}

/*
    ��������� �������� ���������
*/

void ContextManager::MakeContextCurrent () const
{
  impl->MakeContextCurrent (true);
}

/*
    ������ � ��������� ��������� ������ ���������
*/

const ContextDataTable& ContextManager::GetContextDataTable (Stage table_id) const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetContextDataTable", "Null active context");

  return impl->GetContext ()->GetContextDataTable (table_id);
}

ContextDataTable& ContextManager::GetContextDataTable (Stage table_id)
{
  return const_cast<ContextDataTable&> (const_cast<const ContextManager&> (*this).GetContextDataTable (table_id));
}

/*
    ��������� ���������� � ������� ���������� OpenGL
*/

const char* ContextManager::GetExtensions () const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetExtensions", "Null active context");
    
  return impl->GetContext ()->GetExtensionsString ();
}

const char* ContextManager::GetVersion () const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetVersion", "Null active context");
    
  return impl->GetContext ()->GetVersionString ();
}

const char* ContextManager::GetVendor () const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetVendor", "Null active context");

  return impl->GetContext ()->GetVendorString ();
}

const char* ContextManager::GetRenderer () const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetRenderer", "Null active context");

  return impl->GetContext ()->GetRendererString ();
}

/*
    �������� ������������� ��������� � ������� ������
*/

bool ContextManager::IsCompatible (size_t context_id, ISwapChain* swap_chain) const
{
  return impl->IsCompatible (context_id, swap_chain);
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

bool ContextManager::IsSupported (size_t context_id, const Extension& extension) const
{
  return impl->IsSupported (context_id, extension);
}

bool ContextManager::IsSupported (const Extension& extension) const
{
  return impl->IsSupported (extension);
}

/*
   ��������� ��������� �������������� ������������ ���������
*/

const ContextCaps& ContextManager::GetCaps () const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetCaps", "Null active context");
    
  return impl->GetContext ()->GetCaps ();
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
  if (!format)
    return;

  impl->LogPrint (common::vformat (format, args).c_str ());
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
  
  RaiseInvalidOperation (source, "Invalid operation");
}

void ContextManager::ClearErrors () const
{
  impl->ClearErrors ();
}
