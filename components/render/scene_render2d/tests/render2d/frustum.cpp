#include "shared.h"

int main ()
{
  printf ("Results of frustum_test:\n");

  try
  {
      //��������� ����������������

    common::LogFilter log_filter ("render.mid_level.Debug", &print);

      //�������� �����

    Sprite::Pointer      visible_sprite = Sprite::Create (), not_visible_sprite = Sprite::Create ();
    OrthoCamera::Pointer camera = OrthoCamera::Create ();

    not_visible_sprite->Translate (0, 0, 2);

    Scene scene;

    visible_sprite->BindToScene (scene, NodeBindMode_WeakRef);
    not_visible_sprite->BindToScene (scene, NodeBindMode_WeakRef);
    camera->BindToScene (scene, NodeBindMode_WeakRef);

    visible_sprite->SetMaterial ("sprite_material");
    not_visible_sprite->SetMaterial ("sprite_material");

      //�������� �������

    SceneRender render ("Debug", "Renderer2d", "Render2d");

    render.SetLogHandler (&log_print);

      //��������� �������� ������

    Viewport vp1;

    vp1.SetArea       (0, 0, 100, 100);
    vp1.SetCamera     (&*camera);
    vp1.SetRenderPath ("Render2d");

    Screen screen;

    screen.Attach (vp1);

    RenderTarget render_target = render.RenderTarget (0);

    render_target.SetScreen (&screen);

      //�������� ��������

    render.LoadResource ("data/materials.xmtl");

      //���������

    render_target.Update ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
