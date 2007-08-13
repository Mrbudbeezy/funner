#ifndef MEDIALIB_COLLADA_SKIN_HEADER
#define MEDIALIB_COLLADA_SKIN_HEADER

#include <media/collada/utility.h>
#include <math/mathlib.h>

namespace medialib
{

namespace collada
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Skin: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& BindShapeMatrix    () const;
    void               SetBindShapeMatrix (const math::mat4f&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t             JointsCount       () const;                            //���������� ����������
    size_t             CreateJoint       (const char* name);                  //�������� ����������
    void               RemoveJoint       (size_t joint);                      //�������� ����������
    void               RemoveAllJoints   ();                                  //�������� ���� ����������
    void               SetJointInvMatrix (size_t joint, const math::mat4f& invTM); //��������� �������� ������� ����������
    const math::mat4f& JointInvMatrix    (size_t joint) const;                //��������� �������� ������� ����������
    int                FindJoint         (const char* name) const;            //���������� ����� ���������� �� ����� ��� -1 � ������ �������

  protected:
    Skin  (ModelImpl*, const char* id);
    ~Skin ();

  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
