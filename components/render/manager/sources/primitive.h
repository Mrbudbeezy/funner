///forward declarations
class PrimitiveBuffersImpl;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitive
{
  MaterialImpl*                    material;    //��������
  render::low_level::IStateBlock*  state_block; //���� ��������� ���������
  bool                             indexed;     //�������� �� ������ �������� ��������������� ��� ��������� ������ �� ������ ��� ��������
  render::low_level::PrimitiveType type;        //��� ���������
  size_t                           first;       //������ ������ �������/�������
  size_t                           count;       //���������� ����������
  size_t                           tags_count;  //���������� ����� ���������
  const size_t*                    tags;        //���� ���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RendererPrimitiveGroup
{
  size_t                   primitives_count; //���������� ���������� � ������
  const RendererPrimitive* primitives;       //���������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���
///////////////////////////////////////////////////////////////////////////////////////////////////
class PrimitiveImpl: public Object, public CacheSource
{
  public:
    typedef xtl::intrusive_ptr<PrimitiveBuffersImpl> BuffersPtr;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    PrimitiveImpl  (const DeviceManagerPtr&, const MaterialManagerPtr&, const BuffersPtr& buffers);
    ~PrimitiveImpl ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const BuffersPtr& Buffers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t MeshesCount     ();
    size_t AddMesh         (const media::geometry::Mesh&, MeshBufferUsage vb_usage, MeshBufferUsage ib_usage);
    void   RemoveMeshes    (size_t first_mesh, size_t meshes_count);
    void   RemoveAllMeshes ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LinesCount       ();
    size_t AddLines         (size_t lines_count, const Line* lines, const MaterialPtr& material);
    void   UpdateLines      (size_t first_lines, size_t lines_count, const Line* lines);
    void   SetLinesMaterial (size_t first_lines, size_t lines_count, const MaterialPtr& material);
    void   RemoveLines      (size_t first_lines, size_t lines_count);
    void   RemoveAllLines   ();
    void   ReserveLines     (size_t lines_count);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t SpritesCount       ();
    size_t AddSprites         (size_t sprites_count, const Sprite* sprites, const MaterialPtr& material);
    void   UpdateSprites      (size_t first_sprite, size_t sprites_count, const Sprite* sprites);
    void   SetSpritesMaterial (size_t first_sprite, size_t sprites_count, const MaterialPtr& material);
    void   RemoveSprites      (size_t first_sprite, size_t sprites_count);
    void   RemoveAllSprites   ();
    void   ReserveSprites     (size_t sprites_count);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ���������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t                  RendererPrimitiveGroupsCount ();
    RendererPrimitiveGroup* RendererPrimitiveGroups      ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateCache ();
    void ResetCache  ();
    
  private:
    void UpdateCacheCore ();
    void ResetCacheCore  ();
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
