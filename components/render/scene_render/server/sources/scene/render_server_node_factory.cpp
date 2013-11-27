#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

using interchange::NodeType;

Node* NodeFactory::CreateNode (RenderManager& render_manager, NodeType type)
{
  try
  {
    switch (type)
    {
      case interchange::NodeType_Node:       return new Node;  
      case interchange::NodeType_StaticMesh: return new StaticMesh (render_manager);
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::NodeFactory::CreateNode");
    throw;
  }
}
