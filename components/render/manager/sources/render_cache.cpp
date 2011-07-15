#include "shared.h"

using namespace render;

/*
===================================================================================================
    CacheHolder
===================================================================================================
*/

/*
    ����������� / ����������
*/

CacheHolder::CacheHolder ()
  : state (CacheState_Reset)
  , need_update_this (true)  
{
}

CacheHolder::~CacheHolder ()
{
  DetachAllCacheSources ();
}

/*
    ���������� ������ ��������� ���������� ����
*/

void CacheHolder::AttachCacheSource (CacheHolder& source)
{
  if (IsParentOf (source))
    throw xtl::format_operation_exception ("render::CacheHolder::AttachCacheSource", "Cache source has been already attached (to this holder or it's children)");
    
  if (source.IsParentOf (*this))
    throw xtl::format_operation_exception ("render::CacheHolder::AttachCacheSource", "Cache source is a parent of this cache holder");  

  sources.push_back (&source);
  
  try
  {
    source.dependencies.push_back (this);

    InvalidateCache (false);
  }
  catch (...)
  {
    sources.pop_back ();
    throw;
  }
}

void CacheHolder::DetachCacheSource (CacheHolder& source)
{
  sources.remove (&source);
  source.dependencies.remove (this);
  
  InvalidateCache (false);
}

void CacheHolder::DetachAllCacheSources ()
{
  while (!sources.empty ())
    DetachCacheSource (*sources.back ());

  while (!dependencies.empty ())
    dependencies.back ()->DetachCacheSource (*this);
    
  InvalidateCache (false);
}

/*
    �������� �� ������ ������ ������������ �� ��������� � �����������
*/

bool CacheHolder::IsParentOf (CacheHolder& child)
{
  for (HolderList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
  {
    if (*iter == &child)
      return true;
      
    if ((*iter)->IsParentOf (child))
      return true;
  }
  
  return false;
}

/*
    ��������� ����� ������������� ���������� ���� / ��������� �����
*/

void CacheHolder::InvalidateFlags ()
{
  switch (state)
  {
    case CacheState_Valid:
    case CacheState_Broken:    
      break;
    case CacheState_Reset:
    case CacheState_Invalid:
      return;
  }

  for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
    (*iter)->InvalidateFlags ();

  state = CacheState_Invalid;
}

void CacheHolder::InvalidateCache (bool invalidate_dependencies)
{
  if (need_update_this)
    return;
    
  need_update_this = true;
  
  if (invalidate_dependencies)
  {
    for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
      (*iter)->need_update_this = true;  
  }

  InvalidateFlags ();
}

/*
    ������� � ������������� ���������� ���� ���������
*/

void CacheHolder::ResetCache ()
{
  switch (state)
  {
    case CacheState_Valid:
    case CacheState_Invalid:
    case CacheState_Broken:
      break;
    case CacheState_Reset:
      return;
  }    
  
  need_update_this = true;
      
  try
  {
    ResetCacheCore ();
  }
  catch (std::exception& e)
  {
    Log ().Printf ("Unexpected exception: %s\n    at render::CacheHolder::ResetCache", e.what ());
  }
  catch (...)
  {
    Log ().Printf ("Unexpected exception at render::CacheHolder::ResetCache");
  }
  
  state = CacheState_Reset;
  
  for (HolderList::iterator iter=dependencies.begin (), end=dependencies.end (); iter!=end; ++iter)
    (*iter)->ResetCache ();
}

/*
    ���������� ����. ���������� �������� ����� ������ �������������� �������
*/

void CacheHolder::UpdateCache ()
{
  switch (state)
  {    
    case CacheState_Invalid:
    case CacheState_Reset:    
      break;
    case CacheState_Broken:
    case CacheState_Valid:   
      return;
  }    

  for (HolderList::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
    (*iter)->UpdateCache ();
    
  if (need_update_this)
  {   
    try
    {
      UpdateCacheCore ();
    }
    catch (...)
    {
      state            = CacheState_Broken;
      need_update_this = false;

      throw;
    }
    
    need_update_this = false;
  } 
    
  state = CacheState_Valid;
}

/*
===================================================================================================
    CacheSource
===================================================================================================
*/

/*
    ���������� �������� ������� ���������� ����
*/

void CacheSource::UpdateCacheCore ()
{
}

void CacheSource::ResetCacheCore ()
{
}
