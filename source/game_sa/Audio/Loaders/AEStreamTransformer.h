#pragma once

#include "AEDataStream.h"

/**
 * Transformador XOR de 16 bytes empleado por ciertos recursos de audio.
 *
 * La operación es simétrica y depende de la posición absoluta, por lo que puede
 * descifrar fragmentos independientes después de un seek.
 */
class CAEStreamTransformer {
public:
    void Initialise();
    CAEStreamTransformer* Constructor(); // 0x4DC190 (lote 172A, banco Fn172A): `push esi; mov esi,ecx; call 0x4F1750; mov eax,esi; pop esi; ret` (el listado lo atribuia a CAECutsceneTrackManager; nadie lo llama por direccion en el EXE)
    void TransformBuffer(void* buffer, size_t size, uint32 position);

private:
    uint8 table[16]; // EA 3A C4 A1 9A A8 14 F3 48 B0 D7 23 9D E8 FF F1

    friend class CAEDataStream;
    static void InjectHooks();
};

extern CAEStreamTransformer& AEStreamTransformer;
