///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ConfigurationParser
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ConfigurationParser  (const char* file_name, WindowSystem& window_system, PluginManager& plugin_manager);
    ~ConfigurationParser ();

  private:
    void                     ParseConfiguration   (System::Xml::XmlNode^);
    MenuStripItem::Pointer   ParseMenuStripItem   (System::Xml::XmlNode^);
    MenuStrip::Pointer       ParseMenuStrip       (System::Xml::XmlNode^);
    ToolStripButton::Pointer ParseToolStripButton (System::Xml::XmlNode^);
    ToolStrip::Pointer       ParseToolStrip       (System::Xml::XmlNode^);
    stl::string              GenerateUid          ();

  private:
    WindowSystem&            window_system;     //������� �������
    PluginManager&           plugin_manager;    //�������� ��������
    size_t                   next_uid;          //��������� ����� ����������� ��������������
    common::Log              log;               //���
};
