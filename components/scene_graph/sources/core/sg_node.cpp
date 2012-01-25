#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace common;

/*
    ��������������� ���������
*/

namespace
{

const char* LOG_NAME = "scene_graph.Node";

const bool DEFAULT_SCALE_PIVOT_ENABLED       = true; //�������� �� ��������� ��� ������������� ������ ���������������
const bool DEFAULT_ORIENTATION_PIVOT_ENABLED = true; //�������� �� ��������� ��� ������������� ������ ���������

///����� �������
struct Pivot
{
  bool  pivot_enabled;                          //������� �� �����
  bool  need_local_position_after_pivot_update; //���������� ��������� ��������� ��������� � ������ ������
  bool  orientation_pivot_enabled;              //������� ����� ���������
  bool  scale_pivot_enabled;                    //������� ����� ���������������
  vec3f pivot_position;                         //��������� ��������� ������
  vec3f local_position_after_pivot;             //��������� ��������� � ������ ������
  vec3f world_position_after_pivot;             //������� ��������� � ������ ������
  
  Pivot ()
    : pivot_enabled (false)
    , need_local_position_after_pivot_update (false)
    , orientation_pivot_enabled (DEFAULT_ORIENTATION_PIVOT_ENABLED)
    , scale_pivot_enabled (DEFAULT_SCALE_PIVOT_ENABLED)
  {
  }
};

/*
    ���������� �������� ��������������
*/

//���������� ��������������
void affine_compose (const vec3f& position, const quatf& orientation, const vec3f& scale, mat4f& tm)
{
  tm = math::translate (position) * to_matrix (orientation) * math::scale (scale);
}

//������������ �������� �������������� �� ������� ��������������
void affine_decompose (const math::mat4f& matrix, math::vec3f& position, math::quatf& rotation, math::vec3f& scale)
{
  math::mat4f local_matrix (transpose (matrix)); //����� ������� ��������������

  //��������� �������������� �������� �
  for (size_t i = 0; i < 3; i++)
  {
    position [i] = local_matrix [3][i];
    local_matrix [3][i] = 0;
  }

  //��������� �������������� ���������������
  for (size_t i = 0; i < 3; i++)
  {
    //����������� ����� �������-������� ��������������
    float square_length = 0;

    for (size_t j = 0; j < 3; j++)
      square_length += local_matrix [i][j] * local_matrix [i][j];

    scale [i] = sqrt (square_length);

    //������������
    for (size_t j = 0; j < 3; j++)
      local_matrix [i][j] /= scale [i];
  }
 
  math::vec3f temp_z = math::cross (math::vec3f (local_matrix [0][0], local_matrix [0][1], local_matrix [0][2]),
      math::vec3f (local_matrix [1][0], local_matrix [1][1], local_matrix [1][2]));

  if (math::dot (temp_z, math::vec3f (local_matrix [2][0], local_matrix [2][1], local_matrix [2][2])) < 0)
  {
    scale.x = -scale.x;
    
    for (size_t j=0; j<3; j++)
      local_matrix [0][j] = -local_matrix [0][j];
  }

//  rotation = to_quat (transpose (local_matrix));
  math::mat4f m = transpose (local_matrix);
  
  m [0] = normalize (m [0]);
  m [2] = normalize (cross (m [0], m [1]));
  m [1] = normalize (cross (m [2], m [0]));
  
  rotation = normalize (to_quat (m));
}

typedef stl::auto_ptr<Pivot>                                                  PivotPtr;
typedef xtl::signal<void (Node& sender, NodeEvent event)>                     NodeSignal;
typedef xtl::signal<void (Node& sender, Node& child, NodeSubTreeEvent event)> SubTreeNodeSignal;
typedef stl::auto_ptr<common::PropertyMap>                                    PropertyMapPtr;

}

/*
    �������� ���������� Node
*/

struct Node::Impl
{
  class NodeIterator;
  class ControllerIterator;

  scene_graph::Scene* scene;                            //�����, ������� ����������� ������
  stl::string         name;                             //��� ����
  size_t              name_hash;                        //��� �����
  size_t              ref_count;                        //���������� ������ �� ����
  Node*               this_node;                        //������� ����
  Node*               parent;                           //������������ ����
  Node*               first_child;                      //������ �������
  Node*               last_child;                       //��������� �������
  Node*               prev_child;                       //���������� �������
  Node*               next_child;                       //��������� �������
  bool                bind_lock;                        //���� ���������� �� ����� BindToParent
  bool                need_release_at_unbind;           //����� �� ��������� ������� ������ ���� ��� ������������ �� ��������
  NodeSignal          signals [NodeEvent_Num];          //�������
  bool                signal_process [NodeEvent_Num];   //����� ��������� ��������
  SubTreeNodeSignal   subtree_signals [NodeSubTreeEvent_Num]; //������� �������, ����������� � �����-��������
  bool                subtree_signal_process [NodeSubTreeEvent_Num]; //����� ��������� ��������, ����������� � ����� ��������
  PivotPtr            pivot;                            //����� �������
  vec3f               local_position;                   //��������� ���������
  quatf               local_orientation;                //��������� ����������
  vec3f               local_scale;                      //��������� �������
  mat4f               local_tm;                         //������� ��������� ��������������
  vec3f               world_position;                   //������� ���������
  quatf               world_orientation;                //������� ����������
  vec3f               world_scale;                      //������� �������
  mat4f               world_tm;                         //������� ������� ��������������
  bool                orientation_inherit;              //���� ������������ ������������ ����������
  bool                scale_inherit;                    //���� ������������ ������������� ��������
  bool                need_world_transform_update;      //����, ��������������� � ������������� ��������� ������� ��������������
  bool                need_world_tm_update;             //����, ��������������� � ������������� ��������� ������� ������� ��������������
  bool                need_local_tm_update;             //����, ��������������� � ������������� ��������� ������� ��������� ��������������
  bool                need_world_position_update;       //����, ��������������� � ������������� ��������� ������� �������
  bool                need_world_axises_update;         //����, ��������������� � ������������� ��������� ������� ���� � ��������
  size_t              update_lock;                      //������� �������� ���������� ����������
  bool                update_notify;                    //����, ��������������� � ������������� ���������� �� ����������� �� ���������� ���������� ����������
  Node*               first_updatable_child;            //������ ����������� �������
  Node*               last_updatable_child;             //��������� ����������� �������
  Node*               prev_updatable_child;             //���������� ����������� �������
  Node*               next_updatable_child;             //��������� ����������� �������
  ControllerEntry*    first_controller;                 //������ ���������� ������� ����
  ControllerEntry*    last_controller;                  //��������� ���������� ������� ����
  bool                has_updatable_controllers;        //���� �� ����������� �����������
  PropertyMapPtr      properties;                       //�������� ����
  NodeIterator*       first_node_iterator;              //������ �������� �������� ����
  ControllerIterator* first_controller_iterator;        //������ �������� �������� ������������
  
