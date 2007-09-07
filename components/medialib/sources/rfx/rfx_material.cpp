#include "shared.h"

using namespace media::rfx;
using namespace media;
using namespace common;
using namespace stl;

namespace
{

const size_t DEFAULT_MATERIAL_PASSES_RESERVE = 4; //������������� ���������� ��������

/*
    ���� ��� ������� ������������ ���������
*/

struct Pass
{
  Material::ShaderPointer shader;     //������
  PassState               state;      //��������� �������
  CloneMode               clone_mode; //����� �����������
  
  Pass (const Material::ShaderPointer& in_shader, CloneMode in_clone_mode, PassState in_state=PassState_Enabled)
    : shader (in_shader), clone_mode (in_clone_mode), state (in_state) {}
};

typedef vector<Pass> PassArray;

}

/*
    �������� ���������� Material
*/

struct Material::Impl: public SharedResource
{
  string    name;       //��� ���������
  PassArray passes;     //�������
  int       sort_group; //������ ����������  
  
  Impl ();
  Impl (const Impl&);
};

/*
    Impl::Impl
*/

Material::Impl::Impl ()
  : sort_group (0)
{
  passes.reserve (DEFAULT_MATERIAL_PASSES_RESERVE);
}

Material::Impl::Impl (const Impl& impl)
  : name (impl.name), sort_group (impl.sort_group)
{
  passes.reserve (impl.passes.size ());

  for (PassArray::const_iterator i=impl.passes.begin (), end=impl.passes.end (); i!=end; ++i)
    passes.push_back (Pass (i->shader->Clone (i->clone_mode), i->clone_mode, i->state));
}

/*
    ������������ / ����������
*/

Material::Material ()
  : impl (new Impl)
  {}

Material::Material (const Material& material, CloneMode mode)
  : impl (clone_resource (material.impl, mode, "media::Material::Material"))
  {}

Material::~Material ()
{
  release_resource (impl);
}

/*
    ������������
*/

Material& Material::operator = (const Material& material)
{
  Assign (material);

  return *this;
}

void Material::Assign (const Material& material, CloneMode mode)
{
  Material (material, mode).Swap (*this);
}

/*
    ��� ���������
*/

const char* Material::Name () const
{
  return impl->name.c_str ();
}

void Material::Rename (const char* name)
{
  if (!name)
    RaiseNullArgument ("media::rfx::Material::Rename", "name");
    
  impl->name = name;
}

/*
    ���������� ��������
*/

size_t Material::PassesCount () const
{
  return impl->passes.size ();
}

/*
    ��������� ��������
*/

const media::rfx::Shader& Material::Shader (size_t index) const
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::Shader", "index", index, impl->passes.size ());
    
  return *impl->passes [index].shader;
}

media::rfx::Shader& Material::Shader (size_t index)
{
  return const_cast<rfx::Shader&> (const_cast<const Material&> (*this).Shader (index));
}

/*
    ���������� / �������� ��������
*/

void Material::InsertPass (size_t index, const ShaderPointer& shader_ptr, CloneMode mode)
{
  if (index > impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::InsertPass", "index", index, impl->passes.size ());

  if (!shader_ptr)
    RaiseNullArgument ("media::rfx::Material::InsertPass", "shader_ptr");

  impl->passes.insert (impl->passes.begin () + index, Pass (shader_ptr->Clone (mode), mode));
}

size_t Material::AddPass (const ShaderPointer& shader_ptr, CloneMode mode)
{
  InsertPass (impl->passes.size (), shader_ptr, mode);

  return impl->passes.size () - 1;  
}

void Material::RemovePass (size_t index)
{
  if (index >= impl->passes.size ())
    return;
    
  impl->passes.erase (impl->passes.begin () + index);
}

void Material::RemoveAllPasses ()
{
  impl->passes.clear ();
}

/*
    ��������� �������
*/

void Material::SetShader (size_t index, const ShaderPointer& shader_ptr, CloneMode mode)
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::SetShader", "index", index, impl->passes.size ());

  if (!shader_ptr)
    RaiseNullArgument ("media::rfx::Material::SetShader", "shader_ptr");

  impl->passes [index] = Pass (shader_ptr->Clone (mode), mode);
}

