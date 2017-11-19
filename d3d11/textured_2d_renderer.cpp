
#include "ATLXR/d3d11/textured_2d_renderer_d3d11.h"
#include "ATLXR/d3d11/projection_2d_d3d11.h"

namespace atlxrconfig_namespace
{
	textured_2d_renderer_type create_textured_2d_renderer(device_context_type & device_context, const textured_2d_renderer_shader_data_type & shader_data)
	{

		// TODO: Vertex shader stuff is D3D specific but the blend & raster are not. Shouldn't all be in a d3d file.
		vertex_shader_input_element_type elements[4] = 
		{
			{"POSITION", 0, data_format_type::float32_rg, offsetof(textured_2d_vertex_type, position)},
			{"TEXCOORD", 0, data_format_type::float32_rg, offsetof(textured_2d_vertex_type, texture_coordinates)},
			{"COLOR", 0, data_format_type::float32_rgba, offsetof(textured_2d_vertex_type, color_multiply)},
			{"COLOR", 1, data_format_type::float32_rgba, offsetof(textured_2d_vertex_type, color_lerp)},
		};

		vertex_shader_input_structure_type input_structure;
		input_structure.main_function = "vs_main";
		input_structure.elements = elements;
		input_structure.element_count = 4;

		textured_2d_renderer_type result;
		result.vertex_shader = create_vertex_shader(device_context, shader_data.data, input_structure);
		result.pixel_shader = create_pixel_shader(device_context, shader_data.data, {"ps_main"});
		result.blend_state = create_blend_state(device_context);
		result.rasterizer_state = create_rasterizer_state(device_context);

		return result;
	}

	void render_textured_2d(device_context_type & device_context,
							textured_2d_renderer_type & textured_2d_renderer,
							projection_2d_type & projection_2d,
							texture_2d_resource_type & texture_2d,
							buffer_resource_type & index_buffer,
							buffer_resource_type & vertex_buffer,
							unsigned int index_count)
	{
		auto & context = device_context.immediate_context3;

		UINT stride = sizeof(textured_2d_vertex_type);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &vertex_buffer.buffer, &stride, &offset);
		context->IASetIndexBuffer(index_buffer.buffer, DXGI_FORMAT_R16_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(textured_2d_renderer.vertex_shader.input_layout);

		context->VSSetShader(textured_2d_renderer.vertex_shader.shader, nullptr, 0);
		context->VSSetConstantBuffers1(0, 1, &projection_2d.constant_buffer.buffer, nullptr, nullptr);

		context->PSSetShader(textured_2d_renderer.pixel_shader.shader, nullptr, 0);
		context->PSSetShaderResources(0, 1, &texture_2d.shader_resource_view);
		context->PSSetSamplers(0, 1, &texture_2d.sampler_state);

		context->RSSetState(textured_2d_renderer.rasterizer_state.rasterizer_state);

		context->OMSetBlendState(textured_2d_renderer.blend_state.blend_state, nullptr, 0xffffffff);

		context->DrawIndexed(index_count, 0, 0);
	}
}
