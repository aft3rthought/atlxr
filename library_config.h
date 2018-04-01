
#pragma once

#define atlxrconfig_platform d3d

#define atlxrconfig_namespace atlxr
#define atlxrconfig_atlutil atl
#define atlxrconfig_atlcrossplatform atl
#define atlxrconfig_stdlib std

namespace atlxrconfig_namespace
{
	using lib_int = int;
	using lib_unsigned = unsigned;
}

#define atlxr_allocate atlxr_allocate_impl
#define altxr_free altxr_free_impl
