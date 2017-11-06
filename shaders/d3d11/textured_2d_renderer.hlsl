
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
	float4 color_multiply : COLOR0;
	float4 color_lerp : COLOR1;
};

struct pixel_shader_input
{
	float4 position : SV_POSITION;
	float2 texture_coordinates : TEXCOORD0;
	float4 color_multiply : COLOR0;
	float4 color_lerp : COLOR1;
};

pixel_shader_input vs_main(vertex_shader_input input)
{
	pixel_shader_input output;
	output.position = float4(-1.0 + (input.position.x - c_stage_bounds.x) / c_stage_bounds.z * 2.0,
							 -1.0 + (input.position.y - c_stage_bounds.y) / c_stage_bounds.w * 2.0,
							 0.0,
							 1.0);
	output.texture_coordinates = input.texture_coordinates;
	output.color_multiply = input.color_multiply;
	output.color_lerp = input.color_lerp;
	return output;
}

float4 ps_main(pixel_shader_input input) : SV_TARGET
{
	float4 color = texture0.Sample(texture_sampler, input.texture_coordinates);
	float lerp = input.color_lerp.a;
	float unlerp = 1.0 - lerp;
	color.r = (color.r * unlerp + input.color_lerp.r * lerp) * color.a;
	color.g = (color.g * unlerp + input.color_lerp.g * lerp) * color.a;
	color.b = (color.b * unlerp + input.color_lerp.b * lerp) * color.a;
	color.r = color.r * input.color_multiply.r;
	color.g = color.g * input.color_multiply.g;
	color.b = color.b * input.color_multiply.b;
	color.a = color.a *input.color_multiply.a;
	return color;
}
