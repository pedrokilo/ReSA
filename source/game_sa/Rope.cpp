/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRope
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Rope.h"
#include "Ropes.h"

void CRope::InjectHooks() {
    RH_ScopedClass(CRope);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ReleasePickedUpObject, 0x556030);
    RH_ScopedInstall(CreateHookObjectForRope, 0x556070);
    RH_ScopedInstall(UpdateWeightInRope, 0x5561B0);
    RH_ScopedInstall(Remove, 0x556780);
    RH_ScopedInstall(Render, 0x556800);
    RH_ScopedInstall(PickUpObject, 0x5569C0);
    RH_ScopedInstall(Update, 0x557530);
}

// inlined see 0x557959
// use switch like in Android?
// 0x555FB0
bool CRope::DoControlsApply() const {
    return    m_nType == eRopeType::CRANE_MAGNO && CRopes::PlayerControlsCrane == eControlledCrane::MAGNO_CRANE
           || m_nType == eRopeType::WRECKING_BALL && CRopes::PlayerControlsCrane == eControlledCrane::WRECKING_BALL
           || m_nType == eRopeType::CRANE_TROLLEY && CRopes::PlayerControlsCrane == eControlledCrane::LAS_VEGAS_CRANE
           || m_nType == eRopeType::QUARRY_CRANE_ARM && CRopes::PlayerControlsCrane == eControlledCrane::QUARRY_CRANE
           || m_nType == eRopeType::CRANE_MAGNET1
           || m_nType == eRopeType::MAGNET
           || m_nType == eRopeType::CRANE_HARNESS;
}

// 0x556030
void CRope::ReleasePickedUpObject() {
    if (m_pRopeAttachObject) {
        m_pRopeAttachObject->AsPhysical()->physicalFlags.bAttachedToEntity = false;
        m_pRopeAttachObject->AsPhysical()->physicalFlags.bCarriedByRope = false;
        m_pRopeAttachObject = nullptr;
    }
    m_pAttachedEntity->SetUsesCollision(true);
    m_nFlags1 = 60; // 6th, 7th bits set
}

// 0x556070
void CRope::CreateHookObjectForRope() {
    if (m_pAttachedEntity)
        return;

    using namespace ModelIndices;

    const auto modelIndex = [&]() -> ModelIndex {
        switch (m_nType) {
        case eRopeType::CRANE_MAGNET1:
        case eRopeType::CRANE_MAGNO:
        case eRopeType::QUARRY_CRANE_ARM:
        case eRopeType::CRANE_TROLLEY:
            return MI_CRANE_MAGNET;
        case eRopeType::CRANE_HARNESS:
            return MI_CRANE_HARNESS;
        case eRopeType::MAGNET:
            return MI_MINI_MAGNET;
        case eRopeType::WRECKING_BALL:
            return MI_WRECKING_BALL;
        case eRopeType::SWAT:
            return MODEL_INVALID; // Just so the assert below wont be hit.
        default:
            NOTSA_UNREACHABLE(); //assert(0);
        }
    }();
    if (modelIndex == ModelIndex{ MODEL_INVALID }) { // Must do it like this because `ModelIndex` is u16, `MODEL_ID` is i32, and u16 -1 casted to int32 is 0xffff...
        return;
    }

    auto* obj = new CObject(modelIndex, true);
    m_pAttachedEntity = obj;

    obj->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedEntity));
    obj->SetPosn(m_aSegments[NUM_ROPE_SEGMENTS - 1]);
    obj->m_nObjectType = OBJECT_TYPE_DECORATION;
    obj->SetIsStatic(false);
    obj->physicalFlags.bAttachedToEntity = true;

    CWorld::Add(m_pAttachedEntity);

    m_pRopeAttachObject = nullptr;
    m_nFlags1 = 0;
}