  ///�������� �����
  class NodeIterator: public xtl::noncopyable
  {
    public:
      ///�����������
      NodeIterator (Node* in_parent, Node* first_item, Node*& in_last_item, Node* Impl::* in_next_member)
        : item (first_item)
        , last_item (in_last_item)
        , parent (in_parent)
        , next_member (in_next_member)
        , next_iterator (parent->impl->first_node_iterator)
      {
        parent->impl->first_node_iterator = this;
      }
      
      ///����������
      ~NodeIterator ()
      {
        parent->impl->first_node_iterator = next_iterator;
      }

      ///��������� �������� �������� � ������������� � ����������
      Node::Pointer Next ()
      {
        Node::Pointer result = item;
        
        if (!item)
          return result;
        
        item = item->impl->*next_member;
        
        return result;
      }
      
      ///���������� ��������
      NodeIterator* NextIterator () { return next_iterator; }
      
      ///���������� �� �������� ����
      void OnRemove (Node* node)
      {
        if (node != item)
          return;

        Next ();
      }
      
      ///���������� � ���������� ����
      void OnAdd (Node* node)
      {
        if (item || node != last_item)
          return;
          
        item = node;
      }

    private:  
      Node*          item;
      Node*&         last_item;
      Node::Pointer  parent;
      Node* Impl::*  next_member;
      NodeIterator*  next_iterator;  
  };
  
  ///�������� �������� �����
  class NodeChildrenIterator: public NodeIterator
  {
    public:
      NodeChildrenIterator (Node* parent)
        : NodeIterator (parent, parent->impl->first_child, parent->impl->last_child, &Impl::next_child)
      {
      }
  };
  
  ///�������� ����������� �����
  class NodeUpdatablesIterator: public NodeIterator
  {
    public:
      NodeUpdatablesIterator (Node* parent)
        : NodeIterator (parent, parent->impl->first_updatable_child, parent->impl->last_updatable_child, &Impl::next_updatable_child)
      {
      }
  };

  ///�������� ������������
  class ControllerIterator: public xtl::noncopyable
  {
    public:
      ///�����������
      ControllerIterator (Node* in_node)
        : node (in_node)
        , item (in_node->impl->first_controller)
        , next_iterator (node->impl->first_controller_iterator)
      {
        node->impl->first_controller_iterator = this;
      }
      
      ///����������
      ~ControllerIterator ()
      {
        node->impl->first_controller_iterator = next_iterator;
      }

      ///��������� �������� �������� � ������������� � ����������
      ControllerEntry* Next ()
      {
        if (!item)
          return 0;
          
        ControllerEntry* result = item;
          
        item = item->next;
        
        return result;
      }
      
      ///���������� ��������
      ControllerIterator* NextIterator () { return next_iterator; }
      
      ///���������� �� �������� �����������
      void OnRemove (ControllerEntry* entry)
      {
        if (entry != item)
          return;

        Next ();
      }
      
      ///���������� � ���������� �����������
      void OnAdd (ControllerEntry* entry)
      {
        if (item || entry != node->impl->last_controller)
          return;
          
        item = entry;
      }

    private:  
      Node::Pointer       node;
      ControllerEntry*    item;
      ControllerIterator* next_iterator;
  };    
  
  Impl (Node* node) : this_node (node)
  {
    ref_count                 = 1;
    scene                     = 0;
    parent                    = 0;
    first_child               = 0;
    last_child                = 0;
    prev_child                = 0;
    next_child                = 0;
    update_lock               = 0;
    update_notify             = false;
    name_hash                 = strhash (name.c_str ());
    first_updatable_child     = 0;
    last_updatable_child      = 0;
    prev_updatable_child      = 0;
    next_updatable_child      = 0;
    first_controller          = 0;
    last_controller           = 0;
    first_node_iterator       = 0;
    first_controller_iterator = 0;

      //������� �� ���������

    local_scale = 1.0f;
    world_scale = 1.0f;

      //�� ��������� ���� ��������� ��� �������������� ��������

    orientation_inherit = true;
    scale_inherit       = true;

      //�������������� ����������

    need_world_transform_update = false;
    need_local_tm_update        = false;
    need_world_tm_update        = false;
    need_world_position_update  = false;
    need_world_axises_update    = false;
    
      //������� ������� ������ ��������� ��������
      
    for (size_t i=0; i<NodeEvent_Num; i++)
      signal_process [i] = false;
      
    for (size_t i=0; i<NodeSubTreeEvent_Num; i++)
      subtree_signal_process [i] = false;
      
      //������� ����� ���������� �� ����� BindToParent
      
    bind_lock = false;
    need_release_at_unbind = false;
  }
  
  void UpdateIteratorsBeforeRemoveChild (Node* child)
  {    
    for (NodeIterator* i=first_node_iterator; i; i=i->NextIterator ())    
      i->OnRemove (child);
  }
  
  void UpdateIteratorsAfterAddChild (Node* child)
  {
    for (NodeIterator* i=first_node_iterator; i; i=i->NextIterator ())
      i->OnAdd (child);
  }  
  
  void UpdateIteratorsBeforeRemoveController (ControllerEntry* entry)
  {    
    for (ControllerIterator* i=first_controller_iterator; i; i=i->NextIterator ())    
      i->OnRemove (entry);
  }
  
  void UpdateIteratorsAfterAddController (ControllerEntry* entry)
  {
    for (ControllerIterator* i=first_controller_iterator; i; i=i->NextIterator ())
      i->OnAdd (entry);
  }    
  
  Pivot& GetPivot ()
  {
    if (pivot)
      return *pivot;
      
    pivot = PivotPtr (new Pivot);
    
    return *pivot;
  }
  
  const vec3f& PositionAfterPivot ()
  {
    if (!pivot)
      return local_position;
    
    if (pivot->need_local_position_after_pivot_update)
    {
      if (pivot->pivot_enabled)
      {
        static vec3f default_scale (1.0f);
        static quatf default_orientation;

        const vec3f& pivot_scale       = pivot->scale_pivot_enabled ? default_scale : local_scale;
        const quatf& pivot_orientation = pivot->orientation_pivot_enabled ? default_orientation : local_orientation;

        pivot->local_position_after_pivot = local_position
          + pivot_orientation * (pivot_scale * pivot->pivot_position)
          - local_orientation * (local_scale * pivot->pivot_position);
      }
      else
      {
        pivot->local_position_after_pivot = local_position;
      }
      
      pivot->need_local_position_after_pivot_update = false;
    }
    
    return pivot->local_position_after_pivot;
  }
  
  const vec3f& WorldPositionAfterPivot ()
  {
    if (need_world_position_update)
      UpdateWorldPosition ();
      
    return pivot ? pivot->world_position_after_pivot : world_position;
  }
  
  //���������� �� ��������� ����
  void UpdateNotify ()
  {
      //���� ���� �� ��������� � ���������� ���������� - ��������� �������� �� ����������

    if (!update_lock)
    {
      Notify (NodeEvent_AfterUpdate);
      return;
    }

      //����� ������������� ����, ��������������� � ������������� ���������� �������� �� ���������� �� ���������� ���������
      //����������

    update_notify = true;    
  }

