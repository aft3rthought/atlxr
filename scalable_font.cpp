
#include "ATLXR/scalable_font.h"
#include "ATLXR/lodepng.h"

#include "ATLUtil/numeric_util.h"
#include "ATLUtil/bit_string.h"

namespace atlxrconfig_namespace
{
    scalable_font::~scalable_font()
    {
		release();
    }

	void scalable_font::release()
	{
		for(auto & texture : textures)
			texture.free();
		textures.clear();
		characters.clear();
	}

	void scalable_font::unpack(device_context_type & api_context, const region<unsigned char> & in_sprite_sheet_bytes)
	{
		input_bit_string_type bit_string(in_sprite_sheet_bytes.begin(), in_sprite_sheet_bytes.end());

		struct l_CharInfo
		{
			uint32_t m_char;
			float m_advance;
			point2f m_offset;
			size2f m_size;
			uint32_t m_sheetXPos;
			uint32_t m_sheetYPos;
			uint32_t m_sheetWidth;
			uint32_t m_sheetHeight;
		};

		bit_string_read_value(bit_string, field_bloom);

		int fontSheetWidthPower, fontSheetHeightPower;
		bit_string_read_ranged_integer<int>(bit_string, fontSheetWidthPower, 0, 12);
		bit_string_read_ranged_integer<int>(bit_string, fontSheetHeightPower, 0, 12);
		uint32_t fontSheetWidth = 1 << fontSheetWidthPower;
		uint32_t fontSheetHeight = 1 << fontSheetHeightPower;

		int l_numCharacters;
		bit_string_read_chunked_integer<int>(bit_string, l_numCharacters, 7);

		std::vector<l_CharInfo> l_charInfos;
		l_charInfos.reserve(l_numCharacters);

		float l_minBoundsY = std::numeric_limits<float>::max();
		float l_maxBoundsY = -std::numeric_limits<float>::max();
		while(l_numCharacters-- > 0)
		{
			l_charInfos.emplace_back();
			bit_string_read_chunked_integer<uint32_t>(bit_string, l_charInfos.back().m_char, 8);
			bit_string_read_value(bit_string, l_charInfos.back().m_advance);
			bit_string_read_value(bit_string, l_charInfos.back().m_offset.x);
			bit_string_read_value(bit_string, l_charInfos.back().m_offset.y);
			bit_string_read_value(bit_string, l_charInfos.back().m_size.w);
			bit_string_read_value(bit_string, l_charInfos.back().m_size.h);
			bit_string_read_ranged_integer<uint32_t>(bit_string, l_charInfos.back().m_sheetXPos, 0, fontSheetWidth);
			bit_string_read_ranged_integer<uint32_t>(bit_string, l_charInfos.back().m_sheetYPos, 0, fontSheetHeight);
			bit_string_read_ranged_integer<uint32_t>(bit_string, l_charInfos.back().m_sheetWidth, 0, fontSheetWidth);
			bit_string_read_ranged_integer<uint32_t>(bit_string, l_charInfos.back().m_sheetHeight, 0, fontSheetHeight);

			l_minBoundsY = std::min(l_minBoundsY, l_charInfos.back().m_offset.y);
			l_maxBoundsY = std::max(l_maxBoundsY, l_charInfos.back().m_offset.y + l_charInfos.back().m_size.h);
		}

		int32_t l_pngSizeInt32;
		bit_string_read_value(bit_string, l_pngSizeInt32);
		size_t l_pngSize = l_pngSizeInt32;
		bit_string_skip_to_next_byte(bit_string);
		const unsigned char * l_pngData = bit_string.ptr;
		bit_string.ptr += l_pngSize;

		// Read png:
		unsigned char * l_fontSheetData;
		unsigned int l_fontSheetWidth, l_fontSheetHeight;
		auto error = lodepng_decode_memory(&l_fontSheetData,
										   &l_fontSheetWidth,
										   &l_fontSheetHeight,
										   l_pngData,
										   l_pngSize,
										   LCT_GREY,
										   8);

		if(error == 0)
		{
			// Create font entry:
			character_y_extents.max = l_maxBoundsY;
			character_y_extents.min = l_minBoundsY;
			pixel_texel_stride = point2f(1.f / l_fontSheetWidth, 1.f / l_fontSheetHeight);

			// Upload font sheet to video card:
			textures.emplace_back(create_texture_2d_resource(api_context,
															 l_fontSheetWidth,
															 l_fontSheetHeight,
															 false,
															 false,
															 false,
															 false,
															 data_format_type::unorm8_r,
															 sampling_mode::linear,
															 {l_fontSheetData, l_fontSheetData + l_fontSheetWidth * l_fontSheetHeight}));

			// Fill out all the bookkeeping data to render properly:
			for(const auto & l_info : l_charInfos)
			{
				characters.emplace_back(l_info.m_char,
										l_info.m_advance,
										l_info.m_offset,
										l_info.m_size,
										box2f(float(l_info.m_sheetYPos) / float(l_fontSheetHeight),
											  float(l_info.m_sheetXPos + l_info.m_sheetWidth) / float(l_fontSheetWidth),
											  float(l_info.m_sheetYPos + l_info.m_sheetHeight) / float(l_fontSheetHeight),
											  float(l_info.m_sheetXPos) / float(l_fontSheetWidth)));
			}

			// Free image data
			free(l_fontSheetData);
		}
    }
}
