///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Form: public xtl::reference_counter, public xtl::dynamic_cast_root
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetText (const char*);
    const char* Text    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Show       ();
    void Hide       ();
    void SetVisible (bool state);
    bool IsVisible  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                      SetMenuStrip (windows_forms::MenuStrip*);
    windows_forms::MenuStrip* MenuStrip    () { return menu_strip; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert (windows_forms::ToolStrip*);
    void Remove (windows_forms::ToolStrip*);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterInvokers (script::Environment&, const char* library_name);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Form  (windows_forms::WindowSystem& window_system);
    ~Form ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    windows_forms::WindowSystem& WindowSystem () { return window_system; }

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ref-��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual System::Windows::Forms::Form^ Handle () = 0;

  private:
    windows_forms::WindowSystem& window_system; //������� �������
    windows_forms::MenuStrip*    menu_strip;    //������� ���� �����
    AutoString                   text;          //����� �����
};
