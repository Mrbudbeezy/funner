///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationParser
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConfigurationParser  (const char* file_name, WindowSystem& window_system);
    ~ConfigurationParser ();

  private:
    void                   ParseConfiguration (const common::Parser::Iterator&);
    ToolMenuItem::Pointer  ParseMenuItem      (const common::Parser::Iterator&);
    ToolMenuItem::Pointer  ParseSubMenuItem   (const common::Parser::Iterator&);
    ToolMenuStrip::Pointer ParseMenuStrip     (const common::Parser::Iterator&);
    stl::string            GenerateUid        ();

  private:
    WindowSystem&    window_system; //������� �������
    common::ParseLog parse_log;     //�������� �������
    common::Parser   parser;        //������
    size_t           next_uid;      //��������� ����� ����������� ��������������
};
