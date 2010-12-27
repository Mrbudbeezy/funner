#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
===================================================================================================
    ����� ����� GLX ��������
===================================================================================================
*/

typedef GLXContext   (*glXCreateNewContextFn)       (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
typedef void         (*glXDestroyContextFn)         (Display *dpy, GLXContext ctx);
typedef Bool         (*glXMakeContextCurrentFn)     (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef GLXPbuffer   (*glXCreatePbufferFn)          (Display *dpy, GLXFBConfig config, const int *attrib_list);
typedef void         (*glXDestroyPbufferFn)         (Display *dpy, GLXPbuffer pbuf);
typedef void         (*glXSwapBuffersFn)            (Display *dpy, GLXDrawable drawable);
typedef Bool         (*glXIsDirectFn)               (Display *dpy, GLXContext ctx);
typedef const char*  (*glXGetClientStringFn)        (Display *dpy, int name);
typedef Bool         (*glXQueryVersionFn)           (Display *dpy, int *major, int *minor);
typedef const char*  (*glXQueryServerStringFn)      (Display *dpy, int screen, int name);
typedef int          (*glXGetFBConfigAttribFn)      (Display *dpy, GLXFBConfig config, int attribute, int *value);
typedef GLXFBConfig* (*glXGetFBConfigsFn)           (Display *dpy, int screen, int *nelements);
typedef const char*  (*glXQueryExtensionsStringFn)  (Display *dpy, int screen);
typedef void*        (*glXGetProcAddressFn)         (const char *procName);
typedef GLXContext   (*glXGetCurrentContextFn)      (void);
typedef Display*     (*glXGetCurrentDisplayFn)      (void);
typedef GLXDrawable  (*glXGetCurrentDrawableFn)     (void);
typedef GLXDrawable  (*glXGetCurrentReadDrawableFn) (void);

/*
===================================================================================================
    ������������ ����������
===================================================================================================
*/

namespace render
{

namespace low_level
{

namespace opengl
{

namespace glx
{

class DynamicLibrary
{
  public:
///�����������
    DynamicLibrary (const char* in_path)
      : library (0)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::glx::DynamicLibrary::DynamicLibrary";

        //�������� ������������ ����������

      if (!in_path)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path");
        
      path = in_path;
      
        //�������� ����������
        
      size_t dlopen_flags = RTLD_NOW | RTLD_GLOBAL;

      library = dlopen (in_path, dlopen_flags);

      if (!library)
        raise_error (common::format ("::dlopen('%S', %lu)", in_path, dlopen_flags).c_str ());
    }
    
///����������
    ~DynamicLibrary ()
    {
      //if (dlclose (library))
      //  raise_error ("::dlclose");
    }
    
///��������� ����������� ����������
    void* GetLibrary ()
    {
      return library;
    }
    
///��������� ���� � ����������
    const char* GetPath ()
    {
      return path.c_str ();
    }
    
///��������� ����� �����
    void* GetSymbol (const char* symbol_name)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::glx::DynamicLibrary::GetSymbol";
      
      if (!symbol_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "symbol_name");

      void* address = dlsym (library, symbol_name);

      return address;
    }

  private:
    void*       library; //���������� ����������
    stl::string path;    //���� � ����������
};

typedef stl::auto_ptr<DynamicLibrary> DynamicLibraryPtr;

}

}

}

}

/*
===================================================================================================
    GLX ����������
===================================================================================================
*/

struct AdapterLibrary::Impl
{
  Log                    log;             //��������
  DynamicLibraryPtr      dll;             //������������ ����������
  IContextLostListener  *listener;        //��������� ������� ������ ���������
  AdapterLibrary        *prev;            //���������� ����������
  AdapterLibrary        *next;            //��������� ����������
  static AdapterLibrary *first;           //������ ����������
  
