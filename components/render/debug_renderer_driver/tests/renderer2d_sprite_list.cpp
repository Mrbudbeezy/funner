#include "shared.h"

using namespace math;

int main ()
{
  printf ("Results of renderer2d_sprite_list_test:\n");

  try
  {
    Test test;
    
      //ᮧ����� �������� ७���
    
    RendererPtr basic_renderer (DriverManager::CreateRenderer ("debug", "renderer2d"), false);
    
      //�ਢ������ � �㬥୮�� ७����
    
    Renderer2dPtr renderer = xtl::dynamic_pointer_cast<renderer2d::IRenderer> (basic_renderer);
    
      //ᮧ����� ᯨ᪠ �ࠩ⮢
      
    Sprite sprites [4];
    
    size_t sprites_count = sizeof (sprites) / sizeof (*sprites);
    
    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite& s = sprites [i];
      
      s.position   = vec3f (float (i), float (i), float (i));
      s.size       = vec2f (1.0f, float (i * i));
      s.color      = vec4f (0.0f, 1.0f, 0.0f, float (i));
      s.tex_offset = vec2f (0.0f, 0.0f);
      s.tex_size   = vec2f (0.5f, 0.5f);
    }
    
    SpriteListPtr sprite_list (renderer->CreateSpriteList (sprites_count), false);
    
    sprite_list->SetSprites (0, sprites_count, sprites);
    
      //��ᬮ�� ᯨ᪠ �ࠩ⮢
      
    Sprite result_sprites [10];
    
    printf ("SpriteList (size=%u):\n", sprite_list->GetSize ());
    
    sprite_list->GetSprites (0, sprites_count, result_sprites);
    
    for (size_t i=0; i<sprites_count; i++)
    {
      Sprite& s = result_sprites [i];
      
      printf ("sprite #%u: position=[%.0f %.0f %.0f] size=[%.0f %.0f] color=[%.0f %.0f %.0f %.0f] tex_offset=[%.0f %.0f] tex_size=[%.0f %.0f]\n",            
        i, s.position.x, s.position.y, s.position.z, s.size.x, s.size.y, s.color.x, s.color.y, s.color.z, s.color.w, 
        s.tex_offset.x, s.tex_offset.y, s.tex_size.x, s.tex_size.y);
    }    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
