#include "shared.h"

using namespace media;
using namespace common;

/*
    �������� ���������� DdsImage
*/

struct DdsImage::Impl
{
  //���������� �����
};

/*
    ������������ / ���������� / ������������
*/

DdsImage::DdsImage ()
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::DdsImage");
}

DdsImage::DdsImage (const char* file_name)
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::DdsImage");
}

DdsImage::DdsImage (const DdsImage& image)
  : impl (new Impl (*impl))
{
}

DdsImage::~DdsImage ()
{
}

DdsImage& DdsImage::operator = (const DdsImage& image)
{
  DdsImage (image).Swap (*this);

  return *this;
}

/*
    ������� �������� � ��������
*/

size_t DdsImage::Width () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::Width");
  return 0;
}

size_t DdsImage::Height () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::Height");
  return 0;
}

/*
    ���������� ���� / mip-�������
*/

size_t DdsImage::Depth () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::Depth");
  return 0;
}

size_t DdsImage::MipsCount () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::MipsCount");
  return 0;
}

/*
    ����� ����� �������� � ������ / ������ ����
*/

size_t DdsImage::BlockSize () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::BlockSize");
  return 0;
}

size_t DdsImage::LayerSize (size_t mip_level) const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::LayerSize");
  return 0;
}

/*
    ������
*/

DdsPixelFormat DdsImage::Format () const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::Format");
  return (DdsPixelFormat)-1;
}

/*
    ��������� ������� �����
*/

const void* DdsImage::Bitmap (size_t layer, size_t mip_level) const
{
  throw xtl::make_not_implemented_exception ("media::DdsImage::Bitmap");
  return 0;
}

/*
    ��������
*/

void DdsImage::Load (const char* file_name)
{
  DdsImage (file_name).Swap (*this);
}

/*
    �����
*/

void DdsImage::Swap (DdsImage& image)
{
  stl::swap (impl, image.impl);
}

namespace media
{

void swap (DdsImage& image1, DdsImage& image2)
{
  image1.Swap (image2);
}

}
