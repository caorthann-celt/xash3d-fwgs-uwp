#include <Windows.h>

#include "SDL2/SDL.h"
#include "compat/xash_uwp_storage_root.h"

using HostMainFn = int( * )( int argc, char **argv, const char *progname, int bChangeGame, void *changeGame );

static void DebugLine( const char *message )
{
	OutputDebugStringA( "xash3d-uwp: " );
	OutputDebugStringA( message );
	OutputDebugStringA( "\n" );
}

static int RunXash( int, char ** )
{
	if( !xash_uwp_configure_environment() )
	{
		DebugLine( "failed to configure storage root" );
		return 1;
	}

	char valvePath[MAX_PATH * 4];
	if( xash_uwp_get_valve_path( valvePath, sizeof( valvePath ) ) )
	{
		OutputDebugStringA( "xash3d-uwp: valve path: " );
		OutputDebugStringA( valvePath );
		OutputDebugStringA( "\n" );
	}

	HMODULE engine = LoadPackagedLibrary( L"xash.dll", 0 );
	if( !engine )
	{
		DebugLine( "failed to load xash.dll" );
		return 1;
	}

	auto hostMain = reinterpret_cast<HostMainFn>( GetProcAddress( engine, "Host_Main" ) );
	if( !hostMain )
	{
		DebugLine( "xash.dll does not export Host_Main" );
		return 1;
	}

	char appName[] = "xash3d-uwp";
	char gameSwitch[] = "-game";
	char gameName[] = "valve";
	char refSwitch[] = "-ref";
	char refName[] = XASH_UWP_DEFAULT_REF;
	char consoleSwitch[] = "-console";
	char *args[] = { appName, gameSwitch, gameName, refSwitch, refName, consoleSwitch };

	return hostMain( static_cast<int>( sizeof( args ) / sizeof( args[0] ) ), args, gameName, 0, nullptr );
}

int CALLBACK WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	return SDL_WinRTRunApp( RunXash, nullptr );
}
