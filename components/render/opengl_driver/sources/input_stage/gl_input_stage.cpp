#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    ��������� �������� ������
*/

namespace
{

class InputStageState: public IStageState
{
  public:
      //������������
    InputStageState (InputStageState* in_main_state) : owner (0), main_state (in_main_state) {}
    InputStageState (ContextObject* in_owner) : owner (in_owner), main_state (0) {}
    
      //��������� ������������ ���������
    void SetInputLayout (InputLayout* in_layout)
    {
      if (in_layout == layout)
        return;
        
      layout = in_layout;
      
      UpdateNotify ();
    }

      //��������� ������������ ���������
    InputLayout* GetInputLayout () const
    {
      return layout.get ();
    }

      //��������� ���������� ������
    void SetVertexBuffer (unsigned int slot, Buffer* buffer)
    {
      if (buffer == vertex_buffers [slot])
        return;
        
      vertex_buffers [slot] = buffer;
      
      UpdateNotify ();
    }
    
      //��������� ���������� ������
    Buffer* GetVertexBuffer (unsigned int slot) const
    {
      return vertex_buffers [slot].get ();
    }
    
      //��������� ���� ��������� �������
    BufferPtr* GetVertexBuffers ()
    {
      return vertex_buffers.c_array ();
    }

      //��������� ���������� ������
    void SetIndexBuffer (Buffer* buffer)
    {
      if (buffer == index_buffer)
        return;
        
      index_buffer = buffer;
      
      UpdateNotify ();
    }
    
      //��������� ���������� ������
    Buffer* GetIndexBuffer () const
    {
      return index_buffer.get ();
    }
    
      //������ ���������
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //�������������� ���������
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:
      //����������� ���������
    void Copy (const InputStageState& source, const StateBlockMask& mask)
    {
      if (mask.is_layout)
        SetInputLayout (source.GetInputLayout ());

      for (unsigned int i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
        if (mask.is_vertex_buffers [i])
          SetVertexBuffer (i, source.GetVertexBuffer (i));

      if (mask.is_index_buffer)
        SetIndexBuffer (source.GetIndexBuffer ());
    }
    
      //���������� �� ��������� ��������� ������
    void UpdateNotify ()
    {
      if (!owner)
        return;
        
      owner->GetContextManager ().StageRebindNotify (Stage_Input);
    }

  private:
    typedef xtl::array<BufferPtr, DEVICE_VERTEX_BUFFER_SLOTS_COUNT> VertexBufferArray;
    typedef xtl::trackable_ptr<InputLayout>                         LayoutPtr;
    typedef xtl::trackable_ptr<InputStageState>                     InputStageStatePtr;

  private:
    ContextObject*     owner;          //�������� ��������� ������
    InputStageStatePtr main_state;     //�������� ��������� ������
    LayoutPtr          layout;         //�������� ������������ ���������
    VertexBufferArray  vertex_buffers; //��������� ������
    BufferPtr          index_buffer;   //��������� �����
};

}

/*
    �������� ���������� �������� ������ ��������� OpenGL
*/

struct InputStage::Impl: public ContextObject
{
  public:
    /*
        �����������  
    */

