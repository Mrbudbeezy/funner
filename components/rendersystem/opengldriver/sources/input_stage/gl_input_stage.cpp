#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ��������� �������� ������
*/

namespace
{

class InputStageState
{
  public:
      //�����������
    InputStageState ()
    {
    }
    
      //��������� ������������ ���������
    void SetInputLayout (InputLayout* in_layout)
    {
      if (in_layout == layout)
        return;
        
      layout = in_layout;
    }

      //��������� ������������ ���������
    InputLayout* GetInputLayout () const
    {
      return layout.get ();
    }

      //��������� ���������� ������
    void SetVertexBuffer (size_t slot, Buffer* buffer)
    {
      if (buffer == vertex_buffers [slot])
        return;
        
      vertex_buffers [slot] = buffer;
    }
    
      //��������� ���������� ������
    const BufferPtr& GetVertexBuffer (size_t slot) const
    {
      return vertex_buffers [slot];
    }
    
      //��������� ���� ��������� �������
    BufferPtr* GetVertexBuffers ()  /////???const???
    {
      return vertex_buffers.c_array ();
    }

      //��������� ���������� ������
    void SetIndexBuffer (Buffer* buffer)
    {
      if (buffer == index_buffer)
        return;
        
      index_buffer = buffer;
    }
    
      //��������� ���������� ������
    const BufferPtr& GetIndexBuffer () const
    {
      return index_buffer;
    }
  
  private:
    typedef xtl::array<BufferPtr, DEVICE_VERTEX_BUFFER_SLOTS_COUNT> VertexBufferArray;
    typedef xtl::trackable_ptr<InputLayout>                         LayoutPtr;

  private:
    LayoutPtr         layout;         //�������� ������������ ���������
    VertexBufferArray vertex_buffers; //��������� ������
    BufferPtr         index_buffer;   //��������� �����
};

}

/*
    �������� ���������� �������� ������ ��������� OpenGL
*/

struct InputStage::Impl: public ContextObject
{
  typedef xtl::com_ptr<InputLayout> InputLayoutPtr;

  InputStageState state;                        //��������� ������
  InputLayoutPtr  default_layout;               //��������� ������������ ��������� �� ���������  
  bool            has_arb_multitexture;         //�������������� �� GL_ARB_vertex_buffer_object
  bool            has_arb_vertex_buffer_object; //�������������� �� GL_ARB_vertex_buffer_object
  size_t          texture_units_count;          //���������� ���������� ������ �������������� ���������  

  /*
      �����������  
  */

