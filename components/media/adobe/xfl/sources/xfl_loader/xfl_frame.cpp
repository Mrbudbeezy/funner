#include "shared.h"

using namespace common;
using namespace media::adobe::xfl;

/*
    Разбор кадра анимации
*/

void XflParser::ParseFrame (Parser::Iterator iter, Frame& frame)
{
  frame.SetFirstFrame (get<size_t> (*iter, "index"));
  frame.SetDuration   (get<size_t> (*iter, "duration", 1));

  for (Parser::NamesakeIterator i = iter->First ("elements.DOMSymbolInstance"); i; ++i)
    ParseFrameElement (i, frame.Elements (), FrameElementType_SymbolInstance);

  for (Parser::NamesakeIterator i = iter->First ("elements.DOMBitmapInstance"); i; ++i)
    ParseFrameElement (i, frame.Elements (), FrameElementType_ResourceInstance);

  Parser::Iterator animation_core_iter = iter->First ("motionObjectXML.AnimationCore");

  if (animation_core_iter)
  {
    AnimationCore animation_core;

    ParseAnimationCore (animation_core_iter, animation_core);

    frame.SetAnimation (animation_core);
  }
  else
  {
    AnimationCore animation_core;

    animation_core.SetDuration (0);

    frame.SetAnimation (animation_core);
  }
}
