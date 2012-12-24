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
///��������� ����� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetCurlPoint         (PageCurlCorner corner);
    void               SetCurlPoint         (const math::vec2f& point);
    void               SetCurlPoint         (float x, float y);
    const math::vec2f& CurlPoint            () const;
    void               SetCurlPointPosition (const math::vec2f& position);
    void               SetCurlPointPosition (float x, float y);
    const math::vec2f& CurlPointPosition    () const;

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
    void                SetPageColor                     (const math::vec4f& color);  //���� ��������
    void                SetPageColor                     (float red, float green, float blue, float alpha);
    const math::vec4f&  PageColor                        () const;
    void                SetCornerShadowOffset            (float shadow_offset);       //�������� ���� �� ����
    float               CornerShadowOffset               () const;
    void                SetShadowWidth                   (float width);               //������ ����
    float               ShadowWidth                      () const;
    void                SetShadowLogBase                 (float log_base);            //��������� ��������� ��������� ����
    float               ShadowLogBase                    () const;
    void                SetShadowMinLogValue             (float value);               //����������� �������� ���� ��� ����������������
    float               ShadowMinLogValue                () const;
    void                SetFindBestCurlSteps             (size_t count);              //���������� �������� ������ ��������� ������� ������
    size_t              FindBestCurlSteps                () const;
    void                SetBindingMismatchWeight         (float weight);              //��� ���������� ������� ����� �������� ��� ������ ��������� ������� ������
    float               BindingMismatchWeight            () const;
    void                SetRigidPage                     (bool state);                //�������� �� �������� �������
    bool                IsRigidPage                      () const;
    void                SetRigidPagePerspectiveFactor    (float factor);              //����������� ���������� ���� ������� �������� ��� ��������� �����������
    float               RigidPagePerspectiveFactor       () const;

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PageCurl  ();
    ~PageCurl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateBoundsCore ();

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
