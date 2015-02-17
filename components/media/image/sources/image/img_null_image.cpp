#include "shared.h"

using namespace media;
using namespace common;

namespace
{

///��������, ������������ �� ����� ��������
class NullImageImpl: public ImageImpl
{
  public:    
///�����������
    NullImageImpl* Clone ()
    {
      addref (this);
      return this;
    }

///������ / �������������� �������
    PixelFormat Format  () { return PixelFormat_RGBA8; }

    void Convert (PixelFormat new_format)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::Convert", "Convert not supported");
    }

///������� �������� / ��������� ��������
    unsigned int Width  () { return 1; }
    unsigned int Height () { return 1; }
    unsigned int Depth  () { return 1; }

    void Resize (unsigned int width, unsigned int height, unsigned int depth)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::Resize", "Resize not supported");
    }

///������ � �������
    void* Bitmap (unsigned int z)
    {
      static char buffer [8];

      return buffer;
    }

    void PutImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, const void* data)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::PutImage", "Image operations are not supported");      
    }
    
    void GetImage (unsigned int x, unsigned int y, unsigned int z, unsigned int width, unsigned int height, unsigned int depth, PixelFormat format, void* data)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::GetImage", "Image operations are not supported");            
    }

///����������
    void Save (const char* file_name)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::Save", "Save image not supported");
    }

///�������� ����������
    static NullImageImpl* Create ()
    {
      return Singleton<Holder>::Instance ()->impl->Clone ();
    }

  private:    
    struct Holder
    {
      xtl::intrusive_ptr<NullImageImpl> impl;

      Holder () : impl (new NullImageImpl, false) {}
    };
};

}

namespace media
{

/*
    �������� ���������� ��������
*/

ImageImpl* create_null_image ()
{
  return NullImageImpl::Create ();
}

}
