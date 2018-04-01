
#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/numeric_casts.h"

namespace atlxrconfig_namespace
{
	bool finalize_window_context(device_context_type & device_context, window_context_type & window_context)
	{
		// Get a pointer to the back buffer
		if(FAILED(window_context.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&window_context.render_target.target_texture)))
		{
			window_context.free();
			// TODO: Error reporting
			return false;
		}
		else
		{
			// Create a render-target view
			if(FAILED(device_context.device->CreateRenderTargetView(window_context.render_target.target_texture, nullptr, &window_context.render_target.render_target_view)))
			{
				window_context.free();
				// TODO: Error reporting
				return false;
			}

			if(!window_context.render_target.viewport_stack.full())
			{
				D3D11_VIEWPORT viewport;
				viewport.TopLeftX = 0;
				viewport.TopLeftY = 0;
				viewport.Width = window_context.render_target.width;
				viewport.Height = window_context.render_target.height;
				viewport.MinDepth = 0.f;
				viewport.MaxDepth = 1.f;
				device_context.immediate_context->RSSetViewports(1, &viewport);
				window_context.render_target.viewport_stack.push().viewport = viewport;
			} 
		}
		return true;
	}

	window_context_type create_window_context(device_context_type & device_context,
											  const window_type & window,
											  const region<viewport_type> & viewport_stack_storage,
											  const region<scissor_rect_type> & scissor_rect_stack_storage)
	{
		window_context_type result;
		result.window = window;
		result.render_target.width = window.width;
		result.render_target.height = window.height;
		result.render_target.viewport_stack = {viewport_stack_storage};
		result.render_target.scissor_rect_stack = {scissor_rect_stack_storage};

		DXGI_SWAP_CHAIN_DESC1 swapchain_descriptor;
		ZeroMemory(&swapchain_descriptor, sizeof(swapchain_descriptor));
		swapchain_descriptor.Width = atl::default_float_to_unsigned(window.width);		// Match the size of the window.
		swapchain_descriptor.Height = atl::default_float_to_unsigned(window.height);
		swapchain_descriptor.Format = DXGI_FORMAT_B8G8R8A8_UNORM;				// This is the most common swap chain format.
		swapchain_descriptor.Stereo = false;
		swapchain_descriptor.SampleDesc.Count = 1;								// Don't use multi-sampling.
		swapchain_descriptor.SampleDesc.Quality = 0;
		swapchain_descriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchain_descriptor.BufferCount = 2;									// Use double-buffering to minimize latency.
		swapchain_descriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;	// All Windows Store apps must use this SwapEffect.
		swapchain_descriptor.Flags = 0;
		swapchain_descriptor.Scaling = DXGI_SCALING_STRETCH;
		swapchain_descriptor.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// This sequence obtains the DXGI factory that was used to create the Direct3D device above.
		IDXGIAdapter * dxgiAdapter = nullptr;
		IDXGIFactory4 * dxgiFactory = nullptr;
		IDXGISwapChain1 * dxgiSwapChain = nullptr;

		device_context.dxgiDevice3->GetAdapter(&dxgiAdapter);
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
		if(FAILED(dxgiFactory->CreateSwapChainForCoreWindow(device_context.device,
															reinterpret_cast<IUnknown*>(window.core_window.Get()),
															&swapchain_descriptor,
															nullptr,
															&dxgiSwapChain)))
		{
			result.free();
			// TODO: Error reporting
		}
		else
		{
			result.swap_chain = dxgiSwapChain;
			result.swap_chain1 = dxgiSwapChain;
			finalize_window_context(device_context, result);
		}
		return result;
	}

	update_window_result_type update_window_context(device_context_type & device_context,
													window_context_type & window_context,
													const window_type & window)
	{
		/*
		D3D11_VIEWPORT vp;
		vp.Width = game->graphics.window.width;
		vp.Height = game->graphics.window.height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		game->graphics.device_context.immediate_context->RSSetViewports(1, &vp);
		*game->graphics.main_target_viewport_stack.begin() = {vp};
		*/

		if(!window_context.is_valid())
		{
			window_context.free();
			window_context = create_window_context(device_context,
												   window,
												   window_context.render_target.viewport_stack.internal_storage,
												   window_context.render_target.scissor_rect_stack.internal_storage);
		}
		else if(window.width != window_context.window.width
				|| window.height != window_context.window.height
				|| window.core_window != window_context.window.core_window)
		{
			ID3D11RenderTargetView* null_views[] = {nullptr};
			device_context.immediate_context->OMSetRenderTargets(1, null_views, nullptr);
			if(device_context.immediate_context3)
				device_context.immediate_context3->Flush1(D3D11_CONTEXT_TYPE_ALL, nullptr);

			window_context.render_target.target_texture->Release();
			window_context.render_target.target_texture = nullptr;

			window_context.render_target.render_target_view->Release();
			window_context.render_target.render_target_view = nullptr;

			// If the swap chain already exists, resize it.
			auto hr = window_context.swap_chain->ResizeBuffers(2, // Double-buffered swap chain.
															   atl::default_float_to_unsigned(window.width),
															   atl::default_float_to_unsigned(window.height),
															   DXGI_FORMAT_B8G8R8A8_UNORM,
															   0);

			if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				return update_window_result_type::device_lost;
			else if(FAILED(hr))
				return update_window_result_type::other_failure;

			window_context.render_target.width = window.width;
			window_context.render_target.height = window.height;
			// TODO: This is done here, because finalize pushes onto the stack too. Does it have to??
			if(!window_context.render_target.viewport_stack.empty())
				window_context.render_target.viewport_stack.pop();
			window_context.window = window;
			if(!finalize_window_context(device_context, window_context))
				return update_window_result_type::other_failure;
		}
		return update_window_result_type::success;
	}
}
