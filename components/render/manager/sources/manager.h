///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderManagerImpl: public Object, public xtl::trackable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderManagerImpl  ();
    ~RenderManagerImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Строка описания устройства рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Description ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание окна рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowPtr CreateWindow (INativeWindow* window, common::PropertyMap& properties);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор окон рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t    WindowsCount ();
    WindowPtr Window       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание целей рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTargetPtr CreateRenderBuffer       (unsigned int width, unsigned int height, PixelFormat format);
    RenderTargetPtr CreateDepthStencilBuffer (unsigned int width, unsigned int height);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Менеджеры ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::DeviceManager&    DeviceManager    ();
    render::manager::CacheManager&     CacheManager     ();
    render::manager::TextureManager&   TextureManager   ();
    render::manager::PrimitiveManager& PrimitiveManager ();
    render::manager::ProgramManager&   ProgramManager   ();
    render::manager::MaterialManager&  MaterialManager  ();
    render::manager::EffectManager&    EffectManager    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание объектов
///////////////////////////////////////////////////////////////////////////////////////////////////
    EntityPtr CreateEntity ();
    FramePtr  CreateFrame  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка ресурсов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource   (const char* resource_name);
    void LoadResource   (const media::rfx::MaterialLibrary&);
    void LoadResource   (const media::geometry::MeshLibrary&);
    void UnloadResource (const char* resource_name);
    void UnloadResource (const media::rfx::MaterialLibrary&);
    void UnloadResource (const media::geometry::MeshLibrary&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Управление кэшированием
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация на события
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef RenderManager::WindowEventHandler WindowEventHandler;
    typedef RenderManager::EventHandler EventHandler;

    xtl::connection RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const;
    xtl::connection RegisterEventHandler       (RenderManagerEvent event, const EventHandler& handler) const;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    render::manager::Settings& Settings ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
