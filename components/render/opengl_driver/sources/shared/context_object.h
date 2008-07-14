#ifndef RENDER_GL_DRIVER_CONTEXT_OBJECT_HEADER
#define RENDER_GL_DRIVER_CONTEXT_OBJECT_HEADER

#include <xtl/common_exceptions.h>

#include <shared/object.h>
#include <shared/context_manager.h>

namespace render
{

namespace low_level
{

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ContextObject: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ContextObject (const ContextManager&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCompatible (const ContextManager&) const;
    bool IsCompatible (const ContextObject&) const;  

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
          ContextManager& GetContextManager ()       { return context_manager; }
    const ContextManager& GetContextManager () const { return context_manager; }

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MakeContextCurrent () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������� ��������� ������ �������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ContextDataTable& GetContextDataTable (Stage table_id) const;
          ContextDataTable& GetContextDataTable (Stage table_id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ��������� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsSupported (size_t context_id, const Extension& extension) const;
    bool IsSupported (const Extension& extension) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������������� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ContextCaps& GetCaps () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LogPrintf  (const char* format, ...) const;
    void LogVPrintf (const char* format, va_list args) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetValidationState (bool state); //��������� / ���������� �������� ������
    bool GetValidationState () const;
    void CheckErrors        (const char* source) const;
    void RaiseError         (const char* source) const;
    void ClearErrors        () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ������������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void StageRebindNotify (Stage);

  private:
    ContextManager context_manager;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� �������� � ��������� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class DstT, class SrcT>
DstT* cast_object (const ContextManager& owner, SrcT* ptr, const char* source, const char* argument_name);

template <class DstT, class SrcT>
DstT* cast_object (const ContextObject& owner, SrcT* ptr, const char* source, const char* argument_name);

#include <shared/detail/context_object.inl>

}

}

}

#endif
