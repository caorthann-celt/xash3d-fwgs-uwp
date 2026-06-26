#include "xash_uwp_storage_root.h"

#include <direct.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <Windows.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_system.h"

#ifndef XASH_UWP_MAX_PATH
#define XASH_UWP_MAX_PATH 1024
#endif

static bool copy_path( char *dst, size_t dst_size, const char *src )
{
	if( !dst || dst_size == 0 || !src || snprintf( dst, dst_size, "%s", src ) < 0 )
		return false;

	size_t len = strlen( dst );
	while( len > 0 && ( dst[len - 1] == '\\' || dst[len - 1] == '/' ))
		dst[--len] = '\0';

	return dst[0] != '\0';
}

static bool join_path( char *dst, size_t dst_size, const char *root, const char *leaf )
{
	if( !root || !leaf || !root[0] || !leaf[0] )
		return false;

	const char slash = strchr( root, '/' ) ? '/' : '\\';
	const size_t root_len = strlen( root );
	const char *fmt = ( root[root_len - 1] == '\\' || root[root_len - 1] == '/' ) ? "%s%s" : "%s%c%s";

	if( snprintf( dst, dst_size, fmt, root, slash, leaf ) < 0 )
		return false;

	return dst[0] != '\0';
}

static bool ensure_directory( const char *path )
{
	if( !path || !path[0] )
		return false;

	if( _mkdir( path ) == 0 )
		return true;

	return errno == EEXIST;
}

static bool ensure_valve_directory( const char *base_path )
{
	char valve_path[XASH_UWP_MAX_PATH];

	if( !ensure_directory( base_path ))
		return false;

	if( !join_path( valve_path, sizeof( valve_path ), base_path, "valve" ))
		return false;

	return ensure_directory( valve_path );
}

static bool is_writable( const char *root )
{
	char path[XASH_UWP_MAX_PATH];

	if( !join_path( path, sizeof( path ), root, ".uwp-write-test" ))
		return false;

	FILE *file = fopen( path, "wb" );
	if( !file )
		return false;

	fputs( "ok", file );
	fclose( file );
	remove( path );
	return true;
}

static bool try_external_path( char *dst, size_t dst_size )
{
	const char *external_root = "E:\\xash";

	if( !ensure_valve_directory( external_root ))
		return false;

	if( !is_writable( external_root ))
		return false;

	return copy_path( dst, dst_size, external_root );
}

static bool try_local_path( char *dst, size_t dst_size )
{
	char local_root[XASH_UWP_MAX_PATH];
	const char *local_folder = SDL_WinRTGetFSPathUTF8( SDL_WINRT_PATH_LOCAL_FOLDER );

	if( !copy_path( local_root, sizeof( local_root ), local_folder ))
		return false;

	if( !ensure_valve_directory( local_root ))
		return false;

	if( !is_writable( local_root ))
		return false;

	return copy_path( dst, dst_size, local_root );
}

bool xash_uwp_get_base_path( char *dst, size_t dst_size )
{
	if( try_external_path( dst, dst_size ))
		return true;

	return try_local_path( dst, dst_size );
}

bool xash_uwp_get_package_path( char *dst, size_t dst_size )
{
	char *base_path = SDL_GetBasePath();
	bool ok = copy_path( dst, dst_size, base_path );

	if( base_path )
		SDL_free( base_path );

	return ok;
}

bool xash_uwp_get_valve_path( char *dst, size_t dst_size )
{
	char base_path[XASH_UWP_MAX_PATH];

	if( !xash_uwp_get_base_path( base_path, sizeof( base_path )))
		return false;

	return join_path( dst, dst_size, base_path, "valve" );
}

bool xash_uwp_configure_environment( void )
{
	char base_path[XASH_UWP_MAX_PATH];
	char package_path[XASH_UWP_MAX_PATH];

	if( !xash_uwp_get_base_path( base_path, sizeof( base_path )))
		return false;

	if( SDL_setenv( "XASH3D_BASEDIR", base_path, 1 ) != 0 )
		return false;

	SetEnvironmentVariableA( "XASH3D_BASEDIR", base_path );

	if( xash_uwp_get_package_path( package_path, sizeof( package_path )))
	{
		SDL_setenv( "XASH3D_RODIR", package_path, 1 );
		SetEnvironmentVariableA( "XASH3D_RODIR", package_path );
	}

	return true;
}
