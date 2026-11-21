#pragma once

namespace notsa {
void InjectWindowedModeHooks();

namespace WindowedMode {
void AdjustVideoModeOnResize(int32 w, int32 h);
void BeforeCreateDevice() noexcept; // llamada por _rwD3D9System (rwcore.cpp) justo antes de IDirect3D9::CreateDevice
void AfterCreateDevice() noexcept;  // ... y justo despues de un CreateDevice correcto
};
};
