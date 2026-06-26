# Xash3D FWGS UWP build notes

## Install The Tools

Install the basic command line tools:

```powershell
winget install --id Microsoft.VisualStudio.2022.Community -e
winget install --id Kitware.CMake -e
winget install --id Python.Python.3.12 -e
winget install --id Git.Git -e
```

Open Visual Studio Installer and make sure these workloads/components are installed:

```text
Universal Windows Platform development
Desktop development with C++
C++ CMake tools for Windows
Windows 10 SDK 10.0.19041.0 or newer
```

If you prefer doing that from PowerShell, run this and fill in your Visual Studio install path:

```powershell
& "<Visual Studio Installer>\vs_installer.exe" modify `
  --installPath "<VS install path>" `
  --add Microsoft.VisualStudio.Workload.Universal `
  --add Microsoft.VisualStudio.Workload.NativeDesktop `
  --add Microsoft.VisualStudio.Component.VC.CMake.Project `
  --add Microsoft.VisualStudio.Component.Windows10SDK.19041 `
  --includeRecommended `
  --passive `
  --norestart
```

## Get The Source

Keep the engine and game SDK beside each other:

```text
<workspace>
  xash3d-fwgs
  hlsdk-portable
```

Clone them if you have not already:

```powershell
mkdir <workspace>
cd <workspace>
git clone https://github.com/FWGS/xash3d-fwgs.git
git clone https://github.com/FWGS/hlsdk-portable.git
```

Update submodules:

```powershell
git -C <workspace>\xash3d-fwgs submodule update --init --recursive
git -C <workspace>\hlsdk-portable submodule update --init --recursive
```

## Get The UWP Deps

Use `worleydl/uwp-dep` for the UWP SDL2 and Mesa runtime:

```powershell
git clone https://github.com/worleydl/uwp-dep.git <uwp-dep-root>
```

Then point the build at that checkout:

```powershell
$env:XASH_UWP_DEPS_ROOT = "<uwp-dep-root>"
$env:XASH_UWP_SDL2_ROOT = "<uwp-dep-root>\x64"
```

That folder should contain:

```text
x64\bin\SDL2.dll
x64\bin\opengl32.dll
x64\bin\libgallium_wgl.dll
x64\bin\libuwp.dll
x64\bin\dxil.dll
x64\bin\z-1.dll
x64\lib\SDL2.lib
x64\include\SDL2
```

## Open The UWP Dev Prompt

Use a Visual Studio developer prompt for UWP x64, or run:

```powershell
& "<VS>\VC\Auxiliary\Build\vcvarsall.bat" amd64 uwp 10.0.19041.0
```

A newer installed SDK is ok too.

## Build hlsdk-portable

Build the x64 game DLLs:

```powershell
cd <workspace>\hlsdk-portable
cmake -S . -B build-x64 -A x64 -D64BIT=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-x64 --config Release
```

The package step looks for these outputs:

```text
build-x64\dlls\Release\hl_amd64.dll
build-x64\dlls\Release\hl.dll
build-x64\cl_dll\Release\client_amd64.dll
build-x64\cl_dll\Release\client.dll
```

## Build The Engine

Build and install the UWP engine DLLs:

```powershell
cd <workspace>\xash3d-fwgs
.\waf.bat configure -o build-uwp-engine -8 -T release --uwp --sdl2 "$env:XASH_UWP_SDL2_ROOT" --disable-werror --enable-bundled-deps
.\waf.bat build -o build-uwp-engine
.\waf.bat install -o build-uwp-engine --destdir <engine-install-root>
```

The two important switches are `-8` and `--uwp`. `-8` makes the engine x64, and `--uwp` keeps it on the UWP path.

## Build The Package

Point the package project at the engine and game DLL outputs:

```powershell
$env:XASH_UWP_ENGINE_ROOT = "<engine-install-root>"
$env:XASH_UWP_HLSDK_ROOT = "<workspace>\hlsdk-portable\build-x64"
```

Configure and build the MSIX:

```powershell
cmake -S <workspace>\xash3d-fwgs\ports\uwp `
  -B <workspace>\build-uwp-shim `
  -A x64

cmake --build <workspace>\build-uwp-shim `
  --config Release `
  --target xash3d-uwp
```

The package lands here:

```text
<workspace>\build-uwp-shim\AppPackages\xash3d-uwp
```

## Copy Game Files

Copy your Steam Half-Life `valve` folder to the UWP game-data root:

```text
E:\xash\valve
```

If external storage is not available, the app falls back to LocalState:

```text
LocalState\valve
```

Do not use this layout:

```text
LocalState\xash\valve
```
