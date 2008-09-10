#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

namespace
{

/*
===================================================================================================
    ����� ����� WGL ��������
===================================================================================================
*/

typedef HGLRC (WINAPI *wglCreateContextFn)       (HDC dc);
typedef BOOL  (WINAPI *wglDeleteContextFn)       (HGLRC context);
typedef int   (WINAPI *wglDescribePixelFormatFn) (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd);
typedef PROC  (WINAPI *wglGetProcAddressFn)      (LPCSTR name);
typedef BOOL  (WINAPI *wglMakeCurrentFn)         (HDC dc, HGLRC context);
typedef BOOL  (WINAPI *wglSetPixelFormatFn)      (HDC dc, int pixel_format, const PIXELFORMATDESCRIPTOR* pfd);
typedef BOOL  (WINAPI *wglSwapBuffersFn)         (HDC dc);

/*
===================================================================================================
    ����� ����� ICD ��������
===================================================================================================
*/

typedef wglCreateContextFn       DrvCreateContextFn;
typedef wglDeleteContextFn       DrvDeleteContextFn;
typedef wglDescribePixelFormatFn DrvDescribePixelFormatFn;
typedef wglGetProcAddressFn      DrvGetProcAddressFn;
typedef wglSwapBuffersFn         DrvSwapBuffersFn;

typedef HGLRC (WINAPI *DrvCreateLayerContextFn) (HDC dc, int layer);
typedef BOOL  (WINAPI *DrvSwapLayerBuffersFn)   (HDC hdc, UINT fuPlanes);

typedef BOOL  (WINAPI *DrvSetPixelFormatFn)  (HDC hdc, int pixel_format);
typedef void* (WINAPI *DrvSetContextFn)      (HDC dc, HGLRC context, void* callback);
typedef void  (WINAPI *DrvReleaseContextFn)  (HGLRC context);
typedef BOOL  (WINAPI *DrvValidateVersionFn) (DWORD version);

//������� ����� ����� OpenGL-�������
struct IcdTable
{
  DWORD size;
  PROC  table [1];
};

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
    {
      static const char* METHOD_NAME = "render::low_level::opengl::windows::DynamicLibrary::DynamicLibrary";

        //�������� ������������ ����������

      if (!in_path)
        throw xtl::make_null_argument_exception (METHOD_NAME, "path");
        
      path = in_path;
      
        //�������� ����������

      module = LoadLibrary (path.c_str ());
      
      if (!module)
        raise_error ("LoadLibrary");
    }
    
///����������
    ~DynamicLibrary ()
    {
      FreeLibrary (module);
    }
    
///��������� ������
    HMODULE GetModule () { return module; }
    
///��������� ���� � ����������
    const char* GetPath () { return path.c_str (); }
    
///��������� ����� �����
    void* GetSymbol (const char* name)
    {
      if (!name)
        return 0;
        
      return GetProcAddress (module, name);
    }

  private:
    HMODULE     module; //���������� ������
    stl::string path;   //���� � ����������
};

typedef stl::auto_ptr<DynamicLibrary> DynamicLibraryPtr;

/*
===================================================================================================
    ����������� ���������� ��������
===================================================================================================
*/

class AdapterLibrary: public IAdapterLibrary, public xtl::reference_counter
{
  public:
///�����������
    AdapterLibrary (DynamicLibraryPtr& in_dll) : dll (in_dll), current_context (0), current_dc (0), listener (0)
    {
        //����������� ����������

      next = first;
      prev = 0;

      if (first) first->prev = this;

      first = this;
    }

///����������
    ~AdapterLibrary ()
    {
      log.Printf ("...unload dll '%s'", GetName ());
      
        //������ ����������� ����������
        
      if (prev) prev->next = next;
      else      first      = next;
      
      if (next) next->prev = prev;            
    }  
  
///����� ����������
    static AdapterLibrary* FindLibrary (const DynamicLibraryPtr& dll)
    {
      HMODULE module = dll->GetModule ();

      for (AdapterLibrary* i=first; i; i=i->next)
        if (i->dll->GetModule () == module)
          return i;

      return 0;
    }

///��������� �������� ���������
    HGLRC GetCurrentContext () { return current_context; }

///��������� �������� ���������� ������
    HDC GetCurrentDC () { return current_dc; }

///��������� ����� ����������
    const char* GetName () { return dll->GetPath (); }
    
///��������� ������
    HMODULE GetModule () { return dll->GetModule (); }    
    
///������������ ��������, ��������� � ���������� ����������
    void ReleasePixelFormat (HDC dc) {}

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

