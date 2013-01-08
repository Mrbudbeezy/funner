#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ���������
*/

const size_t SHADER_LAYOUTS_TABLE_SIZE   = 32; //������������� ���������� ��������
const size_t MAX_SHADER_ATTRIBUTES_COUNT = 32; //����������� ���������� ���������� ��������� ��������� (���������� ��������� int)

/*
    ���������� ��������� ������
*/

//��������� ������� � ������������� OpenGL
struct InputLayout::GlVertexAttribute
{
  VertexAttributeSemantic semantic;   //��������� ��������
  size_t                  name_index; //������ ����� �������� (� ������ ������������ �������� = 0)
  size_t                  components; //���������� ���������
  GLenum                  type;       //��� ���������
  size_t                  offset;     //�������� ������������ ������ ���������� ������
  size_t                  stride;     //��� ����� ����������
};

//������ ��������� ��������� � ������������� OpenGL
struct InputLayout::GlVertexAttributeGroup
{
  size_t             slot;                     //����� ����� � ��������� �������
  size_t             std_attributes_count;     //���������� ����������� ��������� ���������
  size_t             shader_attributes_count;  //���������� ��������� ��������� ���������
  GlVertexAttribute* attributes;               //��������� ��������
};

/// ������������ ��������� ���������
struct InputLayout::ShaderAttributeLayout: public xtl::reference_counter
{
  typedef stl::vector<int> LocationArray;

  LocationArray        locations;             //������� ��������� ��������� ���������
  xtl::auto_connection on_dictionary_destroy; //���������� � ������������ �������� �������
  size_t               hash;                  //��� ������������
  size_t               used_semantics_mask;   //����� �������������� ��������

///�����������
  ShaderAttributeLayout (VertexAttributeDictionary& dictionary, size_t slots_count, const GlVertexAttributeGroup* groups, const common::StringArray& names)
    : used_semantics_mask ()
  {
    try
    {
        //������ ���������� ��������� ��������� � �������� ������������ ����������

      const GlVertexAttributeGroup* group = groups;

      size_t shader_attributes_count = 0;

      for (size_t i=0; i<slots_count; i++, group++)
        shader_attributes_count += group->shader_attributes_count;

      locations.reserve (shader_attributes_count);

        //���������� ������ ��������� ���������

      group = groups;

      for (size_t i=0; i<slots_count; i++, group++)
      {      
        if (!group->shader_attributes_count)
          continue;

        const GlVertexAttribute* attribute = group->attributes + group->std_attributes_count;

        for (size_t j=0; j<group->shader_attributes_count; j++, attribute++)
        {
          int location = dictionary.FindAttribute (names [attribute->name_index]); //����� ���� -1

          if (location >= 0)
          {
            if (location >= MAX_SHADER_ATTRIBUTES_COUNT)
              throw xtl::format_not_supported_exception ("", "Vertex attribute '%s' location %d is greater than available attributes count %d", names [attribute->name_index], location, MAX_SHADER_ATTRIBUTES_COUNT);

            used_semantics_mask |= 1 << location;
          }

          locations.push_back (location);
        }
      }

        //������ ����

      hash = locations.empty () ? 0 : common::crc32 (&locations [0], sizeof (int) * locations.size ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::low_level::opengl::InputLayout::ShaderAttributeLayout::ShaderAttributeLayout");
      throw;
    }
  }
};

namespace
{

//��������� ������� � GL �������������� ���������
struct VertexAttributeWithSemantic
{
  const VertexAttribute*  attribute;   //�������
  VertexAttributeSemantic gl_semantic; //��������������� ���������

  VertexAttributeWithSemantic (const VertexAttribute* in_attribute, VertexAttributeSemantic in_gl_semantic)
    : attribute (in_attribute)
    , gl_semantic (in_gl_semantic)
  {
  }
};

//�������� ��������� ��������� ���������
class VertexAttributeComparator
{
  public:
    VertexAttributeComparator (size_t in_position_slot) : position_slot (in_position_slot) {}

