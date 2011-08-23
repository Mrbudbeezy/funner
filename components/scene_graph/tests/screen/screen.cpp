#include "shared.h"

//��������� ������� �������� �����
class MyScreenListener: public IScreenListener
{
  public:
    void OnScreenChangeName (const char* new_name)
    {
      printf ("OnChangeName(%s)\n", new_name);
    }
    
    void OnScreenChangeArea (const Rect& rect)
    {
      printf ("OnChangeArea(%d, %d, %u, %u)\n", rect.x, rect.y, rect.width, rect.height);
    }
    
    void OnScreenChangeBackground (bool new_state, const math::vec4f& new_color)
    {
      printf ("OnChangeBackground(%s, %.2f, %.2f, %.2f, %.2f)\n", new_state ? "true" : "false", new_color.x, new_color.y, new_color.z, new_color.w);
    }

    void OnScreenAttachViewport (Viewport& viewport)
    {
      printf ("OnAttachViewport(%s)\n", viewport.Name ());
    }

    void OnScreenDetachViewport (Viewport& viewport)
    {
      printf ("OnDetachViewport(%s)\n", viewport.Name ());
    }
    
    void OnScreenDestroy ()
    {
      printf ("OnDestroy()\n");
    }
};

void print_viewports (const Screen& Screen)
{
  printf ("Screen '%s' viewports (count=%u):", Screen.Name (), Screen.ViewportsCount ());

  for (size_t i=0; i<Screen.ViewportsCount (); i++)
    printf (" '%s'", Screen.Viewport (i).Name ());

  printf ("\n");          
}

int main ()
{
  printf ("Results of Screen_test:\n");

  try
  {
    MyScreenListener listener;
    
      //�������� �������� �����
      
    Screen screen1;
    
      //����������� ���������
      
    screen1.AttachListener (&listener);
    
      //��������� ������� ����������
      
    screen1.SetArea            (10, 20, 300, 40);
    screen1.SetBackgroundColor (0.5f, 0.25f, 1.0f, 0.3f);
    screen1.DisableBackground  ();
    screen1.SetName            ("Screen1");

      //������������� �������� ������

    Viewport viewport1, viewport2;

    viewport1.SetName ("Viewport1");
    viewport2.SetName ("Viewport2");

    screen1.Attach (viewport1);
    screen1.Attach (viewport2);

      //������� �������� ������

    print_viewports (screen1);

      //����������� �������� �����

    Screen screen2 = screen1;

      //�������� �������� ������

    screen2.Detach (viewport1);

      //������� �������� ������

    print_viewports (screen1);

      //������� ���������� �������� ������ � ����������� �� ��������

    screen1.DetachAllViewports ();        
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
