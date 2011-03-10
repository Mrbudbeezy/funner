#include "shared.h"

using namespace render;

/*
    �������� ���������� ��������� ����������
*/

namespace
{

typedef stl::vector<PrimitiveProxy> PrimitiveProxyList;

struct MeshLibraryEntry: public xtl::reference_counter
{
  const media::geometry::MeshLibrary* source_library; //������ �� �������� ���������� ����� (����� ���� ��������������� �� ������ ���������)
  stl::string                         resource_name;  //��� �������
  PrimitiveProxyList                  primitives;
  
  MeshLibraryEntry () : source_library (0) {}
};

typedef xtl::intrusive_ptr<MeshLibraryEntry> MeshLibraryEntryPtr;
typedef stl::list<MeshLibraryEntryPtr>       MeshLibraryList;

}

struct PrimitiveManager::Impl
{
  RenderManagerImpl&    manager;          //������ �� ���������
  PrimitiveProxyManager proxy_manager;    //�������� ������ ��������
  MeshLibraryList       loaded_libraries; //������ ����������� ���������
  
///�����������
  Impl (RenderManagerImpl& in_manager)
    : manager (in_manager)
  {
  }
  
///�������� ����������
  void LoadMeshLibrary (const media::geometry::MeshLibrary& library, const char* name)
  {        
    try
    {
        //�������� ��������� ��������
      
      for (MeshLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
      {
        MeshLibraryEntry& entry = **iter;
        
        if (entry.source_library == &library || entry.resource_name == name)
          throw xtl::format_operation_exception ("", "Mesh library '%s' has been alredy loaded", *name ? name : *library.Name () ? library.Name () : "<reference>");
      }
      
        //�������� ������� ���������� ���������� �����
        
      PrimitiveBuffersPtr primitive_buffers = manager.CreatePrimitiveBuffers (MeshBufferUsage_Default, MeshBufferUsage_Default);
      
        //�������� ����� ����������
        
      MeshLibraryEntryPtr entry (new MeshLibraryEntry, false);
      
      entry->source_library = &library;
      entry->resource_name  = name;
      
      entry->primitives.reserve (library.Size ());
      
      primitive_buffers->EnableCache ();      
      
      for (media::geometry::MeshLibrary::ConstIterator iter=library.CreateIterator (); iter; ++iter)
      {
        PrimitivePtr primitive = manager.CreatePrimitive (primitive_buffers);
        
        PrimitiveProxy proxy = proxy_manager.GetProxy (library.ItemId (iter));
        
        proxy.SetResource (primitive);
        
        entry->primitives.push_back (proxy);
      }

      primitive_buffers->DisableCache ();
      
        //����������� ����������
        
      loaded_libraries.push_back (entry);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::PrimitiveManager::LoadMeshLibrary");
      throw;
    }
  }
  
///�������� ����������
  void UnloadMeshLibrary (const media::geometry::MeshLibrary* source_library, const char* name)
  {
    if (!name)
      name = "";
    
    for (MeshLibraryList::iterator iter=loaded_libraries.begin (), end=loaded_libraries.end (); iter!=end; ++iter)
    {
      MeshLibraryEntry& entry = **iter;
      
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

PrimitiveManager::PrimitiveManager (RenderManagerImpl& owner)
  : impl (new Impl (owner))
{
}

PrimitiveManager::~PrimitiveManager ()
{
}

/*
    ��������: �������� �� ������ ����������� �����
*/

bool PrimitiveManager::IsMeshLibraryResource (const char* name)
{
  return name && media::geometry::MeshLibraryManager::FindLoader (name, common::SerializerFindMode_ByName) != 0;
}

/*
    �������� / �������� ��������� �����
*/

void PrimitiveManager::LoadMeshLibrary (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    media::geometry::MeshLibrary library (name);

    impl->LoadMeshLibrary (library, name);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::LoadMeshLibrary(const char*)");
    throw;
  }
}

void PrimitiveManager::LoadMeshLibrary (const media::geometry::MeshLibrary& library)
{
  try
  {
    impl->LoadMeshLibrary (library, ""); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::PrimitiveManager::LoadMeshLibrary(const media::geometry::MeshLibrary&)");
    throw;
  }
}

void PrimitiveManager::UnloadMeshLibrary (const char* name)
{
  if (!name)
    return;
    
  impl->UnloadMeshLibrary (0, name);
}

void PrimitiveManager::UnloadMeshLibrary (const media::geometry::MeshLibrary& library)
{
  impl->UnloadMeshLibrary (&library, 0);
}

/*
    ��������� ������
*/

PrimitiveProxy PrimitiveManager::GetPrimitiveProxy (const char* name)
{
  return impl->proxy_manager.GetProxy (name);
}

/*
    ����� ������������ ���������
*/

PrimitivePtr PrimitiveManager::FindPrimitive (const char* name)
{
  return impl->proxy_manager.FindResource (name);
}

/*
    ��������� ��������� �� ���������
*/

void PrimitiveManager::SetDefaultPrimitive (const PrimitivePtr& primitive)
{
  impl->proxy_manager.SetDefaultResource (primitive);
}

PrimitivePtr PrimitiveManager::DefaultPrimitive ()
{
  return impl->proxy_manager.DefaultResource ();
}
