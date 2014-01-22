#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace scene_graph;
using namespace scene_graph::controllers;

using media::animation::AnimationBlender;
using media::animation::TargetBlender;

namespace
{

/*
    ��������
*/

const size_t RESERVED_ANIMATIONS_COUNT = 8;                       //������������� ���������� ��������, ������������ �������������� � �����������
const size_t TIME_PRECISION            = 1000;                    //����������� ����������� ������� ��������
const size_t BAD_TIME_NUMERATOR        = ~0u;                     //�������� �����
const char*  LOG_NAME                  = "scene_graph.animation"; //��� ������ ����������������

/*
    ��������������� ���������
*/

///����� ����������� ���������� �������� �� ��������� �����
struct AnimationParameterNameMap
{
  media::animation::PropertyNameMap names; //����� ����
  
///�����������
  AnimationParameterNameMap ()
  {
    names.Set ("position", "Position");
    names.Set ("orientation", "Orientation");
    names.Set ("rotation", "Rotation");
    names.Set ("scale", "Scale");
    names.Set ("world_position", "WorldPosition");
    names.Set ("world_orientation", "WorldOrientation");
    names.Set ("world_rotation", "WorldRotation");
    names.Set ("world_scale", "WorldScale");    
    names.Set ("alpha", "Alpha");
  }
};

typedef common::Singleton<AnimationParameterNameMap> AnimationParameterNameMapSingleton;

}

namespace scene_graph
{

namespace controllers
{

///�������� ���������� ��������
struct AnimationImpl: public xtl::reference_counter, public xtl::trackable, public xtl::noncopyable, public xtl::instance_counter<AnimationImpl>
{
  typedef xtl::signal<void (AnimationEvent event, Animation& animation)> Signal;
  
  enum Mode { Playing, Stopped, Paused };

  AnimationBlender                 blender;                      //������� ��������
  media::animation::Animation      source;                       //�������� ��������
  media::animation::AnimationState state;                        //��������� ��������
  TimeValue                        time;                         //������� �����
  TimeValue                        start_time;                   //����� ������ ��������
  TimeValue                        offset;                       //�������� �������
  float                            duration;                     //������������ ��������
  Mode                             mode;                         //������� �����
  bool                             looped;                       //���� ����������� ��������
  Signal                           signals [AnimationEvent_Num]; //�������

///�����������
  AnimationImpl (const AnimationBlender& in_blender, const media::animation::Animation& in_source)
    : blender (in_blender)
    , source (in_source)
    , time (BAD_TIME_NUMERATOR, 1)
    , start_time (BAD_TIME_NUMERATOR, 1)
    , duration ()
    , mode (Stopped)
    , looped (false)
  {
    float min_time = 0.0f, max_time = 0.0f;

    source.GetTimeLimits (min_time, max_time);

    duration = max_time; //NOT max_time - min_time
  }  
  
///��������� �������� ������������ ������ ��������
  TimeValue Tell ()
  {
    if (mode == Playing) return time - start_time + offset;
    else                 return offset;
  }
  
  float ClampedTell ()
  {
    float offset = Tell ().cast<float> ();

    if (looped) return fmod (offset, duration);
    else        return offset < duration ? Tell ().cast<float> () : duration;
  }  
  
///���������� � ������������� �������
  void Notify (AnimationEvent event, Animation* animation = 0)
  {
    if (event < 0 || event >= AnimationEvent_Num || signals [event].empty ())
      return;
      
    try
    {
      if (animation)
      {
        signals [event] (event, *animation);
      }
      else
      {
        Animation animation (*this);
      
        signals [event] (event, animation);
      }
    }
    catch (std::exception& e)
    {
      common::Log (LOG_NAME).Printf ("%s\n    at scene_graph::controllers::AnimationImpl::Notify", e.what ());
    }
    catch (...)
    {
      common::Log (LOG_NAME).Printf ("unknown exception\n    at scene_graph::controllers::AnimationImpl::Notify");   
    }
  }
};

}

}

