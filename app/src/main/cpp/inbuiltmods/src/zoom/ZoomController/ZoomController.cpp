#include "ZoomController/ZoomController.hpp"
#include "CameraHook/CameraHook.hpp"
#include "Core/Config.hpp"

#include <atomic>
#include <cmath>
#include <algorithm>

namespace zoom_controller {
namespace {

constexpr float kInitialZoomFactor = 0.30f; // Factor awal saat pertama ditekan
constexpr float kMinZoomLimit      = 0.03f; // Batas zoom maksimal (sangat dekat)
constexpr float kMaxZoomLimit      = 0.85f; // Batas zoom minimal

std::atomic<bool> g_active{false};
std::atomic<bool> g_releasing{false};

std::atomic<float> g_targetFactor{kNeutralFactor};
float g_currentFactor = kNeutralFactor;

float Clamp(float value) {
    if (value < kMinZoomLimit) return kMinZoomLimit;
    if (value > kMaxZoomLimit) return kMaxZoomLimit;
    return value;
}

} // namespace

void BeginZoom() {
    g_targetFactor.store(kInitialZoomFactor, std::memory_order_relaxed);
    g_releasing.store(false, std::memory_order_relaxed);
    g_active.store(true, std::memory_order_relaxed);
}

void UpdateDrag(float delta) {
    if (!g_active.load(std::memory_order_relaxed) || g_releasing.load(std::memory_order_relaxed)) {
        return;
    }

    float currentTarget = g_targetFactor.load(std::memory_order_relaxed);
    float newTarget = Clamp(currentTarget + delta);
    g_targetFactor.store(newTarget, std::memory_order_relaxed);
}

void EndZoom() {
    g_targetFactor.store(kNeutralFactor, std::memory_order_relaxed);
    g_releasing.store(true, std::memory_order_relaxed);
}

void Tick() {
    if (!g_active.load(std::memory_order_relaxed)) {
        return;
    }

    float target = g_targetFactor.load(std::memory_order_relaxed);
    bool isReleasing = g_releasing.load(std::memory_order_relaxed);

    // Mengambil opsi kecepatan animasi dari Config (1 - 10, default = 5)
    float speedSetting = static_cast<float>(config::g_settings.zoomAnimSpeed);

    float releaseSpeed = std::clamp(speedSetting * 0.08f, 0.08f, 0.80f);
    float zoomInSpeed  = std::clamp(speedSetting * 0.05f, 0.05f, 0.50f);

    if (isReleasing) {
        // Smooth interpolation kembali ke FOV pemain
        g_currentFactor += (target - g_currentFactor) * releaseSpeed;

        // Jika sudah mendekati FOV normal, matikan override
        if (g_currentFactor >= 0.92f) {
            g_currentFactor = kNeutralFactor;
            g_active.store(false, std::memory_order_relaxed);
            g_releasing.store(false, std::memory_order_relaxed);

            camera_hook::ClearOverride();
            return;
        }
    } else {
        // Smooth interpolation saat zoom in/drag
        float diff = target - g_currentFactor;
        g_currentFactor += diff * zoomInSpeed;
    }

    camera_hook::SetOverride(g_currentFactor);
}

bool IsActive() {
    return g_active.load(std::memory_order_relaxed);
}

} // namespace zoom_controller