// 0x5561B0
int8 CRope::UpdateWeightInRope(float a2, float a3, float a4, int32 a5, float* a6) {
    const auto anchor = m_aSegments.data()[m_nSegments];
    auto direction = CVector{a2-anchor.x,a3-anchor.y,a4-anchor.z};
    m_aSegments[31] = CVector{a2,a3,a4};
    const auto length = float(31-int32(m_nSegments))*m_fTotalLength;
    const auto magnitude = [](const CVector& v){return std::sqrt((v.x*v.x+v.z*v.z)+v.y*v.y);};
    const auto distance = magnitude(direction);
    if (length <= distance) {
        const auto ratio=length/distance;
        a6[0]=direction.x*ratio+anchor.x;a6[1]=direction.y*ratio+anchor.y;a6[2]=direction.z*ratio+anchor.z;
        for(uint32 i=m_nSegments+1;i<32;++i){direction.Normalise();direction*=m_fTotalLength;const auto offset=float(i-m_nSegments);m_aSegments[i]=CVector{direction.x*offset+anchor.x,direction.y*offset+anchor.y,direction.z*offset+anchor.z};}
        return 1;
    }
    for(int pass=0;pass<6;++pass){
        for(int32 i=31;i>m_nSegments+1;--i){
            const auto previous=m_aSegments[i-1];const auto next=m_aSegments[i];const auto delta=next-previous;
            const auto distance=magnitude(delta);if(distance<=m_fTotalLength)break;
            const auto ratio=m_fTotalLength/distance;
            auto& point=m_aSegments[i-1];point=CVector{(previous.x-next.x)*ratio+next.x,(previous.y-next.y)*ratio+next.y,(previous.z-next.z)*ratio+next.z};
            const auto inverse=1.f/CTimer::GetTimeStep();m_aSpeed[i-1]=CVector{(point.x-previous.x)*inverse,(point.y-previous.y)*inverse,(point.z-previous.z)*inverse};
        }
        for(uint32 i=m_nSegments+1;i<31;++i){auto& point=m_aSegments[i];const auto& previous=m_aSegments[i-1];const auto delta=point-previous;const auto distance=magnitude(delta);if(distance>m_fTotalLength){const auto ratio=m_fTotalLength/distance;point=CVector{delta.x*ratio+previous.x,delta.y*ratio+previous.y,delta.z*ratio+previous.z};}}
    }
    for(int32 i=31;i>m_nSegments+1;--i){const auto next=m_aSegments[i];auto& previous=m_aSegments[i-1];const auto delta=next-previous;const auto distance=magnitude(delta);if(distance<=m_fTotalLength)break;const auto ratio=m_fTotalLength/distance;previous=CVector{(previous.x-next.x)*ratio+next.x,(previous.y-next.y)*ratio+next.y,(previous.z-next.z)*ratio+next.z};}
    return 0;
}

// 0x556780
void CRope::Remove() {
    m_nType = eRopeType::NONE;
    if (m_pRopeAttachObject)
        ReleasePickedUpObject();

    if (m_pAttachedEntity) {
        CWorld::Remove(m_pAttachedEntity);
        delete m_pAttachedEntity;
        m_pAttachedEntity = nullptr;
    }
}

// 0x556800
void CRope::Render() {
    // Note: Probably needs adjustments if `NUM_ROPE_SEGMENTS` is changed
    if (!TheCamera.IsSphereVisible(m_aSegments[NUM_ROPE_SEGMENTS / 2], 20.0f))
        return;

    if ((TheCamera.GetPosition() - m_aSegments[0]).Magnitude2D() >= 120.0f)
        return;

    DefinedState();

    const auto GetVertex = [](unsigned i) {
        return &TempBufferVertices.m_3d[i];
    };

    const RwRGBA color = { 0, 0, 0, 128 };
    for (auto i = 0u; i < NUM_ROPE_SEGMENTS; i++) {
        RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);
        RxObjSpace3DVertexSetPos(GetVertex(i), &m_aSegments[i]);
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(FALSE));

    if (RwIm3DTransform(TempBufferVertices.m_3d, NUM_ROPE_SEGMENTS, nullptr, 0)) {
        RxVertexIndex indices[] = { // *(RxVertexIndex(*)[64])0x8CD818
            0,  1,  1,  2,  2,  3,  3,  4,
            4,  5,  5,  6,  6,  7,  7,  8,
            8,  9,  9,  10, 10, 11, 11, 12,
            12, 13, 13, 14, 14, 15, 15, 16,
            16, 17, 17, 18, 18, 19, 19, 20,
            20, 21, 21, 22, 22, 23, 23, 24,
            24, 25, 25, 26, 26, 27, 27, 28,
            28, 29, 29, 30, 30, 31, 31, 32
        };
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, std::size(indices) - 2); // the last two indexes are not used
        RwIm3DEnd();
    }

    if (m_nType == eRopeType::QUARRY_CRANE_ARM) {
        const CVector pos[] = { m_aSegments[0], { 709.32f, 916.20f, 53.0f } }; // Hunter Quarry
        for (auto i = 0u; i < std::size(pos); i++) {
            RxObjSpace3DVertexSetPreLitColor(GetVertex(i), &color);
            RxObjSpace3DVertexSetPos(GetVertex(i), &pos[i]);
        }
        if (RwIm3DTransform(TempBufferVertices.m_3d, std::size(pos), nullptr, 0)) {
            RxVertexIndex indices[] = { 0, 1 };
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, std::size(indices));
            RwIm3DEnd();
        }
    }
}