  Impl (const ContextManager& context_manager) : ContextObject (context_manager)
  {
      //��������� �������� ���������

    MakeContextCurrent();

      //��������� ���������� � �������������� �����������
      
    static Extension ARB_multitexture         = "GL_ARB_multitexture",
                     ARB_vertex_buffer_object = "GL_ARB_vertex_buffer_object",
                     Version_1_3              = "GL_VERSION_1_3",
                     Version_1_5              = "GL_VERSION_1_5";

    has_arb_multitexture         = IsSupported (ARB_multitexture) || IsSupported (Version_1_3);
    has_arb_vertex_buffer_object = IsSupported (ARB_vertex_buffer_object) || IsSupported (Version_1_5);

    if (has_arb_multitexture)
      glGetIntegerv (GL_MAX_TEXTURE_UNITS_ARB, (GLint*)&texture_units_count);
      
    if (texture_units_count > DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      texture_units_count = DEVICE_VERTEX_BUFFER_SLOTS_COUNT;
      
      //������������� ��������� ������
      
    InputLayoutDesc default_layout_desc;
    
    memset (&default_layout_desc, 0 , sizeof default_layout_desc);

    default_layout_desc.index_type = InputDataType_UInt;

    default_layout = InputLayoutPtr (new InputLayout (GetContextManager (), default_layout_desc, texture_units_count), false);
    
    SetInputLayout (default_layout.get ());
      
      //�������� ������
      
    CheckErrors ("render::low_level::opengl::InputStage::Impl");
  }
  
  /*
      �������� ��������� � ��������� �������
  */

  IBuffer* CreateBuffer (const BufferDesc& desc, GLenum buffer_target)
  {
    try
    {
      if (has_arb_vertex_buffer_object)
      {
        return new VboBuffer (GetContextManager (), buffer_target, desc);
      }
      else
      {
        return new SystemMemoryBuffer (GetContextManager (), desc);
      }
    }
    catch (common::Exception& exception)
    {
      exception.Touch ("render::low_level::opengl::InputStage::Impl::CreateBuffer");
      throw;
    }
  }

  /*
      ��������� / ��������� InputLayout
  */

  void SetInputLayout (IInputLayout* in_layout)
  {        
    InputLayout* layout = cast_object<InputLayout> (*this, in_layout, "render::low_level::opengl::InputStage::Impl::SetInputLayout", "layout");

    state.SetInputLayout (layout);
  }
  
  IInputLayout* GetInputLayout ()
  {
    return state.GetInputLayout ();
  }
  
  /*
      ��������� / ��������� ���������� ������
  */
  
  void SetVertexBuffer (size_t slot, IBuffer* in_buffer)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::InputStage::Impl::SetVertexBuffer";
    
    Buffer* buffer = cast_object<Buffer> (*this, in_buffer, METHOD_NAME, "buffer");
    
    if (buffer && !buffer->IsVertexBuffer ())
      Raise<ArgumentException> (METHOD_NAME, "Invalid argument <buffer>. Incompatible vertex buffer, desc.bind_flags=%s", get_name ((BindFlag)buffer->GetBindFlags ()));
    
    if (slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      RaiseOutOfRange (METHOD_NAME, "slot", slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
      
    state.SetVertexBuffer (slot, buffer);
  }  

  IBuffer* GetVertexBuffer (size_t slot)
  {
    if (slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      RaiseOutOfRange ("render::low_level::opengl::InputStage::Impl::GetVertexBuffer", "slot", slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
  
    return state.GetVertexBuffer (slot).get ();
  }  

  /*
      ��������� / ��������� ���������� ������
  */

  void SetIndexBuffer (IBuffer* in_buffer)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::InputStage::Impl::SetIndexBuffer";
    
    Buffer* buffer = cast_object<Buffer> (*this, in_buffer, METHOD_NAME, "buffer");    
    
    if (buffer && !buffer->IsIndexBuffer ())
      Raise<ArgumentException> (METHOD_NAME, "Invalid argument <buffer>. Incompatible index buffer, desc.bind_flags=%s", get_name ((BindFlag)buffer->GetBindFlags ()));

    state.SetIndexBuffer (buffer);
  }
  
  IBuffer* GetIndexBuffer () { return state.GetIndexBuffer ().get (); }
  
  /*
      ��������� ��������� ������ � �������� OpenGL
  */
    
  void Bind (size_t base_vertex, size_t base_index, IndicesLayout* out_indices_layout)  
  {
    static const char* METHOD_NAME = "render::low_level::opengl::InputStage::Impl::Bind";

    InputLayout* layout = state.GetInputLayout ();    

    if (!layout)
      layout = default_layout.get ();

    layout->Bind (base_vertex, base_index, state.GetVertexBuffers (), state.GetIndexBuffer (), out_indices_layout);
  }
};

/*
    ����������� / ����������
*/

InputStage::InputStage (ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

InputStage::~InputStage()
{
}

/*
    �������� ��������
*/

IInputLayout* InputStage::CreateInputLayout (const InputLayoutDesc& desc)
{
  try
  {
    return new InputLayout (impl->GetContextManager (), desc, impl->texture_units_count);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::CreateInputLayout");
    throw;
  }
}

IBuffer* InputStage::CreateVertexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateBuffer (desc, GL_ARRAY_BUFFER);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::CreateVertexBuffer");
    throw;
  } 
}

IBuffer* InputStage::CreateIndexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateBuffer (desc, GL_ELEMENT_ARRAY_BUFFER);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::CreateIndexBuffer");
    throw;
  } 
}

IBuffer* InputStage::CreateConstantBuffer (const BufferDesc& desc)
{
  try
  {
    return new SystemMemoryBuffer (impl->GetContextManager (), desc);      
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::CreateConstantBuffer");
    throw;
  } 
}

/*
    ���������� ������������� ������� ������    
*/

void InputStage::SetInputLayout (IInputLayout* layout)
{
  try
  {
    impl->SetInputLayout (layout);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::SetInputLayout");
    throw;
  }
}

IInputLayout* InputStage::GetInputLayout () const
{
  return impl->GetInputLayout ();
}

/*
    ���������� ���������� ��������
*/

void InputStage::SetVertexBuffer (size_t slot, IBuffer* buffer)
{
  try
  {
    impl->SetVertexBuffer (slot, buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::SetVertexBuffer");
    throw;
  }
}

IBuffer* InputStage::GetVertexBuffer (size_t slot) const
{
  try
  {
    return impl->GetVertexBuffer (slot);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::GetVertexBuffer");
    throw;
  }
}

/*
    ���������� ��������� �������
*/

void InputStage::SetIndexBuffer (IBuffer* buffer)
{
  try
  {
    impl->SetIndexBuffer (buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::SetIndexBuffer");
    throw;
  }
}

IBuffer* InputStage::GetIndexBuffer () const
{
  return impl->GetIndexBuffer ();
}

/*
    ��������� ��������� ������ � �������� OpenGL
*/

void InputStage::Bind (size_t base_vertex, size_t base_index, IndicesLayout* out_indices_layout)
{
  try
  {
    impl->Bind (base_vertex, base_index, out_indices_layout);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::InputStage::Bind");
    throw;
  }  
}
