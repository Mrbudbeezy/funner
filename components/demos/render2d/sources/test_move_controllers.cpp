#include "shared.h"

const char*  SPRITE_MATERIAL = "sprite_material";

const float ROTATION_SPEED = math::constants<float>::pi / 4;
const float STOP_TIME      = 30.f;

struct Test
{
  TestApplication          application;
  Sprite::Pointer          sprite1;
  Sprite::Pointer          sprite1_child1;
  Sprite::Pointer          sprite1_child2;
  Sprite::Pointer          sprite2;
  OrthoCamera::Pointer     camera;
  Scene                    scene;
  Screen                   screen;
  float                    current_angle;
  float                    sprite_position_radius;
  AlignWithNode::Pointer   aligner1;
  AlignWithNode::Pointer   aligner2;
  MoveToNodePoint::Pointer mover;
  LookToNodePoint::Pointer look_to;
  float                    duration;

  Test ()
    : current_angle (0), sprite_position_radius (8), duration (0)
  {
    sprite1 = CreateSprite ();
    sprite2 = CreateSprite ();

    sprite1_child1 = CreateSprite ();
    sprite1_child2 = CreateSprite ();

    sprite1_child1->SetOrientationInherit (false);
    sprite1_child2->SetOrientationInherit (false);

    sprite1_child1->BindToParent (*sprite2);
    sprite1_child2->BindToParent (*sprite2);

    sprite1_child1->SetPosition (-1, -1, 0);
    sprite1_child2->SetPosition (1, -1, 0);

    sprite1->SetColor (1.f, 1.f, 0.f, 1.f);
    sprite2->SetColor (1.f, 0.f, 1.f, 1.f);

    mover = MoveToNodePoint::Create (*sprite2);

    LinearAccelerationEvaluator move_acceleration;

    move_acceleration.SetAcceleration (15);
    move_acceleration.SetDeceleration (15);
    move_acceleration.SetMaxSpeed (5);

    mover->SetAccelerationHandler (move_acceleration);

    mover->Start (*sprite1, 0.f);

    look_to = LookToNodePoint::Create (*sprite2);

    LinearAccelerationEvaluator look_acceleration;

    look_acceleration.SetAcceleration (20);
    look_acceleration.SetDeceleration (20);
    look_acceleration.SetMaxSpeed (57);

    look_to->SetAccelerationHandler (look_acceleration);

    look_to->Start (*sprite1, 0.f, NodeOrt_Y, NodeOrt_Z);

    aligner1 = AlignWithNode::Create (*sprite1_child1);
    aligner2 = AlignWithNode::Create (*sprite1_child2);

    LinearAccelerationEvaluator align_acceleration;

    align_acceleration.SetAcceleration (10);
    align_acceleration.SetDeceleration (10);
    align_acceleration.SetMaxSpeed (27);

    aligner1->SetAccelerationHandler (align_acceleration);
    aligner2->SetAccelerationHandler (align_acceleration);

    aligner1->Start (*sprite2, NodeOrt_Y, NodeOrt_Y, NodeOrt_Z);
    aligner2->Start (*sprite2, NodeOrt_Y, NodeOrt_Y, NodeOrt_Z);

      //создание сцены
    camera = OrthoCamera::Create ();

    camera->BindToScene (scene);
    camera->SetName     ("Camera1");
    camera->SetPosition (0, 0, -3);
    camera->SetLeft     (-10);
    camera->SetRight    (10);
    camera->SetTop      (10);
    camera->SetBottom   (-10);
    camera->SetZNear    (-20);
    camera->SetZFar     (20);

      //создание областей вывода

    Viewport vp;

    screen.SetBackgroundColor (0, 0, 0, 0);
    screen.SetBackgroundState (false);

    vp.SetName            ("Viewport1");
    vp.SetTechnique       ("Render2d");
    vp.SetCamera          (camera.get ());
    vp.SetBackgroundColor (0, 0, 0, 0);
    vp.EnableBackground   ();

    vp.SetArea (0, 0, 100, 100);

    screen.Attach (vp);

      //настройка целевых буферов вывода

    RenderTarget& render_target = application.RenderTarget ();

    render_target.SetScreen (&screen);

      //загрузка ресурсов

    application.LoadResources ();

      //установка idle-функции

    application.SetIdleHandler (xtl::bind (&Test::Idle, this));
  }

  Sprite::Pointer CreateSprite ()
  {
    Sprite::Pointer sprite = Sprite::Create ();

    sprite->SetPosition (cos (current_angle) * sprite_position_radius, sin (current_angle) * sprite_position_radius, 0.f);
    sprite->SetColor    (1.f, 1.f, 1.f, 1.f);
    sprite->SetMaterial (SPRITE_MATERIAL);
    sprite->BindToScene (scene);

    return sprite;
  }

    //обработчик главного цикла приложения
  void Idle ()
  {
    try
    {
      static size_t last_update_time = common::milliseconds ();

      size_t current_time = common::milliseconds ();

      float dt = (current_time - last_update_time) / 1000.f;

      duration += dt;

      if (duration > STOP_TIME)
      {
        mover->Stop ();
        look_to->Stop ();
      }

      current_angle += ROTATION_SPEED * dt;

      sprite1->SetPosition (cos (current_angle) * sprite_position_radius, sin (current_angle) * sprite_position_radius, 0.f);

      scene.Root ().Update (TimeValue (size_t (dt * 1000), 1000));

      last_update_time = current_time;

      application.PostRedraw ();
    }
    catch (std::exception& exception)
    {
      printf ("exception at idle: %s\n", exception.what ());
    }
  }
};

void log_print (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

int main ()
{
  printf ("Results of test_move_to_node_point_controller:\n");

  try
  {
      //настройка протоколированиЯ

    common::LogFilter filter ("*", &log_print);

      //запуск теста

    Test test;

    return test.application.Run ();
  }
  catch (std::exception& e)
  {
    printf ("exception: %s\n", e.what ());
    return 1;
  }
}
