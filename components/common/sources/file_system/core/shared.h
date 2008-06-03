#ifndef COMMONLIB_FILE_SYSTEM_SHARED_HEADER
#define COMMONLIB_FILE_SYSTEM_SHARED_HEADER

#include <common/file.h>
#include <common/singleton.h>
#include <common/strlib.h>
#include <common/hash.h>
#include <common/component.h>

#include <platform/platform.h>

#include <stl/hash_set>
#include <stl/vector>
#include <stl/list>
#include <stl/algorithm>

#include <xtl/function.h>
#include <xtl/bind.h>
#include <xtl/reference_counter.h>
#include <xtl/string.h>

#include <string.h>

namespace common
{

//implementation forwards
struct MountFileSystem;

//���������
const size_t FILE_HASH_BUF_SIZE    = 1024; //������ ������ ������������� ��� �������� ��� ����� �����
const size_t DEFAULT_FILE_BUF_SIZE = 8192; //������ ������ ����� �� ���������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileImpl: public xtl::reference_counter
{
  public:
            FileImpl (filemode_t mode);
    virtual ~FileImpl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    filemode_t Mode () const { return mode; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t Read  (void* buf,size_t size);
    virtual size_t Write (const void* buf,size_t size);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    virtual filepos_t Tell   ();
    virtual filepos_t Seek   (filepos_t new_pos);
    virtual void      Rewind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� / �������� �� ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual filesize_t Size   ();
    virtual void       Resize (filesize_t new_size);
    virtual bool       Eof    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� ������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Flush ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t GetBufferSize () { return 0; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �� ������ ����� ClosedFileImpl
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsClosedFileType () { return false; }

  private:
    filemode_t mode; //����� ������ �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClosedFileImpl: public FileImpl
{
  public:
    ClosedFileImpl ();
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������: �������� �� ������ ����� ClosedFileImpl
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsClosedFileType () { return true; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////    
    static FileImpl* Instance ();
    
  private:
    void Raise (const char* source);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� ��� ���������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class CustomFileImpl: public FileImpl
{
  private:    
    typedef ICustomFileSystem::file_t file_t;

  public:
    CustomFileImpl  (ICustomFileSystemPtr file_system,const char* file_name,filemode_t mode_flags);
    CustomFileImpl  (ICustomFileSystemPtr file_system,file_t handle,filemode_t mode_flags,bool auto_close);
    ~CustomFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();
    size_t      GetBufferSize ();

  private:
    ICustomFileSystemPtr file_system; //�������� �������
    file_t              file_handle; //������� ����� � �������� �������
    bool                auto_close;  //���������� �� ��������� ���� ��� ����������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ����� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemFileImpl: public FileImpl
{
  public: 
    MemFileImpl  (FileImplPtr base_file);
    MemFileImpl  (void* buffer,size_t buffer_size,filemode_t mode);
    ~MemFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    bool        Eof    ();
    size_t      GetBufferSize () { return finish-start; }

  private:
    bool   is_auto_deleted;  //���� ������������ ����� �� ������������� ������� ����� ��� �������� �����
    char*  start;        //��������� �� ������ ������
    char*  finish;       //��������� �� ����� ������
    char*  pos;          //��������� �� ������� ������� ������ ������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class BufferedFileImpl: public FileImpl
{
  public:
    BufferedFileImpl  (FileImplPtr base_file,size_t buffer_size);
    ~BufferedFileImpl ();

    size_t      Read   (void* buf,size_t size);
    size_t      Write  (const void* buf,size_t size);
    filepos_t   Tell   ();
    filepos_t   Seek   (filepos_t new_pos);
    void        Rewind ();
    filesize_t  Size   ();
    void        Resize (filesize_t new_size);
    bool        Eof    ();
    void        Flush  ();
    size_t      GetBufferSize () { return buffer_size; }

  private:
    size_t ReadBuffer  (filepos_t position,void* buf,size_t size);
    size_t WriteBuffer (filepos_t position,const void* buf,size_t size);
    void   ResetBuffer (filepos_t new_cache_start);
    void   FlushBuffer ();

  private:
    FileImplPtr base_file;   //������� ����
    filepos_t   file_pos;     //�������� �������
    filesize_t  file_size;    //������ �����
    char*       buffer;       //����� �����
    size_t      buffer_size;  //������ ������ �����
    filepos_t   cache_start;  //�������� ������� ������ ����������������� �������
    filepos_t   cache_finish; //�������� ������� ����� ����������������� �������
    size_t      dirty_start;  //�������� ������� ������ ����������� �������
    size_t      dirty_finish; //�������� ������� ����� ����������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileListBuilder: public xtl::noncopyable
{
  public:
    typedef stl::vector<FileListItem> FileInfoArray;
    
    FileListBuilder ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetPrefix       (const char* prefix);
    void        SetTruncateSize (size_t size);
    const char* GetPrefix       () const { return prefix.c_str (); }
    size_t      GetTruncateSize () const { return truncate_size; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Insert        (const char* file_name,const FileInfo& file_info);
    void InsertSubname (const char* file_name,size_t file_name_size,const FileInfo& file_info);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList Build (bool sort=false);
    
  private:      
    void InsertInternal (const char* file_name,size_t file_name_size,const FileInfo& file_info);    
        
  private:
    typedef stl::hash_set<size_t> NameHash;
  
    NameHash      items_hash;
    FileInfoArray items;
    stl::string   file_names;
    stl::string   prefix;
    size_t        truncate_size;
    size_t        prefix_hash;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ���-�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PackFileType
{
  FileSystem::PackFileCreater creater;        //������� �������� ���-�����
  size_t                      extension_hash; //��� ����������  
  stl::string                 extension;      //����������
  
  PackFileType (const char* extension,size_t hash,const FileSystem::PackFileCreater&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct SearchPath
{
  size_t      hash; //��� ���� ������
  stl::string path; //���� ������

  SearchPath (const char* path,size_t hash);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���-����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PackFile
{
  size_t              file_name_hash;   //��� ����� ���-�����
  size_t              search_path_hash; //��� ����� ���� ������ ���������������� � ���-������
  ICustomFileSystemPtr file_system;      //��������� �������� ������� ���-�����
  
  PackFile  (size_t file_name_hash,size_t search_path_hash,ICustomFileSystemPtr file_system);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ������� ��� ��������� ���������� � ����� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MountPointFileSystem: public ICustomFileSystem
{
  public:
    MountPointFileSystem (MountFileSystem& owner);
  
    file_t     FileOpen    (const char* name,filemode_t mode_flags,size_t);
    void       FileClose   (file_t);
    size_t     FileRead    (file_t,void* buf,size_t size);
    size_t     FileWrite   (file_t,const void* buf,size_t size);
    void       FileRewind  (file_t);
    filepos_t  FileSeek    (file_t,filepos_t pos);
    filepos_t  FileTell    (file_t);
    filesize_t FileSize    (file_t);
    void       FileResize  (file_t,filesize_t new_size);
    bool       FileEof     (file_t);
    void       FileFlush   (file_t);
    void       Remove      (const char* file_name);
    void       Rename      (const char* file_name,const char* new_name);
    void       Mkdir       (const char* dir_name);
    bool       IsFileExist (const char* file_name);
    bool       GetFileInfo (const char* file_name,FileInfo& info);
    void       Search      (const char* mask,const FileSearchHandler&);
    void       AddRef      () {}
    void       Release     () {}
    
  private:
    MountFileSystem* owner; //��������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct MountFileSystem
{
  size_t                hash;                    //��� �������� ����� ������������
  stl::string           prefix;                  //������� ����� ������������
  ICustomFileSystemPtr  file_system;             //��������� �������� �������
  FileInfo              mount_point_info;        //���������� � ����� ������������
  MountPointFileSystem  mount_point_file_system; //��������� �������� �������

  MountFileSystem (const char* prefix,size_t hash,ICustomFileSystemPtr file_system);
  MountFileSystem (const MountFileSystem&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileSystemImpl
{  
  public:
    typedef FileSystem::LogHandler      LogHandler;
    typedef FileSystem::PackFileCreater PackFileCreater;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileSystemImpl ();
    ~FileSystemImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ���� ��� ������ ������, �������� ������ � ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetDefaultPath (const char* path);
    const char* GetDefaultPath () const { return default_path.c_str (); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddSearchPath        (const char* path,const LogHandler& log_handler);
    void RemoveSearchPath     (const char* path);
    void RemoveAllSearchPaths ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ���������������� ����� ���-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterPackFile   (const char* extension,const PackFileCreater& creater);
    void UnregisterPackFile (const char* extension);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ��������������� ����� ���-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Mount       (const char* path_prefix,ICustomFileSystemPtr file_system);
    void Unmount     (const char* path_prefix);
    void Unmount     (ICustomFileSystemPtr file_system);
    void UnmountAll  ();
    bool IsPathMount (const char* path) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////    
    FileImplPtr OpenFile (const char* src_file_name,filemode_t mode_flags);
    FileImplPtr OpenFile (const char* src_file_name,filemode_t mode_flags,size_t buffer_size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ����� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   SetDefaultFileBufferSize (size_t buffer_size);
    size_t GetDefaultFileBufferSize () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������������� �����, �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (const char* file_name);
    void Rename (const char* file_name,const char* new_name);
    void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool GetFileInfo (const char* file_name,FileInfo& info);
    bool IsFileExist (const char* file_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList Search (const char* wc_mask,size_t flags);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� ������ / �������� ���� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RegisterFile   (File&);
    void UnregisterFile (File&);
    void CloseAllFiles  ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* CompressPath (const char* path); //������������ ������������ ����

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �������� ������� ��������������� � ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    ICustomFileSystemPtr FindMountFileSystem (const char* path,stl::string& result_file_name);
    ICustomFileSystemPtr FindFileSystem      (const char* path,stl::string& result_file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���-�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AddPackFile (const char* path,size_t search_path_hash,const FileSystem::LogHandler& log_handler);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� � �������������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void MountSearch (FileListBuilder& builder,const char* wc_mask,const char* prefix,size_t flags);
   
  private:
    typedef stl::hash_set<File*>       OpenFileSet;
    typedef stl::list<SearchPath>      SearchPathList;
    typedef stl::list<PackFile>        PackFileList;
    typedef stl::list<PackFileType>    PackFileTypeList;
    typedef stl::list<MountFileSystem> MountList;  

  private:    
    OpenFileSet       open_files;               //������ �������� ������
    PackFileTypeList  pack_types;               //������ ����� ���-������
    PackFileList      pack_files;               //������ ���-������ � ���������������� �������� ������
    MountList         mounts;                   //������ �������������� �������� ������
    SearchPathList    search_paths;             //������ ����� ������
    FileImplPtr       closed_file;              //�������� ����
    stl::string       default_path;             //���� �� ��������� (������ �������� ��������)
    stl::string       compress_path;            //����� ��� ������������ ������������ ����
    size_t            default_file_buffer_size; //������ ������ ����� �� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef Singleton<FileSystemImpl> FileSystemSingleton;

}

#endif
