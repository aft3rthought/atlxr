
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	void set_active_render_target(device_context_type & device_context, render_target_type & render_target)
	{
		if(render_target.viewport_stack_head == nullptr)
		{
			D3D11_VIEWPORT fallback_viewport;
			fallback_viewport.TopLeftX = 0;
			fallback_viewport.TopLeftY = 0;
			fallback_viewport.Width = render_target.width;
			fallback_viewport.Height = render_target.height;
			fallback_viewport.MinDepth = 0.f;
			fallback_viewport.MaxDepth = 1.f;
			device_context.immediate_context->RSSetViewports(1, &fallback_viewport);
		}
		else
		{
			device_context.immediate_context->RSSetViewports(1, &render_target.viewport_stack_head->viewport);
		}
		// TODO: Scissor
		device_context.immediate_context->OMSetRenderTargets(1, &render_target.render_target_view, nullptr);// render_target.depth_stencil_view);

	}

	void clear_render_target(device_context_type & device_context, render_target_type & render_target, const color & color)
	{
		// Clear the back buffer and depth stencil view.
		device_context.immediate_context->ClearRenderTargetView(render_target.render_target_view, (float*)&color);
		//device_context.immediate_context->ClearDepthStencilView(render_target.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}
