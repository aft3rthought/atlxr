
#include "ATLXR/d3d11/library_d3d11.h"

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
			else
			{
				// Bind the view
				device_context.immediate_context->OMSetRenderTargets(1, &window_context.render_target.render_target_view, nullptr);

				// Setup the viewport
				D3D11_VIEWPORT vp;
				vp.Width = window_context.window.width;
				vp.Height = window_context.window.height;
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				device_context.immediate_context->RSSetViewports(1, &vp);

				if(device_context.dxgiDevice3)
					device_context.dxgiDevice3->SetMaximumFrameLatency(1);
			}
		}
		return true;
	}

	window_context_type create_window_context(device_context_type & device_context,
											  const window_type & window,
											  const region<view_scissor_bounds> & viewport_stack_storage,
											  const region<view_scissor_bounds> & scissor_rect_stack_storage)
	{
		window_context_type result;
		result.window = window;
		result.render_target.width = window.width;
		result.render_target.height = window.height;
		result.render_target.viewport_stack_storage = viewport_stack_storage;
		result.render_target.scissor_rect_stack_storage = scissor_rect_stack_storage;
		result.render_target.viewport_stack_head = viewport_stack_storage.begin();
		result.render_target.scissor_rect_stack_head = scissor_rect_stack_storage.begin();

		DXGI_SWAP_CHAIN_DESC1 swapchain_descriptor;
		ZeroMemory(&swapchain_descriptor, sizeof(swapchain_descriptor));
		swapchain_descriptor.Width = window.width;		// Match the size of the window.
		swapchain_descriptor.Height = window.height;
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
		if(!window_context.is_valid())
		{
			window_context.free();
			window_context = create_window_context(device_context,
												   window,
												   window_context.render_target.viewport_stack_storage,
												   window_context.render_target.scissor_rect_stack_storage);
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
															   window.width,
															   window.height,
															   DXGI_FORMAT_B8G8R8A8_UNORM,
															   0);

			if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				return update_window_result_type::device_lost;
			else if(FAILED(hr))
				return update_window_result_type::other_failure;

			if(!finalize_window_context(device_context, window_context))
				return update_window_result_type::other_failure;
		}
		return update_window_result_type::success;
	}
}
