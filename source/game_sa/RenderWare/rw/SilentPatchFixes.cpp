/*
 * Correcciones tomadas de SilentPatch (https://github.com/CookiePLMonster/SilentPatch).
 * The MIT License (MIT) - Copyright (c) 2024 Adrian Zdanowicz (Silent)
 * Texto completo de la licencia: docs/THIRD_PARTY_NOTICES.md. Se conserva este aviso en todas las copias o partes sustanciales.
 *
 * "Streaming memory bug" (SilentPatchSA.cpp, GTARtAnimInterpolatorSetCurrentAnim): el EXE, tras inicializar con identidad cada fotograma
 * interpolado de RpAnimBlend, escribe en sus 8 primeros bytes (q.x y q.y) los PUNTEROS keyFrame1/keyFrame2 de la RtAnimAnimation ficticia.
 * Leidos como float, un puntero >= 0x3F800000 (por encima de ~1 GB, frecuente con el exe LARGE ADDRESS AWARE y mucho uso de RAM) vale de 1 a 3e38,
 * y los fotogramas que el blend no reescribe despues (nodos sin secuencia) conservan ese cuaternion -> huesos deformados durante un fotograma.
 * La correccion consiste solo en no escribir esas cabeceras para el esquema RpAnimBlend (nadie las lee: no usa AddAnimTime). Ver rtanim.cpp.
 */
#include "StdInc.h"
#include "SilentPatchFixes.h"
#include "extensions/SilentPatchConfig.h"
#include "rtanim.h"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace SilentPatchFixes {
static int s_forzado = -1;
void ForzarParaPrueba(int estado) { s_forzado = estado; }

bool Activo() {
    if (s_forzado >= 0) {
        return s_forzado != 0;
    }
    // interruptor general (GTASA_SILENTPATCH / SilentPatch= en gta-reversed.ini) y opcion propia RtAnimInterp (GTASA_SILENTPATCH_RTANIMINTERP / SilentPatch_RtAnimInterp=)
    static const bool activo = SilentPatchOption("RtAnimInterp", true);
    return activo;
}

void AutoPrueba() {
    static bool hecho = false;
    static const bool habilitada = []() {
        char v[8]{};
        return GetEnvironmentVariableA("GTASA_SILENTPATCH_SELFTEST", v, sizeof(v)) != 0 && v[0] == '1';
    }();
    if (hecho || !habilitada) {
        return;
    }
    hecho = true;
    FILE* const f = fopen("logs/silentpatch_selftest.log", "w");
    if (f == nullptr) {
        return;
    }
    constexpr int NODOS = 32;
    const auto* const info = RtAnimGetInterpolatorInfo(rwID_RPANIMBLENDPLUGIN);
    RtAnimInterpolator* const interp = RtAnimInterpolatorCreate(NODOS, 0x24);
    if (info == nullptr || interp == nullptr) {
        fprintf(f, "no se pudo preparar el banco (info=%p interp=%p)\n", (const void*)info, (void*)interp);
        fclose(f);
        return;
    }
    RtAnimAnimation falsa{};
    falsa.interpInfo = const_cast<RtAnimInterpolatorInfo*>(info);
    falsa.numFrames  = 2 * NODOS;
    falsa.pFrames    = reinterpret_cast<void*>(uintptr_t(0x5A000000)); // direccion alta ficticia (nunca se desreferencia: la interpolacion RpAnimBlend ignora las entradas)
    for (const int estado : {0, 1, -1}) { // 0 y 1 fuerzan; -1 = segun la configuracion real (GTASA_SILENTPATCH / ini / opcion)
        ForzarParaPrueba(estado);
        std::memset(rtANIMGETINTERPFRAME(interp, 0), 0xCC, size_t(NODOS) * 0x24);
        RtAnimInterpolatorSetCurrentAnim(interp, &falsa);
        int cc = 0, nan = 0, enormes = 0, noIdentidad = 0;
        double maxAbs = 0.0;
        for (int i = 0; i < NODOS; ++i) {
            const auto* const w = static_cast<const uint32*>(rtANIMGETINTERPFRAME(interp, i));
            const auto* const x = static_cast<const float*>(rtANIMGETINTERPFRAME(interp, i));
            const float ident[7] = {0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f};
            bool ni = false;
            for (int k = 0; k < 7; ++k) {
                cc += (w[k] == 0xCCCCCCCCu);
                nan += !std::isfinite(x[k]);
                if (std::isfinite(x[k])) {
                    enormes += (std::fabs(x[k]) > 1000.f);
                    maxAbs = std::fmax(maxAbs, std::fabs(x[k]));
                }
                ni |= (x[k] != ident[k]);
            }
            noIdentidad += ni;
        }
        const auto* const n0 = static_cast<const float*>(rtANIMGETINTERPFRAME(interp, 0));
        fprintf(f, "arreglo=%s (Activo()=%d): nodos=%d palabras_0xCCCCCCCC=%d NaN/inf=%d |valor|>1000=%d nodos_distintos_de_identidad=%d maxAbs=%.6g nodo0.q=(%.6g %.6g %.6g %.6g)\n",
                estado < 0 ? "CONFIG  " : (estado ? "ACTIVO " : "APAGADO"), (int)Activo(), NODOS, cc, nan, enormes, noIdentidad, maxAbs, n0[0], n0[1], n0[2], n0[3]);
    }
    ForzarParaPrueba(-1);
    RtAnimInterpolatorDestroy(interp);
    fclose(f);
}

