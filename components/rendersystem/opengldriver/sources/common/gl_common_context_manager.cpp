#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    ����������: ���������� ������ OpenGL
*/

struct OpenGLExceptionTag;

typedef DerivedException<Exception, OpenGLExceptionTag> OpenGLException;

/*
    �������� ���������� ��������� OpenGL
*/

struct ContextImpl;

typedef size_t                                ContextData [ContextDataTable_Num][CONTEXT_DATA_TABLE_SIZE];
typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;   //��������� �� ������� ������
typedef xtl::intrusive_ptr<ContextImpl>       ContextImplPtr; //��������� �� ���������� ���������
typedef stl::hash_map<size_t, ContextImplPtr> ContextMap;     //����� ������������ ��������� OpenGL � ������� ������

struct ContextImpl: public xtl::reference_counter
{
  Context      context;           //�������� OpenGL
  SwapChainPtr master_swap_chain; //������� ������� ������, ��������� � ����������
  ContextData  data;              //��������� ������ ��������� 

  ContextImpl (ISwapChain* swap_chain) : context (swap_chain), master_swap_chain (swap_chain)
  {
    Init ();
  }

  ContextImpl (ISwapChain* swap_chain, const Context& shared_context) : context (swap_chain, shared_context), master_swap_chain (swap_chain)
  {
    Init ();
  }
  
    //�������������
  void Init ()
  {
    memset (data, 0, sizeof (data));
  }
  
    //��������� ��������� ������
  void SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::SetContextData";
    
    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);
      
    if (element_id >= CONTEXT_DATA_TABLE_SIZE)
      RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);
      
    data [table_id][element_id] = value;
  }

    //������ ��������� ������
  size_t GetContextData (ContextDataTable table_id, size_t element_id)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextImpl::GetContextData";

    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument (METHOD_NAME, "table_id", table_id);

    if (element_id >= CONTEXT_DATA_TABLE_SIZE)
      RaiseOutOfRange (METHOD_NAME, "element_id", element_id, CONTEXT_DATA_TABLE_SIZE);

    return data [table_id][element_id];
  }
  
    //������� ������� ��������� ������
  void ClearContextData (ContextDataTable table_id)
  {
    if (table_id < 0 || table_id >= ContextDataTable_Num)
      RaiseInvalidArgument ("render::low_level::opengl::ContextManager::ClearContextData", "table_id", table_id);

    memset (data [table_id], 0, sizeof (data [table_id]));
  }
  
    //������� ���� ������ ��������� ������
  void ClearContextData ()
  {
    memset (data, 0, sizeof (data));
  }
};

}

/*
    �������� ���������� ContextManager
*/

struct ContextManager::Impl: public xtl::reference_counter
{
  LogHandler          log_handler;                //���������� ����������������
  ContextMap          context_map;                //����� ����������� ������� ������ �� ��������
  ContextImpl*        current_context;            //������� ��������
  ISwapChain*         current_draw_swap_chain;    //������� ������ ��� ���������
  ISwapChain*         current_read_swap_chain;    //������� ������ ��� ������
  size_t              current_context_id;         //������������� �������� ��������� (0 - �����������)
  size_t              next_context_id;            //����� ���������� ������������ ���������
  Trackable::SlotType on_destroy_draw_swap_chain; //���������� �������� ������� ������ ��� ��������� 
  Trackable::SlotType on_destroy_read_swap_chain; //���������� �������� ������� ������ ��� ������
  
  Impl (const LogHandler& in_log_handler) :
    log_handler (in_log_handler),
    current_context (0),
    current_draw_swap_chain (0),
    current_read_swap_chain (0),
    current_context_id (0),
    next_context_id (1),
    on_destroy_draw_swap_chain (xtl::bind (&Impl::RestoreContext, this)),
    on_destroy_read_swap_chain (xtl::bind (&Impl::RestoreContext, this))
  {}
 
    //�������� ���������
  size_t CreateContext (ISwapChain* swap_chain)
  {
    if (!swap_chain)
      RaiseNullArgument ("render::low_level::opengl::ContextManager::CreateContext", "swap_chain");

    ContextImplPtr new_context;

    if (context_map.empty ()) new_context = ContextImplPtr (new ContextImpl (swap_chain), false);
    else                      new_context = ContextImplPtr (new ContextImpl (swap_chain, context_map.begin ()->second->context), false);

    size_t new_context_id = next_context_id;

    context_map.insert_pair (new_context_id, new_context);

    next_context_id++;

    return new_context_id;
  }
  
    //�������� ���������
  void DeleteContext (size_t context_id)
  {
    context_map.erase (context_id);

    if (context_id == current_context_id)
      RestoreContext ();
  }
  
