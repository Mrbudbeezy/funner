///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
class LineGenerator
{
  public:
    enum {
      VERTICES_PER_PRIMITIVE = 2,
      INDICES_PER_PRIMITIVE  = 2
    };

    static void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);
};

class StandaloneLineGenerator: public LineGenerator
{
  public:
    static void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);  
};

class BatchingLineGenerator: public LineGenerator
{
  public:
    BatchingLineGenerator (const math::mat4f& world_tm);

    void Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);  

  private:
    const math::mat4f& world_tm;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteGenerator
{
  public:
    enum {
      VERTICES_PER_PRIMITIVE = 4,
      INDICES_PER_PRIMITIVE  = 6
    };

    static void Generate (const Sprite& src_sprite, const math::vec3f& normal, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);
};

class BillboardSpriteGenerator: public SpriteGenerator
{
  public:
    BillboardSpriteGenerator (const math::vec3f& local_normal, const math::vec3f& local_up, const math::mat4f& world_tm);

    void Generate (const BillboardSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);

  private:
    const math::mat4f& world_tm;
    math::vec3f        world_normal;
    math::vec3f        up;
    math::vec3f        right;
};

class StandaloneOrientedSpriteGenerator: public SpriteGenerator
{
  public:
    StandaloneOrientedSpriteGenerator (const math::vec3f& local_up);

    void Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);  

  private:
    const math::vec3f& up;    
};

class BatchingOrientedSpriteGenerator: public SpriteGenerator
{
  public:
    BatchingOrientedSpriteGenerator (const math::vec3f& local_up, const math::mat4f& world_tm);

