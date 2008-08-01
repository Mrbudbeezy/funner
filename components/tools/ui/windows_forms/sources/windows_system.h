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
///��������� ������� ���������� � ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    IApplicationServer&  ApplicationServer () { return *application_server; }
    script::Environment& ShellEnvironment  () { return *shell_environment; }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    windows_forms::MainForm& MainForm () { return *main_form; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    ChildForm::Pointer CreateChildForm (const char* id, const char* init_string);
    ChildForm::Pointer CreateChildForm (const char* init_string);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef ControlRegistry<MenuStripItem>   MenuStripItemRegistry;
    typedef ControlRegistry<MenuStrip>       MenuStripRegistry;
    typedef ControlRegistry<ToolStripButton> ToolStripButtonRegistry;
    typedef ControlRegistry<ToolStrip>       ToolStripRegistry;    
    typedef ControlRegistry<ChildForm>       ChildFormRegistry;

    MenuStripItemRegistry&   MenuStripItems   () { return menu_strip_items; }
    MenuStripRegistry&       MenuStrips       () { return menu_strips; }
    ToolStripButtonRegistry& ToolStripButtons () { return tool_strip_buttons; }
    ToolStripRegistry&       ToolStrips       () { return tool_strips; }
    ChildFormRegistry&       ChildForms       () { return child_forms; }

  private:
    void RegisterInvokers  ();
    void LoadConfiguration (const char* file_name_mask);

  private:
    typedef xtl::shared_ptr<script::Environment> ShellEnvironmentPtr;

  private:
    ApplicationServerPtr             application_server;  //������ ����������
    windows_forms::MainForm::Pointer main_form;           //������� ����� ����������
    ShellEnvironmentPtr              shell_environment;   //��������� ���������� �����
    script::Shell                    shell;               //���������� �������������
    MenuStripItemRegistry            menu_strip_items;    //������ ��������� ����
    MenuStripRegistry                menu_strips;         //������ ������� ����
    ToolStripButtonRegistry          tool_strip_buttons;  //������ ������
    ToolStripRegistry                tool_strips;         //������ ������� ������
    ChildFormRegistry                child_forms;         //������ �������� ����
    LogFunction                      log_handler;         //������� ����������������
    size_t                           next_child_form_uid; //����� ��������� �������� �����
};
