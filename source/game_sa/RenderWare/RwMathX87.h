#pragma once

// Lote S3 (codigo vivo, parte P3): rutinas x87 de `_rwcseg` (0x857000..0x857432) que RenderWare instala en FINALIZESTART del dispositivo D3D9
// (rwcore.cpp, peticion 17): la multiplicacion de matrices (siempre), las cuatro de vectores (solo sin SSE) y la palabra de control x87.
// Todo en C++ sin __asm: cada operacion de la x87 a 24 bits (la precision que fija 0x857404) es una operacion float con un solo redondeo,
// asi que se usan las escalares SSE de un float en el mismo orden de sumas y productos del EXE. Verificacion: banco fuera del juego (reversing/FnP3).
//
//   0x857000 puntos (con traslacion), 0x85707F vectores (sin traslacion)     [rwVectorsMultFn: salida, entrada, cuenta, matriz]
//   0x8570EF punto (con traslacion), 0x857160 vector (sin traslacion)        [rwVectorMultFn:  salida, entrada, matriz]
//   0x8572A0 producto de matrices (destino, A, B)                            [rwMatrixMultFn]
//   0x857404 pone la x87 a 24 bits con todas las excepciones enmascaradas y guarda la palabra anterior en 0xCAF004; 0x85742B la restaura.
// Los de vectores devuelven la entrada (segundo argumento), como el EXE, no la salida.

#ifdef RWMATHX87_BENCH
#include <stdint.h>
typedef int32_t int32;
struct RwV3d { float x, y, z; };
struct RwMatrix { float v[16]; };
#else
#include "rw/rwplcore.h"
#endif

namespace RwMathX87 {
RwV3d* __cdecl TransformPoints(RwV3d* pointsOut, const RwV3d* pointsIn, int32 numPoints, const RwMatrix* matrix);  // 0x857000
RwV3d* __cdecl TransformVectors(RwV3d* vectorsOut, const RwV3d* vectorsIn, int32 numVectors, const RwMatrix* matrix); // 0x85707F
RwV3d* __cdecl TransformPoint(RwV3d* pointOut, const RwV3d* pointIn, const RwMatrix* matrix);                       // 0x8570EF
RwV3d* __cdecl TransformVector(RwV3d* vectorOut, const RwV3d* vectorIn, const RwMatrix* matrix);                    // 0x857160
void __cdecl MatrixMultiply(RwMatrix* dst, const RwMatrix* a, const RwMatrix* b);                                  // 0x8572A0
void __cdecl SetFpuControlWord();                                                                                  // 0x857404
void __cdecl RestoreFpuControlWord();                                                                              // 0x85742B
} // namespace RwMathX87

void RwMathX87InjectHooks();
