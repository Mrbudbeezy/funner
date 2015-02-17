#ifndef COMMONLIB_FILE_SYSTEM_HEADER
#define COMMONLIB_FILE_SYSTEM_HEADER

#include <ctime>

#include <xtl/intrusive_ptr.h>
#include <xtl/functional_fwd>
#include <xtl/exception.h>

#include <stl/string_fwd>

#include <common/action_queue.h>

#ifdef GetFreeSpace
#undef GetFreeSpace
#endif

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

typedef size_t             filemode_t;
typedef unsigned long long filesize_t;
typedef time_t             filetime_t;
typedef long long          filepos_t;
typedef char               filecryptokey_t [16];

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

    virtual file_t     FileOpen   (const char* name, filemode_t mode_flags, size_t buffer_size) = 0;
    virtual void       FileClose  (file_t) = 0;
    virtual size_t     FileBufferSize (file_t) { return 0; } //ret (size_t)-1 - ����������� ����������
    virtual size_t     FileRead   (file_t, void* buf, size_t size) = 0;
    virtual size_t     FileWrite  (file_t, const void* buf, size_t size) = 0;
    virtual void       FileRewind (file_t) = 0;
    virtual filepos_t  FileSeek   (file_t, filepos_t pos) = 0;
    virtual filepos_t  FileTell   (file_t) = 0;
    virtual filesize_t FileSize   (file_t) = 0;
    virtual void       FileResize (file_t, filesize_t new_size) = 0; 
    virtual bool       FileEof    (file_t) = 0;    
    virtual void       FileFlush  (file_t) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void Remove (const char* file_name) = 0;
    virtual void Rename (const char* file_name, const char* new_name) = 0;
    virtual void Mkdir  (const char* dir_name) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool IsFileExist (const char* file_name) = 0;
    virtual bool GetFileInfo (const char* file_name, FileInfo& info) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual filesize_t GetFreeSpace  (const char* path) = 0;  //returns (filesize_t)-1 if free space can't be determined
    virtual filesize_t GetTotalSpace (const char* path) = 0;  //returns (filesize_t)-1 if free space can't be determined

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void SetFileAttribute    (const char* file_name, const char* attribute, const void* data, size_t size) = 0;
    virtual void GetFileAttribute    (const char* file_name, const char* attribute, void* data, size_t size) = 0;
    virtual bool HasFileAttribute    (const char* file_name, const char* attribute) = 0;
    virtual void RemoveFileAttribute (const char* file_name, const char* attribute) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char* file, const FileInfo& info)> FileSearchHandler;

    virtual void Search (const char* wc_mask, const FileSearchHandler& handler) = 0;

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
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    File  ();
    File  (const File&);
    ~File ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    File& operator = (const File&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Path () const;

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
    size_t Read  (void* buf, size_t size);
    size_t Write (const void* buf, size_t size);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void      Rewind ();
    filepos_t Seek   (filepos_t pos, FileSeekMode mode = FileSeekMode_Set);
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
    
    size_t BufferSize () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� ������ �� ���������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool operator == (const File&) const;
    bool operator != (const File&) const;    
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� ����� (��� ����������� �������������)
///////////////////////////////////////////////////////////////////////////////////////////////////    
    FileImplPtr GetImpl () const;

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
    StdFile (const char* file_name, filemode_t mode_flags);
    StdFile (const char* file_name, filemode_t mode_flags, size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class InputFile: public StdFile
{
  public:
    explicit InputFile (const char* file_name);
             InputFile (const char* file_name, size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class OutputFile: public StdFile
{
  public:
    explicit OutputFile (const char* file_name);
             OutputFile (const char* file_name, size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class TempFile: public StdFile
{
  public:
    explicit TempFile (const char* file_name_pattern); //printf-like format string
             TempFile (const char* file_name_pattern, size_t buffer_size); //printf-like format string
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ����������� ���� ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class AppendFile: public StdFile
{
  public:
    explicit AppendFile (const char* file_name);
             AppendFile (const char* file_name, size_t buffer_size);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MemFile: public File
{
  public:
    MemFile (size_t buffer_reserved_size = 0, filemode_t mode_flags = FileMode_ReadWrite);
    MemFile (void* buf, size_t size, filemode_t mode_flags = FileMode_Write | FileMode_Read | FileMode_Seek | FileMode_Rewind);
    
    void* Buffer () const;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class CustomFile: public File
{
  public:
    CustomFile (ICustomFileSystemPtr file_system, const char* file_name, filemode_t mode);
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
                unsigned short key_bits);
    CryptoFile (const File& source_file, 
                const char* read_crypto_method, 
                const char* write_crypto_method, 
                const void* key, 
                unsigned short key_bits);
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
    ������ �������������� ��������� ���������� ������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class FileCryptoParameters
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    FileCryptoParameters  (const char* read_method, const char* write_method, const void* key, unsigned short key_bits);
    FileCryptoParameters  (const char* read_method, const char* write_method, const char* key_string);
    FileCryptoParameters  (const FileCryptoParameters&);
    ~FileCryptoParameters ();

    FileCryptoParameters& operator = (const FileCryptoParameters&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*    ReadMethod  () const; //����� ���������� ��� ������ �� �����
    const char*    WriteMethod () const; //����� ���������� ��� ������ � ����
    const void*    Key         () const; //��������� �� �����, ���������� ���� ����������
    unsigned short KeyBits     () const; //���������� ����� � �����

  private:
    struct Impl;
    Impl* impl;
};



/*
    ������ �������������� ���������� �������� ��������
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum BackgroundCopyStateStatus
{
  BackgroundCopyStateStatus_Started,      //����������� ��������
  BackgroundCopyStateStatus_InProgress,   //����������� � ��������
  BackgroundCopyStateStatus_Finished,     //����������� ������� ���������
  BackgroundCopyStateStatus_Failed        //��������� ������ �� ����� �����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class BackgroundCopyState
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������/����������/�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BackgroundCopyState  ();
    BackgroundCopyState  (const BackgroundCopyState&);
    ~BackgroundCopyState ();

    BackgroundCopyState& operator = (const BackgroundCopyState&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    BackgroundCopyState Clone () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
//��������� ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void       SetFileSize (filesize_t size);
    filesize_t FileSize    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void       SetBytesCopied (filesize_t size);
    filesize_t BytesCopied    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
////������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                      SetStatus (BackgroundCopyStateStatus status);
    BackgroundCopyStateStatus Status    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetStatusText (const char* error);
    const char* StatusText    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (BackgroundCopyState&);
    
  private:
    struct Impl;

    BackgroundCopyState (Impl*);

  private:
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (BackgroundCopyState&, BackgroundCopyState&);

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
    static void AddSearchPath        (const char* path, const LogHandler& log_handler);
    static void RemoveSearchPath     (const char* path);
    static void RemoveAllSearchPaths ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / �������� ���������������� ����� ���-������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterPackFile   (const char* extension, const PackFileCreater& creater);
    static void UnregisterPackFile (const char* extension);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������������� �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Mount       (const char* path_prefix, ICustomFileSystemPtr file_system);
    static void Mount       (const char* path_prefix, const char* path, const char* force_extension=0);
    static void MountLink   (const char* path_prefix, const char* path);
    static void Unmount     (const char* path_prefix);
    static void Unmount     (ICustomFileSystemPtr file_system);
    static void UnmountAll  ();
    static bool IsPathMount (const char* path); //��������: ����������� �� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetCryptoParameters (const char* path, 
                                     const char* read_crypto_method, 
                                     const char* write_crypto_method, 
                                     const void* key, 
                                     unsigned short key_bits);
    static void                 SetCryptoParameters       (const char* path, const FileCryptoParameters& parameters);
    static bool                 HasCryptoParameters       (const char* path);
    static FileCryptoParameters GetCryptoParameters       (const char* path);
    static void                 RemoveCryptoParameters    (const char* path);
    static void                 RemoveAllCryptoParameters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����� ���������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void GetFileCryptoKey (const char* file_name, filecryptokey_t key);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void SetFileAttribute    (const char* file_name, const char* attribute, const void* data, size_t size);
    static void GetFileAttribute    (const char* file_name, const char* attribute, void* data, size_t size);
    static bool HasFileAttribute    (const char* file_name, const char* attribute);
    static void RemoveFileAttribute (const char* file_name, const char* attribute);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� / �������������� �����, �������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Remove (const char* file_name);
    static void Rename (const char* file_name, const char* new_name);
    static void Mkdir  (const char* dir_name);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void        CopyFile       (const char* source_file_name, const char* destination_file_name, size_t buffer_size = 0);
    static stl::string CopyToTempFile (const char* source_file_name);

    typedef xtl::function<void (const BackgroundCopyState&)> BackgroundCopyFileCallback;

    static Action BackgroundCopyFile (const char* source_file_name, const char* destination_file_name, const BackgroundCopyFileCallback& callback, ActionThread thread, size_t buffer_size = 0);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool       GetFileInfo (const char* file_name, FileInfo& info);
    static bool       IsFileExist (const char* file_name);
    static bool       IsDir       (const char* file_name);
    static filetime_t GetFileTime (const char* file_name);
    static filesize_t GetFileSize (const char* file_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static filesize_t GetFreeSpace  (const char* path);  //returns (unsigned long long)-1 if free space can't be determined
    static filesize_t GetTotalSpace (const char* path);  //returns (unsigned long long)-1 if free space can't be determined

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��� ���� ����������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////        
    static void GetFileHash (File& file, FileHash& out_hash_value);
    static void GetFileHash (File& file, size_t max_hash_size, FileHash& out_hash_value);    
    static void GetFileHash (const char* file_name, FileHash& out_hash_value);
    static void GetFileHash (const char* file_name, size_t max_hash_size, FileHash& out_hash_value);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static FileList Search (const char* wc_mask, size_t flags=FileSearch_FilesAndDirs);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ ����� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void   SetDefaultFileBufferSize (size_t buffer_size);
    static size_t GetDefaultFileBufferSize ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void   SetFileBufferSize (const char* file_name, size_t buffer_size);
    static size_t GetFileBufferSize (const char* file_name);

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
