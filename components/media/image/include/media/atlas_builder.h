#ifndef MEDIALIB_ATLAS_BUILDER_HEADER
#define MEDIALIB_ATLAS_BUILDER_HEADER

#include <xtl/functional_fwd>

#include <media/atlas.h>
#include <media/image.h>

namespace media
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ����������� � ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AtlasBuilderInsertMode
{
  AtlasBuilderInsertMode_Copy,      //����������� ����������� ��� �������
  AtlasBuilderInsertMode_Capture,   //������ ����������� ��� �������
  AtlasBuilderInsertMode_Reference, //�������� ������ �� ����������� ��� �������
  
  AtlasBuilderInsertMode_Default = AtlasBuilderInsertMode_Copy,
  
  AtlasBuilderInsertMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������ �� ����� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AtlasPackFlag
{
  AtlasPackFlag_PowerOfTwoEdges    = 1,       //������� ������� ������
  AtlasPackFlag_InvertTilesX       = 1 << 1,  //������������� ������������ ������ �� ��� X, �������������� � AtlasBuilder
  AtlasPackFlag_InvertTilesY       = 1 << 2,  //������������� ������������ ������ �� ��� Y, �������������� � AtlasBuilder
  AtlasPackFlag_SwapAxises         = 1 << 3,  //�������� ������������ ���� ��� ���������� ������
  AtlasPackFlag_SquareAxises       = 1 << 4,  //������ ������� ����
  AtlasPackFlag_TopRightEdgeMargin = 1 << 5,  //������������ ������ �� �������� � ������� ����� ��������
  AtlasPackFlag_PackToMaxImageSize = 1 << 6,  //��������� ����������� ��������� ���������� �������� ��� ���������� ��������� ������� ������
  AtlasPackFlag_Fast               = 1 << 7,  //������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasBuilder
{
  public:
    struct PackHandlerParams
    {
      size_t              images_count;    //���������� ������������� ��������
      const math::vec2ui* in_sizes;        //������� ��������
      math::vec2ui*       out_origins;     //�������������� ���������� ��������
      bool*               out_was_packed;  //��� ������ �������� true - ���� ���� ���������, false - ���� �� ������. ����� ���� ����� 0
      size_t              margin;          //������ ����� ����������
      size_t              max_image_size;  //������������ ������ �������������� �������� (������������ ������ ���� ���������� ���� )
      size_t              pack_flags;      //��������� ��������
    };

    typedef xtl::function<void (const PackHandlerParams&)> PackHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AtlasBuilder  (const char* pack_handler_name = "default");
    AtlasBuilder  (const PackHandler&);
    ~AtlasBuilder ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������������� ������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetMaxImageSize (size_t max_image_size);
    size_t MaxImageSize    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetMargin (size_t margin);
    size_t Margin    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetPackFlags (size_t pack_flags);
    size_t PackFlags    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (Image&, AtlasBuilderInsertMode mode = AtlasBuilderInsertMode_Default, size_t tag = 0);
    void Insert (const char* image_name, bool keep_in_memory = true, size_t tag = 0);
    void Insert (size_t width, size_t height, PixelFormat format, const void* data, bool copy_data = true, const char* name = "", size_t tag = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t AtlasesCount    ();
    void   BuildAtlas      (size_t index, const char* atlas_image_name, Atlas& result);
    void   BuildAtlasImage (size_t index, Image& result);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (AtlasBuilder&);

  private:
    AtlasBuilder (const AtlasBuilder&); //no impl
    AtlasBuilder& operator = (const AtlasBuilder&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (AtlasBuilder&, AtlasBuilder&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasBuilderManager
{
  public:
    typedef AtlasBuilder::PackHandler PackHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const PackHandler& GetPacker (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPacker       (const char* name, const PackHandler& handler);
    static void UnregisterPacker     (const char* name);
    static void UnregisterAllPackers ();
};

}

#endif
