// g23b: funciones auxiliares pequenas del EXE (CVector, CMatrix, CRT) como secuencias x87 de la maquina FlcX87.
// Cada una repite, instruccion por instruccion, la secuencia x87 del EXE (direccion entre parentesis); los argumentos de memoria son los mismos punteros/flotantes que recibe la funcion del EXE.
#pragma once
#include "flc_soft.h"

// ---- CVector (en el EXE son instancias fuera de linea de los operadores de CVector)
void FlcVecAdd(FlcX87& fp, float* out, const float* a, const float* b);            // 0x40FE30  out = a + b           (z, y, x)
void FlcVecSub(FlcX87& fp, float* out, const float* a, const float* b);            // 0x40FE60  out = a - b           (z, y, x)
void FlcVecMulLeft(FlcX87& fp, float* out, const float& f, const float* v);        // 0x40FE90  out = f * v           (z, y, x)
void FlcVecMulRight(FlcX87& fp, float* out, const float* v, const float& f);       // 0x40FEC0  out = v * f           (z, y, x)
void FlcVecScale(FlcX87& fp, float* v, const float& f);                            // 0x40FEF0  v *= f                (x, y, z)
void FlcVecAddTo(FlcX87& fp, float* v, const float* a);                            // 0x411A00  v += a                (x, y, z)
void FlcVecMagnitude(FlcX87& fp, const float* v);                                  // 0x4082C0  deja |v| en ST0
void FlcVecMagnitudeSquared(FlcX87& fp, const float* v);                           // 0x406DA0  deja |v|^2 en ST0
void FlcVecDot(FlcX87& fp, const float* a, const float* b);                        // 0x40FDB0  deja a.b en ST0       (z, y, x)
void FlcVecNormalise(FlcX87& fp, float* v);                                        // 0x59C910  (0x59C943: longitud 0 -> x = 1)
void FlcVecCross(FlcX87& fp, float* out, const float* a, const float* b);          // 0x59C730  out = a x b (via pila local del EXE)
void FlcMatMul3x3(FlcX87& fp, float* out, const float* m, const float* v);         // 0x59C790  out = M * v (columnas)
void FlcMatInvMul3x3(FlcX87& fp, float* out, const float* m, const float* v);      // 0x59C810  out = M^T * v
void FlcFloatMax(FlcX87& fp, const float& a, const float& b);                      // 0x420800  deja max en ST0 (b si a <= b o NaN)
void FlcFloatMin(FlcX87& fp, const float& a, const float& b);                      // 0x404330  deja min en ST0

// ---- CRT del EXE (argumentos en la pila x87)
void FlcCrtAsin(FlcX87& fp);                // 0x821E70 _CIasin: ST0 = asin(ST0)
void FlcCrtAcos(FlcX87& fp);                // 0x822380 _CIacos: ST0 = acos(ST0)
void FlcCrtPow(FlcX87& fp);                 // 0x822130 _CIpow:  ST1 ^ ST0 -> ST0 (saca uno)
int  FlcCrtFtol2(FlcX87& fp);               // 0x821B40 __ftol2: saca ST0 y devuelve EAX

// Estado del mundo que lee el CRT del EXE (en el juego son globales del EXE; los bancos las fijan).
struct FlcWorld {
    int sse2Pow;        // [0xC9C400]: el CRT usa pow SSE2
    int matherrFlag;    // [0xC9ABFC]: 0 -> el CRT comprueba y notifica las excepciones x87
};
FlcWorld FlcReadWorld();
unsigned int FlcHwSticky();   // banderas acumuladas del x87 real (IE.. PE) en el formato FLC_*
