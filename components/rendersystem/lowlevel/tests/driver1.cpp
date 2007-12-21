#include "shared.h"

int main ()
{
  printf ("Results of driver1_test:\n");
  
  try
  {
    xtl::com_ptr<IDriver> driver (new TestDriver, false);
    
    DriverManager::RegisterDriver ("test_drv", get_pointer (driver));
    
    SwapChainDesc swap_chain_desc; //�� ���樠���������, ��᪮��� �� �㤥� �ᯮ�짮��� � ���쭥�襬    
    
    IDevice*    device     = 0;
    ISwapChain* swap_chain = 0;
    
    DriverManager::CreateSwapChainAndDevice ("*_drv", swap_chain_desc, "", swap_chain, device);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
