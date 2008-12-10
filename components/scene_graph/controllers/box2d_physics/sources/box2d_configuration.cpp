#include "shared.h"

using namespace common;
using namespace scene_graph;
using namespace scene_graph::physics;
using namespace scene_graph::physics::box2d;

namespace
{

/*
    ���������
*/

const b2Vec2 DEFAULT_GRAVITY (0.0f, 0.0f);
const bool   DEFAULT_DO_SLEEP         = false;
const size_t DEFAULT_ITERATIONS_COUNT = 10;

}

/*
    �������
*/

template <class Token, class BaseIter>
inline bool read (xtl::io::token_iterator<Token, BaseIter>& iter, b2Vec2& v)
{
  math::vec2f tmp;
  
  if (!read (iter, tmp))
    return false;

  v.x = tmp.x;
  v.y = tmp.y;

  return true;
}

/*
    �������� ���������� ������������
*/

struct Configuration::Impl
{
  b2Vec2 gravity;          //����������
  bool   is_do_sleep;      //"��������" �� ���������� �������
  size_t iterations_count; //���������� �������� ���������� ����  
  
  void ParseConfiguration (ParseNode& node)
  {
    gravity          = get<b2Vec2> (node, "Gravity", DEFAULT_GRAVITY);
    is_do_sleep      = get<bool> (node, "DoSleep", DEFAULT_DO_SLEEP);
    iterations_count = get<size_t> (node, "IterationsCount", DEFAULT_ITERATIONS_COUNT);
  }
};

/*
    ����������� / ����������
*/

Configuration::Configuration (const char* file_name)
  : impl (new Impl)
{
  try
  {
      //�������� ������������ ����������
    
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
      //������ ������������
      
    Parser parser (file_name, "xml");
    
    ParseNode root = parser.Root ().First ();
    
    if (strcmp (root.Name (), "Box2D"))
    {
      root.Log ().Error (root, "Bad root node tag. Must be 'Box2D'");
    }
    else
    {    
      impl->ParseConfiguration (root);
    }
    
      //����� ��������� �� �������          

    Log log;

    parser.Log ().Print (xtl::bind (&Log::Print, &log, _1));

    if (parser.Log ().HasErrors ())
      throw xtl::format_operation_exception ("", "Bad configuration '%s'", file_name);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("scene_graph::physics::box2d::Configuration::Configuration");
    throw;
  }
}

Configuration::~Configuration ()
{
}

/*
    ��������� ����������
*/

//����������
const b2Vec2& Configuration::GetGravity () const
{
  return impl->gravity;
}

//"��������" �� ���������� �������
bool Configuration::IsDoSleep  () const
{
  return impl->is_do_sleep;
}

//���������� �������� ��������� ����������� ����
size_t Configuration::GetIterationsCount () const
{
  return impl->iterations_count;
}
