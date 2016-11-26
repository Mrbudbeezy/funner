///////////////////////////////////////////////////////////////////////////////////////////////////
///Настройки менеджера рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class Settings: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Settings  ();
    ~Settings ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Уровень протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                      SetLogLevel (render::manager::LogLevel level);
    render::manager::LogLevel LogLevel    ();
    bool                      HasDebugLog () { return LogLevel () >= LogLevel_Debug; }

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
