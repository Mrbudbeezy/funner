#include "shared.h"

using namespace media;
using namespace common;

namespace
{

/*
    ��������������� ����� ���������� ���� ��������
*/

class XmlAtlasSaver
{
  public:
    /*
        �����������
    */
  
    XmlAtlasSaver (const char* file_name, const Atlas& in_atlas)
      : writer (file_name), atlas (in_atlas)
    {
      SaveAtlas ();
    }

  private:  
    /*
        ���������� ����� ��������
    */
    
    void SaveAtlas ()
    {
      XmlWriter::Scope scope (writer, "Atlas");
      
      writer.WriteAttribute ("Image", atlas.Image ());

      SaveTiles ();
    }

    /*
        ���������� ������
    */

    void SaveTiles ()
    {
      for (size_t i = 0; i < atlas.TilesCount (); i++)
        SaveTile (i);
    }

    void SaveTile (size_t index)
    {
      XmlWriter::Scope scope (writer, "Tile");
      const Tile& tile = atlas.Tile (index);

      writer.WriteAttribute ("Id",        tile.name);
      writer.WriteAttribute ("XPosition", tile.origin.x);
      writer.WriteAttribute ("YPosition", tile.origin.y);
      writer.WriteAttribute ("Width",     tile.size.x);
      writer.WriteAttribute ("Height",    tile.size.y);
    }
    
  private:
    XmlWriter    writer;  //������������ XML
    const Atlas& atlas;   //����� ��������    
};

/*
    ���������� ����� ��������
*/

void xatlas_save (const char* file_name, const Atlas& atlas)
{
  XmlAtlasSaver (file_name, atlas);
}

/*
   ��������� ���������� ���� ��������
*/

class XAtlasSaverComponent
{
  public:
    //�������� ����������
    XAtlasSaverComponent () 
    {
      AtlasManager::RegisterSaver ("xatlas", &xatlas_save);
    }
};

extern "C"
{

ComponentRegistrator<XAtlasSaverComponent> XAtlasSaver ("media.image.atlas.savers.xatlas");

}

}
