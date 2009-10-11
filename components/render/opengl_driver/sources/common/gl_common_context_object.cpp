#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    �����������
*/

ContextObject::ContextObject (const ContextManager& in_context_manager)
  : context_manager (in_context_manager)
  {}

/*
    �������� �������������
*/

bool ContextObject::IsCompatible (const ContextManager& manager) const
{
  return context_manager.IsCompatible (manager);
}

bool ContextObject::IsCompatible (const ContextObject& object) const
{
  return context_manager.IsCompatible (object.context_manager);
}

/*
    ��������� �������� ���������
*/

void ContextObject::MakeContextCurrent () const
{
  context_manager.MakeContextCurrent ();
}

bool ContextObject::TryMakeContextCurrent () const
{
  return context_manager.TryMakeContextCurrent ();
}

/*
    ������ � ��������� ��������� ������ �������� ���������
*/

const size_t* ContextObject::GetContextCache () const
{
  return context_manager.GetContextCache ();
}

size_t* ContextObject::GetContextCache ()
{
  return context_manager.GetContextCache ();
}

void ContextObject::SetContextCacheValue (size_t entry_id, size_t value)
{
  context_manager.SetContextCacheValue (entry_id, value);
}

size_t ContextObject::GetContextCacheValue (size_t entry_id) const
{
  return context_manager.GetContextCacheValue (entry_id);
}

/*
    ����������� ��������� ���������� ����������
*/

bool ContextObject::IsSupported (const Extension& extension) const
{
  return context_manager.IsSupported (extension);
}

/*
   ��������� ��������� �������������� ������������ ���������
*/

const ContextCaps& ContextObject::GetCaps () const
{
  return context_manager.GetCaps ();
}

/*
    ����������������
*/

void ContextObject::LogPrintf  (const char* format, ...) const
{
  va_list args;

  va_start (args, format);

  context_manager.LogVPrintf (format, args);
}

void ContextObject::LogVPrintf (const char* format, va_list args) const
{
  context_manager.LogVPrintf (format, args);
}

/*
    �������� ������ OpenGL
*/

void ContextObject::SetValidationState (bool state)
{
  context_manager.SetValidationState (state);  
}

bool ContextObject::GetValidationState () const
{
  return context_manager.GetValidationState ();
}

void ContextObject::CheckErrors (const char* source) const
{
  context_manager.CheckErrors (source);
}

void ContextObject::RaiseError (const char* source) const
{
  context_manager.RaiseError (source);
}

void ContextObject::ClearErrors () const
{
  context_manager.ClearErrors ();
}

/*
    ���������� � ������������� ������� ������
*/

void ContextObject::StageRebindNotify (Stage stage)
{
  context_manager.StageRebindNotify (stage);
}