    //��������� �������� ���������
  void SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::SetContext";
    
    if (context_id == current_context_id && draw_swap_chain == current_draw_swap_chain && read_swap_chain == current_read_swap_chain)
      return;      

      //����� ���������
      
    ContextMap::iterator iter = context_map.find (context_id);
    
    if (iter == context_map.end ())
      RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

    ContextImplPtr context = iter->second;

      //�������� ������������� ��������� � ��������� ������

    if (!context->context.IsCompatible (draw_swap_chain))
      RaiseInvalidArgument (METHOD_NAME, "draw_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

    if (draw_swap_chain != read_swap_chain && !context->context.IsCompatible (read_swap_chain))
      RaiseInvalidArgument (METHOD_NAME, "read_swap_chain", "ISwapChain instance", "Swap chain incompatible with context");

      //����������� ������������ �������� ������� ������

    Trackable *draw_swap_chain_trackable = cast_object<Trackable> (draw_swap_chain, METHOD_NAME, "draw_swap_chain"),
              *read_swap_chain_trackable = cast_object<Trackable> (read_swap_chain, METHOD_NAME, "read_swap_chain");

    draw_swap_chain_trackable->RegisterDestroyHandler (on_destroy_draw_swap_chain);
    read_swap_chain_trackable->RegisterDestroyHandler (on_destroy_read_swap_chain);

      //���������� ������

    current_context         = &*iter->second;
    current_draw_swap_chain = draw_swap_chain;
    current_read_swap_chain = read_swap_chain;
    current_context_id      = context_id;
  }

    //������������ ��������� (����� �������� ��������� ��� ������� ������)
  void RestoreContext ()
  {
      //���� ������� �������� �� ����� - ������������� � �������� ������� ������� ������ �������� ������� ������ ���������
    
    ContextMap::iterator iter = context_map.find (current_context_id);

    if (iter != context_map.end ())
    {
      current_draw_swap_chain = current_read_swap_chain = &*current_context->master_swap_chain;

      return;
    }
    
      //���� ��� ����� ������� ��������
      
    if (context_map.empty ())
    {
        //���� ������� ��� ��������� - ������������ �� ��������

      current_draw_swap_chain = 0;
      current_read_swap_chain = 0;
      current_context         = 0;
      current_context_id      = 0;

      return;
    }

      //�������� � �������� �������� ������ �������� �� ������ ��������� ������ � ��� �������� �������� ������

    ContextImplPtr context = context_map.begin ()->second;

    current_context         = &*context;
    current_read_swap_chain = &*context->master_swap_chain;
    current_draw_swap_chain = current_read_swap_chain;
    current_context_id      = context_map.begin ()->first;
  }
  
    //��������� �������� ���������
  void MakeContextCurrent ()
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::MakeContextCurrent";

    if (!current_context)
      RaiseInvalidOperation (METHOD_NAME, "Null active context");

    try
    {
      current_context->context.MakeCurrent (current_draw_swap_chain, current_read_swap_chain);
    }
    catch (common::Exception& exception)
    {
      exception.Touch (METHOD_NAME);

      throw;
    }
  }
  
    //�������� �������� �� ������� �������� ��������
  bool IsContextCurrent ()
  {
    return current_context && current_context->context.IsCurrent (current_draw_swap_chain, current_read_swap_chain);
  }
  
    //�������� ������� ������ ����������� � ����������
  ISwapChain* CreateCompatibleSwapChain (size_t context_id)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::ContextManager::CreateCompatibleSwapChain";

      //����� ���������

    ContextMap::iterator iter = context_map.find (context_id);

    if (iter == context_map.end ())
      RaiseInvalidArgument (METHOD_NAME, "context_id", context_id);

    ContextImplPtr context = iter->second;
    
      //�������� ������� ������
      
    return SwapChainManager::CreatePBuffer (context->master_swap_chain.get ());
  }
  
    //�������� ������������� ��������� � ������� ������
  bool IsCompatible (size_t context_id, ISwapChain* swap_chain)
  {
      //����� ���������

    ContextMap::iterator iter = context_map.find (context_id);

    if (iter == context_map.end ())
      return false;

    return iter->second->context.IsCompatible (swap_chain);
  }
};

/*
    ������������ / ���������� / ������������
*/

ContextManager::ContextManager (const LogHandler& log_handler)
  : impl (new Impl (log_handler))
{
}

