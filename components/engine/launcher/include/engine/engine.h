#ifndef FUNNER_ENGINE_HEADER
#define FUNNER_ENGINE_HEADER

#ifdef _WIN32
  #define FUNNER_C_API extern "C" __declspec(dllexport)
#elif __GNUC__
  #define FUNNER_C_API extern "C"
#else
  #error "Unknown platform"
#endif

namespace engine
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MouseButton
{
  MouseButton_Left,
  MouseButton_Right,
  MouseButton_Middle,
  MouseButton_X1,
  MouseButton_X2
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindowListener
{
  public:
    ///���������� ������� ����������� ����
    virtual void OnPaint () {}
    
    ///���������� ��������� �������� ����
    virtual void OnSize (size_t width, size_t height) {}

    ///����������� ������� �����
    virtual void OnMouseMove        (int x, int y) {}
    virtual void OnMouseEnter       (int x, int y) {}
    virtual void OnMouseHover       (int x, int y) {}
    virtual void OnMouseLeave       (int x, int y) {}
    virtual void OnMouseDown        (MouseButton button, int x, int y) {}
    virtual void OnMouseUp          (MouseButton button, int x, int y) {}
    virtual void OnMouseDoubleClick (MouseButton button, int x, int y) {}
    virtual void OnKeyDown          (const char* key) {}
    virtual void OnKeyUp            (const char* key) {}

  protected:
    virtual ~IWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IWindow
{
  public:
    ///����������
    virtual ~IWindow () {}
    
    ///����� / ������� ����
    virtual void Show (bool state) = 0;
    
    ///��������� ����������� ������������� ����
    virtual void SetParentHandle (void* handle) = 0;
    
    ///��������� ����������� ������������� ����
    virtual void* GetParentHandle () = 0;
    
    ///��������� �������� ����
    virtual void SetSize (size_t width, size_t height) = 0;
    
    ///��������� ������ ����
    virtual size_t GetWidth () = 0;
    
    ///��������� ������ ����
    virtual size_t GetHeight () = 0;
    
    ///��������� ��������� ����
    virtual void SetPosition (int x, int y) = 0;

    ///��������� �������� ��������� ����
    virtual int GetX () = 0;

    ///��������� �������� ��������� ����
    virtual int GetY () = 0;

    ///���������� ��������� �������
    virtual void AttachListener (IWindowListener* listener) = 0;
    
    ///�������� ��������� �������
    virtual void DetachListener (IWindowListener* listener) = 0;
    
    ///�������� ���� ���������� �������
    virtual void DetachAllListeners () = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IEngine
{
  public:
    ///����������
    virtual ~IEngine () {}
    
    ///��������� ������� ����������
    virtual void SetBaseDir (const char* dir_name) = 0;
    
    ///��������� ������� ����������
    virtual const char* GetBaseDir () = 0;

    ///������ ��������� ������ � ���������������� ������� ������
    virtual bool ParseCommandLine (unsigned int arguments_count, const char** arguments, const char** env = 0) = 0;

    ///������ �������� �����
    virtual void Run () = 0;

    ///�������� ���������� ��������� ������� ����
    virtual IWindow* CreateWindow (const char* name) = 0;

    ///������ �������� �����
    virtual void Execute (const char* command) = 0;
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef engine::IEngine* (*FunnerInitProc)();

FUNNER_C_API engine::IEngine* FunnerInit ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� main �������
///////////////////////////////////////////////////////////////////////////////////////////////////
FUNNER_C_API int FunnerMain (int argc, const char** argv, const char** env = 0);

#endif
