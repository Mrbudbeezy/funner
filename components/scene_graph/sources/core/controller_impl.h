#ifndef SCENE_GRAPH_CONTROLLER_IMPL_HEADER
#define SCENE_GRAPH_CONTROLLER_IMPL_HEADER

namespace scene_graph
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� � ������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ControllerEntry
{
  Controller*      controller; //������ �� ������ �����������
  ControllerEntry* next;       //��������� ����������
  ControllerEntry* prev;       //���������� ����������
  bool             updatable;  //�������� �� ���������� �����������
  
  ControllerEntry ()
    : controller ()
    , next ()
    , prev ()
    , updatable ()
  {
  }
};

}

#endif
