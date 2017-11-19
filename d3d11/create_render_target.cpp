
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	render_target_type create_render_target_for_texture(device_context_type & device_context,
														const region<viewport_type> & viewport_stack_storage,
														const region<scissor_rect_type> & scissor_rect_stack_storage,
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
				result.viewport_stack = {viewport_stack_storage};
				result.scissor_rect_stack = {scissor_rect_stack_storage};
				result.target_texture = texture_2d_resource.texture;
				result.width = width;
				result.height = height;

				if(!result.viewport_stack.full())
				{
					// Setup the viewport
					D3D11_VIEWPORT viewport;
					viewport.Width = width;
					viewport.Height = height;
					viewport.MinDepth = 0.0f;
					viewport.MaxDepth = 1.0f;
					viewport.TopLeftX = 0;
					viewport.TopLeftY = 0;
					result.viewport_stack.push().viewport = viewport;
				}
			}
		}
		return result;
	}
}

// 82 wide x 56 tall