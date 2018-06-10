
#pragma once

#include "ATLUtil/math2d.h"
#include "ATLUtil/color.h"

namespace atlxrconfig_namespace
{
	struct textured_2d_renderer_shader_data_type;
	struct projection_2d_type;

	struct textured_2d_vertex_type
	{
		atl::point2f position;
		atl::point2f texture_coordinates;
		atl::color color_multiply;
		atl::color color_lerp;
	};

	struct textured_2d_renderer_type
	{
		vertex_shader_resource_type vertex_shader;
		pixel_shader_resource_type pixel_shader;
		blend_state_type blend_state;
		rasterizer_state_type rasterizer_state;

		bool is_valid() const
		{
			return vertex_shader.is_valid()
				&& pixel_shader.is_valid()
				&& blend_state.is_valid()
				&& rasterizer_state.is_valid();
		}

		void free()
		{
			vertex_shader.free();
			pixel_shader.free();
			blend_state.free();
			rasterizer_state.free();
		}
	};

	textured_2d_renderer_type create_textured_2d_renderer(device_context_type & device_context, const textured_2d_renderer_shader_data_type & shader_data);
	void render_textured_2d(device_context_type & device_context,
							textured_2d_renderer_type & textured_2d_renderer,
							projection_2d_type & projection_2d,
							texture_2d_resource_type & texture_2d,
							buffer_resource_type & index_buffer,
							buffer_resource_type & vertex_buffer,
							unsigned int index_count);
}
