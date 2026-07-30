#pragma once

#include <functional>

namespace camera_hook {

// Mengaitkan (hook) fungsi CameraAPI::tryGetFOV di vtable Minecraft
bool Install();

// Melepas hook dari memori
void Uninstall();

// Mengesampingkan FOV asli dengan divisor baru
void SetOverride(float factor);

// Mengembalikan kontrol FOV ke vanilla Minecraft
void ClearOverride();

// Mendaftarkan callback yang dieksekusi 1x per frame
void SetFrameTickCallback(std::function<void()> callback);

} // namespace camera_hook
