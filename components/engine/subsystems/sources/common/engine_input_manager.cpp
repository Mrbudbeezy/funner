#include <stl/hash_map>
#include <stl/string>

#include <xtl/connection.h>

#include <input/low_level/driver.h>

#include <input/translation_map.h>
#include <input/translation_map_registry.h>

#include "shared.h"

using namespace engine;
using namespace common;
using namespace input::low_level;

namespace components
{

namespace input_manager_subystem
{

const char* SUBSYSTEM_NAME = "InputManager";
const char* LOG_NAME       = "engine.subsystems.InputManagerSubsystem";
const char* COMPONENT_NAME = LOG_NAME;

const size_t DEFAULT_DEVICE_PROPERTIES_COUNT = 4;

/*
   ���������� ��������� �����
*/

typedef xtl::function<void (const char*)> InputHandler;

class InputManagerSubsystem: public ISubsystem, public IAttachmentRegistryListener<const InputHandler>, public xtl::reference_counter
{
  public:
/// �����������/����������
    InputManagerSubsystem (ParseNode& node)
      : translation_map_registry (get<const char*> (node, "TranslationMapRegistry")),
        log (LOG_NAME)
    {
      for (Parser::NamesakeIterator device_iter = node.First ("DevicesProperties.Device"); device_iter; ++device_iter)
      {
        const char* device_name = get<const char*> (*device_iter, "Name");

        DevicePropertiesArray& device_properties = devices_properties [device_name];

        device_properties.reserve (DEFAULT_DEVICE_PROPERTIES_COUNT);

        for (Parser::NamesakeIterator property_iter = device_iter->First ("Property"); property_iter; ++property_iter)
        {
          const char* property_name  = get<const char*> (*property_iter, "Name");
          float       property_value = get<float> (*property_iter, "Value");

          device_properties.push_back (DeviceProperty (property_name, property_value));
        }
      }

      AttachmentRegistry::Attach (this, AttachmentRegistryAttachMode_ForceNotify);
    }

    ~InputManagerSubsystem ()
    {
      AttachmentRegistry::Detach (this);
    }

///���������� ������� ���������� ����� �������� ������� �����
    void OnRegisterAttachment (const char* attachment_name, const InputHandler& handler)
    {
      if (!attachment_name)
        throw xtl::make_null_argument_exception ("InputManagerSubsystem::OnSetInputHandler", "attachment_name");

      AttachmentPtr new_attachment (new Attachment, false);

      for (size_t i = 0; i < DriverManager::DriversCount (); i++)
      {
        IDriver * volatile current_driver = DriverManager::Driver (i);

        for (size_t j = 0; j < current_driver->GetDevicesCount (); j++)
        {
          try
          {
            const char* device_full_name     = current_driver->GetDeviceFullName (j);
            stl::string profile              = common::format("%s.%s", attachment_name, device_full_name);
            const char* translation_map_name = translation_map_registry.FindNearest (profile.c_str ());

            if (!translation_map_name)
              continue;

            Attachment::DeviceEntryPtr device_entry (new Attachment::DeviceEntry (this), false);

            TranslationMapsMap::iterator translation_map_iter = translation_maps.find (translation_map_name);

            if (translation_map_iter == translation_maps.end ())
              device_entry->translation_map = TranslationMapHolderPtr (new TranslationMapHolder (translation_map_name, translation_maps), false);
            else
              device_entry->translation_map = translation_map_iter->second;

            DevicesMap::iterator device_iter = devices.find (device_full_name);

            if (device_iter == devices.end ())
            {
              DevicePtr current_device (current_driver->CreateDevice (current_driver->GetDeviceName (j)), false);

              device_entry->device = DeviceHolderPtr (new DeviceHolder (current_device.get (), devices), false);

              DevicesProperties::iterator devices_properties_iter = devices_properties.find (device_full_name);

              if (devices_properties_iter != devices_properties.end ())
              {
                for (DevicePropertiesArray::iterator property_iter = devices_properties_iter->second.begin (),
                                                     properties_end = devices_properties_iter->second.end (); property_iter != properties_end; ++property_iter)
                  current_device->SetProperty (property_iter->name.c_str (), property_iter->value);
              }
            }
            else
              device_entry->device = device_iter->second;

            device_entry->handler = handler;

            device_entry->device_connection = device_entry->device->GetDevice ()->RegisterEventHandler (xtl::bind (&Attachment::DeviceEntry::Process, device_entry.get (), _1));

            new_attachment->devices.push_back (device_entry);
          }
          catch (xtl::exception& exception)
          {
            log.Printf ("Can't attach to device '%s' with attachment name '%s': exception '%s'", current_driver->GetDeviceName (j), attachment_name,
              exception.what ());
          }
          catch (...)
          {
            log.Printf ("Can't attach to device '%s' with attachment name '%s': unknown exception", current_driver->GetDeviceName (j), attachment_name);
          }
        }
      }

      attachments[attachment_name] = new_attachment;
    }

///���������� ������� �������� ����� �������� ������� �����
    void OnUnregisterAttachment (const char* attachment_name, const InputHandler&)
    {
      attachments.erase (attachment_name);
    }

/// ������� ������
    void AddRef ()
    {
      addref (this);
    }

