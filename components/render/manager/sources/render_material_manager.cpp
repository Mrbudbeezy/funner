#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� ����������
*/

namespace
{

typedef stl::vector<MaterialProxy> MaterialProxyList;

struct MaterialLibraryEntry: public xtl::reference_counter
{
  const media::rfx::MaterialLibrary* source_library; //������ �� �������� ���������� ���������� (����� ���� ��������������� �� ������ ���������)
  stl::string                        resource_name;  //��� �������
  MaterialProxyList                  materials;      //��������� ����������
  
  MaterialLibraryEntry () : source_library (0) {}
  
  ~MaterialLibraryEntry ()  
  {
    while (!materials.empty ())
    {
      materials.back ().SetResource (MaterialPtr ());

      materials.pop_back ();
    }
  }
};

typedef xtl::intrusive_ptr<MaterialLibraryEntry>                 MaterialLibraryEntryPtr;
typedef stl::list<MaterialLibraryEntryPtr>                       MaterialLibraryList;
typedef stl::hash_map<stl::hash_key<const char*>, MaterialProxy> MaterialProxyMap;

}

struct MaterialManager::Impl
{
  DeviceManagerPtr      device_manager;   //�������� ���������� ���������
  TextureManagerPtr     texture_manager;  //�������� �������
  ProgramManagerPtr     program_manager;  //�������� ��������
  MaterialProxyManager  proxy_manager;    //�������� ������ ��������
  MaterialLibraryList   loaded_libraries; //������ ����������� ���������
  MaterialProxyMap      shared_materials; //������ ��������� ������������ ����������
  Log                   log;              //�������� ���������
  
///�����������
  Impl (const DeviceManagerPtr& in_device_manager, const TextureManagerPtr& in_texture_manager, const ProgramManagerPtr& in_program_manager)
    : device_manager (in_device_manager)
    , texture_manager (in_texture_manager)
    , program_manager (in_program_manager)
  {
  }
  
///�������� ����������
  void LoadMaterialLibrary (const media::rfx::MaterialLibrary& library, const char* name)
  {        
    try
    {
      log.Printf ("Loading material library '%s' with %u materials", name && *name ? name : library.Name (), library.Size ());
      
        //�������� ��������� ��������
      
      for (MaterialLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
      {
        MaterialLibraryEntry& entry = **iter;
        
        if (entry.source_library == &library || entry.resource_name == name)
          throw xtl::format_operation_exception ("", "Material library '%s' has been alredy loaded", *name ? name : *library.Name () ? library.Name () : "<reference>");
      }
      
        //�������� ����� ����������
        
      MaterialLibraryEntryPtr entry (new MaterialLibraryEntry, false);
      
      entry->source_library = &library;
      entry->resource_name  = name;
      
      entry->materials.reserve (library.Size ());
      
      for (media::rfx::MaterialLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        const char* id = library.ItemId (iter);
        
        log.Printf ("...loading material '%s'", id);
        
        MaterialPtr material (new MaterialImpl (device_manager, texture_manager, program_manager, id), false);

        material->Update (*iter);

        MaterialProxy proxy = proxy_manager.GetProxy (id);

        proxy.SetResource (material);

        entry->materials.push_back (proxy);
      }

        //����������� ����������
        
      loaded_libraries.push_back (entry);      
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::MaterialManager::LoadMaterialLibrary");
      throw;
    }
  }
  
///�������� ����������
  void UnloadMaterialLibrary (const media::rfx::MaterialLibrary* source_library, const char* name)
  {
    if (!name)
      name = "";
      
    log.Printf ("Unloading material library '%s'", name && *name ? name : source_library ? source_library->Name () : "");  
    
    for (MaterialLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
    {
      MaterialLibraryEntry& entry = **iter;
      
      if (entry.source_library == source_library || entry.resource_name == name)
      {
        loaded_libraries.erase (iter);
        return;
      }
    }    
  }
};

/*
    ����������� / ����������
*/

MaterialManager::MaterialManager (const DeviceManagerPtr& device_manager, const TextureManagerPtr& texture_manager, const ProgramManagerPtr& program_manager)
  : impl (new Impl (device_manager, texture_manager, program_manager))
{
}

MaterialManager::~MaterialManager ()
{
}

/*
    �������� ����������
*/

MaterialPtr MaterialManager::CreateMaterial ()
{
  try
  {
    return MaterialPtr (new MaterialImpl (impl->device_manager, impl->texture_manager, impl->program_manager), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::CreateMaterial()");
    throw;
  }
}

/*
    ��������: �������� �� ������ ����������� ����������
*/

bool MaterialManager::IsMaterialLibraryResource (const char* name)
{
  return name && media::rfx::MaterialLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    �������� / �������� ��������� ����������
*/

void MaterialManager::LoadMaterialLibrary (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    media::rfx::MaterialLibrary library (name);

    impl->LoadMaterialLibrary (library, name);        
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::LoadMaterialLibrary(const char*)");
    throw;
  }
}

void MaterialManager::LoadMaterialLibrary (const media::rfx::MaterialLibrary& library)
{
  try
  {
    impl->LoadMaterialLibrary (library, "");     
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::LoadMaterialLibrary(const media::rfx::MaterialLibrary&)");
    throw;
  }
}

void MaterialManager::UnloadMaterialLibrary (const char* name)
{
  if (!name)
    return;
    
  impl->UnloadMaterialLibrary (0, name);
}

void MaterialManager::UnloadMaterialLibrary (const media::rfx::MaterialLibrary& library)
{
  impl->UnloadMaterialLibrary (&library, 0);
}

/*
    ����������� ��������� ������������ ����������
*/

void MaterialManager::ShareMaterial (const char* name, const MaterialPtr& material)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!material)
      throw xtl::make_null_argument_exception ("", "material");
      
    impl->log.Printf ("Share material '%s'", name);
      
    if (impl->shared_materials.find (name) != impl->shared_materials.end ())
      throw xtl::format_operation_exception ("", "Material '%s' has been already loaded", name);

    MaterialProxy proxy = impl->proxy_manager.GetProxy (name);

    proxy.SetResource (material);
    
    impl->shared_materials.insert_pair (name, proxy);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::MaterialManager::ShareMaterial");
    throw;
  }
}

void MaterialManager::UnshareMaterial (const char* name)
{
  if (!name)
    return;
    
  MaterialProxyMap::iterator iter = impl->shared_materials.find (name);
  
  if (iter == impl->shared_materials.end ())
    return;    
    
  iter->second.SetResource (MaterialPtr ());
  iter->second.ResetCache ();
    
  impl->shared_materials.erase (iter);  
}

/*
    ��������� ������
*/

MaterialProxy MaterialManager::GetMaterialProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    ����� ������������ ���������
*/

MaterialPtr MaterialManager::FindMaterial (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    ��������� ��������� �� ���������
*/

void MaterialManager::SetDefaultMaterial (const MaterialPtr& material)
{
  impl->proxy_manager.SetDefaultResource (material);
}

MaterialPtr MaterialManager::DefaultMaterial ()
{
  return impl->proxy_manager.DefaultResource ();
}