ContextManager::ContextManager (const ContextManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

ContextManager::~ContextManager ()
{
  release (impl);
}

ContextManager& ContextManager::operator = (const ContextManager& manager)
{  
  addref (manager.impl);
  release (impl);

  impl = manager.impl;

  return *this;
}

/*
    �������� / �������� ���������
*/

size_t ContextManager::CreateContext (ISwapChain* swap_chain)
{
  return impl->CreateContext (swap_chain);
}

void ContextManager::DeleteContext (size_t context_id)
{
  impl->DeleteContext (context_id);
}

/*
    �������� ������� ������ ����������� � ����������
*/

ISwapChain* ContextManager::CreateCompatibleSwapChain (size_t context_id)
{
  return impl->CreateCompatibleSwapChain (context_id);
}

/*
    ����� �������� ���������
*/

void ContextManager::SetContext (size_t context_id, ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
{
  impl->SetContext (context_id, draw_swap_chain, read_swap_chain);
}

ISwapChain* ContextManager::GetDrawSwapChain () const
{
  return impl->current_draw_swap_chain;
}

ISwapChain* ContextManager::GetReadSwapChain () const
{
  return impl->current_read_swap_chain;
}

size_t ContextManager::GetContextId () const
{
  return impl->current_context_id;
}

/*
    ��������� �������� ���������
*/

void ContextManager::MakeContextCurrent () const
{
  impl->MakeContextCurrent ();
}

bool ContextManager::IsContextCurrent () const
{
  return impl->IsContextCurrent ();
}

/*
    ������ � �������� ��������� ������ ���������
*/

void ContextManager::SetContextData (ContextDataTable table_id, size_t element_id, size_t value)
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::SetContextData", "Null active context");

  impl->current_context->SetContextData (table_id, element_id, value);
}

size_t ContextManager::GetContextData (ContextDataTable table_id, size_t element_id) const
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::GetContextData", "Null active context");

  return impl->current_context->GetContextData (table_id, element_id);
}

void ContextManager::ClearContextData (ContextDataTable table_id)
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");
    
  impl->current_context->ClearContextData (table_id);
}

void ContextManager::ClearContextData ()
{
  if (!impl->current_context)
    RaiseInvalidOperation ("render::low_level::opengl::ContextManager::ClearContextData", "Null active context");

  impl->current_context->ClearContextData ();
}

/*
    ��������� ������ �� ������� ����������, ��������� �� �������� ������� ������
*/

const char* ContextManager::GetSwapChainsExtensionString () const
{
  MakeContextCurrent ();

  return Context::GetSwapChainExtensionString ();
}

/*
    �������� ������������� ��������� � ������� ������
*/

bool ContextManager::IsCompatible (size_t context_id, ISwapChain* swap_chain) const
{
  return impl->IsCompatible (context_id, swap_chain);
}

/*
    �������� ������������� ���������� ����������
*/

bool ContextManager::IsCompatible (const ContextManager& manager) const
{
  return impl == manager.impl;
}

/*
    ����������������
*/

void ContextManager::LogPrintf (const char* format, ...) const
{
  va_list list;
  
  va_start   (list, format);  
  LogVPrintf (format, list);
}

void ContextManager::LogVPrintf (const char* format, va_list args) const
{
  if (!format)
    return;

  try
  {
    impl->log_handler (common::vformat (format, args).c_str ());
  }
  catch (...)
  {
    //��������� ��� ����������
  }
}

/*
    �������� ������ OpenGL
*/

void ContextManager::CheckErrors (const char* source) const
{
  if (!source)
    source = "render::low_level::ContextManager::CheckErrors";
    
  MakeContextCurrent ();
  
  GLenum error = glGetError ();
  
  switch (error)
  {
    case GL_NO_ERROR:
      break;
    case GL_INVALID_ENUM:
      Raise<OpenGLException> (source, "OpenGL error: invalid enum");
      break;
    case GL_INVALID_VALUE:
      Raise<OpenGLException> (source, "OpenGL error: invalid value");
      break;
    case GL_INVALID_OPERATION:
      Raise<OpenGLException> (source, "OpenGL error: invalid operation");
      break;
    case GL_STACK_OVERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack overflow");
      break;
    case GL_STACK_UNDERFLOW:
      Raise<OpenGLException> (source, "OpenGL error: stack underflow");
      break;
    case GL_OUT_OF_MEMORY:
      Raise<OpenGLException> (source, "OpenGL error: out of memory");
      break;
    default:
      Raise<OpenGLException> (source, "OpenGL error: code=%d", error);
      break;
  }
}

void ContextManager::RaiseError (const char* source) const
{
  if (!source)
    return;
    
  CheckErrors (source);
  
  RaiseInvalidOperation (source, "Invalid operation");
}
