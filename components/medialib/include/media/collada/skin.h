#ifndef MEDIALIB_COLLADA_SKIN_HEADER
#define MEDIALIB_COLLADA_SKIN_HEADER

#include <media/collada/utility.h>
#include <math/mathlib.h>

namespace medialib
{

namespace collada
{

struct VertexJointWeight
{
  int   joint;
  float weight;
};

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
    const char*        JointName         (size_t joint);                      //��� ����������
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBaseMorph (Morph* base_morph);

          Morph* BaseMorph ();
    const Morph* BaseMorph () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �������� � ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t WeightsCount  () const;
    void   WeightsResize (size_t new_size);

          VertexJointWeight* Weights ();          
    const VertexJointWeight* Weights () const;

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
