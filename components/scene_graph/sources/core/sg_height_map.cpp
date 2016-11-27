#include "shared.h"

using namespace scene_graph;

/*
    Описание реализации карты высот
*/

typedef stl::vector<HeightMap::VertexDesc>             VertexArray;
typedef xtl::signal<void (HeightMap&, HeightMapEvent)> HeightMapSignal;

struct HeightMap::Impl: public xtl::instance_counter<HeightMap>
{
  VertexArray     vertices;                     //вершины карты высот
  size_t          rows_count;                   //количество строк
  size_t          columns_count;                //количество столбцов
  HeightMapSignal signals [HeightMapEvent_Num]; //сигналы
  stl::string     material;                     //имя материала
  
  Impl () : rows_count (0), columns_count (0) {}
};

/*
    Конструктор / деструктор
*/

HeightMap::HeightMap ()
  : impl (new Impl)
{
}

HeightMap::~HeightMap ()
{
}

/*
    Создание карты
*/

HeightMap::Pointer HeightMap::Create ()
{
  return Pointer (new HeightMap, false);
}

/*
    Размеры карты
*/

void HeightMap::SetRowsCount (size_t rows_count)
{
  SetCellsCount (rows_count, ColumnsCount ());
}

void HeightMap::SetColumnsCount (size_t columns_count)
{
  SetCellsCount (RowsCount (), columns_count);
}

void HeightMap::SetCellsCount (size_t rows_count, size_t columns_count)
{
  if (rows_count == impl->rows_count && columns_count == impl->columns_count)
    return;

  if (!rows_count && !columns_count)
  {
    impl->vertices.clear ();
    
    impl->rows_count     = 0;
    impl->columns_count  = 0;

    try
    {
      impl->signals [HeightMapEvent_OnSizesUpdate] (*this, HeightMapEvent_OnSizesUpdate);
    }
    catch (...)
    {      
    }
    
    UpdateVerticesNotify ();
    
    return;
  }
  
  static const char* METHOD_NAME = "scene_graph::HeightMap::SetCellsCount";

  if (!rows_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "rows_count");

  if (!columns_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "columns_count");

  VertexArray new_vertices (rows_count * columns_count);

  for (size_t i=0, count=stl::min (impl->rows_count, rows_count); i<count; i++)
  {
    const VertexDesc* src = &impl->vertices [i * impl->columns_count];
    VertexDesc*       dst = &new_vertices [i * columns_count];

    stl::copy (src, src + stl::min (impl->columns_count, columns_count), dst);
  }

  impl->vertices.swap (new_vertices);

  impl->rows_count    = rows_count;
  impl->columns_count = columns_count;

  try
  {
    impl->signals [HeightMapEvent_OnSizesUpdate] (*this, HeightMapEvent_OnSizesUpdate);
  }
  catch (...)
  {      
  }  
  
  UpdateVerticesNotify ();
}

size_t HeightMap::RowsCount () const
{
  return impl->rows_count;
}

size_t HeightMap::ColumnsCount () const
{
  return impl->columns_count;
}

/*
    Материал
*/

void HeightMap::SetMaterial (const char* material)
{
  if (!material)
    throw xtl::make_null_argument_exception ("scene_graph::HeightMap::SetMaterial", "material");
    
  impl->material = material;
  
  UpdateNotify ();
}

const char* HeightMap::Material () const
{
  return impl->material.c_str ();
}

/*
    Работа с вершинами
*/

const HeightMap::VertexDesc* HeightMap::Vertices () const
{
  return impl->rows_count && impl->columns_count ? &impl->vertices [0] : 0;
}

HeightMap::VertexDesc* HeightMap::Vertices ()
{
  return impl->rows_count && impl->columns_count ? &impl->vertices [0] : 0;
}

const HeightMap::VertexDesc& HeightMap::Vertex (size_t row, size_t column) const
{
  static const char* METHOD_NAME = "scene_graph::HeightMap::Vertex";

  if (row >= impl->rows_count)
    throw xtl::make_range_exception (METHOD_NAME, "row", row, impl->rows_count);
    
  if (column >= impl->columns_count)
    throw xtl::make_range_exception (METHOD_NAME, "column", column, impl->columns_count);

  return impl->vertices [row * impl->columns_count + column];
}

HeightMap::VertexDesc& HeightMap::Vertex (size_t row, size_t column)
{
  return const_cast<VertexDesc&> (const_cast<const HeightMap&> (*this).Vertex (row, column));
}

/*
   Групповые операции
*/

void HeightMap::SetVerticesHeight (float height)
{
  for (VertexArray::iterator iter = impl->vertices.begin (), end = impl->vertices.end (); iter != end; ++iter)
    iter->height = height;
}

void HeightMap::SetVerticesNormal (const math::vec3f& normal)
{
  for (VertexArray::iterator iter = impl->vertices.begin (), end = impl->vertices.end (); iter != end; ++iter)
    iter->normal = normal;
}

void HeightMap::SetVerticesColor (const math::vec4f& color)
{
  for (VertexArray::iterator iter = impl->vertices.begin (), end = impl->vertices.end (); iter != end; ++iter)
    iter->color = color;
}

/*
    Оповещение об обновлении вершин
*/

void HeightMap::UpdateVerticesNotify ()
{
  try
  {
    if (!impl->signals [HeightMapEvent_OnVerticesUpdate].empty ())
      impl->signals [HeightMapEvent_OnVerticesUpdate] (*this, HeightMapEvent_OnVerticesUpdate);
  }
  catch (...)
  {
  }
  
  UpdateNotify ();
}

/*
    Регистрация обработчиков событий
*/

xtl::connection HeightMap::RegisterEventHandler (HeightMapEvent event, const EventHandler& handler)
{
  switch (event)
  {
    case HeightMapEvent_OnVerticesUpdate:
    case HeightMapEvent_OnSizesUpdate:
      break;
    default:
      throw xtl::make_argument_exception ("scene_graph::HeightMap::RegisterEventHandler", "event", event);
  }

  return impl->signals [event].connect (handler);
}

/*
    Динамическая диспетчеризация
*/

void HeightMap::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    VisualModel::AcceptCore (visitor);
}
