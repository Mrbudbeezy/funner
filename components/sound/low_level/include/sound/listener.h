#ifndef SOUND_LISTENER_HEADER
#define SOUND_LISTENER_HEADER

#include <math/vector.h>

namespace sound
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Listener
{
  math::vec3f position;    //позиция
  math::vec3f direction;   //направление
  math::vec3f up;          //направление вверх
  math::vec3f velocity;    //скорость
};

}

#endif
