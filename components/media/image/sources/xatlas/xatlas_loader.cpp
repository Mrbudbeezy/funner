#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

/*
    �������� ����� ��������
*/

namespace components
{

namespace xatlas_loader
{

bool test (const ParseNode& node, const char* name)
{
  return node.First (name);
}

void xatlas_load (const char* file_name, Atlas& atlas)
{
  Parser           p (file_name);
  ParseLog         log = p.Log ();
  Parser::Iterator iter = p.Root ().First("Atlas");
  Atlas            new_atlas;

  static const char* METHOD_NAME = "media::xatlas_load";

  for (Parser::NamesakeIterator i = iter->First ("Image"); i; ++i)
  {
    if (!test (*i, "Name"))
    {
      log.Error (*i, "Incorrect file format, 'Name' property missing");
      break;
    }

    for (Parser::NamesakeIterator j = i->First ("Tile"); j; ++j)
    {
      if (!test (*j, "Name") || !test (*j, "Left") || !test (*j, "Bottom") || !test (*j, "Width") || !test (*j, "Height"))
      {
        log.Error (*j, "Incorrect file format, one of tag property missing");
        break;
      }
    }
  }

  for (size_t i = 0; i < log.MessagesCount (); i++)
    switch (log.MessageType (i))
    {
      case ParseLogMessageType_Error:
      case ParseLogMessageType_FatalError:
        throw xtl::format_operation_exception (METHOD_NAME, log.Message (i));
      default:
        break;
    }

  if (!iter)
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Atlas' root tag");
    
  for (Parser::NamesakeIterator i = iter->First ("Image"); i; ++i)
  {
    const char*  image_name   = common::get<const char*>  (*i, "Name");
    unsigned int image_width  = common::get<unsigned int> (*i, "Width", 0u);
    unsigned int image_height = common::get<unsigned int> (*i, "Height", 0u);

    for (Parser::NamesakeIterator j = i->First ("Tile"); j; ++j)
    {
      const char* tile_name = common::get<const char*> (*j, "Name");

      math::vec2ui origin, size;

      origin.x = common::get<unsigned int> (*j, "Left");
      origin.y = common::get<unsigned int> (*j, "Bottom");
      size.x   = common::get<unsigned int> (*j, "Width");
      size.y   = common::get<unsigned int> (*j, "Height");

      unsigned int tag = common::get<unsigned int> (*j, "Tag", 0);

      new_atlas.Insert (tile_name, image_name, origin, size, tag);
    }

    new_atlas.SetImageSize (image_name, image_width, image_height);
  }

  atlas.Swap (new_atlas);
}

/*
   ��������� �������� ���� ��������
*/

class XAtlasLoaderComponent
{
  public:
    //�������� ����������
    XAtlasLoaderComponent () 
    {
      AtlasManager::RegisterLoader ("xatlas", &xatlas_load);
    }
};

extern "C"
{

ComponentRegistrator<XAtlasLoaderComponent> XAtlasLoader ("media.image.atlas.loaders.xatlas");

}

}

}
