#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
===================================================================================================
    ����� ����� GLX ��������
===================================================================================================
*/

typedef GLXContext  (*glXCreateNewContextFn)       (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
typedef void        (*glXDestroyContextFn)         (Display *dpy, GLXContext ctx);
typedef Bool        (*glXMakeContextCurrentFn)     (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef GLXPbuffer  (*glXCreatePbufferFn)          (Display *dpy, GLXFBConfig config, const int *attrib_list);
typedef void        (*glXDestroyPbufferFn)         (Display *dpy, GLXPbuffer pbuf);
typedef void        (*glXSwapBuffersFn)            (Display *dpy, GLXDrawable drawable);
typedef GLXContext  (*glXGetCurrentContextFn)      (void);
typedef Display*    (*glXGetCurrentDisplayFn)      (void);
typedef GLXDrawable (*glXGetCurrentDrawableFn)     (void);
typedef GLXDrawable (*glXGetCurrentReadDrawableFn) (void);
typedef const char* (*glXGetClientStringFn)        (Display *dpy, int name);
typedef const char* (*glXQueryServerStringFn)      (Display *dpy, int screen, int name);

/*
===================================================================================================
    ������������ ����������
===================================================================================================
*/

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
      if (dlclose (library))
        raise_error ("::dlclose");
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

/*
===================================================================================================
    GLX ����������
===================================================================================================
*/

class GlxAdapterLibrary: public IAdapterLibrary, public xtl::reference_counter
{
  public:
///�����������
    GlxAdapterLibrary (DynamicLibraryPtr& in_dll)
      : dll (in_dll)
      , listener (0)
    {
      try
      {
          //����������� ����������

        next = first;
        prev = 0;

        if (first) first->prev = this;

        first = this;
        
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
        GetSymbol ("glXGetClientString",        fglXGetClientString);
        GetSymbol ("glXQueryServerString",      fglXQueryServerString);

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
        exception.touch ("render::low_level::opengl::glx::GlxAdapterLibrary::GlxAdapterLibrary");
        throw;
      }
    }
    
    ~GlxAdapterLibrary ()
    {
      log.Printf ("...unload dll '%s'", GetName ());
      
        //������ ����������� ����������
        
      if (prev) prev->next = next;
      else      first      = next;
      
      if (next) next->prev = prev;
    }
    
///��������� ����� ����������
    const char* GetName ()
    {
      return dll->GetPath ();
    }
    
//�������� ����������� ������� ����������
    GLXPbuffer CreatePbuffer (Display *dpy, GLXFBConfig config, const int *attrib_list)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::CreatePbuffer");
    }
    
//����������� ����������� ������� ����������
    void DestroyPbuffer (Display *dpy, GLXPbuffer pbuf)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::DestroyPbuffer");
    }

//�������� ������ ��������� GLX-����������
    GLXContext CreateContext (Display *dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::CreateContext");
    }
    
//�������� ��������� GLX
    void DeleteContext (Display *dpy, GLXContext ctx)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::DeleteContext");
    }

//��������� �������� ���������
    void MakeCurrent (Display *dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx, IContextLostListener* listener)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::MakeCurrent");
    }
    
//��������� �������� ���������
    GLXContext GetCurrentContext ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentContext");
    }
    
//��������� �������� ���������� �����������
    Display* GetCurrentDisplay ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentDisplay");
    }
    
//��������� ������� ������� drawable
    GLXDrawable GetCurrentDrawable ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentDrawable");
    }
    
//��������� ������� ������� drawable ��� ������
    GLXDrawable GetCurrentReadDrawable ()
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetCurrentReadDrawable");
    }

//���������� ���������� �������� � �������� �������
    void SwapBuffers (Display *dpy, GLXDrawable drawable)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::SwapBuffers");
    }
    
///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }
    
///����� ����������
    static GlxAdapterLibrary* FindLibrary (const DynamicLibraryPtr& dll)
    {
      void *library = dll->GetLibrary ();

      for (GlxAdapterLibrary* i=first; i; i=i->next)
        if (i->dll->GetLibrary () == library)
          return i;

      return 0;
    }

///��������� ������ ����� �����
    const void* GetProcAddress (const char* name, size_t search_flags)
    {
      throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::GlxAdapterLibrary::GetProcAddress");
    }
    
  protected:
///����������� ������ ����� ����� � ����������� ����� � ��������� ������������ ����������
    template <class Fn> void GetSymbol (const char* symbol, Fn& fn, bool check = true)
    {
      fn = (Fn)dll->GetSymbol (symbol);

      if (!fn && check)
        throw xtl::format_operation_exception ("render::low_level::opengl::glx::AdapterLibrary::GetSymbol",
          "Symbol '%s' not found in library '%s'", symbol, dll->GetPath ());
    }        
    
  protected:
    Log log; //��������
    
  private:
    DynamicLibraryPtr         dll;             //������������ ����������
    IContextLostListener     *listener;        //��������� ������� ������ ���������
    GlxAdapterLibrary        *prev;            //���������� ����������
    GlxAdapterLibrary        *next;            //��������� ����������
    static GlxAdapterLibrary *first;           //������ ����������

  private:
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
    glXGetClientStringFn        fglXGetClientString;
    glXQueryServerStringFn      fglXQueryServerString;
};

GlxAdapterLibrary* GlxAdapterLibrary::first = 0;

}

/*
===================================================================================================
    �������� ����������� ����������
===================================================================================================
*/

/*
    �������� ����������
*/

AdapterLibraryPtr LibraryManager::LoadLibrary (const char* name)
{
  static const char* METHOD_NAME = "render::low_level::opengl::glx::LibraryManager::LoadLibrary";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  try
  {
    Log log;        
    
      //�������� ������������ ����������    
      
    log.Printf ("...load library '%s'", name);
      
    DynamicLibraryPtr dll (new DynamicLibrary (name));
    
      //������� ����� ��� ����������� ����������

    GlxAdapterLibrary* library = GlxAdapterLibrary::FindLibrary (dll);

    if (library)
    {    
      log.Printf ("...library '%s' already loaded", name);
      return library;
    }

      //�������� ����� ����������
      
    return AdapterLibraryPtr (new GlxAdapterLibrary (dll), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("%s('%s')", METHOD_NAME, name);
    throw;
  }
}