  void UpdateWorldTransformNotify ()
  {
    UpdateNotify ();

    if (need_world_transform_update)
      return;      

    need_world_transform_update = true;
    need_world_tm_update        = true;
    need_world_position_update  = true;
    need_world_axises_update    = true;
    
      //���������� ����������� ������� �� ��������� ��������� �������

    try
    {
      this_node->AfterUpdateWorldTransformEvent ();
    }
    catch (...)
    {
      //��������� ��� ����������
    }

      //���������� ���� �������� � ������������� ��������� ������� ��������������

    NodeChildrenIterator iter = this_node;

    while (Node::Pointer node = iter.Next ())
      node->impl->UpdateWorldTransformNotify ();
  }

/*
    ���������� � ������������� ��������� ������� �������������� / �������� ������� ��������������
*/

  void UpdateLocalTransformNotify ()
  {
    need_local_tm_update = true;
    
    if (pivot)
      pivot->need_local_position_after_pivot_update = true;
    
    UpdateWorldTransformNotify ();
  }
  
  void UpdateWorldPosition ()
  {
    UpdateWorldTransform ();    

    if (need_world_position_update)
    {
      for (size_t i = 0; i < 3; i++)
        world_position [i] = world_tm [i][3];
        
      need_world_position_update = false;        
    }    
  }
  
  void UpdateWorldAxises ()
  {
    UpdateWorldTransform ();    
    
    if (need_world_axises_update)
    {
      math::vec3f position;
    
      affine_decompose (world_tm, position, world_orientation, world_scale);
      
      need_world_axises_update = false;      
    }
  }
  
  void UpdateWorldMatrix ()
  {
    UpdateWorldTransform ();
    
    if (need_world_tm_update)
    {
      affine_compose (pivot ? pivot->world_position_after_pivot : world_position, world_orientation, world_scale, world_tm);
      need_world_tm_update = false;    
    }
  }
  
  void UpdateWorldTransform ()
  {
    if (!need_world_transform_update)
      return;
    
    bool pivot_enabled = pivot && pivot->pivot_enabled && (pivot->orientation_pivot_enabled || pivot->scale_pivot_enabled);
    
    if (parent)
    {
      world_tm = parent->WorldTM () * this_node->LocalTM ();
      
      if (!orientation_inherit || !scale_inherit || pivot)
      {
        affine_decompose (world_tm, pivot ? pivot->world_position_after_pivot : world_position, world_orientation, world_scale);        
        
        if (pivot)
        {
          world_position = parent->WorldTM () * local_position;
        }

        if (!orientation_inherit) world_orientation = local_orientation;
        if (!scale_inherit)       world_scale       = local_scale;

        if (pivot_enabled)
        {
          static vec3f default_scale (1.0f);
          static quatf default_orientation;

          const vec3f& pivot_scale       = pivot->scale_pivot_enabled ? scale_inherit ? parent->WorldScale () : default_scale : world_scale;
          const quatf& pivot_orientation = pivot->orientation_pivot_enabled ? orientation_inherit ? parent->WorldOrientation () : default_orientation : world_orientation;

          pivot->world_position_after_pivot = world_position + pivot_orientation * (pivot_scale * pivot->pivot_position) - world_orientation * (world_scale * pivot->pivot_position);
        }
        
        need_world_tm_update       = true;
        need_world_position_update = false;
        need_world_axises_update   = false;
      }
      else
      {
        need_world_tm_update       = false;
        need_world_position_update = true;
        need_world_axises_update   = true;
      }
    }
    else
    {
      world_orientation          = local_orientation;
      world_scale                = local_scale;      
      world_position             = local_position;
      need_world_tm_update       = true;
      need_world_axises_update   = false;
      need_world_position_update = false;

      if (pivot)
      {
        pivot->world_position_after_pivot = PositionAfterPivot ();                
      }
    }

    need_world_transform_update = false;
  }

  void BindToParentImpl (Node* parent_node, NodeBindMode mode, NodeTransformSpace invariant_space)
  {
      //������ �� ������ Bind � ������������ ��������������� �������

    if (bind_lock)
      return;

      //���������� ������������ ������������ ��������������
      
    bool  need_transform_in_world_space = false;
    vec3f old_world_position_after_pivot, old_world_scale;
    quatf old_world_orientation;

    switch (invariant_space)
    {
      case NodeTransformSpace_Parent:
      case NodeTransformSpace_Local:
        break;
      case NodeTransformSpace_World:
        need_transform_in_world_space  = true;
        old_world_position_after_pivot = WorldPositionAfterPivot ();
        old_world_scale                = this_node->WorldScale ();
        old_world_orientation          = this_node->WorldOrientation ();
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "invariant_space", invariant_space);
    }
    
      //��������� ������������ ������ �������������
      
    switch (mode)
    {
      case NodeBindMode_AddRef:
      case NodeBindMode_WeakRef:
        break;
      default:
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "mode", mode);
    }
    
      //���� �������� �� ���������� ��� ������������� � �����������

    if (parent_node == parent)
      return;
      
      //�������� ������� ������������ ��������� ���� �� �����

    if (!parent && scene)
      throw xtl::format_not_supported_exception ("scene_graph::Node::BindToParent", "Attempt to bind scene '%s' root to parent not supproted", scene->Name ());

      //�������� �� �������������� �� ���� � ������ �������
      
    for (Node* node = parent_node; node; node=node->impl->parent)
      if (node == this_node)
        throw xtl::make_argument_exception ("scene_graph::Node::BindToParent", "parent", "Attempt to bind object to one of it's child");
        
      //������������� ���������� �� ����� BindToParent
      
    bind_lock = true;
    
      //����������� ������� ������
      
    Pointer node_lock (this_node);

      //���� � ���� ��� ���� �������� ����������� ���
      
    if (parent)
    {
        //��������� �������� �� ������������ ���� �� ��������

      UnbindNotify ();
      
        //��������� ���������, ����������� �� ������ ����
        
      parent->impl->UpdateIteratorsBeforeRemoveChild (this_node);      

        //���������� ���� �� ��������
      
      if (prev_child) prev_child->impl->next_child = next_child;
      else            parent->impl->first_child    = next_child;
      
      if (next_child) next_child->impl->prev_child = prev_child;
      else            parent->impl->last_child     = prev_child;    
      
        //����������� ���� �� ������ ���������� ��������
        
      UnbindFromParentUpdateList ();
      
        //��������� ������� ������
      
      if (need_release_at_unbind)
      {
        need_release_at_unbind = false;
        this_node->Release ();
      }
    }

      //��������� ���� � ����� ���������

    parent = parent_node;

    if (parent_node)
    {
        //����������� ����� ������ ���� ����� ������� ����� �������������

      if (mode == NodeBindMode_AddRef)
      {
        this_node->AddRef ();
        
        need_release_at_unbind = true;
      }
      else
      {
        need_release_at_unbind = false;
      }

        //������������ ���� � ������ �������� ��������
        
      Impl* parent_impl = parent_node->impl;

      prev_child   = parent_impl->last_child;
      next_child   = 0;
      parent_impl->last_child = this_node;

      if (prev_child) prev_child->impl->next_child = this_node;
      else            parent_impl->first_child     = this_node;
      
        //��������� ������� �����
        
      SetScene (parent_impl->scene);

        //���������� � ������ ���������� ��������

      if (HasUpdatables ())
        BindToParentUpdateList ();
        
        //��������� ���������, ����������� �� ������ ����
        
      parent->impl->UpdateIteratorsAfterAddChild (this_node);        

        //���������� � ������������� ���� � ��������

      BindNotify ();
    }
    else
    {
      prev_child = next_child = 0;

        //��������� ������� �����

      SetScene (0);

        //���������� �� ����������

      this_node->BeginUpdate ();

        //������������ �������������� ������� ���������

      UpdateWorldTransformNotify ();

      this_node->EndUpdate ();      
    }

      //�������������� ������� ���������
      
    if (need_transform_in_world_space)
    {           
      this_node->BeginUpdate ();
      
      if (parent)
      {
        this_node->SetWorldOrientation (old_world_orientation);
        this_node->SetWorldScale       (old_world_scale);
        this_node->Translate           (old_world_position_after_pivot - WorldPositionAfterPivot (), NodeTransformSpace_World);
      }
      else
      {
        this_node->SetOrientation (old_world_orientation);
        this_node->SetScale       (old_world_scale);
        this_node->Translate      (old_world_position_after_pivot - WorldPositionAfterPivot (), NodeTransformSpace_World);
      }
      
      this_node->EndUpdate ();
    }

      //������ ���������� �� ����� BindToParent

    bind_lock = false;
  }

  //���������� � ������������� ���� � ��������
  void BindNotify ()
  {
      //���������� �� ����������
    
    this_node->BeginUpdate ();
      
      //������������ �������������� ������� ���������

    UpdateWorldTransformNotify ();
      
      //��������� �������� � ������������� ���� � ������ ��������

    Notify (NodeEvent_AfterBind);

    this_node->EndUpdate ();
    
      //���������� ������������� ����
      
    if (parent)
      parent->impl->BindChildNotify (*this_node);
  }

  //���������� �� ������������ ���� �� ��������
  void UnbindNotify ()
  {
    Notify (NodeEvent_BeforeUnbind);
    
    if (parent)
      parent->impl->UnbindChildNotify (*this_node);
  }

  //���������� � ������������� ������� ��� ���������� �������
  void BindChildNotify (Node& child)
  {
    Notify (child, NodeSubTreeEvent_AfterBind);
    
    if (parent)
      parent->impl->BindChildNotify (child);
  }

  //��������� �� ������������ ������� ��� ���������� �������
  void UnbindChildNotify (Node& child)
  {
    Notify (child, NodeSubTreeEvent_BeforeUnbind);
    
    if (parent)
      parent->impl->UnbindChildNotify (child);
  }

