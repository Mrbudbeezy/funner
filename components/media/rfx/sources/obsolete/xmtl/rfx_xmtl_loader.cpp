#include "shared.h"

using namespace media::rfx::obsolete;
using namespace common;

/*
    ��������������� ����� �������� ���������� ����������
*/

namespace components
{

namespace xmtl_loader
{

namespace obsolete
{

class XmlMaterialLibraryLoader
{
  private:
    MaterialLibrary& material_library; //���������� ����������
    Parser           parser;           //������
    ParseLog         log;              //�������� ������ ��������    

  private:
    /*
        �������������� string -> int
    */
    
    template <class T> struct Name2Value
    {
      const char* string;
      T           value;
    };
    
    template <class T>
    T GetEnumValue (Parser::Iterator node_iter, const char* attribute, size_t count, const Name2Value<T>* map, const T& default_value)
    {
      Parser::Iterator attr_iter = node_iter->First (attribute);

      if (!attr_iter)
      {
        log.Error (*node_iter, "Attribute '%s' not found");
        return default_value;
      }

      const char* value = get<const char*> (*attr_iter, "", "");

      if (!*value)
        return default_value;

      for (size_t i=0; i<count; i++)
        if (!::strcmp (map [i].string, value))
          return map [i].value;
          
      log.Error (*attr_iter, "Wrong value '%s'", value);

      return default_value;
    }
    
    /*
        ������ ������� "sprite"
    */
    
    SpriteMaterial::Pointer ParseSpriteMaterial (Parser::Iterator profile_iter)
    {
        //�������� ���������

      SpriteMaterial::Pointer material = SpriteMaterial::Create ();            
      
        //������ ����� �������� �����������
      
      material->SetImage (get<const char*> (*profile_iter, "image", material->Image ()));

        //������ ������ ���������� ������

      static const Name2Value<SpriteBlendMode> blend_mode_map [] = {
        {"none",        SpriteBlendMode_None},
        {"translucent", SpriteBlendMode_Translucent},
        {"mask",        SpriteBlendMode_Mask},
        {"additive",    SpriteBlendMode_Additive},
        {"alpha_clamp", SpriteBlendMode_AlphaClamp}
      };
      
      static const size_t blend_mode_map_size = sizeof (blend_mode_map) / sizeof (*blend_mode_map);
      
      material->SetBlendMode (GetEnumValue (profile_iter, "blend_mode", blend_mode_map_size, blend_mode_map, material->BlendMode ()));      
      
        //������ ���������� ��������
        
      bool tiling = false;

      try_read (*profile_iter, "tiling", tiling);

      if (tiling)
      {
        material->SetTiling     (true);
        material->SetTileOffset (get<size_t> (*profile_iter, "tile_offset_x", material->TileOffsetX ()),
                                 get<size_t> (*profile_iter, "tile_offset_y", material->TileOffsetY ()));
        material->SetTileSize   (get<size_t> (*profile_iter, "tile_width", material->TileWidth ()),
                                 get<size_t> (*profile_iter, "tile_height", material->TileHeight ()));
      }

      return material;
    }
    
    /*
        ������ ����������
    */

    void ParseMaterial (Parser::Iterator mtl_iter)
    {
      const char* id = get<const char*> (*mtl_iter, "id");

        //�������� ���������

      Material::Pointer material;
      Parser::Iterator  profile_iter = mtl_iter->First ("sprite_profile");

      if (profile_iter)
        material = ParseSpriteMaterial (profile_iter);
      else
      {
        log.Error (*mtl_iter, "Unknown profile at load material '%s'", id);
        return;
      }
      
      if (!material)
        return;

        //������ ����� ���������

      const char* name = get<const char*> (*mtl_iter, "name", "");

      if (*name)
        material->Rename (name);

        //������ ������ ����������

      int sort_group = material->SortGroup ();

      try_read (*mtl_iter, "sort_group", sort_group);

      material->SetSortGroup (sort_group);

        //����������� ���������

      material_library.Attach (id, material);
    }
    
    /*
        ������ ������ �� ��������
    */
    
    void ParseInstanceMaterial (Parser::Iterator mtl_iter)
    {
      const char *id     = get<const char*> (*mtl_iter, "id"),
                 *source = get<const char*> (*mtl_iter, "source");

      Material::Pointer material_ptr = material_library.Find (source);
      
      if (!material_ptr)
      {
        log.Error (*mtl_iter, "No material with name '%s' found", source);
      }

      material_library.Attach (id, material_ptr);
    }
  
    /*
        ������ ����������
    */
    
    void LoadLibrary (Parser::Iterator library_iter)
    {        
      for_each_child (*library_iter, "material", xtl::bind (&XmlMaterialLibraryLoader::ParseMaterial, this, _1));
      for_each_child (*library_iter, "instance_material", xtl::bind (&XmlMaterialLibraryLoader::ParseInstanceMaterial, this, _1));
    }

  public:
    XmlMaterialLibraryLoader (const char* file_name, MaterialLibrary& in_library, const MaterialLibrary::LogHandler& log_handler)
      : material_library (in_library),
        parser (file_name, "xml"),
        log (parser.Log ())
    {
        //�������� ���������
        
      for_each_child (parser.Root (), "material_library", xtl::bind (&XmlMaterialLibraryLoader::LoadLibrary, this, _1));

        //����� ��������� ��������

      for (size_t i=0, count=log.MessagesCount (); i<count; i++)
        log_handler (log.Message (i));
    }
};

/*
    ����������� ���������� �������� ���������� ����������
*/

class XmlMaterialLibraryLoaderComponent
{
  public:
    XmlMaterialLibraryLoaderComponent ()
    {
      MaterialLibraryManager::RegisterLoader ("xmtl", &LoadLibrary);
    }
    
  private:
    static void LoadLibrary (const char* file_name, MaterialLibrary& library, const MaterialLibrary::LogHandler& log_handler)
    {
      XmlMaterialLibraryLoader (file_name, library, log_handler);
    }
};

extern "C"
{

ComponentRegistrator<XmlMaterialLibraryLoaderComponent> XMtlLoaderObsolete ("media.rfx.loaders.obsolete.XMtl");

}

}

}

}
