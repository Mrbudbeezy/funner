#ifndef XTL_TYPE_INFO_DECL_HEADER
#define XTL_TYPE_INFO_DECL_HEADER

#include <typeinfo>

namespace xtl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ ���������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class type_info
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������� � ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const std::type_info& std_type () const = 0;
    virtual const char*           name     () const = 0;
    virtual bool                  before   (const type_info&) const = 0;
    virtual bool                  before   (const std::type_info&) const = 0;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� ���� � ������� ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool is_void () const = 0;                     //T �������� ����� void
    virtual bool is_integral () const = 0;                 //T �������� ������������� �����
    virtual bool is_floating_point () const = 0;           //T �������� ������������ �����
    virtual bool is_array () const = 0;                    //T �������� ��������
    virtual bool is_pointer () const = 0;                  //T �������� ����������
    virtual bool is_reference () const = 0;                //T �������� �������
    virtual bool is_member_object_pointer () const = 0;    //T �������� ���������� �� ���� ������
    virtual bool is_member_function_pointer () const = 0;  //T �������� ���������� �� �������
    virtual bool is_enum () const = 0;                     //T �������� �������������
    virtual bool is_union () const = 0;                    //T �������� ������������
    virtual bool is_class () const = 0;                    //T �������� �������
    virtual bool is_function () const = 0;                 //T �������� ��������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������������� ���� � ������������ ���������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool is_arithmetic () const = 0;      //T �������� �������������� ����� 
    virtual bool is_fundamental () const = 0;     //T �������� ��������������� ����� 
    virtual bool is_object () const = 0;          //T �������� ��������� ����� (�� ������, void ��� �������)
    virtual bool is_scalar () const = 0;          //T �������� ��������� ����� ������ (is_arithmetic, enum, pointer, member_pointer)
    virtual bool is_compound () const = 0;        //T �������� �� ��������������� �����
    virtual bool is_member_pointer () const = 0;  //T �������� ���������� �� ������� ��� ���� ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual bool is_const () const = 0;                 //T ����� ������������ const
    virtual bool is_volatile () const = 0;              //T ����� ������������ volatile
    virtual bool is_pod () const = 0;                   //T �������� POD-�����
    virtual bool is_empty () const = 0;                 //T �������� ������ �������
    virtual bool is_polymorphic () const = 0;           //T �������� ����������� ����� (�������� ����������� �������)
    virtual bool is_abstract () const = 0;              //T �������� ����������� ����� (�������� ���� �� ���� ����� ����������� �������)
    virtual bool has_trivial_constructor () const = 0;  //T ����� ����������� ����������� �� ���������
    virtual bool has_trivial_copy () const = 0;         //T ����� ����������� ����������� �����������
    virtual bool has_trivial_assign () const = 0;       //T ����� ����������� �������� ������������
    virtual bool has_trivial_destructor () const = 0;   //T ����� ����������� ����������
    virtual bool has_nothrow_constructor () const = 0;  //T ����� ���������� ����������� �� ��������� (�� ����������� ����������)
    virtual bool has_nothrow_copy () const = 0;         //T ����� ���������� ����������� ����������� (�� ����������� ����������)
    virtual bool has_nothrow_assign () const = 0;       //T ����� ���������� �������� ������������ (�� ����������� ����������)
    virtual bool has_virtual_destructor () const = 0;   //T ����� ����������� ����������
    virtual bool is_signed () const = 0;                //T �������� �������� �����
    virtual bool is_unsigned () const = 0;              //T �������� ����������� �����
    virtual unsigned int alignment_of () const = 0;     //����������� ������������ ���� ������
    virtual unsigned int rank () const = 0;             //����������� ����� ������� (������ a [4][5][6] ����� ���� 3)
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ������������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const type_info& remove_const () const = 0;    //�������� ������������� const
    virtual const type_info& remove_volatile () const = 0; //�������� ������������� volatile
    virtual const type_info& remove_cv () const = 0;       //�������� ������������� const volatile
    virtual const type_info& add_const () const = 0;       //���������� ������������� const
    virtual const type_info& add_volatile () const = 0;    //���������� ������������� volatile
    virtual const type_info& add_cv () const = 0;          //���������� ������������� const volatile

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ��������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const type_info& remove_reference () const = 0; //�������� ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const type_info& remove_extent () const = 0;      //�������� ������� ������ ����������� �������

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual const type_info& remove_pointer () const = 0;  //�������� ���������
  
  protected:
    virtual ~type_info () {}
};

}

#endif