/*
    ��������� ��������� �� �����, � ������� ���������� ����
*/

  void SetScene (scene_graph::Scene* in_scene)
  {
      //���� ����� ��������� - ���������� �����

    if (scene == in_scene)
      return;
      
      //���������� �� ������������ �� �����
      
    if (scene)
    {          
      try
      {
        this_node->BeforeSceneDetachEvent ();
      }
      catch (...)
      {
        //���������� ��� ����������
      }
      
      Notify (NodeEvent_BeforeSceneDetach);
    }
    
      //��������� ��������� �� ����� �����

    scene = in_scene;
    
      //���������� ����� � ��������

    NodeChildrenIterator iter = this_node;      
    
    while (Node::Pointer node = iter.Next ())
      node->impl->SetScene (scene);

      //���������� � ����������� � ����� �����
      
    if (!in_scene)
    {
      Notify (NodeEvent_AfterSceneChange);
      return;
    }

    try
    {
      this_node->AfterSceneAttachEvent ();
    }
    catch (...)
    {
      //���������� ��� ����������
    }

    Notify (NodeEvent_AfterSceneAttach);    
    Notify (NodeEvent_AfterSceneChange);
  }

/*
    ���������� �������� � ����������� �������
*/

  void Notify (NodeEvent event)
  {
      //���� ������������ ��� - ���������� ������������
      
    if (!signals [event])
      return;

      //��������� ��� �� ������������ ������

    if (signal_process [event])
      return;
      
      //������������� ���� ��������� �������

    signal_process [event] = true;
    
      //�������������� ���������� �� �������� �� ����� ����������
      
    Pointer self_lock (this_node);
    
      //�������� ����������� �������

    try
    {     
      signals [event] (*this_node, event);
    }
    catch (...)
    {
      //��� ���������� ���������� ������������ ������� ���� �����������
    }
    
      //������� ���� ��������� �������
    
    signal_process [event] = false;
  }

  void Notify (Node& child, NodeSubTreeEvent event)
  {
    //���� ������������ ��� - ���������� ������������

    if (!subtree_signals [event])
      return;

      //��������� ��� �� ������������ ������

    if (subtree_signal_process [event])
      return;
      
      //��������� � ������������� ������� ������������ ���� �������� child

    NodeChildrenIterator iter = &child;
    
    while (Node::Pointer node = iter.Next ())
      Notify (*node, event);
      
      //������������� ���� ��������� �������

    subtree_signal_process [event] = true;
    
      //�������� ����������� �������

    try
    {
      subtree_signals [event] (*this_node, child, event);
    }
    catch (...)
    {
      //��� ���������� ���������� ������������ ������� ���� �����������
    }
    
      //������� ���� ��������� �������
    
    subtree_signal_process [event] = false;  
  }
  
/*
    ������ �� ������� ����������� �����
*/

  void BindToParentUpdateList ()
  {
    if (!parent)
      return;
      
    if (prev_updatable_child || next_updatable_child || parent->impl->first_updatable_child == this_node)
      return;

    if (parent->impl->first_updatable_child) //���� ������ ����������� ����� �� ����
    {
      next_updatable_child = 0;
      prev_updatable_child = parent->impl->last_updatable_child;
      
      prev_updatable_child->impl->next_updatable_child = this_node;
    }
    else
    {
        //���� ������ ����������� ����� ����

      parent->impl->first_updatable_child = this_node;
      
      prev_updatable_child = 0;
      next_updatable_child = 0;
      
        //����������� ����������� ��������

      parent->impl->BindToParentUpdateList ();
    }
    
    parent->impl->last_updatable_child = this_node;    
    
    parent->impl->UpdateIteratorsAfterAddChild (this_node);
  }
  
  void UnbindFromParentUpdateList ()
  {
    if (!parent)
      return;
      
    if (!prev_updatable_child && !next_updatable_child && parent->impl->first_updatable_child != this_node)
      return;
      
    parent->impl->UpdateIteratorsBeforeRemoveChild (this_node);
      
    if (prev_updatable_child)
    {
      prev_updatable_child->impl->next_updatable_child = next_updatable_child;
    }
    else if (this_node == parent->impl->first_updatable_child)
    {
      parent->impl->first_updatable_child = next_updatable_child;
    }

    if (next_updatable_child)
    { 
      next_updatable_child->impl->prev_updatable_child = prev_updatable_child;  
    }
    else if (this_node == parent->impl->last_updatable_child)
    {
      parent->impl->last_updatable_child = prev_updatable_child;
    }    

    prev_updatable_child = 0;
    next_updatable_child = 0;

      //����������� ������ ����������� ��������
          
    if (!parent->impl->first_updatable_child && !parent->impl->has_updatable_controllers)
      parent->impl->UnbindFromParentUpdateList ();
  }
  
  bool HasUpdatables () { return has_updatable_controllers || first_updatable_child; }

