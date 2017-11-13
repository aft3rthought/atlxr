
#include "ATLXR/d3d11/library_d3d11.h"

namespace atlxrconfig_namespace
{
	blend_state_type create_blend_state(device_context_type & device_context)
	{
		blend_state_type result;

		D3D11_BLEND_DESC blend_state_descriptor;
		blend_state_descriptor.AlphaToCoverageEnable = false;
		blend_state_descriptor.IndependentBlendEnable = false;
		auto & rt_blend = blend_state_descriptor.RenderTarget[0];
		rt_blend.SrcBlend = D3D11_BLEND_ONE;
		rt_blend.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		rt_blend.BlendOp = D3D11_BLEND_OP_ADD;
		rt_blend.SrcBlendAlpha = D3D11_BLEND_ONE;
		rt_blend.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		rt_blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		rt_blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		auto status = device_context.device->CreateBlendState(&blend_state_descriptor, &result.blend_state);

		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}
}
