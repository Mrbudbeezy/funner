#include "shared.h"

//��������� ������� �������� �����
class MyDesktopListener: public IDesktopListener, public xtl::reference_counter
{
  public:
    MyDesktopListener ()  { printf ("MyDesktopListener::MyDesktopListener\n"); }
    ~MyDesktopListener () { printf ("MyDesktopListener::~MyDesktopListener\n"); }

    void OnChangeName (const char* new_name)
    {
      printf ("OnChangeName(%s)\n", new_name);
    }
    
    void OnChangeBackgroundColor (const math::vec4f& new_color)
    {
      printf ("OnChangeBackgroundColor(%.2f, %.2f, %.2f, %.2f)\n", new_color.x, new_color.y, new_color.z, new_color.w);
    }

    void OnAttachViewport (Viewport& viewport)
    {
      printf ("OnAttachViewport(%s)\n", viewport.Name ());
    }

    void OnDetachViewport (Viewport& viewport)
    {
      printf ("OnDetachViewport(%s)\n", viewport.Name ());
    }
    
    void OnDestroy ()
    {
      printf ("OnDestroy()\n");
    }

    void AddRef  () { addref (this); }
    void Release () { release (this); }
};

void print_viewports (const Desktop& desktop)
{
  printf ("Desktop '%s' viewports (count=%u):", desktop.Name (), desktop.ViewportsCount ());

  for (size_t i=0; i<desktop.ViewportsCount (); i++)
    printf (" '%s'", desktop.Viewport (i).Name ());

  printf ("\n");          
}

int main ()
{
  printf ("Results of desktop_test:\n");

  try
  {
      //�������� �������� �����
      
    Desktop desktop1;
    
      //����������� ���������
      
    desktop1.AttachListener (xtl::com_ptr<MyDesktopListener> (new MyDesktopListener, false).get ());    
    
      //��������� ������� ����������
      
    desktop1.SetBackgroundColor (0.5f, 0.25f, 1.0f, 0.3f);
    desktop1.SetName            ("Desktop1");

      //������������� �������� ������
      
    Viewport viewport1, viewport2;
    
    viewport1.SetName ("Viewport1");
    viewport2.SetName ("Viewport2");
    
    desktop1.Attach (viewport1);
    desktop1.Attach (viewport2);

      //������� �������� ������

    print_viewports (desktop1);

      //����������� �������� �����

    Desktop desktop2 = desktop1;

      //�������� �������� ������

    desktop2.Detach (viewport1);

      //������� �������� ������

    print_viewports (desktop1);

      //������� ���������� �������� ������ � ����������� �� ��������

    desktop1.DetachAllViewports ();        
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
