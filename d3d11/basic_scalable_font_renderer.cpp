
#include "ATLXR/d3d11/basic_scalable_font_renderer_d3d11.h"
#include "ATLXR/d3d11/projection_2d_d3d11.h"

namespace atlxrconfig_namespace
{
	basic_scalable_font_renderer_type create_basic_scalable_font_renderer(device_context_type & device_context, const basic_scalable_font_renderer_shader_data_type & shader_data)
	{
		vertex_shader_input_element_type elements[5] =
		{
			{"POSITION", 0, data_format_type::float32_rg, offsetof(basic_scalable_font_vertex_type, position)},
			{"TEXCOORD", 0, data_format_type::float32_rg, offsetof(basic_scalable_font_vertex_type, texture_coordinates)},
			{"COLOR", 0, data_format_type::float32_rgba, offsetof(basic_scalable_font_vertex_type, color)},
			{"FONT_EDGE_VALUE", 0, data_format_type::float32_r, offsetof(basic_scalable_font_vertex_type, edge_value)},
			{"FONT_RADIUS_VALUE", 0, data_format_type::float32_r, offsetof(basic_scalable_font_vertex_type, radius_value)},
		};

		vertex_shader_input_structure_type input_structure;
		input_structure.main_function = "vs_main";
		input_structure.elements = elements;
		input_structure.element_count = 5;

		basic_scalable_font_renderer_type result;
		result.vertex_shader = create_vertex_shader(device_context, shader_data.data, input_structure);
		result.pixel_shader = create_pixel_shader(device_context, shader_data.data, {"ps_main"});
		result.blend_state = create_blend_state(device_context);
		result.rasterizer_state = create_rasterizer_state(device_context);

		return result;
	}

	void render_basic_scalable_font(device_context_type & device_context,
									basic_scalable_font_renderer_type & basic_scalable_font_renderer,
									projection_2d_type & projection_2d,
									texture_2d_resource_type & texture_2d,
									buffer_resource_type & index_buffer,
									buffer_resource_type & vertex_buffer,
									unsigned int index_count)
	{
		auto & context = device_context.immediate_context3;

		UINT stride = sizeof(basic_scalable_font_vertex_type);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &vertex_buffer.buffer, &stride, &offset);
		context->IASetIndexBuffer(index_buffer.buffer, DXGI_FORMAT_R16_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(basic_scalable_font_renderer.vertex_shader.input_layout);

		context->VSSetShader(basic_scalable_font_renderer.vertex_shader.shader, nullptr, 0);
		context->VSSetConstantBuffers1(0, 1, &projection_2d.constant_buffer.buffer, nullptr, nullptr);

		context->PSSetShader(basic_scalable_font_renderer.pixel_shader.shader, nullptr, 0);
		context->PSSetShaderResources(0, 1, &texture_2d.shader_resource_view);
		context->PSSetSamplers(0, 1, &texture_2d.sampler_state);

		context->RSSetState(basic_scalable_font_renderer.rasterizer_state.rasterizer_state);

		context->OMSetBlendState(basic_scalable_font_renderer.blend_state.blend_state, nullptr, 0xffffffff);

		context->DrawIndexed(index_count, 0, 0);
	}
}
