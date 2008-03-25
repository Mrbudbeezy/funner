#ifndef SCENE_GRAPH_NODE_HEADER
#define SCENE_GRAPH_NODE_HEADER

#include <mathlib.h>
#include <xtl/functional_fwd>
#include <xtl/dynamic_cast_root.h>
#include <xtl/intrusive_ptr.h>

namespace scene_graph
{

//forward declarations
class Scene;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeBindMode
{
  NodeBindMode_AddRef,  //��� ������������� ���������� ������ �� �������� ������������� �� 1
  NodeBindMode_Capture, //��� ������������� ���������� ������ �� �������� �� �������������
  
  NodeBindMode_Default = NodeBindMode_AddRef
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeSearchMode
{
  NodeSearchMode_OnNextSublevel, //����� ������� ������ �� ��������� ���������
  NodeSearchMode_OnAllSublevels, //����� ������� �� ���� ����������
  
  NodeSearchMode_Default = NodeSearchMode_OnAllSublevels
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ �������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeTransformSpace
{
  NodeTransformSpace_Local,  //�������������� � ��������� ������� ��������� ����
  NodeTransformSpace_Parent, //�������������� � ������� ��������� ������������� ����
  NodeTransformSpace_World   //�������������� � ������� ������� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeTraverseMode
{
  NodeTraverseMode_TopToBottom, //����� ����� �� ��������� � ��������
  NodeTraverseMode_BottomToTop, //����� ����� �� �������� � ���������
  
  NodeTraverseMode_Default = NodeTraverseMode_TopToBottom
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeEvent
{
  NodeEvent_AfterUpdate,       //����������� ����� ���������� ��������� ����
  NodeEvent_BeforeDestroy,     //����������� ����� ��������� ����
  NodeEvent_AfterDestroy,      //����������� ����� �������� ����
  NodeEvent_AfterBind,         //����������� ����� ������������� ���� � ��������
  NodeEvent_BeforeUnbind,      //����������� ����� ������������� ���� �� ��������
  NodeEvent_AfterSceneAttach,  //����������� ����� ������������� ������� � �����
  NodeEvent_BeforeSceneDetach, //����������� ����� ������������� ������� �� �����

  NodeEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �����-��������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeSubTreeEvent
{
  NodeSubTreeEvent_AfterBind,    //����������� ����� ������������� � ���� ������� ��� ���������� �������
  NodeSubTreeEvent_BeforeUnbind, //����������� ����� ������������� ������� �� ���� ������� ��� ���������� �������
  
  NodeSubTreeEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
  //�������� NodeIterator / NodeConstIterator
  //�������� ���������� �� �������� �������� ��� ����������� (������ ����� � Notify)!!!
class Node: public xtl::dynamic_cast_root
{
  friend class Scene;
  public: 
    typedef xtl::com_ptr <Node> Pointer;
    typedef xtl::com_ptr <const Node> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////  
    static Pointer Create ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ������� ����������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
          scene_graph::Scene* Scene ();
    const scene_graph::Scene* Scene () const;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Name    () const;
    void        SetName (const char* new_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   AddRef   () const;   //��������� ����� ������ �� ���� �� 1
    void   Release  () const;   //��������� ����� ������ �� ���� �� 1
    size_t UseCount () const;   //���������� �������� ������ �� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ����������� �� ������� ������ ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer      Parent     ();
    ConstPointer Parent     () const;
    Pointer      FirstChild ();
    ConstPointer FirstChild () const;
    Pointer      LastChild  ();
    ConstPointer LastChild  () const;
    Pointer      PrevChild  ();
    ConstPointer PrevChild  () const;
    Pointer      NextChild  ();
    ConstPointer NextChild  () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
      //��������� ����������
    void BindToParent (Node&              parent,                                      //������������ ����
                       NodeBindMode       mode = NodeBindMode_Default,                 //����� �������������
                       NodeTransformSpace invariant_space = NodeTransformSpace_Local); //������������ ������������ ��������������

      //������������� � �����
    void BindToScene (scene_graph::Scene& scene,                                       //�����
                      NodeBindMode        mode = NodeBindMode_Default,                 //����� �������������
                      NodeTransformSpace  invariant_space = NodeTransformSpace_Local); //������������ ������������ ��������������

      //������ ����������
    void Unbind (NodeTransformSpace invariant_space = NodeTransformSpace_Local);

      //������������ �������
    void UnbindChild (const char* name, NodeTransformSpace invariant_space = NodeTransformSpace_Local);
    void UnbindChild (const char* name, NodeSearchMode find_mode, NodeTransformSpace invariant_space = NodeTransformSpace_Local);

      //������������ ���� ��������    
    void UnbindAllChildren ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Pointer      FindChild (const char* name, NodeSearchMode mode = NodeSearchMode_Default);
    ConstPointer FindChild (const char* name, NodeSearchMode mode = NodeSearchMode_Default) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���� � ������������ ���������������� �� ���� (���������� �������� Visitor)
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::basic_visitor<void> Visitor;

    void Accept (Visitor&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Node&)>       TraverseFunction;
    typedef xtl::function<void (const Node&)> ConstTraverseFunction;

    void Traverse  (const TraverseFunction&, NodeTraverseMode = NodeTraverseMode_Default);
    void Traverse  (const ConstTraverseFunction&, NodeTraverseMode = NodeTraverseMode_Default) const;
    void VisitEach (Visitor&, NodeTraverseMode = NodeTraverseMode_Default) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetPosition   (const math::vec3f&);
    void               SetPosition   (float x, float y, float z);
    void               ResetPosition ();
    const math::vec3f& Position      () const;
    const math::vec3f& WorldPosition () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetOrientation   (const math::quatf&);
    void               SetOrientation   (float angle_in_degrees, float axis_x, float axis_y, float axis_z);
    void               SetOrientation   (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees); //���� ������
    void               ResetOrientation ();
    const math::quatf& Orientation      () const;
    const math::quatf& WorldOrientation () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetScale   (const math::vec3f&);
    void               SetScale   (float sx, float sy, float sz);
    void               ResetScale ();
    const math::vec3f& Scale      () const;
    const math::vec3f& WorldScale () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetOrientationInherit  (bool state); //��������� ����� ������������ ������������ ����������
    void SetScaleInherit        (bool state); //��������� ����� ������������ ������������� ��������
    bool IsOrientationInherited () const;     //����������� �� ������������ ����������
    bool IsScaleInherited       () const;     //����������� �� ������������ �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Translate (const math::vec3f&, NodeTransformSpace = NodeTransformSpace_Parent);
    void Translate (float offset_x, float offset_y, float offset_z, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (const math::quatf&, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (float angle_in_degrees, float axis_x, float axis_y, float axis_z, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (float pitch_in_degrees, float yaw_in_degrees, float roll_in_degrees, NodeTransformSpace = NodeTransformSpace_Parent);
    void Scale     (const math::vec3f&);
    void Scale     (float sx, float sy, float sz);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const math::mat4f& TransformationMatrix (NodeTransformSpace) const;

      //����������� ������
    const math::mat4f& LocalTM  () const { return TransformationMatrix (NodeTransformSpace_Local); }
    const math::mat4f& WorldTM  () const { return TransformationMatrix (NodeTransformSpace_World); }
    const math::mat4f& ParentTM () const { return TransformationMatrix (NodeTransformSpace_Parent); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������������� ���� object � ������� ��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::mat4f ObjectTM (Node&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� Node
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::signal<void (Node& sender, NodeEvent event), xtl::default_signal_accumulator<void> > Signal;
    typedef xtl::signal<void (Node& sender, Node& child, NodeSubTreeEvent event), xtl::default_signal_accumulator<void> > SubTreeSignal;

    Signal&        Event (NodeEvent) const;
    SubTreeSignal& Event (NodeSubTreeEvent) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ����������
///  �� ����� ���������� ���������� �� ���������� �� ������������, � �������������� � ����������
///  ��� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginUpdate ();
    void EndUpdate   ();
    bool IsInUpdateTransaction () const;

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
            Node  ();
    virtual ~Node ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� �� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateNotify () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���������. ���������: ���������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    template <class T> static bool TryAccept (T&, Visitor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����, ���������� ��� ��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AcceptCore (Visitor&);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AfterUpdateWorldTransformEvent () {} //����� ��������� ��������� �������
    virtual void AfterSceneAttachEvent () {}          //����� ������������� ������� � �����
    virtual void BeforeSceneDetachEvent () {}         //����� ������������� ������� �� �����
    
  private:
    static void DestroyNode (Node*);
    void SetScene (scene_graph::Scene* in_scene);

  private:
    Node (const Node&); //no impl
    Node& operator = (const Node&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������������� �����, ��� ������������� ������������ ���������� �� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class NodeUpdateLock
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    NodeUpdateLock  ();
    NodeUpdateLock  (Node&);
    NodeUpdateLock  (const NodeUpdateLock&);
    ~NodeUpdateLock ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    NodeUpdateLock& operator = (const NodeUpdateLock&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (NodeUpdateLock&);

  private:
    Node* node;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (NodeUpdateLock&, NodeUpdateLock&);

#include <sg/detail/node.inl>

}

#endif