    bool operator () (const VertexAttributeWithSemantic& va1, const VertexAttributeWithSemantic& va2) const
    {
      size_t va1_slot = GetSlotIndex (va1.attribute->slot), va2_slot = GetSlotIndex (va2.attribute->slot);

      if (va1_slot == va2_slot)
        return va1.gl_semantic > va2.gl_semantic;

      return va1_slot < va2_slot;
    }

  private:
    size_t GetSlotIndex (size_t slot) const
    {
      static size_t MAX_SLOT_INDEX = DEVICE_VERTEX_BUFFER_SLOTS_COUNT + 1;

      return slot != position_slot ? slot : MAX_SLOT_INDEX;
    }

  private:  
    size_t position_slot;
};

//������� ������� ��������� ��������� ���������
class VertexAttributeSemanticTraits
{
  typedef common::Singleton<VertexAttributeSemanticTraits> ThisSingleton;
  public:
      //�����������
    VertexAttributeSemanticTraits ()
    {
        //������� ������� �������������

      memset (type_compatibility, 0, sizeof type_compatibility);
      memset (format_compatibility, 0, sizeof format_compatibility);

        //��������� ������ �������������

      Set (VertexAttributeSemantic_Position, InputDataType_Short);
      Set (VertexAttributeSemantic_Position, InputDataType_Int);
      Set (VertexAttributeSemantic_Position, InputDataType_Float);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector2);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector3);
      Set (VertexAttributeSemantic_Position, InputDataFormat_Vector4);
      
      Set (VertexAttributeSemantic_Normal, InputDataType_Byte);
      Set (VertexAttributeSemantic_Normal, InputDataType_Short);
      Set (VertexAttributeSemantic_Normal, InputDataType_Int);
      Set (VertexAttributeSemantic_Normal, InputDataType_Float);
      Set (VertexAttributeSemantic_Normal, InputDataFormat_Vector3);
      
      Set (VertexAttributeSemantic_Color, InputDataType_Byte);
      Set (VertexAttributeSemantic_Color, InputDataType_UByte);
      Set (VertexAttributeSemantic_Color, InputDataType_Short);
      Set (VertexAttributeSemantic_Color, InputDataType_UShort);
      Set (VertexAttributeSemantic_Color, InputDataType_Int);
      Set (VertexAttributeSemantic_Color, InputDataType_UInt);
      Set (VertexAttributeSemantic_Color, InputDataType_Float);
      Set (VertexAttributeSemantic_Color, InputDataFormat_Vector3);
      Set (VertexAttributeSemantic_Color, InputDataFormat_Vector4);
      
      for (int i=VertexAttributeSemantic_TexCoord0; i<=VertexAttributeSemantic_TexCoord7; i++)
      {
        VertexAttributeSemantic semantic = (VertexAttributeSemantic)i;
        
        Set (semantic, InputDataType_Short);
        Set (semantic, InputDataType_Int);
        Set (semantic, InputDataType_Float);
        Set (semantic, InputDataFormat_Vector1);
        Set (semantic, InputDataFormat_Vector2);
        Set (semantic, InputDataFormat_Vector3);
        Set (semantic, InputDataFormat_Vector4);
      }
    }  
  
      //�������� ������������� ��������� � ���� ������ ���������
    static bool IsCompatible (VertexAttributeSemantic semantic, InputDataType type)
    {
      return ThisSingleton::Instance ()->type_compatibility [semantic][type];
    }

      //�������� ������������� ��������� � ������� ���������
    static bool IsCompatible (VertexAttributeSemantic semantic, InputDataFormat format)
    {
      return ThisSingleton::Instance ()->format_compatibility [semantic][format];
    }

  private:
    void Set (VertexAttributeSemantic semantic, InputDataType type)
    {
      type_compatibility [semantic][type] = true;
    }
    
    void Set (VertexAttributeSemantic semantic, InputDataFormat format)
    {
      format_compatibility [semantic][format] = true;
    }  

