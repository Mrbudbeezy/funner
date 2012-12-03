#include "shared.h"

using namespace common;
using namespace network;

namespace components
{

namespace url_file_system
{

/*
    ���������
*/

const size_t DOWNLOAD_BUFFER_SIZE = 32784;
const char*  LOG_NAME             = "network.url_file_system";

/*
    URL ����
*/

class UrlFile: public Lockable
{
  public:  
///�����������
    UrlFile (const char* in_url, bool in_is_post)
      : url (in_url)
      , is_post (in_is_post)
      , is_finish_sending (false)
      , end_of_request (false)
      , log (LOG_NAME)
    {
      if (is_post)
        request_file = TempFile ("/system/inetcache/funner_url_file%06u");
    }

///����������
    virtual ~UrlFile () {}
    
///�������� ��������
    virtual size_t     BufferSize () { return request_file.BufferSize (); }
    virtual filepos_t  Tell       () { return 0; }    
    virtual size_t     Read   (void* buf, size_t size) = 0;
    virtual void       Rewind () = 0;
    virtual filepos_t  Seek   (filepos_t pos) = 0;
    virtual filesize_t Size   () = 0;
    virtual void       Resize (filesize_t new_size) = 0;
    virtual bool       Eof    () = 0;
    virtual void       Flush  () = 0;

///������ � ���� �������
    size_t Write (const void* buf, size_t size)
    {
      common::Lock lock (*this);
        
      if (end_of_request)
        throw xtl::format_operation_exception ("", "Can't send data after finish of send request");
                  
      return request_file.Write (buf, size);      
    }    
    
  protected:
///��������� �� ������� ������ �������
    bool IsEndOfRequest () { return end_of_request; }    
    
///URL
    const char* Url () { return url.c_str (); }
    
///URL ����������
    UrlConnection& Connection () { return connection; }
    
///����� ����������������
    common::Log& Log () { return log; }
    
///���������� ������� ������
    void FinishSend ()
    {
      try
      {
        if (end_of_request)
          return;          

        if (is_finish_sending)
          throw xtl::format_operation_exception ("", "URL query '%s' processing failed. Attempt to finish sending after error");

        is_finish_sending = true;
          
        stl::string params = is_post ? common::format ("method=post send_size=%u", request_file.Size ()).c_str () : "";
          
        log.Printf (params.empty () ? "Starting URL query '%s'" : "Starting URL query '%s' (params='%s')", url.c_str (), params.c_str ());
          
        connection = UrlConnection (url.c_str (), params.c_str ());
        
        xtl::uninitialized_storage<char> buffer (DOWNLOAD_BUFFER_SIZE);
        
        if (is_post)
        {        
          request_file.Rewind ();

          for (;;)
          {
            size_t size = request_file.Read (buffer.data (), buffer.size ());

            if (!size)
              break;

            size_t send_size = connection.Send (buffer.data (), size);

            if (send_size != size)
              throw xtl::format_operation_exception ("", "Can't send data %u bytes for URL query '%s'", size, url.c_str ());
          }

          connection.CloseSend ();
        }
        
        if (!request_file.IsClosed ())
        {
          stl::string request_file_path = request_file.Path ();

          request_file.Close ();

          common::FileSystem::Remove (request_file_path.c_str ());
        }        

        const char* status = connection.Status ();

        if (strcmp (status, "Ok"))
          throw xtl::format_operation_exception ("", "URL query '%s' failed. Status is: %s", url.c_str (), status);

        end_of_request = true;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlFile::FinishSend");
        throw;
      }
    }
    
  private:
    UrlConnection connection;        //����������
    stl::string   url;               //URL �������
    File          request_file;      //����� ������ ������ �������
    bool          is_post;           //�������� �� ���������� POST ��������
    bool          is_finish_sending; //���� - ���������� ��������
    bool          end_of_request;    //���� - ������ ���������  
    common::Log   log;               //����� ����������������
};

class SeekableUrlFile: public UrlFile
{
  public:  
///�����������
    SeekableUrlFile (const char* in_url, bool in_is_post, size_t in_buffer_size)
      : UrlFile (in_url, in_is_post)
      , buffer_size (in_buffer_size)
    {
    }  

///�������� �����
    ~SeekableUrlFile ()
    {
      try
      {
        common::Lock lock (*this);
        
        FinishSend ();
        
        if (!response_file.IsClosed ())
        {
          stl::string response_file_path = response_file.Path ();

          response_file.Close ();

          common::FileSystem::Remove (response_file_path.c_str ());
        }    
      }
      catch (...)
      {
      }
    }

///������ ������
    size_t BufferSize ()
    {
      if (!IsEndOfRequest ())
        return UrlFile::BufferSize ();
      
      return response_file.BufferSize ();
    }

///������ �� �����
    size_t Read (void* buf, size_t size)
    {
      common::Lock lock (*this);
        
      FinishSend ();
        
      return response_file.Read (buf, size);
    }
    
///����� ��������� ���������
    void Rewind ()
    {
      common::Lock lock (*this);

      FinishSend ();
        
      response_file.Rewind ();      
    }
    
///����������������
    filepos_t Seek (filepos_t pos)
    {
      common::Lock lock (*this);
        
      FinishSend ();
        
      return response_file.Seek (pos);      
    }
    
