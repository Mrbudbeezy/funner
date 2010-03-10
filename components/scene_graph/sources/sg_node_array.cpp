#include "shared.h"

using namespace scene_graph;

/*
    ��������� ���������� �������
*/

namespace scene_graph
{

class NodeArrayImpl: public xtl::reference_counter
{
  public:
    virtual ~NodeArrayImpl () {}

    virtual NodeArrayLinkMode LinkMode () = 0;
    virtual size_t            Size     () = 0;
    virtual Node&             Item     (size_t index) = 0;
    virtual size_t            Add      (Node& node) = 0;
    virtual void              Remove   (size_t index) = 0;
    virtual void              Remove   (Node& node) = 0;
    virtual void              Clear    () = 0;
    virtual void              Reserve  (size_t count) = 0;
    virtual size_t            Capacity () = 0;
};

}

namespace
{

/*
    ������ � ��������� ������ ���������
*/

struct NodeIntrusiveArray: public NodeArrayImpl
{
  public:
////�������� �������� ����������
    NodeArrayLinkMode LinkMode () { return NodeArrayLinkMode_AddRef; }

///���������� ��������� � �������
    size_t Size () { return items.size (); }

///��������� ��������
    Node& Item (size_t index)
    {
      if (index >= items.size ())
        throw xtl::make_range_exception ("scene_graph::NodeIntrusiveArray::Item", "index", index, items.size ());

      return *items [index];
    }

///���������� ��������
    size_t Add (Node& node)
    {
      items.push_back (&node);
      
      return items.size () - 1;
    }

///�������� �������� �� �������
    void Remove (size_t index)
    {
      if (index >= items.size ())
        return;
        
      items.erase (items.begin () + index);
    }

///�������� �������� �� ��������
    void Remove (Node& node)
    {
      for (Array::iterator iter=items.begin (); iter!=items.end ();)
        if (&**iter == &node) items.erase (iter);
        else                    ++iter;
    }

///������� �������
    void Clear () { items.clear (); }

///�������������� ����� ��� �������� ���������
    void Reserve (size_t count) { items.reserve (count); }
    
///���������� ����������������� ���������
    size_t Capacity () { return items.capacity (); }

  private:
    typedef stl::vector<Node::Pointer> Array;

  private:
    Array items;
};

/*
    ������ � �������������� ��������� ���������
*/

struct NodeWeakRefArray: public NodeArrayImpl
{
  public:
///�����������
    NodeWeakRefArray () : on_destroy_handler (DestroyHandler (this)) {}

////�������� �������� ����������
    NodeArrayLinkMode LinkMode () { return NodeArrayLinkMode_WeakRef; }

///���������� ��������� � �������
    size_t Size () { return items.size (); }

///��������� ��������
    Node& Item (size_t index)
    {
      if (index >= items.size ())
        throw xtl::make_range_exception ("scene_graph::NodeWeakArray::Item", "index", index, items.size ());

      return items [index]->node;
    }

///���������� ��������
    size_t Add (Node& node)
    {
      items.push_back (ItemHolderPtr (new ItemHolder (node, on_destroy_handler), false));
      
      return items.size () - 1;
    }

///�������� �������� �� �������
    void Remove (size_t index)
    {
      if (index >= items.size ())
        return;

      items.erase (items.begin () + index);
    }

///�������� �������� �� ��������
    void Remove (Node& node)
    {
      for (Array::iterator iter=items.begin (); iter!=items.end ();)
        if (&(*iter)->node == &node) items.erase (iter);
        else                             ++iter;
    }

///������� �������
    void Clear () { items.clear (); }

///�������������� ����� ��� �������� ���������
    void Reserve (size_t count) { items.reserve (count); }

///���������� ����������������� ���������
    size_t Capacity () { return items.capacity (); }

  private:      
    struct DestroyHandler
    {
      DestroyHandler (NodeWeakRefArray* in_array) : array (in_array) {}
      
      void operator () (Node& sender, NodeEvent) const { array->NodeWeakRefArray::Remove (*static_cast<Node*> (&sender)); }
      
      NodeWeakRefArray* array;
    };
  
    struct ItemHolder: public xtl::reference_counter
    {      
      Node&              node;
      xtl::auto_connection on_destroy;

      ItemHolder (Node& in_node, const Node::EventHandler& on_destroy_handler) :
        node (in_node),
        on_destroy (node.RegisterEventHandler (NodeEvent_AfterDestroy, on_destroy_handler))
        {}
    };

    typedef xtl::intrusive_ptr<ItemHolder> ItemHolderPtr;
    typedef stl::vector<ItemHolderPtr>     Array;

  private:
    Array              items;
    Node::EventHandler on_destroy_handler;
};

}

/*
    ������������ / ���������� / ������������
*/

NodeArray::NodeArray (NodeArrayLinkMode link_mode)
{
  try
  {
    switch (link_mode)
    {
      case NodeArrayLinkMode_AddRef:
        impl = new NodeIntrusiveArray;
        break;
      case NodeArrayLinkMode_WeakRef:
        impl = new NodeWeakRefArray;
        break;
      default:
        throw xtl::make_argument_exception ("", "link_mode", link_mode);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("scene_graph::NodeArray::NodeArray");
    throw;
  }
}

NodeArray::NodeArray (const NodeArray& array)
  : impl (array.impl)
{
  addref (impl);
}

NodeArray::~NodeArray ()
{
  release (impl);
}

NodeArray& NodeArray::operator = (const NodeArray& array)
{
  NodeArray (array).Swap (*this);

  return *this;
}

/*
    �������� ��������
*/

NodeArrayLinkMode NodeArray::LinkMode () const
{
  return impl->LinkMode ();  
}

/*
    ���������� ��������� � ��������� / �������� �� �������
*/

size_t NodeArray::Size () const
{
  return impl->Size ();
}

bool NodeArray::IsEmpty () const
{
  return impl->Size () == 0;
}

/*
    ��������� �������� ���������
*/

Node& NodeArray::Item (size_t index) const
{
  return impl->Item (index);
}

/*
    ���������� / �������� ��������
*/

size_t NodeArray::Add (Node& node)
{
  return impl->Add (node);
}

void NodeArray::Remove (Node& node)
{
  impl->Remove (node);
}

void NodeArray::Remove (size_t index)
{
  impl->Remove (index);
}

void NodeArray::Clear ()
{
  impl->Clear ();
}

/*
    �������������� ����� ��� �������� ��������
*/

size_t NodeArray::Capacity () const
{
  return impl->Capacity ();
}

void NodeArray::Reserve (size_t count)
{
  impl->Reserve (count);
}

/*
    �����
*/

void NodeArray::Swap (NodeArray& array)
{
  stl::swap (impl, array.impl);
}

namespace scene_graph
{

void swap (NodeArray& array1, NodeArray& array2)
{
  array1.Swap (array2);
}

}
