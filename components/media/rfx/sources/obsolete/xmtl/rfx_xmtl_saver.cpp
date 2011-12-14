#include "shared.h"

using namespace media::rfx::obsolete;
using namespace common;

/*
    ��������������� ������ ��� ���������� ����������
*/

namespace components
{

namespace xmtl_saver
{

namespace obsolete
{

typedef stl::hash_multimap<const Material*, stl::string> MaterialIdMap;
typedef stl::vector<MaterialIdMap::iterator>             MaterialSaveList;

class XmlMaterialLibraryPreSaver: public xtl::visitor<void>
{
  private:
    typedef stl::hash_set<stl::hash_key<const char*> > IdSet;

  private:
    MaterialIdMap&    material_map; //������������� ��������� ��� �����
    MaterialSaveList& save_list;    //������������� ������ ���������� ����������
    IdSet             id_set;       //��������� ������������������ ���������������
    
  private:
      //����������� ������ ���������
    MaterialIdMap::iterator RegisterNewMaterial (const char* id, Material& material)
    {
      if (id_set.count (id))
        return material_map.end ();
      
      id_set.insert (id);
      
      try
      {
        return material_map.insert_pair (&material, id);
      }
      catch (...)
      {
        id_set.erase (id);
        throw;
      }
    }

    MaterialIdMap::iterator RegisterNewMaterial (Material& material)
    {
      MaterialIdMap::iterator iter = RegisterNewMaterial (material.Name (), material);

      for (size_t i=1; iter == material_map.end (); i++)
        iter = RegisterNewMaterial (common::format ("%s#%u", material.Name (), i).c_str (), material);
        
      return iter;
    }
    
    void InsertToSaveList (MaterialIdMap::iterator iter)
    {
      save_list.push_back (iter);
    }
      
  public:
      //�����������
    XmlMaterialLibraryPreSaver (const MaterialLibrary& material_library, MaterialIdMap& in_material_map, MaterialSaveList& in_save_list) 
      : material_map (in_material_map), save_list (in_save_list)
    {
      for (MaterialLibrary::ConstIterator i=material_library.CreateIterator (); i; ++i)
      {
        Material& material = const_cast<Material&> (**i);
        
        MaterialIdMap::iterator iter = RegisterNewMaterial (material_library.ItemId (i), material);
        
        if (iter == material_map.end ())
          continue;
        
        InsertToSaveList (iter);
      }
    }
};

class XmlMaterialLibrarySaver: public xtl::visitor<void, SpriteMaterial>
{
  private: 
    typedef stl::hash_set<const Material*> MaterialSet;
    
  private:
    XmlWriter              writer;           //������������ XML
    const MaterialLibrary& material_library; //���������� ����������
    MaterialIdMap          material_map;     //������������� ��������� ��� �����
    MaterialSet            saved_materials;  //���������� ���������
    MaterialSaveList       save_list;        //������ ����������

  private:
    /*
        ���������� SpriteMaterial
    */

    void visit (SpriteMaterial& material)
    {
      XmlWriter::Scope scope (writer, "sprite_profile");
            
      writer.WriteAttribute ("image", material.Image ());
      writer.WriteAttribute ("blend_mode", get_name (material.BlendMode ()));
      writer.WriteAttribute ("tiling", material.IsTiled ());            

      if (material.IsTiled () || material.TileOffsetX ())
        writer.WriteAttribute ("tile_offset_x", material.TileOffsetX ());

      if (material.IsTiled () || material.TileOffsetY ())
        writer.WriteAttribute ("tile_offset_y", material.TileOffsetY ());

      if (material.IsTiled () || material.TileWidth ())
        writer.WriteAttribute ("tile_width", material.TileWidth ());
        
      if (material.IsTiled () || material.TileHeight ())        
        writer.WriteAttribute ("tile_height", material.TileHeight ());        
    }
    
    /*
        ���������� ���������
    */
    
    void SaveMaterial (const char* id, const Material& material)
    {
      if (saved_materials.find (&material) != saved_materials.end ())
      {
        MaterialIdMap::const_iterator iter = material_map.find (&material);
        
        if (iter == material_map.end ())
          return;
          
          //������ ������ �� �������� ������ ������ ���������

        XmlWriter::Scope scope (writer, "instance_material");

        writer.WriteAttribute ("id", id);
        writer.WriteAttribute ("source", iter->second);

        return;        
      }      
      
        //���������� ���������
        
      XmlWriter::Scope scope (writer, "material");
 
      writer.WriteAttribute ("id", id);
      writer.WriteAttribute ("name", material.Name ());
      writer.WriteAttribute ("sort_group", material.SortGroup ());

        //���������� ������������� ���������

      const_cast<Material&> (material).Accept (*this);            

        //����������� ���������

      saved_materials.insert (&material);
    }
  
    /*
        ���������� ����������
    */
    
    void SaveLibrary ()
    {
      XmlWriter::Scope scope (writer, "material_library");
        
        //���������� ����������
      
      for (MaterialSaveList::iterator i=save_list.begin (), end=save_list.end (); i!=end; ++i)      
        SaveMaterial ((*i)->second.c_str (), *(*i)->first);
    }
  
  public:
    /*
        �����������
    */
    
    XmlMaterialLibrarySaver (const char* file_name, const MaterialLibrary& in_material_library)
      : writer (file_name), material_library (in_material_library)
    {
        //����������� ����������
        
      save_list.reserve (material_library.Size ());
      
      XmlMaterialLibraryPreSaver (material_library, material_map, save_list);

        //���������� ����������

      SaveLibrary ();
    }
};

/*
    ����������� ���������� ���������� ���������� ����������
*/

class XmlMaterialLibrarySaverComponent
{
  public:
    XmlMaterialLibrarySaverComponent ()
    {
      MaterialLibraryManager::RegisterSaver ("xmtl", &SaveLibrary);            
    }
    
  private:
    static void SaveLibrary (const char* file_name, const MaterialLibrary& library, const MaterialLibrary::LogHandler&)
    {
      XmlMaterialLibrarySaver (file_name, library);
    }
};

extern "C"
{

ComponentRegistrator<XmlMaterialLibrarySaverComponent> XMtlSaverObsolete ("media.rfx.savers.obsolete.XMtl");

}

}

}

}
