///������� ������
class BasicRenderer: public xtl::reference_counter, public xtl::noncopyable
{
  public:
///����������� / ����������
    BasicRenderer  (server::RenderManager& manager, const char* effect_name);
    ~BasicRenderer ();

///������ � �����
    server::RenderManager&  RenderManager   () { return manager; } 
    manager::Frame&         Frame           () { return frame; }
    common::PropertyMap&    FrameProperties () { return frame_properties; }

///���������
    void Draw (RenderingContext& parent_context);

  protected:
///��������� ����� ����� �����������
    virtual void ConfigureFrame (RenderingContext& parent_context);

///��������� �������
    virtual void ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, manager::EntityDrawParams& out_params);

///��������� �������
    void DrawVisualModels (RenderingContext& parent_context);

  private:
///��������� ������� ������������ �������
    void Draw (RenderingContext& context, size_t visual_models_count, VisualModel* const* models, void* user_data = 0) const;
    void Draw (RenderingContext& context, const stl::vector<VisualModel*>& models, void* user_data = 0) const;

  private:
///���������� ��������� ��������
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params);
  
  private:
    server::RenderManager manager;                      //�������� ����������
    manager::Frame        frame;                        //�����
    common::PropertyMap   frame_properties;             //�������� �����
    common::PropertyMap   frame_entity_properties;      //�������� ����-������
    size_t                view_transaction_id;          //������� ���������� ������� ����
    size_t                proj_transaction_id;          //������� ���������� ������� ��������
    size_t                view_proj_transaction_id;     //������� ���������� ������� ���-��������
    math::mat4f           view_tm;                      //������� ����
    math::mat4f           view_proj_tm;                 //������� ���-��������
    size_t                mv_matrix_property_index;     //������ �������� ������� ModelView
    size_t                mvp_matrix_property_index;    //������ �������� ������� ModelViewProjection
    size_t                object_matrix_property_index; //������ �������� ������� ObjectMatrix
    size_t                view_matrix_property_index;   //������ �������� ������� View (� ��������� �����)
    size_t                proj_matrix_property_index;   //������ �������� ������� Projection (� �������� �����)
};