namespace
{

struct AnimationTarget {};

///������������ ����
struct Target: public xtl::reference_counter, public xtl::instance_counter<AnimationTarget>
{
  Node*                                                   node;                 //����������� ����
  TargetBlender                                           blender;              //������� ������������ ����
  stl::auto_ptr<common::PropertyBindingMap::Synchronizer> synchronizer;         //������������� �������� ����������� �������
  xtl::auto_connection                                    on_update_connection; //���������� � �������� ���������� ������������ ����
  
///�����������
  Target (const TargetBlender& in_blender)
    : node ()
    , blender (in_blender)
    , on_update_connection (blender.RegisterEventHandler (media::animation::TargetBlenderEvent_OnUpdate, xtl::bind (&Target::Update, this)))
  {
  }
  
///������������� ����
  void BindNode (Node* node)
  {
    if (!node)
    {
      UnbindNode ();
      return;
    }        

    synchronizer.reset (new common::PropertyBindingMap::Synchronizer (node->PropertyBindings ().CreateSynchronizer (const_cast<common::PropertyMap&> (blender.Properties ()))));
    
    this->node = node;    
  }
  
///������������ ����
  void UnbindNode ()
  {
    node = 0;

    synchronizer.reset ();
  }
  
///���������� ������������ ����
  void Update ()
  {
    if (!node || !synchronizer)
      return;
    
    try
    {
      synchronizer->CopyFromPropertyMap ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("scene_graph::controllers::AnimationController::Impl::Target::Update(node='%s')", node->Name ());
      throw;
    }
  }
};

typedef xtl::intrusive_ptr<Target>                           TargetPtr;
typedef stl::hash_map<stl::hash_key<const char*>, TargetPtr> TargetMap;
typedef stl::vector<AnimationImpl*>                          AnimationList;
typedef xtl::signal<void (float time, const char* event)>    EventSignal;

}

/*
    �������� ��������� ������������� �����������
*/

