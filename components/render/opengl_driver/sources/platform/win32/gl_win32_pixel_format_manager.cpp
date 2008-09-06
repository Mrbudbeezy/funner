#include "shared.h"

using namespace render::low_level::opengl::windows;

namespace
{

/*
    ���ਯ�� �ଠ� ���ᥫ�� ���⥪�� ���ன�⢠
*/

struct DcPixelFormat
{
  HDC            dc;           //���⥪�� ���ன�⢠
  int            pixel_format; //�ଠ� ���ᥫ��
  DcPixelFormat* prev;         //�।��騩 ���ਯ��
  DcPixelFormat* next;         //᫥���騩 ���ਯ��  
};

DcPixelFormat* first = 0;

}

/*
    ��७��ࠢ����� �맮���
*/

void PixelFormatManager::RedirectApiCalls (HMODULE module, void* describe_pixel_format_fn)
{
  redirect_dll_call (module, "gdi32.dll", &::SetPixelFormat,      &PixelFormatManager::SetPixelFormat);
  redirect_dll_call (module, "gdi32.dll", &::GetPixelFormat,      &PixelFormatManager::GetPixelFormat);
  redirect_dll_call (module, "gdi32.dll", &::DescribePixelFormat, describe_pixel_format_fn);
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

BOOL WINAPI PixelFormatManager::SetPixelFormat (HDC dc, int pixel_format, PIXELFORMATDESCRIPTOR*)
{
  try
  {
      //�஢�ઠ ���४⭮�� ��㬥�⮢

    if (!dc || !pixel_format)
      return FALSE;

      //���� ���ਯ��
      
    if (GetPixelFormat (dc))
      return FALSE;         
        
      //���������� ������ ���ਯ��
      
    DcPixelFormat* desc = new DcPixelFormat;
    
    desc->dc           = dc;
    desc->pixel_format = pixel_format;
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

/*
    ���⪠ ����ᮢ ���⥪�� ���ன�⢠
*/

void WINAPI PixelFormatManager::ReleasePixelFormat (HDC dc)
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