    filepos_t Tell ()
    {
      common::Lock lock (*this); 
        
      if (IsEndOfRequest ())
        return response_file.Tell ();
        
      return UrlFile::Tell ();
    }

///������ �����
    filesize_t Size ()
    {   
      common::Lock lock (*this);

      FinishSend ();

      return response_file.Size ();
    }
    
    void Resize (filesize_t new_size)
    {
      common::Lock lock (*this);
        
      FinishSend ();
        
      response_file.Resize (new_size);
    }
    
///�������� ����� �����
    bool Eof ()
    {
      common::Lock lock (*this);

      if (!IsEndOfRequest ())
        return false;

      return response_file.Eof ();
    }    
    
///����� ����� �� ����
    void Flush ()
    {
      common::Lock lock (*this);
        
      if (!IsEndOfRequest ())
        return;

      response_file.Flush ();      
    }    
    
  private:
    void FinishSend ()
    {
      if (IsEndOfRequest ())
        return;

      UrlFile::FinishSend ();
      
      TempFile temp_file ("/system/inetcache/funner_url_file%06u", buffer_size);
      
      size_t size;
      
      xtl::uninitialized_storage<char> buffer (DOWNLOAD_BUFFER_SIZE);      
      
      UrlConnection& connection = Connection ();
      
      while ((size = connection.Receive (buffer.data (), buffer.size ())) != 0)
      {
        size_t write_size = temp_file.Write (buffer.data (), size);
        
        if (write_size != size)
          throw xtl::format_operation_exception ("", "Can't write to file '%s' %u bytes", temp_file.Path (), size);
      }

      temp_file.Rewind ();
      
      Log ().Printf ("URL query '%s' processed (response_size=%u)", Url (), temp_file.Size ());
      
      response_file = temp_file;
    }

  private:
    File   response_file;  //���� ������ ������ ������
    size_t buffer_size;    //������ ���� ������ ������    
};

class StreamUrlFile: public UrlFile
{
  public:
///�����������
    StreamUrlFile (const char* in_url, bool in_is_post)
      : UrlFile (in_url, in_is_post)
      , file_pos ()
      , is_eof ()
    {
    }  

///�������� �����
    ~StreamUrlFile ()
    {
      try
      {
        common::Lock lock (*this);
        
        FinishSend ();
      }
      catch (...)
      {
      }
    }

///������ ������
    size_t BufferSize ()
    {
      if (!IsEndOfRequest ())
        return UrlFile::BufferSize ();
      
      return 0;
    }

///������ �� �����
    size_t Read (void* buf, size_t size)
    {
      common::Lock lock (*this);
        
      FinishSend ();
      
      size_t read_size = Connection ().Receive (buf, size);
      
      if (read_size) file_pos += read_size;
      else           is_eof    = true;
        
      return read_size;
    }
    
///����� ��������� ���������
    void Rewind ()
    {
      if (file_pos)      
        throw xtl::format_not_supported_exception ("network::StreamUrlFile::Rewind", "Position operations are not supported");
    }
    
///����������������
    filepos_t Seek (filepos_t pos)
    {
      if (pos == file_pos)
        return file_pos;
      
      throw xtl::format_not_supported_exception ("network::StreamUrlFile::Seek", "Position operations are not supported");
    }
    
    filepos_t Tell ()
    {
      return file_pos;
    }

///������ �����
    filesize_t Size ()
    {   
      common::Lock lock (*this);

      FinishSend ();

      return Connection ().ContentLength ();
    }
    
    void Resize (filesize_t new_size)
    {
      throw xtl::format_not_supported_exception ("network::StreamUrlFile::Seek", "Can't resize URL stream");
    }
    
///�������� ����� �����
    bool Eof ()
    {
      common::Lock lock (*this);

      if (!IsEndOfRequest ())
        return false;

      return is_eof;
    }    
    
///����� ����� �� ����
    void Flush ()
    {
    }
    
  private:
    void FinishSend ()
    {
      if (IsEndOfRequest ())
        return;
        
      UrlFile::FinishSend ();
    }
    
