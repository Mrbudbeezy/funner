#include "shared.h"

/*
    ������ ���������� ��������
*/

void DaeParser::ParseLibraryImages (Parser::Iterator iter)
{
  if (!iter->First ("image"))
  {
    iter->Log ().Warning (*iter, "Empty 'library_images' node. Must be at least one 'image' sub-tag");
    return;
  }
  
  for_each_child (*iter, "image", xtl::bind (&DaeParser::ParseImage, this, _1));
}

void DaeParser::ParseImage (Parser::Iterator iter)
{
  const char *id   = get<const char*> (*iter, "id"),
             *path = get<const char*> (iter->First ("init_from"), "#text");

    //�������� ��������

  Image image;

  image.SetId   (id);
  image.SetPath (path);

    //���������� ������� � ����������

  model.Images ().Insert (id, image);
}