// DIAGNOSTICO TEMPORAL (GTASA_SILENTPATCH_DIAG=1): busca 0xCCCCCCCC en los fotogramas interpolados de un clump esqueletico y, al encontrarlo por primera vez,
// arma un GUARDIAN DE ESCRITURA (DR0-DR2, 4 bytes, escritura) sobre q.x y t.x del nodo 6 y q.y del nodo 7; el VEH registra EIP, valor y pila de cada escritura.
static FILE* s_logGuardian = nullptr;
static volatile LONG s_nEscrituras = 0;
static LONG CALLBACK VehGuardian(PEXCEPTION_POINTERS ep) {
    if (ep->ExceptionRecord->ExceptionCode != EXCEPTION_SINGLE_STEP || (ep->ContextRecord->Dr6 & 0xF) == 0 || s_logGuardian == nullptr) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    const CONTEXT& c = *ep->ContextRecord;
    const int slot = (c.Dr6 & 1) ? 0 : (c.Dr6 & 2) ? 1 : (c.Dr6 & 4) ? 2 : 3;
    const uint32 dir = slot == 0 ? c.Dr0 : slot == 1 ? c.Dr1 : slot == 2 ? c.Dr2 : c.Dr3;
    if (InterlockedIncrement(&s_nEscrituras) <= 3000) {
        fprintf(s_logGuardian, "ESCRITURA dir=%08X nuevo=%08X eip=%08X(tras la instruccion) esp=%08X ebp=%08X eax=%08X ecx=%08X edx=%08X\n", dir, *reinterpret_cast<const uint32*>(dir), c.Eip, c.Esp, c.Ebp, c.Eax, c.Ecx, c.Edx);
        fprintf(s_logGuardian, "  cadena_ebp:");
        uint32 ebp = c.Ebp;
        for (int i = 0; i < 8 && ebp > 0x10000 && !IsBadReadPtr(reinterpret_cast<void*>(ebp), 8); ++i) {
            fprintf(s_logGuardian, " %08X", *reinterpret_cast<const uint32*>(ebp + 4));
            ebp = *reinterpret_cast<const uint32*>(ebp);
        }
        fprintf(s_logGuardian, "\n  pila(valores en codigo):");
        int n = 0;
        for (uint32 p = c.Esp; n < 14 && p < c.Esp + 0x400 && !IsBadReadPtr(reinterpret_cast<void*>(p), 4); p += 4) {
            const uint32 v = *reinterpret_cast<const uint32*>(p);
            if (v >= 0x401000 && v < 0xB00000) { fprintf(s_logGuardian, " %08X", v); ++n; }
        }
        fprintf(s_logGuardian, "\n");
        fflush(s_logGuardian);
    }
    return EXCEPTION_CONTINUE_EXECUTION;
}

