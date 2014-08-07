#include <xtl/common_exceptions.h>
#include <xtl/bind.h>
#include <xtl/function.h>

#include <common/parser.h>
#include <common/property_map.h>
#include <common/property_binding_map.h>
#include <common/strlib.h>

#include <shared/camera.h>
#include <shared/render_manager.h>
#include <shared/server.h>
#include <shared/technique_manager.h>

#include <shared/sg/visual_model.h>

#include <render/manager.h>

namespace render
{

namespace scene
{

namespace server
{

///������� ������� ���������� ��������
class BasicTechnique: public Technique
{
  public:
///����������� / ����������
    BasicTechnique  (RenderManager& in_manager, const common::ParseNode& node);
    ~BasicTechnique ();

  protected:
///������ ��� ���������� ������� � ������ �����
    struct PrivateData
    {
      common::PropertyMap frame_properties;             //�������� �����
      common::PropertyMap frame_entity_properties;      //�������� ����-������
      size_t              view_transaction_id;          //������� ���������� ������� ����
      size_t              proj_transaction_id;          //������� ���������� ������� ��������
      size_t              view_proj_transaction_id;     //������� ���������� ������� ���-��������
      math::mat4f         view_tm;                      //������� ����
      math::mat4f         view_proj_tm;                 //������� ���-��������
      size_t              mv_matrix_property_index;     //������ �������� ������� ModelView
      size_t              mvp_matrix_property_index;    //������ �������� ������� ModelViewProjection
      size_t              object_matrix_property_index; //������ �������� ������� ObjectMatrix
      size_t              view_matrix_property_index;   //������ �������� ������� View (� ��������� �����)
      size_t              proj_matrix_property_index;   //������ �������� ������� Projection (� �������� �����)
      manager::Frame      frame;                        //�����

      PrivateData (BasicTechnique& technique)
        : frame (technique.CreateFrame (*this))
        , view_transaction_id ()
        , proj_transaction_id ()
        , view_proj_transaction_id ()
        , view_tm (1.0f)
        , view_proj_tm (1.0f)
        , mv_matrix_property_index (~0u)
        , mvp_matrix_property_index (~0u)
        , object_matrix_property_index (~0u)
        , view_matrix_property_index (~0u)
        , proj_matrix_property_index (~0u)
      {
      }
    };

///��������� ����� ����� �����������
    virtual void ConfigureFrame (RenderingContext& parent_context, PrivateData& private_data);

///��������� �������
    virtual void ConfigureEntity (manager::Frame& frame, manager::Entity& entity, VisualModel& model, PrivateData& private_data, manager::EntityDrawParams& out_params);

///��������� �������
    void DrawVisualModels (RenderingContext& parent_context, PrivateData& private_data);

///���������� �����
    void UpdateFrameCore (RenderingContext& parent_context, TechniquePrivateData& private_data_holder);

///���������� �������
    void UpdatePropertiesCore ();

///���������� ������� ������� � �������� �������
    void BindProperties (common::PropertyBindingMap&);

  private:
/// �������� �����
    manager::Frame CreateFrame (PrivateData& private_data);

///���������� ��������� ��������
    void EntityDrawHandler (manager::Frame& frame, manager::Entity& entity, void* user_data, manager::EntityDrawParams& out_params);
  
  private:
    RenderManager manager;     //�������� ����������
    stl::string   effect_name; //��� �������
};

}

}

}