    void Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices);  

  private:
    const math::mat4f& world_tm;
    const math::vec3f& up;    
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ������������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T> class DynamicPrimitiveListImpl: public Object, public CacheSource
{
  public:
    typedef T Item;

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    DynamicPrimitiveListImpl  (const MaterialManagerPtr& material_manager);
    ~DynamicPrimitiveListImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetMaterial (const char* material);
    const char* Material    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� / ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Add    (size_t count, const Item* items);
    void   Update (size_t first, size_t count, const Item* items);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Remove (size_t first, size_t count);
    void Clear  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void   Reserve  (size_t count);
    size_t Capacity ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveFromPrimitive (PrimitiveImpl&);

  protected:
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Item* Items ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCacheCore ();

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������������� ������ ����� ���������� �������� (���������� � �������� ���������� ����)
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void UpdateBuffersCore () = 0;

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������
///////////////////////////////////////////////////////////////////////////////////////////////////

inline void LineGenerator::Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{
  DynamicPrimitiveVertex* dst_vertex = dst_vertices;
  const LinePoint*        src_point  = src_line.point;

  for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
  {
    dst_vertex->color     = src_point->color;
    dst_vertex->tex_coord = src_point->tex_offset;
  }

  dst_indices [0] = base_vertex;
  dst_indices [1] = base_vertex + 1;
}

inline void StandaloneLineGenerator::Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{
  DynamicPrimitiveVertex* dst_vertex = dst_vertices;
  const LinePoint*        src_point  = src_line.point;

  for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
    dst_vertex->position = src_point->position;
}

inline BatchingLineGenerator::BatchingLineGenerator (const math::mat4f& in_world_tm)
  : world_tm (in_world_tm)
{
}

inline void BatchingLineGenerator::Generate (const Line& src_line, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{
  DynamicPrimitiveVertex* dst_vertex = dst_vertices;
  const LinePoint*        src_point  = src_line.point;

  for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++, src_point++)
    dst_vertex->position = world_tm * src_point->position;
}

inline void SpriteGenerator::Generate (const Sprite& src_sprite, const math::vec3f& normal, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{  
  DynamicPrimitiveVertex* dst_vertex = dst_vertices;

  for (size_t i=0; i<VERTICES_PER_PRIMITIVE; i++, dst_vertex++)
  {
    dst_vertex->color  = src_sprite.color;
    dst_vertex->normal = normal;
  }

  dst_vertices [0].tex_coord = src_sprite.tex_offset;
  dst_vertices [1].tex_coord = src_sprite.tex_offset + math::vec2f (src_sprite.tex_size.x, 0);
  dst_vertices [2].tex_coord = src_sprite.tex_offset + src_sprite.tex_size;
  dst_vertices [3].tex_coord = src_sprite.tex_offset + math::vec2f (0, src_sprite.tex_size.y);

  static const DynamicPrimitiveIndex indices [INDICES_PER_PRIMITIVE] = {0, 2, 3, 0, 1, 2};

  const DynamicPrimitiveIndex* src_index = indices;
  DynamicPrimitiveIndex*       dst_index = dst_indices;

  for (size_t i=0; i<INDICES_PER_PRIMITIVE; i++, dst_index++, src_index++)
    *dst_index = *src_index + base_vertex;
}

inline BillboardSpriteGenerator::BillboardSpriteGenerator (const math::vec3f& local_normal, const math::vec3f& local_up, const math::mat4f& in_world_tm)
  : world_tm (in_world_tm)
  , world_normal (world_tm * math::vec4f (local_normal, 0))
  , right (cross (local_up, local_normal))
  , up (cross (local_normal, right))
{
}

inline void BillboardSpriteGenerator::Generate (const BillboardSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{  
  math::vec3f ortx = right * src_sprite.size.x * 0.5f,
              orty = up * src_sprite.size.y * 0.5f;

  dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
  dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
  dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
  dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

  SpriteGenerator::Generate (src_sprite, world_normal, base_vertex, dst_vertices, dst_indices);
}

inline StandaloneOrientedSpriteGenerator::StandaloneOrientedSpriteGenerator (const math::vec3f& local_up)
  : up (local_up)
{
}

inline void StandaloneOrientedSpriteGenerator::Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{  
  math::vec3f ortx = cross (up, src_sprite.normal);

  if (src_sprite.rotation != math::anglef ())
    ortx = math::rotate (src_sprite.rotation, src_sprite.normal) * math::vec4f (ortx, 0);

  math::vec3f orty = cross (src_sprite.normal, ortx);

  ortx *= src_sprite.size.x * 0.5f;
  orty *= src_sprite.size.y * 0.5f;

  dst_vertices [0].position = src_sprite.position - ortx - orty;
  dst_vertices [1].position = src_sprite.position + ortx - orty;
  dst_vertices [2].position = src_sprite.position + ortx + orty;
  dst_vertices [3].position = src_sprite.position - ortx + orty;

  SpriteGenerator::Generate (src_sprite, src_sprite.normal, base_vertex, dst_vertices, dst_indices);
}

inline BatchingOrientedSpriteGenerator::BatchingOrientedSpriteGenerator (const math::vec3f& local_up, const math::mat4f& in_world_tm)
  : up (local_up)
  , world_tm (in_world_tm)
{
}

inline void BatchingOrientedSpriteGenerator::Generate (const OrientedSprite& src_sprite, size_t base_vertex, DynamicPrimitiveVertex* dst_vertices, DynamicPrimitiveIndex* dst_indices)
{  
  math::vec3f ortx = cross (up, src_sprite.normal);

  if (src_sprite.rotation != math::anglef ())
    ortx = math::rotate (src_sprite.rotation, src_sprite.normal) * math::vec4f (ortx, 0);

  math::vec3f orty = cross (src_sprite.normal, ortx);

  ortx *= src_sprite.size.x * 0.5f;
  orty *= src_sprite.size.y * 0.5f;

  dst_vertices [0].position = world_tm * (src_sprite.position - ortx - orty);
  dst_vertices [1].position = world_tm * (src_sprite.position + ortx - orty);
  dst_vertices [2].position = world_tm * (src_sprite.position + ortx + orty);
  dst_vertices [3].position = world_tm * (src_sprite.position - ortx + orty);

  SpriteGenerator::Generate (src_sprite, src_sprite.normal, base_vertex, dst_vertices, dst_indices);
}
