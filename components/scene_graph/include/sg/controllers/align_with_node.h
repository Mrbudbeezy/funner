#ifndef SCENE_GRAPH_CONTROLLERS_ALIGN_WITH_NODE_HEADER
#define SCENE_GRAPH_CONTROLLERS_ALIGN_WITH_NODE_HEADER

#include <sg/controller.h>

namespace scene_graph
{

namespace controllers
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ���������� ������ ���� �� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class AlignWithNode: public Controller
{
  public:
    typedef xtl::com_ptr<AlignWithNode>       Pointer;
    typedef xtl::com_ptr<const AlignWithNode> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create (Node& node);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������/��������� ������� �������� �������� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<const math::vec3f& (const math::vec3f& current_velocity, const math::vec3f& distance, float dt)> AccelerationFunction;

    void                        SetAccelerationHandler (const AccelerationFunction& acceleration_function);
    const AccelerationFunction& AccelerationHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ (��� target_node_axis � ������� ��������� target_node, node_axis � �������
///��������� ���� �����������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Start (Node::ConstPointer target_node, NodeOrt target_node_axis, NodeOrt node_axis, NodeOrt node_rotation_axis);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Stop ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    AlignWithNode  (Node& node);
    ~AlignWithNode ();
    
  private:
    void Update (float dt);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

#endif
