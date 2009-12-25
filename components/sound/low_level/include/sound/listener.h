#ifndef SOUND_LISTENER_HEADER
#define SOUND_LISTENER_HEADER

#include <math/vector.h>

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
