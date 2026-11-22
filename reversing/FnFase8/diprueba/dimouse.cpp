// Prueba aislada: DirectInput8 raton (foreground, no exclusivo) + SendInput, con/sin registro de raw input estilo SDL.
// Compilar con b.cmd ; uso: dimouse.exe [none|raw|rawsink|rawlate|rawlate_reacq|rawlate_recreate]
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <dinput.h>
#include <cstdio>
#include <cstring>
int main(int argc, char** argv) {
    const char* modo = argc > 1 ? argv[1] : "none";
    WNDCLASSA wc{}; wc.lpfnWndProc = DefWindowProcA; wc.hInstance = GetModuleHandle(0); wc.lpszClassName = "dit";
    RegisterClassA(&wc);
    HWND w = CreateWindowA("dit", "dit", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 300, 0, 0, wc.hInstance, 0);
    SetForegroundWindow(w); Sleep(300);
    auto reg = [&](DWORD fl) { RAWINPUTDEVICE r{1, 2, fl, w}; printf("RegisterRawInputDevices(%lx)=%d\n", fl, RegisterRawInputDevices(&r, 1, sizeof r)); };
    auto is = [&](const char* n) { return !strcmp(modo, n); };
    if (is("raw")) reg(0);
    if (is("rawsink")) reg(RIDEV_INPUTSINK);
    LPDIRECTINPUT8 di; LPDIRECTINPUTDEVICE8 m;
    DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&di, 0);
    di->CreateDevice(GUID_SysMouse, &m, 0); m->SetDataFormat(&c_dfDIMouse2);
    m->SetCooperativeLevel(w, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    printf("Acquire=0x%lx\n", m->Acquire());
    if (is("rawlate") || is("rawlate_reacq") || is("rawlate_recreate")) reg(RIDEV_INPUTSINK);
    if (is("rawlate_reacq")) { m->Unacquire(); printf("reacquire=0x%lx\n", m->Acquire()); }
    if (is("rawlate_recreate")) {
        m->Unacquire(); m->Release(); di->CreateDevice(GUID_SysMouse, &m, 0); m->SetDataFormat(&c_dfDIMouse2);
        m->SetCooperativeLevel(w, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); printf("recreate acquire=0x%lx\n", m->Acquire());
    }
    long sx = 0, sy = 0;
    for (int i = 0; i < 40; i++) {
        INPUT in{}; in.type = INPUT_MOUSE; in.mi.dx = 5; in.mi.dy = 2; in.mi.dwFlags = MOUSEEVENTF_MOVE; SendInput(1, &in, sizeof in);
        MSG msg; while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) DispatchMessage(&msg);
        Sleep(15);
        DIMOUSESTATE2 s{}; HRESULT hr = m->GetDeviceState(sizeof s, &s); if (FAILED(hr)) { m->Acquire(); continue; }
        sx += s.lX; sy += s.lY;
    }
    printf("modo=%s DI suma lX=%ld lY=%ld (esperado ~200,80)\n", modo, sx, sy);
}
