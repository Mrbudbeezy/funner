#include "shared.h"

int main ()
{
  printf ("Results of depth_stencil_state_test1:\n");
  
  try
  {
    Test test (L"OpenGL device test window (depth_stencil_state1)");
    
    dump_desc (*test.device->GetImmediateContext ()->OSGetDepthStencilState ());    
    
    DepthStencilDesc desc;
    
    desc.depth_test_enable   = false;
    desc.depth_write_enable  = false;
    desc.stencil_test_enable = true;
    desc.depth_compare_mode  = CompareMode_GreaterEqual;
    desc.stencil_read_mask   = 0xfe;
    desc.stencil_write_mask  = 0x17;
    
    StencilDesc &front_stencil = desc.stencil_desc [FaceMode_Front],
                &back_stencil  = desc.stencil_desc [FaceMode_Back];
                
    front_stencil.stencil_compare_mode   = CompareMode_AlwaysFail;
    front_stencil.stencil_fail_operation = StencilOperation_Increment;
    front_stencil.depth_fail_operation   = StencilOperation_Decrement;
    front_stencil.stencil_pass_operation = StencilOperation_Invert; 
    
    back_stencil.stencil_compare_mode   = CompareMode_NotEqual;
    back_stencil.stencil_fail_operation = StencilOperation_Zero;
    back_stencil.depth_fail_operation   = StencilOperation_Replace;
    back_stencil.stencil_pass_operation = StencilOperation_Keep;
    
    DepthStencilStatePtr state (test.device->CreateDepthStencilState (desc), false);
    
    test.device->GetImmediateContext ()->OSSetDepthStencilState (state.get ());
    
    dump_desc (*test.device->GetImmediateContext ()->OSGetDepthStencilState ());
    
    state = 0;
    
    dump_desc (*test.device->GetImmediateContext ()->OSGetDepthStencilState ());    
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }  

  return 0;
}
