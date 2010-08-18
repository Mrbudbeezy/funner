#ifndef COMMONLIB_LICENSE_MANAGER_HEADER
#define COMMONLIB_LICENSE_MANAGER_HEADER

#include <cstddef>

namespace common
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
class LicenseManager
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ��������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void Load (const char* license_path);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static int DaysToExpiration ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ����������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool IsComponentAllowed (const char* component_name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static const char* GetProperty     (const char* property_name);
    static const char* GetProperty     (size_t property_index);
    static const char* GetPropertyName (size_t property_index);
    static bool        HasProperty     (const char* property_name);
    static size_t      PropertiesCount ();
};

}

#endif
