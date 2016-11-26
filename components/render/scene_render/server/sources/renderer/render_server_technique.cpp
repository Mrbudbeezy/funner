#include "shared.h"

using namespace render;
using namespace render::scene::server;

/*
    Описание реализации техники рендеринга
*/

struct Technique::Impl
{
  stl::string                                           name;                            //имя техники
  common::PropertyMap                                   properties;                      //свойства
  common::PropertyMap                                   default_properties;              //свойства по умолчанию
  common::PropertyMap                                   binded_properties;               //значения свойств после связывания
  common::PropertyBindingMap                            property_bindings;               //связывание методом техники со свойствами
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> binded_properties_synchronizer;  //синхронизатор свойств после связывания
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> default_properties_synchronizer; //синхронизатор свойств по умолчанию
  stl::auto_ptr<common::PropertyBindingMapSynchronizer> properties_synchronizer;         //синхронизатор свойств
  bool                                                  need_update_properties;          //флаг необходимости обновления свойств
  bool                                                  need_bind_properties;            //флаг необходимости биндинга свойств
  xtl::auto_connection                                  properties_update_connection;    //соединение с сигналом обновления свойств
  common::PropertyMap::EventHandler                     properties_update_handler;       //обработчик сигнала обновления свойств
  
///Конструктор
  Impl ()
    : need_update_properties (true) 
    , need_bind_properties (true)
  {
  }
};

/*
    Конструктор / деструктор
*/

Technique::Technique ()
  : impl (new Impl)
{
  impl->properties_update_handler = xtl::bind (&Technique::UpdateProperties, this);
}

Technique::~Technique ()
{
}

/*
    Имя техники
*/

void Technique::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Technique::SetName", "name");
    
  impl->name = name;
}

const char* Technique::Name () const
{
  return impl->name.c_str ();
}

/*
    Камера
*/

/*
    Свойства по умолчанию
*/

void Technique::SetDefaultProperties (const common::PropertyMap& properties)
{
  impl->default_properties     = properties.Clone ();
  impl->need_update_properties = true;

  impl->default_properties_synchronizer.reset ();
}

void Technique::SetDefaultProperties (const common::ParseNode& node)
{
  try
  {
    //TODO: cache for node -> property map

    SetDefaultProperties (to_properties (node));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::SetDefaultProperties(const common::ParseNode&)");
    throw;
  }
}

const common::PropertyMap& Technique::DefaultProperties ()
{
  return impl->default_properties;
}

/*
    Свойства рендеринга
*/

void Technique::SetProperties (const common::PropertyMap& properties)
{
  try
  {
    impl->properties                   = properties;
    impl->need_update_properties       = true;
    impl->properties_update_connection = properties.RegisterEventHandler (common::PropertyMapEvent_OnUpdate, impl->properties_update_handler);

    impl->properties_synchronizer.reset ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::UpdateProperties");
    throw;
  }
}

void Technique::UpdateProperties ()
{
  impl->need_update_properties = true;
}

/*
    Связывание свойств техники с методами техники
*/

void Technique::BindProperties (common::PropertyBindingMap&)
{
}

/*
    Отрисовка
*/

void Technique::UpdateFrame (RenderingContext& context, TechniquePrivateData& private_data)
{
  try
  {
      //обновление свойств

    if (impl->need_update_properties)
    {
        //биндинг свойств

      if (impl->need_bind_properties)
      {        
        BindProperties (impl->property_bindings);

        impl->binded_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->binded_properties));
        
        impl->binded_properties_synchronizer->CopyToPropertyMap ();

        impl->need_bind_properties = false;
      }

        //обновление свойств по умолчанию

      if (!impl->default_properties_synchronizer)
      {
        impl->default_properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->default_properties));

        impl->default_properties_synchronizer->CopyFromPropertyMap (); //updates only once
      }

        //обновление свойств

      if (!impl->properties_synchronizer)
      {
          //применение свойств по умолчанию

        impl->binded_properties_synchronizer->CopyFromPropertyMap ();
        impl->default_properties_synchronizer->CopyFromPropertyMap ();

          //созданеи синхронизатора

        impl->properties_synchronizer.reset (new common::PropertyBindingMapSynchronizer (impl->property_bindings, impl->properties));
      }

      impl->properties_synchronizer->CopyFromPropertyMap ();

        //оповещение об обновлении

      UpdatePropertiesCore ();

      impl->need_update_properties = false;
    }

      //отрисовка кадра

    UpdateFrameCore (context, private_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Technique::UpdateFrame");
    throw;
  }
}

/*
    TechniquePrivateData
*/

void TechniquePrivateData::RaiseSizeError (const std::type_info& type, size_t size)
{
  throw xtl::format_operation_exception ("render::scene::server::Technique::RaiseSizeError", "Can't convert TechniquePrivateData to %s because its size %u is larger than max buffer size %u",
    type.name (), size, DATA_BUFFER_SIZE);
}
