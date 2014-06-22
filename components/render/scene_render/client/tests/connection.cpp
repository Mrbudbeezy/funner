#include "shared.h"

namespace
{

///�������� ���������� � ��������
class MyConnection: public scene::interchange::IConnection, public xtl::reference_counter, public xtl::trackable
{
  public:
///�����������
    MyConnection (const char* init_string) 
    {
      common::PropertyMap properties = common::parse_init_string (init_string);
    
      const char* response_connection_name = properties.GetString ("initiator");

      response_connection = xtl::com_ptr<scene::interchange::IConnection> (scene::interchange::ConnectionManager::CreateConnection (response_connection_name, "initiator=MyConnection"), false);
    }

///�������� �� ���������� ����������������
    bool IsInprocessed () { return true; }

///��������� �������� ������ ������
    void ProcessCommands (const scene::interchange::CommandBuffer& commands)
    {
    }

///��������� ������� ���������� �� ��������
    xtl::trackable& GetTrackable () { return *this; }

///������� ������
    void AddRef  () { addref (this); }
    void Release () { release (this); }

///�������� ����������
    static IConnection* CreateConnection (const char* name, const char* init_string)
    {
      return new MyConnection (init_string);
    }

  private:
    xtl::com_ptr<scene::interchange::IConnection> response_connection;
};

}

int main ()
{
  printf ("Results of connection_test:\n");

  try
  {
    static const char* CONNECTION_NAME = "my_test_connection";

    scene::interchange::ConnectionManager::RegisterConnection (CONNECTION_NAME, CONNECTION_NAME, &MyConnection::CreateConnection);

    Client render (CONNECTION_NAME);

    scene::interchange::ConnectionManager::UnregisterConnection (CONNECTION_NAME);

    printf ("description='%s'\n", render.Description ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
