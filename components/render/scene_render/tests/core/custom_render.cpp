#include "shared.h"

Screen* dynamic_screen_ptr = 0;

void debug_print (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

void log_print (const char* message)
{
  printf ("%s\n", message);
}

void setup_render_target (RenderTarget& render_target, const char* query_string)
{
  printf ("Create dynamic render target. Query='%s'\n", query_string);
  
  render_target.SetScreen (dynamic_screen_ptr);
}

int main ()
{
  printf ("Results of custom_render_test:\n");
  
  try
  {
      //��������� ����������������

    xtl::auto_connection log_connection = common::LogSystem::RegisterLogHandler ("render.mid_level.Debug", &debug_print);

      //����������� ����������������� �������
    
    SceneRenderManager::RegisterRender ("MySceneRender", &MySceneRender::Create);

      //�������� ������� �����

    SceneRender render ("*", "*", "MySceneRender");

      //�������� �����

    OrthoCamera::Pointer camera1 = OrthoCamera::Create (), camera2 = OrthoCamera::Create ();
    
      //��������� ������� ������

    camera1->SetName ("Camera1");
    camera2->SetName ("Camera2");

    render.SetLogHandler (&log_print);
    
    Viewport vp1;
    Screen screen, dynamic_screen;
    
    dynamic_screen_ptr = &dynamic_screen;
    
    screen.SetBackgroundState (false);

    vp1.SetArea       (0, 0, 100, 100);
    vp1.SetCamera     (&*camera1);
    vp1.SetRenderPath ("MySceneRender");
    
    screen.Attach (vp1);
    
    RenderTarget render_target = render.RenderTarget (0);
    
    render_target.SetScreen (&screen);
    
      //��������� �������� ����������
      
    render.RegisterQueryHandler ("test_*", &setup_render_target);
    
      //�������� ��������

    render.LoadResource ("data/materials.xmtl");

      //���������

    render_target.Update ();
    
      //������������� ������ � �����
      
    printf ("Bind camera to scene\n");
      
    Scene scene1, scene2;

    scene1.SetName ("Scene1");
    scene2.SetName ("Scene2");

    camera1->BindToScene (scene1);
    camera2->BindToScene (scene2);

    render_target.Update ();
    
      //����� �����
      
    printf ("Change camera scene\n");

    camera1->BindToScene (scene2);

    render_target.Update ();
    
      //��������� ������
      
    printf ("Change viewport camera\n");
    
    vp1.SetCamera (&*camera2);

    camera1->BindToScene (scene1);

    render_target.Update ();    
    
      //������������ ������ �� �����
      
    printf ("Unbind camera from scene\n");
    
    camera2->Unbind ();
    
    render_target.Update ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