  glXCreateNewContextFn       fglXCreateNewContext;
  glXDestroyContextFn         fglXDestroyContext;
  glXMakeContextCurrentFn     fglXMakeContextCurrent;
  glXCreatePbufferFn          fglXCreatePbuffer;
  glXDestroyPbufferFn         fglXDestroyPbuffer;
  glXSwapBuffersFn            fglXSwapBuffers;
  glXGetCurrentContextFn      fglXGetCurrentContext;
  glXGetCurrentDisplayFn      fglXGetCurrentDisplay;
  glXGetCurrentDrawableFn     fglXGetCurrentDrawable;
  glXGetCurrentReadDrawableFn fglXGetCurrentReadDrawable;
  glXIsDirectFn               fglXIsDirect;
  glXGetClientStringFn        fglXGetClientString;
  glXQueryVersionFn           fglXQueryVersion;
  glXQueryServerStringFn      fglXQueryServerString;
  glXGetProcAddressFn         fglXGetProcAddress;
  glXGetFBConfigAttribFn      fglXGetFBConfigAttrib;
  glXGetFBConfigsFn           fglXGetFBConfigs;
  glXQueryExtensionsStringFn  fglXQueryExtensionsString;

///�����������
  Impl (DynamicLibraryPtr& in_dll)
    : dll (in_dll)
    , listener (0)
  {
    try
    {
        //����������� ����������
      
      log.Printf ("...get default GLX-entries");
      
        //������������� ����� �����
        
      GetSymbol ("glXCreateNewContext",       fglXCreateNewContext);
      GetSymbol ("glXDestroyContext",         fglXDestroyContext);
      GetSymbol ("glXMakeContextCurrent",     fglXMakeContextCurrent);
      GetSymbol ("glXCreatePbuffer",          fglXCreatePbuffer);
      GetSymbol ("glXDestroyPbuffer",         fglXDestroyPbuffer);
      GetSymbol ("glXSwapBuffers",            fglXSwapBuffers);
      GetSymbol ("glXGetCurrentDisplay",      fglXGetCurrentDisplay);
      GetSymbol ("glXGetCurrentDrawable",     fglXGetCurrentDrawable);
      GetSymbol ("glXGetCurrentReadDrawable", fglXGetCurrentReadDrawable);
      GetSymbol ("glXIsDirect",               fglXIsDirect);
      GetSymbol ("glXGetClientString",        fglXGetClientString);
      GetSymbol ("glXQueryVersion",           fglXQueryVersion);
      GetSymbol ("glXQueryServerString",      fglXQueryServerString);
      GetSymbol ("glXGetProcAddress",         fglXGetProcAddress);
      GetSymbol ("glXGetFBConfigAttrib",      fglXGetFBConfigAttrib);
      GetSymbol ("glXGetFBConfigs",           fglXGetFBConfigs);
      GetSymbol ("glXQueryExtensionsString",  fglXQueryExtensionsString);

        //����� ����� ����������
      
      Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();
      
      DisplayLock lock (display);

      int major = 0, minor = 0;

      if (!fglXQueryVersion (display, &major, &minor))
        return;

      log.Printf ("...GLX %d.%d library successfully loaded", major, minor);
      log.Printf ("...client vendor: '%s'",     fglXGetClientString (display, GLX_VENDOR));
      log.Printf ("...client version: '%s'",    fglXGetClientString (display, GLX_VERSION));
      log.Printf ("...client extensions: '%s'", fglXGetClientString (display, GLX_EXTENSIONS));
    }
    catch (xtl::exception& exception)
    {
      log.Printf ("...GLX library load failed");
      exception.touch ("render::low_level::opengl::glx::AdapterLibrary::Impl::Impl");
      throw;
    }
  }
  
  ~Impl ()
  {

  }
    
///����������� ������ ����� ����� � ����������� ����� � ��������� ������������ ����������
  template <class Fn> void GetSymbol (const char* symbol, Fn& fn, bool check = true)
  {
    fn = (Fn)dll->GetSymbol (symbol);

    if (!fn && check)
      throw xtl::format_operation_exception ("render::low_level::opengl::glx::AdapterLibrary::Impl::GetSymbol",
        "Symbol '%s' not found in library '%s'", symbol, dll->GetPath ());
  }        
    
///����� ����������
  static AdapterLibrary* FindLibrary (const DynamicLibraryPtr& dll)
  {
    void *library = dll->GetLibrary ();

    for (AdapterLibrary* i=first; i; i=i->impl->next)
      if (i->impl->dll->GetLibrary () == library)
        return i;

    return 0;
  }
};

AdapterLibrary* AdapterLibrary::Impl::first = 0;

/*
    ����������� / ����������
*/

AdapterLibrary::AdapterLibrary (DynamicLibraryPtr& in_dll)
  : impl (new Impl (in_dll))
{
  impl->next = impl->first;
  impl->prev = 0;

  if (impl->first) impl->first->impl->prev = this;

  impl->first = this;
}

AdapterLibrary::~AdapterLibrary ()
{ 
    //������ ����������� ����������
    
  if (impl->prev) impl->prev->impl->next = impl->next;
  else            impl->first            = impl->next;
  
  if (impl->next) impl->next->impl->prev = impl->prev;
}

/*
    �������� ����������
*/

