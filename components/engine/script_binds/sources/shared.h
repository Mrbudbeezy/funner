#ifndef SCRIPTBINDS_SHARED_HEADER
#define SCRIPTBINDS_SHARED_HEADER

#include <stl/hash_set>
#include <stl/numeric>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/implicit_cast.h>
#include <xtl/lexical_cast.h>
#include <xtl/shared_ptr.h>
#include <xtl/trackable_ptr.h>

#include <bv/axis_aligned_box.h>

#include <common/async.h>
#include <common/component.h>
#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <math/basic_spline.h>
#include <math/matrix.h>
#include <math/quat.h>
#include <math/vector.h>

#include <physics/manager.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/library_manager.h>

namespace engine
{

void bind_common_file_library  (script::Environment&);
void bind_common_string_tree   (script::Environment&);
void bind_common_signals       (script::Environment&);
void bind_common_action_queue  (script::Environment&);
void bind_common_timer         (script::Environment&);
void bind_common_async_library (script::Environment&);
void bind_math_splines_library (script::Environment&);

}

#endif
