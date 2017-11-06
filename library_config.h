
#pragma once

#define atlxrconfig_platform d3d

#define atlxrconfig_namespace atlxr
#define atlxrconfig_atlutil atl
#define atlxrconfig_atlcrossplatform atl
#define atlxrconfig_stdlib std

using atlxr_sint = long;
using atlxr_uint = size_t;

#define atlxr_allocate atlxr_allocate_impl
#define altxr_free altxr_free_impl
