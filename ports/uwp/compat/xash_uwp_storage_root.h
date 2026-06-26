#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool xash_uwp_get_base_path( char *dst, size_t dst_size );
bool xash_uwp_get_package_path( char *dst, size_t dst_size );
bool xash_uwp_get_valve_path( char *dst, size_t dst_size );
bool xash_uwp_configure_environment( void );

#ifdef __cplusplus
}
#endif
