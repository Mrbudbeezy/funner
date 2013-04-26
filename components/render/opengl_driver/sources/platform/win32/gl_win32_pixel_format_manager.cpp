#include "shared.h"

using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���������� ������� �������� ��������� ����������
*/

struct DcPixelFormat
{
  HDC                                       dc;           //�������� ����������
  int                                       pixel_format; //������ ��������
  PixelFormatManager::DescribePixelFormatFn describe;     //�������, ����������� ������ ��������
  DcPixelFormat*                            prev;         //���������� ����������
  DcPixelFormat*                            next;         //��������� ����������
};

DcPixelFormat*   first           = 0; //������ ������� � ������ �������� ��������
IAdapterLibrary* default_library = 0; //����������, ������������ ��� �������� ������� �������� "�� ���������"

//�������� ������� ��������� ������
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
    ��������������� �������
*/

void PixelFormatManager::RedirectApiCalls (HMODULE module)
{
  redirect_dll_call (module, "gdi32.dll",    (void*)&::SetPixelFormat,      (void*)xtl::implicit_cast<BOOL (WINAPI*)(HDC, int, LPPIXELFORMATDESCRIPTOR)> (&PixelFormatManager::SetPixelFormat));
  redirect_dll_call (module, "gdi32.dll",    (void*)&::GetPixelFormat,      (void*)&PixelFormatManager::GetPixelFormat);
  redirect_dll_call (module, "gdi32.dll",    (void*)&::DescribePixelFormat, (void*)&PixelFormatManager::DescribePixelFormat);
  redirect_dll_call (module, "kernel32.dll", (void*)&::GetProcAddress,      (void*)&GetProcAddressRedirect);
}

/*
    ��������� ������� ��������
*/

int WINAPI PixelFormatManager::GetPixelFormat (HDC dc)
{
  for (DcPixelFormat* iter=first; iter; iter=iter->next)
    if (iter->dc == dc)
      return iter->pixel_format;

  return 0;
}


/*
    ��������� ������� ��������
*/

BOOL PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, DescribePixelFormatFn describe)
{
  try
  {
      //�������� ������������ ����������

    if (!dc || !pixel_format || !describe)
      return FALSE;

      //����� �����������

    for (DcPixelFormat* iter=first; iter; iter=iter->next)
      if (iter->dc == dc)
      {
        if (iter->pixel_format == pixel_format && describe == iter->describe)
          return TRUE;

        return FALSE;
      }
   
      //���������� ������ �����������

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
    //���������� ���� ����������

    return FALSE;
  }
}

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR*)
{
  return FALSE;
}

BOOL PixelFormatManager::CopyPixelFormat (HDC src_dc, HDC dst_dc)
{
  for (DcPixelFormat* iter=first; iter; iter=iter->next)
    if (iter->dc == src_dc)
      return SetPixelFormat (dst_dc, iter->pixel_format, iter->describe);

  return FALSE;
}

/*
    �������� ������� ��������
*/

int WINAPI PixelFormatManager::DescribePixelFormat (HDC dc, int pixel_format, UINT size, LPPIXELFORMATDESCRIPTOR pfd)
{
    //����� ��������� ��������� ����������

  DcPixelFormat* iter = first;

  for (;iter && iter->dc != dc; iter=iter->next);

    //���� �������� �� ������ - �������� ������� ����������

  if (!iter)
  {
    if (default_library)
      return default_library->DescribePixelFormat (dc, pixel_format, size, pfd);

    return 0;        
  }

  return iter->describe (dc, pixel_format, size, pfd);
}

/*
    ������� �������� ��������� ����������
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
    ��������� ���������� "�� ���������"
*/

void PixelFormatManager::SetDefaultLibrary (IAdapterLibrary* library)
{
  default_library = library;
}
