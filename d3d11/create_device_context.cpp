
#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/array_util.h"
#include "ATLUtil/numeric_casts.h"

namespace atlxrconfig_namespace
{
	device_context_type create_device_context()
	{
		device_context_type result;

		const D3D_FEATURE_LEVEL all_d3d_feature_levels_11_1[] = {
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		const D3D_FEATURE_LEVEL all_d3d_feature_levels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		HRESULT hr = D3D11CreateDevice(nullptr,
									   D3D_DRIVER_TYPE_HARDWARE,
									   nullptr,
									   createDeviceFlags,
									   all_d3d_feature_levels_11_1,
									   atl::default_size_t_to_unsigned(c_array_len(all_d3d_feature_levels_11_1)),
									   D3D11_SDK_VERSION,
									   &result.device,
									   nullptr,
									   &result.immediate_context);
		if(hr == E_INVALIDARG)
		{
			hr = D3D11CreateDevice(nullptr,
								   D3D_DRIVER_TYPE_HARDWARE,
								   nullptr, createDeviceFlags,
								   all_d3d_feature_levels,
								   atl::default_size_t_to_unsigned(c_array_len(all_d3d_feature_levels)),
								   D3D11_SDK_VERSION,
								   &result.device,
								   nullptr,
								   &result.immediate_context);
		}

		if(FAILED(hr))
		{
			result.free();
		}
		else
		{
			result.device->QueryInterface(__uuidof(IDXGIDevice3), (void**)&result.dxgiDevice3);
			result.device->QueryInterface(__uuidof(ID3D11Device3), (void**)&result.device3);
			result.immediate_context->QueryInterface(__uuidof(ID3D11DeviceContext3), (void**)&result.immediate_context3);
		}
		return result;
	}
}
