
Texture2D texture0 : register(t0);

SamplerState texture_sampler : register(s0);

cbuffer stage_bounds_constant_buffer : register(b0)
{
	float4 c_stage_bounds;
};

struct vertex_shader_input
{
	float2 position : POSITION0;
	float2 texture_coordinates : TEXCOORD0;
	float4 color : COLOR0;
	float edge_value : FONT_EDGE_VALUE;
	float radius_value : FONT_RADIUS_VALUE;
};

struct pixel_shader_input
{
	float4 position : SV_POSITION;
	float2 texture_coordinates : TEXCOORD0;
	float4 color : COLOR0;
	float edge_value : FONT_EDGE_VALUE;
	float radius_value : FONT_RADIUS_VALUE;
};

pixel_shader_input vs_main(vertex_shader_input input)
{
	pixel_shader_input output;
	output.position = float4(-1.0 + (input.position.x - c_stage_bounds.x) / c_stage_bounds.z * 2.0,
							 -1.0 + (input.position.y - c_stage_bounds.y) / c_stage_bounds.w * 2.0,
							 0.0,
							 1.0);
	output.texture_coordinates = input.texture_coordinates;
	output.color = input.color;
	output.edge_value = input.edge_value;
	output.radius_value = input.radius_value;
	return output;
}

float4 ps_main(pixel_shader_input input) : SV_TARGET
{
	float df_sample = texture0.Sample(texture_sampler, input.texture_coordinates).r;
	float df_alpha = clamp((df_sample - input.edge_value) / input.radius_value, 0.0, 1.0);
	return input.color * df_alpha;
}
