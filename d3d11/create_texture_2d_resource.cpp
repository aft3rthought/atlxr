
#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/numeric_casts.h"

namespace atlxrconfig_namespace
{
	HRESULT create_texture_sampler_state_and_shader_resource_view(device_context_type & device_context, texture_2d_resource_type & texture_2d_resource, sampling_mode sampling_mode)
	{
		D3D11_SAMPLER_DESC SamDesc;
		SamDesc.Filter = sampling_mode == sampling_mode::linear ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
		SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		SamDesc.MipLODBias = 0.0f;
		SamDesc.MaxAnisotropy = 1;
		SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
		SamDesc.MinLOD = 0;
		SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
		auto result = device_context.device->CreateSamplerState(&SamDesc, &texture_2d_resource.sampler_state);
		if(FAILED(result))
			return result;

		return device_context.device->CreateShaderResourceView(texture_2d_resource.texture, nullptr, &texture_2d_resource.shader_resource_view);
	}

	texture_2d_resource_type create_texture_2d_resource(device_context_type & device_context,
														lib_unsigned width_pixels,
														lib_unsigned height_pixels,
														bool gpu_write_necessary,
														bool cpu_write_necessary,
														bool cpu_read_necessary,
														bool render_target_necessary,
														data_format_type data_format,
														sampling_mode sampling_mode,
														const region_type<unsigned char> & data)
	{
		texture_2d_resource_type result;

		const auto usage_and_cpu_flags = get_usage_and_cpu_flags_for_requirements(gpu_write_necessary, cpu_write_necessary, cpu_read_necessary);

		D3D11_TEXTURE2D_DESC descriptor;
		descriptor.Width = width_pixels;
		descriptor.Height = height_pixels;
		descriptor.MipLevels = 1;
		descriptor.ArraySize = 1;
		descriptor.Format = data_format_to_dxgi_format(data_format);
		descriptor.SampleDesc.Count = 1;
		descriptor.SampleDesc.Quality = 0;
		descriptor.Usage = usage_and_cpu_flags.usage;
		descriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE | (render_target_necessary ? D3D11_BIND_RENDER_TARGET : 0);
		descriptor.CPUAccessFlags = usage_and_cpu_flags.cpu_flags;
		descriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA texture_data;
		texture_data.pSysMem = data.begin();
		texture_data.SysMemPitch = data_format_size(data_format) * width_pixels;
		texture_data.SysMemSlicePitch = atl::default_int_to_unsigned(atl::default_ptrdiff_to_int(data.size()));

		auto status = device_context.device->CreateTexture2D(&descriptor, &texture_data, &result.texture);
		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		status = create_texture_sampler_state_and_shader_resource_view(device_context, result, sampling_mode);
		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}

	texture_2d_resource_type create_uninitialized_texture_2d_resource(device_context_type & device_context,
																	  lib_unsigned width_pixels,
																	  lib_unsigned height_pixels,
																	  bool gpu_write_necessary,
																	  bool cpu_write_necessary,
																	  bool cpu_read_necessary,
																	  bool render_target_necessary,
																	  data_format_type data_format,
																	  sampling_mode sampling_mode)
	{
		texture_2d_resource_type result;

		const auto usage_and_cpu_flags = get_usage_and_cpu_flags_for_requirements(gpu_write_necessary, cpu_write_necessary, cpu_read_necessary);

		D3D11_TEXTURE2D_DESC descriptor;
		descriptor.Width = width_pixels;
		descriptor.Height = height_pixels;
		descriptor.MipLevels = 1;
		descriptor.ArraySize = 1;
		descriptor.Format = data_format_to_dxgi_format(data_format);
		descriptor.SampleDesc.Count = 1;
		descriptor.SampleDesc.Quality = 0;
		descriptor.Usage = usage_and_cpu_flags.usage;
		descriptor.BindFlags = D3D11_BIND_SHADER_RESOURCE | (render_target_necessary ? D3D11_BIND_RENDER_TARGET : 0);
		descriptor.CPUAccessFlags = usage_and_cpu_flags.cpu_flags;
		descriptor.MiscFlags = 0;

		auto status = device_context.device->CreateTexture2D(&descriptor, nullptr, &result.texture);
		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		status = create_texture_sampler_state_and_shader_resource_view(device_context, result, sampling_mode);
		if(status != S_OK)
		{
			result.free();
			// TODO: Error reporting
		}
		return result;
	}
}
