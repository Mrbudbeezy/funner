#include "shared.h"

#include <common/file.h>
#include <common/strlib.h>

using namespace script;
using namespace common;
using namespace xtl;

namespace
{

/*
    ��������� (����� ���������)
*/

const char* COMMON_ASYNC_RESULT_LIBRARY = "Common.AsyncResult";

/*
    ����������� ���������
*/


}

namespace engine
{

void bind_common_async_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (COMMON_ASYNC_RESULT_LIBRARY);

  lib.Register ("get_Completed",  make_invoker (&AsyncResult::IsCompleted));
  lib.Register ("WaitCompleted",  make_invoker (&AsyncResult::WaitCompleted));

  environment.RegisterType<AsyncResult> (COMMON_ASYNC_RESULT_LIBRARY);  
}

}
