#include "Core/Config.hpp"
#include <android/log.h>
#include <fstream>
#include <string>

#define LOG_TAG "ZoomConfig"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace config {

Settings g_settings;

constexpr const char* kConfigPath = "/sdcard/games/com.mojang/minecraftpe/zoom_config.json";

void Load() {
    std::ifstream file(kConfigPath);
    if (!file.is_open()) {
        LOGI("Config: File konfigurasi tidak ditemukan, menggunakan nilai default");
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("zoomAnimSpeed") != std::string::npos) {
            sscanf(line.c_str(), " \"zoomAnimSpeed\": %d", &g_settings.zoomAnimSpeed);
        } else if (line.find("hideHandOnZoom") != std::string::npos) {
            int val = 1;
            sscanf(line.c_str(), " \"hideHandOnZoom\": %d", &val);
            g_settings.hideHandOnZoom = (val != 0);
        }
    }
    file.close();
    LOGI("Config: Konfigurasi berhasil dimuat");
}

void Save() {
    std::ofstream file(kConfigPath, std::ios::trunc);
    if (!file.is_open()) {
        return;
    }

    file << "{\n";
    file << "  \"zoomAnimSpeed\": " << g_settings.zoomAnimSpeed << ",\n";
    file << "  \"hideHandOnZoom\": " << (g_settings.hideHandOnZoom ? 1 : 0) << "\n";
    file << "}\n";
    file.close();
    LOGI("Config: Konfigurasi berhasil disimpan");
}

} // namespace config
