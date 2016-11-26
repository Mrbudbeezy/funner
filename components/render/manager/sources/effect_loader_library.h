///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека загрузчика эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
class EffectLoaderLibrary: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Библиотека
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class Ptr> class Library: public xtl::noncopyable
    {
      public:
        typedef Ptr Item;
      
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
        Library  ();
        ~Library ();
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
        size_t Size ();
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление / удаление элементов
///////////////////////////////////////////////////////////////////////////////////////////////////
        void Add    (const char* id, const Item& item);
        void Remove (const char* id);
        void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск элемента
///////////////////////////////////////////////////////////////////////////////////////////////////
        Item Find (const char* id); //return Item () if item not found
        
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обход библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
        typedef xtl::function<void (const char* id, Item&)> VisitFunction;
        
        void ForEach (const VisitFunction&);

      private:
        struct Impl;
        stl::auto_ptr<Impl> impl;
    };
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    EffectLoaderLibrary  ();
    ~EffectLoaderLibrary ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Доступные библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef Library<LowLevelRasterizerStatePtr>    RasterizerStateLibrary;
    typedef Library<LowLevelBlendStatePtr>         BlendStateLibrary;
    typedef Library<LowLevelDepthStencilStatePtr>  DepthStencilStateLibrary;
    typedef Library<LowLevelSamplerStatePtr>       SamplerStateLibrary;
    typedef Library<LowLevelTextureDescPtr>        TextureDescLibrary;    
    typedef Library<ProgramPtr>                    ProgramLibrary;
    typedef Library<EffectPassPtr>                 EffectPassLibrary;
    typedef Library<EffectPtr>                     EffectLibrary;    
    
    RasterizerStateLibrary&    RasterizerStates        ();
    RasterizerStateLibrary&    RasterizerScissorStates ();    
    BlendStateLibrary&         BlendStates             ();
    DepthStencilStateLibrary&  DepthStencilStates      ();
    SamplerStateLibrary&       SamplerStates           ();
    TextureDescLibrary&        TextureDescs            ();
    ProgramLibrary&            Programs                ();
    media::rfx::ShaderLibrary& Shaders                 ();
    EffectPassLibrary&         EffectPasses            ();
    EffectLibrary&             Effects                 ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузчик эффектов
///////////////////////////////////////////////////////////////////////////////////////////////////
void parse_effect_library (const DeviceManagerPtr&, const TextureManagerPtr&, const common::ParseNode& node, const char* file_name, EffectLoaderLibrary& library);
