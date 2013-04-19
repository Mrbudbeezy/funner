#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::helpers;

namespace
{

//������� ������� ���� ���������� ��������� ��������
struct ProgramParameterComparator
{
  bool operator () (const ProgramParameter* parameter1, const ProgramParameter* parameter2) const
  {
    return parameter1->slot < parameter2->slot;
  }
};

}

/*
    �������� ���������� ������� ���������� ���������
*/

typedef stl::vector<ProgramParameter>      ParameterArray;
typedef stl::vector<ProgramParameterGroup> GroupArray;

struct ProgramParametersLayout::Impl
{  
  ParameterArray parameters; //��������� ��������� ��������
  GroupArray     groups;     //������ ���������� ��������� ��������
  stl::string    names;      //����� ����������

/// �����������
  Impl (const ProgramParametersLayoutDesc& in_desc)
  {
    static const char* METHOD_NAME = "render::low_level::helpers::ProgramParametersLayout::Impl::Impl";

      //�������� ������������ ����������

    if (!in_desc.parameters)
      throw xtl::make_null_argument_exception (METHOD_NAME, "desc.parameters");
      
      /*
          ���������� ������:
            1) �������� ������������ ����������;
            2) ���������� ������� ���������� �� ��������� ��� �������������� �� ������ ������������ ������, � �������� ��� ����������;
            3) ������� ����� ������ ��� ����������.
      */
      
    typedef stl::vector<const ProgramParameter*> ParameterPtrArray;

    ParameterPtrArray src_parameters (in_desc.parameters_count);      
      
    size_t names_length = 0;    

    for (size_t i=0; i<in_desc.parameters_count; i++)
    {
      const ProgramParameter& param = in_desc.parameters [i];

      if (!param.count)
        throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "Null argument desc.parameters[%u].count", i);

      if (!param.name)
        throw xtl::format_exception<xtl::null_argument_exception> (METHOD_NAME, "Null argument desc.parameters[%u].name", i);

      if (param.slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
        throw xtl::make_range_exception (METHOD_NAME, "desc.parameters.slot", param.slot, 0u, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

      if (param.type >= ProgramParameterType_Num)
        throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument value desc.parameters[%u].type=%d", i,
          param.type);

      src_parameters [i]  = &param;
      names_length       += strlen (param.name) + 1;
    }

      //�������������� ���������� �� ��������� �� ������ ������������ ������

    stl::sort (src_parameters.begin (), src_parameters.end (), ProgramParameterComparator ());
    
      //���������� ������� ���������� � ������� ����� ����������
      
    ParameterArray new_parameters;
    GroupArray     new_groups;
    stl::string    new_names;

    new_parameters.reserve (in_desc.parameters_count);
    new_groups.reserve     (DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);
    new_names.reserve      (names_length + 1);

    size_t current_slot = DEVICE_CONSTANT_BUFFER_SLOTS_COUNT + 1; //����� �������� ����� (��� ������������ ��������� ������ ����������)
      
    for (ParameterPtrArray::iterator iter=src_parameters.begin (), end=src_parameters.end (); iter!=end; ++iter)
    {
        //���������� ���������
      
      const ProgramParameter& src_param = **iter;
      
      new_parameters.push_back (src_param);
      
        //���������� ����� ��������� � ��������� �������� ����� ��������� � ������ ���

      new_parameters.back ().name = reinterpret_cast<const char*> (new_names.size ());

      new_names += src_param.name;
      new_names += '\0';

        //������������ ��������� ����� ������

      if (current_slot != src_param.slot)
      {
        new_groups.push_back ();

        ProgramParameterGroup& new_group = new_groups.back ();
        
        new_group.slot       = src_param.slot;
        new_group.parameters = &new_parameters.back ();
        new_group.count      = 0;
        current_slot         = src_param.slot;
      }
    }  

    if (!new_groups.empty ())  
    {
        //������� ���������� ���������� � �������

      for (GroupArray::iterator iter=new_groups.begin (), end=new_groups.end ()-1; iter!=end; ++iter)
        iter->count = iter [1].parameters - iter [0].parameters;

      new_groups.back ().count = new_parameters.end () - new_groups.back ().parameters;
    }

      //�����
      
    parameters.swap (new_parameters);
    groups.swap     (new_groups);
    names.swap      (new_names);
    
      //��������� ���

    for (ParameterArray::iterator iter=parameters.begin (), end=parameters.end (); iter!=end; ++iter)
      iter->name = names.c_str () + reinterpret_cast<size_t> (iter->name);
  }
};

/*
    ����������� / ����������
*/

ProgramParametersLayout::ProgramParametersLayout (const ProgramParametersLayoutDesc& in_desc)
{
  try
  {
    impl.reset (new Impl (in_desc));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::helpers::ProgramParametersLayout");
    throw;
  }
}

ProgramParametersLayout::~ProgramParametersLayout ()
{
}

/*
    ��������� ����������
*/

size_t ProgramParametersLayout::GetParametersCount () const
{
  return impl->parameters.size ();
}

const ProgramParameter* ProgramParametersLayout::GetParameters () const
{
  if (impl->parameters.empty ())
    return 0;

  return &impl->parameters [0];
}

/*
   ��������� ������
*/

const ProgramParameterGroup& ProgramParametersLayout::GetGroup (size_t index)
{
  if (index >= impl->groups.size ())
    throw xtl::make_range_exception ("render::low_level::helpers::ProgramParametersLayout::GetGroup", "index", index, 0u, impl->groups.size ());

  return impl->groups [index];
}

size_t ProgramParametersLayout::GetGroupsCount () const
{
  return impl->groups.size ();
}

const ProgramParameterGroup* ProgramParametersLayout::GetGroups () const
{
  if (impl->groups.empty ())
    return 0;

  return &impl->groups [0];
}
