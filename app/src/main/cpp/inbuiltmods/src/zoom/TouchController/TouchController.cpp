#include "TouchController/TouchController.hpp"
#include "ZoomController/ZoomController.hpp"
#include "ZoomButton/ZoomButton.hpp"

#include <pl/Input.hpp>
#include <android/log.h>

#define LOG_TAG "TouchController"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace touch_controller {
namespace {

constexpr int kActionMask        = 0xFF;
constexpr int kActionDown        = 0;
constexpr int kActionUp          = 1;
constexpr int kActionMove        = 2;
constexpr int kActionCancel      = 3;
constexpr int kActionPointerDown = 5;
constexpr int kActionPointerUp   = 6;

int g_trackedPointerId = -1;
float g_lastY = 0.0f;

bool OnTouch(const pl::input::TouchEvent& ev) {
    int action = ev.action & kActionMask;

    switch (action) {
        case kActionDown:
        case kActionPointerDown:
            // Tangkap hanya jika sentuhan berada di dalam area tombol ZM
            if (g_trackedPointerId == -1 && zoom_button::Contains(ev.x, ev.y)) {
                g_trackedPointerId = ev.pointerId;
                g_lastY = ev.y;
                zoom_controller::BeginZoom();
                return true; // Konsumsi event DOWN agar tidak memukul/menghancurkan blok
            }
            return false; // Jari lain (kamera) diteruskan ke Minecraft

        case kActionMove:
            if (g_trackedPointerId != -1 && ev.pointerId == g_trackedPointerId) {
                float deltaY = ev.y - g_lastY; // Geser ke atas = deltaY negatif (zoom in)
                g_lastY = ev.y;

                // Sensitivitas drag disesuaikan agar transisi zoom halus
                zoom_controller::UpdateDrag(deltaY * 0.0015f);
            }
            // CRITICAL: Selalu return false pada MOVE agar Minecraft 
            // tetap menerima input rotasi kamera dari jari kedua!
            return false;

        case kActionUp:
        case kActionPointerUp:
        case kActionCancel:
            if (ev.pointerId == g_trackedPointerId) {
                g_trackedPointerId = -1;
                zoom_controller::EndZoom();
                return true; // Konsumsi event UP tombol zoom
            }
            return false;

        default:
            return false;
    }
}

} // namespace

void Install() {
    pl::input::registerTouchCallback(OnTouch);
    LOGI("TouchController: Multi-touch passthrough & smooth drag siap");
}

} // namespace touch_controller
