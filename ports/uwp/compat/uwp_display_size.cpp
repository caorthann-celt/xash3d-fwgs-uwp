#include "uwp_display_size.h"

#include <algorithm>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <gamingdeviceinformation.h>

#include <winrt/Windows.Graphics.Display.Core.h>

using winrt::Windows::Graphics::Display::Core::HdmiDisplayInformation;

static int s_cached_width = 0;
static int s_cached_height = 0;
static int s_has_cached_size = 0;

static bool get_hdmi_size(int* width, int* height)
{
    try {
        HdmiDisplayInformation display = HdmiDisplayInformation::GetForCurrentView();
        auto mode = display.GetCurrentDisplayMode();

        const int raw_width = mode.ResolutionWidthInRawPixels();
        const int raw_height = mode.ResolutionHeightInRawPixels();
        if (raw_width <= 0 || raw_height <= 0) {
            return false;
        }

        *width = raw_width;
        *height = raw_height;
        return true;
    } catch (...) {
        return false;
    }
}

static void get_xbox_render_cap(int* width, int* height)
{
    GAMING_DEVICE_MODEL_INFORMATION info = {};
    if (FAILED(GetGamingDeviceModelInformation(&info))) {
        return;
    }

    switch (info.deviceId) {
        case GAMING_DEVICE_DEVICE_ID_XBOX_ONE:
        case GAMING_DEVICE_DEVICE_ID_XBOX_ONE_S:
            *width = 1920;
            *height = 1080;
            break;
        case GAMING_DEVICE_DEVICE_ID_XBOX_SERIES_S:
            *width = 2560;
            *height = 1440;
            break;
        case GAMING_DEVICE_DEVICE_ID_XBOX_ONE_X:
        case GAMING_DEVICE_DEVICE_ID_XBOX_ONE_X_DEVKIT:
        case GAMING_DEVICE_DEVICE_ID_XBOX_SERIES_X:
        case GAMING_DEVICE_DEVICE_ID_XBOX_SERIES_X_DEVKIT:
            *width = 3840;
            *height = 2160;
            break;
        default:
            break;
    }
}

extern "C" int xash_uwp_get_render_size(int* width, int* height)
{
    if (!width || !height) {
        return 0;
    }

    if (s_has_cached_size) {
        *width = s_cached_width;
        *height = s_cached_height;
        return 1;
    }

    int hdmi_width = 0;
    int hdmi_height = 0;
    if (!get_hdmi_size(&hdmi_width, &hdmi_height)) {
        return 0;
    }

    int cap_width = hdmi_width;
    int cap_height = hdmi_height;
    get_xbox_render_cap(&cap_width, &cap_height);

    *width = std::min(hdmi_width, cap_width);
    *height = std::min(hdmi_height, cap_height);
    if (*width <= 0 || *height <= 0) {
        return 0;
    }

    s_cached_width = *width;
    s_cached_height = *height;
    s_has_cached_size = 1;
    return 1;
}
