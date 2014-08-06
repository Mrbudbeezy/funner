#include "shared.h"

namespace render
{

namespace manager
{

void sync_buffers (low_level::IDevice& device, low_level::UsageMode usage_mode, low_level::BindFlag bind_flags, size_t offset, const void* src_data, size_t src_data_size, size_t buffer_capacity, size_t& dst_buffer_size, LowLevelBufferPtr& dst_buffer)
{
  try
  {
    if (dst_buffer_size < buffer_capacity)
    {
      low_level::BufferDesc desc;

      memset (&desc, 0, sizeof desc);

      desc.size         = buffer_capacity;
      desc.bind_flags   = bind_flags;
      desc.access_flags = low_level::AccessFlag_Write;
      desc.usage_mode   = usage_mode;

      LowLevelBufferPtr buffer (device.CreateBuffer (desc), false);

      dst_buffer      = buffer;
      dst_buffer_size = buffer_capacity;
    }

    if (!src_data && src_data_size)
      throw xtl::make_null_argument_exception ("", "src_data");

    if (offset > dst_buffer_size || dst_buffer_size - offset < src_data_size)
      throw xtl::make_range_exception ("", "offset", offset, dst_buffer_size);

    if (!src_data_size)
      return;

    if (src_data)
      dst_buffer->SetData (offset, src_data_size, src_data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::sync_buffers");
    throw;
  }
}

}

}
