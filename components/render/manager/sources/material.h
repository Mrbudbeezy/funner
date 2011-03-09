///////////////////////////////////////////////////////////////////////////////////////////////////
///��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class MaterialImpl: public Object, public CacheSource
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    MaterialImpl  ();
    ~MaterialImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Id    ();
    void        SetId (const char* id);

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Update (const media::rfx::Material&);
    
  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
