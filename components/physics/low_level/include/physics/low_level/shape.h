#ifndef PHYSICS_LOW_LEVEL_SHAPE_HEADER
#define PHYSICS_LOW_LEVEL_SHAPE_HEADER

#include <physics/low_level/object.h>

namespace physics
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class IShape : virtual public IObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual float Margin    () = 0;
    virtual void  SetMargin (float value) = 0;
};

}

}

#endif
