#include "shared.h"

using namespace render;

//TODO: Attach/Detach with refcount - for multiple attachment

/*
===================================================================================================
    CacheHolder
===================================================================================================
*/

/*
    ����������� / ����������
*/

CacheHolder::CacheHolder ()
  : need_update (true)
{
}

CacheHolder::~CacheHolder ()
{
  DetachAll ();
}

/*
    ���������� ������ ��������� ���������� ����
*/

void CacheHolder::Attach (CacheHolder& child)
{
  if (IsParentOf (child))
    throw xtl::format_operation_exception ("render::CacheHolder::Attach", "child", "Child cache holder has been already attached (to this holder or it's children)");
    
  if (child.IsParentOf (*this))
    throw xtl::format_operation_exception ("render::CacheHolder::Attach", "child", "Child cache holder is a parent of this cache holder");  

  child_holders.push_back (&child);
  
  try
  {
    child.parent_holders.push_back (this);

    Invalidate ();
  }
  catch (...)
  {
    child_holders.pop_back ();
    throw;
  }
}

void CacheHolder::Detach (CacheHolder& child)
{
  stl::remove (child_holders.begin (), child_holders.end (), &child);
  stl::remove (child.parent_holders.begin (), child.parent_holders.end (), this);
}

void CacheHolder::DetachAll ()
{
  while (!child_holders.empty ())
    Detach (*child_holders.back ());

  while (!parent_holders.empty ())
    parent_holders.back ()->Detach (*this);
}

/*
    �������� �� ������ ������ ������������ �� ��������� � �����������
*/

bool CacheHolder::IsParentOf (CacheHolder& child)
{
  for (HolderList::iterator iter=child_holders.begin (), end=child_holders.end (); iter!=end; ++iter)
  {
    if (*iter == &child)
      return true;
      
    if ((*iter)->IsParentOf (child))
      return true;
  }
  
  return false;
}

/*
    ������� � ������������� ���������� ���� ���������
*/

void CacheHolder::Invalidate ()
{
  if (need_update)
    return;
    
  need_update = true;
  
  for (HolderList::iterator iter=parent_holders.begin (), end=parent_holders.end (); iter!=end; ++iter)
    (*iter)->Invalidate ();
}

/*
    ���������� ����. ���������� �������� ����� ������ �������������� �������
*/

void CacheHolder::UpdateCache ()
{
  if (!need_update)
    return;
    
  UpdateCacheCore ();
  
  need_update = false;
}

/*
===================================================================================================
    CacheSource
===================================================================================================
*/

/*
    ���������� ������ ��������� ���������� ����
*/

void CacheSource::Attach (CacheHolder& holder)
{
  CacheHolder::Attach (holder);
}

void CacheSource::Detach (CacheHolder& holder)
{
  CacheHolder::Detach (holder);
}

/*
    ������� � ������������� ���������� ����
*/

void CacheSource::Invalidate ()
{
  CacheHolder::Invalidate ();
  
  UpdateCache ();
}

/*
    ���������� �������� ������� ���������� ����.
*/

void CacheSource::UpdateCacheCore ()
{
}
