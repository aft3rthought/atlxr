

#pragma once

#include "ATLXR/select_library.h"

#include "ATLUtil/math2d.h"
#include "ATLUtil/region.h"

#include <vector>

namespace atlxrconfig_namespace
{
	using namespace atlxrconfig_atlutil;
	using namespace atlxrconfig_stdlib;

	struct sprite_frame
	{
		texture_2d_resource_type texture;
		box2f texture_coordinates;
		box2f area;

		sprite_frame() {}
		sprite_frame(texture_2d_resource_type in_texture,
					 const box2f & in_texture_coordinates,
					 const box2f & in_area)
		{
			texture = in_texture;
			texture_coordinates = in_texture_coordinates;
			area = in_area;
		}
	};

    struct sprite_sheet
    {
        vector<texture_2d_resource_type> textures;
        vector<sprite_frame> sprites;

        ~sprite_sheet();

        void unpack(device_context_type & api_context, const region_type<unsigned char> & in_sprite_sheet_bytes, const sampling_mode in_sampling_mode);
		void release();
    };
}