/*
    ���������� ��������� ����
*/

  void UpdateNode (const TimeValue& time)
  {
    static const char* METHOD_NAME = "scene_graph::Node::Impl::UpdateNode";

    if (!has_updatable_controllers)
      return;
    
    try
    {
      this_node->BeginUpdate ();

      ControllerIterator iter = this_node;
      
      while (ControllerEntry* entry = iter.Next ())
      {
        if (entry->updatable)
          entry->controller->UpdateState (time);
      }
      
      this_node->EndUpdate ();
    }
    catch (xtl::exception& e)
    {
      common::Log log (LOG_NAME);

      log.Printf ("'%s': node '%s' update exception '%s'", METHOD_NAME, name.c_str (), e.what ());

      //���������� ���� ����������
    }
    catch (...)
    {
      common::Log log (LOG_NAME);

      log.Printf ("'%s': node '%s' update unknown exception", METHOD_NAME, name.c_str ());

      //���������� ���� ����������
    }
  }
};

/*
    ����������� / ����������
*/

Node::Node ()
  : impl (new Impl (this)) 
{
}

Node::~Node ()
{
    //��������� ������������� �����

  UnbindAllChildren ();
  Unbind ();
  
    //������������ ���� ������������

  DetachAllControllers ();  
  
    //���������� �� �������� ����
    
  impl->Notify (NodeEvent_AfterDestroy);
  
    //������� ���������� ����

  delete impl;
}

/*
    �������� ����
*/

Node::Pointer Node::Create ()
{
  return Pointer (new Node, false);
}

/*
    �����, ������� ����������� ������
*/

Scene* Node::Scene ()
{
  return impl->scene;
}

const Scene* Node::Scene () const
{
  return impl->scene;
}

/*
    ��� ����
*/

const char* Node::Name () const
{
  return impl->name.c_str ();
}

void Node::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Node::SetName", "name");

  impl->name      = name;
  impl->name_hash = strhash (name);
  
  UpdateNotify ();
}

/*
    �������� ���� (����� ���� NULL)
*/

common::PropertyMap* Node::Properties ()
{
  return impl->properties.get ();
}

const common::PropertyMap* Node::Properties () const
{
  return impl->properties.get ();
}

void Node::SetProperties (common::PropertyMap* properties)
{
  if (properties)
  {
    if (impl->properties)
    {
      *impl->properties = *properties;
    }
    else
    {
      impl->properties.reset (new common::PropertyMap (*properties));
    }
  }
  else
  {
    impl->properties.reset ();
  }

  UpdateNotify ();
}

void Node::SetProperties (const common::PropertyMap& properties)
{
  SetProperties (&const_cast<common::PropertyMap&> (properties));
}

/*
    ������� ������
*/

void Node::AddRef () const
{
  if (impl->ref_count)
    impl->ref_count++;
}

void Node::Release () const
{
    //������ �� ���������� �������� � �����������

  if (impl->signal_process [NodeEvent_BeforeDestroy] || !impl->ref_count)
    return;

  if (!--impl->ref_count)
  {
      //��������� �������� �� �������� ����

    impl->Notify (NodeEvent_BeforeDestroy);

    delete this;
  }
}

size_t Node::UseCount () const
{
  return impl->ref_count;
}

/*
    ����� ��� ����������� �� ������� ������ ��������
*/

Node::Pointer Node::Parent ()
{
  return impl->parent;
}

Node::ConstPointer Node::Parent () const
{
  return impl->parent;
}

Node::Pointer Node::FirstChild ()
{
  return impl->first_child;
}

Node::ConstPointer Node::FirstChild () const
{
  return impl->first_child;
}

Node::Pointer Node::LastChild ()
{
  return impl->last_child;
}

Node::ConstPointer Node::LastChild () const
{
  return impl->last_child;
}

Node::Pointer Node::PrevChild ()
{
  return impl->prev_child;
}

Node::ConstPointer Node::PrevChild () const
{
  return impl->prev_child;
}

Node::Pointer Node::NextChild ()
{
  return impl->next_child;
}

Node::ConstPointer Node::NextChild () const
{
  return impl->next_child;
}

/*
    ������������� ���� � ��������
*/

void Node::Unbind (NodeTransformSpace invariant_space)
{
  impl->BindToParentImpl (0, NodeBindMode_WeakRef, invariant_space);
}

void Node::BindToParent (Node& parent, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  impl->BindToParentImpl (&parent, mode, invariant_space);
}

void Node::BindToScene (scene_graph::Scene& scene, NodeBindMode mode, NodeTransformSpace invariant_space)
{
  BindToParent (scene.Root (), mode, invariant_space);
}

void Node::UnbindChild (const char* name, NodeTransformSpace invariant_space)
{
  UnbindChild (name, NodeSearchMode_OnNextSublevel, invariant_space);
}

void Node::UnbindChild (const char* name, NodeSearchMode mode, NodeTransformSpace invariant_space)
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Node::UnbindChild", "name");

  Node::Pointer child = FindChild (name, mode);
  
  if (!child)
    return;

  child->Unbind (invariant_space);
}

void Node::UnbindAllChildren ()
{
  while (impl->last_child)
    impl->last_child->Unbind ();
}

/*
    ����� ������� �� �����
*/

Node::Pointer Node::FindChild (const char* name, NodeSearchMode mode) //no throw
{
  return xtl::const_pointer_cast<Node> (const_cast<const Node&> (*this).FindChild (name, mode));
}

Node::ConstPointer Node::FindChild (const char* name, NodeSearchMode mode) const //no throw
{
  if (!name)
    throw xtl::make_null_argument_exception ("scene_graph::Node::FindChild", "name");    
    
  size_t name_hash = strhash (name);
    
  switch (mode)
  {
    case NodeSearchMode_OnNextSublevel:
      for (const Node* node=impl->first_child; node; node=node->impl->next_child)
        if (node->impl->name_hash == name_hash && node->impl->name == name)
          return node;

      break;
    case NodeSearchMode_OnAllSublevels:
      for (const Node* node=impl->first_child; node; node=node->impl->next_child)
      {
        if (node->impl->name_hash == name_hash && name == node->impl->name)
          return node;

        Node::ConstPointer child = node->FindChild (name, mode);

        if (child)
          return child;
      }

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::FindChild", "mode", mode);
      break;
  }

  return 0;  
}

/*
    ��������� ���� � ������������ ���������������� �� ���� (���������� �������� Visitor)
*/

void Node::Accept (Visitor& visitor) const
{
  const_cast<Node&> (*this).AcceptCore (visitor);
}

void Node::AcceptCore (Visitor& visitor)
{
  visitor (*this);
}

/*
    ����� ��������
*/

