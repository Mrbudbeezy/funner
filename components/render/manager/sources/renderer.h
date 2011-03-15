struct RendererEntity;
struct RendererFrame;
struct RendererPrimitive;

///�������� ������ ��� ����������
struct RendererFrame
{
    //������� / ��������????
  IStateBlock*   state_block;  //���� ��������� �����
  size_t         passes_count; //���������� ��������
  RendererPass** passes;       //�������
};

///�������� �������
struct RendererPass
{
  IStateBlock*        state_block;      //���� ��������� �������
  size_t              primitives_count; //���������� ���������� �������
  RendererPrimitive** primitives;       //��������� �������
  size_t              passes_count;     //���������� ��������� ��������
  RendererPass**      passes;           //��������� �������
};

///�������� ���������
struct RendererPrimitive
{
  IStateBlock*                     material_state_block; //���� ��������� ��������� ���������
  IStateBlock*                     entity_state_block;   //���� ��������� ������� ���������
  render::low_level::PrimitiveType type;                 //��� ���������
  size_t                           first;                //������ ������ �������/�������
  size_t                           count;                //���������� ����������
};

///��������� �������
void draw (IDevice& device, RendererPass& pass);