    Impl (const ContextManager& context_manager) : ContextObject (context_manager), state (this)
    {
        //��������� �������� ���������

      MakeContextCurrent();

      ffp_texture_units_count = GetCaps ().ffp_texture_units_count;
        
      if (ffp_texture_units_count > DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
        ffp_texture_units_count = DEVICE_VERTEX_BUFFER_SLOTS_COUNT;
        
        //������������� ��������� ������
        
      InputLayoutDesc default_layout_desc;
      
      memset (&default_layout_desc, 0 , sizeof default_layout_desc);

#ifndef OPENGL_ES_SUPPORT
      default_layout_desc.index_type = InputDataType_UInt;
#else
      default_layout_desc.index_type = InputDataType_UShort;
#endif

      default_layout = InputLayoutPtr (new InputLayout (GetContextManager (), default_layout_desc, ffp_texture_units_count), false);
      
      SetInputLayout (default_layout.get ());
        
        //�������� ������
        
      CheckErrors ("render::low_level::opengl::InputStage::Impl");
    }
    
    /*
        ��������� ������� ��������� ��������� ������
    */
    
    InputStageState& GetState () { return state; }
    
    /*
        �������� �������
    */

    IBuffer* CreateBuffer (const BufferDesc& desc, GLenum buffer_target)
    {
      try
      {
#if !defined (OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
        if (GetCaps ().has_arb_vertex_buffer_object)
        {
          return new VboBuffer (GetContextManager (), buffer_target, desc);
        }
        else
        {
          return new SystemMemoryInputBuffer (GetContextManager (), buffer_target, desc);
        }
#else
        return new VboBuffer (GetContextManager (), buffer_target, desc);
#endif
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::InputStage::Impl::CreateBuffer");
        throw;
      }
    }
    
    IBuffer* CreateSystemMemoryBuffer (const BufferDesc& desc)
    {
      return new ConstantBuffer (GetContextManager (), desc);
    }
    
    /*
        �������� InputLayout
    */
    
    IInputLayout* CreateInputLayout (const InputLayoutDesc& desc)
    {
      return new InputLayout (GetContextManager (), desc, ffp_texture_units_count);
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
    
    void SetVertexBuffer (unsigned int slot, IBuffer* in_buffer)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::InputStage::Impl::SetVertexBuffer";
      
      Buffer* buffer = cast_object<Buffer> (*this, in_buffer, METHOD_NAME, "buffer");
      
      if (buffer && !buffer->IsVertexBuffer ())
        throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument <buffer>. Incompatible vertex buffer, desc.bind_flags=%s", get_name ((BindFlag)buffer->GetBindFlags ()));
      
      if (slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
        throw xtl::make_range_exception (METHOD_NAME, "slot", slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
        
      state.SetVertexBuffer (slot, buffer);
    }  

    IBuffer* GetVertexBuffer (unsigned int slot)
    {
      if (slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
        throw xtl::make_range_exception ("render::low_level::opengl::InputStage::Impl::GetVertexBuffer", "slot", slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);
    
      return state.GetVertexBuffer (slot);
    }  

    /*
        ��������� / ��������� ���������� ������
    */

    void SetIndexBuffer (IBuffer* in_buffer)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::InputStage::Impl::SetIndexBuffer";
      
      Buffer* buffer = cast_object<Buffer> (*this, in_buffer, METHOD_NAME, "buffer");    
      
      if (buffer && !buffer->IsIndexBuffer ())
        throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Invalid argument <buffer>. Incompatible index buffer, desc.bind_flags=%s", get_name ((BindFlag)buffer->GetBindFlags ()));

      state.SetIndexBuffer (buffer);
    }
    
    IBuffer* GetIndexBuffer () { return state.GetIndexBuffer (); }
    
    /*
        ��������� ��������� ������ � �������� OpenGL
    */
      
    void Bind (unsigned int base_vertex, unsigned int base_index, IVertexAttributeDictionary* dictionary, IndicesLayout* out_indices_layout)
    {
      try
      {
        InputLayout* layout = state.GetInputLayout ();    

        if (!layout)
          layout = default_layout.get ();

        layout->Bind (base_vertex, base_index, state.GetVertexBuffers (), dictionary, state.GetIndexBuffer (), out_indices_layout);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::InputStage::Impl::Bind");
        throw;
      }
    }

  private:
    typedef xtl::com_ptr<InputLayout> InputLayoutPtr;

  private:
    InputStageState state;                   //��������� ������
    InputLayoutPtr  default_layout;          //��������� ������������ ��������� �� ���������  
    unsigned int    ffp_texture_units_count; //���������� ���������� ������ �������������� ��������� ��� FFP
};

/*
    ����������� / ����������
*/

InputStage::InputStage (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

InputStage::~InputStage()
{
}

/*
    �������� ������� ��������� ������
*/

IStageState* InputStage::CreateStageState ()
{
  return new InputStageState (&impl->GetState ());
}

/*
    �������� ��������
*/

IInputLayout* InputStage::CreateInputLayout (const InputLayoutDesc& desc)
{
  try
  {
    return impl->CreateInputLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::CreateInputLayout");
    throw;
  }
}

IBuffer* InputStage::CreateVertexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateBuffer (desc, GL_ARRAY_BUFFER);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::CreateVertexBuffer");
    throw;
  } 
}

IBuffer* InputStage::CreateIndexBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateBuffer (desc, GL_ELEMENT_ARRAY_BUFFER);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::CreateIndexBuffer");
    throw;
  } 
}

IBuffer* InputStage::CreateConstantBuffer (const BufferDesc& desc)
{
  try
  {
    return impl->CreateSystemMemoryBuffer (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::CreateConstantBuffer");
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
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::SetInputLayout");
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

void InputStage::SetVertexBuffer (unsigned int slot, IBuffer* buffer)
{
  try
  {
    impl->SetVertexBuffer (slot, buffer);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::SetVertexBuffer");
    throw;
  }
}

IBuffer* InputStage::GetVertexBuffer (unsigned int slot) const
{
  try
  {
    return impl->GetVertexBuffer (slot);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::GetVertexBuffer");
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
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::SetIndexBuffer");
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

void InputStage::Bind (unsigned int base_vertex, unsigned int base_index, IVertexAttributeDictionary* dictionary, IndicesLayout* out_indices_layout)
{
  try
  {
    impl->Bind (base_vertex, base_index, dictionary, out_indices_layout);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::InputStage::Bind");
    throw;
  }  
}

/*
    ��������� ����� ���������
*/

const char* InputStage::GetVertexAttributeSemanticName (VertexAttributeSemantic semantic)
{
  return get_semantic_name (semantic);
}
