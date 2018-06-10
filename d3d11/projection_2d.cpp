
#include "ATLXR/d3d11/projection_2d_d3d11.h"

#include "ATLUtil/math2d.h"

namespace atlxrconfig_namespace
{
	projection_2d_type create_projection_2d(device_context_type & device_context, const region_type<box2f> & stack_storage)
	{
		projection_2d_type result;
		result.projection_bounds_stack = region_stack_type<box2f>(stack_storage);
		result.constant_buffer = create_uninitialized_buffer(device_context, true, false, false, buffer_variety_type::uniform_constant, sizeof(box2f));
		return result;
	}

	void projection_2d_update_resource(device_context_type & device_context, projection_2d_type & projection_2d)
	{
		const auto & projection_bounds = projection_2d.projection_bounds_stack.top();
		float projection_bounds_values[4] = {
			projection_bounds.l,
			projection_bounds.b,
			projection_bounds.width(),
			projection_bounds.height()
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
		if(!projection_2d.projection_bounds_stack.full())
		{
			projection_2d.projection_bounds_stack.push() = new_projection_bounds;
			projection_2d_update_resource(device_context, projection_2d);
		}
	}

	void projection_2d_modify_top(device_context_type & device_context, projection_2d_type & projection_2d, const box2f & new_projection_bounds)
	{
		if(!projection_2d.projection_bounds_stack.empty())
		{
			projection_2d.projection_bounds_stack.top() = new_projection_bounds;
			projection_2d_update_resource(device_context, projection_2d);
		}
	}

	void projection_2d_pop(device_context_type & device_context, projection_2d_type & projection_2d)
	{
		if(projection_2d.projection_bounds_stack.size() > 1)
		{
			projection_2d.projection_bounds_stack.pop();
			projection_2d_update_resource(device_context, projection_2d);
		}
	}
}
