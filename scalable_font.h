
#pragma once

#include "ATLXR/select_library.h"

#include "ATLUtil/math2d.h"
#include "ATLUtil/region.h"

#include <vector>

namespace atlxrconfig_namespace
{
	using namespace atlxrconfig_atlutil;
	using namespace atlxrconfig_stdlib;

    struct scalable_font_character_type
    {
    public:
        uint64_t code_point;
        float advance;
        point2f offset;
        size2f size;
        box2f texture_coordinates;

		scalable_font_character_type(uint64_t in_code_point,
									 float in_advance,
									 const point2f & in_offset,
									 const size2f & in_size,
									 const box2f & in_texture_coordinates)
		{
			code_point = in_code_point;
			advance = in_advance;
			offset = in_offset;
			size = in_size;
			texture_coordinates = in_texture_coordinates;
		}
    };

    struct scalable_font
    {
    public:
		vector<texture_2d_resource_type> textures;
		vector<scalable_font_character_type> characters;
		point2f pixel_texel_stride;
		rangef character_y_extents;
		float field_bloom;

        ~scalable_font();

        void unpack(device_context_type & api_context, const region<unsigned char> & in_sprite_sheet_bytes);
		void release();
    };
}
