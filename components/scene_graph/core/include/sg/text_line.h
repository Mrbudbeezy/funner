#ifndef SCENE_GRAPH_TEXT_LINE_HEADER
#define SCENE_GRAPH_TEXT_LINE_HEADER

#include <stl/auto_ptr.h>
#include <sg/entity.h>

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum TextLineAlignment
{
  TextLineAlignment_Center = 0,   //�� ������
  TextLineAlignment_Left = 1,     //�� ������ ����
  TextLineAlignment_Right = 2,    //�� ������� ����
  TextLineAlignment_Top = 1,      //�� �������� ����
  TextLineAlignment_Bottom = 2,   //�� ������� ����
  TextLineAlignment_BaseLine = 3, //�� ������� �����

  TextLineAlignment_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class TextLine: public Entity
{
  public:
    typedef xtl::com_ptr<TextLine>       Pointer;
    typedef xtl::com_ptr<const TextLine> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetColor (const math::vec4f& color);
    void               SetColor (float red, float green, float blue);
    void               SetColor (float red, float green, float blue, float alpha);
    const math::vec4f& Color    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������������ �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetCharsColorFactors (size_t first, size_t count, const math::vec4f& color);
    const math::vec4f& CharColor            (size_t index) const;
    void               CharsColors          (size_t first, size_t count, math::vec4f* colors) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void           SetText     (const char* text);
    void           SetText     (const wchar_t* text);
    const char*    Text        () const;
    const wchar_t* TextUnicode () const;
    size_t         TextLength  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t TextHash        () const;
    size_t TextUnicodeHash () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetFont (const char* font_name);
    const char* Font    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetAlignment           (TextLineAlignment horizontal, TextLineAlignment vertical);
    void              SetHorizontalAlignment (TextLineAlignment alignment);
    void              SetVerticalAlignment   (TextLineAlignment alignment);
    TextLineAlignment VerticalAlignment      () const;
    TextLineAlignment HorizontalAlignment    () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    TextLine  ();
    ~TextLine ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AcceptCore (Visitor&);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

#endif