struct AnimationController::Impl: public xtl::trackable, public xtl::instance_counter<AnimationController>
{
  AnimationController& owner;         //������ �� ���������
  AnimationManager     manager;       //�������� ��������
  AnimationBlender     blender;       //������� ��������
  TargetMap            targets;       //������������ ����
  AnimationList        animations;    //��������
  EventSignal          event_signal;  //������ ���������� � ������������� �������
  EventHandler         event_handler; //���������� ���������� � ������������� �������
  common::Log          log;           //����� ����������������
  
///�����������
  Impl (AnimationController& controller, const AnimationManager& in_manager)
    : owner (controller)
    , manager (in_manager)
    , event_handler (xtl::bind (&Impl::UpdateNotify, this, _1, _2))
    , log (LOG_NAME)
  {
    connect_tracker (blender.RegisterEventHandler (media::animation::AnimationBlenderEvent_OnTargetAdded,   xtl::bind (&Impl::AddTarget, this, _2, _3)));
    connect_tracker (blender.RegisterEventHandler (media::animation::AnimationBlenderEvent_OnTargetRemoved, xtl::bind (&Impl::RemoveTarget, this, _2)));
    
    Node* node = owner.AttachedNode ();
    
    if (node)
    {
      connect_tracker (node->RegisterEventHandler (NodeSubTreeEvent_AfterBind, xtl::bind (&Impl::BindNode, this, _2)));
      connect_tracker (node->RegisterEventHandler (NodeSubTreeEvent_BeforeUnbind, xtl::bind (&Impl::UnbindNode, this, _2)));
    }
  }
  
///���������� ������������ ����
  void AddTarget (const char* target_name, TargetBlender& target_blender)
  {
    try
    {
        //��������� ������������� ����
      
      Node* root = owner.AttachedNode ();
      
      if (!root)
        throw xtl::format_operation_exception ("", "Animation node was detached");

        //�������� ������������

      if (targets.find (target_name) != targets.end ())
        throw xtl::format_operation_exception ("", "Animation target '%s' has already registered", target_name);
        
        //�������� ������������ ����

      TargetPtr target (new Target (target_blender), false);
        
        //����������� ������������ ����

      targets.insert_pair (target_name, target);

        //����� �������

      Node::Pointer node;

      if (!strcmp (root->Name (), target_name))
      {
        node = root;
      }
      else
      {    
        node = root->FindChild (target_name, NodeSearchMode_OnAllSublevels);
      }
      
      if (node)
        target->BindNode (&*node);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at scene_graph::controllers::AnimationController::Impl::AddTarget", e.what ());
    }
    catch (...)
    {
      log.Printf ("uknown exception\n    at scene_graph::controllers::AnimationController::Impl::AddTarget");
    }    
  }
  
///�������� ������������ ����
  void RemoveTarget (const char* target_name)
  {
    if (!target_name)
      return;

    targets.erase (target_name);
  }
  
///���������� � ������������� ����
  void BindNode (Node& child)
  {
    try
    {
      const char* target_name = child.Name ();
      
      if (!target_name)
        return;

      TargetMap::iterator iter = targets.find (target_name);

      if (iter == targets.end ())
        return;

      Target& target = *iter->second;

      if (target.node)
        throw xtl::format_operation_exception ("", "Duplicate node for animation target '%s'", target_name);

      target.BindNode (&child);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at scene_graph::controllers::AnimationController::Impl::BindNode", e.what ());      
    }
    catch (...)
    {
      log.Printf ("uknown exception\n    at scene_graph::controllers::AnimationController::Impl::BindNode");
    }
  }
  
///���������� �� ������������ ����
  void UnbindNode (Node& child)
  {
    const char* target_name = child.Name ();
    
    if (!target_name)
      return;

    TargetMap::iterator iter = targets.find (target_name);

    if (iter == targets.end ())
      return;

    Target& target = *iter->second;

    if (target.node == &child)
      target.UnbindNode ();
  }
  
///�������� ��������
  void RemoveAnimation (AnimationImpl* animation)
  {
    animations.erase (stl::remove (animations.begin (), animations.end (), animation), animations.end ());
  }
  
///���������� � ������������� �������
  void UpdateNotify (float time, const char* event)
  {
    event_signal (time, event);
  }
};

/*
===================================================================================================
    AnimationController
===================================================================================================
*/

/*
    ����������� / ����������
*/

AnimationController::AnimationController (Node& node, AnimationManager& manager)
  : Controller (node, ControllerTimeMode_Absolute)
  , impl (new Impl (*this, manager))
{
  NodeOwnsController ();
}

AnimationController::~AnimationController ()
{
}

/*
    �������� �����������
*/

