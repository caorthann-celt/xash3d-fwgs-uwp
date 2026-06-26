#include <math.h>

#include "common.h"
#include "keydefs.h"
#include "input.h"
#include "client.h"
#include "ref_common.h"
#include "xash_uwp_virtual_mouse.h"

#define XASH_UWP_MENU_MOUSE_AXIS_LEFTX 0
#define XASH_UWP_MENU_MOUSE_AXIS_LEFTY 1
#define XASH_UWP_MENU_MOUSE_BUTTON_A 0
#define XASH_UWP_MENU_MOUSE_DEADZONE 8000.0f
#define XASH_UWP_MENU_MOUSE_SPEED 900.0f

static float s_axis_x;
static float s_axis_y;
static float s_cursor_x;
static float s_cursor_y;
static qboolean s_active;

static float xash_uwp_virtual_mouse_normalize_axis( int value )
{
	float axis = (float)value;
	float magnitude = fabsf( axis );

	if( magnitude <= XASH_UWP_MENU_MOUSE_DEADZONE )
		return 0.0f;

	axis = ( magnitude - XASH_UWP_MENU_MOUSE_DEADZONE ) / ( 32767.0f - XASH_UWP_MENU_MOUSE_DEADZONE );
	return value < 0 ? -axis : axis;
}

qboolean xash_uwp_virtual_mouse_handle_axis( int axis, int value )
{
	if( !UI_IsVisible() )
		return false;

	if( axis == XASH_UWP_MENU_MOUSE_AXIS_LEFTX )
	{
		s_axis_x = xash_uwp_virtual_mouse_normalize_axis( value );
		return true;
	}

	if( axis == XASH_UWP_MENU_MOUSE_AXIS_LEFTY )
	{
		s_axis_y = xash_uwp_virtual_mouse_normalize_axis( value );
		return true;
	}

	return false;
}

void xash_uwp_virtual_mouse_handle_button( int button, int state )
{
	if( UI_IsVisible() && button == XASH_UWP_MENU_MOUSE_BUTTON_A )
		Key_Event( K_MOUSE1, state );
}

void xash_uwp_virtual_mouse_update( void )
{
	int old_x, old_y;
	int new_x, new_y;
	float frametime;

	if( !UI_IsVisible() || refState.width <= 0 || refState.height <= 0 )
	{
		s_active = false;
		return;
	}

	if( !s_active )
	{
		UI_GetCursorPos( &old_x, &old_y );
		s_cursor_x = bound( 0, old_x, refState.width );
		s_cursor_y = bound( 0, old_y, refState.height );
		s_active = true;
	}

	if( !s_axis_x && !s_axis_y )
		return;

	frametime = host.realframetime > 0.0f ? host.realframetime : host.frametime;
	s_cursor_x += s_axis_x * XASH_UWP_MENU_MOUSE_SPEED * frametime;
	s_cursor_y += s_axis_y * XASH_UWP_MENU_MOUSE_SPEED * frametime;

	s_cursor_x = bound( 0.0f, s_cursor_x, (float)refState.width );
	s_cursor_y = bound( 0.0f, s_cursor_y, (float)refState.height );

	UI_GetCursorPos( &old_x, &old_y );
	new_x = (int)s_cursor_x;
	new_y = (int)s_cursor_y;

	if( new_x != old_x || new_y != old_y )
		UI_MouseMove( new_x, new_y );
}
