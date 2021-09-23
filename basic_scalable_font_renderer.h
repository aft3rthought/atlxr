
#pragma once

#include "ATLUtil/math2d.h"
#include "ATLUtil/color.h"

namespace atlxrconfig_namespace
{
	struct basic_scalable_font_renderer_shader_data_type;
	struct projection_2d_type;

	struct basic_scalable_font_vertex_type
	{
		point2f position;
		point2f texture_coordinates;
		color_premul color;
		float edge_value;
		float radius_value;

		basic_scalable_font_vertex_type() : color(0.f, 0.f, 0.f, 0.f) {}
	};

	struct basic_scalable_font_renderer_type
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

	basic_scalable_font_renderer_type create_basic_scalable_font_renderer(device_context_type & device_context, const basic_scalable_font_renderer_shader_data_type & shader_data);
	void render_basic_scalable_font(device_context_type & device_context,
									basic_scalable_font_renderer_type & basic_scalable_font_renderer,
									projection_2d_type & projection_2d,
									texture_2d_resource_type & texture_2d,
									buffer_resource_type & index_buffer,
									buffer_resource_type & vertex_buffer,
									unsigned int index_count);
}
