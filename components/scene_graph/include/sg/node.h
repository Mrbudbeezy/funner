#ifndef SCENE_GRAPH_NODE_HEADER
#define SCENE_GRAPH_NODE_HEADER

#include <math/angle.h>
#include <math/matrix.h>
#include <math/quat.h>

#include <xtl/functional_fwd>
#include <xtl/dynamic_cast_root.h>
#include <xtl/intrusive_ptr.h>

namespace common
{

//forward declaration
class PropertyMap;

}

namespace xtl
{

//forward declaration
class any_reference;

}

namespace scene_graph
{

//forward declarations
class Scene;
class Node;
class Controller;

//implementation forwards
struct ControllerEntry;

namespace controllers
{

//forward declaration
class DefaultController;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeBindMode
{
  NodeBindMode_AddRef,  //��� ������������� ���������� ������ �� ���� ������������� �� 1
  NodeBindMode_WeakRef, //��� ������������� ���������� ������ �� ���� �� �������������
  
  NodeBindMode_Default = NodeBindMode_WeakRef
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
  NodeTraverseMode_OnlyThis,    //����������� ������ - � ������ ��������� ������ ������� ����

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
  NodeEvent_AfterSceneChange,  //����������� ����� ��������� �����

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
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum NodeOrt
{
  NodeOrt_X,   //��� Ox
  NodeOrt_Y,   //��� Oy
  NodeOrt_Z,   //��� Oz
  
  NodeOrt_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������ �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class INodeTraverser
{
  public:
    virtual void operator () (Node& node) = 0;
    
  protected:
    virtual ~INodeTraverser () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
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
///�������� ���� (����� ���� NULL)
///////////////////////////////////////////////////////////////////////////////////////////////////
    common::PropertyMap*       Properties    ();
    const common::PropertyMap* Properties    () const;
    void                       SetProperties (common::PropertyMap* properties);
    void                       SetProperties (const common::PropertyMap& properties);

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
    typedef xtl::function<void (Node&)> TraverseFunction;

    void Traverse  (const TraverseFunction&, NodeTraverseMode = NodeTraverseMode_Default) const;
    void Traverse  (INodeTraverser&, NodeTraverseMode = NodeTraverseMode_Default) const;
    void VisitEach (Visitor&, NodeTraverseMode = NodeTraverseMode_Default) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetPosition      (const math::vec3f&);
    void               SetPosition      (float x, float y, float z);
    void               ResetPosition    ();
    void               SetWorldPosition (const math::vec3f&);
    void               SetWorldPosition (float x, float y, float z);
    const math::vec3f& Position         () const;
    const math::vec3f& WorldPosition    () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ���� � ��������� ������� ��������� (����� ���������� ��������� � ��������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetPivotPosition           (const math::vec3f& pivot);
    void               SetPivotPosition           (float x, float y, float z);
    void               ResetPivotPosition         ();
    const math::vec3f& PivotPosition              () const;
    void               SetOrientationPivotEnabled (bool state); //��������� ����� ���������� ������ ���������
    void               SetScalePivotEnabled       (bool state); //��������� ����� ���������� ������ ��������
    bool               OrientationPivotEnabled    () const;
    bool               ScalePivotEnabled          () const;
    bool               PivotEnabled               () const; //��������: ������������ �� pivot � ������ ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetOrientation      (const math::quatf&);
    void SetOrientation      (const math::anglef& angle, float axis_x, float axis_y, float axis_z);
    void SetOrientation      (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll); //���� ������
    void ResetOrientation    ();
    void SetWorldOrientation (const math::quatf&);
    void SetWorldOrientation (const math::anglef& angle, float axis_x, float axis_y, float axis_z);
    void SetWorldOrientation (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll); //���� ������

    const math::quatf& Orientation      () const;
    const math::quatf& WorldOrientation () const;        

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetScale      (const math::vec3f&);
    void               SetScale      (float sx, float sy, float sz);
    void               SetWorldScale (const math::vec3f&);
    void               SetWorldScale (float sx, float sy, float sz);    
    void               ResetScale    ();
    const math::vec3f& Scale         () const;
    const math::vec3f& WorldScale    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LookTo (const math::vec3f& target_point, NodeTransformSpace = NodeTransformSpace_Local);
    void LookTo (const math::vec3f& target_point, const math::vec3f& up_vector, NodeTransformSpace = NodeTransformSpace_Local);
    void LookTo (const math::vec3f& target_point, NodeOrt direction, NodeOrt invariant, NodeTransformSpace = NodeTransformSpace_Local);

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
    void Rotate    (const math::anglef& angle, float axis_x, float axis_y, float axis_z, NodeTransformSpace = NodeTransformSpace_Parent);
    void Rotate    (const math::anglef& pitch, const math::anglef& yaw, const math::anglef& roll, NodeTransformSpace = NodeTransformSpace_Parent);
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
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::vec3f Ort (NodeOrt ort, NodeTransformSpace space) const;

      //����������� ������
    math::vec3f LocalOrtX  () const { return Ort (NodeOrt_X, NodeTransformSpace_Local); }
    math::vec3f LocalOrtY  () const { return Ort (NodeOrt_Y, NodeTransformSpace_Local); }
    math::vec3f LocalOrtZ  () const { return Ort (NodeOrt_Z, NodeTransformSpace_Local); }
    math::vec3f WorldOrtX  () const { return Ort (NodeOrt_X, NodeTransformSpace_World); }
    math::vec3f WorldOrtY  () const { return Ort (NodeOrt_Y, NodeTransformSpace_World); }
    math::vec3f WorldOrtZ  () const { return Ort (NodeOrt_Z, NodeTransformSpace_World); }
    math::vec3f ParentOrtX () const { return Ort (NodeOrt_X, NodeTransformSpace_Parent); }
    math::vec3f ParentOrtY () const { return Ort (NodeOrt_Y, NodeTransformSpace_Parent); }
    math::vec3f ParentOrtZ () const { return Ort (NodeOrt_Z, NodeTransformSpace_Parent); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� �������������� ���� object � ������� ��������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    math::mat4f ObjectTM (Node&) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ���� � ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Node& sender, NodeEvent event)> EventHandler;
    typedef xtl::function<void (Node& sender, Node& child, NodeSubTreeEvent event)> SubTreeEventHandler;

    xtl::connection RegisterEventHandler (NodeEvent event, const EventHandler& handler) const;
    xtl::connection RegisterEventHandler (NodeSubTreeEvent event, const SubTreeEventHandler& handler) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������ ����������
///  �� ����� ���������� ���������� �� ���������� �� ������������, � �������������� � ����������
///  ��� ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void BeginUpdate ();
    void EndUpdate   ();
    bool IsInUpdateTransaction () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void DetachAllControllers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (float dt)> UpdateHandler;

    xtl::com_ptr<controllers::DefaultController> AttachController (const UpdateHandler&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::com_ptr<Controller>       FirstController ();
    xtl::com_ptr<Controller>       LastController  ();
    xtl::com_ptr<const Controller> FirstController () const;
    xtl::com_ptr<const Controller> LastController  () const;

    template <class T> typename T::Pointer      FirstController ();
    template <class T> typename T::Pointer      LastController  ();
    template <class T> typename T::ConstPointer FirstController () const;
    template <class T> typename T::ConstPointer LastController  () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ���� � ��� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (float dt, NodeTraverseMode mode = NodeTraverseMode_Default);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������������� (for internal use)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void AttachController     (ControllerEntry&);
    void DetachController     (ControllerEntry&);
    void UpdateControllerList ();

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
           void SetScene    (scene_graph::Scene* in_scene);

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