    void Release ()
    {
      release (this);
    }

  private:
    InputManagerSubsystem (const InputManagerSubsystem&);             //no impl
    InputManagerSubsystem& operator = (const InputManagerSubsystem&); //no impl

  private:
    class TranslationMapHolder;
    class DeviceHolder;

    typedef xtl::com_ptr<IDevice>                                            DevicePtr;
    typedef stl::hash_map<stl::hash_key<const char*>, DeviceHolder*>         DevicesMap;
    typedef stl::hash_map<stl::hash_key<const char*>, TranslationMapHolder*> TranslationMapsMap;
    typedef xtl::intrusive_ptr<DeviceHolder>                                 DeviceHolderPtr;
    typedef xtl::intrusive_ptr<TranslationMapHolder>                         TranslationMapHolderPtr;

    class TranslationMapHolder : public xtl::reference_counter
    {
      public:
        TranslationMapHolder (const char* translation_map_name, TranslationMapsMap& translation_maps)
          : translation_map (translation_map_name), container (&translation_maps)
        {
          this_iter = translation_maps.insert_pair (translation_map_name, this).first;
        }

        ~TranslationMapHolder ()
        {
          container->erase (this_iter);
        }

        input::TranslationMap& GetTranslationMap ()
        {
          return translation_map;
        }

      private:
        input::TranslationMap        translation_map;
        TranslationMapsMap::iterator this_iter;
        TranslationMapsMap*          container;
    };

    class DeviceHolder : public xtl::reference_counter
    {
      public:
        DeviceHolder (IDevice* in_device, DevicesMap& device_map)
          : device (in_device), container (&device_map)
        {
          this_iter = device_map.insert_pair (device->GetFullName (), this).first;
        }

        ~DeviceHolder ()
        {
          container->erase (this_iter);
        }

        IDevice* GetDevice ()
        {
          return device.get ();
        }

      private:
        DevicePtr            device;
        DevicesMap::iterator this_iter;
        DevicesMap*          container;
    };

    struct Attachment : public xtl::reference_counter
    {
      public:
        struct DeviceEntry : public xtl::reference_counter
        {
          DeviceHolderPtr         device;
          TranslationMapHolderPtr translation_map;
          InputHandler            handler;
          xtl::auto_connection    device_connection;
          ISubsystem              *subsystem;

          DeviceEntry (ISubsystem *in_subsystem)
            : subsystem (in_subsystem)
            {}

          void Process (const char* event)
          {
            xtl::com_ptr<ISubsystem> subsystem_holder (subsystem);
            DeviceEntryPtr           this_holder (this);

            translation_map->GetTranslationMap ().ProcessEvent (event, handler);
          }
        };

        typedef xtl::intrusive_ptr<DeviceEntry> DeviceEntryPtr;

      private:
        typedef stl::vector<DeviceEntryPtr>     DeviceArray;

      public:
        DeviceArray  devices;
    };

    typedef xtl::intrusive_ptr<Attachment>                           AttachmentPtr;
    typedef stl::hash_map<stl::hash_key<const char*>, AttachmentPtr> AttachmentMap;

    struct DeviceProperty
    {
      stl::string name;
      float       value;

      DeviceProperty (const char* in_name, float in_value) : name (in_name), value (in_value) {}
    };

    typedef stl::vector<DeviceProperty>                                      DevicePropertiesArray;
    typedef stl::hash_map<stl::hash_key<const char*>, DevicePropertiesArray> DevicesProperties;

  private:
    input::TranslationMapRegistry translation_map_registry;
    TranslationMapsMap            translation_maps;
    DevicesMap                    devices;
    AttachmentMap                 attachments;
    DevicesProperties             devices_properties;
    common::Log                   log;
};

/*
   ��������� ��������� �����
*/

class InputManagerComponent
{
  public:
    InputManagerComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<ISubsystem> subsystem (new InputManagerSubsystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::InputManagerComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<InputManagerComponent> InputManagerSubsystem (COMPONENT_NAME);

}

}

}
