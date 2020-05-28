
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	lib_int get_bind_flags_for_buffer_variety(buffer_variety_type buffer_variety)
	{
		switch(buffer_variety)
		{
			case buffer_variety_type::vertex: return D3D11_BIND_VERTEX_BUFFER;
			case buffer_variety_type::index: return D3D11_BIND_INDEX_BUFFER;
			case buffer_variety_type::uniform_constant: return D3D11_BIND_CONSTANT_BUFFER;
		}
		return lib_int{0};
	}

	D3D11_BUFFER_DESC create_buffer_descriptor(bool gpu_write_necessary,
											   bool cpu_write_necessary,
											   bool cpu_read_necessary,
											   buffer_variety_type buffer_variety,
											   lib_unsigned size)
	{
		const auto usage_and_cpu_flags = get_usage_and_cpu_flags_for_requirements(gpu_write_necessary, cpu_write_necessary, cpu_read_necessary);

		D3D11_BUFFER_DESC descriptor;
		descriptor.BindFlags = get_bind_flags_for_buffer_variety(buffer_variety);
		descriptor.ByteWidth = size;
		descriptor.CPUAccessFlags = usage_and_cpu_flags.cpu_flags;
		descriptor.MiscFlags = 0;
		descriptor.StructureByteStride = 0;
		descriptor.Usage = usage_and_cpu_flags.usage;

		return descriptor;
	}

	buffer_resource_type create_buffer(device_context_type & device_context,
									   bool gpu_write_necessary,
									   bool cpu_write_necessary,
									   bool cpu_read_necessary,
									   buffer_variety_type buffer_variety,
									   const region_type<unsigned char> & data)
	{
		buffer_resource_type result;

		auto descriptor = create_buffer_descriptor(gpu_write_necessary,
												   cpu_write_necessary,
												   cpu_read_necessary,
												   buffer_variety,
												   data.size());

		D3D11_SUBRESOURCE_DATA buffer_data;
		buffer_data.pSysMem = data.begin();
		buffer_data.SysMemPitch = 0;
		buffer_data.SysMemSlicePitch = 0;

		auto status = device_context.device->CreateBuffer(&descriptor, &buffer_data, &result.buffer);

		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}

	buffer_resource_type create_uninitialized_buffer(device_context_type & device_context,
													 bool gpu_write_necessary,
													 bool cpu_write_necessary,
													 bool cpu_read_necessary,
													 buffer_variety_type buffer_variety,
													 lib_unsigned size)
	{
		buffer_resource_type result;

		constexpr lib_unsigned resource_alignment_size = 16;
		auto descriptor = create_buffer_descriptor(gpu_write_necessary,
												   cpu_write_necessary,
												   cpu_read_necessary,
												   buffer_variety,
												   atl::integer_divide_rounding_up(size, resource_alignment_size) * resource_alignment_size);

		auto status = device_context.device->CreateBuffer(&descriptor, nullptr, &result.buffer);

		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}
}
