
#pragma once

namespace atlxrconfig_namespace
{
	struct projection_2d_type;

	projection_2d_type create_projection_2d(device_context_type & device_context, const region<box2f> & stack_storage);
	void projection_2d_push(device_context_type & device_context, projection_2d_type & projection_2d, const box2f & new_projection_bounds);
	void projection_2d_pop(device_context_type & device_context, projection_2d_type & projection_2d);
}
