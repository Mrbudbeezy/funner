#ifndef SOUND_SHARED_HEADER
#define SOUND_SHARED_HEADER

#include <mathlib.h>

namespace sound
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Listener
{
  math::vec3f position;    //�������
  math::vec3f direction;   //�����������
  math::vec3f up;          //����������� �����
  math::vec3f velocity;    //��������  
};

}

#endif
