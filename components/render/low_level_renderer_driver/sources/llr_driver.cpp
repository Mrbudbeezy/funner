#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    ����������� / ����������
*/

Driver::Driver ()
{
}

Driver::~Driver ()
{
  UnregisterDriver ();
}

/*
    �������� ��������
*/

const char* Driver::GetDescription ()
{
  return "render::mid_level::low_level_driver::Driver";
}

/*
    ������������ ��������� ������ ������������
*/

size_t Driver::GetRenderersCount ()
{
  return renderer_entries.size ();
}

const char* Driver::GetRendererName (size_t index)
{
  if (index >= GetRenderersCount ())
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::Driver::GetRendererName", "index", index, 0u, GetRenderersCount ());

  return renderer_entries[index]->renderer_name.c_str ();
}

/*
    �������� ���������� ������������
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      throw xtl::make_not_implemented_exception (METHOD_NAME); //!!!!!!!!!!

  throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

/*
   ��� ��������
*/

const char* Driver::Name ()
{
  return "LowLevel";
}
    
/*
   ����������� ������ �����������
*/

void Driver::RegisterRenderer (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::RegisterRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!device)
    throw xtl::make_null_argument_exception (METHOD_NAME, "device");

  if (!swap_chain)
    throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Name already registered");

  renderer_entries.push_back (RendererEntryPtr (new RendererEntry (name, device, swap_chain)));

  if (renderer_entries.size () == 1)
    DriverManager::RegisterDriver (Name (), this);
}

void Driver::UnregisterRenderer (const char* name)
{
  if (!name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
    {
      renderer_entries.erase (iter);

      if (renderer_entries.empty ())
        UnregisterDriver ();

      return;
    }
}

void Driver::UnregisterAllRenderers ()
{
  renderer_entries.clear ();
  UnregisterDriver ();
}

void Driver::UnregisterDriver ()
{
  DriverManager::UnregisterDriver (Name ());
}

namespace
{

/*
   �������� �������� �������������� ������� �����������
*/

typedef common::Singleton<Driver> LowLevelDriverSingleton;

}

/*
   ������� �������������� ������� �����������
*/

/*
   ��� ��������
*/

const char* LowLevelDriver::Name ()
{
  return LowLevelDriverSingleton::Instance ().Name ();
}
    
/*
   ��������� ��������
*/

IDriver* LowLevelDriver::Driver ()
{
  return LowLevelDriverSingleton::InstancePtr ();
}
    
/*
   ����������� ������ �����������
*/

void LowLevelDriver::RegisterRenderer (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
{
  LowLevelDriverSingleton::Instance ().RegisterRenderer (name, device, swap_chain);
}

void LowLevelDriver::UnregisterRenderer (const char* name)
{
  LowLevelDriverSingleton::Instance ().UnregisterRenderer (name);
}

void LowLevelDriver::UnregisterAllRenderers ()
{
  LowLevelDriverSingleton::Instance ().UnregisterAllRenderers ();
}
