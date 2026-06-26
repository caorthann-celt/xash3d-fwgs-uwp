#pragma once

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

qboolean xash_uwp_virtual_mouse_handle_axis( int axis, int value );
void xash_uwp_virtual_mouse_handle_button( int button, int state );
void xash_uwp_virtual_mouse_update( void );

#ifdef __cplusplus
}
#endif