AdapterLibraryPtr AdapterLibrary::LoadLibrary (const char* path)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::LibraryManager::LoadLibrary";
  
  if (!path)
    throw xtl::make_null_argument_exception (METHOD_NAME, "path");

  try
  {
    Log log;        
    
      //�������� ������������ ����������    
      
    log.Printf ("...load library '%s'", path);
      
    DynamicLibraryPtr dll (new DynamicLibrary (path));
    
      //������� ����� ��� ����������� ����������

    AdapterLibrary* library = AdapterLibrary::Impl::FindLibrary (dll);

    if (library)
    {
      log.Printf ("...library '%s' already loaded", path);
      return library;
    }

      //�������� ����� ����������
      
    return AdapterLibraryPtr (new AdapterLibrary (dll), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("%s('%s')", METHOD_NAME, path);
    throw;
  }
}

/*
    ��������� ����� ����������
*/
    
const char* AdapterLibrary::GetName ()
{
  return impl->dll->GetPath ();
}

/*
    �������� ����������� ������� ����������
*/

GLXPbuffer AdapterLibrary::CreatePbuffer (Display *dpy, GLXFBConfig config, const int *attrib_list)
{
  try
  {
    GLXPbuffer buf = impl->fglXCreatePbuffer (dpy, config, attrib_list);
    
    if (!buf)
      raise_error ("glXCreatePbuffer failed");
      
    return buf;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::CreatePbuffer");
    throw;
  }
}

/*
    ����������� ����������� ������� ����������
*/
    
void AdapterLibrary::DestroyPbuffer (Display *dpy, GLXPbuffer pbuf)
{
  impl->fglXDestroyPbuffer (dpy, pbuf);
}

/*
    �������� ������ ��������� GLX-����������
*/

GLXContext AdapterLibrary::CreateContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
{
  try
  {
    GLXContext ctx = impl->fglXCreateNewContext (dpy, config, render_type, share_list, direct);
    
    if (!ctx)
      raise_error ("glXCreateNewContext failed");
      
    return ctx;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::CreateContext");
    throw;
  }
}

/*
    �������� ��������� GLX
*/

void AdapterLibrary::DestroyContext (Display *dpy, GLXContext ctx)
{
  impl->fglXDestroyContext (dpy, ctx);
}

/*
    ��������� �������� ���������
*/

void AdapterLibrary::MakeCurrent (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx, IContextLostListener* new_listener)
{
    //����� �������� ���������

  if (impl->listener)
  {
    try
    {
      impl->listener->OnLostCurrent ();
    }
    catch (...)
    {
      //���������� ���� ����������
    }
  }

  impl->fglXMakeContextCurrent (dpy, draw, read, ctx);
  
  impl->listener = new_listener;
}

/*
    ��������� �������� ���������
*/
    
GLXContext AdapterLibrary::GetCurrentContext ()
{
  return impl->fglXGetCurrentContext ();
}

/*
    ��������� ������� ������� drawable
*/
    
GLXDrawable AdapterLibrary::GetCurrentDrawable ()
{
  return impl->fglXGetCurrentDrawable ();
}

/*
    ��������� ������� ������� drawable ��� ������
*/
    
GLXDrawable AdapterLibrary::GetCurrentReadDrawable ()
{
  return impl->fglXGetCurrentReadDrawable ();
}

/*
    ���������� ���������� � ������������ GLX-������ �����
*/

int AdapterLibrary::GetFBConfigAttrib (Display *dpy, GLXFBConfig config, int attribute, int *value)
{
  return impl->fglXGetFBConfigAttrib (dpy, config, attribute, value);
}

/*
    ���������� ������ ������������ GLX-������ �����, ��������������� �������� ���������
*/

GLXFBConfig* AdapterLibrary::GetFBConfigs (Display *dpy, int screen, int *nelements)
{
  return impl->fglXGetFBConfigs (dpy, screen, nelements);
}

/*
    ���������� ���������� �������� � �������� �������
*/

void AdapterLibrary::SwapBuffers (Display *dpy, GLXDrawable drawable)
{
  impl->fglXSwapBuffers (dpy, drawable);
}

/*
    ��������� ������ �������������� ����������
*/

const char* GetExtensionsString (Display *dpy, int screen)
{
  return impl->fglXQueryExtensionsString (dpy, screen);
}
    
/*
    ��������� ������ ����� �����
*/

const void* AdapterLibrary::GetProcAddress (const char* name, size_t search_flags)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::AdapterLibrary::GetProcAddress";

  if (!name)
  {
    if (search_flags & EntrySearch_NoThrow)
      return 0;

    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
  }

  const void* address = 0;

  if (!address && (search_flags & EntrySearch_Context))
    address = (void*)impl->fglXGetProcAddress (name);
    
  if (address)
    return address;
    
  if (search_flags & EntrySearch_Library)
    address = impl->dll->GetSymbol (name);

  if (address)
    return address;

  throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in GL library '%s'", name, GetName ());
}