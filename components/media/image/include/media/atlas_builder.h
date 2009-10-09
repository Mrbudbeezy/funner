#ifndef MEDIALIB_ATLAS_BUILDER_HEADER
#define MEDIALIB_ATLAS_BUILDER_HEADER

#include <xtl/functional_fwd>

#include <media/atlas.h>

namespace media
{

//forward declaration
class Image;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� ����������� � ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AtlasBuilderInsertMode
{
  AtlasBuilderInsertMode_Copy,    //����������� ����������� ��� �������
  AtlasBuilderInsertMode_Capture, //������ ����������� ��� �������
  
  AtlasBuilderInsertMode_Default = AtlasBuilderInsertMode_Copy,
  
  AtlasBuilderInsertMode_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���������� ������ �� ����� ���������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum AtlasPackFlag
{
  AtlasPackFlag_PowerOfTwoEdges = 1, //������� ������� ������
  AtlasPackFlag_InvertTilesX    = 2, //������������� ������������ ������ �� ��� X
  AtlasPackFlag_InvertTilesY    = 4, //������������� ������������ ������ �� ��� Y
  AtlasPackFlag_SwapAxises      = 8, //�������� ������������ ���� ��� ���������� ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������ �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AtlasBuilder
{
  public:
    typedef xtl::function<void (size_t images_count, const math::vec2ui* in_sizes, math::vec2ui* out_origins, size_t pack_flags)> PackHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AtlasBuilder  (const char* pack_handler_name = "default");
    AtlasBuilder  (const PackHandler&);
    ~AtlasBuilder ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� �������� �������� (������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetAtlasImageName (const char* name);
    const char* AtlasImageName    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (media::Image&, AtlasBuilderInsertMode mode = AtlasBuilderInsertMode_Default);
    void Insert (const char* image_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Reset ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Build (media::Atlas& out_atlas, media::Image& out_atlas_image, size_t pack_flags);

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
