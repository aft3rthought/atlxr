

#include "ATLXR/sprite_sheet.h"
#include "ATLXR/lodepng.h"

#include "ATLUtil/bit_string.h"
#include "ATLUtil/numeric_casts.h"

namespace atlxrconfig_namespace
{
	sprite_sheet::~sprite_sheet()
	{
		release();
	}

	void sprite_sheet::release()
	{
		for(auto & texture : textures)
			texture.free();
		textures.clear();
		sprites.clear();
	}

    void sprite_sheet::unpack(device_context_type & api_context, const region<unsigned char> & in_sprite_sheet_bytes, const sampling_mode in_sampling_mode)
    {
		input_bit_string_type bit_string(in_sprite_sheet_bytes.begin(), in_sprite_sheet_bytes.end());

		// Load frames:
		uint32_t numSheets;
		atl::bit_string_read_chunked_integer<uint32_t>(bit_string, numSheets, 3);
		uint32_t totalSprites;
		atl::bit_string_read_chunked_integer<uint32_t>(bit_string, totalSprites, 7);

		sprites.resize(totalSprites);
		textures.resize(numSheets);

		vector<size_t> sheet_sprites;

		for(auto & texture : textures)
		{
			uint32_t fontSheetWidthPower, fontSheetHeightPower;
			atl::bit_string_read_ranged_integer<uint32_t>(bit_string, fontSheetWidthPower, 0, 12);
			atl::bit_string_read_ranged_integer<uint32_t>(bit_string, fontSheetHeightPower, 0, 12);
			uint32_t sheetWidth = 1 << fontSheetWidthPower;
			uint32_t sheetHeight = 1 << fontSheetHeightPower;

			atl::size2f sheetSize(atl::default_u32_to_float(sheetWidth), atl::default_u32_to_float(sheetHeight));

			sheet_sprites.clear();

			uint32_t sheetSprites;
			atl::bit_string_read_ranged_integer<uint32_t>(bit_string, sheetSprites, 0, totalSprites);
			while(sheetSprites-- > 0)
			{
				// Deserialize the frame index:
				uint32_t spriteIndex;
				atl::bit_string_read_ranged_integer<uint32_t>(bit_string, spriteIndex, 0, totalSprites - 1);

				// Deserialize the texture coordinates:
				atl::box2f l_texCoords;
				{
					uint32_t l_texT, l_texR, l_texB, l_texL;
					atl::bit_string_read_ranged_integer<uint32_t>(bit_string, l_texT, 0, sheetHeight);
					atl::bit_string_read_ranged_integer<uint32_t>(bit_string, l_texR, 0, sheetWidth);
					atl::bit_string_read_ranged_integer<uint32_t>(bit_string, l_texB, 0, sheetHeight);
					atl::bit_string_read_ranged_integer<uint32_t>(bit_string, l_texL, 0, sheetWidth);

					if(l_texR - l_texL == 1)
					{
						l_texCoords.l = l_texCoords.r = (float(l_texL) + 0.5f) / float(sheetWidth);
					}
					else
					{
						l_texCoords.l = (float(l_texL)) / float(sheetWidth);
						l_texCoords.r = (float(l_texR)) / float(sheetWidth);
					}

					if(l_texB - l_texT == 1)
					{
						l_texCoords.t = l_texCoords.b = (float(l_texT) + 0.5f) / float(sheetHeight);
					}
					else
					{
						l_texCoords.t = (float(l_texT)) / float(sheetHeight);
						l_texCoords.b = (float(l_texB)) / float(sheetHeight);
					}
				}

				// Deserialize the drawn area:
				atl::box2f l_area;
				atl::bit_string_read_values(bit_string, l_area.t, l_area.r, l_area.b, l_area.l);

				sprites[spriteIndex].texture_coordinates = l_texCoords;
				sprites[spriteIndex].area = l_area;
				sheet_sprites.emplace_back(spriteIndex);
			}

			// Decode PNG data and create graphics resource:
			int32_t numPNGBytesInt32;
			atl::bit_string_read_value(bit_string, numPNGBytesInt32);
			size_t numPNGBytes = numPNGBytesInt32;
			atl::bit_string_skip_to_next_byte(bit_string);
			const unsigned char * l_pngBytes = bit_string.ptr;
			bit_string.ptr += numPNGBytes;

			unsigned char * imageDataOut = nullptr;
			unsigned int imageWidthOut;
			unsigned int imageHeightOut;
			unsigned int decodeError = lodepng_decode32(&imageDataOut, &imageWidthOut, &imageHeightOut, l_pngBytes, numPNGBytes);
			if(decodeError <= 0)
			{
				if(imageWidthOut == sheetWidth && imageHeightOut == sheetHeight)
				{
					// Allocate GL resource:
					texture = create_texture_2d_resource(api_context,
														 sheetWidth,
														 sheetHeight,
														 false,
														 false,
														 false,
														 false,
														 data_format_type::unorm8_rgba,
														 in_sampling_mode,
														 {imageDataOut, imageDataOut + imageWidthOut * imageHeightOut * 4});
					for(auto idx : sheet_sprites)
						sprites[idx].texture = texture;
				}
				delete[] imageDataOut;
			}
		}
    }
}
