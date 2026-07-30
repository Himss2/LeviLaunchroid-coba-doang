#pragma once

namespace zoom_controller {

constexpr float kNeutralFactor = 1.0f;
constexpr float kMinFactor     = 0.10f;
constexpr float kMaxFactor     = 4.00f;

// Memulai state zoom (reset target & mengaktifkan override)
void BeginZoom();

// Menyesuaikan target zoom berdasarkan pergerakan/drag
void UpdateDrag(float delta);

// Menghentikan hold dan memulai animasi rilis (kembali ke FOV normal)
void EndZoom();

// Dijalankan setiap frame untuk memperbarui animasi transisi
void Tick();

// Memeriksa apakah zoom sedang aktif
bool IsActive();

} // namespace zoom_controller
