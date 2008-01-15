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
typedef stl::bitset<GlExtension_Num>                    ExtensionsFlags; //����� ��������� ����������

//���������� ������� ������ �������������� ����������
void set_extensions_flags (const char* extensions, ExtensionsFlags& flags, bool value)
{
  stl::string mask;

  for (const char* pos=extensions; *pos;)
  {
    for (;*pos && *pos==' '; ++pos);
    
    const char* first    = pos;
    bool        wildcard = false;
    
    for (bool loop=true; loop; ++pos)
      switch (*pos)
      {
        case '\0':
        case ' ':
          loop = false;
          break;
        case '?':
        case '*':
          wildcard = true;
        default:
          break;
      }
      
    if (!wildcard)
    {
      GlExtension extension_id = get_extension_id (first, size_t (pos-first-1));
      
      if (extension_id == GlExtension_Unknown)
        continue;

      flags [extension_id] = true;            
    }
    else
    {            
      mask.assign (first, size_t (pos - first - 1));

      if (!strcmp (mask.c_str (), "*")) //��������� ������������ ������ ������� / ��������� ���� ������
      {
        if (value) flags.set ();    
        else       flags.reset ();        
      }
      else
      {      
        for (int id=0; id<GlExtension_Num; id++)
          if (wcimatch (get_extension_name ((GlExtension)id), mask.c_str ()))
            flags.set (id, value);
      }
    }    
  }
}

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
    const Context&  GetContext () const { return context; }    
    ISwapChain*     GetMasterSwapChain () const { return master_swap_chain.get (); }
    
      //��������� ������ ��������� ����������
          ExtensionsFlags& GetExtensions ()       { return extensions; }      
    const ExtensionsFlags& GetExtensions () const { return extensions; }
    
      //��������� ������ OpenGL
    const char* GetVersion () const { return version.c_str (); }
       
      //��������� ��������� ������
    void SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::SetContextData";
      
      if (table_id < 0 || table_id >= ContextDataTable_Num)
        RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);
        
      if (element_id >= CONTEXT_DATA_TABLE_SIZE)
        RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);
        
      data [table_id][element_id] = value;
    }

      //������ ��������� ������
    size_t GetContextData (ContextDataTable table_id, size_t element_id)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::GetContextData";

      if (table_id < 0 || table_id >= ContextDataTable_Num)
        RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);

      if (element_id >= CONTEXT_DATA_TABLE_SIZE)
        RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);

      return data [table_id][element_id];
    }
    
      //������� ������� ��������� ������
    void ClearContextData (ContextDataTable table_id)
    {
      if (table_id < 0 || table_id >= ContextDataTable_Num)
        RaiseInvalidArgument ("render::low_level::opengl::ContextManager::ClearContextData", "table_id", table_id);

      memset (data [table_id], 0, sizeof (data [table_id]));
    }
    
      //������� ���� ������ ��������� ������
    void ClearContextData ()
    {
      memset (data, 0, sizeof (data));
    }  
    
  private:

      //�������������
    void Init ()        
    {
        //������� ������� ��������� ������ ���������
      
      memset (data, 0, sizeof (data));
      
        //����� ��������� ���������
      
      context.MakeCurrent (master_swap_chain.get ());
      
        //����������� ��������� ����������        

      const char* supported_extensions = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));

      set_extensions_flags (supported_extensions, extensions, true);
      
        //����������� ��������� ������
        
      extensions [GlExtension_Version_1_1] = GLEW_VERSION_1_1 != 0;
      extensions [GlExtension_Version_1_2] = GLEW_VERSION_1_2 != 0;
      extensions [GlExtension_Version_1_3] = GLEW_VERSION_1_3 != 0;
      extensions [GlExtension_Version_1_4] = GLEW_VERSION_1_4 != 0;
      extensions [GlExtension_Version_1_5] = GLEW_VERSION_1_5 != 0;
      extensions [GlExtension_Version_2_0] = GLEW_VERSION_2_0 != 0;
      extensions [GlExtension_Version_2_1] = GLEW_VERSION_2_1 != 0;
      
      version = reinterpret_cast<const char*> (glGetString (GL_VERSION));
    }

  private:
    typedef size_t                   ContextData [ContextDataTable_Num][CONTEXT_DATA_TABLE_SIZE];
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;   //��������� �� ������� ������  
    
  private:
    Context         context;           //�������� OpenGL
    SwapChainPtr    master_swap_chain; //������� ������� ������, ��������� � ����������
    ExtensionsFlags extensions;        //����� �������������� ����������
    stl::string     version;           //��������� ������ OpenGL
    ContextData     data;              //��������� ������ ��������� 
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
      on_destroy_draw_swap_chain (xtl::bind (&Impl::RestoreContext, this)),
      on_destroy_read_swap_chain (xtl::bind (&Impl::RestoreContext, this))
    {
      if (!init_string)
        RaiseNullArgument ("render::low_level::opengl::ContextManager::ContextManager", "init_string");          
        
      enabled_extensions.set ();
      
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
      
      ExtensionsFlags& context_extensions = new_context->GetExtensions ();

      context_extensions &= enabled_extensions;
      
      for (int i=0; i<GlExtension_Num; i++)
        if (required_extensions [i] && !context_extensions [i])
        {
          RaiseNotSupported (METHOD_NAME, "Could not create new context. Reason: required extension '%s' not supported",
            get_extension_name ((GlExtension)i));
        }
        
      if (!min_version.empty () && strcmp (new_context->GetVersion (), min_version.c_str ()) < 0)
      {
        RaiseNotSupported (METHOD_NAME, "Could not create new context. Reason: OpenGL version '%s' not supported "
          "(version='%s')", min_version.c_str (), new_context->GetVersion ());
      }

      if (!max_version.empty ())
      {
        static const char*  versions []    = {"1.1", "1.2", "1.3", "1.4", "1.5", "2.0", "2.1"};
        static const size_t versions_count = sizeof (versions) / sizeof (*versions);

        for (size_t i=0; i<versions_count; i++)
        {
          GlExtension id = (GlExtension)(GlExtension_Version_1_1 + i);

          if (context_extensions [id] && strcmp (max_version.c_str (), versions [i]) < 0)
            context_extensions [id] = false;
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

        //����� ���������
        
      ContextMap::iterator iter = context_map.find (context_id);
      
      if (iter == context_map.end ())
        RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

      ContextImplPtr context = iter->second;

        //�������� ������������� ��������� � ��������� ������

      if (!context->GetContext ().IsCompatible (draw_swap_chain))
        RaiseInvalidArgument (METHOD_NAME, "draw_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

      if (draw_swap_chain != read_swap_chain && !context->GetContext ().IsCompatible (read_swap_chain))
        RaiseInvalidArgument (METHOD_NAME, "read_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

        //����������� ������������ �������� ������� ������

      Trackable *draw_swap_chain_trackable = cast_object<Trackable> (draw_swap_chain, METHOD_NAME, "draw_swap_chain"),
                *read_swap_chain_trackable = cast_object<Trackable> (read_swap_chain, METHOD_NAME, "read_swap_chain");

      draw_swap_chain_trackable->RegisterDestroyHandler (on_destroy_draw_swap_chain);
      read_swap_chain_trackable->RegisterDestroyHandler (on_destroy_read_swap_chain);

        //���������� ������

      current_context         = &*iter->second;
      current_draw_swap_chain = draw_swap_chain;
      current_read_swap_chain = read_swap_chain;
      current_context_id      = context_id;
    }
    
      //��������� �������� ���������
    void MakeContextCurrent ()
    {
      static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::MakeContextCurrent";

      if (!current_context)
        RaiseInvalidOperation (METHOD_NAME, "Null active context");

      try
      {
        current_context->GetContext ().MakeCurrent (current_draw_swap_chain, current_read_swap_chain);
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);

        throw;
      }
    }
    
      //�������� �������� �� ������� �������� ��������
    bool IsContextCurrent ()
    {
      return current_context && current_context->GetContext ().IsCurrent (current_draw_swap_chain, current_read_swap_chain);
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
    bool IsSupported (size_t context_id, GlExtension extension_id) const
    {
      if (extension_id < 0 || extension_id >= GlExtension_Num)
        return false;
      
        //����� ���������

      ContextMap::const_iterator iter = context_map.find (context_id);

      if (iter == context_map.end ())
        return false;
        
      return iter->second->GetExtensions ()[extension_id];
    }
      
    bool IsSupported (GlExtension extension_id) const
    {
      if (!current_context || extension_id < 0 || extension_id >= GlExtension_Num)
        return false;
        
      return current_context->GetExtensions ()[extension_id];
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
    
  private:
      //���������� ��������
    void AddInitProperty (const char* name, const char* value)
    {
      if (!common::string_wrappers::stricmp (name, "require"))
      {
        set_extensions_flags (value, required_extensions, true);
      }
      else if (!common::string_wrappers::stricmp (name, "disable"))
      {     
        set_extensions_flags (value, enabled_extensions, false);
      }
      else if (!common::string_wrappers::stricmp (name, "enable"))
      {     
        set_extensions_flags (value, enabled_extensions, true);
      }
      else if (!common::string_wrappers::strnicmp (name, "gl_", 3))
      {
        bool flag = atoi (value) != 0;

        set_extensions_flags (name, flag ? required_extensions : enabled_extensions, flag);
      }
      else if (!common::string_wrappers::stricmp (name, "min_version"))
      {
        min_version = value;
      }
      else if (!common::string_wrappers::stricmp (name, "max_version"))
      {
        max_version = value;
      }
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
    LogHandler          log_handler;                //���������� ����������������
    ExtensionsFlags     required_extensions;        //����������, ������������� � ������ ������������
    ExtensionsFlags     enabled_extensions;         //����������, ����������� � ������������� � ������ ������������
    stl::string         min_version;                //����������� ��������� ������ OpenGL
    stl::string         max_version;                //������������ ����������� ������ OpenGL
    ContextMap          context_map;                //����� ����������� ������� ������ �� ��������
    ContextImpl*        current_context;            //������� ��������
    ISwapChain*         current_draw_swap_chain;    //������� ������ ��� ���������
    ISwapChain*         current_read_swap_chain;    //������� ������ ��� ������
    size_t              current_context_id;         //������������� �������� ��������� (0 - �����������)
    size_t              next_context_id;            //����� ���������� ������������ ���������
    Trackable::SlotType on_destroy_draw_swap_chain; //���������� �������� ������� ������ ��� ��������� 
    Trackable::SlotType on_destroy_read_swap_chain; //���������� �������� ������� ������ ��� ������  
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
  impl->MakeContextCurrent ();
}

bool ContextManager::IsContextCurrent () const
{
  return impl->IsContextCurrent ();
}

/*
    ������ � �������� ��������� ������ ���������
*/

void ContextManager::SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::SetContextData", "Null active context");

  impl->GetContext ()->SetContextData (table_id, element_id, value);
}

size_t ContextManager::GetContextData (ContextDataTable table_id, size_t element_id) const
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetContextData", "Null active context");

  return impl->GetContext ()->GetContextData (table_id, element_id);
}

void ContextManager::ClearContextData (ContextDataTable table_id)
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");
    
  impl->GetContext ()->ClearContextData (table_id);
}

void ContextManager::ClearContextData ()
{
  if (!impl->GetContext ())
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");

  impl->GetContext ()->ClearContextData ();
}

/*
    ��������� ������ �� ������� ����������, ��������� �� �������� ������� ������
*/

const char* ContextManager::GetSwapChainsExtensionString () const
{
  MakeContextCurrent ();

  return Context::GetSwapChainExtensionString ();
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

bool ContextManager::IsSupported (size_t context_id, GlExtension extension_id) const
{
  return impl->IsSupported (context_id, extension_id);
}

bool ContextManager::IsSupported (GlExtension extension_id) const
{
  return impl->IsSupported (extension_id);
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

void ContextManager::CheckErrors (const char* source) const
{
  if (!source)
    source = "render::low_level::ContextManager::CheckErrors";
    
  MakeContextCurrent ();
  
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
    default:
      Raise<OpenGLException> (source, "OpenGL error: code=%d", error);
      break;
  }
}

void ContextManager::RaiseError (const char* source) const
{
  if (!source)
    return;
    
  CheckErrors (source);
  
  RaiseInvalidOperation (source, "Invalid operation");
}