void Node::Traverse (const TraverseFunction& fn, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      fn (const_cast<Node&> (*this));
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse(const TraverseFunction&)", "mode", mode);
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    Impl::NodeChildrenIterator iter = const_cast<Node*> (this); 
    
    while (Node::Pointer node = iter.Next ())
      node->Traverse (fn, mode);
  }
    
  if (mode == NodeTraverseMode_BottomToTop)
    fn (const_cast<Node&> (*this));
}

void Node::Traverse (INodeTraverser& traverser, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      traverser (const_cast<Node&> (*this));
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Traverse(INodeTraverser&)", "mode", mode);
      break;
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    Impl::NodeChildrenIterator iter = const_cast<Node*> (this);
    
    while (Node::Pointer node = iter.Next ())
      node->Traverse (traverser, mode);
  }

  if (mode == NodeTraverseMode_BottomToTop)
    traverser (const_cast<Node&> (*this));
}

void Node::VisitEach (Visitor& visitor, NodeTraverseMode mode) const
{
  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      const_cast<Node&> (*this).AcceptCore (visitor);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::VisitEach", "mode", mode);
      break;
  }

  if (mode != NodeTraverseMode_OnlyThis)
  {
    Impl::NodeChildrenIterator iter = const_cast<Node*> (this);
    
    while (Node::Pointer node = iter.Next ())
      node->VisitEach (visitor, mode);
  }

  if (mode == NodeTraverseMode_BottomToTop)
    const_cast<Node&> (*this).AcceptCore (visitor);
}

/*
    ��������� ����
*/

void Node::SetPosition (const vec3f& position)
{
  impl->local_position = position;

  impl->UpdateLocalTransformNotify ();
}

void Node::SetPosition  (float x, float y, float z)
{
  SetPosition (vec3f (x, y, z));
}

void Node::ResetPosition ()
{
  SetPosition (vec3f (0.0f));
}

void Node::SetWorldPosition (const vec3f& position)
{
  Translate (position - WorldPosition (), NodeTransformSpace_World);
}

void Node::SetWorldPosition (float x, float y, float z)
{
  SetWorldPosition (vec3f (x, y, z));
}

const vec3f& Node::Position () const
{
  return impl->local_position;
}

const vec3f& Node::WorldPosition () const
{
  if (impl->need_world_position_update)
    impl->UpdateWorldPosition ();
    
  return impl->world_position;
}

/*
    ����� ���� � ��������� ������� ��������� (����� ���������� ��������� � ��������)
*/

void Node::SetPivotPosition (const math::vec3f& pivot_position)
{
  static float       EPS = 0.001f;
  static math::vec3f ZERO_VEC (0.0f);

  if (equal (pivot_position, ZERO_VEC, EPS))
  {
    ResetPivotPosition ();
  }
  else
  {
    Pivot& pivot = impl->GetPivot ();
    
    pivot.pivot_position = pivot_position;
    pivot.pivot_enabled  = true;
    
    impl->UpdateLocalTransformNotify ();
  }
}

void Node::SetPivotPosition (float x, float y, float z)
{
  SetPivotPosition (math::vec3f (x, y, z));
}

void Node::ResetPivotPosition ()
{
  if (!impl->pivot)
    return;

  Pivot& pivot = impl->GetPivot ();

  pivot.pivot_enabled  = false;
  pivot.pivot_position = math::vec3f (0.0f);
  
  impl->UpdateLocalTransformNotify ();
}

const math::vec3f& Node::PivotPosition () const
{
  static vec3f ZERO_VEC (0.0f);

  return impl->pivot ? impl->pivot->pivot_position : ZERO_VEC;
}

//��������� ����� ���������� ������ ���������
void Node::SetOrientationPivotEnabled (bool state)
{
  if (!impl->pivot && state == DEFAULT_ORIENTATION_PIVOT_ENABLED)
    return;
    
  Pivot& pivot = impl->GetPivot ();
  
  pivot.orientation_pivot_enabled = state;
  
  impl->UpdateLocalTransformNotify ();
}

//��������� ����� ���������� ������ ��������
void Node::SetScalePivotEnabled (bool state)
{
  if (!impl->pivot && state == DEFAULT_SCALE_PIVOT_ENABLED)
    return;
    
  Pivot& pivot = impl->GetPivot ();
  
  pivot.scale_pivot_enabled = state;
  
  impl->UpdateLocalTransformNotify ();
}

bool Node::OrientationPivotEnabled () const
{
  return impl->pivot ? impl->pivot->orientation_pivot_enabled : DEFAULT_ORIENTATION_PIVOT_ENABLED;
}

bool Node::ScalePivotEnabled () const
{
  return impl->pivot ? impl->pivot->scale_pivot_enabled : DEFAULT_SCALE_PIVOT_ENABLED;
}

//��������: ������������ �� pivot � ������ ����
bool Node::PivotEnabled () const
{
  return impl->pivot && impl->pivot->pivot_enabled && (impl->pivot->orientation_pivot_enabled || impl->pivot->scale_pivot_enabled);
}

/*
    ���������� ����
*/

void Node::SetOrientation (const quatf& orientation)
{
  impl->local_orientation = orientation;
  
  impl->UpdateLocalTransformNotify ();
}

void Node::SetOrientation (const math::anglef& angle, float axis_x, float axis_y, float axis_z)
{
  SetOrientation (to_quat (angle, vec3f (axis_x, axis_y, axis_z)));
}

void Node::SetOrientation (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll)
{
  SetOrientation (to_quat (pitch, yaw, roll));
}

void Node::SetWorldOrientation (const quatf& orientation)
{
//  if (impl->orientation_inherit)
//  {
    Rotate (orientation * inverse (WorldOrientation ()), NodeTransformSpace_World);
//  }
//  else
//  {
//    SetOrientation (orientation);
//  }
}

void Node::SetWorldOrientation (const math::anglef& angle, float axis_x, float axis_y, float axis_z)
{
  SetWorldOrientation (to_quat (angle, vec3f (axis_x, axis_y, axis_z)));
}

void Node::SetWorldOrientation (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll)
{
  SetWorldOrientation (to_quat (pitch, yaw, roll));
}

void Node::ResetOrientation ()
{
  SetOrientation (quatf ());
}

const quatf& Node::Orientation () const
{
  return impl->local_orientation;
}

const quatf& Node::WorldOrientation () const
{
  if (impl->need_world_axises_update)
    impl->UpdateWorldAxises ();
    
  return impl->world_orientation;
}

/*
    ���������������� ����
*/

void Node::LookTo (const vec3f& target_point, const vec3f& up, NodeTransformSpace space)
{
  math::vec3f x, y, z;

  switch (space)
  {
    case NodeTransformSpace_Local:
      z = normalize (target_point);
      y = normalize (up);

      break;
    case NodeTransformSpace_Parent:
    {
      quatf q = inverse (normalize (Orientation ()));

      z = normalize (q * vec4f (target_point - Position ()));
      y = normalize (q * vec4f (up, 0));

      break;
    }
    case NodeTransformSpace_World:
    {            
      quatf q = inverse (normalize (WorldOrientation ()));

      z = normalize (q * vec4f (target_point - WorldPosition ()));
      y = normalize (q * vec4f (up, 0));

      break;
    }
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::LookTo(const vec3f&, const vec3f&, NodeTransformSpace)", "space", space);
  }
  
  static const float EPS = 0.001f;
  
  if (qlen (y) < EPS || qlen (z) < EPS || equal (y, z, EPS))
    return; //������������� ���������������� �� ������ ��������� ������� ���������

  mat3f view;

  x = cross (y, z);
  y = cross (z, x);

  view [0] = x;
  view [1] = y;
  view [2] = z;
  view     = transpose (view);  
  
  quatf rotation = normalize (to_quat (view));  

  SetOrientation (rotation * impl->local_orientation);
}

