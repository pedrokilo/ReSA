#pragma once

struct IMediaControl;

#define WM_GRAPHNOTIFY 1037

namespace VideoPlayer {

void InjectHooks();

void Shutdown();
void WaitState(int32 state);
long OnGraphNotify(); // HRESULT
long UpdateWindow(); // HRESULT
void Release();
void Play(int32 nCmdShow, const char* path);
auto GetMediaControl() -> IMediaControl*;
uint32 SumMasked24(const uint32* begin, const uint32* end); // 0x7467D0 (lote 156A, banco Fn156A)

} // namespace VideoPlayer
