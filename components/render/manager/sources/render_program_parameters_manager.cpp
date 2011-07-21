#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    �������� ���������� ��������� ���������� �������� ��������
*/

typedef stl::hash_multimap<size_t, ProgramParametersLayout*> LayoutMap;
typedef CacheMap<size_t, ProgramParametersLayoutPtr>         CompositeLayoutMap;

struct ProgramParametersManager::Impl: public xtl::trackable
{
  LowLevelDevicePtr   device;            //���������� ������������
  LayoutMap           layouts;           //������� ���������� ��������� ������������
  CompositeLayoutMap  composite_layouts; //��������� ������� ���������� ��������� ������������
  SettingsPtr         settings;          //��������� ��������� ����������

///�����������
  Impl (const LowLevelDevicePtr& in_device, const SettingsPtr& in_settings, const CacheManagerPtr& cache_manager)
    : device (in_device)
    , settings (in_settings)
    , composite_layouts (cache_manager)
  {
    static const char* METHOD_NAME = "render::ProgramParametersManager::Impl";
    
    if (!device)
      throw xtl::make_null_argument_exception (METHOD_NAME, "device");
      
    if (!settings)
      throw xtl::make_null_argument_exception (METHOD_NAME, "settings");
  }

///����������
  ~Impl ()
  {
    layouts.clear ();
    composite_layouts.Clear ();
  }

///�������� �������
  void RemoveLayout (size_t hash, ProgramParametersLayout* layout)
  {
    stl::pair<LayoutMap::iterator, LayoutMap::iterator> range = layouts.equal_range (hash);
    
    for (; range.first!=range.second; ++range.first)
      if (range.first->second == layout)
      {
        layouts.erase (range.first);        

        return;        
      }
  }  
};

/*
    ����������� / ����������
*/

ProgramParametersManager::ProgramParametersManager (const LowLevelDevicePtr& device, const SettingsPtr& settings, const CacheManagerPtr& cache_manager)
{
  try
  {
    impl = new Impl (device, settings, cache_manager);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::ProgramParametersManager");
    throw;
  }
}

ProgramParametersManager::~ProgramParametersManager ()
{
}

/*
    ��������� ������� ����������
*/

ProgramParametersLayoutPtr ProgramParametersManager::GetParameters (ProgramParametersSlot slot, const common::PropertyLayout& layout)
{
  try
  {        
    stl::pair<LayoutMap::iterator, LayoutMap::iterator> range = impl->layouts.equal_range (layout.Hash ());    
    
    for (size_t parameters_count=layout.Size (); range.first!=range.second; ++range.first)
      if (range.first->second->Parameters ().parameters_count == parameters_count)
        return range.first->second;

    ProgramParametersLayoutPtr result_layout (new ProgramParametersLayout (impl->device, impl->settings), false);

    result_layout->SetSlot (slot, layout);

    result_layout->connect_tracker (xtl::bind (&Impl::RemoveLayout, &*impl, layout.Hash (), &*result_layout), *impl);
    
    impl->layouts.insert_pair (layout.Hash (), result_layout.get ());

    return result_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::GetParameters(const common::PropertyLayout&)");
    throw;
  }
}

ProgramParametersLayoutPtr ProgramParametersManager::GetParameters
 (const ProgramParametersLayout* layout1,
  const ProgramParametersLayout* layout2,
  const ProgramParametersLayout* layout3)
{
  try
  {
    size_t hash = 0xffffffff;

    if (layout3) hash = common::crc32 (&layout3, sizeof (layout3), hash);
    if (layout2) hash = common::crc32 (&layout2, sizeof (layout2), hash);
    if (layout1) hash = common::crc32 (&layout1, sizeof (layout1), hash);

    if (ProgramParametersLayoutPtr* layout = impl->composite_layouts.Find (hash))
      return *layout;

    ProgramParametersLayoutPtr result_layout (new ProgramParametersLayout (impl->device, impl->settings), false);

    if (layout1) result_layout->Attach (*layout1);
    if (layout2) result_layout->Attach (*layout2);
    if (layout3) result_layout->Attach (*layout3);

    impl->composite_layouts.Add (hash, result_layout);

    return result_layout;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersManager::GetParameters(const common::ProgramParametersLayout*,const ProgramParametersLayout*,const ProgramParametersLayout*)");
    throw;
  }
}
