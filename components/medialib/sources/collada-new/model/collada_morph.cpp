#include <media/collada/morph.h>
#include "collection.h"

using namespace medialib::collada;
using namespace common;

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

/*
    ���� ��������
*/

struct MorphTarget::Impl
{
  collada::Mesh& mesh;   //���
  float          weight; //���
  
  Impl (collada::Mesh& in_mesh) : mesh (in_mesh), weight (1.0f) {}
};

/*
    ����������� / ����������
*/

MorphTarget::MorphTarget (medialib::collada::Mesh& mesh)
  : impl (new Impl (mesh))
  {}

MorphTarget::~MorphTarget ()
{
  delete impl;
}

medialib::collada::Mesh& MorphTarget::Mesh ()
{
  return impl->mesh;
}

const medialib::collada::Mesh& MorphTarget::Mesh () const
{
  return impl->mesh;
}

/*
    ���
*/

void MorphTarget::SetWeight (float weight)
{
  impl->weight = weight;
}

float MorphTarget::Weight () const
{
  return impl->weight;
}

/*
    ������    
*/

//�������������� MorphTarget
class ConstructableMorphTarget: public MorphTarget
{
  public:
    ConstructableMorphTarget (medialib::collada::Mesh& mesh) : MorphTarget (mesh) {}
};

//������ MorphTarget
class MorphTargetListImpl: public Collection<MorphTarget, ConstructableMorphTarget, true>
{
  public:
    MorphTargetListImpl (Entity& owner) : Collection<MorphTarget, ConstructableMorphTarget, true> (owner) {}
  
    MorphTarget& Create (Mesh& mesh)
    {
      ConstructableMorphTarget* target = new ConstructableMorphTarget (mesh);

      try
      {
        InsertCore (*target);

        return *target;
      }
      catch (...)
      {
        delete target;
        throw;
      }
    }
};

struct Morph::Impl
{
  MorphMethod         method;    //����� ��������
  Mesh&               base_mesh; //������� ���
  MorphTargetListImpl targets;   //���� ��������  
  
  Impl (Entity& owner, Mesh& in_base_mesh) : method (MorphMethod_Normalized), base_mesh (in_base_mesh), targets (owner) {}
};

/*
    ����������� / ����������
*/

Morph::Morph (Mesh& base_mesh, ModelImpl* model, const char* id)
  : Entity (model, id),
    impl (new Impl (*this, base_mesh))
    {}

Morph::~Morph ()
{
  delete impl;
}

/*
    ����� ��������
*/

MorphMethod Morph::Method () const
{
  return impl->method;
}

void Morph::SetMethod (MorphMethod method)
{
  switch (method)
  {
    case MorphMethod_Normalized:
    case MorphMethod_Relative:
      break;
    default:
      RaiseInvalidArgument ("medialib::collada::Morph::SetMethod", "method", method);
      break;
  }
  
  impl->method = method;
}

/*
    ������� ���
*/

Mesh& Morph::BaseMesh ()
{
  return impl->base_mesh;
}

const Mesh& Morph::BaseMesh () const
{
  return impl->base_mesh;
}

/*
    ������ ����� ��������
*/

Morph::TargetList& Morph::Targets ()
{
  return impl->targets;
}

const Morph::TargetList& Morph::Targets () const
{
  return impl->targets;
}