void Node::LookTo (const math::vec3f& target_point, NodeOrt direction, NodeOrt invariant, NodeTransformSpace space)
{
  static const char* METHOD_NAME = "scene_graph::Node::LookTo(const vec3f&, NodeOrt, NodeOrt, NodeTransformSpace)";
  
    //���������� ������� ����� � ��������� ������� ���������

  vec3f local_dir;

  switch (space)
  {
    case NodeTransformSpace_Local:
      local_dir = normalize (target_point);

      break;
    case NodeTransformSpace_Parent:
    {
      quatf q = inverse (normalize (Orientation ()));

      local_dir = normalize (q * vec4f (target_point - Position ()));

      break;
    }
    case NodeTransformSpace_World:
    {            
      quatf q = inverse (normalize (WorldOrientation ()));      

      local_dir = normalize (q * vec4f (target_point - WorldPosition ()));

      break;
    }
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "space", space);
  }

    //�������� ������������ ����
  
  switch (direction)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      break;    
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "direction", direction);
  }
  
  switch (invariant)
  {
    case NodeOrt_X:
    case NodeOrt_Y:
    case NodeOrt_Z:
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "invariant", invariant);
  }
  
  if (invariant == direction)
    throw xtl::format_operation_exception (METHOD_NAME, "Bad configuration invariant = direction");
    
    //���������������
    
  switch (direction)
  {
    case NodeOrt_X:
      switch (invariant)
      {
        case NodeOrt_Y:
          LookTo (cross (local_dir, vec3f (0, 1, 0)), vec3f (0, 1, 0), NodeTransformSpace_Local);
          break;
        case NodeOrt_Z:
        {
          LookTo (vec3f (0, 0, 1), cross (vec3f (0, 0, 1), local_dir), NodeTransformSpace_Local);
          break;
        }
        default:
          break;
      }

      break;
    case NodeOrt_Y:
      switch (invariant)
      {
        case NodeOrt_X:
        {
          vec3f x (1, 0, 0),
                z = cross (x, local_dir),
                y = cross (z, x);
          
          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        case NodeOrt_Z:
          LookTo (vec3f (0, 0, 1), local_dir, NodeTransformSpace_Local);
          break;
        default:
          break;
      }

      break;
    case NodeOrt_Z:
      switch (invariant)
      {
        case NodeOrt_X:
        {
          vec3f x (1, 0, 0),
                y = cross (local_dir, x),
                z = cross (x, y);

          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        case NodeOrt_Y:
        {
          vec3f y (0, 1, 0),
                x = cross (y, local_dir),
                z = cross (x, y);
          
          LookTo (z, y, NodeTransformSpace_Local);
          break;
        }
        default:
          break;
      }
      
      break;
    default:
      break;
  }
}

void Node::LookTo (const math::vec3f& target_point, NodeTransformSpace space)
{
  LookTo (target_point, NodeOrt_Z, NodeOrt_Y, space);
}

/*
    ������� ����
*/

void Node::SetScale (const vec3f& scale)
{
  impl->local_scale = scale;
  
  impl->UpdateLocalTransformNotify ();
}

void Node::SetScale (float sx, float sy, float sz)
{
  SetScale (vec3f (sx, sy, sz));
}

void Node::SetWorldScale (const vec3f& scale)
{
//  if (impl->scale_inherit)
//  {
    Scale (scale / WorldScale ());
//  }
//  else
//  {
//    SetScale (scale);
//  }
}

void Node::SetWorldScale (float sx, float sy, float sz)
{
  SetWorldScale (vec3f (sx, sy, sz));
}

void Node::ResetScale ()
{
  SetScale (vec3f (1.0f));
}

const vec3f& Node::Scale () const
{
  return impl->local_scale;
}

const vec3f& Node::WorldScale () const
{
  if (impl->need_world_axises_update)
    impl->UpdateWorldAxises ();
    
  return impl->world_scale;
}

/*
    ���������� ������������� ��������������
*/

//��������� ����� ������������ ������������ ����������
void Node::SetOrientationInherit (bool state)
{
  if (state == impl->orientation_inherit)
    return;

  impl->orientation_inherit = state;

  impl->UpdateWorldTransformNotify ();
}

//����������� �� ������������ ����������
bool Node::IsOrientationInherited () const
{
  return impl->orientation_inherit;
}

//��������� ����� ������������ ������������� ��������
void Node::SetScaleInherit (bool state)
{
  if (state == impl->scale_inherit)
    return;

  impl->scale_inherit = state;

  impl->UpdateWorldTransformNotify ();
}

//����������� �� ������������ �������
bool Node::IsScaleInherited () const
{
  return impl->scale_inherit;
}

/*
    ������������ ��������������
*/

void Node::Translate (const math::vec3f& offset, NodeTransformSpace space)
{
  switch (space) 
  {
    case NodeTransformSpace_Local:
      impl->local_position += impl->local_orientation * offset;
      break;
    case NodeTransformSpace_Parent:
      impl->local_position += offset;
      break;
    case NodeTransformSpace_World:   
      if (impl->parent) impl->local_position += inverse (impl->parent->WorldOrientation ()) * offset / impl->parent->WorldScale ();
      else              impl->local_position += offset;

      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Translate", "space", space);
  }
  
  impl->UpdateLocalTransformNotify ();
}

void Node::Translate (float offset_x, float offset_y, float offset_z, NodeTransformSpace space)
{
  Translate (vec3f (offset_x, offset_y, offset_z), space);
}

void Node::Rotate (const math::quatf& q, NodeTransformSpace space)
{
  switch (space)
  {
    case NodeTransformSpace_Local:
      impl->local_orientation *= q;
      break;    
    case NodeTransformSpace_Parent:
      impl->local_orientation = q * impl->local_orientation;
      break;
    case NodeTransformSpace_World:
    {
      const quatf& world_orientation = WorldOrientation ();

      impl->local_orientation = impl->local_orientation * inverse (world_orientation) * q * world_orientation;
      break;
    }      
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Rotate", "space", space);
      break;
  }

  impl->UpdateLocalTransformNotify ();
}

void Node::Rotate (const math::anglef& angle, float axis_x, float axis_y, float axis_z, NodeTransformSpace space)
{
  Rotate (to_quat (angle, vec3f (axis_x, axis_y, axis_z)), space);
}

void Node::Rotate (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll, NodeTransformSpace space)
{
  Rotate (to_quat (pitch, yaw, roll), space);
}

void Node::Scale (const math::vec3f& scale)
{
  impl->local_scale *= scale;  
  
  impl->UpdateLocalTransformNotify ();
}

void Node::Scale (float sx, float sy, float sz)
{
  Scale (vec3f (sx, sy, sz));
}

/*
    ��������� ������ �������������� ����
*/

const mat4f& Node::TransformationMatrix (NodeTransformSpace space) const
{
  static mat4f idNode;

  switch (space)
  {
    case NodeTransformSpace_Local:
      if (impl->need_local_tm_update)
      {
        affine_compose (impl->PositionAfterPivot (), impl->local_orientation, impl->local_scale, impl->local_tm);

        impl->need_local_tm_update = false;
      }

      return impl->local_tm;
    case NodeTransformSpace_Parent:
      return impl->parent ? impl->parent->TransformationMatrix (NodeTransformSpace_World) : idNode;
    case NodeTransformSpace_World:
      if (impl->need_world_tm_update)
        impl->UpdateWorldMatrix ();

      return impl->world_tm;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::TransformationMatrix", "space", space);
  } 
}

/*
    ��������� ����
*/

vec3f Node::Ort (NodeOrt ort, NodeTransformSpace space) const
{
  static const char* METHOD_NAME = "scene_graph::Node::Ort";

    //��������� ������� ��������������

  const mat4f* tm;
  
  switch (space)
  {
    case NodeTransformSpace_Local:
      tm = &LocalTM ();
      break;
    case NodeTransformSpace_World:
      tm = &WorldTM ();
      break;
    case NodeTransformSpace_Parent:
      tm = &ParentTM ();
      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "space", space);
  }
  
    //��������� ���
    
  vec4f local_ort;

  switch (ort)
  {
    case NodeOrt_X: local_ort = vec4f (1, 0, 0, 0); break;
    case NodeOrt_Y: local_ort = vec4f (0, 1, 0, 0); break;
    case NodeOrt_Z: local_ort = vec4f (0, 0, 1, 0); break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "ort", ort);
  }  
  
  return normalize (vec3f (*tm * local_ort));
}

