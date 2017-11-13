
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	render_target_type create_render_target_for_texture(device_context_type & device_context,
														const region<view_scissor_bounds> & viewport_stack_storage,
														const region<view_scissor_bounds> & scissor_rect_stack_storage,
														texture_2d_resource_type & texture_2d_resource,
														atlxr_uint width,
														atlxr_uint height)
	{
		render_target_type result;

		if(texture_2d_resource.is_valid())
		{
			if(FAILED(device_context.device->CreateRenderTargetView(texture_2d_resource.texture, nullptr, &result.render_target_view)))
			{
				result.free();
				// TODO: Error reporting
			}
			else
			{
				result.viewport_stack_head = viewport_stack_storage.begin();
				result.viewport_stack_storage = viewport_stack_storage;
				result.scissor_rect_stack_head = scissor_rect_stack_storage.begin();
				result.scissor_rect_stack_storage = scissor_rect_stack_storage;
				result.target_texture = texture_2d_resource.texture;
				result.width = width;
				result.height = height;

				// Setup the viewport
				D3D11_VIEWPORT viewport;
				viewport.Width = width;
				viewport.Height = height;
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				viewport.TopLeftX = 0;
				viewport.TopLeftY = 0;
				result.viewport_stack_head->viewport = viewport;
			}
		}
		return result;
	}
}

// 82 wide x 56 tall