struct ArmaArgs { DWORD tid; uint32 dir[3]; };
static DWORD WINAPI ArmaHilo(LPVOID p) {
    const auto* const a = static_cast<ArmaArgs*>(p);
    HANDLE h = OpenThread(THREAD_ALL_ACCESS, FALSE, a->tid);
    if (h == nullptr) { return 1; }
    SuspendThread(h);
    CONTEXT c{};
    c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(h, &c);
    c.Dr0 = a->dir[0]; c.Dr1 = a->dir[1]; c.Dr2 = a->dir[2];
    c.Dr7 = 0;
    for (int i = 0; i < 3; ++i) { c.Dr7 |= (1u << (2 * i)) | (0xDu << (16 + 4 * i)); }
    c.Dr6 = 0;
    SetThreadContext(h, &c);
    ResumeThread(h);
    CloseHandle(h);
    return 0;
}

void DiagFrames(const char* donde, const void* frames, int nFrames, const void* clump, int extra) {
    static const bool on = []() { char v[4]{}; return GetEnvironmentVariableA("GTASA_SILENTPATCH_DIAG", v, 4) && v[0] == '1'; }();
    if (!on || frames == nullptr) { return; }
    static FILE* f = fopen("logs/sp_diag.log", "w");
    static int n = 0;
    static bool armado = false;
    const auto* const fd = static_cast<const char*>(frames);
    if (!armado && nFrames > 7) {
        for (int i = 0; i < nFrames && !armado; ++i) {
            const auto* const w = static_cast<const uint32*>(*reinterpret_cast<void* const*>(fd + size_t(i) * 0x18 + 0x10));
            for (int k = 0; w != nullptr && k < 7 && !armado; ++k) {
                if (w[k] != 0xCCCCCCCCu) { continue; }
                const auto* const k6 = static_cast<const uint32*>(*reinterpret_cast<void* const*>(fd + 6 * 0x18 + 0x10));
                const auto* const k7 = static_cast<const uint32*>(*reinterpret_cast<void* const*>(fd + 7 * 0x18 + 0x10));
                static ArmaArgs args{};
                args.tid = GetCurrentThreadId();
                args.dir[0] = uint32(uintptr_t(k6 + 0)); args.dir[1] = uint32(uintptr_t(k6 + 4)); args.dir[2] = uint32(uintptr_t(k7 + 1));
                s_logGuardian = fopen("logs/sp_guardian.log", "w");
                AddVectoredExceptionHandler(1, VehGuardian);
                HANDLE ht = CreateThread(nullptr, 0, ArmaHilo, &args, 0, nullptr);
                if (ht != nullptr) { WaitForSingleObject(ht, 5000); CloseHandle(ht); }
                if (s_logGuardian) { fprintf(s_logGuardian, "ARMADO donde=%s clump=%p DR0=%08X DR1=%08X DR2=%08X (detectado frame=%d palabra=%d)\n", donde, clump, args.dir[0], args.dir[1], args.dir[2], i, k); fflush(s_logGuardian); }
                armado = true;
            }
        }
    }
    if (f == nullptr || n > 4000) { return; }
    for (int i = 0; i < nFrames; ++i) {
        const void* const kf = *reinterpret_cast<void* const*>(fd + size_t(i) * 0x18 + 0x10);
        if (kf == nullptr) { continue; }
        const auto* const w = static_cast<const uint32*>(kf);
        for (int k = 0; k < 7; ++k) {
            if (w[k] == 0xCCCCCCCCu) {
                fprintf(f, "%s clump=%p frame=%d kf=%p palabra=%d extra=%d\n", donde, clump, i, kf, k, extra);
                fflush(f);
                ++n;
            }
        }
    }
}
} // namespace SilentPatchFixes
