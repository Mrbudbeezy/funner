#include "shared.h"

using namespace engine;

namespace
{

/*
    ���� � �������
*/

struct AttachmentKey
{
  size_t                name_hash; //��� �����
  const std::type_info* type;      //��� ����� ��������
  
  AttachmentKey (const char* name, const std::type_info& in_type) :
    name_hash (common::strhash (name)), type (&in_type) {}
    
  bool operator == (const AttachmentKey& key) const { return name_hash == key.name_hash && type == key.type; }
};

size_t hash (const AttachmentKey& key)
{
  return stl::hash (key.name_hash, stl::hash (key.type));
}

}

namespace engine
{

/*
    ���������� ��������� ����� ��������
*/

class AttachmentRegistryImpl: public xtl::reference_counter
{
  public:
///����������
    ~AttachmentRegistryImpl ()
    {
      UnregisterAll ();
    }
  
///����������� ����� ��������
    void Register (const char* name, detail::IBasicAttachment* attachment)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistry::Register";
      
        //�������� ������������ ����������, ������ ��������
      
      if (!attachment)
        throw xtl::make_null_argument_exception (METHOD_NAME, "attachment");
      
      AttachmentPtr         attachment_holder (attachment);
      const std::type_info& type = attachment->GetType ();

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        //����� ��������� � ����� ����� ��������

      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);
      
      if (iter == attachments.end ())
      {
          //���������� ����� ����� ��������

        attachments.insert_pair (key, AttachmentEntryPtr (new AttachmentEntry (name, type, attachment_holder), false));
      }
      else
      {
        iter->second->attachment = attachment_holder;
      }

        //���������� �� ��������� ����� ����� ��������

      RegisterAttachmentNotify (name, type, attachment);      
    }
    
///������ ����������� ����� ��������
    void Unregister (const char* name, const std::type_info& type)
    {
      if (!name)
        return;

        //����� ����� ��������

      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);

      if (iter == attachments.end ())
        return;

        //���������� �� �������� ����� ��������

      AttachmentEntry& entry = *iter->second;

      UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());

        //�������� ����� ��������

      attachments.erase (iter);
    }
    
///������ ����������� ���� ����� �������� ������������ ����
    void UnregisterAll (const std::type_info& type)
    {
      for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end ();)
        if (iter->second->type == &type)
        {
          AttachmentMap::iterator next = iter;

          ++next;

            //���������� �� �������� ����� ��������

          AttachmentEntry& entry = *iter->second;

          UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());

            //�������� ����� ��������

          attachments.erase (iter);

          iter = next;
        }
        else ++iter;
    }
    
///������ ����������� ���� ����� ��������
    void UnregisterAll ()
    {
      for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end ();)
      {
          //���������� �� �������� ����� ��������

        AttachmentEntry& entry = *iter->second;

        UnregisterAttachmentNotify (entry.name.c_str (), *entry.type, entry.attachment.get ());
      }

      attachments.clear ();
    }

///����� ����� ��������
    detail::IBasicAttachment* Find (const char* name, const std::type_info& type, bool raise_exception)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistryImpl::Find";

        //�������� ������������ ����������

      if (!name)
      {
        if (raise_exception)
          throw xtl::make_null_argument_exception (METHOD_NAME, "name");

        return 0;
      }
      
        //����� ����� �������� �� �����
      
      AttachmentKey           key (name, type);
      AttachmentMap::iterator iter = attachments.find (key);

      if (iter == attachments.end ())
      {
        if (raise_exception)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Attachment point not found");

        return 0;
      }

      return iter->second->attachment.get ();
    }

///���������� ��������� �������
    void Attach (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode)
    {
      static const char* METHOD_NAME = "engine::AttachmentRegistryImpl::Attach";
      
        //�������� ������������ ����������

      if (!listener)
        throw xtl::make_null_argument_exception (METHOD_NAME, "listener");

      switch (mode)
      {
        case AttachmentRegistryAttachMode_Quiet:
        case AttachmentRegistryAttachMode_ForceNotify:
          break;
        default:
          throw xtl::make_argument_exception (METHOD_NAME, "mode", mode);
      }

        //���������� ���������

      listeners.push_back (ListenerEntry (type, listener));      
      
        //�������������� ���������� � ����������� ����� ��������
        
      if (mode == AttachmentRegistryAttachMode_ForceNotify)
      {        
        for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end (); ++iter)
        {
          AttachmentEntry& entry = *iter->second;
          
          if (entry.type == &type)
          {
            try
            {
              listener->OnRegisterAttachmentCore (entry.name.c_str (), entry.attachment.get ());
            }
            catch (...)
            {
              //���������� ���� ����������
            }
          }
        }
      }
    }
    
