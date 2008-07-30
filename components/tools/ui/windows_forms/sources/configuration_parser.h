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
    void                     ParseConfiguration   (const common::Parser::Iterator&);
    MenuStripItem::Pointer   ParseMenuStripItem   (const common::Parser::Iterator&);
    MenuStrip::Pointer       ParseMenuStrip       (const common::Parser::Iterator&);
    ToolStripButton::Pointer ParseToolStripButton (const common::Parser::Iterator&);
    ToolStrip::Pointer       ParseToolStrip       (const common::Parser::Iterator&);    
    stl::string              GenerateUid          ();

  private:
    WindowSystem&    window_system; //������� �������
    common::ParseLog parse_log;     //�������� �������
    common::Parser   parser;        //������
    size_t           next_uid;      //��������� ����� ����������� ��������������
};
