#include "shared.h"

using namespace render;

namespace
{

/*
    ����� ������ ���������
*/

typedef stl::vector<media::rfx::Shader> ShaderArray;
typedef stl::vector<TexmapDesc>         TexmapDescArray;

struct ProgramCommonData: public xtl::reference_counter, public DebugIdHolder
{
  DeviceManagerPtr           device_manager;         //�������� ���������� ���������
  stl::string                name;                   //��� ���������
  ShaderArray                shaders;                //�������
  TexmapDescArray            texmaps;                //���������� �����
  stl::string                static_options;         //����������� ����� ���������� ��������
  stl::string                dynamic_options;        //����� ������������ �����
  ShaderOptionsLayout        dynamic_options_layout; //������������ ������������ �����
  Log                        log;                    //��������
  bool                       need_update;            //���������� ���������� ��������� ������
  bool                       has_framemaps;          //��������� ��������� �� ����������� ����� �����
  ProgramParametersLayoutPtr parameters_layout;      //������������ ���������� ���������
  PropertyBuffer             properties;             //�������� ���������
  LowLevelStateBlockPtr      state_block;            //���� ������ ����������
  
///�����������
  ProgramCommonData (const DeviceManagerPtr& in_device_manager, const char* in_name)
    : device_manager (in_device_manager)
    , dynamic_options_layout (&in_device_manager->CacheManager ())
    , need_update (true)
    , has_framemaps (false)
    , properties (in_device_manager)
  {
    try
    {        
      name = in_name;        

      render::low_level::StateBlockMask mask;

      mask.ss_constant_buffers [ProgramParametersSlot_Program] = true;

      state_block = LowLevelStateBlockPtr (device_manager->Device ().CreateStateBlock (mask), false);
      
      if (device_manager->Settings ().HasDebugLog ())
        log.Printf ("Program '%s' common data created (id=%u)", name.c_str (), Id ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::ProgramCommonData::ProgramCommonData");
      throw;
    }
  }
  
///����������
  ~ProgramCommonData ()
  {
    if (device_manager->Settings ().HasDebugLog ())
      log.Printf ("Program '%s' common data destroyed (id=%u)", name.c_str (), Id ());    
  }
  
///���������� ���������� ����
  void Update ()
  {
    try
    {
      if (!need_update)
        return;
        
      has_framemaps = false;
        
      for (TexmapDescArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
        if (iter->is_framemap)
        {
          has_framemaps = true;
          break;
        }
        
      common::PropertyMap new_properties;
      
      size_t channel = 0;
      
      for (TexmapDescArray::iterator iter=texmaps.begin (), end=texmaps.end (); iter!=end; ++iter)
      {
        TexmapDesc& desc = *iter;        
        
        new_properties.SetProperty (desc.param_name.c_str (), (int)desc.channel);
      }
      
      ProgramParametersLayoutPtr new_layout = device_manager->ProgramParametersManager ().GetParameters (ProgramParametersSlot_Program, new_properties.Layout ());
      
      properties.SetProperties (new_properties);
      
      device_manager->Device ().SSSetConstantBuffer (ProgramParametersSlot_Program, properties.Buffer ().get ());
      
      state_block->Capture ();
      
      parameters_layout = new_layout;
        
      need_update = false;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::ProgramCommonData::Update");
      throw;
    }
  }
};

typedef xtl::intrusive_ptr<ProgramCommonData> ProgramCommonDataPtr;

/*
    ��������� ��������������� ���� �����
*/

struct OptionsCacheCombinationKey
{
  size_t hash1, hash2;
  
  OptionsCacheCombinationKey (size_t in_hash1, size_t in_hash2) : hash1 (in_hash1), hash2 (in_hash2) {}
  
  bool operator == (const OptionsCacheCombinationKey& key) const
  {
    return hash1 == key.hash1 && hash2 == key.hash2;
  }
};

struct OptionsCacheCombinationValue: public xtl::reference_counter
{
  ShaderOptions options;
  ProgramPtr    program;
};

size_t hash (const OptionsCacheCombinationKey& key)
{
  return key.hash1 * key.hash2;
}

typedef xtl::intrusive_ptr<OptionsCacheCombinationValue>                      OptionsCacheCombinationValuePtr;
typedef CacheMap<OptionsCacheCombinationKey, OptionsCacheCombinationValuePtr> OptionsCacheCombinationMap;

}

/*
    �������� ���������� ���������
*/

typedef stl::hash_map<size_t, ProgramPtr> ProgramMap;

struct Program::Impl: public DebugIdHolder
{
  ProgramCommonDataPtr        common_data;               //����� ������ ���������
  ShaderOptions               options;                   //����� ������� ���������� ���������
  ProgramMap                  derived_programs;          //����������� ���������  
  OptionsCacheCombinationMap  options_cache_combinations; //���������� ����� �����
  LowLevelProgramPtr          low_level_program;         //�������������� ���������
  
///�����������
  Impl (const DeviceManagerPtr& device_manager, const char* name, const char* static_options, const char* dynamic_options)
    : options_cache_combinations (&device_manager->CacheManager ())
  {
    try
    {
      common_data = ProgramCommonDataPtr (new ProgramCommonData (device_manager, name), false);      
              
      common_data->static_options  = static_options;      
      common_data->dynamic_options = dynamic_options;
      
      common::StringArray option_array = common::split (dynamic_options);
      
      ShaderOptionsLayout& layout = common_data->dynamic_options_layout;
      
      for (size_t i=0, count=option_array.Size (); i<count; i++)
        layout.Add (option_array [i]);
        
      if (common_data->device_manager->Settings ().HasDebugLog ())
        common_data->log.Printf ("Program '%s' created (id=%u)", common_data->name.c_str (), Id ());        
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::Program::Impl::Impl");
      throw;
    }
  }
  
