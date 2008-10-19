#include "shared.h"

using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���ਯ�� �ଠ� ���ᥫ�� ���⥪�� ���ன�⢠
*/

struct DcPixelFormat
{
  HDC                                       dc;           //���⥪�� ���ன�⢠
  int                                       pixel_format; //�ଠ� ���ᥫ��
  PixelFormatManager::DescribePixelFormatFn describe;     //�㭪��, ����뢠��� �ଠ� ���ᥫ��
  DcPixelFormat*                            prev;         //�।��騩 ���ਯ��
  DcPixelFormat*                            next;         //᫥���騩 ���ਯ��
};

DcPixelFormat*   first           = 0; //���� ����� � ᯨ᪥ �ଠ⮢ ���ᥫ��
IAdapterLibrary* default_library = 0; //������⥪�, �ᯮ��㥬�� ��� ���ᠭ�� �ଠ� ���ᥫ�� "�� 㬮�砭��"

//���墠� �㭪樨 ����祭�� ����
PROC WINAPI GetProcAddressRedirect (HMODULE module, const LPCSTR name)
{
  void* proc = (void*)GetProcAddress (module, name);
  
  if (proc == &::SetPixelFormat)
    return reinterpret_cast<PROC> (xtl::implicit_cast<BOOL (WINAPI*)(HDC, int, LPPIXELFORMATDESCRIPTOR)> (&PixelFormatManager::SetPixelFormat));
    
  if (proc == &::GetPixelFormat)
    return reinterpret_cast<PROC> (&PixelFormatManager::GetPixelFormat);
  
  if (proc == &::DescribePixelFormat)
    return reinterpret_cast<PROC> (&PixelFormatManager::DescribePixelFormat);
    
  if (proc == &::GetProcAddress)
    return reinterpret_cast<PROC> (&GetProcAddressRedirect);
    
  return (PROC)proc;
}

}

/*
    ��७��ࠢ����� �맮���
*/

void PixelFormatManager::RedirectApiCalls (HMODULE module)
{
  redirect_dll_call (module, "gdi32.dll",    (void*)&::SetPixelFormat,      (void*)xtl::implicit_cast<BOOL (WINAPI*)(HDC, int, LPPIXELFORMATDESCRIPTOR)> (&PixelFormatManager::SetPixelFormat));
  redirect_dll_call (module, "gdi32.dll",    (void*)&::GetPixelFormat,      (void*)&PixelFormatManager::GetPixelFormat);
  redirect_dll_call (module, "gdi32.dll",    (void*)&::DescribePixelFormat, (void*)&PixelFormatManager::DescribePixelFormat);
  redirect_dll_call (module, "kernel32.dll", (void*)&::GetProcAddress,      (void*)&GetProcAddressRedirect);
}

/*
    ����祭�� �ଠ� ���ᥫ��
*/

int WINAPI PixelFormatManager::GetPixelFormat (HDC dc)
{
  for (DcPixelFormat* iter=first; iter; iter=iter->next)
    if (iter->dc == dc)
      return iter->pixel_format;

  return 0;
}


/*
    ��⠭���� �ଠ� ���ᥫ��
*/

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, DescribePixelFormatFn describe)
{
  try
  {
      //�஢�ઠ ���४⭮�� ��㬥�⮢

    if (!dc || !pixel_format || !describe)
      return FALSE;

      //���� ���ਯ��
      
    if (GetPixelFormat (dc))
      return FALSE;         
        
      //���������� ������ ���ਯ��

    DcPixelFormat* desc = new DcPixelFormat;

    desc->dc           = dc;
    desc->pixel_format = pixel_format;
    desc->describe     = describe;
    desc->prev         = 0;
    desc->next         = first;

    if (first) first->prev = desc;

    first = desc;

    return TRUE;
  }
  catch (...)
  {
    //���������� ��� �᪫�祭��

    return FALSE;
  }
}

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR*)
{
  return FALSE;
}

/*
    ���ᠭ�� �ଠ� ���ᥫ��
*/

int WINAPI PixelFormatManager::DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
{
    //���� �宦����� ���⥪�� ���ன�⢠

  DcPixelFormat* iter = first;

  for (;iter && iter->dc != dc; iter=iter->next);

    //�᫨ ���⥪�� �� ������ - ���ᠭ�� �ଠ� ����������

  if (!iter)
  {
    if (default_library)
      return default_library->DescribePixelFormat (dc, pixel_format, size, pfd);

    return 0;        
  }

  return iter->describe (dc, pixel_format, size, pfd);
}

/*
    ���⪠ ����ᮢ ���⥪�� ���ன�⢠
*/

void PixelFormatManager::ReleasePixelFormat (HDC dc)
{
  for (DcPixelFormat* iter=first; iter;)
    if (iter->dc == dc)
    {
      DcPixelFormat* next = iter->next;
      
      if (iter->prev) iter->prev->next = iter->next;
      else            first            = iter->next;

      if (iter->next) iter->next->prev = iter->prev;

      delete iter;
      
      iter = next;
    }
    else iter = iter->next;
}

/*
    ��⠭���� ������⥪� "�� 㬮�砭��"
*/

void PixelFormatManager::SetDefaultLibrary (IAdapterLibrary* library)
{
  default_library = library;
}