  protected:
///����������� ������ ����� �����
    void* GetSymbol (const char* name) { return dll->GetSymbol (name); }    
    
///����������� ������ ����� ����� � ����������� ����� � ��������� ������������ ����������
    template <class Fn> void GetSymbol (const char* symbol, Fn& fn, bool check = true)
    {
      fn = reinterpret_cast<Fn> (dll->GetSymbol (symbol));

      if (!fn && check)
        throw xtl::format_operation_exception ("render::low_level::opengl::windows::AdapterLibrary::GetSymbol",
          "Symbol '%s' not found in library '%s'", symbol, dll->GetPath ());
    }        

///���������� � ����� �������� ���������
    void MakeCurrentNotify (HDC new_dc, HGLRC new_context, IContextLostListener* new_listener)
    {
      if (listener)
      {
        try
        {
          listener->OnLostCurrent ();
        }
        catch (...)
        {
          //���������� ���� ����������
        }
      }

      current_dc      = new_dc;
      current_context = new_context;
      listener        = new_listener;
    }
    
  protected:
    Log log; //��������

  private:
    DynamicLibraryPtr      dll;             //������������ ����������
    HGLRC                  current_context; //������� ��������
    HDC                    current_dc;      //������� �������� ���������� ������
    IContextLostListener*  listener;        //��������� ������� ������ ���������
    AdapterLibrary*        prev;            //���������� ����������
    AdapterLibrary*        next;            //��������� ����������
    static AdapterLibrary* first;           //������ ����������
};

AdapterLibrary* AdapterLibrary::first = 0;

/*
===================================================================================================
    WGL ����������
===================================================================================================
*/

class WglAdapterLibrary: public AdapterLibrary
{
  public:
///�����������
    WglAdapterLibrary (DynamicLibraryPtr& dll) : AdapterLibrary (dll)
    {
      try
      {
        log.Printf ("...get default WGL-entries");
        
          //������������� ����� �����
          
        GetSymbol ("wglCreateContext",       fwglCreateContext);
        GetSymbol ("wglDeleteContext",       fwglDeleteContext);
        GetSymbol ("wglDescribePixelFormat", fwglDescribePixelFormat);
        GetSymbol ("wglGetProcAddress",      fwglGetProcAddress);
        GetSymbol ("wglMakeCurrent",         fwglMakeCurrent);
        GetSymbol ("wglSetPixelFormat",      fwglSetPixelFormat);
        GetSymbol ("wglSwapBuffers",         fwglSwapBuffers);
        
        static const char* OPENGL32_DLL = "opengl32";
        
        if (!xtl::xstrnicmp (GetName (), OPENGL32_DLL, strlen (OPENGL32_DLL)))
          fwglSetPixelFormat = &::SetPixelFormat;
          
          //��������������� ������� GDI

        PixelFormatManager::RedirectApiCalls (GetModule ());

        log.Printf ("...WGL library successfully loaded");
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("...WGL library load failed");
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::WglAdapterLibrary");
        throw;
      }
    }    
    
///��������� �������� ������� ��������
    int DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
    {
      return fwglDescribePixelFormat (dc, pixel_format, size, pfd);
    }

///��������� ������� ��������
    void SetPixelFormat (HDC dc, int pixel_format)
    {
      try
      {
        PIXELFORMATDESCRIPTOR pfd;

        memset (&pfd, 0, sizeof pfd);

        if (!PixelFormatManager::SetPixelFormat (dc, pixel_format, fwglDescribePixelFormat))
          raise_error ("render::low_level::opengl::windows::PixelFormatManager::SetPixelFormat");

        if (!fwglSetPixelFormat (dc, pixel_format, &pfd))
          raise_error ("wglSetPixelFormat");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::SetPixelFormat");
        throw;
      }
    }
    
///�������� ���������
    HGLRC CreateContext (HDC dc)
    {
      try
      {
        HGLRC context = fwglCreateContext (dc);
        
        if (!context)
          raise_error ("wglCreateContext");
        
        return context;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::CreateContext");
        throw;
      }
    }

///�������� ���������
    void DeleteContext (HGLRC context)
    {
      try
      {
        if (!fwglDeleteContext (context))
          raise_error ("wglDeleteContext");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::DeleteContext");
        throw;
      }
    }    

///��������� �������� ���������
    void MakeCurrent (HDC dc, HGLRC context, IContextLostListener* listener)
    {
      try
      {
        MakeCurrentNotify (0, 0, 0);

        if (!fwglMakeCurrent (dc, context))
          raise_error ("wglMakeCurrent");

        MakeCurrentNotify (dc, context, listener);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::MakeCurrent");
        throw;
      }
    }    

///��������� ������ ����� ����� OpenGL
    const void* GetProcAddress (const char* name, size_t search_flags)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::windows::WglAdapterLibrary::GetProcAddress";

