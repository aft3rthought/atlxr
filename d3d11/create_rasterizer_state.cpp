
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	rasterizer_state_type create_rasterizer_state(device_context_type & device_context)
	{
		rasterizer_state_type result;

		D3D11_RASTERIZER_DESC rasterizer_state_descriptor;
		rasterizer_state_descriptor.FillMode = D3D11_FILL_SOLID;
		rasterizer_state_descriptor.CullMode = D3D11_CULL_NONE;
		rasterizer_state_descriptor.FrontCounterClockwise = false;
		rasterizer_state_descriptor.DepthBias = 0;
		rasterizer_state_descriptor.SlopeScaledDepthBias = 0.f;
		rasterizer_state_descriptor.DepthBiasClamp = 0.f;
		rasterizer_state_descriptor.DepthClipEnable = false;
		rasterizer_state_descriptor.ScissorEnable = false;
		rasterizer_state_descriptor.MultisampleEnable = false;
		rasterizer_state_descriptor.AntialiasedLineEnable = false;

		auto status = device_context.device->CreateRasterizerState(&rasterizer_state_descriptor, &result.rasterizer_state);

		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}
}
