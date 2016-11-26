class InputLayoutManager;
class ProgramParametersManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class DeviceManager: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DeviceManager  (const LowLevelDevicePtr& device, const LowLevelDriverPtr& driver, const SettingsPtr& settings, const CacheManagerPtr& cache_manager);
    ~DeviceManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение устройства / драйвера
///////////////////////////////////////////////////////////////////////////////////////////////////
    low_level::IDevice&        Device           ();
    low_level::IDeviceContext& ImmediateContext ();
    low_level::IDriver&        Driver           ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Возможности устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    const low_level::DeviceCaps& DeviceCaps ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер кэширования
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::CacheManager& CacheManager ();
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер лэйаутов геометрии
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::InputLayoutManager& InputLayoutManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджер параметров программ шэйдинга
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::ProgramParametersManager& ProgramParametersManager ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::Settings& Settings ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
