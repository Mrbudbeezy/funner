///forward declarations
class PrimitiveBuffersImpl;

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
    PrimitiveImpl  (const DeviceManagerPtr&, const BuffersPtr& buffers);
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
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
