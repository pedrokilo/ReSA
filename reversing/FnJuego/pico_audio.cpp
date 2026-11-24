// Medidor del nivel de audio REAL de un proceso (WASAPI, IAudioMeterInformation de la sesion de audio del PID): demuestra que el juego saca sonido
// al mezclador de Windows aunque nadie lo oiga. Uso: pico_audio.exe <pid> <segundos> [intervalo_ms]. Imprime una linea por muestra: t_ms sesiones_del_pid estado pico.
#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv) {
    if (argc < 3) return 2;
    DWORD pid = atoi(argv[1]); int seg = atoi(argv[2]); int dt = argc > 3 ? atoi(argv[3]) : 500;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IMMDeviceEnumerator* en = NULL; IMMDevice* dev = NULL; IAudioSessionManager2* mgr = NULL;
    if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&en))) { printf("sin enumerador\n"); return 3; }
    if (FAILED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev))) { printf("sin dispositivo de salida de audio\n"); return 4; }
    if (FAILED(dev->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&mgr))) { printf("sin gestor de sesiones\n"); return 5; }
    DWORD t0 = GetTickCount(); float maxpico = 0; int muestras = 0, conPico = 0;
    while ((int)(GetTickCount() - t0) < seg * 1000) {
        IAudioSessionEnumerator* se = NULL; int n = 0, mios = 0; float pico = 0; int estado = -1;
        if (SUCCEEDED(mgr->GetSessionEnumerator(&se))) {
            se->GetCount(&n);
            for (int i = 0; i < n; i++) {
                IAudioSessionControl* c = NULL; if (FAILED(se->GetSession(i, &c))) continue;
                IAudioSessionControl2* c2 = NULL; DWORD p = 0;
                if (SUCCEEDED(c->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&c2))) {
                    c2->GetProcessId(&p);
                    if (p == pid) {
                        mios++; AudioSessionState st; c->GetState(&st); estado = st;
                        IAudioMeterInformation* mt = NULL; if (SUCCEEDED(c->QueryInterface(__uuidof(IAudioMeterInformation), (void**)&mt))) { float v = 0; mt->GetPeakValue(&v); if (v > pico) pico = v; mt->Release(); }
                    }
                    c2->Release();
                }
                c->Release();
            }
            se->Release();
        }
        muestras++; if (pico > 0.0001f) conPico++; if (pico > maxpico) maxpico = pico;
        printf("%lu sesiones=%d estado=%d(0 inactiva,1 activa) pico=%.4f\n", GetTickCount() - t0, mios, estado, pico); fflush(stdout);
        Sleep(dt);
    }
    printf("RESUMEN muestras=%d con_sonido=%d pico_max=%.4f\n", muestras, conPico, maxpico);
    return 0;
}

