///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Settings: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Settings  ();
    ~Settings ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void             SetLogLevel (render::LogLevel level);
    render::LogLevel LogLevel    ();
    bool             HasDebugLog () { return LogLevel () >= LogLevel_Debug; }

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};