#pragma once

#include "RenderWare.h"

#include "Vector.h"
#include "Matrix.h"
#include "extensions/EntityRef.hpp"

class CEscalator {
public:
    // 0x717110
    CEscalator() {
        // Como el original: solo se escriben la palabra completa tras m_pAttachMatrix (0x74), m_StepObjectsCreated y
        // los 42 peldaños; el resto (incluida la referencia a la entidad) queda sin tocar.
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(&m_Rotation) + 0x44) = 0;
        m_StepObjectsCreated = 0;
        m_StepObjects.fill(nullptr);
    }
    // 0x717130 - Como el original: solo se destruye la matriz (sin limpiar la referencia de la entidad).
    ~CEscalator() {}

    void SwitchOff();
    void AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity);
    void Update();

    // Helpers
    CEntity* GetEntity() const {
        return m_Entity;
    }

    void Remove() {
        SwitchOff();
        m_Exist = false;
    }

private:
    friend class CEscalators;

    CVector                  m_Start;
    CVector                  m_Bottom;
    CVector                  m_Top;
    CVector                  m_End;
    CMatrix                  m_Rotation;
    bool                     m_Exist;
    uint8                    m_StepObjectsCreated;
    bool                     m_MoveDown;
    uint32                   m_NumIntermediatePlanes;
    uint32                   m_NumBottomPlanes;
    uint32                   m_NumTopPlanes;
    char                     _unused[8];
    union { CSphere          m_Bounding; }; // Como el original: el constructor no inicializa la esfera (CSphere pone ceros por defecto)
    float                    m_CurrentPosition;
    CEntity*                 m_Entity; // Como el original: puntero registrado con RegisterReference, sin limpieza automática
    std::array<CObject*, 42> m_StepObjects;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    auto Constructor() { this->CEscalator::CEscalator(); return this; }
    auto Destructor() { this->CEscalator::~CEscalator(); return this; }
};

VALIDATE_SIZE(CEscalator, 0x150);
