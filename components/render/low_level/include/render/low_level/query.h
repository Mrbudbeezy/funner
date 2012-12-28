#ifndef RENDER_LOW_LEVEL_QUERY_HEADER
#define RENDER_LOW_LEVEL_QUERY_HEADER

#include <render/low_level/object.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
enum QueryType
{
  QueryType_Event,                   //�������
  QueryType_Occlusion,               //���������� ������������ ��������
  QueryType_OcclusionPredicate,      //��� �� ��������� ���� �� ���� �������
  QueryType_PipelineStatistics,      //���������� ���������
  QueryType_StreamOutput0Statistics, //���������� ���������� ������ ������ � ����� 0
  QueryType_StreamOutput1Statistics, //���������� ���������� ������ ������ � ����� 1
  QueryType_StreamOutput2Statistics, //���������� ���������� ������ ������ � ����� 2
  QueryType_StreamOutput3Statistics, //���������� ���������� ������ ������ � ����� 3
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct PipelineStatisticsQueryDesc
{
  size_t state_changes;       //���������� ������������ ���������
  size_t draw_calls;          //���������� ������� Draw
  size_t draw_indexed_calls;  //���������� ������� DrawIndexed
  size_t primitives_count;    //���������� ����������
};

typedef size_t OcclusionQueryDesc;
typedef bool   OcclusionPredicateQueryDesc;
typedef bool   EventQueryDesc;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����������� ���������� ������ ������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct StreamOutputStatisticsQueryDesc
{
  UInt64 written_primitives_count;           //���������� ���������� ����������
  UInt64 required_storage_primitives_count;  //��������� ��� ������ ���������� ����������
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ��������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IQuery: virtual public IObject
{
  public:    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� �������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    virtual QueryType GetType () = 0;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////  
    virtual void GetData    (size_t size, void* data) = 0;
    virtual bool TryGetData (size_t size, void* data) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class IPredicate: virtual public IQuery {};

}

}

#endif
