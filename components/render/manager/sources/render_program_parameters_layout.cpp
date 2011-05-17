#include "shared.h"

using namespace render;

namespace
{

/*
    ���������
*/

const size_t RESERVED_LAYOUTS_COUNT = 4; //������������� ���������� ��������� ��������

/*
     ���� ����������
*/

struct Slot
{
  common::PropertyLayout layout;            //������������ ����������
  xtl::auto_connection   update_connection; //���������� � ��������, ���������� ��� ��������� ����������
  size_t                 layout_hash;       //��� �������

  Slot () : layout_hash (0) {}
};

}

/*
    �������� ���������� ������������ ���������
*/

typedef stl::vector<render::low_level::ProgramParameter> ParameterArray;
typedef stl::vector<ProgramParametersLayoutPtr>          LayoutArray;

struct ProgramParametersLayout::Impl: public CacheHolder, public DebugIdHolder
{
  render::low_level::ProgramParametersLayoutDesc  desc;                              //�������� ����������
  ParameterArray                                  parameters;                        //���������
  Slot                                            slots [ProgramParametersSlot_Num]; //����� ����������
  LayoutArray                                     layouts;                           //��������� ������������ �������
  size_t                                          hash;                              //��� ����� ����������
  bool                                            need_rebuild;                      //���������� �� ���������� ������������ ��������
  LowLevelDevicePtr                               device;                            //���������� ���������
  LowLevelProgramParametersLayoutPtr              device_layout;                     //������ ������������ ���������� ���������� ���������
  SettingsPtr                                     settings;                          //��������� ��������� ����������
  Log                                             log;                               //�������� ���������� ���������
  
///�����������
  Impl (const LowLevelDevicePtr& in_device, const SettingsPtr& in_settings)
    : need_rebuild (true)
    , device (in_device)
    , settings (in_settings)
  {
    static const char* METHOD_NAME = "render::ProgramParametersLayout::Impl::Impl";
    
    if (!device)
      throw xtl::make_null_argument_exception (METHOD_NAME, "device");
      
    if (!settings)
      throw xtl::make_null_argument_exception (METHOD_NAME, "settings");
      
    if (settings->HasDebugLog ())
      log.Printf ("Program parameters layout created (id=%u)", Id ());

    layouts.reserve (RESERVED_LAYOUTS_COUNT);
  }
  
///����������
  ~Impl ()
  {
    if (settings->HasDebugLog ())
      log.Printf ("Program parameters layout destroyed (id=%u)", Id ());
  }

///���������� ������ ����������
  void Rebuild ()
  {
    static render::low_level::ProgramParameter default_parameter;
    
    if (!need_rebuild)
      return;                 
      
      //���������� ����
      
    UpdateCache ();
      
      //�������������� ������ ��� �������� ����������
    
    size_t parameters_count = 0;
    
    for (LayoutArray::iterator iter=layouts.begin (), end=layouts.end (); iter!=end; ++iter)
      parameters_count += (*iter)->Parameters ().parameters_count;
      
    for (int i=0; i<ProgramParametersSlot_Num; i++)
      parameters_count += slots [i].layout.Size ();

    bool has_debug_log = settings->HasDebugLog ();  
    
    if (has_debug_log)
      log.Printf ("Updating program parameters layout with %u parameters (id=%u):", parameters_count, Id ());
      
    ParameterArray new_parameters;

    new_parameters.reserve (parameters_count);
    
      //���������� ������      
      
    if (parameters_count)
    {
      for (LayoutArray::iterator iter=layouts.begin (), end=layouts.end (); iter!=end; ++iter)
      {
        const render::low_level::ProgramParametersLayoutDesc& layout_desc = (*iter)->Parameters ();

        new_parameters.insert (new_parameters.end (), layout_desc.parameters, layout_desc.parameters + layout_desc.parameters_count);
      }            
      
      size_t new_hash = 0xffffffff;

      for (int i=0; i<ProgramParametersSlot_Num; i++)
      {
        const common::PropertyLayout& layout = slots [i].layout;

        slots [i].layout_hash = layout.Hash ();        
        new_hash              = common::crc32 (&slots [i].layout_hash, sizeof (size_t), new_hash);

        if (!layout.Size ())
          continue;
          
        if (has_debug_log)
          log.Printf ("...slot[%u]: hash=%08x, parameters_count=%u", i, slots [i].layout_hash, layout.Size ());          

        const common::PropertyDesc* property = layout.Properties ();
        size_t                      count    = layout.Size ();
        
        for (size_t j=0;count--; property++, j++)
        {
          render::low_level::ProgramParameter parameter;

          memset (&parameter, 0, sizeof (parameter));

          parameter.name   = property->name;
          parameter.slot   = i;
          parameter.count  = property->elements_count;
          parameter.offset = property->offset;
          
          switch (property->type)
          {            
            case common::PropertyType_String:
              continue; //������ �� �������������� ��������
            case common::PropertyType_Integer:
              parameter.type = render::low_level::ProgramParameterType_Int;
              break;
            case common::PropertyType_Float:
              parameter.type = render::low_level::ProgramParameterType_Float;
              break;            
            case common::PropertyType_Vector:
              parameter.type = render::low_level::ProgramParameterType_Float4;
              break;            
            case common::PropertyType_Matrix:
              parameter.type = render::low_level::ProgramParameterType_Float4x4;
              break;            
            default:
              throw xtl::format_operation_exception ("render::ProgramParametersLayout", "Unexpected property[%u] '%s' type %s at parsing", j, parameter.name, get_name (property->type));
          }
          
          if (has_debug_log)
          {
            if (parameter.count != 1) log.Printf ("......name='%s', type=%sx%u, offset=%u", parameter.name, get_name (property->type), parameter.count, parameter.offset);
            else                      log.Printf ("......name='%s', type=%s, offset=%u", parameter.name, get_name (property->type), parameter.offset);
          }

          new_parameters.push_back (parameter);          
        }
      }

      memset (&desc, 0, sizeof (desc));

      desc.parameters_count = new_parameters.size ();
      desc.parameters       = new_parameters.empty () ? &default_parameter : &parameters [0];

        //������ ����

      hash = new_hash;
    }
    else
    {
        //��������� �������� ������ ���������� ����������      

      desc.parameters_count = 0;
      desc.parameters       = &default_parameter;
      hash                  = 0xffffffff;
    }
    
    if (has_debug_log)
      log.Printf ("...program parameters layout updated (hash=%08x)", hash);
    
    parameters.swap (new_parameters);
        
    need_rebuild  = false;
    device_layout = LowLevelProgramParametersLayoutPtr ();
  }
  
///������ � �����
  void ResetCacheCore ()
  {
    need_rebuild  = true;
    device_layout = LowLevelProgramParametersLayoutPtr ();
  }

