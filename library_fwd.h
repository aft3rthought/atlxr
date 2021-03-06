
#pragma once

#include "ATLXR/library_config.h"

#include "ATLUtil/region_stack.h"
#include "ATLUtil/color.h"

namespace atlxrconfig_namespace
{
	using namespace atlxrconfig_atlutil;

	constexpr lib_unsigned enum_mask_unorm  = 0b0000000000000001;
	constexpr lib_unsigned enum_mask_snorm  = 0b0000000000000010;
	constexpr lib_unsigned enum_mask_uint   = 0b0000000000000100;
	constexpr lib_unsigned enum_mask_sint   = 0b0000000000001000;
	constexpr lib_unsigned enum_mask_float  = 0b0000000000010000;
	constexpr lib_unsigned enum_mask_size8  = 0b0000000000100000;
	constexpr lib_unsigned enum_mask_size16 = 0b0000000001000000;
	constexpr lib_unsigned enum_mask_size32 = 0b0000000010000000;
	constexpr lib_unsigned enum_mask_count1 = 0b0000000100000000;
	constexpr lib_unsigned enum_mask_count2 = 0b0000001000000000;
	constexpr lib_unsigned enum_mask_count3 = 0b0000010000000000;
	constexpr lib_unsigned enum_mask_count4 = 0b0000100000000000;

	enum class data_format_type : lib_unsigned
	{
		invalid = 0,
		unorm8_r = enum_mask_unorm | enum_mask_size8 | enum_mask_count1,
		unorm8_rg = enum_mask_unorm | enum_mask_size8 | enum_mask_count2,
		unorm8_rgb = enum_mask_unorm | enum_mask_size8 | enum_mask_count3,
		unorm8_rgba = enum_mask_unorm | enum_mask_size8 | enum_mask_count4,
		float32_r = enum_mask_float | enum_mask_size32 | enum_mask_count1,
		float32_rg = enum_mask_float | enum_mask_size32 | enum_mask_count2,
		float32_rgb = enum_mask_float | enum_mask_size32 | enum_mask_count3,
		float32_rgba = enum_mask_float | enum_mask_size32 | enum_mask_count4,
	};

	inline lib_unsigned data_format_size(data_format_type data_format)
	{
		switch(data_format)
		{
			case data_format_type::unorm8_r: return 1;
			case data_format_type::unorm8_rg: return 2;
			case data_format_type::unorm8_rgb: return 3;
			case data_format_type::unorm8_rgba: return 4;
			case data_format_type::float32_r: return 4;
			case data_format_type::float32_rg: return 8;
			case data_format_type::float32_rgb: return 12;
			case data_format_type::float32_rgba: return 16;
		}
		return 1;
	}

	struct viewport_type;
	struct scissor_rect_type;
	struct render_target_type;
	struct window_type;
	struct device_context_type;
	struct window_context_type;
	struct blend_state_type;
	struct rasterizer_state_type;
	struct buffer_resource_type;
	struct texture_2d_resource_type;
	struct pixel_shader_resource_type;
	struct vertex_shader_resource_type;
	struct vertex_shader_input_element_type;
	struct vertex_shader_input_structure_type;
	struct pixel_shader_input_structure_type;

	enum class buffer_variety_type : lib_int
	{
		vertex,
		index,
		uniform_constant,
	};

	enum class sampling_mode : lib_int
	{
		linear,
		nearest_neighbor,
	};

	device_context_type create_device_context();

	window_context_type create_window_context(device_context_type & device_context,
											  const window_type & window,
											  const region_type<viewport_type> & viewport_stack_storage,
											  const region_type<scissor_rect_type> & scissor_rect_stack_storage);

	render_target_type create_render_target_for_texture(device_context_type & device_context,
														const region_type<viewport_type> & viewport_stack_storage,
														const region_type<scissor_rect_type> & scissor_rect_stack_storage,
														texture_2d_resource_type & texture_2d_resource,
														lib_unsigned width,
														lib_unsigned height);

	blend_state_type create_blend_state(device_context_type & device_context);

	rasterizer_state_type create_rasterizer_state(device_context_type & device_context);

	buffer_resource_type create_buffer(device_context_type & device_context,
									   bool gpu_write_necessary,
									   bool cpu_write_necessary,
									   bool cpu_read_necessary,
									   buffer_variety_type buffer_variety,
									   const region_type<unsigned char> & data);

	buffer_resource_type create_uninitialized_buffer(device_context_type & device_context,
													 bool gpu_write_necessary,
													 bool cpu_write_necessary,
													 bool cpu_read_necessary,
													 buffer_variety_type buffer_variety,
													 lib_unsigned size);

	vertex_shader_resource_type create_vertex_shader(device_context_type & device_context,
													 const region_type<unsigned char> & shader_text,
													 const vertex_shader_input_structure_type & input_structure);

	pixel_shader_resource_type create_pixel_shader(device_context_type & device_context,
												   const region_type<unsigned char> & shader_text,
												   const pixel_shader_input_structure_type & input_structure);

	texture_2d_resource_type create_texture_2d_resource(device_context_type & device_context,
														lib_unsigned width_pixels,
														lib_unsigned height_pixels,
														bool gpu_write_necessary,
														bool cpu_write_necessary,
														bool cpu_read_necessary,
														bool render_target_necessary,
														data_format_type data_format,
														sampling_mode sampling_mode,
														const region_type<unsigned char> & data);

	texture_2d_resource_type create_uninitialized_texture_2d_resource(device_context_type & device_context,
																	  lib_unsigned width_pixels,
																	  lib_unsigned height_pixels,
																	  bool gpu_write_necessary,
																	  bool cpu_write_necessary,
																	  bool cpu_read_necessary,
																	  bool render_target_necessary,
																	  data_format_type data_format,
																	  sampling_mode sampling_mode);

	enum class update_window_result_type
	{
		device_lost,
		other_failure,
		success
	};

	update_window_result_type update_window_context(device_context_type & device_context,
													window_context_type & window_context,
													const window_type & window);

	update_window_result_type present_swap_chain(device_context_type & device_context, window_context_type & window_context);

	void set_active_render_target(device_context_type & device_context,
								  render_target_type & render_target);

	void clear_render_target(device_context_type & device_context,
							 render_target_type & render_target,
							 const color & color);
}
