#pragma once

namespace config {

struct Settings {
    int zoomAnimSpeed   = 5;    // Kecepatan animasi (1 = lambat, 10 = sangat cepat)
    bool hideHandOnZoom = true; // Sembunyikan tangan saat zoom aktif
    float posX          = 20.0f;
    float posY          = 20.0f;
    float scale         = 1.0f;
    int opacity         = 100;
};

extern Settings g_settings;

// Memuat konfigurasi dari penyimpanan lokal
void Load();

// Menyimpan konfigurasi ke penyimpanan lokal
void Save();

} // namespace config
