#include "shared.h"

namespace
{

class VisualModelTest: public VisualModel
{
  public:
    static Pointer Create () { return Pointer (new VisualModelTest, false); }
};

}

int main ()
{
  printf ("Results of visual_model_test:\n");
  
  VisualModel::Pointer model (VisualModelTest::Create ());

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  {
    Scissor::Pointer scissor (Scissor::Create ());

    model->SetScissor (scissor.get (), NodeBindMode_WeakRef);

    printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");
  }

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  {
    Scissor::Pointer scissor (Scissor::Create ());

    model->SetScissor (scissor.get (), NodeBindMode_AddRef);
  }

  printf ("scissor is %s\n", model->Scissor () ? "not null" : "null");

  return 0;
}
