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
    size_t Width  () { return 1; }
    size_t Height () { return 1; }
    size_t Depth  () { return 1; }

    void Resize (size_t width, size_t height, size_t depth)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::Resize", "Resize not supported");
    }

///������ � �������
    void* Bitmap (size_t z)
    {
      static char buffer [8];

      return buffer;
    }

    void PutImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, const void* data)
    {
      throw xtl::format_not_supported_exception ("media::NullImageImpl::PutImage", "Image operations are not supported");      
    }
    
    void GetImage (size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, PixelFormat format, void* data)
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
