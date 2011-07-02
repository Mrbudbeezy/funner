#include "shared.h"

//TODO: ����� ����

using namespace render;

namespace
{

/*
    ��������������� ����� ���������� ����� �������
*/

struct ShaderOptionsBuilder: public xtl::reference_counter
{
  struct OptionDesc
  {
    stl::string          name;  //��� ��������
    int                  index; //������ �������� � ����� �������
    common::PropertyType type;  //��� ��������
  };
  
  typedef stl::vector<OptionDesc> OptionDescArray;
  
  OptionDescArray option_descs; //������ �����
  
///�����������
  ShaderOptionsBuilder (const common::PropertyLayout& layout, const common::StringArray& defines)
  {
    option_descs.reserve (defines.Size ());
    
    const char** define_names = defines.Data ();

    for (size_t i=0, count=defines.Size (); i<count; i++)
    {
      OptionDesc option = {define_names [i], layout.IndexOf (define_names [i])};
      
      if (option.index != -1)
        option.type = layout.PropertyType (option.index);
      
      option_descs.push_back (option);
    }
  }
  
///��������� ����� ���������� �������
  void GetShaderOptions (const common::PropertyMap& properties, ShaderOptions& out_options, stl::string& value_buffer)
  {
    out_options.options.clear ();
    
    for (OptionDescArray::iterator iter=option_descs.begin (), end=option_descs.end (); iter!=end; ++iter)
    {
      const OptionDesc& option_desc = *iter;
      
      if (iter != option_descs.begin ())
        out_options.options += ' ';
      
      out_options.options += option_desc.name;
      
      if (option_desc.index != -1)
      {
          //TODO: ������� ������������ �������� ������� (� ����������� �� ����)
        
        properties.GetProperty (option_desc.index, value_buffer);
        
        out_options.options += '=';
        out_options.options += value_buffer;
      }
    }
    
    out_options.options_hash = common::strhash (out_options.options.c_str ());
  }
};

typedef xtl::intrusive_ptr<ShaderOptionsBuilder> ShaderOptionsBuilderPtr;

}

/*
    �������� ���������� ������ �����-����������� �������
*/

typedef stl::hash_multimap<size_t, ShaderOptionsBuilderPtr> ShaderOptionsBuilderMap;

struct ShaderDefineList::Impl
{
  common::StringArray     defines;      //������ �������� �����-�����������
  ShaderOptionsBuilderMap builders;     //���������� ����� �������
  stl::string             value_buffer; //��������� ����� ��� ������������ �����
};

/*
    ����������� / ����������
*/

ShaderDefineList::ShaderDefineList ()
  : impl (new Impl)
{
}

ShaderDefineList::~ShaderDefineList ()
{
}

/*
    ���������� �������������� ����������� �����
*/

size_t ShaderDefineList::CachedShaderBuildersCount ()
{
  return impl->builders.size ();
}

/*
    �����-�����������
*/

size_t ShaderDefineList::Size ()
{
  return impl->defines.Size ();
}

const char* ShaderDefineList::Item (size_t index)
{
  return impl->defines [index];
}

void ShaderDefineList::Add (const char* name)
{
  impl->defines += name;
}

void ShaderDefineList::Remove (const char* name)
{
  if (!name)
    return;

  const char** defines = impl->defines.Data ();

  for (size_t i=0, count=impl->defines.Size (); i<count; i++)
    if (strcmp (defines [i], name))
    {
      impl->defines.Remove (i);
      
      i--;
      count--;
    }
}

void ShaderDefineList::Clear ()
{
  impl->defines.Clear ();
}

/*
    ��������� ����� �������
*/

void ShaderDefineList::GetShaderOptions (const common::PropertyMap& defines, ShaderOptions& out_options)
{
  try
  {
    size_t hash = defines.Layout ().Hash ();
    
    ShaderOptionsBuilderMap::iterator iter = impl->builders.find (hash);
    
    if (iter != impl->builders.end ())
    {
      iter->second->GetShaderOptions (defines, out_options, impl->value_buffer);
      return;
    }
    
    ShaderOptionsBuilderPtr builder (new ShaderOptionsBuilder (defines.Layout (), impl->defines), false);
    
    impl->builders.insert_pair (hash, builder);
    
    builder->GetShaderOptions (defines, out_options, impl->value_buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::ShaderDefineList::GetShaderOptions");
    throw;
  }
}
