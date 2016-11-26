#ifndef SCENE_GRAPH_CONTROLLERS_MOVE_TO_NODE_POINT_HEADER
#define SCENE_GRAPH_CONTROLLERS_MOVE_TO_NODE_POINT_HEADER

#include <sg/controller.h>

namespace scene_graph
{

namespace controllers
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контроллер следования за точкой узла
///////////////////////////////////////////////////////////////////////////////////////////////////
class MoveToNodePoint: public Controller
{
  public:
    typedef xtl::com_ptr<MoveToNodePoint>       Pointer;
    typedef xtl::com_ptr<const MoveToNodePoint> ConstPointer;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание контроллера
///////////////////////////////////////////////////////////////////////////////////////////////////
    static Pointer Create (Node& node);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка/получение функции рассчета ускорения узла
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<const math::vec3f& (const math::vec3f& current_velocity, const math::vec3f& distance, float dt)> AccelerationFunction;

    void                        SetAccelerationHandler (const AccelerationFunction& acceleration_function);
    const AccelerationFunction& AccelerationHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка / получение пространства рассчета скорости
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetTransformSpace (NodeTransformSpace transform_space);
    NodeTransformSpace TransformSpace    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Запуск движения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Start (const Node& node, const math::vec3f& node_space_position);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Остановка движения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Stop ();

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    MoveToNodePoint  (Node& node);
    ~MoveToNodePoint ();
    
  private:
    void Update (const TimeValue& dt);

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

}

}

#endif
