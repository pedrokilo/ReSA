// Prueba dirigida de la carrera CdStreamSync / CdStreamThread (SilentPatch "CdStream data racing issue", opcion CdStreamSyncFix).
// Reproduce LITERALMENTE los protocolos de source/game_sa/CdStreamInfo.cpp (CdStreamRead, CdStreamSync, CdStreamThread) con una "lectura" simulada:
//   modo orig : sin seccion critica (como el EXE)
//   modo fix  : con seccion critica comun (como SilentPatch / CdStreamSyncFix)
// El hilo principal pide una lectura y sincroniza de inmediato, N veces; si el hilo principal no avanza durante `plazo` ms se da el bloqueo por reproducido.
// Uso: prueba_carrera.exe <orig|fix> <iteraciones> <plazo_ms> <espera_lectura (iteraciones de giro)>
#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <atomic>

struct Stream {
    volatile unsigned sectorsToRead = 0;
    volatile bool bLocked = false;
    volatile bool bInUse = false;
    volatile int status = 0;
    HANDLE sem = nullptr;
};

static Stream S;
static HANDLE gQueueSem;
static CRITICAL_SECTION gCS;
static bool gFix;
static int gSpin;
static std::atomic<unsigned long long> gProgress{0};
static std::atomic<bool> gStop{false};

// CdStreamRead (hilo principal): campos y aviso al hilo de lectura
static void Read() {
    S.status = 0;
    S.sectorsToRead = 1;
    S.bLocked = false;
    ReleaseSemaphore(gQueueSem, 1, nullptr);
}

// CdStreamSync (hilo principal)
static void Sync() {
    if (gFix) {
        EnterCriticalSection(&gCS);
        const bool mustWait = S.sectorsToRead != 0;
        if (mustWait) S.bLocked = true;
        LeaveCriticalSection(&gCS);
        if (mustWait) WaitForSingleObject(S.sem, INFINITE);
    } else if (S.sectorsToRead) {
        S.bLocked = true;
        WaitForSingleObject(S.sem, INFINITE);
    }
    S.bInUse = false;
}

// CdStreamThread (hilo de lectura)
static DWORD WINAPI Thread(LPVOID) {
    while (!gStop) {
        if (WaitForSingleObject(gQueueSem, 50) != WAIT_OBJECT_0) continue;
        S.bInUse = true;
        for (volatile int i = 0; i < gSpin; i++) {}  // "lectura"
        if (gFix) {
            EnterCriticalSection(&gCS);
            S.sectorsToRead = 0;
            const bool locked = S.bLocked;
            LeaveCriticalSection(&gCS);
            if (locked) ReleaseSemaphore(S.sem, 1, nullptr);
        } else {
            S.sectorsToRead = 0;
            if (S.bLocked) ReleaseSemaphore(S.sem, 1, nullptr);
        }
        S.bInUse = false;
    }
    return 0;
}

static DWORD WINAPI Main(LPVOID p) {
    const unsigned long long n = (unsigned long long)p;
    for (unsigned long long i = 0; i < n; i++) {
        Read();
        Sync();
        gProgress = i + 1;
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 5) { puts("uso: prueba_carrera <orig|fix> <iteraciones> <plazo_ms> <giro>"); return 2; }
    gFix = !strcmp(argv[1], "fix");
    const unsigned long long n = strtoull(argv[2], nullptr, 10);
    const DWORD plazo = (DWORD)atoi(argv[3]);
    gSpin = atoi(argv[4]);
    InitializeCriticalSection(&gCS);
    gQueueSem = CreateSemaphoreA(nullptr, 0, 5, nullptr);
    S.sem = CreateSemaphoreA(nullptr, 0, 2, nullptr);
    HANDLE th = CreateThread(nullptr, 0, Thread, nullptr, 0, nullptr);
    HANDLE mh = CreateThread(nullptr, 0, Main, (LPVOID)n, 0, nullptr);
    unsigned long long last = 0;
    DWORD stall = 0;
    for (;;) {
        if (WaitForSingleObject(mh, 50) == WAIT_OBJECT_0) {
            printf("%s: COMPLETO %llu iteraciones sin bloqueo\n", argv[1], (unsigned long long)gProgress);
            gStop = true; WaitForSingleObject(th, 1000);
            return 0;
        }
        const unsigned long long now = gProgress;
        if (now != last) { last = now; stall = 0; } else { stall += 50; }
        if (stall >= plazo) {
            printf("%s: BLOQUEO tras %llu iteraciones (principal sin avanzar %lu ms; sectorsToRead=%u bLocked=%d)\n", argv[1], now, (unsigned long)stall, S.sectorsToRead, (int)S.bLocked);
            fflush(stdout);
            TerminateProcess(GetCurrentProcess(), 1);
        }
    }
}