      if (!name)
      {
        if (search_flags & EntrySearch_NoThrow)
          return 0;

        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      }

      const void* address = 0;

      if (!address && (search_flags & EntrySearch_Context))
        address = fwglGetProcAddress (name);    
        
      if (address)
        return address;

      if (search_flags & EntrySearch_Library)
        address = GetSymbol (name);

      if (address)
        return address;

      if (search_flags & EntrySearch_NoThrow)
        return 0;

      throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in WGL library '%s'", name, GetName ());
    }

///����� �������
    void SwapBuffers (HDC dc)
    {
      try
      {
        if (!fwglSwapBuffers (dc))
          raise_error ("wglSwapBuffers");      
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::WglAdapterLibrary::SwapBuffers");
        throw;
      }
    }

  private:
    wglCreateContextFn       fwglCreateContext;
    wglDeleteContextFn       fwglDeleteContext;
    wglDescribePixelFormatFn fwglDescribePixelFormat;
    wglGetProcAddressFn      fwglGetProcAddress;
    wglMakeCurrentFn         fwglMakeCurrent;
    wglSetPixelFormatFn      fwglSetPixelFormat;
    wglSwapBuffersFn         fwglSwapBuffers;
};

/*
===================================================================================================
    ICD ����������
===================================================================================================
*/

