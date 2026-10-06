#pragma once

enum class eStencilShadowObjType : uint8 {
    NONE,
    OBJECT,
    VEHICLE
};

class CStencilShadowObject {
public:
    CEntity*              m_pOwner;
    int16                 m_NumShadowFaces;
    eStencilShadowObjType m_Type;
    uint32                m_FaceID;                  //!< Como el original (+8): capacidad en CVector (caras * 15)
    uint32                m_SizeOfShadowFacesData;   //!< Como el original (+0xC): CVector escritos hasta ahora
    CVector*              m_ShadowFacesData;
    CStencilShadowObject* m_pNext;
    CStencilShadowObject* m_pPrev;

public:
    static void InjectHooks();

    CStencilShadowObject() = default; // 0x70F8D0
    ~CStencilShadowObject() = default; // 0x70F8E0

    void Destroy();

    //! 0x711280: toma este objeto libre para una entidad con datos de sombra y lo pasa a la lista activa; devuelve this o nullptr
    CStencilShadowObject* Create(CEntity* owner, eStencilShadowObjType type);

private:
    CStencilShadowObject* Constructor() {
        this->CStencilShadowObject::CStencilShadowObject();
        return this;
    }

    CStencilShadowObject* Destructor() {
        this->CStencilShadowObject::~CStencilShadowObject();
        return this;
    }
};

VALIDATE_SIZE(CStencilShadowObject, 0x1C);
