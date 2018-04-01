
#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/numeric_casts.h"

namespace atlxrconfig_namespace
{
	void set_active_render_target(device_context_type & device_context, render_target_type & render_target)
	{
		if(render_target.viewport_stack.empty())
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
			device_context.immediate_context->RSSetViewports(1, &render_target.viewport_stack.top().viewport);
		}
		if(render_target.scissor_rect_stack.empty())
		{
			D3D11_RECT fallback_scissor_rect;
			fallback_scissor_rect.top = 0;
			fallback_scissor_rect.right = LONG{atl::default_float_to_long(render_target.width)};
			fallback_scissor_rect.bottom = LONG{atl::default_float_to_long(render_target.height)};
			fallback_scissor_rect.left = 0;
			device_context.immediate_context->RSSetScissorRects(1, &fallback_scissor_rect);
		}
		else
		{
			device_context.immediate_context->RSSetScissorRects(1, &render_target.scissor_rect_stack.top().scissor_rect);
		}
		// TODO: Depth stencil view
		device_context.immediate_context->OMSetRenderTargets(1, &render_target.render_target_view, nullptr);// render_target.depth_stencil_view);

		if(device_context.dxgiDevice3)
			device_context.dxgiDevice3->SetMaximumFrameLatency(1);
	}

	void clear_render_target(device_context_type & device_context, render_target_type & render_target, const color & color)
	{
		// TODO: Depth stencil view
		device_context.immediate_context->ClearRenderTargetView(render_target.render_target_view, (float*)&color);
		//device_context.immediate_context->ClearDepthStencilView(render_target.depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
}
