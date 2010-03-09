///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class EntityImpl: public Object
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    EntityImpl  ();
    ~EntityImpl ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void                       SetProperties (const common::PropertyMap&);
    const common::PropertyMap& Properties    ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ��������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetTransformation (const math::mat4f&);
    const math::vec4f& Transformation    ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� (��� ���������)
///  �������������� ���������� �� ������� Entity::Transformation � ������ ���� ��� �� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void               SetJointsCount         (size_t count);
    size_t             JointsCount            ();
    void               SetJointTransformation (size_t joint_index, const math::mat4f&);
    const math::mat4f& JointTransformation    (size_t joint_index);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � �������� �����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t LodsCount ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const PrimitivePtr& Primitive           (size_t level_of_detail);
    void                SetPrimitive        (const PrimitivePtr&, size_t level_of_detail);
    bool                HasPrimitive        (size_t level_of_detail);
    void                ResetAllPrimitives  ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Entity Wrap ();

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};