  Impl (const Impl& impl, const ShaderOptions& in_options)
    : common_data (impl.common_data)
    , options_cache_combinations (&common_data->device_manager->CacheManager ())
    , options (impl.options)
  {
    options.options      += " ";
    options.options      += in_options.options;
    options.options_hash  = common::strhash (options.options.c_str ());
    
    if (common_data->device_manager->Settings ().HasDebugLog ())
      common_data->log.Printf ("Program '%s' created for options = '%s' (id=%u)", common_data->name.c_str (), options.options.c_str (), Id ());    
  }
  
///����������
  ~Impl ()
  {
    if (common_data->device_manager->Settings ().HasDebugLog ())
      common_data->log.Printf ("Program '%s' destroyed for options = '%s' (id=%u)", common_data->name.c_str (), options.options.c_str (), Id ());
  }
};

/*
    ������������ / ����������
*/

Program::Program (const DeviceManagerPtr& device_manager, const char* name, const char* static_options, const char* dynamic_options)
{
  try
  {
    if (!static_options)
      throw xtl::make_null_argument_exception ("", "static_options");
      
    if (!dynamic_options)
      throw xtl::make_null_argument_exception ("", "dynamic_options");

    if (!device_manager)
      throw xtl::make_null_argument_exception ("", "device_manager");    
      
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
    
    impl = new Impl (device_manager, name, static_options, dynamic_options); 
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::Program");
    throw;
  }
}

Program::Program (Program& parent, const ShaderOptions& options)
  : impl (new Impl (*parent.impl, options))
{
}

Program::~Program ()
{
}

/*
    ������� ���������
*/

void Program::Attach (const media::rfx::Shader& shader)
{
  impl->common_data->shaders.push_back (shader);
}

void Program::Detach (const media::rfx::Shader& shader)
{
  for (ShaderArray::iterator iter=impl->common_data->shaders.begin (), end=impl->common_data->shaders.end (); iter<end;)
    if (iter->Id () == shader.Id ())
    {
      ShaderArray::iterator next = iter;
      
      ++next;
      
      impl->common_data->shaders.erase (iter);
      
      iter = next;
      --end;
    }
    else ++iter;
}

void Program::DetachAllShaders ()
{
  impl->common_data->shaders.clear ();
}

size_t Program::ShadersCount ()
{
  return impl->common_data->shaders.size ();
}

/*
    ����� ������ ���������
*/

const char* Program::StaticOptions ()
{
  return impl->common_data->static_options.c_str ();
}

const char* Program::DynamicOptions ()
{
  return impl->common_data->dynamic_options.c_str ();
}

/*
    ����������� ��������� ���������� ����� �� ����� ������ � ��� ���������
*/

size_t Program::TexmapsCount ()
{
  return impl->common_data->texmaps.size ();
}

bool Program::HasFramemaps ()
{
  try
  { 
    if (!impl->common_data->need_update)
      return impl->common_data->has_framemaps;
    
    impl->common_data->Update ();
  
    return impl->common_data->has_framemaps;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::HasFramemaps");
    throw;
  }
}

const TexmapDesc* Program::Texmaps ()
{
  if (impl->common_data->texmaps.empty ())
    return 0;
    
  return &impl->common_data->texmaps [0];
}

const TexmapDesc& Program::Texmap (size_t index)
{
  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception ("render::Program::Texmap", "index", index, impl->common_data->texmaps.size ());
    
  return impl->common_data->texmaps [index];
}

void Program::SetTexmap (size_t index, size_t channel, const char* semantic, const char* param_name, bool is_framemap)
{
  static const char* METHOD_NAME = "render::Program::SetTexmap";

  if (!semantic)
    throw xtl::make_null_argument_exception (METHOD_NAME, "semantic");
    
  if (!param_name)
      throw xtl::make_null_argument_exception (METHOD_NAME, "param_name");

  if (index >= impl->common_data->texmaps.size ())
    throw xtl::make_range_exception (METHOD_NAME, "index", index, impl->common_data->texmaps.size ());
    
  TexmapDesc& desc = impl->common_data->texmaps [index];
  
  desc.channel     = channel;
  desc.semantic    = semantic;
  desc.param_name  = param_name;
  desc.is_framemap = is_framemap;
  
  impl->common_data->need_update = true;
}

size_t Program::AddTexmap (size_t channel, const char* semantic, const char* param_name, bool is_framemap)
{
  static const char* METHOD_NAME = "render::Program::SetTexmap";
  
  impl->common_data->texmaps.push_back ();
  
  size_t index = impl->common_data->texmaps.size () - 1;
  
  try
  {
    SetTexmap (index, channel, semantic, param_name, is_framemap);
    
    return index;
  }
  catch (...)
  {
    impl->common_data->texmaps.pop_back ();
    throw;
  }
}

void Program::RemoveTexmap (size_t index)
{
  if (index >= impl->common_data->texmaps.size ())
    return;

  impl->common_data->texmaps.erase (impl->common_data->texmaps.begin () + index);
  
  impl->common_data->need_update = true;  
}

void Program::RemoveAllTexmaps ()
{
  impl->common_data->texmaps.clear ();
  
  impl->common_data->need_update = true;  
}

/*
    ��������� ����������� ���������
*/

Program& Program::DerivedProgram (const ShaderOptions& options)
{
  try
  {
    if (options.options.empty ())
      return *this;
      
    ProgramMap::iterator iter = impl->derived_programs.find (options.options_hash);
    
    if (iter != impl->derived_programs.end ())
      return *iter->second;
      
    ProgramPtr program (new Program (*this, options), false);
    
    impl->derived_programs.insert_pair (options.options_hash, program);
    
    return *program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::DerivedProgram(const ShaderOptions&)");
    throw;
  }
}

Program& Program::DerivedProgram (ShaderOptionsCache& cache)
{
  try
  {
    const ShaderOptions& options = cache.GetShaderOptions (impl->common_data->dynamic_options_layout);
    
    return DerivedProgram (options);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::DerivedProgram(const ShaderOptionsCache&)");
    throw;
  }
}

Program& Program::DerivedProgram (ShaderOptionsCache& cache1, ShaderOptionsCache& cache2)
{
  try
  {
    size_t options_count1 = cache1.Properties ().Size (),
           options_count2 = cache2.Properties ().Size ();

      //��������� ������� �������

    if (!options_count1 && options_count2)
      return *this;

    if (!options_count1)
      return DerivedProgram (cache2);

    if (!options_count2)
      return DerivedProgram (cache1);

      //��������� ������ ������
      
    const ShaderOptions& options1 = cache1.GetShaderOptions (impl->common_data->dynamic_options_layout);
    const ShaderOptions& options2 = cache2.GetShaderOptions (impl->common_data->dynamic_options_layout);    

    OptionsCacheCombinationKey key (options1.options_hash, options2.options_hash);

    if (OptionsCacheCombinationValuePtr* result = impl->options_cache_combinations.Find (key))
      return *(*result)->program;

    ShaderOptions derived_options;
    
    derived_options.options      = options1.options + " " + options2.options;
    derived_options.options_hash = common::strhash (derived_options.options.c_str ());

    OptionsCacheCombinationValuePtr value (new OptionsCacheCombinationValue, false);

    value->options = derived_options;
    value->program = &DerivedProgram (derived_options);

    impl->options_cache_combinations.Add (key, value);

    return *value->program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::DerivedProgramShaderOptionsCache&,ShaderOptionsCache&)");
    throw;
  }
}

/*
    ��������� �������������� ��������� ��������
*/

namespace
{

struct ShaderCompilerLog
{
  ShaderCompilerLog (ProgramCommonData& in_common_data)
    : common_data (in_common_data)
  {
  }
  
