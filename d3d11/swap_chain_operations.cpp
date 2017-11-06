
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	update_window_result_type present_swap_chain(device_context_type & device_context, window_context_type & window_context)
	{
		HRESULT hr;
		if(window_context.swap_chain1)
		{
			DXGI_PRESENT_PARAMETERS parameters = {0};
			hr = window_context.swap_chain1->Present1(1, 0, &parameters);
		}
		else
		{
			hr = window_context.swap_chain->Present(1, 0);
		}

		if(device_context.immediate_context3)
		{
			device_context.immediate_context3->DiscardView1(window_context.render_target.render_target_view, nullptr, 0);
//			m_d3dContext->DiscardView1(m_d3dDepthStencilView.Get(), nullptr, 0);
		}

		if(hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			return update_window_result_type::device_lost;
		else if(FAILED(hr))
			return update_window_result_type::other_failure;
		return update_window_result_type::success;
	}
}
