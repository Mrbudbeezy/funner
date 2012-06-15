#ifndef SCENE_GRAPH_SCREEN_SHARED_HEADER
#define SCENE_GRAPH_SCREEN_SHARED_HEADER

#include <stl/vector>
#include <stl/string>
#include <stl/hash_map>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/iterator.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>
#include <xtl/stat_counter.h>

#include <common/property_map.h>

#include <sg/screen.h>

namespace scene_graph
{

const size_t DEFAULT_SCREEN_WIDTH  = 100; //�ਭ� �࠭� �� 㬮�砭��
const size_t DEFAULT_SCREEN_HEIGHT = 100; //���� �࠭� �� 㬮�砭��

}

#endif
