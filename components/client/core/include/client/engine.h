#ifndef CLIENT_ENGINE_HEADER
#define CLIENT_ENGINE_HEADER

namespace client
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������               
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEngineSubsystem
{
  public:
    virtual const char* Name () = 0;

    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~IEngineSubsystem () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEngineStartupParams
{
  protected:
    virtual ~IEngineStartupParams () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Engine
{
  public:
    typedef xtl::com_ptr<IEngineSubsystem>  SubsystemPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Engine (Client& client, const char* configuration_branch_name, IEngineStartupParams* engine_startup_params = 0);
    ~Engine ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*     ConfigurationBranch () const;
    client::Client& Client              () const;    

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadResource (const char* resource_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::trackable& GetTrackable () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t            SubsystemsCount () const;
    IEngineSubsystem& Subsystem       (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSubsystem    (const SubsystemPointer subsystem);
    void RemoveSubsystem (const SubsystemPointer subsystem);

  private:
    Engine (const Engine&);
    Engine& operator = (const Engine&);
};

xtl::trackable& get_trackable (const Engine& engine);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class StartupManager
{
  public:
    typedef xtl::function<void (VarRegistry&, IEngineStartupParams*, Engine&)> StartupHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������/�������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    //??????? ����� ��������� ����� ������� � StartupHandler'��
    static void RegisterStartupHandler       (const char* node_name, const StartupHandler& startup_handler, int order = 0); //��� ���� order, ��� ������ �����������
    static void UnregisterStartupHandler     (const char* node_name);
    static void UnregisterAllStartupHandlers ();
};

}

#endif
