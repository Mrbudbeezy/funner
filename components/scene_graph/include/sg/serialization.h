#ifndef SCENE_GRAPH_SCENE_SERIALIZER_HEADER
#define SCENE_GRAPH_SCENE_SERIALIZER_HEADER

#include <xtl/functional_fwd>

#include <common/serializer_manager.h>

#include <sg/node.h>

namespace scene_graph
{

//forward declaration
class SceneManager;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef xtl::function<Node::Pointer ()> NodeCreator;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
typedef common::ResourceSerializerManager<
  NodeCreator (const char* file_name, SceneManager& manager),
  void (const char* file_name, const Node& node, SceneManager& manager)>
SceneSerializerManager;

}

#endif
