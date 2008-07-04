#include "shared.h"

struct TestScene
{
  Sprite::Pointer      sprite;
  OrthoCamera::Pointer camera;
  Scene                scene;  

  TestScene ()
  {
    sprite = Sprite::Create ();
    camera = OrthoCamera::Create ();    
    
    sprite->BindToScene (scene, NodeBindMode_Capture);
    camera->BindToScene (scene, NodeBindMode_Capture);

    sprite->SetName ("Sprite1");
    camera->SetName ("Camera1");
    sprite->SetMaterial ("sprite_material");
    sprite->SetColor (math::vec4f (1.0f, 1.0f, 1.0f));
    sprite->SetAlpha (0.5f);
    
    sprite->Scale (20, 20, 1);
    
    camera->SetPosition (0, 0, -3);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (-10);
    camera->SetZFar     (10);    
  }
};

void idle (TestApplication& app, TestScene& scene)
{
  try
  {
//    scene.sprite->Rotate (1, 0, 0, 1);
    
    app.PostRedraw ();
  }
  catch (std::exception& exception)
  {
    printf ("exception at idle: %s\n", exception.what ());
  }
}

int main ()
{
  printf ("Results of test_sprites:\n");

  try
  {    
      //������������� ����������

    TestApplication test;
    
      //������������� �������
      
    SceneRender& render = test.Render ();
    
    render.SetBackgroundColor (math::vec4f (1, 1, 1, 1));
    
      //�������� �����
      
    TestScene scene;
      
      //�������� �������� ������
      
    Viewport vp;
    
    vp.SetName       ("Viewport1");
    vp.SetRenderPath ("Render2d");
    vp.SetCamera     (scene.camera.get ());
    
    vp.SetArea (0, 0, 100, 100);
    
    render.Attach (vp);

      //��������� idle-�������

    test.SetIdleHandler (xtl::bind (&idle, _1, xtl::ref (scene)));

      //������ ����������

    return test.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