// 0x5569C0
void CRope::PickUpObject(CEntity* obj) {
    if (m_pRopeAttachObject == obj)
        return;

    if (m_pRopeAttachObject)
        ReleasePickedUpObject();

    obj->RegisterReference(&m_pAttachedEntity);
    m_pRopeAttachObject = obj;

    // TODO: Move model => world space translation into CEntity
    // MultiplyMatrixWithVector should be used here
    CVector height = { {}, {}, CRopes::FindPickupHeight(obj) };
    m_pAttachedEntity->SetPosn(obj->GetPosition() + obj->GetMatrix().TransformVector(height));
    m_pAttachedEntity->SetUsesCollision(false);

    obj->AsPhysical()->physicalFlags.bAttachedToEntity = true;
    if (obj->GetIsTypeVehicle()) {
        if (obj->GetStatus() == STATUS_SIMPLE)
        {
            obj->SetStatus(STATUS_PHYSICS);
        }
    } else if (obj->GetIsTypeObject()) {
        if (obj->GetIsStatic()) {
            obj->AsObject()->SetIsStatic(false);
            obj->AsObject()->AddToMovingList();
            obj->AsObject()->m_nFakePhysics = 0;
        }
    }
}

// 0x557530
void CRope::Update() {
    const auto field=[]<typename T>(void* entity,size_t offset)->T&{return *reinterpret_cast<T*>(static_cast<uint8*>(entity)+offset);};
    const auto position=[&](CEntity* entity)->CVector&{auto* matrix=field.operator()<CMatrix*>(entity,0x14);return matrix?matrix->GetPosition():field.operator()<CVector>(entity,4);};
    const auto matrix=[&](CEntity* entity)->CMatrix&{return *field.operator()<CMatrix*>(entity,0x14);};
    const auto speed=[&](CEntity* entity)->CVector&{return field.operator()<CVector>(entity,0x44);};
    const auto flags=[&](CEntity* entity)->uint32&{return field.operator()<uint32>(entity,0x40);};
    const auto model=[&](CEntity* entity){return int32(field.operator()<int16>(entity,0x22));};
    const auto height=[&](CEntity* entity){const auto info=reinterpret_cast<uintptr_t*>(0xA9B0C8)[model(entity)];return StaticRef<float>(StaticRef<uintptr_t>(info+0x14)+0x14);};
    const auto top=[&](CEntity* entity){return matrix(entity)*CVector{0.f,0.f,height(entity)};};
    const auto matchingModel=[&](CEntity* entity,std::initializer_list<uintptr_t> addresses){const auto id=model(entity);for(const auto address:addresses)if(id==StaticRef<int32>(address))return true;return false;};
    const auto magnitude=[](const CVector& v){return std::sqrt((v.y*v.y+v.z*v.z)+v.x*v.x);};
    const auto visitPool=[&](uintptr_t poolAddress,int32 stride,auto&& callback){auto* pool=StaticRef<uint8*>(poolAddress);const auto storage=*reinterpret_cast<uint8**>(pool);const auto map=*reinterpret_cast<int8**>(pool+4);for(int32 i=*reinterpret_cast<int32*>(pool+8)-1;i>=0;--i)if(map[i]>=0&&callback(reinterpret_cast<CEntity*>(storage+i*stride)))break;};
    const float blend=float(std::pow(StaticRef<double>(0x863E40),double(CTimer::GetTimeStep())));
    const auto camera=TheCamera.GetPosition();const auto cameraDelta=camera-m_aSegments[0];
    if(std::sqrt(cameraDelta.y*cameraDelta.y+cameraDelta.x*cameraDelta.x)>=200.f)return;
    if(!(m_nFlags2&1)&&m_nTime<CTimer::GetTimeInMS()){m_aSpeed[0].z-=CTimer::GetTimeStep()*StaticRef<float>(0x863E38);m_aSegments[0].x=CTimer::GetTimeStep()*m_aSpeed[0].x+m_aSegments[0].x;m_aSegments[0].y=CTimer::GetTimeStep()*m_aSpeed[0].y+m_aSegments[0].y;m_aSegments[0].z=CTimer::GetTimeStep()*m_aSpeed[0].z+m_aSegments[0].z;}
    if((m_nFlags2&4)&&(CTimer::GetFrameCounter()&7)==2)m_fGroundZ=CWorld::FindGroundZFor3DCoord(m_aSegments[0],nullptr,nullptr);
    if((m_nFlags2&4)&&m_pRopeAttachObject&&((field.operator()<uint8>(m_pRopeAttachObject,0x36)&7)==2||matchingModel(m_pRopeAttachObject,{0x8CD6C8,0x8CD6CC,0x8CD6D0,0x8CD6D8})))m_fGroundZ=position(m_pAttachedEntity).z-.5f;
    for(uint32 i=m_nSegments+1;i<32;++i){const auto previous=m_aSegments[i];m_aSpeed[i].x+=float(int32(CGeneral::GetRandomNumber()&15)-8)*.001f;m_aSpeed[i].y+=float(int32(CGeneral::GetRandomNumber()&15)-8)*.001f;const auto weight=1.f-blend;m_aSpeed[i]=CVector{weight*m_aSpeed[i-1].x+blend*m_aSpeed[i].x,weight*m_aSpeed[i-1].y+blend*m_aSpeed[i].y,weight*m_aSpeed[i-1].z+blend*m_aSpeed[i].z};m_aSegments[i].z-=CTimer::GetTimeStep()*.15f;if(m_nFlags2&4)m_aSegments[i].z=std::max(m_fGroundZ+.3f,m_aSegments[i].z);const auto delta=m_aSegments[i]-m_aSegments[i-1];const auto ratio=m_fTotalLength/std::sqrt((delta.x*delta.x+delta.y*delta.y)+delta.z*delta.z);m_aSegments[i]=CVector{delta.x*ratio+m_aSegments[i-1].x,delta.y*ratio+m_aSegments[i-1].y,delta.z*ratio+m_aSegments[i-1].z};const auto inverse=1.f/CTimer::GetTimeStep();m_aSpeed[i]=CVector{inverse*(m_aSegments[i].x-previous.x),inverse*(m_aSegments[i].y-previous.y),inverse*(m_aSegments[i].z-previous.z)};}
    const auto type=uint8(m_nType);
    if(type>=1&&type<=7){
        const bool controls=DoControlsApply();
        if(controls){auto* pad=CPad::GetPad(0);if(type<=3){const auto axis=pad->GetCarGunUpDown();if((axis<0&&StaticRef<uint8>(0xA44496))||(axis>0&&StaticRef<uint8>(0xA44495)))m_fSegmentLength-=(float(axis)*CTimer::GetTimeStep())*StaticRef<float>(0x858C14);if(m_fSegmentLength>.84f)m_fSegmentLength=.84f;}else{
            const auto change=[&](uintptr_t enable,size_t padOffset,bool increase){if(!StaticRef<uint8>(enable))return;const auto amount=(float(field.operator()<int16>(pad,padOffset))*CTimer::GetTimeStep())*StaticRef<float>(0x858C14);if(amount>0.f&&(increase?amount+m_fSegmentLength<.9f:.01f<m_fSegmentLength-amount))AudioEngine.ReportMissionAudioEvent(104,reinterpret_cast<CPhysical*>(m_pRopeHolder),0.f,1.f);m_fSegmentLength+=increase?amount:-amount;};change(0xA44496,0x1C,true);change(0xA44495,0x20,false);
        }}
        m_fSegmentLength=std::clamp(m_fSegmentLength,.01f,.9f);
        auto* suspended=m_pRopeAttachObject?m_pRopeAttachObject:m_pAttachedEntity;float coupling=StaticRef<float>(0x8CD898);
        if(m_pRopeAttachObject){const auto mass=model(suspended)==428?750.f:field.operator()<float>(suspended,0x8C);coupling=(StaticRef<float>(0x8CD89C)*StaticRef<float>(0x863E30))*mass+coupling;coupling=std::min(coupling,.5f);field.operator()<uint8>(suspended,0xB8)=0;}else field.operator()<uint32>(suspended,0x1C)|=1;
        if(suspended){const auto oldPosition=position(suspended);CVector newPosition;
            if(UpdateWeightInRope(oldPosition.x,oldPosition.y,oldPosition.z,0x3DCCCCCD,&newPosition.x)){
                position(suspended)=newPosition;auto relativeSpeed=speed(suspended);auto holderOffset=m_aSegments[0]-position(m_pRopeHolder);auto holderSpeed=reinterpret_cast<CPhysical*>(m_pRopeHolder)->GetSpeed(holderOffset);if(type>=4&&type<=7){holderOffset=CVector{0.f,0.f,0.f};holderSpeed=CVector{0.f,0.f,0.f};}relativeSpeed-=holderSpeed;auto direction=position(suspended)-m_aSegments[0];direction.Normalise();const auto dot=(direction.z*relativeSpeed.z+direction.x*relativeSpeed.x)+direction.y*relativeSpeed.y;if(dot>0.f){relativeSpeed.x-=direction.x*dot;relativeSpeed.y-=dot*direction.y;relativeSpeed.z-=dot*direction.z;}const auto delta=CVector{(relativeSpeed.x+holderSpeed.x)-speed(suspended).x,(holderSpeed.y+relativeSpeed.y)-speed(suspended).y,(holderSpeed.z+relativeSpeed.z)-speed(suspended).z};const auto remaining=1.f-coupling;speed(suspended)=CVector{delta.x*remaining+speed(suspended).x,delta.y*remaining+speed(suspended).y,delta.z*remaining+speed(suspended).z};speed(m_pRopeHolder)=CVector{speed(m_pRopeHolder).x-delta.x*coupling,speed(m_pRopeHolder).y-delta.y*coupling,speed(m_pRopeHolder).z-delta.z*coupling};
                if(!m_pRopeAttachObject)matrix(suspended).ForceUpVector(-direction);
                else {const CMatrix before(matrix(suspended));matrix(suspended).ForceUpVector(-direction);const CMatrix after(matrix(suspended));for(const auto offset:{0u,0x10u,0x20u}){auto& row=field.operator()<CVector>(&matrix(suspended),offset);const auto& oldRow=field.operator()<CVector>(const_cast<CMatrix*>(&before),offset);const auto& newRow=field.operator()<CVector>(const_cast<CMatrix*>(&after),offset);row=CVector{newRow.x*.1f+oldRow.x*.9f,newRow.y*.1f+oldRow.y*.9f,newRow.z*.1f+oldRow.z*.9f};}}
            }
            if(m_pRopeAttachObject){matrix(m_pAttachedEntity)=matrix(m_pRopeAttachObject);speed(m_pAttachedEntity)=speed(m_pRopeAttachObject);position(m_pAttachedEntity)=top(m_pRopeAttachObject);}
        }
        const auto end=m_aSegments[31];
        if(m_pRopeAttachObject){if(controls&&StaticRef<uint8>(0xA44494)&&CPad::GetPad(0)->CarGunJustDown())ReleasePickedUpObject();if(m_pRopeAttachObject&&(flags(m_pRopeAttachObject)&0x20000000)){flags(m_pRopeAttachObject)&=0xFDFFFFFF;flags(m_pRopeAttachObject)&=0x7FFFFFFF;m_pRopeAttachObject=nullptr;field.operator()<uint32>(m_pAttachedEntity,0x1C)|=1;m_nFlags1=60;}if(m_pRopeAttachObject&&(field.operator()<uint8>(m_pRopeAttachObject,0x36)&7)==2&&field.operator()<int32>(m_pRopeAttachObject,0x594)==9&&field.operator()<uintptr_t>(m_pRopeAttachObject,0x460))ReleasePickedUpObject();}
        else if(m_nFlags1)--m_nFlags1;
        else {
            const auto shorten=[&](const CVector& pickup){if(end.z-pickup.z>0.f){const auto inverse=1.f/m_fMass;m_fSegmentLength=(m_fSegmentLength-(end.z-pickup.z)*inverse)-inverse*m_fTotalLength;if(m_fSegmentLength<=.01f)m_fSegmentLength=.01f;}};
            const auto attach=[&](CEntity* entity,const CVector& pickup,bool setPositionCall){m_pRopeAttachObject=entity;entity->RegisterReference(&m_pRopeAttachObject);flags(entity)|=0x80000000;if(setPositionCall)m_pAttachedEntity->SetPosn(top(entity));else position(m_pAttachedEntity)=top(entity);field.operator()<uint32>(m_pAttachedEntity,0x1C)&=0xFFFFFFFE;shorten(pickup);};
            if(type==2){visitPool(0xB74490,0x7C4,[&](CEntity* entity){auto* ped=reinterpret_cast<CPed*>(entity);if(field.operator()<int32>(entity,0x530)==55||ped->IsPlayer()||(field.operator()<uint32>(entity,0x46C)&0x100))return false;const auto pickup=position(entity);const auto delta=pickup-end;if(std::sqrt((delta.x*delta.x+delta.y*delta.y)+delta.z*delta.z)>=2.5f)return false;attach(entity,pickup,true);return true;});}
            else if(type==1||type==3||type==4||type==6){
                visitPool(0xB74494,0xA18,[&](CEntity* entity){const auto vehicleType=field.operator()<int32>(entity,0x594),id=model(entity);if(!(vehicleType==0||(vehicleType==9&&!field.operator()<uintptr_t>(entity,0x460))||vehicleType==1||id==473||id==539)||(flags(entity)&0x20000000)||!(field.operator()<uint8>(entity,0x42D)&0x10)||(type==3&&id!=564)||(field.operator()<uint8>(entity,0x4A8)&0x60))return false;const auto pickup=top(entity);const auto delta=pickup-end;if(magnitude(delta)>=2.5f)return false;m_pRopeAttachObject=entity;entity->RegisterReference(&m_pRopeAttachObject);flags(entity)|=0x80000000;auto& status=field.operator()<uint8>(entity,0x36);if((status&0xF8)==0x10)status=uint8((status&7)|0x18);position(m_pAttachedEntity)=top(entity);field.operator()<uint32>(m_pAttachedEntity,0x1C)&=0xFFFFFFFE;shorten(pickup);return true;});
            }
            if(!m_pRopeAttachObject&&(type==3||type==4||type==6||type==7)){
                visitPool(0xB7449C,0x19C,[&](CEntity* entity){if(!(field.operator()<uint32>(entity,0x140)&0x40000))return false;const bool crate=matchingModel(entity,{0x8CD6C8,0x8CD6CC,0x8CD6D0,0x8CD6D4,0x8CD6D8});const bool debris=matchingModel(entity,{0x8CD704,0x8CD708,0x8CD70C,0x8CD714})&&!field.operator()<uintptr_t>(entity,0xFC);const bool magnet=matchingModel(entity,{0x8CD724,0x8CD734,0x8CD738,0x8CD73C});if(!((type==4&&crate)||(type==7&&matchingModel(entity,{0x8CD6DC}))||(type==6&&debris)||(type==3&&magnet)))return false;
                    auto pickup=top(entity);int rotations=0;if(matchingModel(entity,{0x8CD6C8,0x8CD6CC,0x8CD6D0,0x8CD6D8})){const auto h=height(entity);const auto compare=[&](CVector candidate,int turns){const auto transformed=matrix(entity)*candidate;if(pickup.z<transformed.z){pickup=transformed;rotations=turns;}};compare({0.f,0.f,-h},2);compare({h,0.f,0.f},3);compare({-h,0.f,0.f},1);}const auto delta=pickup-end;if(std::sqrt((delta.x*delta.x+delta.y*delta.y)+delta.z*delta.z)>=2.5f)return false;attach(entity,pickup,true);if(field.operator()<uint32>(entity,0x1C)&0x40004){entity->SetIsStatic(false);reinterpret_cast<CPhysical*>(entity)->AddToMovingList();}flags(entity)|=0x2000000;for(int i=0;i<rotations;++i){auto& mat=matrix(entity);const auto oldRight=mat.GetRight();mat.GetRight()=mat.GetUp();mat.GetUp()=-oldRight;}return true;});
            }
        }
    }
    if(!(m_nFlags2&1)&&m_aSegments[0].z<-50.f)Remove();m_nFlags2&=0xFE;
}
