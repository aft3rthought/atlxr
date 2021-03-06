
#pragma once

#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/math2d_fwd.h"

#include "ATLXR/projection_2d.h"

namespace atlxrconfig_namespace
{
	struct projection_2d_type
	{
		region_stack_type<box2f> projection_bounds_stack = {{nullptr, nullptr}, nullptr};

		buffer_resource_type constant_buffer;

		bool is_valid() const
		{
			return !projection_bounds_stack.internal_storage.empty() && constant_buffer.is_valid();
		}

		void free()
		{
			constant_buffer.free();
		}
	};
}
