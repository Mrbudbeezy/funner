///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������� �� ���� Windows Forms
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowSystem: public ICustomWindowSystem, public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowSystem  (IApplicationServer*);
    ~WindowSystem ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������� �� ������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Execute (const char* name, const void* buffer, size_t buffer_size);
    void Execute (const char* command);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetLogHandler (const LogFunction& log);
    const LogFunction& GetLogHandler () { return log_handler; }
    
    void LogMessage (const char* message);    
    void LogPrintf  (const char* format, ...);
    void LogVPrintf (const char* format, va_list list);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddRef  ();
    void Release ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IApplicationServer& ApplicationServer () { return *application_server; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef ControlRegistry<MenuStripItem>   MenuStripItemRegistry;
    typedef ControlRegistry<MenuStrip>       MenuStripRegistry;
    typedef ControlRegistry<ToolStripButton> ToolStripButtonRegistry;
    typedef ControlRegistry<ToolStrip>       ToolStripRegistry;    

    MenuStripItemRegistry&   MenuStripItems   () { return menu_strip_items; }
    MenuStripRegistry&       MenuStrips       () { return menu_strips; }
    ToolStripButtonRegistry& ToolStripButtons () { return tool_strip_buttons; }
    ToolStripRegistry&       ToolStrips       () { return tool_strips; }

  private:
    void RegisterInvokers  ();
    void LoadConfiguration (const char* file_name_mask);

  private:
    typedef xtl::shared_ptr<script::Environment> ShellEnvironmentPtr;

  private:
    ApplicationServerPtr    application_server; //������ ����������
    Form::Pointer           main_form;          //������� ����� ����������
    ShellEnvironmentPtr     shell_environment;  //��������� ���������� �����
    script::Shell           shell;              //���������� �������������
    MenuStripItemRegistry   menu_strip_items;   //������ ��������� ����
    MenuStripRegistry       menu_strips;        //������ ������� ����
    ToolStripButtonRegistry tool_strip_buttons; //������ ������
    ToolStripRegistry       tool_strips;        //������ ������� ������
    LogFunction             log_handler;        //������� ����������������
};
