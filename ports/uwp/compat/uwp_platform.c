#include "platform/platform.h"

void Win32_Init( qboolean con_showalways )
{
	(void)con_showalways;
}

void Win32_Shutdown( void )
{
}

qboolean Win32_NanoSleep( int nsec )
{
	Platform_Sleep( Q_max( 1, nsec / 1000000 ) );
	return true;
}

qboolean Platform_DebuggerPresent( void )
{
	return false;
}

void Platform_ShellExecute( const char *path, const char *parms )
{
	(void)path;
	(void)parms;
}

void Platform_SetStatus( const char *status )
{
	(void)status;
}

void Wcon_CreateConsole( qboolean con_showalways )
{
	(void)con_showalways;
}

void Wcon_DestroyConsole( void )
{
}

void Wcon_InitConsoleCommands( void )
{
}

void Wcon_ShowConsole( qboolean show )
{
	(void)show;
}

void Wcon_DisableInput( void )
{
}

char *Wcon_Input( void )
{
	return NULL;
}

void Wcon_WinPrint( const char *pMsg )
{
	OutputDebugStringA( pMsg );
}

void Sys_SetupCrashHandler( const char *argv0 )
{
	(void)argv0;
}

void Sys_RestoreCrashHandler( void )
{
}
