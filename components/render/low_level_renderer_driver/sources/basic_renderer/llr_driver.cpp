#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    ���������
*/

namespace
{

const char* DRIVER_NAME = "LowLevel"; //��� ��������

}

/*
    ��������� ������� ���������� � ������ ��������
*/

struct Driver::RendererEntry
{
  stl::string             renderer_name;
  xtl::com_ptr<IRenderer> renderer;

  RendererEntry (const char*                     in_renderer_name,
                 render::low_level::IDevice*     device,
                 size_t                          swap_chains_count,
                 render::low_level::ISwapChain** swap_chains)
    : renderer_name (in_renderer_name),
      renderer (create_renderer2d (device, swap_chains_count, swap_chains), false)
    {}
};

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
    �������� ������� ������������
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
    {
      (*iter)->renderer->AddRef ();

      return (*iter)->renderer.get ();
    }

  throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

/*
   ����������� ������ �����������
*/

void Driver::RegisterRenderer
 (const char*                     name,
  render::low_level::IDevice*     device,
  size_t                          swap_chains_count,
  render::low_level::ISwapChain** swap_chains)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::RegisterRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Name already registered");

  renderer_entries.push_back (RendererEntryPtr (new RendererEntry (name, device, swap_chains_count, swap_chains)));

  if (renderer_entries.size () == 1)
    DriverManager::RegisterDriver (DRIVER_NAME, this);
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
  DriverManager::UnregisterDriver (DRIVER_NAME);
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
  return DRIVER_NAME;
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

void LowLevelDriver::RegisterRenderer
 (const char*                     name,
  render::low_level::IDevice*     device,
  size_t                          swap_chains_count,
  render::low_level::ISwapChain** swap_chains)
{
  LowLevelDriverSingleton::Instance ().RegisterRenderer (name, device, swap_chains_count, swap_chains);
}

void LowLevelDriver::UnregisterRenderer (const char* name)
{
  LowLevelDriverSingleton::Instance ().UnregisterRenderer (name);
}

void LowLevelDriver::UnregisterAllRenderers ()
{
  LowLevelDriverSingleton::Instance ().UnregisterAllRenderers ();
}
