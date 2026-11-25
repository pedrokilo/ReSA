// Banco oraculo: lado NUESTRO. Compila, sin tocar el juego, el codigo extraido por extraer.py (X87Math.cpp lote S11A y rphanim.cpp)
// contra las cabeceras reales del proyecto, y lo expone con una interfaz C de arrays de float para compararlo con librw.
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <windows.h>
#include <d3d9.h>
#include <cstdint>
template <typename T> T& StaticRef(uintptr_t a) { return *reinterpret_cast<T*>(a); }
#include <rwcore.h>
#include <rpworld.h>
#include "rphanim.h"

// --- dependencias externas de rphanim.cpp, sustituidas en el banco (semantica documentada) ---
static RwGlobals g_engine{};
// RwMatrixMultiply del proyecto (rwplcore.cpp 0x7F18B0): atajo de identidad (flag 0x20000) copiando la otra matriz ENTERA (64 bytes) y, si no, producto + flags = flags2 & flags1.
static void MultDefault(RwMatrix* o, const RwMatrix* a, const RwMatrix* b) {
    const float* A = (const float*)a; const float* B = (const float*)b; float r[16];
    // filas right, up, at, pos (4 floats cada una; la cuarta es el flag/relleno)
    for (int i = 0; i < 4; ++i) {
        r[i*4+0] = A[i*4+0]*B[0] + A[i*4+1]*B[4] + A[i*4+2]*B[8]  + (i==3 ? 1.0f : 0.0f)*B[12];
        r[i*4+1] = A[i*4+0]*B[1] + A[i*4+1]*B[5] + A[i*4+2]*B[9]  + (i==3 ? 1.0f : 0.0f)*B[13];
        r[i*4+2] = A[i*4+0]*B[2] + A[i*4+1]*B[6] + A[i*4+2]*B[10] + (i==3 ? 1.0f : 0.0f)*B[14];
    }
    float* O = (float*)o;
    for (int i = 0; i < 4; ++i) { O[i*4+0] = r[i*4+0]; O[i*4+1] = r[i*4+1]; O[i*4+2] = r[i*4+2]; }
}
RwMatrix* RwMatrixMultiply(RwMatrix* out, const RwMatrix* m1, const RwMatrix* m2) {
    const RwUInt32 f1 = m1->flags, f2 = m2->flags;
    if (f1 & 0x20000u) { memcpy(out, m2, sizeof(RwMatrix)); return out; }
    if (f2 & 0x20000u) { memcpy(out, m1, sizeof(RwMatrix)); return out; }
    MultDefault(out, m1, m2);
    out->flags = f2 & f1;
    return out;
}
RwFrame* RwFrameUpdateObjects(RwFrame* f) { return f; }
RwBool RwFrameDirty(const RwFrame*) { return FALSE; }
RwMatrix* RwFrameGetLTM(RwFrame* f) { return &f->ltm; }

#define BANCO_SIN_EXTERN
#include "gen/hanim_math_extraido.inc"
#include "gen/hanim_update_extraido.inc"

static float __cdecl sqrtf_c(float v) { return std::sqrt(v); }

extern "C" {
__declspec(dllexport) void n_interp(float* out, float* in1, float* in2, float t) { HAnimKeyFrameInterpolateCpp(out, in1, in2, t, sqrtf_c); }
__declspec(dllexport) void n_blend(float* out, float* in1, float* in2, float a) { HAnimKeyFrameBlendCpp(out, in1, in2, a, sqrtf_c); }
__declspec(dllexport) void n_apply(float* m, const float* f) { HAnimKeyFrameApplyCpp(m, f); }
__declspec(dllexport) void n_add(float* o, const float* a, const float* b) { HAnimKeyFrameAddCpp(o, a, b); }
__declspec(dllexport) void n_mulrecip(float* f, const float* s) { HAnimKeyFrameMulRecipCpp(f, s); }
}
static void __cdecl applyCB(void* m, void* f) { HAnimKeyFrameApplyCpp((float*)m, (const float*)f); }

// Jerarquia: frames = numNodes x 9 floats (hdr0, hdr1, qx, qy, qz, qw, tx, ty, tz); salida = numNodes x 16 floats.
extern "C" int n_update(int numNodes, const int* nodeFlags, const float* frames, float* outMats, unsigned hflags, int NOMATRICES_unused) {
    (void)NOMATRICES_unused;
    const int keySize = 9 * 4;
    RtAnimInterpolator* anim = (RtAnimInterpolator*)calloc(1, sizeof(RtAnimInterpolator) + numNodes * keySize);
    anim->numNodes = numNodes; anim->currentInterpKeyFrameSize = keySize; anim->keyFrameApplyCB = applyCB;
    memcpy((char*)anim + sizeof(RtAnimInterpolator), frames, numNodes * keySize);
    RpHAnimHierarchy h{}; h.flags = (RpHAnimHierarchyFlag)hflags; h.numNodes = numNodes; h.currentAnim = anim;
    RpHAnimNodeInfo* ni = (RpHAnimNodeInfo*)calloc(numNodes, sizeof(RpHAnimNodeInfo));
    for (int i = 0; i < numNodes; ++i) { ni[i].flags = nodeFlags[i]; ni[i].nodeIndex = i; }
    h.pNodeInfo = ni;
    RwMatrix* mats = (RwMatrix*)calloc(numNodes, sizeof(RwMatrix)); h.pMatrixArray = mats;
    RpHAnimHierarchyUpdateMatrices(&h);
    memcpy(outMats, mats, numNodes * sizeof(RwMatrix));
    free(mats); free(ni); free(anim);
    return 0;
}
