#ifndef SCRIPTBINDS_SHARED_HEADER
#define SCRIPTBINDS_SHARED_HEADER

#include <stl/hash_set>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/implicit_cast.h>
#include <xtl/lexical_cast.h>
#include <xtl/shared_ptr.h>

#include <bv/axis_aligned_box.h>

#include <common/component.h>
#include <common/console.h>
#include <common/file.h>
#include <common/log.h>
#include <common/property_map.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <script/bind.h>
#include <script/environment.h>
#include <script/library_manager.h>

namespace engine
{

void bind_common_file_library (script::Environment&);
void bind_common_string_tree  (script::Environment&);
void bind_common_signals      (script::Environment&);
void bind_common_action_queue (script::Environment&);
void bind_common_timer        (script::Environment&);
void bind_common_property_map (script::Environment&);

}

#endif