  void UpdateCacheCore ()
  {
  }  

///���������� �� ���������� �������
  void OnPropertiesUpdated (ProgramParametersSlot slot)
  {
    if (slots [slot].layout_hash == slots [slot].layout.Hash ())
      return;

    slots [slot].layout_hash = slots [slot].layout.Hash ();

    ResetCacheCore ();
  }
};

/*
    ����������� / ����������
*/

ProgramParametersLayout::ProgramParametersLayout (const LowLevelDevicePtr& device, const SettingsPtr& settings)
  : impl (new Impl (device, settings))
{
}

ProgramParametersLayout::~ProgramParametersLayout ()
{
}

/*
    ������������ ����������
*/

const render::low_level::ProgramParametersLayoutDesc& ProgramParametersLayout::Parameters ()
{
  if (impl->need_rebuild)
    impl->Rebuild ();
    
  return impl->desc;
}

/*
    ���
*/

size_t ProgramParametersLayout::Hash () const
{
  if (impl->need_rebuild)
    impl->Rebuild ();
    
  return impl->hash;
}

/*
    ��������� ���������� � ��������������� �����
*/

void ProgramParametersLayout::SetSlot (ProgramParametersSlot slot, const common::PropertyLayout& layout)
{
  if (slot < 0 || slot >= ProgramParametersSlot_Num)
    throw xtl::make_range_exception ("render::ProgramParametersLayout::SetSlot", "slot", slot, 0, ProgramParametersSlot_Num);
    
  impl->slots [slot].layout      = layout;
  impl->slots [slot].layout_hash = layout.Hash ();

  impl->slots [slot].update_connection.disconnect ();
  
  impl->slots [slot].layout.Capture ();
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::AttachSlot (ProgramParametersSlot slot, const common::PropertyMap& map)
{
  if (slot < 0 || slot >= ProgramParametersSlot_Num)
    throw xtl::make_range_exception ("render::ProgramParametersLayout::AttachSlot", "slot", slot, 0, ProgramParametersSlot_Num);

  impl->slots [slot].layout      = map.Layout ();
  impl->slots [slot].layout_hash = map.Layout ().Hash ();

  impl->slots [slot].update_connection = map.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, xtl::bind (&Impl::OnPropertiesUpdated, &*impl, slot));

  impl->need_rebuild = true;
}

