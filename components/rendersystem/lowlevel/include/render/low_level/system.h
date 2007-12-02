#ifndef RENDER_LOW_LEVEL_SYSTEM_HEADER
#define RENDER_LOW_LEVEL_SYSTEM_HEADER

#include <render/low_level/driver.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� �������������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterDriver   (const char* name, IDriver* driver);
    static void UnregisterDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� �������� �� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IDriver* FindDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ISwapChain* CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc);
    static ISwapChain* CreateSwapChain (const char* driver_mask, const char* output_mask, const SwapChainDesc& swap_chain_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ���������� ���������
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool CreateSwapChainAndDevice (const char*          driver_mask,     //����� ����� ��������
                                          const SwapChainDesc& swap_chain_desc, //���������� ������� ������
                                          const char*          init_string,     //������ �������������
                                          ISwapChain*&         out_swap_chain,  //�������������� ������� ������
                                          IDevice*&            out_device);     //�������������� ���������� ���������
    static bool CreateSwapChainAndDevice (const char*          driver_mask,     //����� ����� ��������
                                          const char*          output_mask,     //����� ����� ���������� ������
                                          const SwapChainDesc& swap_chain_desc, //���������� ������� ������
                                          const char*          init_string,     //������ �������������
                                          ISwapChain*&         out_swap_chain,  //�������������� ������� ������
                                          IDevice*&            out_device);     //�������������� ���������� ���������                                          
};

}

}

#endif
