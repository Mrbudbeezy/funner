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
  RaiseNotImplemented ("media::DdsImage::DdsImage");
}

DdsImage::DdsImage (const char* file_name)
{
  RaiseNotImplemented ("media::DdsImage::DdsImage");
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
  RaiseNotImplemented ("media::DdsImage::Width");
  return 0;
}

size_t DdsImage::Height () const
{
  RaiseNotImplemented ("media::DdsImage::Height");
  return 0;
}

/*
    ���������� ���� / mip-�������
*/

size_t DdsImage::Depth () const
{
  RaiseNotImplemented ("media::DdsImage::Depth");
  return 0;
}

size_t DdsImage::MipsCount () const
{
  RaiseNotImplemented ("media::DdsImage::MipsCount");
  return 0;
}

/*
    ����� ����� �������� � ������ / ������ ����
*/

size_t DdsImage::BlockSize () const
{
  RaiseNotImplemented ("media::DdsImage::BlockSize");
  return 0;
}

size_t DdsImage::LayerSize (size_t mip_level) const
{
  RaiseNotImplemented ("media::DdsImage::LayerSize");
  return 0;
}

/*
    ������
*/

DdsPixelFormat DdsImage::Format () const
{
  RaiseNotImplemented ("media::DdsImage::Format");
  return (DdsPixelFormat)-1;
}

/*
    ��������� ������� �����
*/

const void* DdsImage::Bitmap (size_t layer, size_t mip_level) const
{
  RaiseNotImplemented ("media::DdsImage::Bitmap");
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