void ProgramParametersLayout::ResetSlot (ProgramParametersSlot slot)
{
  if (slot < 0 || slot >= ProgramParametersSlot_Num)
    throw xtl::make_range_exception ("render::ProgramParametersLayout::ResetSlot", "slot", slot, 0, ProgramParametersSlot_Num);
    
  impl->slots [slot].layout_hash = 0;

  impl->slots [slot].layout.Clear ();

  impl->slots [slot].update_connection.disconnect ();
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::ResetAllSlots ()
{
  for (int i=0; i<ProgramParametersSlot_Num; i++)
  {
    impl->slots [i].layout_hash = 0;

    impl->slots [i].layout.Clear ();

    impl->slots [i].update_connection.disconnect ();
  }

  impl->need_rebuild = true;
}

/*
    ���������� ����������
*/

void ProgramParametersLayout::Attach (const ProgramParametersLayout& layout)
{
  impl->AttachCacheSource (*layout.impl);

  try
  {
    impl->layouts.push_back (&const_cast<ProgramParametersLayout&> (layout));
  }
  catch (...)
  {
    impl->DetachCacheSource (*layout.impl);
    throw;
  }
  
  impl->need_rebuild = true;
}

void ProgramParametersLayout::Detach (const ProgramParametersLayout& layout)
{
  LayoutArray::iterator first = stl::remove (impl->layouts.begin (), impl->layouts.end (), &const_cast<ProgramParametersLayout&> (layout));
  
  for (LayoutArray::iterator iter=first; iter!=impl->layouts.end (); ++iter)
    impl->DetachCacheSource (*(*iter)->impl);

  impl->layouts.erase (first, impl->layouts.end ());

  impl->need_rebuild = true;
}

void ProgramParametersLayout::DetachAll ()
{
  for (LayoutArray::iterator iter=impl->layouts.begin (); iter!=impl->layouts.end (); ++iter)
    impl->DetachCacheSource (*(*iter)->impl);

  impl->layouts.clear ();
  
  impl->need_rebuild = true;
}

/*
    ��������� ������� ������������ ���������� ��������������� ���������� ���������
*/

LowLevelProgramParametersLayoutPtr& ProgramParametersLayout::DeviceLayout ()
{
  try
  {
    if (impl->need_rebuild)
      impl->Rebuild ();
    
    if (impl->device_layout)
      return impl->device_layout;
      
    if (impl->settings->HasDebugLog ())
      impl->log.Printf ("Creating low-level layout for program parameters layout (id=%u)", impl->Id ());

    impl->device_layout = LowLevelProgramParametersLayoutPtr (impl->device->CreateProgramParametersLayout (impl->desc), false);                    
      
    return impl->device_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ProgramParametersLayout::DeviceLayout");
    throw;
  }      
}
