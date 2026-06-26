#include "enginecallback_menu.h"
#include "xash_uwp_menu_cursor.h"

static int xash_uwp_menu_cursor_size( int screen_height )
{
	if( screen_height <= 720 )
		return 24;

	if( screen_height <= 1080 )
		return 32;

	if( screen_height <= 1440 )
		return 42;

	return 64;
}

void xash_uwp_draw_menu_cursor( int x, int y, int screen_height )
{
	static HIMAGE hCursor = 0;
	static bool triedLoad = false;
	const int size = xash_uwp_menu_cursor_size( screen_height );

	if( !triedLoad )
	{
		hCursor = EngFuncs::PIC_Load( "gfx/shell/uwp_cursor" );
		triedLoad = true;
	}

	if( hCursor )
	{
		EngFuncs::PIC_Set( hCursor, 255, 255, 255, 255 );
		EngFuncs::PIC_DrawTrans( x, y, size, size );
		return;
	}

	EngFuncs::FillRGBA( x, y, 2, size * 9 / 16, 255, 255, 255, 255 );
	EngFuncs::FillRGBA( x, y, size * 3 / 8, 2, 255, 255, 255, 255 );
	EngFuncs::FillRGBA( x + 2, y + size * 7 / 16, size / 4, 2, 255, 255, 255, 255 );
	EngFuncs::FillRGBA( x + size * 5 / 16, y + 2, 2, size * 3 / 8, 255, 255, 255, 255 );
	EngFuncs::FillRGBA( x + 2, y + 2, size / 4, size * 3 / 8, 0, 0, 0, 220 );
}
