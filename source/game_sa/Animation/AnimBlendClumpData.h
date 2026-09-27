/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "RenderWare.h"
#include "Vector.h"
#include "AnimBlendFrameData.h"

/**
 * Estado de animación asociado a un RpClump.
 *
 * Mantiene la lista de CAnimBlendAssociation activas y un array alineado con los
 * datos de cada frame/bone que participan en el blend. El array pertenece a esta
 * instancia; m_PedPosition sólo referencia una posición mantenida externamente.
 */
class CAnimBlendClumpData {
public:
    CAnimBlendLink m_AnimList; //!< Lista de asociaciones que se reproducen sobre este clump.
    union {
        uint32 m_NumFrameData; // Número de frame data en clumps skinned.
        uint32 m_NumBones;     // Mismo campo interpretado como número de bones en clumps no skinned.
    };
    CVector*            m_PedPosition; //!< Posición externa asociada al ped/clump; no owned.
    AnimBlendFrameData* m_FrameDatas;  //!< Bloque alineado propiedad de esta instancia.

public:
    static void InjectHooks();

    CAnimBlendClumpData();
    ~CAnimBlendClumpData();
    CAnimBlendClumpData* Constructor() { this->CAnimBlendClumpData::CAnimBlendClumpData(); return this; }
    CAnimBlendClumpData* Destructor()  { this->CAnimBlendClumpData::~CAnimBlendClumpData(); return this; }

    void ForAllFrames(void (*callback)(AnimBlendFrameData*, void*), void* data);

    /*!
     * @notsa
     * @brief Recorre todos los frames usando un functor/lambda.
     * @param Fn Función invocada con un puntero a cada AnimBlendFrameData.
    */
    template<typename Functor>
    void ForAllFramesF(Functor&& Fn) {
        for (auto& frame : std::span{ m_FrameDatas, m_NumFrameData }) {
            std::invoke(Fn, &frame);
        }
    }

    auto& GetAnims() { return m_AnimList; }

private:
    void ForAllFramesInSPR(void (*callback)(AnimBlendFrameData*, void*), void* data, uint32 a3);
    void LoadFramesIntoSPR();
public:

    AnimBlendFrameData& GetRootFrameData() const { assert(m_NumFrameData >= 1); return m_FrameDatas[0]; }
    void SetNumberOfBones(uint32 numBones);
    auto GetFrames() const { return m_FrameDatas; }
};
VALIDATE_SIZE(CAnimBlendClumpData, 0x14);
