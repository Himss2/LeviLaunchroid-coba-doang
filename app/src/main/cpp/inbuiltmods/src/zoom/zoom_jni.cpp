#include <jni.h>
#include "CameraHook/CameraHook.hpp"
#include "ZoomController/ZoomController.hpp"
#include "Core/Config.hpp"
#include <android/log.h>

#define LOG_TAG "ZoomModJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jboolean JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeInit(JNIEnv* env, jclass clazz) {
    LOGI("ZoomMod: nativeInit() dipanggil dari Java UI");

    // Load konfigurasi penyimpana (kecepatan transisi, hide hand, dll.)
    config::Load();

    // Daftarkan callback frame tick untuk menganimasikan FOV secara smooth
    camera_hook::SetFrameTickCallback([]() {
        zoom_controller::Tick();
    });

    // Kaitkan (hook) vtable CameraAPI::tryGetFOV
    if (!camera_hook::Install()) {
        LOGE("ZoomMod: Gagal memasang CameraHook!");
        return JNI_FALSE;
    }

    LOGI("ZoomMod: Integrasi C++ Native Engine dan Java UI Berhasil!");
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnKeyDown(JNIEnv* env, jclass clazz) {
    zoom_controller::BeginZoom();
}

JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnKeyUp(JNIEnv* env, jclass clazz) {
    zoom_controller::EndZoom();
}

JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeOnScroll(JNIEnv* env, jclass clazz, jfloat delta) {
    if (!zoom_controller::IsActive()) {
        zoom_controller::BeginZoom();
    }
    // Menyesuaikan zoom factor saat tombol/gesture scroll digunakan
    zoom_controller::UpdateDrag(-delta * 0.05f);
}

JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeSetZoomLevel(JNIEnv* env, jclass clazz, jlong level) {
    float factor = static_cast<float>(level) / 100.0f;
    zoom_controller::UpdateDrag(factor);
}

JNIEXPORT void JNICALL
Java_org_levimc_launcher_core_mods_inbuilt_nativemod_ZoomMod_nativeSetTransitionDuration(JNIEnv* env, jclass clazz, jint duration) {
    int speed = static_cast<int>(duration);
    if (speed < 1) speed = 1;
    if (speed > 10) speed = 10;

    config::g_settings.zoomAnimSpeed = speed;
    config::Save();
}

} // extern "C"