///�������� ��������� �������
    void Detach (const std::type_info& type, IBasicAttachmentRegistryListener* listener, AttachmentRegistryAttachMode mode)
    {
        //�������� ������������ ����������
      
      if (!listener)
        return;
        
      switch (mode)
      {
        case AttachmentRegistryAttachMode_Quiet:
        case AttachmentRegistryAttachMode_ForceNotify:
          break;
        default:
          return;
      }        
        
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end ();)
        if (iter->type == &type && iter->listener == listener)
        {
          ListenerList::iterator next = iter;

          ++next;
          
          IBasicAttachmentRegistryListener* listener = iter->listener;
          
            //�������������� ���������� �� ������ ����������� ����� ��������

          if (mode == AttachmentRegistryAttachMode_ForceNotify)
          {        
            for (AttachmentMap::iterator iter=attachments.begin (); iter!=attachments.end (); ++iter)
            {
              AttachmentEntry& entry = *iter->second;

              if (entry.type == &type)
              {
                try
                {
                  listener->OnRegisterAttachmentCore (entry.name.c_str (), entry.attachment.get ());
                }
                catch (...)
                {
                  //���������� ���� ����������
                }
              }
            }
          }          

            //�������� ���������

          listeners.erase (iter);

          iter = next;
        }
        else ++iter;
    }
    
  private:
///���������� � ����������� ����� ��������
    void RegisterAttachmentNotify (const char* name, const std::type_info& type, detail::IBasicAttachment* attachment)
    {
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end (); ++iter)
        if (iter->type == &type)
        {
          try
          {
            iter->listener->OnRegisterAttachmentCore (name, attachment);
          }
          catch (...)
          {
            //���������� ���� ����������
          }
        }
    }

///���������� �� �������� ����� ��������
    void UnregisterAttachmentNotify (const char* name, const std::type_info& type, detail::IBasicAttachment* attachment)
    {
      for (ListenerList::iterator iter=listeners.begin (); iter!=listeners.end (); ++iter)
        if (iter->type == &type)
        {
          try
          {
            iter->listener->OnUnregisterAttachmentCore (name, attachment);
          }
          catch (...)
          {
            //���������� ���� ����������
          }
        }
    }

  private:
    typedef stl::auto_ptr<detail::IBasicAttachment> AttachmentPtr;

    struct AttachmentEntry: public xtl::reference_counter
    {
      stl::string           name;
      AttachmentPtr         attachment;
      const std::type_info* type;

      AttachmentEntry (const char* in_name, const std::type_info& in_type, AttachmentPtr& in_attachment) :
        name (in_name), attachment (in_attachment), type (&in_type) {}
    };  

    struct ListenerEntry
    {
      IBasicAttachmentRegistryListener* listener;
      const std::type_info*             type;

      ListenerEntry (const std::type_info& in_type, IBasicAttachmentRegistryListener* in_listener) :
        listener (in_listener), type (&in_type) {}
    };

    typedef xtl::intrusive_ptr<AttachmentEntry>              AttachmentEntryPtr;
    typedef stl::hash_map<AttachmentKey, AttachmentEntryPtr> AttachmentMap;
    typedef stl::list<ListenerEntry>                         ListenerList;

  private:
    AttachmentMap attachments; //����� ����� ��������
    ListenerList  listeners;   //���������
};

typedef common::Singleton<AttachmentRegistryImpl> AttachmentRegistrySingleton;

}

/*
    �������� ��� �������� � AttachmentRegistry
*/

void AttachmentRegistry::RegisterCore (const char* name, detail::IBasicAttachment* attachment)
{
  AttachmentRegistrySingleton::Instance ().Register (name, attachment);
}

void AttachmentRegistry::UnregisterCore (const char* name, const std::type_info& type)
{
  AttachmentRegistrySingleton::Instance ().Unregister (name, type);
}

void AttachmentRegistry::UnregisterAllCore (const std::type_info& type)
{
  AttachmentRegistrySingleton::Instance ().UnregisterAll (type);
}

detail::IBasicAttachment* AttachmentRegistry::FindCore (const char* name, const std::type_info& type, bool raise_exception)
{
  return AttachmentRegistrySingleton::Instance ().Find (name, type, raise_exception);
}

void AttachmentRegistry::AttachCore
 (const std::type_info&             type,
  IBasicAttachmentRegistryListener* listener,
  AttachmentRegistryAttachMode      mode)
{
  AttachmentRegistrySingleton::Instance ().Attach (type, listener, mode);
}

void AttachmentRegistry::DetachCore
 (const std::type_info&             type,
  IBasicAttachmentRegistryListener* listener,
  AttachmentRegistryAttachMode      mode)
{
  AttachmentRegistrySingleton::Instance ().Detach (type, listener, mode);
}
