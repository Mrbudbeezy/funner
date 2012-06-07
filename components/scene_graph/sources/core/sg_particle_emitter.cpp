#include "shared.h"

using namespace scene_graph;

/*
    �������� ���������� ParticleEmitter
*/

struct ParticleEmitter::Impl: public xtl::instance_counter<ParticleEmitter>
{
  stl::string          declaration_name;                     //��� ������� ������
  Node*                particles_parent;                     //����, � ������� ��������� �������� ������������ �������
  xtl::auto_connection particles_parent_destroy_connection;  //���������� ��������� �������� �������� ����

  Impl (const char* in_declaration_name, Node::Pointer in_particles_parent)
    : declaration_name (in_declaration_name), particles_parent (in_particles_parent.get ())
  {
    if (!particles_parent)
      throw xtl::make_null_argument_exception ("scene_graph::ParticleEmitter::ParticleEmitter", "particles_parent");

    particles_parent_destroy_connection = particles_parent->RegisterEventHandler (NodeEvent_BeforeDestroy, xtl::bind (&ParticleEmitter::Impl::OnBaseNodeDestroy, this));
  }

  void OnBaseNodeDestroy ()
  {
    particles_parent = 0;
  }
};

/*
    ����������� / ����������
*/

ParticleEmitter::ParticleEmitter (const char* declaration_name, Node::Pointer particles_parent)
  : impl (new Impl (declaration_name, particles_parent))
  {}

ParticleEmitter::~ParticleEmitter ()
{
  delete impl;
}

/*
    �������� ��������
*/

ParticleEmitter::Pointer ParticleEmitter::Create (const char* declaration_name, Node::Pointer particles_parent)
{
  return Pointer (new ParticleEmitter (declaration_name, particles_parent), false);
}

/*
   ��� ������� ������
*/

const char* ParticleEmitter::DeclarationName () const
{
  return impl->declaration_name.c_str ();
}

/*
   ����, � ������� ��������� �������� ������������ �������
*/

Node::Pointer ParticleEmitter::ParticlesParent () const
{
  return impl->particles_parent;
}

/*
    �����, ���������� ��� ��������� �������
*/

void ParticleEmitter::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
