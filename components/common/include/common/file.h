#ifndef COMMONLIB_FILE_SYSTEM_HEADER
#define COMMONLIB_FILE_SYSTEM_HEADER

#include <ctime>

#include <xtl/intrusive_ptr.h>
#include <xtl/functional_fwd>
#include <xtl/exception.h>

#include <stl/string_fwd>

namespace common
{

//implementation forwards
class FileImpl;
class FileList;
class FileListImpl;
class FileListBuilder;
class ICustomFileSystem;

//intrusive pointers
typedef xtl::intrusive_ptr<FileImpl>     FileImplPtr;
typedef xtl::intrusive_ptr<FileListImpl> FileListImplPtr;
typedef xtl::com_ptr<ICustomFileSystem>  ICustomFileSystemPtr;

/*
    �������� ����������
*/

struct FileException:         virtual public xtl::exception {}; //��� �������� ������ �������� ����������
struct FileNotFoundException:         public FileException {};  //���� �� ������
struct FileNotDirException:           public FileException {};  //���� �� �������� ���������
struct FileLoadException:             public FileException {};  //������ �������� �����
struct FileClosedException:           public FileException {};  //������� ��������� � ��������� �����
struct FileNoSpaceException:          public FileException {};  //������������ ����� �� �������� ��� ���������� ��������
struct FileMountException:            public FileException {};  //������ ������������ �������� �������
struct BufferedFileException:         public FileException {};  //���������� ��������� � ������������ �����

/*
    �������� ���������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������� 
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FileMode
{
  FileMode_Read        = 1,  //������ ���������
  FileMode_Write       = 2,  //������ ���������
  FileMode_Resize      = 4,  //��������� ������� ���������
  FileMode_Rewind      = 8,  //����� ��������� ��������� ��������
  FileMode_Seek        = 16, //����������� ��������� ��������� ���������
  FileMode_Create      = 32, //��� ���������� ���� ��������, ������ ����� ���������
  FileMode_StreamRead  = FileMode_Read | FileMode_Rewind,                        //������ ������
  FileMode_StreamWrite = FileMode_Write | FileMode_Rewind | FileMode_Resize,     //������ ������
  FileMode_ReadOnly    = FileMode_StreamRead | FileMode_Seek,                    //������ ����� ������������� �������
  FileMode_WriteOnly   = FileMode_Create | FileMode_StreamWrite | FileMode_Seek, //������ ����� ������������� ������� (����� ������� �����)
  FileMode_ReadWrite   = FileMode_ReadOnly | FileMode_StreamWrite,               //������/������ ����� ������������� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FileSearch
{
  FileSearch_Files        = 1,  //������ �����
  FileSearch_Dirs         = 2,  //������ ��������
  FileSearch_Sort         = 4,  //������������� �� �����
  FileSearch_NoPacks      = 8,  //�� ������ � �����
  FileSearch_SubDirs      = 16, //������ � ������������
  FileSearch_FullPaths    = 32, //�������� ������ ����
  FileSearch_FilesAndDirs = FileSearch_Files | FileSearch_Dirs
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum FileSeekMode
{
  FileSeekMode_Set,     //��������� ������� �� ������ �����
  FileSeekMode_Current, //��������� ������� �� �������� ��������� ��������� ���������
  FileSeekMode_End      //��������� ������� �� ����� �����
};

typedef size_t filesize_t, filemode_t;
typedef time_t filetime_t;
typedef int    filepos_t;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FileInfo
{
  filetime_t time_create;  //����� �������� �����
  filetime_t time_access;  //����� ���������� ������� � �����
  filetime_t time_modify;  //����� ��������� ����������� �����
  filesize_t size;         //������ �����
  bool       is_dir;       //�������� �� ���� �����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FileHash
{
  size_t        crc32;    //���-����� CRC32
  unsigned char md5 [16]; //���-����� MD5
};

/*
    ������������� ���������� �������� �������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class ICustomFileSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef void* file_t;

    virtual file_t     FileOpen   (const char* name,filemode_t mode_flags,size_t buffer_size) = 0;
    virtual void       FileClose  (file_t) = 0;
    virtual size_t     FileBufferSize (file_t) { return 0; } //ret (size_t)-1 - ����������� ����������
    virtual size_t     FileRead   (file_t,void* buf,size_t size) = 0;
    virtual size_t     FileWrite  (file_t,const void* buf,size_t size) = 0;
    virtual void       FileRewind (file_t) = 0;
    virtual filepos_t  FileSeek   (file_t,filepos_t pos) = 0;
    virtual filepos_t  FileTell   (file_t) = 0;
    virtual filesize_t FileSize   (file_t) = 0;
    virtual void       FileResize (file_t,filesize_t new_size) = 0; 
    virtual bool       FileEof    (file_t) = 0;    
    virtual void       FileFlush  (file_t) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Remove (const char* file_name) = 0;
    virtual void Rename (const char* file_name,const char* new_name) = 0;
    virtual void Mkdir  (const char* dir_name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsFileExist (const char* file_name) = 0;
    virtual bool GetFileInfo (const char* file_name,FileInfo& info) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* file, const FileInfo& info)> FileSearchHandler;

    virtual void Search (const char* wc_mask,const FileSearchHandler& handler) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;
    
  protected:
    virtual ~ICustomFileSystem () {}
};

/*
    ������ ��� ������ � ������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class File
{
  friend class FileSystem;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    File  ();
    File  (const File&);
    ~File ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    File& operator = (const File&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    filemode_t Mode        () const;
    bool       CanRead     () const;
    bool       CanWrite    () const;
    bool       CanResize   () const;
    bool       CanSeek     () const;
    bool       CanRewind   () const;
    bool       IsReadOnly  () const;
    bool       IsWriteOnly () const;
    bool       IsClosed    () const;
    bool       IsBuffered  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ / ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Read  (void* buf,size_t size);
    size_t Write (const void* buf,size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void      Rewind ();
    filepos_t Seek   (filepos_t pos,FileSeekMode mode = FileSeekMode_Set);
    filepos_t Tell   () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����� / �������� �� ����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    filesize_t Size   () const;
    void       Resize (filesize_t new_size); 
    bool       Eof    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Flush ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ������ �� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const File&) const;
    bool operator != (const File&) const;

  protected:
    File (FileImplPtr);

  private:
    FileImplPtr impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class StdFile: public File
{
  public:
    StdFile (const char* file_name,filemode_t mode_flags);
    StdFile (const char* file_name,filemode_t mode_flags,size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputFile: public StdFile
{
  public:
    explicit InputFile (const char* file_name);
             InputFile (const char* file_name,size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputFile: public StdFile
{
  public:
    explicit OutputFile (const char* file_name);
             OutputFile (const char* file_name,size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AppendFile: public StdFile
{
  public:
    explicit AppendFile (const char* file_name);
             AppendFile (const char* file_name,size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemFile: public File
{
  public:
    MemFile (void* buf,size_t size,filemode_t mode_flags=FileMode_ReadWrite);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class CustomFile: public File
{
  public:
    CustomFile (ICustomFileSystemPtr file_system,const char* file_name,filemode_t mode);
    CustomFile (ICustomFileSystemPtr      file_system,
                ICustomFileSystem::file_t handle,
                filemode_t                mode,
                bool                      auto_close = false);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� � �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class CryptoFile: public File
{
  public:
    CryptoFile (const File& source_file,
                const char* read_crypto_method,
                const void* key,
                size_t      key_bits);
    CryptoFile (const File& source_file,
                const char* read_crypto_method,
                const char* write_crypto_method,
                const void* key,
                size_t      key_bits);
};

/*
    ������ ������������� ����� ������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct FileListItem
{
  const char* name; //��� �����
  FileInfo    info; //���������� � �����
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileListIterator
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileListIterator  ();
    FileListIterator  (const FileList&);
    FileListIterator  (const FileListIterator&);
    ~FileListIterator ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileListIterator& operator = (const FileListIterator&);
    FileListIterator& operator = (const FileList&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileListIterator& operator ++ ();
    FileListIterator& operator -- ();
    FileListIterator  operator ++ (int);
    FileListIterator  operator -- (int);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const FileListItem* operator -> () const;
    const FileListItem& operator *  () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    operator bool () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const FileListIterator&) const;
    bool operator != (const FileListIterator&) const;
    bool operator <  (const FileListIterator&) const;
    bool operator >  (const FileListIterator&) const;
    bool operator <= (const FileListIterator&) const;
    bool operator >= (const FileListIterator&) const;          

  private:
    FileListImplPtr list_impl;
    size_t          pos;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileList
{
  friend class FileListIterator;
  friend class FileListBuilder;
  public:
    typedef FileListIterator Iterator;
    typedef FileListItem     ItemType;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList  ();
    FileList  (const FileList&);
    ~FileList ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileList& operator = (const FileList&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������ / ���������� �� i-� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t          Size () const;
    const ItemType& Item (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Iterator GetIterator () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ������� �� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const FileList&) const;
    bool operator != (const FileList&) const;

  private:
    FileList (FileListImpl*);

  private:
    FileListImplPtr impl;
};

/*
    ������ �������������� ���������� �������� ��������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileSystem
{
  public:
    typedef xtl::function<void (const char* message)>            LogHandler;
    typedef xtl::function<ICustomFileSystem* (const char* path)> PackFileCreater;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        SetCurrentDir (const char* path);
    static const char* GetCurrentDir ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void AddSearchPath        (const char* path);
    static void AddSearchPath        (const char* path,const LogHandler& log_handler);
    static void RemoveSearchPath     (const char* path);
    static void RemoveAllSearchPaths ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ���������������� ����� ���-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPackFile   (const char* extension,const PackFileCreater& creater);
    static void UnregisterPackFile (const char* extension);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Mount       (const char* path_prefix,ICustomFileSystemPtr file_system);
    static void Unmount     (const char* path_prefix);
    static void Unmount     (ICustomFileSystemPtr file_system);
    static void UnmountAll  ();
    static bool IsPathMount (const char* path); //��������: ����������� �� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������������� �����, �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Remove (const char* file_name);
    static void Rename (const char* file_name,const char* new_name);
    static void Mkdir  (const char* dir_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool       GetFileInfo (const char* file_name,FileInfo& info);
    static bool       IsFileExist (const char* file_name);
    static bool       IsDir       (const char* file_name);
    static filetime_t GetFileTime (const char* file_name);
    static filesize_t GetFileSize (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ���� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////        
    static void GetFileHash (File& file,FileHash& out_hash_value);
    static void GetFileHash (const char* file_name,FileHash& out_hash_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static FileList Search (const char* wc_mask,size_t flags=FileSearch_FilesAndDirs);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ����� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void   SetDefaultFileBufferSize (size_t buffer_size);
    static size_t GetDefaultFileBufferSize ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���� �������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void CloseAllFiles ();    

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ����� � ������������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static stl::string GetNormalizedFileName (const char* file_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ����� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        LoadTextFile (const char* file_name, stl::string& buffer);
    static stl::string LoadTextFile (const char* file_name);
};

}

#endif