class IcdAdapterLibrary: public AdapterLibrary
{
  public:
///�����������
    IcdAdapterLibrary (DynamicLibraryPtr& dll) : AdapterLibrary (dll)
    {
      try
      {
        log.Printf ("...get ICD entries");
        
          //������������� ����� �����

        GetSymbol ("DrvCreateContext",       fDrvCreateContext, false);
        GetSymbol ("DrvCreateLayerContext",  fDrvCreateLayerContext, false);                
        GetSymbol ("DrvDeleteContext",       fDrvDeleteContext);
        GetSymbol ("DrvDescribePixelFormat", fDrvDescribePixelFormat);
        GetSymbol ("DrvGetProcAddress",      fDrvGetProcAddress);
        GetSymbol ("DrvReleaseContext",      fDrvReleaseContext);
        GetSymbol ("DrvSetContext",          fDrvSetContext);
        GetSymbol ("DrvSetPixelFormat",      fDrvSetPixelFormat);
        GetSymbol ("DrvSwapBuffers",         fDrvSwapBuffers, false);
        GetSymbol ("DrvSwapLayerBuffers",    fDrvSwapLayerBuffers, false);
        GetSymbol ("DrvValidateVersion",     fDrvValidateVersion);

          //�������� ������������� � �����������

        if (!fDrvCreateContext && !fDrvCreateLayerContext)
          throw xtl::format_not_supported_exception ("", "No DrvCreateContext/DrvCreateLayerContext entries in dll '%s'", GetName ());

        if (!fDrvSwapBuffers && !fDrvSwapLayerBuffers)
          throw xtl::format_not_supported_exception ("", "No DrvSwapBuffers/DevSwapLayerBuffers entries in dll '%s'", GetName ());

        static const size_t REQUIRED_ICD_VERSION = 1;        

        if (!fDrvValidateVersion (REQUIRED_ICD_VERSION))
          throw xtl::format_not_supported_exception ("", "Adapter library '%s' doesn't support version %u", GetName (), REQUIRED_ICD_VERSION);          

          //�������� ������� ���������������� �������          

        void* set_callback_procs_fn = 0;

        GetSymbol ("DrvSetCallbackProcs", set_callback_procs_fn, false);

        if (set_callback_procs_fn)
          throw xtl::format_not_supported_exception ("", "Adapter library '%s' has unsupported function 'DrvSetCallbackProcs'", GetName ());
          
          //��������������� ������� GDI
          
        PixelFormatManager::RedirectApiCalls (GetModule ());
       
        log.Printf ("...ICD library successfully loaded");
      }
      catch (xtl::exception& exception)
      {
        log.Printf ("...ICD library load failed");
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::IcdAdapterLibrary");
        throw;
      }
    }

///��������� �������� ������� ��������
    int DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
    {
      return fDrvDescribePixelFormat (dc, pixel_format, size, pfd);
    }

///��������� ������� ��������
    void SetPixelFormat (HDC dc, int pixel_format)
    {
      try
      { 
        PIXELFORMATDESCRIPTOR pfd;

        memset (&pfd, 0, sizeof pfd);

        if (!PixelFormatManager::SetPixelFormat (dc, pixel_format, fDrvDescribePixelFormat))
          raise_error ("render::low_level::opengl::windows::PixelFormatManager::SetPixelFormat");

        if (!fDrvSetPixelFormat (dc, pixel_format))
          raise_error ("DrvSetPixelFormat");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::SetPixelFormat");
        throw;
      }
    }
    
///�������� ���������
    HGLRC CreateContext (HDC dc)
    {
      try
      {
        HGLRC context = 0;
        
        if (fDrvCreateLayerContext)
        {
          context = fDrvCreateLayerContext (dc, 0);

          if (!context)
            raise_error ("DrvCreateLayerContext");

          return context;
        }
          
        if (fDrvCreateContext)
        {
          context = fDrvCreateContext (dc);

          if (!context)
            raise_error ("DrvCreateContext");

          return context;        
        }

        throw xtl::format_operation_exception ("", "Null DrvCreateLayerContext/DrvCreateContext entries");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::CreateContext");
        throw;
      }
    }

///�������� ���������
    void DeleteContext (HGLRC context)
    {
      try
      {
        if (!fDrvDeleteContext (context))
          raise_error ("DrvDeleteContext");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::DeleteContext");
        throw;
      }
    }

///��������� �������� ���������
    void MakeCurrent (HDC dc, HGLRC context, IContextLostListener* listener)
    {
      try
      {
          //������ �������� ���������
          
        if (current_library != this)
        {
          if (current_library)
            current_library->MakeCurrent (0, 0, 0);
        }
        else
        {
            //���������� ���������� �������� ���������

          HGLRC current_context = AdapterLibrary::GetCurrentContext ();
          HDC   current_dc      = AdapterLibrary::GetCurrentDC ();

            //���������� �� ������ �������� ���������

          MakeCurrentNotify (0, 0, 0);

          current_icd_table = 0;
          current_library   = 0;

          if (current_context)
            fDrvReleaseContext (current_context);
        }

        if (!dc && !context)
        {
          MakeCurrentNotify (0, 0, listener);
          return;
        }

          //��������� �������� ���������

        if (!fDrvSetContext (dc, context, &SetContextCallBack))
          raise_error ("DrvSetContext");

        current_library = this;

          //���������� �� ��������� �������� ���������

        MakeCurrentNotify (dc, context, listener);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::MakeCurrent");
        throw;
      }
    }    
    
///��������� ������ ����� ����� OpenGL
    const void* GetProcAddress (const char* name, size_t search_flags)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::windows::WglAdapterLibrary::GetProcAddress";

      if (!name)
      {
        if (search_flags & EntrySearch_NoThrow)
          return 0;

        throw xtl::make_null_argument_exception (METHOD_NAME, "name");
      }

      const void* address = 0;

