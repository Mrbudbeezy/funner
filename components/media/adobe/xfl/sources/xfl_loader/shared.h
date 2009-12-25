#ifndef MEDIALIB_ADOBE_XFL_LOADER_SHARED_HEADER
#define MEDIALIB_ADOBE_XFL_LOADER_SHARED_HEADER

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>
#include <xtl/token_parser.h>

#include <math/vector.h>

#include <common/component.h>
#include <common/log.h>
#include <common/parser.h>
#include <common/strlib.h>

#include <media/adobe/xfl.h>

namespace media
{

namespace adobe
{

namespace xfl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� xfl ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class XflParser
{
  public:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    XflParser  (const char* file_name, const char* path_prefix, Document& document);
    ~XflParser ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void PrintLog ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� �� ������ ���� #ffffff
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool ReadHexColor (const char* hex_string, math::vec3f& color);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������ �� ���� ���� "0, value"
///////////////////////////////////////////////////////////////////////////////////////////////////
    float ReadFloatFromVec2 (common::Parser::Iterator, const char* property_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CheckDocument     ();
    bool CheckTimeline     (Timeline&);
    bool CheckFrameElement (const FrameElement&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ParseRoot          (common::Parser::Iterator);
    void ParseLibraries     (common::Parser::Iterator);
    void ParseResources     (common::Parser::Iterator);
    void ParseSymbols       (common::Parser::Iterator);
    void ParseTimelines     (common::Parser::Iterator);
    void ParseBitmap        (common::Parser::Iterator, Resource&);
    void ParseSymbol        (common::Parser::Iterator, Symbol&);
    void ParseTimeline      (common::Parser::Iterator, Timeline&);
    void ParseLayer         (common::Parser::Iterator, Layer&);
    void ParseFrame         (common::Parser::Iterator, Frame&);
    void ParseFrameElement  (common::Parser::Iterator, Frame::FrameElementList&, FrameElementType);
    void ParseAnimationCore (common::Parser::Iterator, AnimationCore&);

    void ParsePropertyAnimationContainer (common::Parser::Iterator, AnimationCore&, const char* property_prefix);
    void ParsePropertyAnimation          (common::Parser::Iterator, PropertyAnimation&, const char* property_prefix);
    void ParsePropertyAnimationKeyframe  (common::Parser::Iterator, PropertyAnimationKeyframe&);

  private:
    Document&      document;    //����������� ��������
    common::Parser parser;      //������
    common::Log    log;         //��������
    stl::string    path_prefix; //���� � xfl ������
};

}

}

}

#endif
