///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClientWindowManager: public xtl::noncopyable
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ClientWindowManager  (ServerLoopbackConnection&);
    ~ClientWindowManager ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachWindow     (const char* name, syslib::Window& window, const common::PropertyMap& properties);
    void DetachWindow     (const char* name);
    void DetachAllWindows ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
