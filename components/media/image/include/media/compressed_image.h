#ifndef MEDIALIB_COMPRESSED_IMAGE_HEADER
#define MEDIALIB_COMPRESSED_IMAGE_HEADER

#include <cstddef>

#include <common/serializer_manager.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����� ������ ����� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct CompressedImageBlockDesc
{
  unsigned int offset; //�������� �� ������ ������ ������
  unsigned int size;   //������ ����� � ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class CompressedImage
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    CompressedImage  ();
    CompressedImage  (const char* file_name);
    CompressedImage  (const CompressedImage&);
    ~CompressedImage ();

    CompressedImage& operator = (const CompressedImage&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  () const;
    unsigned int Height () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���� / mip-������� / ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int LayersCount () const;
    unsigned int MipsCount   () const;
    unsigned int BlocksCount () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ - ���������-�������, �������������� ���������������� �������� ������ ������� � �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Format () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void*                     Data   () const;
    const CompressedImageBlockDesc* Blocks () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void*  Bitmap     (unsigned int layer, unsigned int mip_level) const;
    unsigned int BitmapSize (unsigned int layer, unsigned int mip_level) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (CompressedImage&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterDefaultLoaders ();    

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (CompressedImage&, CompressedImage&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomCompressedImage
{
  public:
    virtual ~ICustomCompressedImage () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int Width       () = 0;
    virtual unsigned int Height      () = 0;
    virtual unsigned int LayersCount () = 0;
    virtual unsigned int MipsCount   () = 0;
    virtual const char*  Format      () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const void*                     Data   () = 0;
    virtual const CompressedImageBlockDesc* Blocks () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<ICustomCompressedImage* (const char* file_name), void ()> CompressedImageManager;

}

#endif
