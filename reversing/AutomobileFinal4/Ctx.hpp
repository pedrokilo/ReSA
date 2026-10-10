#pragma once
// Contexto de una ejecucion (un lado de un par): generador determinista y fixtures coherentes del mundo falso de coches.
// IMPORTANTE (dominio): todos los flotantes salen de rangos finitos y razonables (sin NaN/denormales/sNaN); las matrices son
// ortonormales; el handling es una copia de una entrada real del EXE; los model infos tienen la vtable real de CVehicleModelInfo.
#include "Support.hpp"

namespace AutomobileFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

// Modelos de coche reales (indices del juego) usados por los fixtures: todos son CAutomobile (no moto/barco/avion/helicoptero).
inline constexpr int kCarModels[]{
    400,401,402,404,405,407,408,409,410,411,412,413,415,416,418,419,420,421,422,423,424,426,427,428,429,431,432,433,434,436,437,438,
    439,440,442,443,444,445,451,455,456,457,458,459,466,467,470,471,472,473,474,475,477,478,479,480,483,489,490,492,494,495,496,497,
    498,499,500,502,503,504,505,506,507,508,516,517,518,524,525,526,527,528,529,530,531,532,533,534,535,536,540,541,542,543,544,545,
    546,547,548,549,550,551,552,554,555,556,557,558,559,560,561,562,565,566,567,568,571,572,573,574,575,576,578,579,580,582,585,587,
    588,589,596,597,598,599,600,601,602,603,604,605,609};

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    json out, ret;
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    // flotante uniforme en [lo,hi] (siempre finito)
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    bool chance(uint32 n){ return n && next()%n==0; }          // 1 de cada n
    uint32 below(uint32 n){ return n ? next()%n : 0u; }
    template<class T> T pick(std::initializer_list<T> l){ return l.begin()[next()%l.size()]; }
    int carModel(){ return kCarModels[next()%std::size(kCarModels)]; }
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    // Guarda un hash del buffer en la salida (para estado fuera de 'w' que el grupo registra por su cuenta)
    void Hash(const char* name,const void* p,size_t n){ out[name]=Fnv64(p,n); }
    json Bytes(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }

    // --- matrices ---
    // CMatrix: right 0x00, forward 0x10, up 0x20, pos 0x30 (flags/pads en 0x0C/0x1C/0x2C/0x3C); RwMatrix* de enlace 0x40, 'posee' 0x44.
    static void SetMatrix(void* m,const double r[9],float x,float y,float z){
        auto* p=static_cast<uint8*>(m); std::memset(p,0,0x48);
        for(int row=0;row<3;++row)for(int col=0;col<3;++col)At<float>(p,0x10*row+4*col)=float(r[3*row+col]);
        At<float>(p,0x30)=x; At<float>(p,0x34)=y; At<float>(p,0x38)=z; }
    // Rotacion aleatoria: giro completo en Z, inclinacion maxima 'tilt' (rad) en X/Y. Filas = right, forward, up.
    void RandRot(double r[9],float tilt){
        const double yaw=double(uni(-3.14f,3.14f)), pit=double(uni(-tilt,tilt)), rol=double(uni(-tilt,tilt));
        const double cy=std::cos(yaw), sy=std::sin(yaw), cp=std::cos(pit), sp=std::sin(pit), cr=std::cos(rol), sr=std::sin(rol);
        // R = Rz(yaw) * Rx(pit) * Ry(rol), filas right/forward/up
        r[0]=cy*cr-sy*sp*sr; r[1]=sy*cr+cy*sp*sr; r[2]=-cp*sr;
        r[3]=-sy*cp;         r[4]=cy*cp;          r[5]=sp;
        r[6]=cy*sr+sy*sp*cr; r[7]=sy*sr-cy*sp*cr; r[8]=cp*cr; }
    void RandMatrix(void* m,float tilt,float range=200.f){ double r[9]; RandRot(r,tilt); SetMatrix(m,r,uni(-range,range),uni(-range,range),uni(0.f,30.f)); }

    // --- model info / handling falsos ---
    // CVehicleModelInfo falso k con la vtable viva del EXE, colmodel w.colm[k], estructura w.vs[k] y handling w.hd[k].
    void FillMi(int k,int handlingId){
        auto* m=w.mi[k]; std::memset(m,0,MI_SIZE);
        static std::array<std::array<uintptr_t,17>,NMI> vtc{};
        std::memcpy(vtc[k].data(),reinterpret_cast<void*>(VT_VEHMI),17*4);
        At<void*>(m,0)=vtc[k].data();
        At<void*>(m,0x14)=w.colm[k];
        At<uint8>(m,0x4A)=uint8(handlingId);
        At<void*>(m,0x5C)=w.vs[k];
        At<int32>(m,0x3C)=0;                       // VEHICLE_TYPE_AUTOMOBILE
        std::memset(w.vs[k],0,VS_SIZE);
    }
    // Copia una entrada real del handling a w.hd[k] (dominio real) y rellena el handling de vuelo con valores razonables.
    void FillHandling(int k,int idx){
        std::memcpy(w.hd[k],reinterpret_cast<void*>(HANDLING_BASE+HD_SIZE*unsigned(idx)),HD_SIZE);
        std::memset(w.fh[k],0,FH_SIZE); for(size_t o=0;o<0x58;o+=4)At<float>(w.fh[k],o)=uni(0.001f,1.f);
    }
    // Registra el model info k en la tabla global de modelos (se restaura con RestoreAll: Save de MODELPTRS+4*390, 250 entradas).
    void InstallMi(int modelIndex,int k){ AtA<uint32>(MODELPTRS+4u*unsigned(modelIndex))=uint32(uintptr_t(w.mi[k])); }

    // Coche base k, coherente: matriz ortonormal, vtable falsa, tipo vehiculo, handling real, model info propio instalado en
    // la tabla de modelos. NO toca el resto de campos (cero): cada grupo rellena lo que su funcion lee.
    CAutomobile* BuildBase(int k,int modelIndex,float tilt=0.3f){
        auto* p=Cr(k); std::memset(p,0,CAR_SIZE);
        At<void*>(p,0)=vtFake.data();
        RandMatrix(w.mat[k],tilt); At<void*>(p,0x14)=w.mat[k];
        for(int q=0;q<3;++q)At<float>(p,4+4*q)=At<float>(w.mat[k],0x30+4*q);      // m_placement.pos
        At<float>(p,0x10)=0.f;
        auto* v=reinterpret_cast<CAutomobile*>(p);
        v->m_nModelIndex=uint16(modelIndex);
        v->SetType(ENTITY_TYPE_VEHICLE); v->SetStatus(STATUS_SIMPLE); v->SetUsesCollision(true);
        const int hid=int(below(100)); FillMi(k,hid); FillHandling(k,hid); InstallMi(modelIndex,k);
        v->m_pHandlingData=reinterpret_cast<tHandlingData*>(w.hd[k]);
        v->m_pFlyingHandlingData=reinterpret_cast<tFlyingHandlingData*>(w.fh[k]);
        v->m_nHandlingFlagsIntValue=v->m_pHandlingData->m_nHandlingFlags;
        v->m_fMass=v->m_pHandlingData->m_fMass; v->m_fTurnMass=v->m_pHandlingData->m_fTurnMass;
        v->m_vecCentreOfMass=v->m_pHandlingData->m_vecCentreOfMass;
        v->m_fHealth=1000.f; v->m_fElasticity=0.05f; v->m_fBuoyancyConstant=v->m_pHandlingData->m_fBuoyancyConstant;
        v->m_nVehicleType=VEHICLE_TYPE_AUTOMOBILE; v->m_nVehicleSubType=VEHICLE_TYPE_AUTOMOBILE;
        v->m_vehicleSpecialColIndex=-1;
        v->m_nCreatedBy=RANDOM_VEHICLE;
        return v;
    }

    // --- marcos RW falsos ---
    // Marco k con matriz de modelado identica a la de un coche aleatorio local; sin hijos ni padre.
    void* Frame(int k,float x=0.f,float y=0.f,float z=0.f){
        auto* f=w.fr[k]; std::memset(f,0,FR_SIZE);
        const double id[9]{1,0,0, 0,1,0, 0,0,1}; SetMatrix(f+0x10,id,x,y,z); SetMatrix(f+0x50,id,x,y,z);
        return f; }
    int treeN{};
};
}
