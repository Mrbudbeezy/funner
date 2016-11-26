///////////////////////////////////////////////////////////////////////////////////////////////////
///Опции компиляции шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ShaderOptions
{
  stl::string options;      //строка опций
  size_t      options_hash; //хэш строки опций
  
  ShaderOptions () : options_hash (0xffffffff) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Список расположения опций шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderOptionsLayout: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderOptionsLayout  (const CacheManagerPtr& = CacheManagerPtr ());
    ~ShaderOptionsLayout ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество закэшированных генераторов опций
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CachedShaderBuildersCount ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Опции
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      Size   ();
    const char* Item   (size_t index);
    void        Add    (const char* name);
    void        Remove (const char* name);
    void        Clear  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Хэш расположения
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Hash ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение опций шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    void GetShaderOptions (const common::PropertyMap& defines, ShaderOptions& out_options);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение объекта оповещения об удалении
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable ();
  
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Кэш опций
///////////////////////////////////////////////////////////////////////////////////////////////////
class ShaderOptionsCache: public Object, public CacheSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    ShaderOptionsCache  (const CacheManagerPtr& = CacheManagerPtr ());
    ~ShaderOptionsCache ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Количество закэшированных расположений опций шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t CachedLayoutsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Полный список опций
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение опций шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
    const ShaderOptions& GetShaderOptions (ShaderOptionsLayout& layout);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
