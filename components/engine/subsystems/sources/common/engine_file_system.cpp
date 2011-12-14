#include "shared.h"

using namespace engine;
using namespace common;

namespace components
{

namespace file_system
{

/*
    ���������
*/

const char* SUBSYSTEM_NAME = "FileSystem";                   //��� ����������
const char* COMPONENT_NAME = "engine.subsystems.FileSystem"; //��� ����������
const char* LOG_NAME       = COMPONENT_NAME;

/*
   ���������� �������� �������
*/

class FileSystem : public ISubsystem, public xtl::reference_counter
{
  public:
/// �����������/����������
    FileSystem (common::ParseNode& node)
    {
        //������ ���������� ����������

      ParseNode crypto_node = node.First ("Crypto");

      if (crypto_node)
      {
        for (Parser::NamesakeIterator iter=crypto_node.First ("File"); iter; ++iter)
        {
          const char* file_name    = get<const char*> (*iter, "Name");
          const char* read_method  = get<const char*> (*iter, "ReadMethod");
          const char* write_method = get<const char*> (*iter, "WriteMethod", read_method);
          const char* key_file     = get<const char*> (*iter, "KeyFile");

          filecryptokey_t key;

          common::FileSystem::GetFileCryptoKey (key_file, key);

          common::FileCryptoParameters crypto_parameters (read_method, write_method, key, sizeof (key) * CHAR_BIT);

          common::FileSystem::SetCryptoParameters (file_name, crypto_parameters);
        }
      }
      
        //������ ���������� �����������
        
      if (ParseNode cache_node = node.First ("Cache"))
      {
        for (Parser::NamesakeIterator iter=cache_node.First ("File"); iter; ++iter)
        {
          const char* file_name    = get<const char*> (*iter, "Name");
          size_t      buffer_size  = get<size_t> (*iter, "BufferSize");

          common::FileSystem::SetFileBufferSize (file_name, buffer_size);
        }
      }
        
        //������������ �����
        
      ParseNode mount_node = node.First ("Mount");
      
      if (mount_node)
      {        
        for (Parser::NamesakeIterator iter=mount_node.First ("File"); iter; ++iter)
        {
          const char* link_name       = get<const char*> (*iter, "Name");
          const char* path            = get<const char*> (*iter, "Path");
          const char* force_extension = get<const char*> (*iter, "ForceExtension", (const char*)0);
          
          if (!common::FileSystem::IsDir (path) && !common::FileSystem::IsFileExist (path))
            common::FileSystem::Mkdir (path);

          common::FileSystem::Mount (link_name, path, force_extension);
        }
      }

        //���������� ����� ������

      const char* paths_string = get<const char*> (node, "Paths", "");

      StringArray path_list = split (paths_string);

      for (size_t i=0; i<path_list.Size (); i++)
      {
        const char* path = path_list [i];

        paths.Add (path);
      }

      Log log (LOG_NAME);
      common::FileSystem::LogHandler log_handler (xtl::bind (&common::Log::Print, &log, _1));

      for (size_t i=0; i<paths.Size (); i++)
      {
        const char* path = paths [i];

        common::FileSystem::AddSearchPath (path, log_handler);
      }      
    }

    ~FileSystem ()
    {
      for (size_t i=0; i<paths.Size (); i++)
        common::FileSystem::RemoveSearchPath (paths [i]);
    }

/// ������� ������
    void AddRef ()  { addref (this); }
    void Release () { release (this); }

  private:
    FileSystem (const FileSystem&);             //no impl
    FileSystem& operator = (const FileSystem&); //no impl

  private:
    common::StringArray paths;
};

/*
   ���������
*/

class FileSystemComponent
{
  public:
    //�������� ����������
    FileSystemComponent ()
    {
      StartupManager::RegisterStartupHandler (SUBSYSTEM_NAME, &StartupHandler);
    }

  private:
    static void StartupHandler (ParseNode& node, SubsystemManager& manager)
    {
      try
      {
        xtl::com_ptr<FileSystem> subsystem (new FileSystem (node), false);

        manager.Add (SUBSYSTEM_NAME, subsystem.get ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("engine::FileSystemComponent::StartupHandler");
        throw;
      }
    }
};

extern "C"
{

ComponentRegistrator<FileSystemComponent> FileSystem (COMPONENT_NAME);

}

}

}
