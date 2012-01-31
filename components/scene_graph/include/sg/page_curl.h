#ifndef SCENE_GRAPH_PAGE_CURL_HEADER
#define SCENE_GRAPH_PAGE_CURL_HEADER

#include <sg/entity.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlCorner
{
  PageCurlCorner_LeftTop,
  PageCurlCorner_LeftBottom,
  PageCurlCorner_RightTop,
  PageCurlCorner_RightBottom
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlPageType
{
  PageCurlPageType_FrontLeft,
  PageCurlPageType_FrontRight,
  PageCurlPageType_BackLeft,
  PageCurlPageType_BackRight,

  PageCurlPageType_Num,

  PageCurlPageType_Front = PageCurlPageType_FrontLeft,
  PageCurlPageType_Back  = PageCurlPageType_BackLeft,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� � ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum PageCurlMode
{
  PageCurlMode_SinglePage,                //���� ��������
  PageCurlMode_DoublePageSingleMaterial,  //��� ��������, ���� ��������
  PageCurlMode_DoublePageDoubleMaterial   //��� ��������, ������ �� ����� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ���������� �������������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class PageCurl : public Entity
{
  public:
    typedef xtl::com_ptr<PageCurl>       Pointer;
    typedef xtl::com_ptr<const PageCurl> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetMode (PageCurlMode mode);
    PageCurlMode Mode    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPageMaterial (PageCurlPageType type, const char* name);
    const char* PageMaterial    (PageCurlPageType type) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetSize (const math::vec2f& size);
    void               SetSize (float width, float height);
    const math::vec2f& Size    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetCurlCorner     (PageCurlCorner corner);
    PageCurlCorner     CurlCorner        () const;
    void               SetCornerPosition (const math::vec2f& position);
    void               SetCornerPosition (float x, float y);
    const math::vec2f& CornerPosition    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetCurlRadius        (float curl_radius);
    float CurlRadius           () const;
    void  SetMinimumCurlRadius (float minimum_curl_radius);
    float MinimumCurlRadius    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                SetGridSize                      (const math::vec2ui& size);  //����������� ��������� �����
    void                SetGridSize                      (size_t x, size_t y);
    const math::vec2ui& GridSize                         () const;
    void                SetPageColor                     (const math::vec3f& color);  //���� ��������
    void                SetPageColor                     (float red, float green, float blue);
    const math::vec3f&  PageColor                        () const;
    void                SetCornerShadowOffset            (float shadow_offset);       //�������� ���� �� ����
    float               CornerShadowOffset               () const;
    void                SetShadowWidth                   (float width);               //������ ����
    float               ShadowWidth                      () const;
    void                SetShadowDensity                 (float density);             //��������� ����
    float               ShadowDensity                    () const;
    void                SetShadowGrowPower               (float power);               //������� ���������� ���� ��� ���������� ������
    float               ShadowGrowPower                  () const;
    void                SetOppositeCornerShadowGrowPower (float power);               //������� ���������� ���� ��� ���������� ������ ��� �������� ��������������� ���� ��������
    float               OppositeCornerShadowGrowPower    () const;
    void                SetFindBestCurlSteps             (size_t count);              //���������� �������� ������ ��������� ������� ������
    size_t              FindBestCurlSteps                () const;
    void                SetBindingMismatchWeight         (float weight);              //��� ���������� ������� ����� �������� ��� ������ ��������� ������� ������
    float               BindingMismatchWeight            () const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PageCurl  ();
    ~PageCurl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
    struct Impl;
    Impl* impl;
};

}

#endif