  private:
    bool type_compatibility [VertexAttributeSemantic_Num][InputDataType_Num];
    bool format_compatibility [VertexAttributeSemantic_Num][InputDataFormat_Num];
};
  
}

/*
    ����������� / ����������
*/

InputLayout::InputLayout (const ContextManager& context_manager, const InputLayoutDesc& desc, size_t in_tex_units_count)
  : ContextObject (context_manager)
  , shader_layouts (0)
  , shader_attribute_names (0, 0)
  , tex_units_count (in_tex_units_count)
{
  try
  {
    SetDesc (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputLayout::InputLayout");
    throw;
  }
}

InputLayout::~InputLayout ()
{
}

/*
    ������ � �������������� ��������� ���������
*/

InputLayout::ShaderAttributeLayout& InputLayout::GetShaderLayout (VertexAttributeDictionary& dictionary)
{
  size_t id = dictionary.GetId ();

  ShaderAttributeLayoutMap::iterator iter = shader_layouts.find (id);

  if (iter != shader_layouts.end ())
    return *iter->second;

  ShaderAttributeLayoutPtr layout (new ShaderAttributeLayout (dictionary, vertex_attribute_groups.size (), &vertex_attribute_groups [0], shader_attribute_names), false);

  layout->on_dictionary_destroy = dictionary.GetTrackable ().connect_tracker (xtl::bind (&InputLayout::RemoveShaderLayout, this, id));

  if (shader_layouts.empty ())
    shader_layouts.resize (SHADER_LAYOUTS_TABLE_SIZE);

  shader_layouts.insert_pair (id, layout);

  return *layout;
}

void InputLayout::RemoveShaderLayout (size_t id)
{
  shader_layouts.erase (id);
}

/*
    ��������� �����������
*/

void InputLayout::SetDesc (const InputLayoutDesc& desc)
{ 
  static const char* METHOD_NAME = "render::low_level::opengl::InputLayout::SetDesc";

    //�������� ������������ ����������

    //TODO: instancing

  if (desc.instance_data_step_rate)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Instancing not supported. Please check InputLayoutDesc::instance_data_step_rate field");

  if (desc.is_per_instance)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Instancing not supported. Please check InputLayoutDesc::is_per_instance field");
  
    //�������������� ������������ ���������� ������
    
  GLenum new_index_data_type;
  size_t new_index_size;

  switch (desc.index_type)
  {
    case InputDataType_UByte:
      new_index_data_type = GL_UNSIGNED_BYTE;
      new_index_size      = sizeof (unsigned char);
      break;
    case InputDataType_UShort:
      new_index_data_type = GL_UNSIGNED_SHORT;
      new_index_size      = sizeof (unsigned short);
      break;
    case InputDataType_UInt:
#ifndef OPENGL_ES_SUPPORT
      new_index_data_type = GL_UNSIGNED_INT;
      new_index_size      = sizeof (unsigned int);
      break;
#endif
    case InputDataType_Byte:
    case InputDataType_Short:
    case InputDataType_Int:
    case InputDataType_Float:
      throw xtl::format_not_supported_exception (METHOD_NAME, "desc.index_type=%s unsupported", get_name (desc.index_type));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.index_type", desc.index_type);
  }

      //�������� ������ ������� ��������� ��������� � ������� ����� ��������� ���������

  GlVertexAttributeArray      new_vertex_attributes;
  GlVertexAttributeGroupArray new_vertex_attribute_groups;
  common::StringArray         new_names (0, 0);

  size_t new_semantics_mask = 0;    

    //�������� ������� ��������� ���������
    
  if (desc.vertex_attributes_count)
  {
    if (!desc.vertex_attributes)
      throw xtl::make_null_argument_exception (METHOD_NAME, "desc.vertex_attributes");

      //��������� �������� ���������
      
    const ContextCaps& caps = GetCaps ();

      //��������������� �������� � ���������� ��������� ���������

    typedef xtl::array<size_t, VertexAttributeSemantic_Num> SemanticAttribute;
    typedef stl::vector<VertexAttributeWithSemantic>        VertexAttributePtrArray;

    VertexAttributePtrArray va_ptrs;
    SemanticAttribute       semantic_attribute;
    
    static const size_t NO_ATTRIBUTE = size_t (-1);
    
    semantic_attribute.assign (NO_ATTRIBUTE);

    va_ptrs.reserve (desc.vertex_attributes_count);

    size_t shader_attributes_count = 0, shader_attribute_names_length = 0;

    for (size_t i=0; i<desc.vertex_attributes_count; i++)
    {
      const VertexAttribute& va = desc.vertex_attributes [i];

        //�������� ������������ ��������� ���������

      VertexAttributeSemantic semantic = get_semantic_by_name (va.semantic);

      if (semantic < 0)
      {
#ifndef OPENGL_ES_SUPPORT
        switch (va.type)
        {
          case InputDataType_Byte:
          case InputDataType_UByte:
          case InputDataType_Short:
          case InputDataType_UShort:
          case InputDataType_Int:
          case InputDataType_UInt:
            if (!caps.glVertexAttribIPointer_fn)
              throw xtl::format_exception<xtl::not_supported_exception> (METHOD_NAME, "Integer vertex attribute '%s' is not supported (no GL_NV_vertex_program4 extension)", va.semantic);

          case InputDataType_Float:
            if (!caps.glVertexAttribPointer_fn)
              throw xtl::format_exception<xtl::not_supported_exception> (METHOD_NAME, "Vertex attribute '%s' is not supported (no GL_ARB_vertex_shader extension)", va.semantic);

            break;
          default:
            break; //exception will be thrown below
        }

        shader_attributes_count++;
        
        shader_attribute_names_length += strlen (va.semantic) + 1;
#else
        throw xtl::format_exception<xtl::not_supported_exception> (METHOD_NAME, "Vertex attribute '%s' is not supported", va.semantic);
#endif
      }
      else
      {
        switch (semantic)
        {
          case VertexAttributeSemantic_Position:
          case VertexAttributeSemantic_Normal:
          case VertexAttributeSemantic_Color:
          case VertexAttributeSemantic_TexCoord0:
            break;
          case VertexAttributeSemantic_TexCoord1:
          case VertexAttributeSemantic_TexCoord2:
          case VertexAttributeSemantic_TexCoord3:
          case VertexAttributeSemantic_TexCoord4:
          case VertexAttributeSemantic_TexCoord5:
          case VertexAttributeSemantic_TexCoord6:
          case VertexAttributeSemantic_TexCoord7:
          {
            if (!caps.has_arb_multitexture)
              throw xtl::format_not_supported_exception (METHOD_NAME, "Bad desc.vertex_attribute[%u].semantic=%s (GL_ARB_multitexture & GL_VERSION_1_3 not supported)",
                                 i, va.semantic);

            size_t tex_unit = semantic - VertexAttributeSemantic_TexCoord0;

            if (tex_unit >= tex_units_count)
              throw xtl::format_not_supported_exception (METHOD_NAME, "Bad desc.vertex_attribute[%u].semantic=%s (only %u texture units supported)",
                                 i, va.semantic, tex_units_count);

            break;
          }
          default:
            throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].semantic=%s", i, va.semantic);
        }
      }

      new_semantics_mask |= 1 << semantic;

        //��������� ��������� ��������� ��������� ���������           

      if (semantic_attribute [semantic] != NO_ATTRIBUTE)
        throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].semantic=%s (semantic has been "
          "already defined in attribute %u)", i, va.semantic, semantic_attribute [semantic]);

      semantic_attribute [semantic] = i;

        //�������� ������������ ������� ������ � ���� ���������

      switch (va.format)
      {
        case InputDataFormat_Value:
        case InputDataFormat_Vector2:
        case InputDataFormat_Vector3:
        case InputDataFormat_Vector4:
          if (!VertexAttributeSemanticTraits::IsCompatible (semantic, va.format))
            throw xtl::format_exception<xtl::not_supported_exception> (METHOD_NAME, "Bad desc.vertex_attribute[%u] (semantic %s incompatible with format %s)", i,
                                          va.semantic, get_name (va.format));

          break;
        default:
          throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].format=%d", i, va.format);
      }
      
      switch (va.type)
      {
        case InputDataType_Byte:
        case InputDataType_UByte:
        case InputDataType_Short:
        case InputDataType_UShort:
        case InputDataType_Int:
        case InputDataType_UInt:
        case InputDataType_Float:
          if (!VertexAttributeSemanticTraits::IsCompatible (semantic, va.type))
            throw xtl::format_exception<xtl::not_supported_exception> (METHOD_NAME, "Bad desc.vertex_attribute[%u] (semantic %s incompatible with type %s)", i,
                                          va.semantic, get_name (va.type));

          break;
        default:
          throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument desc.vertex_attributes[%u].type=%d", i, va.type);
      }
      
        //�������� ������������ ������ �����

      if (va.slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
        throw xtl::format_exception<xtl::bad_range> (METHOD_NAME, "Argument desc.vertex_attributes[%u].slot=%u is out of range [0;%u)",
                                            i, va.slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

        //���������� ��������� �� ������� � ������

      va_ptrs.push_back (VertexAttributeWithSemantic (&va, semantic));    
    }
    
      //���� ����� ��������� ��������� ����������� ������� � ���������� VertexAttributeSemantic_Position - ��������� ����������
      
    size_t position_attribute = semantic_attribute [VertexAttributeSemantic_Position];

    if (position_attribute == NO_ATTRIBUTE)
      throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument desc.vertex_attributes. Attribute with semantic 'VertexAttributeSemantic_Position' not found");

      //���������� ��������� ��������� ��� ����������� ��������� � �������� OpenGL

    stl::sort (va_ptrs.begin (), va_ptrs.end (), VertexAttributeComparator (desc.vertex_attributes [position_attribute].slot));

      //�������������� ��������� ���������

    new_names.Reserve (shader_attributes_count);
    new_names.ReserveBuffer (shader_attribute_names_length);

    new_vertex_attributes.resize (desc.vertex_attributes_count);
    new_vertex_attribute_groups.reserve (DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

    size_t current_slot = DEVICE_VERTEX_BUFFER_SLOTS_COUNT + 1; //����� �������� ����� (��� ������������ ��������� ������ ��������� ���������)

    GlVertexAttributeGroup* current_group = 0;

    for (size_t i=0; i<desc.vertex_attributes_count; i++)
    {
      const VertexAttribute& va    = *va_ptrs [i].attribute;
      GlVertexAttribute&     gl_va = new_vertex_attributes [i];

        //������������ ��������� ����� ������

      if (current_slot != va.slot)
      {
        new_vertex_attribute_groups.push_back ();

        GlVertexAttributeGroup& new_group = new_vertex_attribute_groups.back ();
        
        new_group.slot                    = va.slot;
        new_group.attributes              = &gl_va;
        new_group.std_attributes_count    = 0;
        new_group.shader_attributes_count = 0;
        current_slot                      = va.slot;
        current_group                     = &new_group;
      }

        //�������������� ���������� ��������

      VertexAttributeSemantic semantic = va_ptrs [i].gl_semantic;

      gl_va.semantic = semantic;

      if (semantic < 0)
      {
        new_names.Add (va.semantic);

        gl_va.name_index = new_names.Size () - 1;
       
        current_group->shader_attributes_count++;
      }
      else
      {
        gl_va.name_index = 0;

        current_group->std_attributes_count++;
      }

      switch (va.format)
      {
        case InputDataFormat_Value:   gl_va.components = 1; break;
        case InputDataFormat_Vector2: gl_va.components = 2; break;
        case InputDataFormat_Vector3: gl_va.components = 3; break;
        case InputDataFormat_Vector4: gl_va.components = 4; break;
          break;
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Internal error at vertex attribute format convertation");
      }
      
      size_t type_size = 0;

      switch (va.type)
      {
        case InputDataType_Byte:
          gl_va.type = GL_BYTE;
          type_size  = sizeof (char);
          break;
        case InputDataType_UByte:
          gl_va.type = GL_UNSIGNED_BYTE;
          type_size  = sizeof (unsigned char);
          break;
        case InputDataType_Short:
          gl_va.type = GL_SHORT;
          type_size  = sizeof (short);
          break;
        case InputDataType_UShort:
          gl_va.type = GL_UNSIGNED_SHORT;
          type_size  = sizeof (unsigned short);
          break;
#ifndef OPENGL_ES_SUPPORT          
        case InputDataType_Int:
          gl_va.type = GL_INT;
          type_size  = sizeof (int);
          break;
        case InputDataType_UInt:
          gl_va.type = GL_UNSIGNED_INT;
          type_size  = sizeof (unsigned int);
          break;
#endif
        case InputDataType_Float:
          gl_va.type = GL_FLOAT;
          type_size  = sizeof (float);
          break;
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Internal error at vertex attribute type convertation");
      }
      
      gl_va.offset = va.offset;
      gl_va.stride = va.stride ? va.stride : type_size * gl_va.components;
    }

      //��������� �������� ����� ��������� ���������

    if (new_vertex_attribute_groups.empty ())
      throw xtl::format_operation_exception (METHOD_NAME, "Internal error at build vertex attribute groups");
  }  

    //��������� �������� ��������� �������

  index_data_type     = new_index_data_type;
  index_size          = new_index_size;
  index_buffer_offset = desc.index_buffer_offset;

  new_names.Swap (shader_attribute_names);
  new_vertex_attributes.swap (vertex_attributes);
  new_vertex_attribute_groups.swap (vertex_attribute_groups);

  attributes_hash = crc32 (&vertex_attributes.front (), vertex_attributes.size () * sizeof (GlVertexAttribute));

  used_semantics_mask = new_semantics_mask;
  
    //���������� �� ��������� ��������� ������

  StageRebindNotify (Stage_Input);
}

/*
    ��������� ��������� � �������� OpenGL
*/

namespace
{

void set_client_capability (GLenum capability, size_t current_mask, size_t required_mask, size_t pos)
{
  size_t mask = 1 << pos;

  if (current_mask & ~required_mask & mask)
  {
    glDisableClientState (capability);
    return;
  }

  if (required_mask & ~current_mask & mask)
  {
    glEnableClientState (capability);
    return;
  }
}

}

void InputLayout::BindVertexAttributes (size_t base_vertex, BufferPtr* vertex_buffers, ShaderAttributeLayout* shader_layout)
{
  static const char* METHOD_NAME = "render::low_level::opengl::InputLayout::BindVertexAttributes";

  const ContextCaps& caps = GetCaps ();

    //�������� ����������� ��������� ��������� �������
  
  for (GlVertexAttributeGroupArray::iterator iter=vertex_attribute_groups.begin (), end=vertex_attribute_groups.end (); iter!=end; ++iter)
    if (!vertex_buffers [iter->slot])
      throw xtl::format_operation_exception (METHOD_NAME, "Null vertex buffer #%u", iter->slot);
      
    //���������� �������������� � ��������� ������������ ��������� ��������
    
  const size_t current_enabled_semantics_mask = GetContextCacheValue (CacheEntry_EnabledSemantics);

  if (current_enabled_semantics_mask != used_semantics_mask)
  {
    set_client_capability (GL_VERTEX_ARRAY, current_enabled_semantics_mask, used_semantics_mask, VertexAttributeSemantic_Position);
    set_client_capability (GL_NORMAL_ARRAY, current_enabled_semantics_mask, used_semantics_mask, VertexAttributeSemantic_Normal);
    set_client_capability (GL_COLOR_ARRAY,  current_enabled_semantics_mask, used_semantics_mask, VertexAttributeSemantic_Color);
    
    if (caps.has_arb_multitexture)
    {
      size_t mask = 1 << VertexAttributeSemantic_TexCoord0;

      for (size_t i=0; i<tex_units_count; i++, mask <<= 1)
      {
        if (current_enabled_semantics_mask & ~used_semantics_mask & mask)
        {
          caps.glClientActiveTexture_fn (GL_TEXTURE0 + i);
          glDisableClientState          (GL_TEXTURE_COORD_ARRAY);
          continue;
        }

        if (used_semantics_mask & ~current_enabled_semantics_mask & mask)
        {
          caps.glClientActiveTexture_fn (GL_TEXTURE0 + i);
          glEnableClientState           (GL_TEXTURE_COORD_ARRAY);
          continue;
        }
      }
    }
    else
    {
      set_client_capability (GL_TEXTURE_COORD_ARRAY,  current_enabled_semantics_mask, used_semantics_mask, VertexAttributeSemantic_TexCoord0);
    }    

    SetContextCacheValue (CacheEntry_EnabledSemantics, used_semantics_mask);
  }

#ifndef OPENGL_ES_SUPPORT
  const size_t current_enabled_shader_semantics_mask = GetContextCacheValue (CacheEntry_EnabledShaderSemantics),
               used_shader_semantics_mask            = shader_layout ? shader_layout->used_semantics_mask : 0;

  if (current_enabled_shader_semantics_mask != used_shader_semantics_mask)
  {
    size_t mask = 1;

    for (size_t i=0; i<MAX_SHADER_ATTRIBUTES_COUNT; i++, mask <<= 1)
    {
      if (current_enabled_shader_semantics_mask & ~used_shader_semantics_mask & mask)
      {
	caps.glDisableVertexAttribArray_fn (i);
        continue;
      }

      if (used_shader_semantics_mask & ~current_enabled_shader_semantics_mask & mask)
      {
        caps.glEnableVertexAttribArray_fn (i);
        continue;
      }
    }

    SetContextCacheValue (CacheEntry_EnabledShaderSemantics, used_shader_semantics_mask);
  }
#endif

    //��������� ��������� �������

  int* attribute_location = shader_layout ? &shader_layout->locations [0] : (int*)0;
   
  for (GlVertexAttributeGroupArray::iterator group_iter=vertex_attribute_groups.begin (), group_end=vertex_attribute_groups.end (); group_iter!=group_end; ++group_iter)
  {
      //��������� ���������� ������
      
    Buffer& vb = *vertex_buffers [group_iter->slot];
    
    vb.Bind ();
    
    const char* base_offset = (const char*)vb.GetDataPointer ();
    
      //��������� ��������� ���������

    size_t attributes_count = group_iter->std_attributes_count + group_iter->shader_attributes_count;
      
    for (GlVertexAttribute *va_iter=group_iter->attributes, *va_end=va_iter+attributes_count; va_iter!=va_end; ++va_iter)
    {
      const GlVertexAttribute& va     = *va_iter;
      const void*              offset = base_offset + va.offset + base_vertex * va.stride; //������ ���������� �������� �������� ���������
      
      switch (va.semantic)
      {
        case VertexAttributeSemantic_Position:
          glVertexPointer (va.components, va.type, va.stride, offset);          
          break;
        case VertexAttributeSemantic_Normal:
          glNormalPointer (va.type, va.stride, offset);
          break;
        case VertexAttributeSemantic_Color:
          glColorPointer (va.components, va.type, va.stride, offset);
          break;
        case VertexAttributeSemantic_TexCoord0:
        case VertexAttributeSemantic_TexCoord1:
        case VertexAttributeSemantic_TexCoord2:
        case VertexAttributeSemantic_TexCoord3:
        case VertexAttributeSemantic_TexCoord4:
        case VertexAttributeSemantic_TexCoord5:
        case VertexAttributeSemantic_TexCoord6:
        case VertexAttributeSemantic_TexCoord7:          
        {
          if (caps.has_arb_multitexture)
          {
            size_t tex_unit = va.semantic - VertexAttributeSemantic_TexCoord0;

            caps.glClientActiveTexture_fn (GL_TEXTURE0 + tex_unit);
          }

          glTexCoordPointer (va.components, va.type, va.stride, offset);
          break;
        }
        default:
        {
          if (va.semantic >= 0 || !attribute_location)
            continue;

          int location = *attribute_location++;

          if (location < 0)
            continue;

#ifndef OPENGL_ES_SUPPORT
          switch (va.type)
          {
            case GL_BYTE:
            case GL_UNSIGNED_BYTE:
            case GL_SHORT:
            case GL_UNSIGNED_SHORT:
            case GL_INT:
            case GL_UNSIGNED_INT:
              caps.glVertexAttribIPointer_fn ((GLuint)location, va.components, va.type, va.stride, offset);
              break;
            case GL_DOUBLE:
              caps.glVertexAttribLPointer_fn ((GLuint)location, va.components, va.type, va.stride, offset);
              break;
            default:
              caps.glVertexAttribPointer_fn ((GLuint)location, va.components, va.type, GL_FALSE, va.stride, offset);
              break;
          }
#endif

          break;
        }
      }
    }
  }
}

void InputLayout::Bind
 (size_t                     base_vertex,
  size_t                     base_index,
  BufferPtr*                 vertex_buffers,
  VertexAttributeDictionary* dictionary,
  Buffer*                    index_buffer,
  IndicesLayout*             out_indices_layout)
{
  try
  {
    
      //��������� �������� ���������

    MakeContextCurrent ();  
    
      //�������� ������������� ������������� layout
      
    const size_t current_base_vertex        = GetContextCacheValue (CacheEntry_CurrentBaseVertex),  
                 current_layout_hash        = GetContextCacheValue (CacheEntry_CurrentLayoutHash),
                 current_buffers_hash       = GetContextCacheValue (CacheEntry_CurrentBuffersHash),
                 current_shader_layout_hash = GetContextCacheValue (CacheEntry_CurrentShaderLayoutHash);

    size_t vertex_buffers_id [DEVICE_VERTEX_BUFFER_SLOTS_COUNT];

    for (size_t i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      vertex_buffers_id [i] = vertex_buffers [i] ? vertex_buffers [i]->GetId () : 0;

    size_t buffers_hash = crc32 (vertex_buffers_id, sizeof vertex_buffers_id);

    ShaderAttributeLayout* shader_layout = dictionary ? &GetShaderLayout (*dictionary) : (ShaderAttributeLayout*)0;

    size_t shader_layout_hash = shader_layout ? shader_layout->hash : 0;

      //��������� ��������� ���������  

    if (current_base_vertex != base_vertex || current_layout_hash != attributes_hash || buffers_hash != current_buffers_hash || current_shader_layout_hash != shader_layout_hash)
    {
      BindVertexAttributes (base_vertex, vertex_buffers, shader_layout);

      SetContextCacheValue (CacheEntry_CurrentBaseVertex,       base_vertex);
      SetContextCacheValue (CacheEntry_CurrentLayoutHash,       attributes_hash);
      SetContextCacheValue (CacheEntry_CurrentBuffersHash,      buffers_hash);
      SetContextCacheValue (CacheEntry_CurrentShaderLayoutHash, shader_layout_hash);
    }

      //��������� ���������� ������
    
    if (out_indices_layout)
    {
        //��������� ���������� ������ � �������� OpenGL

      if (!index_buffer)
        throw xtl::format_operation_exception ("", "Null index buffer");

      index_buffer->Bind ();

        //������ �������� ������� ������� ������������ ������ ���������� ������

      out_indices_layout->type = index_data_type;
      out_indices_layout->data = (char*)index_buffer->GetDataPointer () + index_buffer_offset + base_index * index_size;
    }

      //�������� ������

    CheckErrors ("");
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::InputLayout::Bind");
    throw;
  }
}
