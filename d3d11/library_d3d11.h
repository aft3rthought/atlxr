
#pragma once

#include "ATLXR/library_fwd.h"
#include "ATLXR/d3d11/system_headers.h"

namespace atlxrconfig_namespace
{
	template <typename result_type>
	inline result_type * atlxr_allocate_impl(lib_unsigned count)
	{
		return (result_type *)malloc(sizeof(result_type) * count);
	}

	inline void altxr_free_impl(void * pointer)
	{
		free(pointer);
	}

	template <typename pointer_type>
	void free_d3d_resource(pointer_type *& ptr)
	{
		if(ptr) ptr->Release();
		ptr = nullptr;
	}

	struct device_context_type
	{
		ID3D11Device * device = nullptr;
		ID3D11DeviceContext * immediate_context = nullptr;

		IDXGIDevice3 * dxgiDevice3 = nullptr;
		ID3D11Device3 * device3 = nullptr;
		ID3D11DeviceContext3 * immediate_context3 = nullptr;

		bool is_valid() const
		{
			return device != nullptr && immediate_context != nullptr;
		}

		void free()
		{
			free_d3d_resource(device);
			free_d3d_resource(immediate_context);
		}
	};

	struct viewport_type
	{
		D3D11_VIEWPORT viewport;
	};

	struct scissor_rect_type 
	{
		D3D11_RECT scissor_rect;
	};

	struct render_target_type
	{
		float width = 0.f;
		float height = 0.f;
		region_stack_type<viewport_type> viewport_stack = {{nullptr, nullptr}, nullptr};
		region_stack_type<scissor_rect_type> scissor_rect_stack = {{nullptr, nullptr}, nullptr};
		ID3D11RenderTargetView * render_target_view = nullptr;
		ID3D11Texture2D * target_texture = nullptr;

		bool is_valid() const
		{
			return render_target_view != nullptr && target_texture != nullptr;
		}

		void free()
		{
			free_d3d_resource(render_target_view);
			free_d3d_resource(target_texture);
		}
	};

	struct window_type
	{
		Platform::Agile<Windows::UI::Core::CoreWindow> core_window;
		float width = 0;
		float height = 0;
		float dpi = 0;
		lib_unsigned orientation = 0;
	};

	struct window_context_type
	{
		IDXGISwapChain * swap_chain = nullptr;
		window_type window;
		render_target_type render_target;

		IDXGISwapChain1 * swap_chain1 = nullptr;

		bool is_valid() const
		{
			return swap_chain != nullptr && render_target.is_valid();
		}

		void free()
		{
			free_d3d_resource(swap_chain);
			render_target.free();
		}
	};

	struct blend_state_type
	{
		ID3D11BlendState * blend_state = nullptr;

		bool is_valid() const
		{
			return blend_state != nullptr;
		}

		void free()
		{
			free_d3d_resource(blend_state);
		}
	};

	struct rasterizer_state_type
	{
		ID3D11RasterizerState * rasterizer_state = nullptr;

		bool is_valid() const
		{
			return rasterizer_state != nullptr;
		}

		void free()
		{
			free_d3d_resource(rasterizer_state);
		}
	};
	
	struct buffer_resource_type
	{
		ID3D11Buffer * buffer = nullptr;

		bool is_valid() const
		{
			return buffer != nullptr;
		}

		void free()
		{
			free_d3d_resource(buffer);
		}
	};

	struct texture_2d_resource_type
	{
		ID3D11Texture2D * texture = nullptr;
		ID3D11SamplerState * sampler_state = nullptr;
		ID3D11ShaderResourceView * shader_resource_view = nullptr;

		bool is_valid() const
		{
			return texture != nullptr && sampler_state != nullptr && shader_resource_view != nullptr;
		}

		void free()
		{
			free_d3d_resource(texture);
			free_d3d_resource(sampler_state);
		}
	};

	struct pixel_shader_resource_type
	{
		ID3D11PixelShader * shader = nullptr;

		bool is_valid() const
		{
			return shader != nullptr;
		}

		void free()
		{
			free_d3d_resource(shader);
		}
	};

	struct vertex_shader_resource_type
	{
		ID3D11VertexShader * shader = nullptr;
		ID3D11InputLayout * input_layout = nullptr;

		bool is_valid() const
		{
			return shader != nullptr && input_layout != nullptr;
		}

		void free()
		{
			free_d3d_resource(shader);
			free_d3d_resource(input_layout);
		}
	};

	inline DXGI_FORMAT data_format_to_dxgi_format(data_format_type format)
	{
		switch(format)
		{
			case data_format_type::unorm8_r: return DXGI_FORMAT_R8_UNORM;
			case data_format_type::unorm8_rg: return DXGI_FORMAT_R8G8_UNORM;
			case data_format_type::unorm8_rgba: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case data_format_type::float32_r: return DXGI_FORMAT_R32_FLOAT;
			case data_format_type::float32_rg: return DXGI_FORMAT_R32G32_FLOAT;
			case data_format_type::float32_rgb: return DXGI_FORMAT_R32G32B32_FLOAT;
			case data_format_type::float32_rgba: return DXGI_FORMAT_R32G32B32A32_FLOAT;

			// unsupported:
			case data_format_type::unorm8_rgb: return DXGI_FORMAT_UNKNOWN;
		}
		return DXGI_FORMAT_UNKNOWN;
	}

	struct usage_and_cpu_flags_type
	{
		D3D11_USAGE usage;
		lib_unsigned cpu_flags;
	};

	inline usage_and_cpu_flags_type get_usage_and_cpu_flags_for_requirements(bool gpu_write_necessary,
																	  bool cpu_write_necessary,
																	  bool cpu_read_necessary)
	{
		if(gpu_write_necessary && !cpu_read_necessary && !cpu_write_necessary)
			return {D3D11_USAGE_DEFAULT, 0};
		else if(cpu_write_necessary && !gpu_write_necessary && !cpu_read_necessary)
			return {D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE};
		else if(cpu_write_necessary || gpu_write_necessary || cpu_read_necessary)
			return {D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE};
		return {D3D11_USAGE_IMMUTABLE, 0};
	}

	struct vertex_shader_input_element_type
	{
		const char * semantic_name;
		lib_unsigned semantic_index;
		data_format_type data_format;
		lib_unsigned offset;
	};

	struct vertex_shader_input_structure_type
	{
		vertex_shader_input_element_type * elements;
		lib_unsigned element_count;
		const char * main_function;
	};

	struct pixel_shader_input_structure_type
	{
		const char * main_function;
	};
}
