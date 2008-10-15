#include "shared.h"

using namespace media;
using namespace common;
using namespace stl;

/*
    �������� ����� ��������
*/

namespace
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

  for (Parser::NamesakeIterator i = iter->First ("Tile"); i; ++i)
    if (!test (*i, "Id") || !test (*i, "XPosition") || !test (*i, "YPosition") || !test (*i, "Width") || !test (*i, "Heigth"))
    {
      log.Error (*i, "Incorrect file format, one of tag property missing");
      break;
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
    
  if (!test (*iter, "Image"))
    throw xtl::format_operation_exception (METHOD_NAME, "Incorrect file format, no 'Image' property");
    
  stl::string string_buffer;    

  read (*iter, "Image", string_buffer);

  new_atlas.SetImage (string_buffer.c_str ());

  for (Parser::NamesakeIterator i = iter->First ("Tile"); i; ++i)
  {
    Tile new_tile;

    read (*i, "Id",        string_buffer);
    read (*i, "XPosition", new_tile.origin.x);
    read (*i, "YPosition", new_tile.origin.y);
    read (*i, "Width",     new_tile.size.x);
    read (*i, "Heigth",    new_tile.size.y);

    new_tile.name = string_buffer.c_str ();

    new_atlas.Insert (new_tile);
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
