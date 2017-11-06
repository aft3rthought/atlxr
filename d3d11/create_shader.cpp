
#include "ATLXR/d3d11/library_d3d11.h"

#include "ATLUtil/array_util.h"

#include <d3dcompiler.h>

namespace atlxrconfig_namespace
{
	ID3DBlob * compile_shader(const char * target, const char * entry_point, const region<unsigned char> & shader_text)
	{
		ID3DBlob * result = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob * error_messages = nullptr;
		auto hr = D3DCompile(shader_text.begin(), shader_text.size(), nullptr, nullptr, nullptr, entry_point, target, flags, 0, &result, &error_messages);
		if(FAILED(hr))
		{
			if(error_messages)
			{
				OutputDebugStringA((char*)error_messages->GetBufferPointer());
				error_messages->Release();
			}

			if(result)
				result->Release();
			result = nullptr;
		}
		return result;
	}

	vertex_shader_resource_type create_vertex_shader(device_context_type & device_context,
													 const region<unsigned char> & shader_text,
													 const vertex_shader_input_structure_type & input_structure)
	{
		vertex_shader_resource_type result;

		const char * target = nullptr;
		switch(device_context.device->GetFeatureLevel())
		{
			default:
				target = "vs_5_0";
				break;
			case D3D_FEATURE_LEVEL_10_1:
			case D3D_FEATURE_LEVEL_10_0:
				target = "vs_4_0";
				break;
			case D3D_FEATURE_LEVEL_9_3:
				target = "vs_4_0_level_9_3";
				break;
			case D3D_FEATURE_LEVEL_9_2:
			case D3D_FEATURE_LEVEL_9_1:
				target = "vs_4_0_level_9_1";
				break;
		}

		ID3DBlob * bytecode_blob = compile_shader(target, input_structure.main_function, shader_text);

		if(bytecode_blob)
		{
			if(FAILED(device_context.device->CreateVertexShader(bytecode_blob->GetBufferPointer(), bytecode_blob->GetBufferSize(), nullptr, &result.shader)))
			{
				result.free();
				// TODO: Error reporting
			}
			else
			{
				auto d3d_vertex_input_descriptors = atlxr_allocate<D3D11_INPUT_ELEMENT_DESC>(input_structure.element_count);
				{
					auto d3d_descriptor = d3d_vertex_input_descriptors;
					auto atlxr_element = input_structure.elements;
					for(; d3d_descriptor < d3d_vertex_input_descriptors + input_structure.element_count; d3d_descriptor++, atlxr_element++)
					{
						d3d_descriptor->AlignedByteOffset = atlxr_element->offset;
						d3d_descriptor->Format = data_format_to_dxgi_format(atlxr_element->data_format);
						d3d_descriptor->InputSlot = 0;
						d3d_descriptor->InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
						d3d_descriptor->InstanceDataStepRate = 0;
						d3d_descriptor->SemanticIndex = atlxr_element->semantic_index;
						d3d_descriptor->SemanticName = atlxr_element->semantic_name;
					}
				}

				if(FAILED(device_context.device->CreateInputLayout(d3d_vertex_input_descriptors,
																   input_structure.element_count,
																   bytecode_blob->GetBufferPointer(),
																   bytecode_blob->GetBufferSize(),
																   &result.input_layout)))
				{
					result.free();
					// TODO: Error reporting
				}
				altxr_free(d3d_vertex_input_descriptors);
			}
			bytecode_blob->Release();
		}
		return result;
	}

	pixel_shader_resource_type create_pixel_shader(device_context_type & device_context,
												   const region<unsigned char> & shader_text,
												   const pixel_shader_input_structure_type & input_structure)
	{
		pixel_shader_resource_type result;

		const char * target = nullptr;
		switch(device_context.device->GetFeatureLevel())
		{
			default:
				target = "ps_5_0";
				break;
			case D3D_FEATURE_LEVEL_10_1:
			case D3D_FEATURE_LEVEL_10_0:
				target = "ps_4_0";
				break;
			case D3D_FEATURE_LEVEL_9_3:
				target = "ps_4_0_level_9_3";
				break;
			case D3D_FEATURE_LEVEL_9_2:
			case D3D_FEATURE_LEVEL_9_1:
				target = "ps_4_0_level_9_1";
				break;
		}

		ID3DBlob * bytecode_blob = compile_shader(target, input_structure.main_function, shader_text);

		if(bytecode_blob)
		{
			if(FAILED(device_context.device->CreatePixelShader(bytecode_blob->GetBufferPointer(), bytecode_blob->GetBufferSize(), nullptr, &result.shader)))
			{
				result.free();
				// TODO: Error reporting 
			}
			bytecode_blob->Release();
		}
		return result;
	}
}
