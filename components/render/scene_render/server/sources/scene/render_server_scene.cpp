#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    �������� ���������� �����
*/

struct Scene::Impl: public xtl::reference_counter
{
  stl::string name; //��� �����
};

/*
    ������������ / ���������� / ������������
*/

Scene::Scene ()
  : impl (new Impl)
{
}

Scene::Scene (const Scene& scene)
  : impl (scene.impl)
{
  addref (impl);
}

Scene::~Scene ()
{
  release (impl);
}

Scene& Scene::operator = (const Scene& scene)
{
  Scene tmp (scene);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    ��� �����
*/

void Scene::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::scene::server::Scene::SetName", "name");

  impl->name = name;
}

const char* Scene::Name () const
{
  return impl->name.c_str ();
}
