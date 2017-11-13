
#include "ATLXR/d3d11/projection_2d_d3d11.h"

#include "ATLUtil/math2d.h"

namespace atlxrconfig_namespace
{
	projection_2d_type create_projection_2d(device_context_type & device_context, const region<box2f> & stack_storage)
	{
		projection_2d_type result;
		result.projection_bounds_stack = stack_storage;
		result.constant_buffer = create_uninitialized_buffer(device_context, true, false, false, buffer_variety_type::uniform_constant, sizeof(box2f));
		return result;
	}

	void projection_2d_update_resource(device_context_type & device_context, projection_2d_type & projection_2d)
	{
		float projection_bounds_values[4] = {
			projection_2d.projection_bounds_head->l,
			projection_2d.projection_bounds_head->b,
			projection_2d.projection_bounds_head->width(),
			projection_2d.projection_bounds_head->height()
		};
		device_context.immediate_context3->UpdateSubresource1(projection_2d.constant_buffer.buffer,
															  0,
															  nullptr,
															  &projection_bounds_values,
															  0,
															  0,
															  0);
	}

	void projection_2d_push(device_context_type & device_context, projection_2d_type & projection_2d, const box2f & new_projection_bounds)
	{
		if(projection_2d.projection_bounds_head == nullptr)
			projection_2d.projection_bounds_head = projection_2d.projection_bounds_stack.begin();
		else
			projection_2d.projection_bounds_head++;

		*projection_2d.projection_bounds_head = new_projection_bounds;

		projection_2d_update_resource(device_context, projection_2d);
	}

	void projection_2d_pop(device_context_type & device_context, projection_2d_type & projection_2d)
	{
		if(projection_2d.projection_bounds_head > projection_2d.projection_bounds_stack.begin() - 1)
		{
			projection_2d.projection_bounds_head--;
			projection_2d_update_resource(device_context, projection_2d);
		}
	}
}