  private:
    filepos_t  file_pos;  //������� � ������
    bool       is_eof;    //��������� �� ����� �����
};

/*
    URL �������� �������
*/

class UrlCustomFileSystem: public ICustomFileSystem, public xtl::reference_counter
{
  public:
///�����������
    UrlCustomFileSystem (const char* in_prefix)
      : prefix (in_prefix)
      , log (LOG_NAME)
    {
    }
  
///������ � ������
    file_t FileOpen (const char* name, filemode_t mode_flags, size_t buffer_size)
    {
      try
      {
        if (!name)
          throw xtl::make_null_argument_exception ("", "name");          
          
        stl::string url = (prefix + name).c_str ();
          
        log.Printf ("Open url file '%s' (file_mode=%s)", url.c_str (), common::strfilemode (mode_flags).c_str ());
        
        stl::auto_ptr<UrlFile> file;
        
        if (mode_flags & (FileMode_Seek | FileMode_Rewind | FileMode_Resize))
        {
          file.reset (new SeekableUrlFile (url.c_str (), (mode_flags & FileMode_Write) != 0, buffer_size));
        }
        else
        {
          file.reset (new StreamUrlFile (url.c_str (), (mode_flags & FileMode_Write) != 0));
        }

        return reinterpret_cast<file_t> (file.release ());
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileOpen");
        throw;
      }
    }
    
    void FileClose (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);

        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        delete file;
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileClose");
        throw;
      }      
    }
    
    size_t FileBufferSize (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        
          
        return file->BufferSize ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileBufferSize");
        throw;
      }     
    }
    
    size_t FileRead (file_t handle, void* buf, size_t size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        return file->Read (buf, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileRead");
        throw;
      }      
    }
    
    size_t FileWrite (file_t handle, const void* buf, size_t size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");
          
        return file->Write (buf, size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileWrite");
        throw;
      }
    }
    
    void FileRewind (file_t handle)
    {
      try      
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        return file->Rewind ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileRewind");
        throw;
      }      
    }
    
    filepos_t FileSeek (file_t handle, filepos_t pos)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        return file->Seek (pos);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileSeek");
        throw;
      }      
    }
    
    filepos_t FileTell (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        return file->Tell ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileTell");
        throw;
      }      
    }
    
    filesize_t FileSize (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");

        return file->Size ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileSize");
        throw;
      }      
    }
    
    void FileResize (file_t handle, filesize_t new_size)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        return file->Resize (new_size);
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileResize");
        throw;
      }      
    }
    
    bool FileEof (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        return file->Eof ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileEof");
        throw;
      }      
    }
    
    void FileFlush (file_t handle)
    {
      try
      {
        UrlFile* file = reinterpret_cast<UrlFile*> (handle);
        
        if (!file)
          throw xtl::make_null_argument_exception ("", "file");        

        return file->Flush ();
      }
      catch (xtl::exception& e)
      {
        e.touch ("network::UrlCustomFileSystem::FileFlush");
        throw;
      }      
    }

///���������� ������������� ������
    void Remove (const char* file_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Remove operation not supported on url links");
    }
    
    void Rename (const char* file_name, const char* new_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Rename operation not supported on url links");      
    }

    void Mkdir (const char* dir_name)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::Remove", "Mkdir operation not supported on url links");      
    }

///��������� ���������� � �����
    bool IsFileExist (const char* file_name)
    {
      return false;
    }
    
    bool GetFileInfo (const char* file_name, FileInfo& info)
    {
      return false;
    }

//�������� ��������
    void SetFileAttribute (const char* file_name, const char* attribute, const void* data, size_t size)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::SetFileAttribute");
    }

    void GetFileAttribute (const char* file_name, const char* attribute, void* data, size_t size)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::GetFileAttribute");
    }

    bool HasFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::HasFileAttribute");
    }

    void RemoveFileAttribute (const char* file_name, const char* attribute)
    {
      throw xtl::format_not_supported_exception ("network::UrlCustomFileSystem::RemoveFileAttribute");
    }

///����� �����
    void Search (const char* wc_mask, const FileSearchHandler& handler)
    {
      //������ �� URL ������ ���
    }

///������� ������
    void AddRef ()
    {
      addref (this);
    }
    
    void Release ()
    {
      release (this);
    }
    
  private:
    stl::string prefix;
    Log         log;
};

/*
    ��������� ������� CURL
*/

class UrlFileSystemComponent
{
  public:
    UrlFileSystemComponent ()    
    {
      common::FileSystem::MountLink ("http://", "/inet/http");
      common::FileSystem::MountLink ("https://", "/inet/https");
      common::FileSystem::MountLink ("ftp://", "/inet/ftp");
      common::FileSystem::Mount     ("/inet/http", xtl::com_ptr<ICustomFileSystem> (new UrlCustomFileSystem ("http://"), false).get ());
      common::FileSystem::Mount     ("/inet/https", xtl::com_ptr<ICustomFileSystem> (new UrlCustomFileSystem ("https://"), false).get ());
      common::FileSystem::Mount     ("/inet/ftp", xtl::com_ptr<ICustomFileSystem> (new UrlCustomFileSystem ("ftp://"), false).get ());
    }
};

extern "C"
{

common::ComponentRegistrator<UrlFileSystemComponent> UrlFileSystem ("common.file_systems.url_file_system");

}

}

}