/*
    ��������� ������� �������������� ���� object � ������� ��������� ������� ����
*/

mat4f Node::ObjectTM (Node& object) const
{
  return inverse (WorldTM ()) * object.WorldTM ();
}

/*
    �������� �� ������� Node
*/

xtl::connection Node::RegisterEventHandler (NodeEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= NodeEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::Node::Event(NodeEvent)", "event", event);

  return impl->signals [event].connect (handler);
}

xtl::connection Node::RegisterEventHandler (NodeSubTreeEvent event, const SubTreeEventHandler& handler) const
{
  if (event < 0 || event >= NodeSubTreeEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::Node::Event(NodeSubTreeEvent)", "event", event);

  return impl->subtree_signals [event].connect (handler);
}

/*
    ���������� ������������ ����������
*/

void Node::BeginUpdate ()
{
  impl->update_lock++;
}

void Node::EndUpdate ()
{
  if (!impl->update_lock)
    throw xtl::format_not_supported_exception ("scene_graph::Node::EndUpdate", "Attempt to call EndUpdate without previous BeginUpdate call");  
    
  if (!--impl->update_lock)
  {
      //���� �� ����� ���������� ���������� ���� ��������� - ��������� �������� �� ����������
    
    if (impl->update_notify)
    {
      impl->Notify (NodeEvent_AfterUpdate);
      
      impl->update_notify = false;
    }
  }
}

bool Node::IsInUpdateTransaction () const
{
  return impl->update_lock > 0;  
}

//���������� �� ��������� ����
void Node::UpdateNotify () const
{
  impl->UpdateNotify ();
}

void Node::SetScene (scene_graph::Scene* in_scene)
{
  impl->SetScene (in_scene);
}

/*
    ������������� / ������������ �����������
*/

xtl::com_ptr<controllers::DefaultController> Node::AttachController (const UpdateHandler& updater)
{
  return controllers::DefaultController::Create (*this, updater);
}

void Node::AttachController (ControllerEntry& entry)
{
  if (!entry.controller)
    throw xtl::format_operation_exception ("scene_graph::Node::AttachController", "entry.controller");

  entry.next            = 0;
  entry.prev            = impl->last_controller;
  impl->last_controller = &entry;
 
  if (entry.prev) entry.prev->next       = &entry;
  else            impl->first_controller = &entry;
  
  if (entry.updatable)
  {
    impl->has_updatable_controllers = true;

    impl->BindToParentUpdateList ();
  }
  
  impl->UpdateIteratorsAfterAddController (&entry);
}

void Node::DetachController (ControllerEntry& entry)
{
  if (!entry.prev && !entry.next && &entry != impl->first_controller)
    return;

  impl->UpdateIteratorsBeforeRemoveController (&entry);

  if (entry.prev)                            entry.prev->next       = entry.next;
  else if (&entry == impl->first_controller) impl->first_controller = entry.next;
  
  if (entry.next)                           entry.next->prev      = entry.prev;
  else if (&entry == impl->last_controller) impl->last_controller = entry.prev;
  
  entry.prev = entry.next = 0;

  if (entry.updatable)
    UpdateControllerList ();
}

void Node::UpdateControllerList ()
{
  impl->has_updatable_controllers = false;
  
  for (ControllerEntry* entry=impl->first_controller; entry; entry=entry->next)
    if (entry->updatable)
    {
      impl->has_updatable_controllers = true;
      break;
    }

  if (!impl->HasUpdatables ()) //���� ��� ����������� �������� � ������������ - ������������ �� ������������� ������ ����������
    impl->UnbindFromParentUpdateList ();
}

void Node::DetachAllControllers ()
{
  while (impl->first_controller)
    impl->first_controller->controller->Detach ();
}

/*
   ������� ������������
*/

xtl::com_ptr<Controller> Node::FirstController ()
{
  if (!impl->first_controller)
    return xtl::com_ptr<Controller> ();

  return impl->first_controller->controller;
}

xtl::com_ptr<Controller> Node::LastController ()
{
  if (!impl->last_controller)
    return xtl::com_ptr<Controller> ();

  return impl->last_controller->controller;
}

xtl::com_ptr<const Controller> Node::FirstController () const
{
  return const_cast<Node&> (*this).FirstController ();
}

xtl::com_ptr<const Controller> Node::LastController () const
{
  return const_cast<Node&> (*this).LastController ();
}

/*
    ���������� ��������� ���� � ��� ��������
*/

void Node::Update (const TimeValue& time, NodeTraverseMode mode)
{  
    //�������� ������������ ����������

  Pointer lock (this);

  switch (mode)
  {
    case NodeTraverseMode_BottomToTop:
      break;
    case NodeTraverseMode_OnlyThis:
    case NodeTraverseMode_TopToBottom:
      impl->UpdateNode (time);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::Node::Update", "mode", mode);
  }
  
  if (mode != NodeTraverseMode_OnlyThis && impl->first_updatable_child)
  {
    Impl::NodeUpdatablesIterator iter = this;    
    
    while (Node::Pointer node = iter.Next ())
      node->Update (time, mode);
  }
  
  if (mode == NodeTraverseMode_BottomToTop)
  {
    impl->UpdateNode (time);
  }

  if (!impl->HasUpdatables ())
    impl->UnbindFromParentUpdateList ();
}
