#include "shared.h"

using namespace common;
using namespace media;
using namespace media::xfont;

/*
    �������� ������
*/

namespace components
{

namespace xfont_loader
{

IFontDesc* xfont_load (const char* file_name)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    return new XFontFontDesc (file_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::xfont_load");
    throw;
  }
}

/*
   ��������� �������� �������
*/

class XFontLoaderComponent
{
  public:
    //�������� ����������
    XFontLoaderComponent ()
    {
      FontManager::RegisterLoader ("xfont", &xfont_load);
    }
};

extern "C"
{

ComponentRegistrator<XFontLoaderComponent> XFontLoader ("media.font.loaders.xfont");

}

}

}