AnimationController::Pointer AnimationController::Create (Node& node, AnimationManager& manager)
{
  try
  {
    return Pointer (new AnimationController (node, manager), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::Create");
    throw;
  }
}

/*
    ������� �������� � �������
*/

size_t AnimationController::AnimationsCount () const
{
  return impl->animations.size ();
}

Animation AnimationController::Animation (size_t index) const
{
  if (index >= impl->animations.size ())
    throw xtl::make_range_exception ("scene_graph::controllers::AnimationController::Animation", "index", index, impl->animations.size ());

  return scene_graph::controllers::Animation (*impl->animations [index]);
}

/*
    ������������ ��������
*/

Animation AnimationController::PlayAnimation (const char* name)
{
  return PlayAnimation (name, Animation::EventHandler ());
}

Animation AnimationController::PlayAnimation (const char* name, const Animation::EventHandler& on_finish_handler)
{
  try
  {
    if (!AttachedNode ())
      throw xtl::format_operation_exception ("", "Can't create animation. Controller's node was detached");
    
    return impl->manager.PlayAnimation (name, *AttachedNode (), on_finish_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::PlayAnimation");
    throw;
  }
}

Animation AnimationController::CreateAnimation (const char* name)
{
  try
  {
    if (!AttachedNode ())
      throw xtl::format_operation_exception ("", "Can't create animation. Controller's node was detached");
    
    return impl->manager.CreateAnimation (name, *AttachedNode ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::CreateAnimation(const char*)");
    throw;
  }
}

scene_graph::controllers::Animation AnimationController::CreateAnimation (const media::animation::Animation& animation)
{
  try
  {    
      //�������� ��������
      
    xtl::intrusive_ptr<AnimationImpl> source (new AnimationImpl (impl->blender, animation), false);
    
    source->connect_tracker (xtl::bind (&Impl::RemoveAnimation, &*impl, &*source), *impl);

      //����������� ��������
    
    impl->animations.push_back (&*source);
    
    return scene_graph::controllers::Animation (*source);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::CreateAnimation(const media::animation::Animation&)");
    throw;
  }
}

/*
    ��������� ���� ��������
*/

void AnimationController::StopAllAnimations ()
{
  try
  {
    for (AnimationList::iterator iter=impl->animations.begin (), end=impl->animations.end (); iter!=end; ++iter)
      scene_graph::controllers::Animation (**iter).Stop ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::StopAllAnimations");
    throw;
  }  
}

/*
    ����������
*/

void AnimationController::Update (const TimeValue& value)
{  
  try
  {
      //���������� �������

    for (AnimationList::iterator iter=impl->animations.begin (), end=impl->animations.end (); iter!=end; ++iter)
    {
      AnimationImpl& animation = **iter;
      
      if (animation.mode != AnimationImpl::Playing)
      {
        animation.time = value;
        continue;
      }
      
        //���������� �������
      
      TimeValue prev_time   = animation.time;
      TimeValue prev_offset = animation.Tell ();
      
      if (animation.start_time.numerator () == BAD_TIME_NUMERATOR)
        animation.start_time = value;
      
      animation.time = value;
      
        //���������� ��������� ��������

      float foffset = animation.ClampedTell ();

      animation.state.SetTime (foffset);
      
        //���������� � ������������� �������
        
      animation.Notify (AnimationEvent_OnUpdate);
      
      if (prev_time.numerator () != BAD_TIME_NUMERATOR)
      {
        float prev_foffset = prev_offset.cast<float> ();
        
        if (size_t (foffset / animation.duration) > size_t (prev_foffset / animation.duration))
          animation.Notify (AnimationEvent_OnFinish);
      }      
    }

      //���������� �������� ��������

    impl->blender.Update (impl->event_handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controller::AnimationController::Update");
    throw;
  }
}

/*
    ��������� �������
*/

xtl::connection AnimationController::RegisterEventHandler (const EventHandler& handler) const
{
  return impl->event_signal.connect (handler);
}

/*
    ����������� ����������� ��� ���������� �������� �� ����� �����
*/

void AnimationController::SetParameterMapping (const char* channel_name, const char* property_name)
{
  try
  {
    AnimationParameterNameMapSingleton::Instance ()->names.Set (channel_name, property_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::SetParameterMapping");
    throw;
  }
}

void AnimationController::RemoveParameterMapping (const char* channel_name)
{
  try
  {
    AnimationParameterNameMapSingleton::Instance ()->names.Reset (channel_name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::RemoveParameterMapping");
    throw;
  }
}

void AnimationController::RemoveAllParameterMappings ()
{
  try
  {
    AnimationParameterNameMapSingleton::Instance ()->names.Clear ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::AnimationController::RemoveAllParameterMappings");
    throw;
  }
}

/*
===================================================================================================
    Animation
===================================================================================================
*/

/*
    ������������ / ���������� / ������������
*/

Animation::Animation (AnimationImpl& in_impl)
  : impl (&in_impl)
{
  addref (impl);
}

Animation::Animation (const Animation& animation)
  : impl (animation.impl)
{
  addref (impl);
}

Animation::~Animation ()
{
  if (impl->decrement ())
  {
    impl->Notify (AnimationEvent_OnDestroy, this);
    
    if (impl->use_count () == 0)
      delete impl;
  }
}

Animation& Animation::operator = (const Animation& animation)
{
  Animation (animation).Swap (*this);
  return *this;
}

/*
    ��� ��������
*/

const char* Animation::Name () const
{
  return impl->source.Name ();
}

/*
    ������������ ��������
*/

void Animation::Play ()
{
  try
  {
    impl->start_time = impl->time;    
    
    switch (impl->mode)
    {
      case AnimationImpl::Playing:
        impl->offset = TimeValue ();
        break;
      default:
      case AnimationImpl::Stopped:
          //���������� �������� � �������

        impl->state = impl->blender.AddSource (impl->source, AnimationParameterNameMapSingleton::Instance ()->names);
      case AnimationImpl::Paused:
          //���������� � ������������� �������

        impl->Notify (AnimationEvent_OnPlay);        
        break;
    }  
    
    impl->mode = AnimationImpl::Playing;
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::Animation::Play");
    throw;
  }
}

void Animation::Stop ()
{
  try
  {
    switch (impl->mode)
    {
      case AnimationImpl::Playing:
      case AnimationImpl::Paused:
        impl->state = media::animation::AnimationState ();      
        break;
      default:
      case AnimationImpl::Stopped:
        return;
    }

    impl->mode   = AnimationImpl::Stopped;
    impl->offset = TimeValue ();

      //���������� � ������������� �������

    impl->Notify (AnimationEvent_OnStop);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("scene_graph::controllers::Animation::Stop");
    throw;
  }
}

void Animation::Pause()
{
  switch (impl->mode)
  {
    case AnimationImpl::Stopped:
    case AnimationImpl::Paused:
      return;
    default:
      break;      
  }
    
  impl->mode   = AnimationImpl::Paused;
  impl->offset = impl->time - impl->start_time;
  
    //���������� � ������������� �������
  
  impl->Notify (AnimationEvent_OnPause);
}

bool Animation::IsPlaying () const
{
  switch (impl->mode)
  {
    case AnimationImpl::Stopped:
    case AnimationImpl::Paused:
      return false;
    default:
      break;
  }
    
  if (impl->looped)
    return true;
    
  float offset   = impl->Tell ().cast<float> (),
        duration = impl->duration;  

  return offset < duration;
}

/*
    ������������ ��������
*/

float Animation::Duration () const
{
  return impl->duration;
}

/*
    ����������������
*/

float Animation::Tell () const
{
  return impl->ClampedTell ();
}

void Animation::Seek (float offset, AnimationSeekMode mode)
{
  float duration = impl->duration;

  if (offset < 0.0f)
    offset = 0.0f; 

  switch (mode)
  {
    case AnimationSeekMode_Set:
      impl->offset = TimeValue (static_cast<size_t> (offset * TIME_PRECISION), TIME_PRECISION);
      break;
    case AnimationSeekMode_Current:
      impl->offset = impl->Tell () + TimeValue (static_cast<size_t> (offset * TIME_PRECISION), TIME_PRECISION);
      break;
    case AnimationSeekMode_End:
      impl->offset = TimeValue (static_cast<size_t> ((duration - offset) * TIME_PRECISION), TIME_PRECISION);
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::controllers::Animation::Seek", "seek_mode", mode);
  }

  impl->start_time = impl->time;
}

/*
    �������� �� �������
*/

xtl::connection Animation::RegisterEventHandler (AnimationEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= AnimationEvent_Num)
    throw xtl::make_argument_exception ("scene_graph::controllers::Animation::RegisterEventHandler", "event", event);
    
  return impl->signals [event].connect (handler);
}

/*
    ����������� ��������
*/

void Animation::SetLooping (bool state)
{
  impl->looped = state;
}

bool Animation::IsLooping  () const
{
  return impl->looped;
}

/*
    ���
*/

void Animation::SetWeight (float weight)
{
  impl->state.SetWeight (weight);
}

float Animation::Weight () const
{
  return impl->state.Weight ();
}

/*
    �����
*/

void Animation::Swap (Animation& animation)
{
  stl::swap (impl, animation.impl);
}

namespace scene_graph
{

namespace controllers
{

void swap (Animation& animation1, Animation& animation2)
{
  animation1.Swap (animation2);
}

}

}
