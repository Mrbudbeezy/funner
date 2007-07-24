#ifndef MEDIALIB_COLLADA_CONTROLLER_HEADER
#define MEDIALIB_COLLADA_CONTROLLER_HEADER

#include <media/collada/utility.h>
#include <math/mathlib.h>

namespace medialib
{

namespace collada
{

//forward declarations
class Skin;
class Morph;
class Controller;
class Mesh;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������a ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef ILibrary<Skin>       SkinLibrary;
typedef ILibrary<Morph>      MorphLibrary;
typedef ILibrary<Controller> ControllerLibrary;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ControllerType
{
  ControllerType_Skin,  //���� ����������
  ControllerType_Morph  //���� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum MorphMethod
{
  MorphMethod_Normalized, //(Target1, Target2, ...)*(w1, w2, ...) = (1-w1-w2-...)*BaseMesh + w1*Target1 + w2*Target2 + ...
  MorphMethod_Relative    //(Target1, Target2, ...) + (w1, w2, ...) = BaseMesh + w1*Target1 + w2*Target2 + ...
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct JointWeight
{
  size_t joint;  //����� ����������
  float  weight; //��� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� � ����������� ����������� �� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  size_t first_weight;  //������ ������� ���� ���������� ������������ �� �������
  size_t weights_count; //���������� ����� ���������� ����������� �� �������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
class Skin: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    SkinLibrary& Library () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////  
    const collada::Mesh& Mesh () const;
          collada::Mesh& Mesh ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������ 
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& BindShapeMatrix () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t             JointsCount    () const;
    const char*        JointName      (size_t joint_index) const;
    const math::mat4f& JointInvMatrix (size_t joint_index) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                 InfluencesCount () const; //��������� � ����������� ������ � ����
    size_t                 WeightsCount    () const; 
    const JointWeight*     Weights         () const;
    const VertexInfluence* Influences      () const;
    
  protected:
    Skin  (SkinLibrary& library, const char* id);
    ~Skin ();
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Morph: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MorphLibrary& Library () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���
///////////////////////////////////////////////////////////////////////////////////////////////////  
    const collada::Mesh& Mesh () const;
          collada::Mesh& Mesh ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MorphMethod Method () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t               TargetsCount () const;
    float                TargetWeight (size_t index) const;
    const collada::Mesh& TargetMesh   (size_t index) const;
          collada::Mesh& TargetMesh   (size_t index);

  protected:
    Morph  (MorphLibrary& library, const char* id);
    ~Morph ();
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Controller: public Entity
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    ControllerLibrary& Library () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    collada::ControllerType Type () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� / ���� � ����������� �� ���� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const collada::Morph& Morph () const;
          collada::Morph& Morph ();
    const collada::Skin&  Skin  () const;
          collada::Skin&  Skin  ();
    
  protected:
    Controller  (collada::ControllerType type, collada::Morph& morph, collada::Skin&  skin, ControllerLibrary& library, const char* id);
    ~Controller ();
    
  private:
    struct Impl;
    Impl* impl;
};

}

}

#endif