/*
    ������������� �������
*/

void Material::MovePass (size_t source_index, size_t destination_index)
{
  if (source_index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::MovePass", "source_index", source_index, impl->passes.size ());

  if (destination_index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::MovePass", "destination_index", destination_index, impl->passes.size ());
    
  Pass slot = impl->passes [source_index];
    
  impl->passes.erase (impl->passes.begin () + source_index);
  impl->passes.insert (impl->passes.begin () + destination_index, slot);
}

void Material::SwapPasses (size_t index1, size_t index2)
{
  if (index1 >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::SwapPasses", "index1", index1, impl->passes.size ());

  if (index2 >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::SwapPasses", "index2", index2, impl->passes.size ());
    
  stl::swap (impl->passes [index1], impl->passes [index2]);
}

/*
    ����� ��������
*/

int Material::FindPass (const char* name) const
{
  if (!name)
    RaiseNullArgument ("media::rfx::Material::FindPass", "name");
    
  size_t hash = strhash (name);
  
  for (PassArray::const_iterator i=impl->passes.begin (), end=impl->passes.end (); i!=end; ++i)
    if (i->shader->NameHash () == hash)
      return i - impl->passes.begin ();

  return -1;
}

/*
    ����� �������
*/

Material::ConstShaderPointer Material::FindShader (const char* name) const
{
  int pass_index = FindPass (name);
  
  if (pass_index == -1)
    return 0;
    
  return impl->passes [pass_index].shader;
}

Material::ShaderPointer Material::FindShader (const char* name)
{
  return xtl::const_pointer_cast<media::rfx::Shader> (const_cast<const Material&> (*this).FindShader (name));
}

/*
    ���������� / ���������� ��������
*/

media::rfx::PassState Material::PassState (size_t index) const
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::PassState", "index", index, impl->passes.size ());
    
  return impl->passes [index].state;
}

void Material::SetPassState (size_t index, media::rfx::PassState state)
{
  if (index >= impl->passes.size ())
    RaiseOutOfRange ("media::rfx::Material::SetPassState", "index", index, impl->passes.size ());

  switch (state)
  {
    case PassState_Enabled:
    case PassState_Disabled:
      break;
    default:
      RaiseInvalidArgument ("media::rfx::Material::SetPassState", "state", state);
      break;
  }

  impl->passes [index].state = state;
}

/*
    ������������ ��������
*/

void Material::VisitEachPass (Visitor& visitor)
{
  for (PassArray::iterator i=impl->passes.begin (), end=impl->passes.end (); i!=end; ++i)
    if (i->state)
      i->shader->Accept (visitor);
}

/*
    �������� ���������� ������� ���������
    ��������� ������������ ��� ���������� "���" �� ���� ��������
*/

bool Material::IsEnabled (ShaderPin pin) const
{
  if (pin < 0 || pin >= ShaderPin_Num)
    RaiseInvalidArgument ("media::rfx::Material::IsEnabled", "pin", pin);

  for (PassArray::const_iterator i=impl->passes.begin (), end=impl->passes.end (); i!=end; ++i)
    if (i->shader->IsEnabled (pin))
      return true;

  return false;
}

/*
    ��������� ������ ����������
*/

void Material::SetSortGroup (int group)
{
  impl->sort_group = group;
}

int Material::SortGroup () const
{
  return impl->sort_group;
}

/*
    ������� ���������
*/

void Material::Clear ()
{
  RemoveAllPasses ();
  impl->name.clear ();
}

/*
    �����
*/

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);  
}

namespace media
{

namespace rfx
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
