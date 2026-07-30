#include <jni.h>
#include "CameraHook/CameraHook.hpp"
#include "ZoomController/ZoomController.hpp"
#include "TouchController/TouchController.hpp"
#include "ZoomButton/ZoomButton.hpp"
#include "Core/Config.hpp"
#include <android/log.h>

#define LOG_TAG "ZoomModJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

// 1. Inisialisasi Hook FOV Kamera saat launcher mengaktifkan Zoom Mod
JNIEXPORT jboolean JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeInit(JNIEnv* env, jclass clazz) {
    LOGI("ZoomMod: nativeInit() dipanggil oleh launcher");

    // Muat konfigurasi awal & pasang Touch Callback
    config::Load();
    touch_controller::Install();

    // Registrasi frame tick per-frame untuk merender tombol ZM dan animasi release
    camera_hook::SetFrameTickCallback([]() {
        zoom_controller::Tick();
        zoom_button::Draw(zoom_controller::IsActive());
    });

    // Pasang hook vtable CameraAPI::tryGetFOV (slot 7)
    if (!camera_hook::Install()) {
        LOGE("ZoomMod: Gagal memasang CameraHook!");
        return JNI_FALSE;
    }

    zoom_button::Install();
    LOGI("ZoomMod: Inisialisasi ZoomRewrite berhasil!");
    return JNI_TRUE;
}

// 2. Dipanggil saat tombol zoom keyboard / overlay di-hold (KeyDown)
JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnKeyDown(JNIEnv* env, jclass clazz) {
    zoom_controller::BeginZoom();
}

// 3. Dipanggil saat tombol zoom dilepas (KeyUp)
JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnKeyUp(JNIEnv* env, jclass clazz) {
    zoom_controller::EndZoom();
}

// 4. Dipanggil saat scroll mouse digunakan
JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnScroll(JNIEnv* env, jclass clazz, jfloat delta) {
    if (!zoom_controller::IsActive()) {
        zoom_controller::BeginZoom();
    }
    // Delta scroll diubah menjadi perubahan kedalaman zoom
    zoom_controller::UpdateDrag(-delta * 0.05f);
}

// 5. Dipanggil untuk mengubah level zoom dari slider/setting
JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeSetZoomLevel(JNIEnv* env, jclass clazz, jlong level) {
    float factor = static_cast<float>(level) / 100.0f;
    zoom_controller::UpdateDrag(factor);
}

// 6. Mengatur kecepatan transisi/animasi zoom
JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeSetTransitionDuration(JNIEnv* env, jclass clazz, jint duration) {
    int speed = static_cast<int>(duration);
    if (speed < 1) speed = 1;
    if (speed > 10) speed = 10;

    config::g_settings.zoomAnimSpeed = speed;
    config::Save();
}

} // extern "C"