      if (!address && (search_flags & EntrySearch_Context))
        address = fDrvGetProcAddress (name);    
        
      if (address)
        return address;

      const IcdTable* icd_table = current_library == this ? current_icd_table : 0;
        
      if (search_flags & EntrySearch_Library)
      {
        int icd_table_index = get_icd_table_index (name);

        if (icd_table && icd_table_index >= 0 && icd_table_index < (int)icd_table->size)
          address = icd_table->table [icd_table_index];
      }

      if (address)
        return address;

      if (search_flags & EntrySearch_NoThrow)
        return 0;
        
      if (!icd_table)
        throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in ICD library '%s' (driver is not active)",
          name, GetName ());

      throw xtl::format_operation_exception (METHOD_NAME, "OpenGL entry '%s' not found in ICD library '%s' (ICD table has %u entries)",
        name, GetName (), icd_table->size);
    }

///����� �������
    void SwapBuffers (HDC dc)
    {            
      try
      {
        if (fDrvSwapLayerBuffers)
        {
          if (!fDrvSwapLayerBuffers (dc, WGL_SWAP_MAIN_PLANE))
            raise_error ("DrvSwapLayerBuffers");
            
          return;
        }

        if (fDrvSwapBuffers)
        {
          if (!fDrvSwapBuffers (dc))
            raise_error ("DrvSwapBuffers");

          return;
        }        
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::windows::IcdAdapterLibrary::SwapBuffers");
        throw;
      }
    }

///������������ ��������, ��������� � ���������� ����������
    void ReleasePixelFormat (HDC dc)
    {
      PixelFormatManager::ReleasePixelFormat (dc);
    }

  private:
///��������� ICD-������� ����� �����
    static DWORD CALLBACK SetContextCallBack (const IcdTable* table)
    {
      current_icd_table = table;

      return 0;
    }

  private:
    DrvValidateVersionFn     fDrvValidateVersion;
    DrvCreateContextFn       fDrvCreateContext;
    DrvCreateLayerContextFn  fDrvCreateLayerContext;
    DrvDeleteContextFn       fDrvDeleteContext;
    DrvDescribePixelFormatFn fDrvDescribePixelFormat;
    DrvGetProcAddressFn      fDrvGetProcAddress;
    DrvSetPixelFormatFn      fDrvSetPixelFormat;
    DrvSwapBuffersFn         fDrvSwapBuffers;
    DrvSetContextFn          fDrvSetContext;
    DrvReleaseContextFn      fDrvReleaseContext;
    DrvSwapLayerBuffersFn    fDrvSwapLayerBuffers;    
    
    static const IcdTable*    current_icd_table;
    static IcdAdapterLibrary* current_library;
};

const IcdTable*    IcdAdapterLibrary::current_icd_table = 0;
IcdAdapterLibrary* IcdAdapterLibrary::current_library   = 0;

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
  static const char* METHOD_NAME = "render::low_level::opengl::windows::LibraryManager::LoadLibrary";
  
  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  try
  {
    Log log;        
    
      //�������� ������������ ����������    
      
    log.Printf ("...load library '%s'", name);
      
    DynamicLibraryPtr dll = new DynamicLibrary (name);
    
      //������� ����� ��� ����������� ����������

    AdapterLibrary* library = AdapterLibrary::FindLibrary (dll);

    if (library)
    {    
      log.Printf ("...library '%s' already loaded", name);
      return library;
    }

      //�������� ����� ����������
      
    if (dll->GetSymbol ("wglCreateContext"))
      return AdapterLibraryPtr (new WglAdapterLibrary (dll), false);
      
    if (dll->GetSymbol ("DrvCreateContext") || dll->GetSymbol ("DrvCreateLayerContext"))
      return AdapterLibraryPtr (new IcdAdapterLibrary (dll), false);

    log.Printf ("...library load failed (unknown type)");

    throw xtl::format_operation_exception ("", "Invalid library '%s'. Could not initialize WGL/ICD driver "
      "(neither wglCreateContext nor DrvCreateContext nor DrvCreateLayerContext found)", name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("%s('%s')", METHOD_NAME, name);
    throw;
  }
}