  void operator () (const char* message)
  {
      //��������� ����������� ��������� �� ���������� ������
      
    static const char* IGNORE_MESSAGES [] = {
      "*: No errors.",
      "Fragment shader(s) linked, vertex shader(s) linked*",
      "*shader was successfully compiled to run on hardware.",
      "Fragment shader(s) linked, vertex shader(s) linked."
    };
    
    static const size_t IGNORE_MESSAGES_COUNT = sizeof (IGNORE_MESSAGES) / sizeof (*IGNORE_MESSAGES);
    
    for (size_t i=0; i<IGNORE_MESSAGES_COUNT; i++)
      if (common::wcimatch (message, IGNORE_MESSAGES [i]))
        return;
      
      //����� ���������
    
    static const char* LINKER_PREFIX        = "linker: ";
    static size_t      LINKER_PREFIX_LENGTH = strlen (LINKER_PREFIX);
    
    if (common::wcimatch (message, "linker:*"))
    {
      common_data.log.Printf ("%s: %s", common_data.name.c_str (), message + LINKER_PREFIX_LENGTH);
      return;
    }
    
    common_data.log.Printf ("%s", message);
  }
  
  ProgramCommonData& common_data;
};    

}

const LowLevelProgramPtr& Program::LowLevelProgram ()
{
  try
  {
    if (impl->low_level_program)
      return impl->low_level_program;
      
    render::low_level::IDevice& device = impl->common_data->device_manager->Device ();
      
    stl::string options = impl->common_data->static_options + " " + impl->options.options;
      
    static const size_t SHADERS_RESERVE_COUNT = 4;

    stl::vector<render::low_level::ShaderDesc> shaders;
    
    shaders.reserve (SHADERS_RESERVE_COUNT);        

    for (ShaderArray::iterator iter=impl->common_data->shaders.begin (), end=impl->common_data->shaders.end (); iter!=end; ++iter)
    {
      media::rfx::Shader& shader = *iter;      
      
      render::low_level::ShaderDesc desc;
      
      memset (&desc, 0, sizeof (desc));      
      
      desc.name             = shader.Name ();  
      desc.profile          = shader.Profile ();
      desc.source_code_size = shader.SourceCodeSize ();
      desc.source_code      = shader.SourceCode ();
      desc.options          = options.c_str ();      
      
      shaders.push_back (desc);
    }

    if (shaders.empty ())
      throw xtl::format_operation_exception ("", "No shaders found for shading");
      
    impl->common_data->log.Printf ("Build program '%s'...", impl->common_data->name.c_str ());

    LowLevelProgramPtr program (device.CreateProgram (shaders.size (), &shaders [0], ShaderCompilerLog (*impl->common_data)), false);
    
    impl->common_data->log.Printf ("...program '%s' successfully built", impl->common_data->name.c_str ());
      
    impl->low_level_program = program;
  
    return impl->low_level_program;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::LowLevelProgram");
    throw;
  }
}

/*
    ���� ��������� ���������
*/

LowLevelStateBlockPtr Program::StateBlock ()
{
  try
  { 
    if (!impl->common_data->need_update)
      return impl->common_data->state_block;
    
    impl->common_data->Update ();
  
    return impl->common_data->state_block;    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::StateBlock");
    throw;
  }
}

/*
    ��������� ������� ������������ ���������� ��������� ��������
*/

ProgramParametersLayoutPtr Program::ParametersLayout ()
{
  try
  { 
    if (!impl->common_data->need_update)
      return impl->common_data->parameters_layout;
    
    impl->common_data->Update ();
    
    return impl->common_data->parameters_layout;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::Program::ParametersLayout");
    throw;
  }
}
