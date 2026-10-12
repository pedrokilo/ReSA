/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Interior_c
 * Reconstrucción: COMPLETA — 52/52 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 52/52 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifdef _MSC_VER
#pragma warning(push, 0)
#endif
#include "StdInc.h"
#include "Interior_c.h"
#include "InteriorGroup_c.h"
#include "Furniture_c.h"
#include "FurnitureManager_c.h"
#include "InteriorManager_c.h"
#include "Building.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Address labels retained for direct comparison with the extracted assembly.
#pragma warning(push)
#pragma warning(disable: 4102)

namespace {
int32 InteriorRandomCoordinate(int32 extent) {
    const int32 random = CGeneral::GetRandomNumber() & 0xFFFF;
    const float* factor = reinterpret_cast<const float*>(0x858B14);
    const auto convert = reinterpret_cast<void(__cdecl*)()>(0x821B40);
    int32 result;
    __asm {
        fild random
        mov eax, factor
        fmul dword ptr [eax]
        fimul extent
        call convert
        mov result, eax
    }
    return result;
}
void InteriorLocalTileCentre(int32 negativeWidth, int32 negativeHeight, int32 negativeDepth, float offsetX, float offsetY, CVector* output) {
    const float half = 0.5f;
    __asm {
        mov eax, output
        fild negativeWidth
        fmul half
        fadd offsetX
        fadd half
        fstp dword ptr [eax]
        fild negativeHeight
        fmul half
        fadd offsetY
        fadd half
        fstp dword ptr [eax+4]
        fild negativeDepth
        fmul half
        fstp dword ptr [eax+8]
    }
}
}
namespace {
bool InteriorProjectionInside(const RwV3d* axis, const CVector* delta, int32 dimension, float bias) {
    const float half = 0.5f;
    uint16 status;
    __asm {
        mov ecx, axis
        mov edx, delta
        fld dword ptr [ecx+4]
        fmul dword ptr [edx+4]
        fld dword ptr [ecx]
        fmul dword ptr [edx]
        faddp st(1), st(0)
        fld dword ptr [ecx+8]
        fmul dword ptr [edx+8]
        faddp st(1), st(0)
        fabs
        fild dimension
        fmul half
        fadd bias
        fcompp
        fnstsw ax
        mov status, ax
    }
    const uint8 flags = static_cast<uint8>(status >> 8) & 5;
    return flags == 0 || flags == 5;
}
}
namespace {
uint32 InteriorTruncate(float value) {
    const auto convert = reinterpret_cast<void(__cdecl*)()>(0x821B40);
    uint32 result;
    __asm { fld value
        call convert
        mov result, eax }
    return result;
}
uint32 InteriorPositionSeed(const CVector* position, int32 extra) {
    const auto convert = reinterpret_cast<void(__cdecl*)()>(0x821B40);
    uint32 result;
    __asm { mov eax, position
        fld dword ptr [eax+8]
        fmul dword ptr [eax+4]
        fmul dword ptr [eax]
        fiadd extra
        call convert
        mov result, eax }
    return result;
}
bool InteriorNearEntrance(const CVector* camera, const CVector* centre) {
    const float hundred = 100.0f;
    uint16 status;
    __asm { mov ecx, camera
        mov edx, centre
        fld dword ptr [ecx]
        fsub dword ptr [edx]
        fld dword ptr [ecx+4]
        fsub dword ptr [edx+4]
        fld st(0)
        fmul st(0), st(1)
        fld st(2)
        fmul st(0), st(3)
        faddp st(1), st(0)
        fcomp hundred
        fnstsw ax
        fstp st(0)
        fstp st(0)
        mov status, ax }
    const uint8 flags = static_cast<uint8>(status >> 8) & 5;
    return flags == 1 || flags == 4;
}
}

void Interior_c::InjectHooks() {
    RH_ScopedClass(Interior_c);
    RH_ScopedCategory("Interior");

    //RH_ScopedInstall(Constructor, 0x5921D0, { .reversed = false });
    //RH_ScopedInstall(Destructor, 0x591360, { .reversed = false });

    RH_ScopedInstall(Bedroom_AddTableItem, 0x593F10, { .reversed = true });
    RH_ScopedInstall(FurnishBedroom, 0x593FC0, { .reversed = true });
    RH_ScopedInstall(Kitchen_FurnishEdges, 0x596930, { .reversed = true });
    RH_ScopedInstall(FurnishKitchen, 0x5970B0, { .reversed = true });
    RH_ScopedInstall(Lounge_AddTV, 0x597240, { .reversed = true });
    RH_ScopedInstall(Lounge_AddHifi, 0x597430, { .reversed = true });
    RH_ScopedInstall(Lounge_AddChairInfo, 0x5974E0, { .reversed = true });
    RH_ScopedInstall(Lounge_AddSofaInfo, 0x5975C0, { .reversed = true });
    RH_ScopedInstall(FurnishLounge, 0x597740, { .reversed = true });
    RH_ScopedInstall(Office_PlaceEdgeFillers, 0x599210, { .reversed = true });
    RH_ScopedInstall(Office_PlaceDesk, 0x5993E0, { .reversed = true });
    RH_ScopedInstall(Office_PlaceEdgeDesks, 0x5995B0, { .reversed = true });
    RH_ScopedInstall(Office_FurnishEdges, 0x599770, { .reversed = true });
    RH_ScopedInstall(Office_PlaceDeskQuad, 0x599960, { .reversed = true });
    RH_ScopedInstall(Office_FurnishCenter, 0x599A30, { .reversed = true });
    RH_ScopedInstall(FurnishOffice, 0x599AF0, { .reversed = true });
    RH_ScopedInstall(Shop_Place3PieceUnit, 0x599BB0, { .reversed = true });
    RH_ScopedInstall(Shop_PlaceEdgeUnits, 0x599DC0, { .reversed = true });
    RH_ScopedInstall(Shop_PlaceCounter, 0x599EF0, { .reversed = true });
    RH_ScopedInstall(Shop_PlaceFixedUnits, 0x59A030, { .reversed = true });
    RH_ScopedInstall(Shop_FurnishCeiling, 0x59A130, { .reversed = true });
    RH_ScopedInstall(Shop_AddShelfInfo, 0x59A140, { .reversed = true });
    RH_ScopedInstall(Shop_FurnishEdges, 0x59A1B0, { .reversed = true });
    RH_ScopedInstall(GetBoundingBox, 0x593DB0, { .reversed = true });
    RH_ScopedInstall(Init, 0x593BF0, { .reversed = true });
    RH_ScopedInstall(ResetTiles, 0x593910, { .reversed = true });
    RH_ScopedInstall(PlaceObject, 0x5934E0, { .reversed = true });
    RH_ScopedInstall(GetFurnitureEntity, 0x5913B0, { .reversed = true });
    RH_ScopedInstall(IsPtInside, 0x5913E0, { .reversed = true });
    RH_ScopedInstall(CalcMatrix, 0x5914D0, { .reversed = true });
    RH_ScopedInstall(Furnish, 0x591590, { .reversed = true });
    RH_ScopedInstall(Unfurnish, 0x5915D0, { .reversed = true });
    RH_ScopedInstall(CheckTilesEmpty, 0x591680, { .reversed = true });
    RH_ScopedInstall(SetTilesStatus, 0x591700, { .reversed = true });
    RH_ScopedInstall(SetCornerTiles, 0x5917C0, { .reversed = true });
    RH_ScopedInstall(GetTileStatus, 0x5918E0, { .reversed = true });
    RH_ScopedInstall(GetNumEmptyTiles, 0x591920, { .reversed = true });
    RH_ScopedInstall(GetRandomTile, 0x591B20, { .reversed = true });
    RH_ScopedInstall(Shop_FurnishAisles, 0x59A590, { .reversed = true });
    RH_ScopedInstall(GetTileCentre, 0x591BD0, { .reversed = true });
    RH_ScopedInstall(AddGotoPt, 0x591D20, { .reversed = true });
    RH_ScopedInstall(AddInteriorInfo, 0x591E40, { .reversed = true });
    RH_ScopedInstall(AddPickups, 0x591F90, { .reversed = true });
    RH_ScopedInstall(Exit, 0x592230, { .reversed = true });
    RH_ScopedInstall(FindBoundingBox, 0x5922C0, { .reversed = true });
    RH_ScopedInstall(CalcExitPts, 0x5924A0, { .reversed = true });
    RH_ScopedInstall(IsVisible, 0x5929F0, { .reversed = true });
    RH_ScopedInstall(PlaceFurniture, 0x592AA0, { .reversed = true });
    RH_ScopedInstall(PlaceFurnitureOnWall, 0x593120, { .reversed = true });
    RH_ScopedInstall(PlaceFurnitureInCorner, 0x593340, { .reversed = true });
    RH_ScopedInstall(FindEmptyTiles, 0x591C50, { .reversed = true });
    RH_ScopedInstall(FurnishShop, 0x59A790, { .reversed = true });
}

// 0x593BF0
int32 Interior_c::Init(const CVector& pos) {
    CalcMatrix(const_cast<CVector*>(&pos));
    ResetTiles();
    auto* entity = m_pGroup->GetEntity();
    CMatrix snapshot(reinterpret_cast<RwMatrix*>(&entity->GetMatrix()), false);
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    if (box[0] != 99) {
        const auto* other = StaticRef<const uint8*>(0xBB3DAC);
        uint32 seed;
        if (other) {
            const uint32 a = InteriorTruncate(*reinterpret_cast<const float*>(other + 0x18));
            const uint32 b = InteriorTruncate(*reinterpret_cast<const float*>(other + 0x14));
            const uint32 c = InteriorTruncate(*reinterpret_cast<const float*>(other + 0x8));
            const uint32 d = InteriorTruncate(snapshot.GetPosition().z);
            const uint32 e = InteriorTruncate(snapshot.GetPosition().y);
            const uint32 f = InteriorTruncate(snapshot.GetPosition().x);
            seed = a * b * c + d * e * f + box[0x1E];
        } else seed = InteriorPositionSeed(&snapshot.GetPosition(), box[0x1E]);
        plugin::Call<0x821B11, uint32>(seed);
    }
    field_40C = 0;
    m_interiorInfosCount = 0;
    switch (box[0]) {
    case 0: FurnishShop(0); break;
    case 1: FurnishOffice(); break;
    case 2: FurnishLounge(); break;
    case 3: FurnishBedroom(); break;
    case 4: FurnishKitchen(); break;
    }
    CalcExitPts();
    if (!g_interiorMan.HasInteriorHadStealDataSetup(this)) {
        auto& count = StaticRef<int32>(0xBB3914);
        if (count < 64) { reinterpret_cast<int32*>(0xBB3918)[count] = m_interiorId; ++count; }
    }
    if (box[0] == 2 || box[0] == 3) AddPickups();
    return 1;
}

// 0x592230
void Interior_c::Exit() {
    CPickups::RemovePickUpsInArea(m_matrix.pos.x - 50.0f, m_matrix.pos.x + 50.0f,
                                m_matrix.pos.y - 50.0f, m_matrix.pos.y + 50.0f,
                                m_matrix.pos.z - 50.0f, m_matrix.pos.z + 50.0f);
    Unfurnish();
}

// 0x593F10
// Exact native call boundaries preserve x87 and all incoming registers.
static __declspec(naked) void InteriorHomeCall00407180() { __asm { push 000407180h
    ret } }
static __declspec(naked) void InteriorHomeCall005911E0() { __asm { push 0005911E0h
    ret } }
static __declspec(naked) void InteriorHomeCall00591220() { __asm { push 000591220h
    ret } }
static __declspec(naked) void InteriorHomeCall00591700() { __asm { push 000591700h
    ret } }
static __declspec(naked) void InteriorHomeCall005917C0() { __asm { push 0005917C0h
    ret } }
static __declspec(naked) void InteriorHomeCall005918E0() { __asm { push 0005918E0h
    ret } }
static __declspec(naked) void InteriorHomeCall00591C50() { __asm { push 000591C50h
    ret } }
static __declspec(naked) void InteriorHomeCall00591D20() { __asm { push 000591D20h
    ret } }
static __declspec(naked) void InteriorHomeCall00591E40() { __asm { push 000591E40h
    ret } }
static __declspec(naked) void InteriorHomeCall00592AA0() { __asm { push 000592AA0h
    ret } }
static __declspec(naked) void InteriorHomeCall00593120() { __asm { push 000593120h
    ret } }
static __declspec(naked) void InteriorHomeCall00593340() { __asm { push 000593340h
    ret } }
static __declspec(naked) void InteriorHomeCall005934E0() { __asm { push 0005934E0h
    ret } }
static __declspec(naked) void InteriorHomeCall00593F10() { __asm { push 000593F10h
    ret } }
static __declspec(naked) void InteriorHomeCall00596930() { __asm { push 000596930h
    ret } }
static __declspec(naked) void InteriorHomeCall00597240() { __asm { push 000597240h
    ret } }
static __declspec(naked) void InteriorHomeCall00597430() { __asm { push 000597430h
    ret } }
static __declspec(naked) void InteriorHomeCall005974E0() { __asm { push 0005974E0h
    ret } }
static __declspec(naked) void InteriorHomeCall005975C0() { __asm { push 0005975C0h
    ret } }
static __declspec(naked) void InteriorHomeCall00821B1E() { __asm { push 000821B1Eh
    ret } }
static __declspec(naked) void InteriorHomeCall00821B40() { __asm { push 000821B40h
    ret } }

static __declspec(naked) void InteriorHomeNativeBedroom_AddTableItem() {
    __asm {
    InteriorHomeBedroom_AddTableItem_00593F10:
        push ecx
    InteriorHomeBedroom_AddTableItem_00593F11:
        fild dword ptr [esp+14h]
    InteriorHomeBedroom_AddTableItem_00593F15:
        mov eax,dword ptr [esp+10h]
    InteriorHomeBedroom_AddTableItem_00593F19:
        test eax,eax
    InteriorHomeBedroom_AddTableItem_00593F1B:
        push esi
    InteriorHomeBedroom_AddTableItem_00593F1C:
        fstp dword ptr [esp+4]
    InteriorHomeBedroom_AddTableItem_00593F20:
        mov esi,ecx
    InteriorHomeBedroom_AddTableItem_00593F22:
        fild dword ptr [esp+1Ch]
    InteriorHomeBedroom_AddTableItem_00593F26:
        fstp dword ptr [esp+18h]
    InteriorHomeBedroom_AddTableItem_00593F2A:
        je InteriorHomeBedroom_AddTableItem_00593F4B
    InteriorHomeBedroom_AddTableItem_00593F2C:
        cmp eax,2
    InteriorHomeBedroom_AddTableItem_00593F2F:
        je InteriorHomeBedroom_AddTableItem_00593F4B
    InteriorHomeBedroom_AddTableItem_00593F31:
        cmp eax,1
    InteriorHomeBedroom_AddTableItem_00593F34:
        je InteriorHomeBedroom_AddTableItem_00593F3B
    InteriorHomeBedroom_AddTableItem_00593F36:
        cmp eax,3
    InteriorHomeBedroom_AddTableItem_00593F39:
        jne InteriorHomeBedroom_AddTableItem_00593F59
    InteriorHomeBedroom_AddTableItem_00593F3B:
        fld dword ptr [esp+18h]
    InteriorHomeBedroom_AddTableItem_00593F3F:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeBedroom_AddTableItem_00593F45:
        fstp dword ptr [esp+18h]
    InteriorHomeBedroom_AddTableItem_00593F49:
        jmp InteriorHomeBedroom_AddTableItem_00593F59
    InteriorHomeBedroom_AddTableItem_00593F4B:
        fld dword ptr [esp+4]
    InteriorHomeBedroom_AddTableItem_00593F4F:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeBedroom_AddTableItem_00593F55:
        fstp dword ptr [esp+4]
    InteriorHomeBedroom_AddTableItem_00593F59:
        mov eax,dword ptr [esi+14h]
    InteriorHomeBedroom_AddTableItem_00593F5C:
        mov edx,dword ptr [esp+10h]
    InteriorHomeBedroom_AddTableItem_00593F60:
        xor ecx,ecx
    InteriorHomeBedroom_AddTableItem_00593F62:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeBedroom_AddTableItem_00593F65:
        mov eax,dword ptr [esp+0Ch]
    InteriorHomeBedroom_AddTableItem_00593F69:
        push ecx
    InteriorHomeBedroom_AddTableItem_00593F6A:
        push 0FFFFFFFFh
    InteriorHomeBedroom_AddTableItem_00593F6C:
        push edx
    InteriorHomeBedroom_AddTableItem_00593F6D:
        push eax
    InteriorHomeBedroom_AddTableItem_00593F6E:
        mov ecx,0BAB380h
    InteriorHomeBedroom_AddTableItem_00593F73:
        call InteriorHomeCall005911E0
    InteriorHomeBedroom_AddTableItem_00593F78:
        fild dword ptr [esp+20h]
    InteriorHomeBedroom_AddTableItem_00593F7C:
        push ecx
    InteriorHomeBedroom_AddTableItem_00593F7D:
        mov ecx,esi
    InteriorHomeBedroom_AddTableItem_00593F7F:
        fmul dword ptr ds:[0085991Ch]
    InteriorHomeBedroom_AddTableItem_00593F85:
        fstp dword ptr [esp]
    InteriorHomeBedroom_AddTableItem_00593F88:
        push 3F000000h
    InteriorHomeBedroom_AddTableItem_00593F8D:
        fld dword ptr [esp+20h]
    InteriorHomeBedroom_AddTableItem_00593F91:
        sub esp,8
    InteriorHomeBedroom_AddTableItem_00593F94:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeBedroom_AddTableItem_00593F9A:
        fstp dword ptr [esp+4]
    InteriorHomeBedroom_AddTableItem_00593F9E:
        fld dword ptr [esp+14h]
    InteriorHomeBedroom_AddTableItem_00593FA2:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeBedroom_AddTableItem_00593FA8:
        fstp dword ptr [esp]
    InteriorHomeBedroom_AddTableItem_00593FAB:
        push eax
    InteriorHomeBedroom_AddTableItem_00593FAC:
        push 1
    InteriorHomeBedroom_AddTableItem_00593FAE:
        call InteriorHomeCall005934E0
    InteriorHomeBedroom_AddTableItem_00593FB3:
        pop esi
    InteriorHomeBedroom_AddTableItem_00593FB4:
        pop ecx
    InteriorHomeBedroom_AddTableItem_00593FB5:
        ret 18h
    }
}

CObject* Interior_c::Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7) {
    return reinterpret_cast<CObject*(__thiscall*)(Interior_c*, int32, int32, int32, int32, int32, int32)>(&InteriorHomeNativeBedroom_AddTableItem)(this, a2, a3, a4, a5, a6, a7);
}

// 0x593FC0
static __declspec(naked) void InteriorHomeNativeFurnishBedroom() {
    __asm {
    InteriorHomeFurnishBedroom_00593FC0:
        sub esp,20h
    InteriorHomeFurnishBedroom_00593FC3:
        push ebx
    InteriorHomeFurnishBedroom_00593FC4:
        push ebp
    InteriorHomeFurnishBedroom_00593FC5:
        push esi
    InteriorHomeFurnishBedroom_00593FC6:
        mov esi,ecx
    InteriorHomeFurnishBedroom_00593FC8:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_00593FCB:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_00593FCD:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_00593FD0:
        push edi
    InteriorHomeFurnishBedroom_00593FD1:
        mov ebx,1
    InteriorHomeFurnishBedroom_00593FD6:
        push ecx
    InteriorHomeFurnishBedroom_00593FD7:
        push ebx
    InteriorHomeFurnishBedroom_00593FD8:
        push 3
    InteriorHomeFurnishBedroom_00593FDA:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_00593FDF:
        call InteriorHomeCall00591220
    InteriorHomeFurnishBedroom_00593FE4:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_00593FE7:
        push 0
    InteriorHomeFurnishBedroom_00593FE9:
        push 7
    InteriorHomeFurnishBedroom_00593FEB:
        mov byte ptr [esi+00000791h],al
    InteriorHomeFurnishBedroom_00593FF1:
        movsx eax,byte ptr [edx+5]
    InteriorHomeFurnishBedroom_00593FF5:
        push 2
    InteriorHomeFurnishBedroom_00593FF7:
        push 2
    InteriorHomeFurnishBedroom_00593FF9:
        push 0
    InteriorHomeFurnishBedroom_00593FFB:
        dec eax
    InteriorHomeFurnishBedroom_00593FFC:
        push eax
    InteriorHomeFurnishBedroom_00593FFD:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00593FFF:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_00594004:
        push 0
    InteriorHomeFurnishBedroom_00594006:
        push 0
    InteriorHomeFurnishBedroom_00594008:
        push 0
    InteriorHomeFurnishBedroom_0059400A:
        push 0
    InteriorHomeFurnishBedroom_0059400C:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_00594010:
        push ecx
    InteriorHomeFurnishBedroom_00594011:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_00594015:
        push edx
    InteriorHomeFurnishBedroom_00594016:
        push 0
    InteriorHomeFurnishBedroom_00594018:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059401A:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059401C:
        push ebx
    InteriorHomeFurnishBedroom_0059401D:
        push 0
    InteriorHomeFurnishBedroom_0059401F:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594021:
        push 0
    InteriorHomeFurnishBedroom_00594023:
        push 3
    InteriorHomeFurnishBedroom_00594025:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594027:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_0059402C:
        mov ebp,eax
    InteriorHomeFurnishBedroom_0059402E:
        mov eax,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_00594032:
        test eax,eax
    InteriorHomeFurnishBedroom_00594034:
        jle InteriorHomeFurnishBedroom_00594146
    InteriorHomeFurnishBedroom_0059403A:
        movsx ecx,byte ptr [esi+00000791h]
    InteriorHomeFurnishBedroom_00594041:
        push 0
    InteriorHomeFurnishBedroom_00594043:
        push 0
    InteriorHomeFurnishBedroom_00594045:
        push 0
    InteriorHomeFurnishBedroom_00594047:
        push 0
    InteriorHomeFurnishBedroom_00594049:
        push 0
    InteriorHomeFurnishBedroom_0059404B:
        push 0
    InteriorHomeFurnishBedroom_0059404D:
        push 0
    InteriorHomeFurnishBedroom_0059404F:
        dec eax
    InteriorHomeFurnishBedroom_00594050:
        push eax
    InteriorHomeFurnishBedroom_00594051:
        mov eax,dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_00594055:
        push eax
    InteriorHomeFurnishBedroom_00594056:
        push ebx
    InteriorHomeFurnishBedroom_00594057:
        push 0
    InteriorHomeFurnishBedroom_00594059:
        push ecx
    InteriorHomeFurnishBedroom_0059405A:
        push ebx
    InteriorHomeFurnishBedroom_0059405B:
        push 3
    InteriorHomeFurnishBedroom_0059405D:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059405F:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_00594064:
        test eax,eax
    InteriorHomeFurnishBedroom_00594066:
        je InteriorHomeFurnishBedroom_00594146
    InteriorHomeFurnishBedroom_0059406C:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_00594070:
        cmp eax,ebx
    InteriorHomeFurnishBedroom_00594072:
        jne InteriorHomeFurnishBedroom_00594092
    InteriorHomeFurnishBedroom_00594074:
        mov dword ptr [esp+24h],ebx
    InteriorHomeFurnishBedroom_00594078:
        mov ebx,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_0059407C:
        dec ebx
    InteriorHomeFurnishBedroom_0059407D:
        mov ecx,2
    InteriorHomeFurnishBedroom_00594082:
        mov eax,3
    InteriorHomeFurnishBedroom_00594087:
        mov dword ptr [esp+20h],ebx
    InteriorHomeFurnishBedroom_0059408B:
        mov edi,ecx
    InteriorHomeFurnishBedroom_0059408D:
        jmp InteriorHomeFurnishBedroom_00594114
    InteriorHomeFurnishBedroom_00594092:
        cmp eax,3
    InteriorHomeFurnishBedroom_00594095:
        jne InteriorHomeFurnishBedroom_005940BD
    InteriorHomeFurnishBedroom_00594097:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_0059409A:
        movzx edx,byte ptr [edx+2]
    InteriorHomeFurnishBedroom_0059409E:
        mov ebx,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_005940A2:
        lea ecx,[edx-2]
    InteriorHomeFurnishBedroom_005940A5:
        dec ebx
    InteriorHomeFurnishBedroom_005940A6:
        mov dword ptr [esp+24h],ecx
    InteriorHomeFurnishBedroom_005940AA:
        mov eax,4
    InteriorHomeFurnishBedroom_005940AF:
        mov dword ptr [esp+20h],ebx
    InteriorHomeFurnishBedroom_005940B3:
        mov ecx,2
    InteriorHomeFurnishBedroom_005940B8:
        lea edi,[edx-3]
    InteriorHomeFurnishBedroom_005940BB:
        jmp InteriorHomeFurnishBedroom_00594114
    InteriorHomeFurnishBedroom_005940BD:
        test eax,eax
    InteriorHomeFurnishBedroom_005940BF:
        jne InteriorHomeFurnishBedroom_005940E4
    InteriorHomeFurnishBedroom_005940C1:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005940C4:
        movzx edx,byte ptr [edx+3]
    InteriorHomeFurnishBedroom_005940C8:
        mov edi,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_005940CC:
        lea ecx,[edx-2]
    InteriorHomeFurnishBedroom_005940CF:
        dec edi
    InteriorHomeFurnishBedroom_005940D0:
        mov dword ptr [esp+20h],ecx
    InteriorHomeFurnishBedroom_005940D4:
        mov ecx,ebx
    InteriorHomeFurnishBedroom_005940D6:
        mov eax,3
    InteriorHomeFurnishBedroom_005940DB:
        mov dword ptr [esp+24h],edi
    InteriorHomeFurnishBedroom_005940DF:
        lea ebx,[edx-3]
    InteriorHomeFurnishBedroom_005940E2:
        jmp InteriorHomeFurnishBedroom_00594114
    InteriorHomeFurnishBedroom_005940E4:
        cmp eax,2
    InteriorHomeFurnishBedroom_005940E7:
        jne InteriorHomeFurnishBedroom_00594104
    InteriorHomeFurnishBedroom_005940E9:
        mov edi,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_005940ED:
        dec edi
    InteriorHomeFurnishBedroom_005940EE:
        mov dword ptr [esp+20h],ebx
    InteriorHomeFurnishBedroom_005940F2:
        mov ecx,ebx
    InteriorHomeFurnishBedroom_005940F4:
        mov eax,4
    InteriorHomeFurnishBedroom_005940F9:
        mov dword ptr [esp+24h],edi
    InteriorHomeFurnishBedroom_005940FD:
        mov ebx,2
    InteriorHomeFurnishBedroom_00594102:
        jmp InteriorHomeFurnishBedroom_00594114
    InteriorHomeFurnishBedroom_00594104:
        mov eax,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594108:
        mov ecx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_0059410C:
        mov edi,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594110:
        mov ebx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594114:
        fild dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_00594118:
        push ebp
    InteriorHomeFurnishBedroom_00594119:
        push ecx
    InteriorHomeFurnishBedroom_0059411A:
        sub esp,8
    InteriorHomeFurnishBedroom_0059411D:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_00594121:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594123:
        fild dword ptr [esp+34h]
    InteriorHomeFurnishBedroom_00594127:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_0059412A:
        push eax
    InteriorHomeFurnishBedroom_0059412B:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishBedroom_00594130:
        push 0
    InteriorHomeFurnishBedroom_00594132:
        push 2
    InteriorHomeFurnishBedroom_00594134:
        push 1
    InteriorHomeFurnishBedroom_00594136:
        push 1
    InteriorHomeFurnishBedroom_00594138:
        push ebx
    InteriorHomeFurnishBedroom_00594139:
        push edi
    InteriorHomeFurnishBedroom_0059413A:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059413C:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_00594141:
        mov ebx,1
    InteriorHomeFurnishBedroom_00594146:
        mov edx,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_0059414A:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_0059414E:
        movsx ecx,byte ptr [esi+00000791h]
    InteriorHomeFurnishBedroom_00594155:
        push 0
    InteriorHomeFurnishBedroom_00594157:
        push 0
    InteriorHomeFurnishBedroom_00594159:
        push 0
    InteriorHomeFurnishBedroom_0059415B:
        push 0
    InteriorHomeFurnishBedroom_0059415D:
        push 0
    InteriorHomeFurnishBedroom_0059415F:
        push 0
    InteriorHomeFurnishBedroom_00594161:
        push 0
    InteriorHomeFurnishBedroom_00594163:
        add edx,2
    InteriorHomeFurnishBedroom_00594166:
        push edx
    InteriorHomeFurnishBedroom_00594167:
        push eax
    InteriorHomeFurnishBedroom_00594168:
        push ebx
    InteriorHomeFurnishBedroom_00594169:
        push 0
    InteriorHomeFurnishBedroom_0059416B:
        push ecx
    InteriorHomeFurnishBedroom_0059416C:
        push ebx
    InteriorHomeFurnishBedroom_0059416D:
        push 3
    InteriorHomeFurnishBedroom_0059416F:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594171:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_00594176:
        test eax,eax
    InteriorHomeFurnishBedroom_00594178:
        je InteriorHomeFurnishBedroom_0059425B
    InteriorHomeFurnishBedroom_0059417E:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_00594182:
        cmp eax,ebx
    InteriorHomeFurnishBedroom_00594184:
        jne InteriorHomeFurnishBedroom_005941A6
    InteriorHomeFurnishBedroom_00594186:
        mov dword ptr [esp+20h],ebx
    InteriorHomeFurnishBedroom_0059418A:
        mov ebx,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_0059418E:
        add ebx,2
    InteriorHomeFurnishBedroom_00594191:
        mov eax,4
    InteriorHomeFurnishBedroom_00594196:
        mov dword ptr [esp+24h],ebx
    InteriorHomeFurnishBedroom_0059419A:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_0059419C:
        mov edi,2
    InteriorHomeFurnishBedroom_005941A1:
        jmp InteriorHomeFurnishBedroom_00594229
    InteriorHomeFurnishBedroom_005941A6:
        cmp eax,3
    InteriorHomeFurnishBedroom_005941A9:
        jne InteriorHomeFurnishBedroom_005941CB
    InteriorHomeFurnishBedroom_005941AB:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005941AE:
        movzx edx,byte ptr [edx+2]
    InteriorHomeFurnishBedroom_005941B2:
        mov ebx,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_005941B6:
        lea ecx,[edx-2]
    InteriorHomeFurnishBedroom_005941B9:
        add ebx,2
    InteriorHomeFurnishBedroom_005941BC:
        mov dword ptr [esp+20h],ecx
    InteriorHomeFurnishBedroom_005941C0:
        mov dword ptr [esp+24h],ebx
    InteriorHomeFurnishBedroom_005941C4:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_005941C6:
        lea edi,[edx-3]
    InteriorHomeFurnishBedroom_005941C9:
        jmp InteriorHomeFurnishBedroom_00594229
    InteriorHomeFurnishBedroom_005941CB:
        test eax,eax
    InteriorHomeFurnishBedroom_005941CD:
        jne InteriorHomeFurnishBedroom_005941F7
    InteriorHomeFurnishBedroom_005941CF:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005941D2:
        movzx edx,byte ptr [edx+3]
    InteriorHomeFurnishBedroom_005941D6:
        mov edi,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_005941DA:
        lea ecx,[edx-2]
    InteriorHomeFurnishBedroom_005941DD:
        add edi,2
    InteriorHomeFurnishBedroom_005941E0:
        mov dword ptr [esp+24h],ecx
    InteriorHomeFurnishBedroom_005941E4:
        mov eax,4
    InteriorHomeFurnishBedroom_005941E9:
        mov dword ptr [esp+20h],edi
    InteriorHomeFurnishBedroom_005941ED:
        mov ecx,3
    InteriorHomeFurnishBedroom_005941F2:
        lea ebx,[edx-3]
    InteriorHomeFurnishBedroom_005941F5:
        jmp InteriorHomeFurnishBedroom_00594229
    InteriorHomeFurnishBedroom_005941F7:
        cmp eax,2
    InteriorHomeFurnishBedroom_005941FA:
        jne InteriorHomeFurnishBedroom_00594219
    InteriorHomeFurnishBedroom_005941FC:
        mov edi,dword ptr [esp+14h]
    InteriorHomeFurnishBedroom_00594200:
        mov eax,3
    InteriorHomeFurnishBedroom_00594205:
        add edi,2
    InteriorHomeFurnishBedroom_00594208:
        mov dword ptr [esp+24h],ebx
    InteriorHomeFurnishBedroom_0059420C:
        mov dword ptr [esp+20h],edi
    InteriorHomeFurnishBedroom_00594210:
        mov ecx,eax
    InteriorHomeFurnishBedroom_00594212:
        mov ebx,2
    InteriorHomeFurnishBedroom_00594217:
        jmp InteriorHomeFurnishBedroom_00594229
    InteriorHomeFurnishBedroom_00594219:
        mov eax,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_0059421D:
        mov ecx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594221:
        mov edi,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594225:
        mov ebx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594229:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_0059422D:
        push ebp
    InteriorHomeFurnishBedroom_0059422E:
        push ecx
    InteriorHomeFurnishBedroom_0059422F:
        sub esp,8
    InteriorHomeFurnishBedroom_00594232:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_00594236:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594238:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_0059423C:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_0059423F:
        push eax
    InteriorHomeFurnishBedroom_00594240:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishBedroom_00594245:
        push 0
    InteriorHomeFurnishBedroom_00594247:
        push 2
    InteriorHomeFurnishBedroom_00594249:
        push 1
    InteriorHomeFurnishBedroom_0059424B:
        push 1
    InteriorHomeFurnishBedroom_0059424D:
        push ebx
    InteriorHomeFurnishBedroom_0059424E:
        push edi
    InteriorHomeFurnishBedroom_0059424F:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594251:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_00594256:
        mov ebx,1
    InteriorHomeFurnishBedroom_0059425B:
        movsx edx,byte ptr [esi+00000791h]
    InteriorHomeFurnishBedroom_00594262:
        push 0
    InteriorHomeFurnishBedroom_00594264:
        push 0
    InteriorHomeFurnishBedroom_00594266:
        push 0
    InteriorHomeFurnishBedroom_00594268:
        push 0
    InteriorHomeFurnishBedroom_0059426A:
        push 0
    InteriorHomeFurnishBedroom_0059426C:
        push 0
    InteriorHomeFurnishBedroom_0059426E:
        push 0
    InteriorHomeFurnishBedroom_00594270:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594272:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594274:
        push ebx
    InteriorHomeFurnishBedroom_00594275:
        push 0
    InteriorHomeFurnishBedroom_00594277:
        push edx
    InteriorHomeFurnishBedroom_00594278:
        push 3
    InteriorHomeFurnishBedroom_0059427A:
        push 3
    InteriorHomeFurnishBedroom_0059427C:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059427E:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_00594283:
        movsx eax,byte ptr [esi+00000791h]
    InteriorHomeFurnishBedroom_0059428A:
        push 0
    InteriorHomeFurnishBedroom_0059428C:
        push 0
    InteriorHomeFurnishBedroom_0059428E:
        push 0
    InteriorHomeFurnishBedroom_00594290:
        push 0
    InteriorHomeFurnishBedroom_00594292:
        push 0
    InteriorHomeFurnishBedroom_00594294:
        push 0
    InteriorHomeFurnishBedroom_00594296:
        push 0
    InteriorHomeFurnishBedroom_00594298:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059429A:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059429C:
        push ebx
    InteriorHomeFurnishBedroom_0059429D:
        push 0
    InteriorHomeFurnishBedroom_0059429F:
        push eax
    InteriorHomeFurnishBedroom_005942A0:
        push 2
    InteriorHomeFurnishBedroom_005942A2:
        push 3
    InteriorHomeFurnishBedroom_005942A4:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005942A6:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_005942AB:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_005942B0:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_005942B5:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_005942B9:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005942BD:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_005942C3:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishBedroom_005942C9:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_005942CE:
        cmp eax,19h
    InteriorHomeFurnishBedroom_005942D1:
        jge InteriorHomeFurnishBedroom_0059431D
    InteriorHomeFurnishBedroom_005942D3:
        push 0
    InteriorHomeFurnishBedroom_005942D5:
        push 0
    InteriorHomeFurnishBedroom_005942D7:
        lea ecx,[esp+20h]
    InteriorHomeFurnishBedroom_005942DB:
        push ecx
    InteriorHomeFurnishBedroom_005942DC:
        lea edx,[esp+28h]
    InteriorHomeFurnishBedroom_005942E0:
        push edx
    InteriorHomeFurnishBedroom_005942E1:
        push 0
    InteriorHomeFurnishBedroom_005942E3:
        lea eax,[esp+24h]
    InteriorHomeFurnishBedroom_005942E7:
        push eax
    InteriorHomeFurnishBedroom_005942E8:
        push 0
    InteriorHomeFurnishBedroom_005942EA:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_005942EC:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_005942EE:
        push ebx
    InteriorHomeFurnishBedroom_005942EF:
        push 0
    InteriorHomeFurnishBedroom_005942F1:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_005942F3:
        push 6
    InteriorHomeFurnishBedroom_005942F5:
        push 2
    InteriorHomeFurnishBedroom_005942F7:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005942F9:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_005942FE:
        test eax,eax
    InteriorHomeFurnishBedroom_00594300:
        je InteriorHomeFurnishBedroom_005943BF
    InteriorHomeFurnishBedroom_00594306:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_0059430A:
        mov ecx,dword ptr [esp+18h]
    InteriorHomeFurnishBedroom_0059430E:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeFurnishBedroom_00594312:
        push eax
    InteriorHomeFurnishBedroom_00594313:
        push ecx
    InteriorHomeFurnishBedroom_00594314:
        push edx
    InteriorHomeFurnishBedroom_00594315:
        push eax
    InteriorHomeFurnishBedroom_00594316:
        push 8
    InteriorHomeFurnishBedroom_00594318:
        jmp InteriorHomeFurnishBedroom_005943B6
    InteriorHomeFurnishBedroom_0059431D:
        cmp eax,32h
    InteriorHomeFurnishBedroom_00594320:
        jl InteriorHomeFurnishBedroom_00594375
    InteriorHomeFurnishBedroom_00594322:
        cmp eax,4Bh
    InteriorHomeFurnishBedroom_00594325:
        jge InteriorHomeFurnishBedroom_005943BF
    InteriorHomeFurnishBedroom_0059432B:
        push 0
    InteriorHomeFurnishBedroom_0059432D:
        push 0
    InteriorHomeFurnishBedroom_0059432F:
        lea eax,[esp+20h]
    InteriorHomeFurnishBedroom_00594333:
        push eax
    InteriorHomeFurnishBedroom_00594334:
        lea ecx,[esp+28h]
    InteriorHomeFurnishBedroom_00594338:
        push ecx
    InteriorHomeFurnishBedroom_00594339:
        push 0
    InteriorHomeFurnishBedroom_0059433B:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_0059433F:
        push edx
    InteriorHomeFurnishBedroom_00594340:
        push 0
    InteriorHomeFurnishBedroom_00594342:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594344:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594346:
        push ebx
    InteriorHomeFurnishBedroom_00594347:
        push 0
    InteriorHomeFurnishBedroom_00594349:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059434B:
        push 6
    InteriorHomeFurnishBedroom_0059434D:
        push 2
    InteriorHomeFurnishBedroom_0059434F:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594351:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_00594356:
        test eax,eax
    InteriorHomeFurnishBedroom_00594358:
        je InteriorHomeFurnishBedroom_00594375
    InteriorHomeFurnishBedroom_0059435A:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_0059435E:
        mov ecx,dword ptr [esp+18h]
    InteriorHomeFurnishBedroom_00594362:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeFurnishBedroom_00594366:
        push eax
    InteriorHomeFurnishBedroom_00594367:
        push ecx
    InteriorHomeFurnishBedroom_00594368:
        push edx
    InteriorHomeFurnishBedroom_00594369:
        push eax
    InteriorHomeFurnishBedroom_0059436A:
        push 8
    InteriorHomeFurnishBedroom_0059436C:
        push 2
    InteriorHomeFurnishBedroom_0059436E:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594370:
        call InteriorHomeCall00593F10
    InteriorHomeFurnishBedroom_00594375:
        push 0
    InteriorHomeFurnishBedroom_00594377:
        push 0
    InteriorHomeFurnishBedroom_00594379:
        lea eax,[esp+20h]
    InteriorHomeFurnishBedroom_0059437D:
        push eax
    InteriorHomeFurnishBedroom_0059437E:
        lea ecx,[esp+28h]
    InteriorHomeFurnishBedroom_00594382:
        push ecx
    InteriorHomeFurnishBedroom_00594383:
        push 0
    InteriorHomeFurnishBedroom_00594385:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_00594389:
        push edx
    InteriorHomeFurnishBedroom_0059438A:
        push 0
    InteriorHomeFurnishBedroom_0059438C:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_0059438E:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594390:
        push ebx
    InteriorHomeFurnishBedroom_00594391:
        push 0
    InteriorHomeFurnishBedroom_00594393:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594395:
        push 6
    InteriorHomeFurnishBedroom_00594397:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594399:
        push 2
    InteriorHomeFurnishBedroom_0059439B:
        call InteriorHomeCall00593120
    InteriorHomeFurnishBedroom_005943A0:
        test eax,eax
    InteriorHomeFurnishBedroom_005943A2:
        je InteriorHomeFurnishBedroom_005943BF
    InteriorHomeFurnishBedroom_005943A4:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishBedroom_005943A8:
        mov ecx,dword ptr [esp+18h]
    InteriorHomeFurnishBedroom_005943AC:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeFurnishBedroom_005943B0:
        push eax
    InteriorHomeFurnishBedroom_005943B1:
        push ecx
    InteriorHomeFurnishBedroom_005943B2:
        push edx
    InteriorHomeFurnishBedroom_005943B3:
        push eax
    InteriorHomeFurnishBedroom_005943B4:
        push 3
    InteriorHomeFurnishBedroom_005943B6:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005943B8:
        push 2
    InteriorHomeFurnishBedroom_005943BA:
        call InteriorHomeCall00593F10
    InteriorHomeFurnishBedroom_005943BF:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005943C2:
        mov al,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_005943C5:
        cmp al,4Bh
    InteriorHomeFurnishBedroom_005943C7:
        jb InteriorHomeFurnishBedroom_005943EE
    InteriorHomeFurnishBedroom_005943C9:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_005943CE:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_005943D3:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_005943D7:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005943DB:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_005943E1:
        fmul dword ptr ds:[00858BA4h]
    InteriorHomeFurnishBedroom_005943E7:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_005943EC:
        jmp InteriorHomeFurnishBedroom_0059442A
    InteriorHomeFurnishBedroom_005943EE:
        cmp al,32h
    InteriorHomeFurnishBedroom_005943F0:
        jb InteriorHomeFurnishBedroom_0059441E
    InteriorHomeFurnishBedroom_005943F2:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_005943F7:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_005943FC:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_00594400:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594404:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_0059440A:
        fmul dword ptr ds:[00859CE4h]
    InteriorHomeFurnishBedroom_00594410:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_00594415:
        mov edi,14h
    InteriorHomeFurnishBedroom_0059441A:
        sub edi,eax
    InteriorHomeFurnishBedroom_0059441C:
        jmp InteriorHomeFurnishBedroom_0059442C
    InteriorHomeFurnishBedroom_0059441E:
        push 64h
    InteriorHomeFurnishBedroom_00594420:
        push 32h
    InteriorHomeFurnishBedroom_00594422:
        call InteriorHomeCall00407180
    InteriorHomeFurnishBedroom_00594427:
        add esp,8
    InteriorHomeFurnishBedroom_0059442A:
        mov edi,eax
    InteriorHomeFurnishBedroom_0059442C:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_00594431:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_00594436:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_0059443A:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_0059443E:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_00594444:
        fmul dword ptr ds:[00858B34h]
    InteriorHomeFurnishBedroom_0059444A:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_0059444F:
        cmp eax,edi
    InteriorHomeFurnishBedroom_00594451:
        jge InteriorHomeFurnishBedroom_005944CD
    InteriorHomeFurnishBedroom_00594453:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_00594457:
        push ecx
    InteriorHomeFurnishBedroom_00594458:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_0059445C:
        push edx
    InteriorHomeFurnishBedroom_0059445D:
        push 2
    InteriorHomeFurnishBedroom_0059445F:
        push 2
    InteriorHomeFurnishBedroom_00594461:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594463:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishBedroom_00594468:
        test al,al
    InteriorHomeFurnishBedroom_0059446A:
        je InteriorHomeFurnishBedroom_005944CD
    InteriorHomeFurnishBedroom_0059446C:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_0059446F:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_00594471:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_00594474:
        push ecx
    InteriorHomeFurnishBedroom_00594475:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594477:
        push 2
    InteriorHomeFurnishBedroom_00594479:
        push 8
    InteriorHomeFurnishBedroom_0059447B:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_00594480:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_00594485:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594489:
        push 0
    InteriorHomeFurnishBedroom_0059448B:
        push 3D4CCCCDh
    InteriorHomeFurnishBedroom_00594490:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594496:
        sub esp,8
    InteriorHomeFurnishBedroom_00594499:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059449B:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_0059449F:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_005944A3:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_005944A9:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_005944AC:
        push eax
    InteriorHomeFurnishBedroom_005944AD:
        push 0
    InteriorHomeFurnishBedroom_005944AF:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishBedroom_005944B4:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005944B8:
        mov eax,dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_005944BC:
        push 0
    InteriorHomeFurnishBedroom_005944BE:
        push 2
    InteriorHomeFurnishBedroom_005944C0:
        push 2
    InteriorHomeFurnishBedroom_005944C2:
        push 2
    InteriorHomeFurnishBedroom_005944C4:
        push edx
    InteriorHomeFurnishBedroom_005944C5:
        push eax
    InteriorHomeFurnishBedroom_005944C6:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005944C8:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_005944CD:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_005944D2:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_005944D7:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_005944DB:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005944DF:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_005944E5:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishBedroom_005944EB:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_005944F0:
        cmp eax,edi
    InteriorHomeFurnishBedroom_005944F2:
        jge InteriorHomeFurnishBedroom_0059456A
    InteriorHomeFurnishBedroom_005944F4:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_005944F8:
        push ecx
    InteriorHomeFurnishBedroom_005944F9:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_005944FD:
        push edx
    InteriorHomeFurnishBedroom_005944FE:
        push ebx
    InteriorHomeFurnishBedroom_005944FF:
        push ebx
    InteriorHomeFurnishBedroom_00594500:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594502:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishBedroom_00594507:
        test al,al
    InteriorHomeFurnishBedroom_00594509:
        je InteriorHomeFurnishBedroom_0059456A
    InteriorHomeFurnishBedroom_0059450B:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_0059450E:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_00594510:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_00594513:
        push ecx
    InteriorHomeFurnishBedroom_00594514:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594516:
        push 5
    InteriorHomeFurnishBedroom_00594518:
        push 8
    InteriorHomeFurnishBedroom_0059451A:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_0059451F:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_00594524:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594528:
        push 0
    InteriorHomeFurnishBedroom_0059452A:
        push 3D4CCCCDh
    InteriorHomeFurnishBedroom_0059452F:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594535:
        sub esp,8
    InteriorHomeFurnishBedroom_00594538:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059453A:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_0059453E:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_00594542:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594548:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_0059454B:
        push eax
    InteriorHomeFurnishBedroom_0059454C:
        push 0
    InteriorHomeFurnishBedroom_0059454E:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishBedroom_00594553:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594557:
        mov eax,dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_0059455B:
        push 0
    InteriorHomeFurnishBedroom_0059455D:
        push 2
    InteriorHomeFurnishBedroom_0059455F:
        push ebx
    InteriorHomeFurnishBedroom_00594560:
        push ebx
    InteriorHomeFurnishBedroom_00594561:
        push edx
    InteriorHomeFurnishBedroom_00594562:
        push eax
    InteriorHomeFurnishBedroom_00594563:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594565:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_0059456A:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_0059456F:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_00594574:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_00594578:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_0059457C:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_00594582:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishBedroom_00594588:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_0059458D:
        cmp eax,edi
    InteriorHomeFurnishBedroom_0059458F:
        jge InteriorHomeFurnishBedroom_00594607
    InteriorHomeFurnishBedroom_00594591:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_00594595:
        push ecx
    InteriorHomeFurnishBedroom_00594596:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_0059459A:
        push edx
    InteriorHomeFurnishBedroom_0059459B:
        push ebx
    InteriorHomeFurnishBedroom_0059459C:
        push ebx
    InteriorHomeFurnishBedroom_0059459D:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059459F:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishBedroom_005945A4:
        test al,al
    InteriorHomeFurnishBedroom_005945A6:
        je InteriorHomeFurnishBedroom_00594607
    InteriorHomeFurnishBedroom_005945A8:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005945AB:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_005945AD:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_005945B0:
        push ecx
    InteriorHomeFurnishBedroom_005945B1:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_005945B3:
        push 4
    InteriorHomeFurnishBedroom_005945B5:
        push 8
    InteriorHomeFurnishBedroom_005945B7:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_005945BC:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_005945C1:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005945C5:
        push 0
    InteriorHomeFurnishBedroom_005945C7:
        push 3D4CCCCDh
    InteriorHomeFurnishBedroom_005945CC:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_005945D2:
        sub esp,8
    InteriorHomeFurnishBedroom_005945D5:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005945D7:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_005945DB:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_005945DF:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_005945E5:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_005945E8:
        push eax
    InteriorHomeFurnishBedroom_005945E9:
        push 0
    InteriorHomeFurnishBedroom_005945EB:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishBedroom_005945F0:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005945F4:
        mov eax,dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_005945F8:
        push 0
    InteriorHomeFurnishBedroom_005945FA:
        push 2
    InteriorHomeFurnishBedroom_005945FC:
        push ebx
    InteriorHomeFurnishBedroom_005945FD:
        push ebx
    InteriorHomeFurnishBedroom_005945FE:
        push edx
    InteriorHomeFurnishBedroom_005945FF:
        push eax
    InteriorHomeFurnishBedroom_00594600:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594602:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_00594607:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_0059460C:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_00594611:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_00594615:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594619:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_0059461F:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishBedroom_00594625:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_0059462A:
        cmp eax,edi
    InteriorHomeFurnishBedroom_0059462C:
        jge InteriorHomeFurnishBedroom_005946A4
    InteriorHomeFurnishBedroom_0059462E:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_00594632:
        push ecx
    InteriorHomeFurnishBedroom_00594633:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_00594637:
        push edx
    InteriorHomeFurnishBedroom_00594638:
        push ebx
    InteriorHomeFurnishBedroom_00594639:
        push ebx
    InteriorHomeFurnishBedroom_0059463A:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059463C:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishBedroom_00594641:
        test al,al
    InteriorHomeFurnishBedroom_00594643:
        je InteriorHomeFurnishBedroom_005946A4
    InteriorHomeFurnishBedroom_00594645:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_00594648:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_0059464A:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_0059464D:
        push ecx
    InteriorHomeFurnishBedroom_0059464E:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594650:
        push 3
    InteriorHomeFurnishBedroom_00594652:
        push 8
    InteriorHomeFurnishBedroom_00594654:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_00594659:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_0059465E:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594662:
        push 0
    InteriorHomeFurnishBedroom_00594664:
        push 3D4CCCCDh
    InteriorHomeFurnishBedroom_00594669:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_0059466F:
        sub esp,8
    InteriorHomeFurnishBedroom_00594672:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594674:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_00594678:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_0059467C:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594682:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_00594685:
        push eax
    InteriorHomeFurnishBedroom_00594686:
        push 0
    InteriorHomeFurnishBedroom_00594688:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishBedroom_0059468D:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594691:
        mov eax,dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_00594695:
        push 0
    InteriorHomeFurnishBedroom_00594697:
        push 2
    InteriorHomeFurnishBedroom_00594699:
        push ebx
    InteriorHomeFurnishBedroom_0059469A:
        push ebx
    InteriorHomeFurnishBedroom_0059469B:
        push edx
    InteriorHomeFurnishBedroom_0059469C:
        push eax
    InteriorHomeFurnishBedroom_0059469D:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059469F:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_005946A4:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishBedroom_005946A9:
        and eax,0FFFFh
    InteriorHomeFurnishBedroom_005946AE:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishBedroom_005946B2:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_005946B6:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishBedroom_005946BC:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishBedroom_005946C2:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_005946C7:
        cmp eax,edi
    InteriorHomeFurnishBedroom_005946C9:
        jge InteriorHomeFurnishBedroom_00594743
    InteriorHomeFurnishBedroom_005946CB:
        lea ecx,[esp+24h]
    InteriorHomeFurnishBedroom_005946CF:
        push ecx
    InteriorHomeFurnishBedroom_005946D0:
        lea edx,[esp+24h]
    InteriorHomeFurnishBedroom_005946D4:
        push edx
    InteriorHomeFurnishBedroom_005946D5:
        push 2
    InteriorHomeFurnishBedroom_005946D7:
        push 2
    InteriorHomeFurnishBedroom_005946D9:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005946DB:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishBedroom_005946E0:
        test al,al
    InteriorHomeFurnishBedroom_005946E2:
        je InteriorHomeFurnishBedroom_00594743
    InteriorHomeFurnishBedroom_005946E4:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_005946E7:
        xor ecx,ecx
    InteriorHomeFurnishBedroom_005946E9:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishBedroom_005946EC:
        push ecx
    InteriorHomeFurnishBedroom_005946ED:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_005946EF:
        push 6
    InteriorHomeFurnishBedroom_005946F1:
        push 8
    InteriorHomeFurnishBedroom_005946F3:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_005946F8:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_005946FD:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594701:
        push 0
    InteriorHomeFurnishBedroom_00594703:
        push 3D4CCCCDh
    InteriorHomeFurnishBedroom_00594708:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_0059470E:
        sub esp,8
    InteriorHomeFurnishBedroom_00594711:
        mov ecx,esi
    InteriorHomeFurnishBedroom_00594713:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishBedroom_00594717:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishBedroom_0059471B:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594721:
        fstp dword ptr [esp]
    InteriorHomeFurnishBedroom_00594724:
        push eax
    InteriorHomeFurnishBedroom_00594725:
        push 0
    InteriorHomeFurnishBedroom_00594727:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishBedroom_0059472C:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishBedroom_00594730:
        mov eax,dword ptr [esp+20h]
    InteriorHomeFurnishBedroom_00594734:
        push 0
    InteriorHomeFurnishBedroom_00594736:
        push 2
    InteriorHomeFurnishBedroom_00594738:
        push ebx
    InteriorHomeFurnishBedroom_00594739:
        push ebx
    InteriorHomeFurnishBedroom_0059473A:
        push edx
    InteriorHomeFurnishBedroom_0059473B:
        push eax
    InteriorHomeFurnishBedroom_0059473C:
        mov ecx,esi
    InteriorHomeFurnishBedroom_0059473E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishBedroom_00594743:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_00594746:
        xor edx,edx
    InteriorHomeFurnishBedroom_00594748:
        mov dl,byte ptr [ecx+1Fh]
    InteriorHomeFurnishBedroom_0059474B:
        mov ecx,0BAB380h
    InteriorHomeFurnishBedroom_00594750:
        push edx
    InteriorHomeFurnishBedroom_00594751:
        push 0FFFFFFFFh
    InteriorHomeFurnishBedroom_00594753:
        push ebx
    InteriorHomeFurnishBedroom_00594754:
        push 8
    InteriorHomeFurnishBedroom_00594756:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishBedroom_0059475B:
        mov ebx,dword ptr [esi+14h]
    InteriorHomeFurnishBedroom_0059475E:
        movzx edx,byte ptr [ebx+3]
    InteriorHomeFurnishBedroom_00594762:
        push 0
    InteriorHomeFurnishBedroom_00594764:
        mov edi,eax
    InteriorHomeFurnishBedroom_00594766:
        lea eax,[esp+2Ch]
    InteriorHomeFurnishBedroom_0059476A:
        push eax
    InteriorHomeFurnishBedroom_0059476B:
        movzx eax,byte ptr [edi+0Dh]
    InteriorHomeFurnishBedroom_0059476F:
        mov dword ptr [esp+2Ch],edx
    InteriorHomeFurnishBedroom_00594773:
        lea ecx,[esp+34h]
    InteriorHomeFurnishBedroom_00594777:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishBedroom_0059477B:
        mov dword ptr [esp+2Ch],eax
    InteriorHomeFurnishBedroom_0059477F:
        push ecx
    InteriorHomeFurnishBedroom_00594780:
        push 0
    InteriorHomeFurnishBedroom_00594782:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594788:
        push 0
    InteriorHomeFurnishBedroom_0059478A:
        fild dword ptr [esp+38h]
    InteriorHomeFurnishBedroom_0059478E:
        push 0
    InteriorHomeFurnishBedroom_00594790:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_00594796:
        fsubp st(1),st(0)
    InteriorHomeFurnishBedroom_00594798:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_0059479D:
        movzx ecx,byte ptr [ebx+2]
    InteriorHomeFurnishBedroom_005947A1:
        movzx edx,byte ptr [edi+0Ch]
    InteriorHomeFurnishBedroom_005947A5:
        mov dword ptr [esp+3Ch],ecx
    InteriorHomeFurnishBedroom_005947A9:
        push eax
    InteriorHomeFurnishBedroom_005947AA:
        fild dword ptr [esp+40h]
    InteriorHomeFurnishBedroom_005947AE:
        mov dword ptr [esp+40h],edx
    InteriorHomeFurnishBedroom_005947B2:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_005947B8:
        fild dword ptr [esp+40h]
    InteriorHomeFurnishBedroom_005947BC:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishBedroom_005947C2:
        fsubp st(1),st(0)
    InteriorHomeFurnishBedroom_005947C4:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishBedroom_005947C9:
        push eax
    InteriorHomeFurnishBedroom_005947CA:
        push edi
    InteriorHomeFurnishBedroom_005947CB:
        mov ecx,esi
    InteriorHomeFurnishBedroom_005947CD:
        call InteriorHomeCall00592AA0
    InteriorHomeFurnishBedroom_005947D2:
        pop edi
    InteriorHomeFurnishBedroom_005947D3:
        pop esi
    InteriorHomeFurnishBedroom_005947D4:
        pop ebp
    InteriorHomeFurnishBedroom_005947D5:
        pop ebx
    InteriorHomeFurnishBedroom_005947D6:
        add esp,20h
    InteriorHomeFurnishBedroom_005947D9:
        ret
    }
}

void Interior_c::FurnishBedroom() {
    reinterpret_cast<void(__thiscall*)(Interior_c*)>(&InteriorHomeNativeFurnishBedroom)(this);
}

// 0x596930
static __declspec(naked) void InteriorHomeNativeKitchen_FurnishEdges() {
    __asm {
    InteriorHomeKitchen_FurnishEdges_00596930:
        push ebp
    InteriorHomeKitchen_FurnishEdges_00596931:
        mov ebp,esp
    InteriorHomeKitchen_FurnishEdges_00596933:
        and esp,0FFFFFFF8h
    InteriorHomeKitchen_FurnishEdges_00596936:
        sub esp,228h
    InteriorHomeKitchen_FurnishEdges_0059693C:
        push ebx
    InteriorHomeKitchen_FurnishEdges_0059693D:
        push ebp
    InteriorHomeKitchen_FurnishEdges_0059693E:
        push esi
    InteriorHomeKitchen_FurnishEdges_0059693F:
        mov esi,ecx
    InteriorHomeKitchen_FurnishEdges_00596941:
        mov eax,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596944:
        movsx cx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_0059694C:
        movzx ebx,byte ptr [eax+3]
    InteriorHomeKitchen_FurnishEdges_00596950:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596951:
        movzx edi,byte ptr [eax+2]
    InteriorHomeKitchen_FurnishEdges_00596955:
        movzx eax,byte ptr [eax+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596959:
        push eax
    InteriorHomeKitchen_FurnishEdges_0059695A:
        push ecx
    InteriorHomeKitchen_FurnishEdges_0059695B:
        dec edi
    InteriorHomeKitchen_FurnishEdges_0059695C:
        push 7
    InteriorHomeKitchen_FurnishEdges_0059695E:
        dec ebx
    InteriorHomeKitchen_FurnishEdges_0059695F:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596961:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596966:
        mov dword ptr [esp+30h],edi
    InteriorHomeKitchen_FurnishEdges_0059696A:
        mov dword ptr [esp+20h],ebx
    InteriorHomeKitchen_FurnishEdges_0059696E:
        mov byte ptr [esp+2Ah],0
    InteriorHomeKitchen_FurnishEdges_00596973:
        mov byte ptr [esp+2Bh],0
    InteriorHomeKitchen_FurnishEdges_00596978:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_0059697D:
        push 0
    InteriorHomeKitchen_FurnishEdges_0059697F:
        mov ebp,eax
    InteriorHomeKitchen_FurnishEdges_00596981:
        lea edx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596985:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596986:
        lea eax,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_0059698A:
        push eax
    InteriorHomeKitchen_FurnishEdges_0059698B:
        push 1
    InteriorHomeKitchen_FurnishEdges_0059698D:
        push 1
    InteriorHomeKitchen_FurnishEdges_0059698F:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596991:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596992:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596994:
        push ebp
    InteriorHomeKitchen_FurnishEdges_00596995:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596997:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_0059699C:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_0059699E:
        je InteriorHomeKitchen_FurnishEdges_005969A5
    InteriorHomeKitchen_FurnishEdges_005969A0:
        mov byte ptr [esp+1Ah],1
    InteriorHomeKitchen_FurnishEdges_005969A5:
        push 0
    InteriorHomeKitchen_FurnishEdges_005969A7:
        lea ecx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_005969AB:
        push ecx
    InteriorHomeKitchen_FurnishEdges_005969AC:
        lea edx,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_005969B0:
        push edx
    InteriorHomeKitchen_FurnishEdges_005969B1:
        push 0
    InteriorHomeKitchen_FurnishEdges_005969B3:
        push 1
    InteriorHomeKitchen_FurnishEdges_005969B5:
        push 0
    InteriorHomeKitchen_FurnishEdges_005969B7:
        push ebx
    InteriorHomeKitchen_FurnishEdges_005969B8:
        push edi
    InteriorHomeKitchen_FurnishEdges_005969B9:
        push ebp
    InteriorHomeKitchen_FurnishEdges_005969BA:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_005969BC:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_005969C1:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_005969C3:
        je InteriorHomeKitchen_FurnishEdges_005969CA
    InteriorHomeKitchen_FurnishEdges_005969C5:
        mov byte ptr [esp+1Bh],1
    InteriorHomeKitchen_FurnishEdges_005969CA:
        mov ebp,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_005969CD:
        mov al,byte ptr [ebp+7]
    InteriorHomeKitchen_FurnishEdges_005969D0:
        mov cl,byte ptr [ebp+0Dh]
    InteriorHomeKitchen_FurnishEdges_005969D3:
        cmp al,cl
    InteriorHomeKitchen_FurnishEdges_005969D5:
        jg InteriorHomeKitchen_FurnishEdges_005969D9
    InteriorHomeKitchen_FurnishEdges_005969D7:
        mov al,cl
    InteriorHomeKitchen_FurnishEdges_005969D9:
        mov cl,byte ptr [ebp+0Fh]
    InteriorHomeKitchen_FurnishEdges_005969DC:
        movsx eax,al
    InteriorHomeKitchen_FurnishEdges_005969DF:
        xor edx,edx
    InteriorHomeKitchen_FurnishEdges_005969E1:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_005969E3:
        setle dl
    InteriorHomeKitchen_FurnishEdges_005969E6:
        dec edx
    InteriorHomeKitchen_FurnishEdges_005969E7:
        and edx,eax
    InteriorHomeKitchen_FurnishEdges_005969E9:
        mov al,byte ptr [ebp+9]
    InteriorHomeKitchen_FurnishEdges_005969EC:
        cmp al,cl
    InteriorHomeKitchen_FurnishEdges_005969EE:
        jg InteriorHomeKitchen_FurnishEdges_005969F2
    InteriorHomeKitchen_FurnishEdges_005969F0:
        mov al,cl
    InteriorHomeKitchen_FurnishEdges_005969F2:
        movsx eax,al
    InteriorHomeKitchen_FurnishEdges_005969F5:
        xor ecx,ecx
    InteriorHomeKitchen_FurnishEdges_005969F7:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_005969F9:
        setle cl
    InteriorHomeKitchen_FurnishEdges_005969FC:
        dec ecx
    InteriorHomeKitchen_FurnishEdges_005969FD:
        and ecx,eax
    InteriorHomeKitchen_FurnishEdges_005969FF:
        mov eax,ecx
    InteriorHomeKitchen_FurnishEdges_00596A01:
        jle InteriorHomeKitchen_FurnishEdges_00596A13
    InteriorHomeKitchen_FurnishEdges_00596A03:
        test edx,edx
    InteriorHomeKitchen_FurnishEdges_00596A05:
        mov dword ptr [esp+30h],eax
    InteriorHomeKitchen_FurnishEdges_00596A09:
        jle InteriorHomeKitchen_FurnishEdges_00596A0F
    InteriorHomeKitchen_FurnishEdges_00596A0B:
        mov ebp,edx
    InteriorHomeKitchen_FurnishEdges_00596A0D:
        jmp InteriorHomeKitchen_FurnishEdges_00596A3C
    InteriorHomeKitchen_FurnishEdges_00596A0F:
        xor ebp,ebp
    InteriorHomeKitchen_FurnishEdges_00596A11:
        jmp InteriorHomeKitchen_FurnishEdges_00596A52
    InteriorHomeKitchen_FurnishEdges_00596A13:
        test edx,edx
    InteriorHomeKitchen_FurnishEdges_00596A15:
        jle InteriorHomeKitchen_FurnishEdges_00596A23
    InteriorHomeKitchen_FurnishEdges_00596A17:
        mov ebp,edx
    InteriorHomeKitchen_FurnishEdges_00596A19:
        mov dword ptr [esp+30h],0
    InteriorHomeKitchen_FurnishEdges_00596A21:
        jmp InteriorHomeKitchen_FurnishEdges_00596A3C
    InteriorHomeKitchen_FurnishEdges_00596A23:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596A24:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A26:
        call InteriorHomeCall00407180
    InteriorHomeKitchen_FurnishEdges_00596A2B:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596A2C:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A2E:
        mov ebp,eax
    InteriorHomeKitchen_FurnishEdges_00596A30:
        call InteriorHomeCall00407180
    InteriorHomeKitchen_FurnishEdges_00596A35:
        add esp,10h
    InteriorHomeKitchen_FurnishEdges_00596A38:
        mov dword ptr [esp+30h],eax
    InteriorHomeKitchen_FurnishEdges_00596A3C:
        test ebp,ebp
    InteriorHomeKitchen_FurnishEdges_00596A3E:
        jle InteriorHomeKitchen_FurnishEdges_00596A52
    InteriorHomeKitchen_FurnishEdges_00596A40:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A42:
        push 2
    InteriorHomeKitchen_FurnishEdges_00596A44:
        push ebp
    InteriorHomeKitchen_FurnishEdges_00596A45:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596A47:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A49:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A4B:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596A4D:
        call InteriorHomeCall00591700
    InteriorHomeKitchen_FurnishEdges_00596A52:
        mov ebx,dword ptr [esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596A56:
        test ebx,ebx
    InteriorHomeKitchen_FurnishEdges_00596A58:
        jle InteriorHomeKitchen_FurnishEdges_00596A6B
    InteriorHomeKitchen_FurnishEdges_00596A5A:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A5C:
        push 2
    InteriorHomeKitchen_FurnishEdges_00596A5E:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596A5F:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596A61:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A63:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596A64:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596A66:
        call InteriorHomeCall00591700
    InteriorHomeKitchen_FurnishEdges_00596A6B:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596A6E:
        movsx cx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596A76:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00596A78:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596A7B:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596A7C:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596A7D:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A7F:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596A81:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596A86:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_00596A8B:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A8D:
        lea edx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596A91:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596A92:
        lea ecx,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_00596A96:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596A97:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596A99:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596A9B:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596A9D:
        push ebp
    InteriorHomeKitchen_FurnishEdges_00596A9E:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596AA0:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596AA1:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596AA3:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_00596AA8:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596AAB:
        movsx cx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596AB3:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00596AB5:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596AB8:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596AB9:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596ABA:
        push 2
    InteriorHomeKitchen_FurnishEdges_00596ABC:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596ABE:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596AC3:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_00596AC8:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596ACA:
        lea edx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596ACE:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596ACF:
        lea ecx,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_00596AD3:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596AD4:
        push 3
    InteriorHomeKitchen_FurnishEdges_00596AD6:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596AD8:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596ADA:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596ADB:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596ADC:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596ADD:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596ADF:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_00596AE4:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596AE7:
        mov al,byte ptr [edx+10h]
    InteriorHomeKitchen_FurnishEdges_00596AEA:
        or ecx,0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596AED:
        cmp al,0FFh
    InteriorHomeKitchen_FurnishEdges_00596AEF:
        je InteriorHomeKitchen_FurnishEdges_00596AF4
    InteriorHomeKitchen_FurnishEdges_00596AF1:
        movsx ecx,al
    InteriorHomeKitchen_FurnishEdges_00596AF4:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596AF6:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596AF8:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596AFA:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596AFC:
        lea eax,[esp+38h]
    InteriorHomeKitchen_FurnishEdges_00596B00:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596B01:
        movsx eax,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596B08:
        lea edx,[esp+48h]
    InteriorHomeKitchen_FurnishEdges_00596B0C:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596B0D:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B0F:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596B10:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B12:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596B14:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B16:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596B17:
        push 3
    InteriorHomeKitchen_FurnishEdges_00596B19:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596B1B:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596B1D:
        call InteriorHomeCall00593120
    InteriorHomeKitchen_FurnishEdges_00596B22:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596B24:
        je InteriorHomeKitchen_FurnishEdges_00596B5F
    InteriorHomeKitchen_FurnishEdges_00596B26:
        mov eax,dword ptr [esp+28h]
    InteriorHomeKitchen_FurnishEdges_00596B2A:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596B2D:
        inc eax
    InteriorHomeKitchen_FurnishEdges_00596B2E:
        mov dword ptr [esp+28h],eax
    InteriorHomeKitchen_FurnishEdges_00596B32:
        movzx edx,byte ptr [ecx+3]
    InteriorHomeKitchen_FurnishEdges_00596B36:
        mov dword ptr [esp+14h],edx
    InteriorHomeKitchen_FurnishEdges_00596B3A:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B3C:
        push 2
    InteriorHomeKitchen_FurnishEdges_00596B3E:
        fild dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596B42:
        sub esp,8
    InteriorHomeKitchen_FurnishEdges_00596B45:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596B47:
        fsub dword ptr ds:[00858CE8h]
    InteriorHomeKitchen_FurnishEdges_00596B4D:
        fstp dword ptr [esp+4]
    InteriorHomeKitchen_FurnishEdges_00596B51:
        fild dword ptr [esp+38h]
    InteriorHomeKitchen_FurnishEdges_00596B55:
        fstp dword ptr [esp]
    InteriorHomeKitchen_FurnishEdges_00596B58:
        push 5
    InteriorHomeKitchen_FurnishEdges_00596B5A:
        call InteriorHomeCall00591E40
    InteriorHomeKitchen_FurnishEdges_00596B5F:
        movsx edx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596B66:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B68:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B6A:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B6C:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B6E:
        lea eax,[esp+38h]
    InteriorHomeKitchen_FurnishEdges_00596B72:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596B73:
        lea ecx,[esp+48h]
    InteriorHomeKitchen_FurnishEdges_00596B77:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596B78:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B7A:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596B7C:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596B7E:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596B80:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B82:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596B83:
        push 5
    InteriorHomeKitchen_FurnishEdges_00596B85:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596B87:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596B89:
        call InteriorHomeCall00593120
    InteriorHomeKitchen_FurnishEdges_00596B8E:
        movsx edx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596B95:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B97:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B99:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B9B:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596B9D:
        lea eax,[esp+38h]
    InteriorHomeKitchen_FurnishEdges_00596BA1:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596BA2:
        lea ecx,[esp+48h]
    InteriorHomeKitchen_FurnishEdges_00596BA6:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596BA7:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BA9:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596BAB:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596BAD:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596BAF:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BB1:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596BB2:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596BB4:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596BB6:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596BB8:
        call InteriorHomeCall00593120
    InteriorHomeKitchen_FurnishEdges_00596BBD:
        movsx edx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596BC4:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BC6:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BC8:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BCA:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BCC:
        lea eax,[esp+38h]
    InteriorHomeKitchen_FurnishEdges_00596BD0:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596BD1:
        lea ecx,[esp+48h]
    InteriorHomeKitchen_FurnishEdges_00596BD5:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596BD6:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BD8:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596BDA:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596BDC:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596BDE:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596BE0:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596BE1:
        push 6
    InteriorHomeKitchen_FurnishEdges_00596BE3:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596BE5:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596BE7:
        call InteriorHomeCall00593120
    InteriorHomeKitchen_FurnishEdges_00596BEC:
        mov eax,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596BEF:
        movsx dx,byte ptr [esi+00000791h]
    InteriorHomeKitchen_FurnishEdges_00596BF7:
        xor ecx,ecx
    InteriorHomeKitchen_FurnishEdges_00596BF9:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596BFC:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596BFD:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596BFE:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596C00:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596C02:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596C07:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_00596C0C:
        mov ebx,eax
    InteriorHomeKitchen_FurnishEdges_00596C0E:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00596C10:
        mov ecx,20h
    InteriorHomeKitchen_FurnishEdges_00596C15:
        lea edi,[esp+000001B8h]
    InteriorHomeKitchen_FurnishEdges_00596C1C:
        rep stosd
    InteriorHomeKitchen_FurnishEdges_00596C1E:
        lea edi,[ebp+1]
    InteriorHomeKitchen_FurnishEdges_00596C21:
        mov ebp,dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596C25:
        cmp edi,ebp
    InteriorHomeKitchen_FurnishEdges_00596C27:
        mov dword ptr [esp+1Ch],0
    InteriorHomeKitchen_FurnishEdges_00596C2F:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596C33:
        jge InteriorHomeKitchen_FurnishEdges_00596C8C
    InteriorHomeKitchen_FurnishEdges_00596C35:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596C37:
        lea eax,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596C3B:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596C3C:
        lea ecx,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_00596C40:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596C41:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596C43:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596C45:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596C47:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596C48:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596C4A:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596C4B:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596C4D:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_00596C52:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596C54:
        je InteriorHomeKitchen_FurnishEdges_00596C83
    InteriorHomeKitchen_FurnishEdges_00596C56:
        fild dword ptr [esp+14h]
    InteriorHomeKitchen_FurnishEdges_00596C5A:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596C5E:
        mov dword ptr [esp+eax*4+000000B8h],3F000000h
    InteriorHomeKitchen_FurnishEdges_00596C69:
        mov dword ptr [esp+eax*4+00000138h],42B40000h
    InteriorHomeKitchen_FurnishEdges_00596C74:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596C7A:
        inc eax
    InteriorHomeKitchen_FurnishEdges_00596C7B:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeKitchen_FurnishEdges_00596C7F:
        fstp dword ptr [esp+eax*4+34h]
    InteriorHomeKitchen_FurnishEdges_00596C83:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00596C84:
        cmp edi,ebp
    InteriorHomeKitchen_FurnishEdges_00596C86:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596C8A:
        jl InteriorHomeKitchen_FurnishEdges_00596C35
    InteriorHomeKitchen_FurnishEdges_00596C8C:
        mov eax,dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596C90:
        mov edi,1
    InteriorHomeKitchen_FurnishEdges_00596C95:
        cmp eax,edi
    InteriorHomeKitchen_FurnishEdges_00596C97:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596C9B:
        jle InteriorHomeKitchen_FurnishEdges_00596D00
    InteriorHomeKitchen_FurnishEdges_00596C9D:
        lea ecx,[ecx]
    InteriorHomeKitchen_FurnishEdges_00596CA0:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596CA2:
        lea edx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596CA6:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596CA7:
        lea eax,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_00596CAB:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596CAC:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596CAE:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596CB0:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596CB2:
        push ebp
    InteriorHomeKitchen_FurnishEdges_00596CB3:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596CB4:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596CB5:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596CB7:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_00596CBC:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596CBE:
        je InteriorHomeKitchen_FurnishEdges_00596CF3
    InteriorHomeKitchen_FurnishEdges_00596CC0:
        fild dword ptr [esp+14h]
    InteriorHomeKitchen_FurnishEdges_00596CC4:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596CC8:
        mov dword ptr [esp+eax*4+00000138h],0
    InteriorHomeKitchen_FurnishEdges_00596CD3:
        inc eax
    InteriorHomeKitchen_FurnishEdges_00596CD4:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596CDA:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeKitchen_FurnishEdges_00596CDE:
        fstp dword ptr [esp+eax*4+000000B4h]
    InteriorHomeKitchen_FurnishEdges_00596CE5:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596CE9:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596CEF:
        fstp dword ptr [esp+eax*4+34h]
    InteriorHomeKitchen_FurnishEdges_00596CF3:
        mov eax,dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596CF7:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00596CF8:
        cmp edi,eax
    InteriorHomeKitchen_FurnishEdges_00596CFA:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596CFE:
        jl InteriorHomeKitchen_FurnishEdges_00596CA0
    InteriorHomeKitchen_FurnishEdges_00596D00:
        mov edi,dword ptr [esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596D04:
        dec ebp
    InteriorHomeKitchen_FurnishEdges_00596D05:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00596D06:
        cmp edi,ebp
    InteriorHomeKitchen_FurnishEdges_00596D08:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596D0C:
        jg InteriorHomeKitchen_FurnishEdges_00596D70
    InteriorHomeKitchen_FurnishEdges_00596D0E:
        mov edi,edi
    InteriorHomeKitchen_FurnishEdges_00596D10:
        mov eax,dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596D14:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596D16:
        lea ecx,[esp+30h]
    InteriorHomeKitchen_FurnishEdges_00596D1A:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596D1B:
        lea edx,[esp+2Ch]
    InteriorHomeKitchen_FurnishEdges_00596D1F:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596D20:
        push 3
    InteriorHomeKitchen_FurnishEdges_00596D22:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596D24:
        push 0
    InteriorHomeKitchen_FurnishEdges_00596D26:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596D27:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596D28:
        push ebx
    InteriorHomeKitchen_FurnishEdges_00596D29:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596D2B:
        call InteriorHomeCall00592AA0
    InteriorHomeKitchen_FurnishEdges_00596D30:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596D32:
        je InteriorHomeKitchen_FurnishEdges_00596D67
    InteriorHomeKitchen_FurnishEdges_00596D34:
        fild dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596D38:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596D3C:
        mov dword ptr [esp+eax*4+00000138h],43870000h
    InteriorHomeKitchen_FurnishEdges_00596D47:
        inc eax
    InteriorHomeKitchen_FurnishEdges_00596D48:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596D4E:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeKitchen_FurnishEdges_00596D52:
        fstp dword ptr [esp+eax*4+000000B4h]
    InteriorHomeKitchen_FurnishEdges_00596D59:
        fild dword ptr [esp+14h]
    InteriorHomeKitchen_FurnishEdges_00596D5D:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596D63:
        fstp dword ptr [esp+eax*4+34h]
    InteriorHomeKitchen_FurnishEdges_00596D67:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00596D68:
        cmp edi,ebp
    InteriorHomeKitchen_FurnishEdges_00596D6A:
        mov dword ptr [esp+14h],edi
    InteriorHomeKitchen_FurnishEdges_00596D6E:
        jle InteriorHomeKitchen_FurnishEdges_00596D10
    InteriorHomeKitchen_FurnishEdges_00596D70:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596D73:
        xor edx,edx
    InteriorHomeKitchen_FurnishEdges_00596D75:
        mov dl,byte ptr [ecx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596D78:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596D7D:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596D7E:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596D80:
        push 0Ah
    InteriorHomeKitchen_FurnishEdges_00596D82:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596D84:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_00596D89:
        mov edi,eax
    InteriorHomeKitchen_FurnishEdges_00596D8B:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596D90:
        cmp eax,3FFFh
    InteriorHomeKitchen_FurnishEdges_00596D95:
        jge InteriorHomeKitchen_FurnishEdges_00596DC9
    InteriorHomeKitchen_FurnishEdges_00596D97:
        mov al,byte ptr [esp+1Ah]
    InteriorHomeKitchen_FurnishEdges_00596D9B:
        test al,al
    InteriorHomeKitchen_FurnishEdges_00596D9D:
        je InteriorHomeKitchen_FurnishEdges_00596DC9
    InteriorHomeKitchen_FurnishEdges_00596D9F:
        mov eax,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596DA2:
        movzx ecx,byte ptr [eax+3]
    InteriorHomeKitchen_FurnishEdges_00596DA6:
        mov dword ptr [esp+10h],ecx
    InteriorHomeKitchen_FurnishEdges_00596DAA:
        push 42340000h
    InteriorHomeKitchen_FurnishEdges_00596DAF:
        push 3F866666h
    InteriorHomeKitchen_FurnishEdges_00596DB4:
        fild dword ptr [esp+18h]
    InteriorHomeKitchen_FurnishEdges_00596DB8:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596DB9:
        fsub dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596DBF:
        fstp dword ptr [esp]
    InteriorHomeKitchen_FurnishEdges_00596DC2:
        push 3F000000h
    InteriorHomeKitchen_FurnishEdges_00596DC7:
        jmp InteriorHomeKitchen_FurnishEdges_00596E0C
    InteriorHomeKitchen_FurnishEdges_00596DC9:
        mov al,byte ptr [esp+1Bh]
    InteriorHomeKitchen_FurnishEdges_00596DCD:
        test al,al
    InteriorHomeKitchen_FurnishEdges_00596DCF:
        je InteriorHomeKitchen_FurnishEdges_00596E16
    InteriorHomeKitchen_FurnishEdges_00596DD1:
        mov eax,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596DD4:
        movzx edx,byte ptr [eax+3]
    InteriorHomeKitchen_FurnishEdges_00596DD8:
        movzx eax,byte ptr [eax+2]
    InteriorHomeKitchen_FurnishEdges_00596DDC:
        mov dword ptr [esp+10h],edx
    InteriorHomeKitchen_FurnishEdges_00596DE0:
        push 439D8000h
    InteriorHomeKitchen_FurnishEdges_00596DE5:
        push 3F866666h
    InteriorHomeKitchen_FurnishEdges_00596DEA:
        fild dword ptr [esp+18h]
    InteriorHomeKitchen_FurnishEdges_00596DEE:
        sub esp,8
    InteriorHomeKitchen_FurnishEdges_00596DF1:
        mov dword ptr [esp+20h],eax
    InteriorHomeKitchen_FurnishEdges_00596DF5:
        fsub dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596DFB:
        fstp dword ptr [esp+4]
    InteriorHomeKitchen_FurnishEdges_00596DFF:
        fild dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596E03:
        fsub dword ptr ds:[00858B8Ch]
    InteriorHomeKitchen_FurnishEdges_00596E09:
        fstp dword ptr [esp]
    InteriorHomeKitchen_FurnishEdges_00596E0C:
        push edi
    InteriorHomeKitchen_FurnishEdges_00596E0D:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00596E0F:
        push 1
    InteriorHomeKitchen_FurnishEdges_00596E11:
        call InteriorHomeCall005934E0
    InteriorHomeKitchen_FurnishEdges_00596E16:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596E1A:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596E1C:
        jle InteriorHomeKitchen_FurnishEdges_005970A0
    InteriorHomeKitchen_FurnishEdges_00596E22:
        fild dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596E26:
        fstp dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596E2A:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596E2F:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596E34:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596E38:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596E3C:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596E42:
        fmul dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596E46:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596E4B:
        mov ebp,3F800000h
    InteriorHomeKitchen_FurnishEdges_00596E50:
        mov dword ptr [esp+14h],eax
    InteriorHomeKitchen_FurnishEdges_00596E54:
        mov dword ptr [esp+eax*4+000001B8h],ebp
    InteriorHomeKitchen_FurnishEdges_00596E5B:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596E60:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596E65:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596E69:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596E6D:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596E73:
        fmul dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596E77:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596E7C:
        mov ebx,eax
    InteriorHomeKitchen_FurnishEdges_00596E7E:
        mov eax,dword ptr [esp+ebx*4+000001B8h]
    InteriorHomeKitchen_FurnishEdges_00596E85:
        xor edi,edi
    InteriorHomeKitchen_FurnishEdges_00596E87:
        cmp eax,ebp
    InteriorHomeKitchen_FurnishEdges_00596E89:
        jne InteriorHomeKitchen_FurnishEdges_00596ECC
    InteriorHomeKitchen_FurnishEdges_00596E8B:
        jmp InteriorHomeKitchen_FurnishEdges_00596E90
    InteriorHomeKitchen_FurnishEdges_00596E8D:
        lea ecx,[ecx]
    InteriorHomeKitchen_FurnishEdges_00596E90:
        cmp edi,1Eh
    InteriorHomeKitchen_FurnishEdges_00596E93:
        jge InteriorHomeKitchen_FurnishEdges_00596EC7
    InteriorHomeKitchen_FurnishEdges_00596E95:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596E9A:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596E9F:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596EA3:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596EA7:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596EAD:
        fmul dword ptr [esp+20h]
    InteriorHomeKitchen_FurnishEdges_00596EB1:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596EB6:
        mov ebx,eax
    InteriorHomeKitchen_FurnishEdges_00596EB8:
        mov eax,dword ptr [esp+ebx*4+000001B8h]
    InteriorHomeKitchen_FurnishEdges_00596EBF:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00596EC0:
        cmp eax,ebp
    InteriorHomeKitchen_FurnishEdges_00596EC2:
        je InteriorHomeKitchen_FurnishEdges_00596E90
    InteriorHomeKitchen_FurnishEdges_00596EC4:
        cmp edi,1Eh
    InteriorHomeKitchen_FurnishEdges_00596EC7:
        jne InteriorHomeKitchen_FurnishEdges_00596ECC
    InteriorHomeKitchen_FurnishEdges_00596EC9:
        or ebx,0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596ECC:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596ECF:
        mov al,byte ptr [ecx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596ED2:
        cmp al,4Bh
    InteriorHomeKitchen_FurnishEdges_00596ED4:
        jb InteriorHomeKitchen_FurnishEdges_00596EFD
    InteriorHomeKitchen_FurnishEdges_00596ED6:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596EDB:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596EE0:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596EE4:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596EE8:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596EEE:
        fmul dword ptr ds:[00858BA4h]
    InteriorHomeKitchen_FurnishEdges_00596EF4:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596EF9:
        mov ebp,eax
    InteriorHomeKitchen_FurnishEdges_00596EFB:
        jmp InteriorHomeKitchen_FurnishEdges_00596F55
    InteriorHomeKitchen_FurnishEdges_00596EFD:
        cmp al,32h
    InteriorHomeKitchen_FurnishEdges_00596EFF:
        jb InteriorHomeKitchen_FurnishEdges_00596F2B
    InteriorHomeKitchen_FurnishEdges_00596F01:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596F06:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596F0B:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596F0F:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596F13:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596F19:
        fmul dword ptr ds:[00859CE4h]
    InteriorHomeKitchen_FurnishEdges_00596F1F:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596F24:
        mov ebp,14h
    InteriorHomeKitchen_FurnishEdges_00596F29:
        jmp InteriorHomeKitchen_FurnishEdges_00596F53
    InteriorHomeKitchen_FurnishEdges_00596F2B:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596F30:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596F35:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596F39:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596F3D:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596F43:
        fmul dword ptr ds:[00859B60h]
    InteriorHomeKitchen_FurnishEdges_00596F49:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596F4E:
        mov ebp,32h
    InteriorHomeKitchen_FurnishEdges_00596F53:
        sub ebp,eax
    InteriorHomeKitchen_FurnishEdges_00596F55:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_00596F59:
        xor edi,edi
    InteriorHomeKitchen_FurnishEdges_00596F5B:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596F5D:
        jle InteriorHomeKitchen_FurnishEdges_00597018
    InteriorHomeKitchen_FurnishEdges_00596F63:
        jmp InteriorHomeKitchen_FurnishEdges_00596F70
    InteriorHomeKitchen_FurnishEdges_00596F65:
        lea esp,[esp+00000000h]
    InteriorHomeKitchen_FurnishEdges_00596F6C:
        lea esp,[esp]
    InteriorHomeKitchen_FurnishEdges_00596F70:
        fld dword ptr [esp+edi*4+000001B8h]
    InteriorHomeKitchen_FurnishEdges_00596F77:
        fcomp dword ptr ds:[00858B50h]
    InteriorHomeKitchen_FurnishEdges_00596F7D:
        fnstsw ax
    InteriorHomeKitchen_FurnishEdges_00596F7F:
        test ah,44h
    InteriorHomeKitchen_FurnishEdges_00596F82:
        jp InteriorHomeKitchen_FurnishEdges_0059700B
    InteriorHomeKitchen_FurnishEdges_00596F88:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596F8D:
        and eax,0FFFFh
    InteriorHomeKitchen_FurnishEdges_00596F92:
        mov dword ptr [esp+10h],eax
    InteriorHomeKitchen_FurnishEdges_00596F96:
        fild dword ptr [esp+10h]
    InteriorHomeKitchen_FurnishEdges_00596F9A:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeKitchen_FurnishEdges_00596FA0:
        fmul dword ptr ds:[00858628h]
    InteriorHomeKitchen_FurnishEdges_00596FA6:
        call InteriorHomeCall00821B40
    InteriorHomeKitchen_FurnishEdges_00596FAB:
        cmp eax,ebp
    InteriorHomeKitchen_FurnishEdges_00596FAD:
        jge InteriorHomeKitchen_FurnishEdges_0059700B
    InteriorHomeKitchen_FurnishEdges_00596FAF:
        call InteriorHomeCall00821B1E
    InteriorHomeKitchen_FurnishEdges_00596FB4:
        cmp eax,3FFFh
    InteriorHomeKitchen_FurnishEdges_00596FB9:
        jge InteriorHomeKitchen_FurnishEdges_00596FCA
    InteriorHomeKitchen_FurnishEdges_00596FBB:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596FBE:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00596FC0:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596FC3:
        push eax
    InteriorHomeKitchen_FurnishEdges_00596FC4:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596FC6:
        push 3
    InteriorHomeKitchen_FurnishEdges_00596FC8:
        jmp InteriorHomeKitchen_FurnishEdges_00596FD7
    InteriorHomeKitchen_FurnishEdges_00596FCA:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00596FCD:
        xor edx,edx
    InteriorHomeKitchen_FurnishEdges_00596FCF:
        mov dl,byte ptr [ecx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00596FD2:
        push edx
    InteriorHomeKitchen_FurnishEdges_00596FD3:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00596FD5:
        push 4
    InteriorHomeKitchen_FurnishEdges_00596FD7:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00596FDC:
        push 8
    InteriorHomeKitchen_FurnishEdges_00596FDE:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_00596FE3:
        test eax,eax
    InteriorHomeKitchen_FurnishEdges_00596FE5:
        je InteriorHomeKitchen_FurnishEdges_0059700B
    InteriorHomeKitchen_FurnishEdges_00596FE7:
        mov ecx,dword ptr [esp+edi*4+00000138h]
    InteriorHomeKitchen_FurnishEdges_00596FEE:
        mov edx,dword ptr [esp+edi*4+38h]
    InteriorHomeKitchen_FurnishEdges_00596FF2:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00596FF3:
        mov ecx,dword ptr [esp+edi*4+000000BCh]
    InteriorHomeKitchen_FurnishEdges_00596FFA:
        push 3F866666h
    InteriorHomeKitchen_FurnishEdges_00596FFF:
        push edx
    InteriorHomeKitchen_FurnishEdges_00597000:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00597001:
        push eax
    InteriorHomeKitchen_FurnishEdges_00597002:
        push 0
    InteriorHomeKitchen_FurnishEdges_00597004:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00597006:
        call InteriorHomeCall005934E0
    InteriorHomeKitchen_FurnishEdges_0059700B:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeKitchen_FurnishEdges_0059700F:
        inc edi
    InteriorHomeKitchen_FurnishEdges_00597010:
        cmp edi,eax
    InteriorHomeKitchen_FurnishEdges_00597012:
        jl InteriorHomeKitchen_FurnishEdges_00596F70
    InteriorHomeKitchen_FurnishEdges_00597018:
        mov edi,dword ptr [esp+14h]
    InteriorHomeKitchen_FurnishEdges_0059701C:
        cmp edi,0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_0059701F:
        je InteriorHomeKitchen_FurnishEdges_0059705E
    InteriorHomeKitchen_FurnishEdges_00597021:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00597024:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00597026:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeKitchen_FurnishEdges_00597029:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_0059702E:
        push eax
    InteriorHomeKitchen_FurnishEdges_0059702F:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00597031:
        push 8
    InteriorHomeKitchen_FurnishEdges_00597033:
        push 4
    InteriorHomeKitchen_FurnishEdges_00597035:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_0059703A:
        mov ecx,dword ptr [esp+edi*4+00000138h]
    InteriorHomeKitchen_FurnishEdges_00597041:
        mov edx,dword ptr [esp+edi*4+38h]
    InteriorHomeKitchen_FurnishEdges_00597045:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00597046:
        mov ecx,dword ptr [esp+edi*4+000000BCh]
    InteriorHomeKitchen_FurnishEdges_0059704D:
        push 3F866666h
    InteriorHomeKitchen_FurnishEdges_00597052:
        push edx
    InteriorHomeKitchen_FurnishEdges_00597053:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00597054:
        push eax
    InteriorHomeKitchen_FurnishEdges_00597055:
        push 1
    InteriorHomeKitchen_FurnishEdges_00597057:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_00597059:
        call InteriorHomeCall005934E0
    InteriorHomeKitchen_FurnishEdges_0059705E:
        cmp ebx,0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00597061:
        je InteriorHomeKitchen_FurnishEdges_005970A0
    InteriorHomeKitchen_FurnishEdges_00597063:
        mov edx,dword ptr [esi+14h]
    InteriorHomeKitchen_FurnishEdges_00597066:
        xor eax,eax
    InteriorHomeKitchen_FurnishEdges_00597068:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeKitchen_FurnishEdges_0059706B:
        mov ecx,0BAB380h
    InteriorHomeKitchen_FurnishEdges_00597070:
        push eax
    InteriorHomeKitchen_FurnishEdges_00597071:
        push 0FFFFFFFFh
    InteriorHomeKitchen_FurnishEdges_00597073:
        push 9
    InteriorHomeKitchen_FurnishEdges_00597075:
        push 4
    InteriorHomeKitchen_FurnishEdges_00597077:
        call InteriorHomeCall005911E0
    InteriorHomeKitchen_FurnishEdges_0059707C:
        mov ecx,dword ptr [esp+ebx*4+00000138h]
    InteriorHomeKitchen_FurnishEdges_00597083:
        mov edx,dword ptr [esp+ebx*4+38h]
    InteriorHomeKitchen_FurnishEdges_00597087:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00597088:
        mov ecx,dword ptr [esp+ebx*4+000000BCh]
    InteriorHomeKitchen_FurnishEdges_0059708F:
        push 3F866666h
    InteriorHomeKitchen_FurnishEdges_00597094:
        push edx
    InteriorHomeKitchen_FurnishEdges_00597095:
        push ecx
    InteriorHomeKitchen_FurnishEdges_00597096:
        push eax
    InteriorHomeKitchen_FurnishEdges_00597097:
        push 1
    InteriorHomeKitchen_FurnishEdges_00597099:
        mov ecx,esi
    InteriorHomeKitchen_FurnishEdges_0059709B:
        call InteriorHomeCall005934E0
    InteriorHomeKitchen_FurnishEdges_005970A0:
        pop edi
    InteriorHomeKitchen_FurnishEdges_005970A1:
        pop esi
    InteriorHomeKitchen_FurnishEdges_005970A2:
        pop ebp
    InteriorHomeKitchen_FurnishEdges_005970A3:
        pop ebx
    InteriorHomeKitchen_FurnishEdges_005970A4:
        mov esp,ebp
    InteriorHomeKitchen_FurnishEdges_005970A6:
        pop ebp
    InteriorHomeKitchen_FurnishEdges_005970A7:
        ret
    }
}

CObject* Interior_c::Kitchen_FurnishEdges() {
    return reinterpret_cast<CObject*(__thiscall*)(Interior_c*)>(&InteriorHomeNativeKitchen_FurnishEdges)(this);
}

// 0x5970B0
static __declspec(naked) void InteriorHomeNativeFurnishKitchen() {
    __asm {
    InteriorHomeFurnishKitchen_005970B0:
        sub esp,0Ch
    InteriorHomeFurnishKitchen_005970B3:
        push ebx
    InteriorHomeFurnishKitchen_005970B4:
        push ebp
    InteriorHomeFurnishKitchen_005970B5:
        push esi
    InteriorHomeFurnishKitchen_005970B6:
        push edi
    InteriorHomeFurnishKitchen_005970B7:
        push 0
    InteriorHomeFurnishKitchen_005970B9:
        mov esi,ecx
    InteriorHomeFurnishKitchen_005970BB:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishKitchen_005970BE:
        movsx ecx,byte ptr [eax+5]
    InteriorHomeFurnishKitchen_005970C2:
        push 7
    InteriorHomeFurnishKitchen_005970C4:
        push 1
    InteriorHomeFurnishKitchen_005970C6:
        push 2
    InteriorHomeFurnishKitchen_005970C8:
        dec ecx
    InteriorHomeFurnishKitchen_005970C9:
        push 0
    InteriorHomeFurnishKitchen_005970CB:
        push ecx
    InteriorHomeFurnishKitchen_005970CC:
        mov ecx,esi
    InteriorHomeFurnishKitchen_005970CE:
        call InteriorHomeCall00591700
    InteriorHomeFurnishKitchen_005970D3:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishKitchen_005970D6:
        movzx edi,byte ptr [eax+2]
    InteriorHomeFurnishKitchen_005970DA:
        movzx ebx,byte ptr [eax+3]
    InteriorHomeFurnishKitchen_005970DE:
        sub edi,2
    InteriorHomeFurnishKitchen_005970E1:
        mov ebp,1
    InteriorHomeFurnishKitchen_005970E6:
        sub ebx,2
    InteriorHomeFurnishKitchen_005970E9:
        cmp edi,ebp
    InteriorHomeFurnishKitchen_005970EB:
        jl InteriorHomeFurnishKitchen_00597118
    InteriorHomeFurnishKitchen_005970ED:
        lea ecx,[ecx]
    InteriorHomeFurnishKitchen_005970F0:
        push 0
    InteriorHomeFurnishKitchen_005970F2:
        push 3
    InteriorHomeFurnishKitchen_005970F4:
        push 1
    InteriorHomeFurnishKitchen_005970F6:
        push 1
    InteriorHomeFurnishKitchen_005970F8:
        push ebx
    InteriorHomeFurnishKitchen_005970F9:
        push ebp
    InteriorHomeFurnishKitchen_005970FA:
        mov ecx,esi
    InteriorHomeFurnishKitchen_005970FC:
        call InteriorHomeCall00591700
    InteriorHomeFurnishKitchen_00597101:
        push 0
    InteriorHomeFurnishKitchen_00597103:
        push 3
    InteriorHomeFurnishKitchen_00597105:
        push 1
    InteriorHomeFurnishKitchen_00597107:
        push 1
    InteriorHomeFurnishKitchen_00597109:
        push 0
    InteriorHomeFurnishKitchen_0059710B:
        push ebp
    InteriorHomeFurnishKitchen_0059710C:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059710E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishKitchen_00597113:
        inc ebp
    InteriorHomeFurnishKitchen_00597114:
        cmp ebp,edi
    InteriorHomeFurnishKitchen_00597116:
        jle InteriorHomeFurnishKitchen_005970F0
    InteriorHomeFurnishKitchen_00597118:
        xor ebp,ebp
    InteriorHomeFurnishKitchen_0059711A:
        test ebx,ebx
    InteriorHomeFurnishKitchen_0059711C:
        jl InteriorHomeFurnishKitchen_00597148
    InteriorHomeFurnishKitchen_0059711E:
        mov edi,edi
    InteriorHomeFurnishKitchen_00597120:
        push 0
    InteriorHomeFurnishKitchen_00597122:
        push 3
    InteriorHomeFurnishKitchen_00597124:
        push 1
    InteriorHomeFurnishKitchen_00597126:
        push 1
    InteriorHomeFurnishKitchen_00597128:
        push ebp
    InteriorHomeFurnishKitchen_00597129:
        push 1
    InteriorHomeFurnishKitchen_0059712B:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059712D:
        call InteriorHomeCall00591700
    InteriorHomeFurnishKitchen_00597132:
        push 0
    InteriorHomeFurnishKitchen_00597134:
        push 3
    InteriorHomeFurnishKitchen_00597136:
        push 1
    InteriorHomeFurnishKitchen_00597138:
        push 1
    InteriorHomeFurnishKitchen_0059713A:
        push ebp
    InteriorHomeFurnishKitchen_0059713B:
        push edi
    InteriorHomeFurnishKitchen_0059713C:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059713E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishKitchen_00597143:
        inc ebp
    InteriorHomeFurnishKitchen_00597144:
        cmp ebp,ebx
    InteriorHomeFurnishKitchen_00597146:
        jle InteriorHomeFurnishKitchen_00597120
    InteriorHomeFurnishKitchen_00597148:
        push 0
    InteriorHomeFurnishKitchen_0059714A:
        push 0
    InteriorHomeFurnishKitchen_0059714C:
        push 1
    InteriorHomeFurnishKitchen_0059714E:
        push 1
    InteriorHomeFurnishKitchen_00597150:
        mov ecx,esi
    InteriorHomeFurnishKitchen_00597152:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishKitchen_00597157:
        push 0
    InteriorHomeFurnishKitchen_00597159:
        push 0
    InteriorHomeFurnishKitchen_0059715B:
        push ebx
    InteriorHomeFurnishKitchen_0059715C:
        push 1
    InteriorHomeFurnishKitchen_0059715E:
        mov ecx,esi
    InteriorHomeFurnishKitchen_00597160:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishKitchen_00597165:
        push 0
    InteriorHomeFurnishKitchen_00597167:
        push 0
    InteriorHomeFurnishKitchen_00597169:
        push 1
    InteriorHomeFurnishKitchen_0059716B:
        push edi
    InteriorHomeFurnishKitchen_0059716C:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059716E:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishKitchen_00597173:
        push 0
    InteriorHomeFurnishKitchen_00597175:
        push 0
    InteriorHomeFurnishKitchen_00597177:
        push ebx
    InteriorHomeFurnishKitchen_00597178:
        push edi
    InteriorHomeFurnishKitchen_00597179:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059717B:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishKitchen_00597180:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishKitchen_00597183:
        xor eax,eax
    InteriorHomeFurnishKitchen_00597185:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeFurnishKitchen_00597188:
        mov ecx,0BAB380h
    InteriorHomeFurnishKitchen_0059718D:
        push eax
    InteriorHomeFurnishKitchen_0059718E:
        push 0
    InteriorHomeFurnishKitchen_00597190:
        push 4
    InteriorHomeFurnishKitchen_00597192:
        call InteriorHomeCall00591220
    InteriorHomeFurnishKitchen_00597197:
        mov ecx,esi
    InteriorHomeFurnishKitchen_00597199:
        mov byte ptr [esi+00000791h],al
    InteriorHomeFurnishKitchen_0059719F:
        call InteriorHomeCall00596930
    InteriorHomeFurnishKitchen_005971A4:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishKitchen_005971A7:
        xor edx,edx
    InteriorHomeFurnishKitchen_005971A9:
        mov dl,byte ptr [ecx+1Fh]
    InteriorHomeFurnishKitchen_005971AC:
        mov ecx,0BAB380h
    InteriorHomeFurnishKitchen_005971B1:
        push edx
    InteriorHomeFurnishKitchen_005971B2:
        push 0FFFFFFFFh
    InteriorHomeFurnishKitchen_005971B4:
        push 1
    InteriorHomeFurnishKitchen_005971B6:
        push 8
    InteriorHomeFurnishKitchen_005971B8:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishKitchen_005971BD:
        mov ebx,dword ptr [esi+14h]
    InteriorHomeFurnishKitchen_005971C0:
        movzx edx,byte ptr [ebx+3]
    InteriorHomeFurnishKitchen_005971C4:
        push 0
    InteriorHomeFurnishKitchen_005971C6:
        mov edi,eax
    InteriorHomeFurnishKitchen_005971C8:
        lea eax,[esp+18h]
    InteriorHomeFurnishKitchen_005971CC:
        push eax
    InteriorHomeFurnishKitchen_005971CD:
        movzx eax,byte ptr [edi+0Dh]
    InteriorHomeFurnishKitchen_005971D1:
        mov dword ptr [esp+18h],edx
    InteriorHomeFurnishKitchen_005971D5:
        lea ecx,[esp+20h]
    InteriorHomeFurnishKitchen_005971D9:
        push ecx
    InteriorHomeFurnishKitchen_005971DA:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishKitchen_005971DE:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishKitchen_005971E2:
        push 0
    InteriorHomeFurnishKitchen_005971E4:
        push 0
    InteriorHomeFurnishKitchen_005971E6:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishKitchen_005971EC:
        push 0
    InteriorHomeFurnishKitchen_005971EE:
        fild dword ptr [esp+28h]
    InteriorHomeFurnishKitchen_005971F2:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishKitchen_005971F8:
        fsubp st(1),st(0)
    InteriorHomeFurnishKitchen_005971FA:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishKitchen_005971FF:
        movzx ecx,byte ptr [ebx+2]
    InteriorHomeFurnishKitchen_00597203:
        movzx edx,byte ptr [edi+0Ch]
    InteriorHomeFurnishKitchen_00597207:
        mov dword ptr [esp+28h],ecx
    InteriorHomeFurnishKitchen_0059720B:
        push eax
    InteriorHomeFurnishKitchen_0059720C:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishKitchen_00597210:
        mov dword ptr [esp+2Ch],edx
    InteriorHomeFurnishKitchen_00597214:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishKitchen_0059721A:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishKitchen_0059721E:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishKitchen_00597224:
        fsubp st(1),st(0)
    InteriorHomeFurnishKitchen_00597226:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishKitchen_0059722B:
        push eax
    InteriorHomeFurnishKitchen_0059722C:
        push edi
    InteriorHomeFurnishKitchen_0059722D:
        mov ecx,esi
    InteriorHomeFurnishKitchen_0059722F:
        call InteriorHomeCall00592AA0
    InteriorHomeFurnishKitchen_00597234:
        pop edi
    InteriorHomeFurnishKitchen_00597235:
        pop esi
    InteriorHomeFurnishKitchen_00597236:
        pop ebp
    InteriorHomeFurnishKitchen_00597237:
        pop ebx
    InteriorHomeFurnishKitchen_00597238:
        add esp,0Ch
    InteriorHomeFurnishKitchen_0059723B:
        ret
    }
}

void Interior_c::FurnishKitchen() {
    reinterpret_cast<void(__thiscall*)(Interior_c*)>(&InteriorHomeNativeFurnishKitchen)(this);
}

// 0x597240
static __declspec(naked) void InteriorHomeNativeLounge_AddTV() {
    __asm {
    InteriorHomeLounge_AddTV_00597240:
        sub esp,10h
    InteriorHomeLounge_AddTV_00597243:
        mov eax,dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_00597247:
        push esi
    InteriorHomeLounge_AddTV_00597248:
        mov esi,ecx
    InteriorHomeLounge_AddTV_0059724A:
        mov ecx,eax
    InteriorHomeLounge_AddTV_0059724C:
        and ecx,3
    InteriorHomeLounge_AddTV_0059724F:
        test eax,eax
    InteriorHomeLounge_AddTV_00597251:
        mov dword ptr [esp+10h],ecx
    InteriorHomeLounge_AddTV_00597255:
        jne InteriorHomeLounge_AddTV_0059729E
    InteriorHomeLounge_AddTV_00597257:
        mov edx,dword ptr [esi+14h]
    InteriorHomeLounge_AddTV_0059725A:
        movzx eax,byte ptr [edx+3]
    InteriorHomeLounge_AddTV_0059725E:
        mov dword ptr [esp+18h],eax
    InteriorHomeLounge_AddTV_00597262:
        push 0
    InteriorHomeLounge_AddTV_00597264:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_00597266:
        fild dword ptr [esp+20h]
    InteriorHomeLounge_AddTV_0059726A:
        push ecx
    InteriorHomeLounge_AddTV_0059726B:
        fld dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddTV_00597271:
        mov dword ptr [esp+10h],3F000000h
    InteriorHomeLounge_AddTV_00597279:
        mov dword ptr [esp+18h],3FC00000h
    InteriorHomeLounge_AddTV_00597281:
        fsubr st(0),st(1)
    InteriorHomeLounge_AddTV_00597283:
        fst dword ptr [esp+24h]
    InteriorHomeLounge_AddTV_00597287:
        fstp dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_0059728B:
        fsub dword ptr ds:[00858CA0h]
    InteriorHomeLounge_AddTV_00597291:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddTV_00597294:
        push 3F800000h
    InteriorHomeLounge_AddTV_00597299:
        jmp InteriorHomeLounge_AddTV_00597380
    InteriorHomeLounge_AddTV_0059729E:
        cmp eax,2
    InteriorHomeLounge_AddTV_005972A1:
        jne InteriorHomeLounge_AddTV_005972EB
    InteriorHomeLounge_AddTV_005972A3:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeLounge_AddTV_005972A6:
        movzx edx,byte ptr [ecx+2]
    InteriorHomeLounge_AddTV_005972AA:
        mov dword ptr [esp+18h],edx
    InteriorHomeLounge_AddTV_005972AE:
        push 0
    InteriorHomeLounge_AddTV_005972B0:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_005972B2:
        fild dword ptr [esp+20h]
    InteriorHomeLounge_AddTV_005972B6:
        push 3F800000h
    InteriorHomeLounge_AddTV_005972BB:
        fld dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddTV_005972C1:
        mov dword ptr [esp+24h],3F000000h
    InteriorHomeLounge_AddTV_005972C9:
        mov dword ptr [esp+14h],3F000000h
    InteriorHomeLounge_AddTV_005972D1:
        fsubr st(0),st(1)
    InteriorHomeLounge_AddTV_005972D3:
        push ecx
    InteriorHomeLounge_AddTV_005972D4:
        fstp dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_005972D8:
        fld dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_005972DC:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddTV_005972E2:
        fstp dword ptr [esp+1Ch]
    InteriorHomeLounge_AddTV_005972E6:
        jmp InteriorHomeLounge_AddTV_00597377
    InteriorHomeLounge_AddTV_005972EB:
        cmp eax,1
    InteriorHomeLounge_AddTV_005972EE:
        jne InteriorHomeLounge_AddTV_00597320
    InteriorHomeLounge_AddTV_005972F0:
        push 0
    InteriorHomeLounge_AddTV_005972F2:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_005972F4:
        push 3F800000h
    InteriorHomeLounge_AddTV_005972F9:
        mov dword ptr [esp+10h],3F000000h
    InteriorHomeLounge_AddTV_00597301:
        mov dword ptr [esp+18h],3F000000h
    InteriorHomeLounge_AddTV_00597309:
        mov dword ptr [esp+24h],3F000000h
    InteriorHomeLounge_AddTV_00597311:
        mov dword ptr [esp+14h],3FC00000h
    InteriorHomeLounge_AddTV_00597319:
        push 3F800000h
    InteriorHomeLounge_AddTV_0059731E:
        jmp InteriorHomeLounge_AddTV_00597380
    InteriorHomeLounge_AddTV_00597320:
        cmp eax,3
    InteriorHomeLounge_AddTV_00597323:
        jne InteriorHomeLounge_AddTV_00597389
    InteriorHomeLounge_AddTV_00597325:
        mov eax,dword ptr [esi+14h]
    InteriorHomeLounge_AddTV_00597328:
        movzx ecx,byte ptr [eax+2]
    InteriorHomeLounge_AddTV_0059732C:
        movzx edx,byte ptr [eax+3]
    InteriorHomeLounge_AddTV_00597330:
        mov dword ptr [esp+18h],ecx
    InteriorHomeLounge_AddTV_00597334:
        push 0
    InteriorHomeLounge_AddTV_00597336:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_00597338:
        fild dword ptr [esp+20h]
    InteriorHomeLounge_AddTV_0059733C:
        mov dword ptr [esp+20h],edx
    InteriorHomeLounge_AddTV_00597340:
        fld dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddTV_00597346:
        sub esp,8
    InteriorHomeLounge_AddTV_00597349:
        fsubr st(0),st(1)
    InteriorHomeLounge_AddTV_0059734B:
        fst dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_0059734F:
        fstp dword ptr [esp+1Ch]
    InteriorHomeLounge_AddTV_00597353:
        fild dword ptr [esp+28h]
    InteriorHomeLounge_AddTV_00597357:
        fld dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddTV_0059735D:
        fsubr st(0),st(1)
    InteriorHomeLounge_AddTV_0059735F:
        fst dword ptr [esp+28h]
    InteriorHomeLounge_AddTV_00597363:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddTV_00597369:
        fstp dword ptr [esp+18h]
    InteriorHomeLounge_AddTV_0059736D:
        fsub dword ptr ds:[00858CA0h]
    InteriorHomeLounge_AddTV_00597373:
        fstp dword ptr [esp+4]
    InteriorHomeLounge_AddTV_00597377:
        fsub dword ptr ds:[00858CA0h]
    InteriorHomeLounge_AddTV_0059737D:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddTV_00597380:
        mov ecx,esi
    InteriorHomeLounge_AddTV_00597382:
        push 0
    InteriorHomeLounge_AddTV_00597384:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddTV_00597389:
        mov eax,dword ptr [esi+14h]
    InteriorHomeLounge_AddTV_0059738C:
        xor ecx,ecx
    InteriorHomeLounge_AddTV_0059738E:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeLounge_AddTV_00597391:
        push ecx
    InteriorHomeLounge_AddTV_00597392:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_00597394:
        push 3
    InteriorHomeLounge_AddTV_00597396:
        push 2
    InteriorHomeLounge_AddTV_00597398:
        mov ecx,0BAB380h
    InteriorHomeLounge_AddTV_0059739D:
        call InteriorHomeCall005911E0
    InteriorHomeLounge_AddTV_005973A2:
        fild dword ptr [esp+10h]
    InteriorHomeLounge_AddTV_005973A6:
        mov edx,dword ptr [esp+18h]
    InteriorHomeLounge_AddTV_005973AA:
        push ecx
    InteriorHomeLounge_AddTV_005973AB:
        fmul dword ptr ds:[0085991Ch]
    InteriorHomeLounge_AddTV_005973B1:
        mov ecx,dword ptr [esp+8]
    InteriorHomeLounge_AddTV_005973B5:
        fst dword ptr [esp+14h]
    InteriorHomeLounge_AddTV_005973B9:
        fadd dword ptr ds:[00858CB4h]
    InteriorHomeLounge_AddTV_005973BF:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddTV_005973C2:
        push 3F000000h
    InteriorHomeLounge_AddTV_005973C7:
        push edx
    InteriorHomeLounge_AddTV_005973C8:
        push ecx
    InteriorHomeLounge_AddTV_005973C9:
        push eax
    InteriorHomeLounge_AddTV_005973CA:
        push 1
    InteriorHomeLounge_AddTV_005973CC:
        mov ecx,esi
    InteriorHomeLounge_AddTV_005973CE:
        call InteriorHomeCall005934E0
    InteriorHomeLounge_AddTV_005973D3:
        call InteriorHomeCall00821B1E
    InteriorHomeLounge_AddTV_005973D8:
        cmp eax,3FFFh
    InteriorHomeLounge_AddTV_005973DD:
        mov edx,dword ptr [esi+14h]
    InteriorHomeLounge_AddTV_005973E0:
        mov ecx,0BAB380h
    InteriorHomeLounge_AddTV_005973E5:
        jge InteriorHomeLounge_AddTV_005973F3
    InteriorHomeLounge_AddTV_005973E7:
        xor eax,eax
    InteriorHomeLounge_AddTV_005973E9:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeLounge_AddTV_005973EC:
        push eax
    InteriorHomeLounge_AddTV_005973ED:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_005973EF:
        push 7
    InteriorHomeLounge_AddTV_005973F1:
        jmp InteriorHomeLounge_AddTV_005973FD
    InteriorHomeLounge_AddTV_005973F3:
        xor eax,eax
    InteriorHomeLounge_AddTV_005973F5:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeLounge_AddTV_005973F8:
        push eax
    InteriorHomeLounge_AddTV_005973F9:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddTV_005973FB:
        push 9
    InteriorHomeLounge_AddTV_005973FD:
        push 2
    InteriorHomeLounge_AddTV_005973FF:
        call InteriorHomeCall005911E0
    InteriorHomeLounge_AddTV_00597404:
        mov ecx,dword ptr [esp+10h]
    InteriorHomeLounge_AddTV_00597408:
        mov edx,dword ptr [esp+8]
    InteriorHomeLounge_AddTV_0059740C:
        push ecx
    InteriorHomeLounge_AddTV_0059740D:
        mov ecx,dword ptr [esp+10h]
    InteriorHomeLounge_AddTV_00597411:
        push 3F000000h
    InteriorHomeLounge_AddTV_00597416:
        push edx
    InteriorHomeLounge_AddTV_00597417:
        push ecx
    InteriorHomeLounge_AddTV_00597418:
        push eax
    InteriorHomeLounge_AddTV_00597419:
        mov ecx,esi
    InteriorHomeLounge_AddTV_0059741B:
        push 1
    InteriorHomeLounge_AddTV_0059741D:
        call InteriorHomeCall005934E0
    InteriorHomeLounge_AddTV_00597422:
        pop esi
    InteriorHomeLounge_AddTV_00597423:
        add esp,10h
    InteriorHomeLounge_AddTV_00597426:
        ret 10h
    }
}

CObject* Interior_c::Lounge_AddTV(int32 a2, int32 a3, int32 a4, int32 a5) {
    return reinterpret_cast<CObject*(__thiscall*)(Interior_c*, int32, int32, int32, int32)>(&InteriorHomeNativeLounge_AddTV)(this, a2, a3, a4, a5);
}

// 0x597430
static __declspec(naked) void InteriorHomeNativeLounge_AddHifi() {
    __asm {
    InteriorHomeLounge_AddHifi_00597430:
        push ecx
    InteriorHomeLounge_AddHifi_00597431:
        fild dword ptr [esp+0Ch]
    InteriorHomeLounge_AddHifi_00597435:
        mov eax,dword ptr [esp+8]
    InteriorHomeLounge_AddHifi_00597439:
        push esi
    InteriorHomeLounge_AddHifi_0059743A:
        mov esi,ecx
    InteriorHomeLounge_AddHifi_0059743C:
        fstp dword ptr [esp+10h]
    InteriorHomeLounge_AddHifi_00597440:
        mov ecx,eax
    InteriorHomeLounge_AddHifi_00597442:
        fild dword ptr [esp+14h]
    InteriorHomeLounge_AddHifi_00597446:
        and ecx,3
    InteriorHomeLounge_AddHifi_00597449:
        test eax,eax
    InteriorHomeLounge_AddHifi_0059744B:
        mov dword ptr [esp+4],ecx
    InteriorHomeLounge_AddHifi_0059744F:
        fstp dword ptr [esp+0Ch]
    InteriorHomeLounge_AddHifi_00597453:
        je InteriorHomeLounge_AddHifi_00597474
    InteriorHomeLounge_AddHifi_00597455:
        cmp eax,2
    InteriorHomeLounge_AddHifi_00597458:
        je InteriorHomeLounge_AddHifi_00597474
    InteriorHomeLounge_AddHifi_0059745A:
        cmp eax,1
    InteriorHomeLounge_AddHifi_0059745D:
        je InteriorHomeLounge_AddHifi_00597464
    InteriorHomeLounge_AddHifi_0059745F:
        cmp eax,3
    InteriorHomeLounge_AddHifi_00597462:
        jne InteriorHomeLounge_AddHifi_00597482
    InteriorHomeLounge_AddHifi_00597464:
        fld dword ptr [esp+0Ch]
    InteriorHomeLounge_AddHifi_00597468:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddHifi_0059746E:
        fstp dword ptr [esp+0Ch]
    InteriorHomeLounge_AddHifi_00597472:
        jmp InteriorHomeLounge_AddHifi_00597482
    InteriorHomeLounge_AddHifi_00597474:
        fld dword ptr [esp+10h]
    InteriorHomeLounge_AddHifi_00597478:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddHifi_0059747E:
        fstp dword ptr [esp+10h]
    InteriorHomeLounge_AddHifi_00597482:
        mov edx,dword ptr [esi+14h]
    InteriorHomeLounge_AddHifi_00597485:
        xor eax,eax
    InteriorHomeLounge_AddHifi_00597487:
        mov al,byte ptr [edx+1Fh]
    InteriorHomeLounge_AddHifi_0059748A:
        mov ecx,0BAB380h
    InteriorHomeLounge_AddHifi_0059748F:
        push eax
    InteriorHomeLounge_AddHifi_00597490:
        push 0FFFFFFFFh
    InteriorHomeLounge_AddHifi_00597492:
        push 8
    InteriorHomeLounge_AddHifi_00597494:
        push 2
    InteriorHomeLounge_AddHifi_00597496:
        call InteriorHomeCall005911E0
    InteriorHomeLounge_AddHifi_0059749B:
        fild dword ptr [esp+4]
    InteriorHomeLounge_AddHifi_0059749F:
        push ecx
    InteriorHomeLounge_AddHifi_005974A0:
        mov ecx,esi
    InteriorHomeLounge_AddHifi_005974A2:
        fmul dword ptr ds:[0085991Ch]
    InteriorHomeLounge_AddHifi_005974A8:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddHifi_005974AB:
        push 3F000000h
    InteriorHomeLounge_AddHifi_005974B0:
        fld dword ptr [esp+14h]
    InteriorHomeLounge_AddHifi_005974B4:
        sub esp,8
    InteriorHomeLounge_AddHifi_005974B7:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddHifi_005974BD:
        fstp dword ptr [esp+4]
    InteriorHomeLounge_AddHifi_005974C1:
        fld dword ptr [esp+20h]
    InteriorHomeLounge_AddHifi_005974C5:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddHifi_005974CB:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddHifi_005974CE:
        push eax
    InteriorHomeLounge_AddHifi_005974CF:
        push 1
    InteriorHomeLounge_AddHifi_005974D1:
        call InteriorHomeCall005934E0
    InteriorHomeLounge_AddHifi_005974D6:
        pop esi
    InteriorHomeLounge_AddHifi_005974D7:
        pop ecx
    InteriorHomeLounge_AddHifi_005974D8:
        ret 10h
    }
}

CObject* Interior_c::Lounge_AddHifi(int32 a2, int32 a3, int32 a4, int32 a5) {
    return reinterpret_cast<CObject*(__thiscall*)(Interior_c*, int32, int32, int32, int32)>(&InteriorHomeNativeLounge_AddHifi)(this, a2, a3, a4, a5);
}

// 0x5974E0
static __declspec(naked) void InteriorHomeNativeLounge_AddChairInfo() {
    __asm {
    InteriorHomeLounge_AddChairInfo_005974E0:
        mov edx,dword ptr [esp+4]
    InteriorHomeLounge_AddChairInfo_005974E4:
        lea eax,[edx-2]
    InteriorHomeLounge_AddChairInfo_005974E7:
        and eax,3
    InteriorHomeLounge_AddChairInfo_005974EA:
        test edx,edx
    InteriorHomeLounge_AddChairInfo_005974EC:
        jne InteriorHomeLounge_AddChairInfo_00597528
    InteriorHomeLounge_AddChairInfo_005974EE:
        mov edx,dword ptr [esp+0Ch]
    InteriorHomeLounge_AddChairInfo_005974F2:
        push edx
    InteriorHomeLounge_AddChairInfo_005974F3:
        push eax
    InteriorHomeLounge_AddChairInfo_005974F4:
        mov eax,dword ptr [ecx+14h]
    InteriorHomeLounge_AddChairInfo_005974F7:
        movzx edx,byte ptr [eax+3]
    InteriorHomeLounge_AddChairInfo_005974FB:
        dec edx
    InteriorHomeLounge_AddChairInfo_005974FC:
        mov dword ptr [esp+14h],edx
    InteriorHomeLounge_AddChairInfo_00597500:
        sub esp,8
    InteriorHomeLounge_AddChairInfo_00597503:
        fild dword ptr [esp+1Ch]
    InteriorHomeLounge_AddChairInfo_00597507:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddChairInfo_0059750D:
        fstp dword ptr [esp+4]
    InteriorHomeLounge_AddChairInfo_00597511:
        fild dword ptr [esp+18h]
    InteriorHomeLounge_AddChairInfo_00597515:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddChairInfo_0059751B:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddChairInfo_0059751E:
        push 1
    InteriorHomeLounge_AddChairInfo_00597520:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddChairInfo_00597525:
        ret 0Ch
    InteriorHomeLounge_AddChairInfo_00597528:
        cmp edx,2
    InteriorHomeLounge_AddChairInfo_0059752B:
        jne InteriorHomeLounge_AddChairInfo_00597550
    InteriorHomeLounge_AddChairInfo_0059752D:
        mov edx,dword ptr [esp+0Ch]
    InteriorHomeLounge_AddChairInfo_00597531:
        fild dword ptr [esp+8]
    InteriorHomeLounge_AddChairInfo_00597535:
        push edx
    InteriorHomeLounge_AddChairInfo_00597536:
        push eax
    InteriorHomeLounge_AddChairInfo_00597537:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddChairInfo_0059753D:
        push 3F800000h
    InteriorHomeLounge_AddChairInfo_00597542:
        push ecx
    InteriorHomeLounge_AddChairInfo_00597543:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddChairInfo_00597546:
        push 1
    InteriorHomeLounge_AddChairInfo_00597548:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddChairInfo_0059754D:
        ret 0Ch
    InteriorHomeLounge_AddChairInfo_00597550:
        cmp edx,1
    InteriorHomeLounge_AddChairInfo_00597553:
        jne InteriorHomeLounge_AddChairInfo_00597578
    InteriorHomeLounge_AddChairInfo_00597555:
        mov edx,dword ptr [esp+0Ch]
    InteriorHomeLounge_AddChairInfo_00597559:
        fild dword ptr [esp+8]
    InteriorHomeLounge_AddChairInfo_0059755D:
        push edx
    InteriorHomeLounge_AddChairInfo_0059755E:
        push eax
    InteriorHomeLounge_AddChairInfo_0059755F:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddChairInfo_00597565:
        push ecx
    InteriorHomeLounge_AddChairInfo_00597566:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddChairInfo_00597569:
        push 3F800000h
    InteriorHomeLounge_AddChairInfo_0059756E:
        push 1
    InteriorHomeLounge_AddChairInfo_00597570:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddChairInfo_00597575:
        ret 0Ch
    InteriorHomeLounge_AddChairInfo_00597578:
        cmp edx,3
    InteriorHomeLounge_AddChairInfo_0059757B:
        jne InteriorHomeLounge_AddChairInfo_005975B4
    InteriorHomeLounge_AddChairInfo_0059757D:
        mov edx,dword ptr [esp+0Ch]
    InteriorHomeLounge_AddChairInfo_00597581:
        fild dword ptr [esp+8]
    InteriorHomeLounge_AddChairInfo_00597585:
        push edx
    InteriorHomeLounge_AddChairInfo_00597586:
        push eax
    InteriorHomeLounge_AddChairInfo_00597587:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddChairInfo_0059758D:
        mov eax,dword ptr [ecx+14h]
    InteriorHomeLounge_AddChairInfo_00597590:
        movzx edx,byte ptr [eax+2]
    InteriorHomeLounge_AddChairInfo_00597594:
        sub esp,8
    InteriorHomeLounge_AddChairInfo_00597597:
        dec edx
    InteriorHomeLounge_AddChairInfo_00597598:
        mov dword ptr [esp+1Ch],edx
    InteriorHomeLounge_AddChairInfo_0059759C:
        fstp dword ptr [esp+4]
    InteriorHomeLounge_AddChairInfo_005975A0:
        fild dword ptr [esp+1Ch]
    InteriorHomeLounge_AddChairInfo_005975A4:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddChairInfo_005975AA:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddChairInfo_005975AD:
        push 1
    InteriorHomeLounge_AddChairInfo_005975AF:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddChairInfo_005975B4:
        ret 0Ch
    }
}

void Interior_c::Lounge_AddChairInfo(int32 a2, int32 a3, CEntity* entityIgnoredCollision) {
    reinterpret_cast<void(__thiscall*)(Interior_c*, int32, int32, CEntity*)>(&InteriorHomeNativeLounge_AddChairInfo)(this, a2, a3, entityIgnoredCollision);
}

// 0x5975C0
static __declspec(naked) void InteriorHomeNativeLounge_AddSofaInfo() {
    __asm {
    InteriorHomeLounge_AddSofaInfo_005975C0:
        mov eax,dword ptr [esp+4]
    InteriorHomeLounge_AddSofaInfo_005975C4:
        push ebx
    InteriorHomeLounge_AddSofaInfo_005975C5:
        push ebp
    InteriorHomeLounge_AddSofaInfo_005975C6:
        push esi
    InteriorHomeLounge_AddSofaInfo_005975C7:
        push edi
    InteriorHomeLounge_AddSofaInfo_005975C8:
        lea edi,[eax-2]
    InteriorHomeLounge_AddSofaInfo_005975CB:
        and edi,3
    InteriorHomeLounge_AddSofaInfo_005975CE:
        test eax,eax
    InteriorHomeLounge_AddSofaInfo_005975D0:
        mov esi,ecx
    InteriorHomeLounge_AddSofaInfo_005975D2:
        jne InteriorHomeLounge_AddSofaInfo_00597636
    InteriorHomeLounge_AddSofaInfo_005975D4:
        mov eax,dword ptr [esi+14h]
    InteriorHomeLounge_AddSofaInfo_005975D7:
        fild dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_005975DB:
        movzx ecx,byte ptr [eax+3]
    InteriorHomeLounge_AddSofaInfo_005975DF:
        mov ebx,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005975E3:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddSofaInfo_005975E9:
        dec ecx
    InteriorHomeLounge_AddSofaInfo_005975EA:
        mov dword ptr [esp+14h],ecx
    InteriorHomeLounge_AddSofaInfo_005975EE:
        push ebx
    InteriorHomeLounge_AddSofaInfo_005975EF:
        fstp dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005975F3:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005975F7:
        fild dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_005975FB:
        push edi
    InteriorHomeLounge_AddSofaInfo_005975FC:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_005975FE:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_00597604:
        fstp dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_00597608:
        mov ebp,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_0059760C:
        push ebp
    InteriorHomeLounge_AddSofaInfo_0059760D:
        push edx
    InteriorHomeLounge_AddSofaInfo_0059760E:
        push 1
    InteriorHomeLounge_AddSofaInfo_00597610:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_00597615:
        fld dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_00597619:
        fadd dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_0059761F:
        push ebx
    InteriorHomeLounge_AddSofaInfo_00597620:
        push edi
    InteriorHomeLounge_AddSofaInfo_00597621:
        push ebp
    InteriorHomeLounge_AddSofaInfo_00597622:
        push ecx
    InteriorHomeLounge_AddSofaInfo_00597623:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddSofaInfo_00597626:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_00597628:
        push 1
    InteriorHomeLounge_AddSofaInfo_0059762A:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_0059762F:
        pop edi
    InteriorHomeLounge_AddSofaInfo_00597630:
        pop esi
    InteriorHomeLounge_AddSofaInfo_00597631:
        pop ebp
    InteriorHomeLounge_AddSofaInfo_00597632:
        pop ebx
    InteriorHomeLounge_AddSofaInfo_00597633:
        ret 0Ch
    InteriorHomeLounge_AddSofaInfo_00597636:
        cmp eax,2
    InteriorHomeLounge_AddSofaInfo_00597639:
        jne InteriorHomeLounge_AddSofaInfo_00597687
    InteriorHomeLounge_AddSofaInfo_0059763B:
        fild dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_0059763F:
        mov ebx,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_00597643:
        push ebx
    InteriorHomeLounge_AddSofaInfo_00597644:
        push edi
    InteriorHomeLounge_AddSofaInfo_00597645:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddSofaInfo_0059764B:
        push 3F800000h
    InteriorHomeLounge_AddSofaInfo_00597650:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_00597652:
        fstp dword ptr [esp+24h]
    InteriorHomeLounge_AddSofaInfo_00597656:
        mov eax,dword ptr [esp+24h]
    InteriorHomeLounge_AddSofaInfo_0059765A:
        push eax
    InteriorHomeLounge_AddSofaInfo_0059765B:
        push 1
    InteriorHomeLounge_AddSofaInfo_0059765D:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_00597662:
        fld dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_00597666:
        fadd dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_0059766C:
        push ebx
    InteriorHomeLounge_AddSofaInfo_0059766D:
        push edi
    InteriorHomeLounge_AddSofaInfo_0059766E:
        push 3F800000h
    InteriorHomeLounge_AddSofaInfo_00597673:
        push ecx
    InteriorHomeLounge_AddSofaInfo_00597674:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddSofaInfo_00597677:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_00597679:
        push 1
    InteriorHomeLounge_AddSofaInfo_0059767B:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_00597680:
        pop edi
    InteriorHomeLounge_AddSofaInfo_00597681:
        pop esi
    InteriorHomeLounge_AddSofaInfo_00597682:
        pop ebp
    InteriorHomeLounge_AddSofaInfo_00597683:
        pop ebx
    InteriorHomeLounge_AddSofaInfo_00597684:
        ret 0Ch
    InteriorHomeLounge_AddSofaInfo_00597687:
        cmp eax,1
    InteriorHomeLounge_AddSofaInfo_0059768A:
        jne InteriorHomeLounge_AddSofaInfo_005976D7
    InteriorHomeLounge_AddSofaInfo_0059768C:
        fild dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_00597690:
        mov ebx,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_00597694:
        push ebx
    InteriorHomeLounge_AddSofaInfo_00597695:
        push edi
    InteriorHomeLounge_AddSofaInfo_00597696:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddSofaInfo_0059769C:
        fstp dword ptr [esp+20h]
    InteriorHomeLounge_AddSofaInfo_005976A0:
        mov ecx,dword ptr [esp+20h]
    InteriorHomeLounge_AddSofaInfo_005976A4:
        push ecx
    InteriorHomeLounge_AddSofaInfo_005976A5:
        push 3F800000h
    InteriorHomeLounge_AddSofaInfo_005976AA:
        push eax
    InteriorHomeLounge_AddSofaInfo_005976AB:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_005976AD:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_005976B2:
        fld dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_005976B6:
        fadd dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_005976BC:
        push ebx
    InteriorHomeLounge_AddSofaInfo_005976BD:
        push edi
    InteriorHomeLounge_AddSofaInfo_005976BE:
        push ecx
    InteriorHomeLounge_AddSofaInfo_005976BF:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddSofaInfo_005976C2:
        push 3F800000h
    InteriorHomeLounge_AddSofaInfo_005976C7:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_005976C9:
        push 1
    InteriorHomeLounge_AddSofaInfo_005976CB:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_005976D0:
        pop edi
    InteriorHomeLounge_AddSofaInfo_005976D1:
        pop esi
    InteriorHomeLounge_AddSofaInfo_005976D2:
        pop ebp
    InteriorHomeLounge_AddSofaInfo_005976D3:
        pop ebx
    InteriorHomeLounge_AddSofaInfo_005976D4:
        ret 0Ch
    InteriorHomeLounge_AddSofaInfo_005976D7:
        cmp eax,3
    InteriorHomeLounge_AddSofaInfo_005976DA:
        jne InteriorHomeLounge_AddSofaInfo_00597737
    InteriorHomeLounge_AddSofaInfo_005976DC:
        mov edx,dword ptr [esi+14h]
    InteriorHomeLounge_AddSofaInfo_005976DF:
        movzx eax,byte ptr [edx+2]
    InteriorHomeLounge_AddSofaInfo_005976E3:
        mov ebx,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005976E7:
        dec eax
    InteriorHomeLounge_AddSofaInfo_005976E8:
        mov dword ptr [esp+14h],eax
    InteriorHomeLounge_AddSofaInfo_005976EC:
        push ebx
    InteriorHomeLounge_AddSofaInfo_005976ED:
        push edi
    InteriorHomeLounge_AddSofaInfo_005976EE:
        fild dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005976F2:
        fsub dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_005976F8:
        fstp dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_005976FC:
        fild dword ptr [esp+20h]
    InteriorHomeLounge_AddSofaInfo_00597700:
        mov ebp,dword ptr [esp+1Ch]
    InteriorHomeLounge_AddSofaInfo_00597704:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeLounge_AddSofaInfo_0059770A:
        fstp dword ptr [esp+20h]
    InteriorHomeLounge_AddSofaInfo_0059770E:
        mov ecx,dword ptr [esp+20h]
    InteriorHomeLounge_AddSofaInfo_00597712:
        push ecx
    InteriorHomeLounge_AddSofaInfo_00597713:
        push ebp
    InteriorHomeLounge_AddSofaInfo_00597714:
        push 1
    InteriorHomeLounge_AddSofaInfo_00597716:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_00597718:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_0059771D:
        fld dword ptr [esp+18h]
    InteriorHomeLounge_AddSofaInfo_00597721:
        fadd dword ptr ds:[00858624h]
    InteriorHomeLounge_AddSofaInfo_00597727:
        push ebx
    InteriorHomeLounge_AddSofaInfo_00597728:
        push edi
    InteriorHomeLounge_AddSofaInfo_00597729:
        push ecx
    InteriorHomeLounge_AddSofaInfo_0059772A:
        fstp dword ptr [esp]
    InteriorHomeLounge_AddSofaInfo_0059772D:
        push ebp
    InteriorHomeLounge_AddSofaInfo_0059772E:
        mov ecx,esi
    InteriorHomeLounge_AddSofaInfo_00597730:
        push 1
    InteriorHomeLounge_AddSofaInfo_00597732:
        call InteriorHomeCall00591E40
    InteriorHomeLounge_AddSofaInfo_00597737:
        pop edi
    InteriorHomeLounge_AddSofaInfo_00597738:
        pop esi
    InteriorHomeLounge_AddSofaInfo_00597739:
        pop ebp
    InteriorHomeLounge_AddSofaInfo_0059773A:
        pop ebx
    InteriorHomeLounge_AddSofaInfo_0059773B:
        ret 0Ch
    }
}

void Interior_c::Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision) {
    reinterpret_cast<void(__thiscall*)(Interior_c*, int32, int32, CEntity*)>(&InteriorHomeNativeLounge_AddSofaInfo)(this, sitType, offsetX, entityIgnoredCollision);
}

// 0x597740
static __declspec(naked) void InteriorHomeNativeFurnishLounge() {
    __asm {
    InteriorHomeFurnishLounge_00597740:
        sub esp,28h
    InteriorHomeFurnishLounge_00597743:
        push ebx
    InteriorHomeFurnishLounge_00597744:
        push ebp
    InteriorHomeFurnishLounge_00597745:
        push esi
    InteriorHomeFurnishLounge_00597746:
        push edi
    InteriorHomeFurnishLounge_00597747:
        mov esi,ecx
    InteriorHomeFurnishLounge_00597749:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_0059774C:
        movzx ecx,byte ptr [eax+3]
    InteriorHomeFurnishLounge_00597750:
        movzx edx,byte ptr [eax+2]
    InteriorHomeFurnishLounge_00597754:
        movsx eax,byte ptr [eax+5]
    InteriorHomeFurnishLounge_00597758:
        push 0
    InteriorHomeFurnishLounge_0059775A:
        push 7
    InteriorHomeFurnishLounge_0059775C:
        push 1
    InteriorHomeFurnishLounge_0059775E:
        add ecx,edx
    InteriorHomeFurnishLounge_00597760:
        shl ecx,1
    InteriorHomeFurnishLounge_00597762:
        push 2
    InteriorHomeFurnishLounge_00597764:
        push 0
    InteriorHomeFurnishLounge_00597766:
        dec eax
    InteriorHomeFurnishLounge_00597767:
        mov dword ptr [esp+30h],ecx
    InteriorHomeFurnishLounge_0059776B:
        push eax
    InteriorHomeFurnishLounge_0059776C:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059776E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597773:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597776:
        movsx edx,byte ptr [ecx+5]
    InteriorHomeFurnishLounge_0059777A:
        push 0
    InteriorHomeFurnishLounge_0059777C:
        push 2
    InteriorHomeFurnishLounge_0059777E:
        push 1
    InteriorHomeFurnishLounge_00597780:
        push 1
    InteriorHomeFurnishLounge_00597782:
        push 0
    InteriorHomeFurnishLounge_00597784:
        sub edx,2
    InteriorHomeFurnishLounge_00597787:
        push edx
    InteriorHomeFurnishLounge_00597788:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059778A:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_0059778F:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597792:
        movsx ecx,byte ptr [eax+5]
    InteriorHomeFurnishLounge_00597796:
        push 0
    InteriorHomeFurnishLounge_00597798:
        push 2
    InteriorHomeFurnishLounge_0059779A:
        push 1
    InteriorHomeFurnishLounge_0059779C:
        push 1
    InteriorHomeFurnishLounge_0059779E:
        inc ecx
    InteriorHomeFurnishLounge_0059779F:
        push 0
    InteriorHomeFurnishLounge_005977A1:
        push ecx
    InteriorHomeFurnishLounge_005977A2:
        mov ecx,esi
    InteriorHomeFurnishLounge_005977A4:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_005977A9:
        lea edx,[esp+30h]
    InteriorHomeFurnishLounge_005977AD:
        push edx
    InteriorHomeFurnishLounge_005977AE:
        lea eax,[esp+38h]
    InteriorHomeFurnishLounge_005977B2:
        push eax
    InteriorHomeFurnishLounge_005977B3:
        lea ecx,[esp+28h]
    InteriorHomeFurnishLounge_005977B7:
        push ecx
    InteriorHomeFurnishLounge_005977B8:
        lea edx,[esp+30h]
    InteriorHomeFurnishLounge_005977BC:
        push edx
    InteriorHomeFurnishLounge_005977BD:
        lea eax,[esp+20h]
    InteriorHomeFurnishLounge_005977C1:
        push eax
    InteriorHomeFurnishLounge_005977C2:
        push 0
    InteriorHomeFurnishLounge_005977C4:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_005977C6:
        push 1
    InteriorHomeFurnishLounge_005977C8:
        push 0
    InteriorHomeFurnishLounge_005977CA:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_005977CC:
        push 2
    InteriorHomeFurnishLounge_005977CE:
        push 2
    InteriorHomeFurnishLounge_005977D0:
        mov ecx,esi
    InteriorHomeFurnishLounge_005977D2:
        mov dword ptr [esp+48h],0FFFFFFFFh
    InteriorHomeFurnishLounge_005977DA:
        call InteriorHomeCall00593340
    InteriorHomeFurnishLounge_005977DF:
        test eax,eax
    InteriorHomeFurnishLounge_005977E1:
        je InteriorHomeFurnishLounge_00597803
    InteriorHomeFurnishLounge_005977E3:
        mov ecx,dword ptr [esp+20h]
    InteriorHomeFurnishLounge_005977E7:
        mov edx,dword ptr [esp+24h]
    InteriorHomeFurnishLounge_005977EB:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_005977EF:
        push 0
    InteriorHomeFurnishLounge_005977F1:
        push ecx
    InteriorHomeFurnishLounge_005977F2:
        push edx
    InteriorHomeFurnishLounge_005977F3:
        push eax
    InteriorHomeFurnishLounge_005977F4:
        mov ecx,esi
    InteriorHomeFurnishLounge_005977F6:
        call InteriorHomeCall00597240
    InteriorHomeFurnishLounge_005977FB:
        mov ecx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_005977FF:
        mov dword ptr [esp+18h],ecx
    InteriorHomeFurnishLounge_00597803:
        mov edx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_00597807:
        push 1
    InteriorHomeFurnishLounge_00597809:
        push 2
    InteriorHomeFurnishLounge_0059780B:
        push 2
    InteriorHomeFurnishLounge_0059780D:
        push edx
    InteriorHomeFurnishLounge_0059780E:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597810:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_00597815:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597818:
        movzx edi,byte ptr [eax+2]
    InteriorHomeFurnishLounge_0059781C:
        movzx ebx,byte ptr [eax+3]
    InteriorHomeFurnishLounge_00597820:
        sub edi,2
    InteriorHomeFurnishLounge_00597823:
        mov ebp,1
    InteriorHomeFurnishLounge_00597828:
        sub ebx,2
    InteriorHomeFurnishLounge_0059782B:
        cmp edi,ebp
    InteriorHomeFurnishLounge_0059782D:
        jl InteriorHomeFurnishLounge_00597858
    InteriorHomeFurnishLounge_00597830:
        push 0
    InteriorHomeFurnishLounge_00597832:
        push 3
    InteriorHomeFurnishLounge_00597834:
        push 1
    InteriorHomeFurnishLounge_00597836:
        push 1
    InteriorHomeFurnishLounge_00597838:
        push ebx
    InteriorHomeFurnishLounge_00597839:
        push ebp
    InteriorHomeFurnishLounge_0059783A:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059783C:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597841:
        push 0
    InteriorHomeFurnishLounge_00597843:
        push 3
    InteriorHomeFurnishLounge_00597845:
        push 1
    InteriorHomeFurnishLounge_00597847:
        push 1
    InteriorHomeFurnishLounge_00597849:
        push 1
    InteriorHomeFurnishLounge_0059784B:
        push ebp
    InteriorHomeFurnishLounge_0059784C:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059784E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597853:
        inc ebp
    InteriorHomeFurnishLounge_00597854:
        cmp ebp,edi
    InteriorHomeFurnishLounge_00597856:
        jle InteriorHomeFurnishLounge_00597830
    InteriorHomeFurnishLounge_00597858:
        mov ebp,1
    InteriorHomeFurnishLounge_0059785D:
        cmp ebx,ebp
    InteriorHomeFurnishLounge_0059785F:
        jl InteriorHomeFurnishLounge_00597889
    InteriorHomeFurnishLounge_00597861:
        push 0
    InteriorHomeFurnishLounge_00597863:
        push 3
    InteriorHomeFurnishLounge_00597865:
        push 1
    InteriorHomeFurnishLounge_00597867:
        push 1
    InteriorHomeFurnishLounge_00597869:
        push ebp
    InteriorHomeFurnishLounge_0059786A:
        push 1
    InteriorHomeFurnishLounge_0059786C:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059786E:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597873:
        push 0
    InteriorHomeFurnishLounge_00597875:
        push 3
    InteriorHomeFurnishLounge_00597877:
        push 1
    InteriorHomeFurnishLounge_00597879:
        push 1
    InteriorHomeFurnishLounge_0059787B:
        push ebp
    InteriorHomeFurnishLounge_0059787C:
        push edi
    InteriorHomeFurnishLounge_0059787D:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059787F:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597884:
        inc ebp
    InteriorHomeFurnishLounge_00597885:
        cmp ebp,ebx
    InteriorHomeFurnishLounge_00597887:
        jle InteriorHomeFurnishLounge_00597861
    InteriorHomeFurnishLounge_00597889:
        push 0
    InteriorHomeFurnishLounge_0059788B:
        push 0
    InteriorHomeFurnishLounge_0059788D:
        push 1
    InteriorHomeFurnishLounge_0059788F:
        push 1
    InteriorHomeFurnishLounge_00597891:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597893:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishLounge_00597898:
        push 0
    InteriorHomeFurnishLounge_0059789A:
        push 0
    InteriorHomeFurnishLounge_0059789C:
        push ebx
    InteriorHomeFurnishLounge_0059789D:
        push 1
    InteriorHomeFurnishLounge_0059789F:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978A1:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishLounge_005978A6:
        push 0
    InteriorHomeFurnishLounge_005978A8:
        push 0
    InteriorHomeFurnishLounge_005978AA:
        push 1
    InteriorHomeFurnishLounge_005978AC:
        push edi
    InteriorHomeFurnishLounge_005978AD:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978AF:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishLounge_005978B4:
        push 0
    InteriorHomeFurnishLounge_005978B6:
        push 0
    InteriorHomeFurnishLounge_005978B8:
        push ebx
    InteriorHomeFurnishLounge_005978B9:
        push edi
    InteriorHomeFurnishLounge_005978BA:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978BC:
        call InteriorHomeCall00591D20
    InteriorHomeFurnishLounge_005978C1:
        push 0
    InteriorHomeFurnishLounge_005978C3:
        push 2
    InteriorHomeFurnishLounge_005978C5:
        push 2
    InteriorHomeFurnishLounge_005978C7:
        push 0
    InteriorHomeFurnishLounge_005978C9:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978CB:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_005978D0:
        push 0
    InteriorHomeFurnishLounge_005978D2:
        push 2
    InteriorHomeFurnishLounge_005978D4:
        push 2
    InteriorHomeFurnishLounge_005978D6:
        push 2
    InteriorHomeFurnishLounge_005978D8:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978DA:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_005978DF:
        push 0
    InteriorHomeFurnishLounge_005978E1:
        push 2
    InteriorHomeFurnishLounge_005978E3:
        push 2
    InteriorHomeFurnishLounge_005978E5:
        push 1
    InteriorHomeFurnishLounge_005978E7:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978E9:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_005978EE:
        push 0
    InteriorHomeFurnishLounge_005978F0:
        push 2
    InteriorHomeFurnishLounge_005978F2:
        push 2
    InteriorHomeFurnishLounge_005978F4:
        push 3
    InteriorHomeFurnishLounge_005978F6:
        mov ecx,esi
    InteriorHomeFurnishLounge_005978F8:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_005978FD:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597900:
        xor ecx,ecx
    InteriorHomeFurnishLounge_00597902:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishLounge_00597905:
        push ecx
    InteriorHomeFurnishLounge_00597906:
        push 0
    InteriorHomeFurnishLounge_00597908:
        push 2
    InteriorHomeFurnishLounge_0059790A:
        mov ecx,0BAB380h
    InteriorHomeFurnishLounge_0059790F:
        call InteriorHomeCall00591220
    InteriorHomeFurnishLounge_00597914:
        push 0
    InteriorHomeFurnishLounge_00597916:
        push 0
    InteriorHomeFurnishLounge_00597918:
        push 0
    InteriorHomeFurnishLounge_0059791A:
        push 0
    InteriorHomeFurnishLounge_0059791C:
        lea edx,[esp+24h]
    InteriorHomeFurnishLounge_00597920:
        push edx
    InteriorHomeFurnishLounge_00597921:
        lea ecx,[esp+24h]
    InteriorHomeFurnishLounge_00597925:
        push ecx
    InteriorHomeFurnishLounge_00597926:
        push 0
    InteriorHomeFurnishLounge_00597928:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_0059792A:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_0059792C:
        push 1
    InteriorHomeFurnishLounge_0059792E:
        push 0
    InteriorHomeFurnishLounge_00597930:
        movsx edx,al
    InteriorHomeFurnishLounge_00597933:
        push edx
    InteriorHomeFurnishLounge_00597934:
        push 0
    InteriorHomeFurnishLounge_00597936:
        push 2
    InteriorHomeFurnishLounge_00597938:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059793A:
        mov byte ptr [esi+00000791h],al
    InteriorHomeFurnishLounge_00597940:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597945:
        test eax,eax
    InteriorHomeFurnishLounge_00597947:
        je InteriorHomeFurnishLounge_005979AA
    InteriorHomeFurnishLounge_00597949:
        mov ecx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_0059794D:
        push eax
    InteriorHomeFurnishLounge_0059794E:
        mov eax,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597952:
        push eax
    InteriorHomeFurnishLounge_00597953:
        push ecx
    InteriorHomeFurnishLounge_00597954:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597956:
        call InteriorHomeCall005975C0
    InteriorHomeFurnishLounge_0059795B:
        mov edx,dword ptr [esp+14h]
    InteriorHomeFurnishLounge_0059795F:
        mov eax,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_00597963:
        push 0
    InteriorHomeFurnishLounge_00597965:
        push 0
    InteriorHomeFurnishLounge_00597967:
        push 0
    InteriorHomeFurnishLounge_00597969:
        push 0
    InteriorHomeFurnishLounge_0059796B:
        push 0
    InteriorHomeFurnishLounge_0059796D:
        push 0
    InteriorHomeFurnishLounge_0059796F:
        push 2
    InteriorHomeFurnishLounge_00597971:
        push edx
    InteriorHomeFurnishLounge_00597972:
        push eax
    InteriorHomeFurnishLounge_00597973:
        push 1
    InteriorHomeFurnishLounge_00597975:
        push 0
    InteriorHomeFurnishLounge_00597977:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597979:
        push 4
    InteriorHomeFurnishLounge_0059797B:
        push 2
    InteriorHomeFurnishLounge_0059797D:
        mov ecx,esi
    InteriorHomeFurnishLounge_0059797F:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597984:
        test eax,eax
    InteriorHomeFurnishLounge_00597986:
        jne InteriorHomeFurnishLounge_005979AA
    InteriorHomeFurnishLounge_00597988:
        mov ecx,dword ptr [esp+14h]
    InteriorHomeFurnishLounge_0059798C:
        mov edx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_00597990:
        push eax
    InteriorHomeFurnishLounge_00597991:
        push eax
    InteriorHomeFurnishLounge_00597992:
        push eax
    InteriorHomeFurnishLounge_00597993:
        push eax
    InteriorHomeFurnishLounge_00597994:
        push eax
    InteriorHomeFurnishLounge_00597995:
        push eax
    InteriorHomeFurnishLounge_00597996:
        push 3
    InteriorHomeFurnishLounge_00597998:
        push ecx
    InteriorHomeFurnishLounge_00597999:
        push edx
    InteriorHomeFurnishLounge_0059799A:
        push 1
    InteriorHomeFurnishLounge_0059799C:
        push eax
    InteriorHomeFurnishLounge_0059799D:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_0059799F:
        push 4
    InteriorHomeFurnishLounge_005979A1:
        push 2
    InteriorHomeFurnishLounge_005979A3:
        mov ecx,esi
    InteriorHomeFurnishLounge_005979A5:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_005979AA:
        movsx edx,byte ptr [esi+00000791h]
    InteriorHomeFurnishLounge_005979B1:
        push 0
    InteriorHomeFurnishLounge_005979B3:
        push 0
    InteriorHomeFurnishLounge_005979B5:
        push 0
    InteriorHomeFurnishLounge_005979B7:
        push 0
    InteriorHomeFurnishLounge_005979B9:
        lea eax,[esp+24h]
    InteriorHomeFurnishLounge_005979BD:
        push eax
    InteriorHomeFurnishLounge_005979BE:
        lea ecx,[esp+24h]
    InteriorHomeFurnishLounge_005979C2:
        push ecx
    InteriorHomeFurnishLounge_005979C3:
        push 0
    InteriorHomeFurnishLounge_005979C5:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_005979C7:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_005979C9:
        push 1
    InteriorHomeFurnishLounge_005979CB:
        push 0
    InteriorHomeFurnishLounge_005979CD:
        push edx
    InteriorHomeFurnishLounge_005979CE:
        push 1
    InteriorHomeFurnishLounge_005979D0:
        push 2
    InteriorHomeFurnishLounge_005979D2:
        mov ecx,esi
    InteriorHomeFurnishLounge_005979D4:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_005979D9:
        test eax,eax
    InteriorHomeFurnishLounge_005979DB:
        je InteriorHomeFurnishLounge_005979EF
    InteriorHomeFurnishLounge_005979DD:
        mov ecx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_005979E1:
        push eax
    InteriorHomeFurnishLounge_005979E2:
        mov eax,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_005979E6:
        push eax
    InteriorHomeFurnishLounge_005979E7:
        push ecx
    InteriorHomeFurnishLounge_005979E8:
        mov ecx,esi
    InteriorHomeFurnishLounge_005979EA:
        call InteriorHomeCall005974E0
    InteriorHomeFurnishLounge_005979EF:
        movsx ecx,byte ptr [esi+00000791h]
    InteriorHomeFurnishLounge_005979F6:
        push 0
    InteriorHomeFurnishLounge_005979F8:
        push 0
    InteriorHomeFurnishLounge_005979FA:
        push 0
    InteriorHomeFurnishLounge_005979FC:
        push 0
    InteriorHomeFurnishLounge_005979FE:
        lea edx,[esp+24h]
    InteriorHomeFurnishLounge_00597A02:
        push edx
    InteriorHomeFurnishLounge_00597A03:
        lea eax,[esp+24h]
    InteriorHomeFurnishLounge_00597A07:
        push eax
    InteriorHomeFurnishLounge_00597A08:
        push 0
    InteriorHomeFurnishLounge_00597A0A:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597A0C:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597A0E:
        push 1
    InteriorHomeFurnishLounge_00597A10:
        push 0
    InteriorHomeFurnishLounge_00597A12:
        push ecx
    InteriorHomeFurnishLounge_00597A13:
        push 1
    InteriorHomeFurnishLounge_00597A15:
        push 2
    InteriorHomeFurnishLounge_00597A17:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597A19:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597A1E:
        test eax,eax
    InteriorHomeFurnishLounge_00597A20:
        je InteriorHomeFurnishLounge_00597A34
    InteriorHomeFurnishLounge_00597A22:
        mov edx,dword ptr [esp+14h]
    InteriorHomeFurnishLounge_00597A26:
        push eax
    InteriorHomeFurnishLounge_00597A27:
        mov eax,dword ptr [esp+14h]
    InteriorHomeFurnishLounge_00597A2B:
        push edx
    InteriorHomeFurnishLounge_00597A2C:
        push eax
    InteriorHomeFurnishLounge_00597A2D:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597A2F:
        call InteriorHomeCall005974E0
    InteriorHomeFurnishLounge_00597A34:
        cmp dword ptr [esp+1Ch],1Ch
    InteriorHomeFurnishLounge_00597A39:
        jle InteriorHomeFurnishLounge_00597A80
    InteriorHomeFurnishLounge_00597A3B:
        movsx eax,byte ptr [esi+00000791h]
    InteriorHomeFurnishLounge_00597A42:
        push 0
    InteriorHomeFurnishLounge_00597A44:
        push 0
    InteriorHomeFurnishLounge_00597A46:
        push 0
    InteriorHomeFurnishLounge_00597A48:
        push 0
    InteriorHomeFurnishLounge_00597A4A:
        lea ecx,[esp+24h]
    InteriorHomeFurnishLounge_00597A4E:
        push ecx
    InteriorHomeFurnishLounge_00597A4F:
        lea edx,[esp+24h]
    InteriorHomeFurnishLounge_00597A53:
        push edx
    InteriorHomeFurnishLounge_00597A54:
        push 0
    InteriorHomeFurnishLounge_00597A56:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597A58:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597A5A:
        push 1
    InteriorHomeFurnishLounge_00597A5C:
        push 0
    InteriorHomeFurnishLounge_00597A5E:
        push eax
    InteriorHomeFurnishLounge_00597A5F:
        push 1
    InteriorHomeFurnishLounge_00597A61:
        push 2
    InteriorHomeFurnishLounge_00597A63:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597A65:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597A6A:
        test eax,eax
    InteriorHomeFurnishLounge_00597A6C:
        je InteriorHomeFurnishLounge_00597A80
    InteriorHomeFurnishLounge_00597A6E:
        mov ecx,dword ptr [esp+14h]
    InteriorHomeFurnishLounge_00597A72:
        mov edx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_00597A76:
        push eax
    InteriorHomeFurnishLounge_00597A77:
        push ecx
    InteriorHomeFurnishLounge_00597A78:
        push edx
    InteriorHomeFurnishLounge_00597A79:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597A7B:
        call InteriorHomeCall005974E0
    InteriorHomeFurnishLounge_00597A80:
        mov edi,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597A84:
        test edi,edi
    InteriorHomeFurnishLounge_00597A86:
        je InteriorHomeFurnishLounge_00597A97
    InteriorHomeFurnishLounge_00597A88:
        push 0
    InteriorHomeFurnishLounge_00597A8A:
        push 0
    InteriorHomeFurnishLounge_00597A8C:
        push 2
    InteriorHomeFurnishLounge_00597A8E:
        push 0
    InteriorHomeFurnishLounge_00597A90:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597A92:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_00597A97:
        cmp edi,2
    InteriorHomeFurnishLounge_00597A9A:
        je InteriorHomeFurnishLounge_00597AAB
    InteriorHomeFurnishLounge_00597A9C:
        push 0
    InteriorHomeFurnishLounge_00597A9E:
        push 0
    InteriorHomeFurnishLounge_00597AA0:
        push 2
    InteriorHomeFurnishLounge_00597AA2:
        push 2
    InteriorHomeFurnishLounge_00597AA4:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597AA6:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_00597AAB:
        cmp edi,1
    InteriorHomeFurnishLounge_00597AAE:
        je InteriorHomeFurnishLounge_00597ABF
    InteriorHomeFurnishLounge_00597AB0:
        push 0
    InteriorHomeFurnishLounge_00597AB2:
        push 0
    InteriorHomeFurnishLounge_00597AB4:
        push 2
    InteriorHomeFurnishLounge_00597AB6:
        push 1
    InteriorHomeFurnishLounge_00597AB8:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597ABA:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_00597ABF:
        cmp edi,3
    InteriorHomeFurnishLounge_00597AC2:
        je InteriorHomeFurnishLounge_00597AD3
    InteriorHomeFurnishLounge_00597AC4:
        push 0
    InteriorHomeFurnishLounge_00597AC6:
        push 0
    InteriorHomeFurnishLounge_00597AC8:
        push 2
    InteriorHomeFurnishLounge_00597ACA:
        push 3
    InteriorHomeFurnishLounge_00597ACC:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597ACE:
        call InteriorHomeCall005917C0
    InteriorHomeFurnishLounge_00597AD3:
        push 0
    InteriorHomeFurnishLounge_00597AD5:
        push 0
    InteriorHomeFurnishLounge_00597AD7:
        lea eax,[esp+30h]
    InteriorHomeFurnishLounge_00597ADB:
        push eax
    InteriorHomeFurnishLounge_00597ADC:
        lea ecx,[esp+38h]
    InteriorHomeFurnishLounge_00597AE0:
        push ecx
    InteriorHomeFurnishLounge_00597AE1:
        push 0
    InteriorHomeFurnishLounge_00597AE3:
        lea edx,[esp+24h]
    InteriorHomeFurnishLounge_00597AE7:
        push edx
    InteriorHomeFurnishLounge_00597AE8:
        push 0
    InteriorHomeFurnishLounge_00597AEA:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597AEC:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597AEE:
        push 1
    InteriorHomeFurnishLounge_00597AF0:
        push 0
    InteriorHomeFurnishLounge_00597AF2:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597AF4:
        push 6
    InteriorHomeFurnishLounge_00597AF6:
        push 2
    InteriorHomeFurnishLounge_00597AF8:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597AFA:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597AFF:
        test eax,eax
    InteriorHomeFurnishLounge_00597B01:
        je InteriorHomeFurnishLounge_00597B1B
    InteriorHomeFurnishLounge_00597B03:
        mov eax,dword ptr [esp+28h]
    InteriorHomeFurnishLounge_00597B07:
        mov ecx,dword ptr [esp+2Ch]
    InteriorHomeFurnishLounge_00597B0B:
        mov edx,dword ptr [esp+10h]
    InteriorHomeFurnishLounge_00597B0F:
        push 0
    InteriorHomeFurnishLounge_00597B11:
        push eax
    InteriorHomeFurnishLounge_00597B12:
        push ecx
    InteriorHomeFurnishLounge_00597B13:
        push edx
    InteriorHomeFurnishLounge_00597B14:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597B16:
        call InteriorHomeCall00597430
    InteriorHomeFurnishLounge_00597B1B:
        push 0
    InteriorHomeFurnishLounge_00597B1D:
        push 0
    InteriorHomeFurnishLounge_00597B1F:
        push 0
    InteriorHomeFurnishLounge_00597B21:
        push 0
    InteriorHomeFurnishLounge_00597B23:
        push 0
    InteriorHomeFurnishLounge_00597B25:
        push 0
    InteriorHomeFurnishLounge_00597B27:
        push 0
    InteriorHomeFurnishLounge_00597B29:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B2B:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B2D:
        push 1
    InteriorHomeFurnishLounge_00597B2F:
        push 0
    InteriorHomeFurnishLounge_00597B31:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B33:
        push 5
    InteriorHomeFurnishLounge_00597B35:
        push 2
    InteriorHomeFurnishLounge_00597B37:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597B39:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597B3E:
        push 0
    InteriorHomeFurnishLounge_00597B40:
        push 0
    InteriorHomeFurnishLounge_00597B42:
        push 0
    InteriorHomeFurnishLounge_00597B44:
        push 0
    InteriorHomeFurnishLounge_00597B46:
        push 0
    InteriorHomeFurnishLounge_00597B48:
        push 0
    InteriorHomeFurnishLounge_00597B4A:
        push 0
    InteriorHomeFurnishLounge_00597B4C:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B4E:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B50:
        push 1
    InteriorHomeFurnishLounge_00597B52:
        push 0
    InteriorHomeFurnishLounge_00597B54:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B56:
        push 0
    InteriorHomeFurnishLounge_00597B58:
        push 8
    InteriorHomeFurnishLounge_00597B5A:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597B5C:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597B61:
        push 0
    InteriorHomeFurnishLounge_00597B63:
        push 0
    InteriorHomeFurnishLounge_00597B65:
        push 0
    InteriorHomeFurnishLounge_00597B67:
        push 0
    InteriorHomeFurnishLounge_00597B69:
        push 0
    InteriorHomeFurnishLounge_00597B6B:
        push 0
    InteriorHomeFurnishLounge_00597B6D:
        push 0
    InteriorHomeFurnishLounge_00597B6F:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B71:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B73:
        push 1
    InteriorHomeFurnishLounge_00597B75:
        push 0
    InteriorHomeFurnishLounge_00597B77:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597B79:
        push 0
    InteriorHomeFurnishLounge_00597B7B:
        push 8
    InteriorHomeFurnishLounge_00597B7D:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597B7F:
        call InteriorHomeCall00593120
    InteriorHomeFurnishLounge_00597B84:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597B87:
        mov al,byte ptr [eax+1Fh]
    InteriorHomeFurnishLounge_00597B8A:
        cmp al,4Bh
    InteriorHomeFurnishLounge_00597B8C:
        jb InteriorHomeFurnishLounge_00597BB5
    InteriorHomeFurnishLounge_00597B8E:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597B93:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597B98:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597B9C:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597BA0:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597BA6:
        fmul dword ptr ds:[00858BA4h]
    InteriorHomeFurnishLounge_00597BAC:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597BB1:
        mov edi,eax
    InteriorHomeFurnishLounge_00597BB3:
        jmp InteriorHomeFurnishLounge_00597C0D
    InteriorHomeFurnishLounge_00597BB5:
        cmp al,32h
    InteriorHomeFurnishLounge_00597BB7:
        jb InteriorHomeFurnishLounge_00597BE3
    InteriorHomeFurnishLounge_00597BB9:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597BBE:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597BC3:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597BC7:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597BCB:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597BD1:
        fmul dword ptr ds:[00859CE4h]
    InteriorHomeFurnishLounge_00597BD7:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597BDC:
        mov edi,14h
    InteriorHomeFurnishLounge_00597BE1:
        jmp InteriorHomeFurnishLounge_00597C0B
    InteriorHomeFurnishLounge_00597BE3:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597BE8:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597BED:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597BF1:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597BF5:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597BFB:
        fmul dword ptr ds:[00859B60h]
    InteriorHomeFurnishLounge_00597C01:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597C06:
        mov edi,32h
    InteriorHomeFurnishLounge_00597C0B:
        sub edi,eax
    InteriorHomeFurnishLounge_00597C0D:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597C12:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597C17:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597C1B:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597C1F:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597C25:
        fmul dword ptr ds:[00858B34h]
    InteriorHomeFurnishLounge_00597C2B:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597C30:
        cmp eax,edi
    InteriorHomeFurnishLounge_00597C32:
        jge InteriorHomeFurnishLounge_00597CAE
    InteriorHomeFurnishLounge_00597C34:
        lea ecx,[esp+18h]
    InteriorHomeFurnishLounge_00597C38:
        push ecx
    InteriorHomeFurnishLounge_00597C39:
        lea edx,[esp+20h]
    InteriorHomeFurnishLounge_00597C3D:
        push edx
    InteriorHomeFurnishLounge_00597C3E:
        push 2
    InteriorHomeFurnishLounge_00597C40:
        push 2
    InteriorHomeFurnishLounge_00597C42:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597C44:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishLounge_00597C49:
        test al,al
    InteriorHomeFurnishLounge_00597C4B:
        je InteriorHomeFurnishLounge_00597CAE
    InteriorHomeFurnishLounge_00597C4D:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597C50:
        xor ecx,ecx
    InteriorHomeFurnishLounge_00597C52:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishLounge_00597C55:
        push ecx
    InteriorHomeFurnishLounge_00597C56:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597C58:
        push 2
    InteriorHomeFurnishLounge_00597C5A:
        push 8
    InteriorHomeFurnishLounge_00597C5C:
        mov ecx,0BAB380h
    InteriorHomeFurnishLounge_00597C61:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishLounge_00597C66:
        fild dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597C6A:
        push 0
    InteriorHomeFurnishLounge_00597C6C:
        push 3D4CCCCDh
    InteriorHomeFurnishLounge_00597C71:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597C77:
        sub esp,8
    InteriorHomeFurnishLounge_00597C7A:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597C7C:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishLounge_00597C80:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishLounge_00597C84:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597C8A:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597C8D:
        push eax
    InteriorHomeFurnishLounge_00597C8E:
        push 0
    InteriorHomeFurnishLounge_00597C90:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishLounge_00597C95:
        mov edx,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597C99:
        mov eax,dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597C9D:
        push 0
    InteriorHomeFurnishLounge_00597C9F:
        push 2
    InteriorHomeFurnishLounge_00597CA1:
        push 2
    InteriorHomeFurnishLounge_00597CA3:
        push 2
    InteriorHomeFurnishLounge_00597CA5:
        push edx
    InteriorHomeFurnishLounge_00597CA6:
        push eax
    InteriorHomeFurnishLounge_00597CA7:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597CA9:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597CAE:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597CB3:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597CB8:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597CBC:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597CC0:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597CC6:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishLounge_00597CCC:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597CD1:
        cmp eax,edi
    InteriorHomeFurnishLounge_00597CD3:
        jge InteriorHomeFurnishLounge_00597D4F
    InteriorHomeFurnishLounge_00597CD5:
        lea ecx,[esp+1Ch]
    InteriorHomeFurnishLounge_00597CD9:
        push ecx
    InteriorHomeFurnishLounge_00597CDA:
        lea edx,[esp+1Ch]
    InteriorHomeFurnishLounge_00597CDE:
        push edx
    InteriorHomeFurnishLounge_00597CDF:
        push 1
    InteriorHomeFurnishLounge_00597CE1:
        push 1
    InteriorHomeFurnishLounge_00597CE3:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597CE5:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishLounge_00597CEA:
        test al,al
    InteriorHomeFurnishLounge_00597CEC:
        je InteriorHomeFurnishLounge_00597D4F
    InteriorHomeFurnishLounge_00597CEE:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597CF1:
        xor ecx,ecx
    InteriorHomeFurnishLounge_00597CF3:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishLounge_00597CF6:
        push ecx
    InteriorHomeFurnishLounge_00597CF7:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597CF9:
        push 5
    InteriorHomeFurnishLounge_00597CFB:
        push 8
    InteriorHomeFurnishLounge_00597CFD:
        mov ecx,0BAB380h
    InteriorHomeFurnishLounge_00597D02:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishLounge_00597D07:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597D0B:
        push 0
    InteriorHomeFurnishLounge_00597D0D:
        push 3D4CCCCDh
    InteriorHomeFurnishLounge_00597D12:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597D18:
        sub esp,8
    InteriorHomeFurnishLounge_00597D1B:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597D1D:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishLounge_00597D21:
        fild dword ptr [esp+28h]
    InteriorHomeFurnishLounge_00597D25:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597D2B:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597D2E:
        push eax
    InteriorHomeFurnishLounge_00597D2F:
        push 0
    InteriorHomeFurnishLounge_00597D31:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishLounge_00597D36:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597D3A:
        mov eax,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597D3E:
        push 0
    InteriorHomeFurnishLounge_00597D40:
        push 2
    InteriorHomeFurnishLounge_00597D42:
        push 1
    InteriorHomeFurnishLounge_00597D44:
        push 1
    InteriorHomeFurnishLounge_00597D46:
        push edx
    InteriorHomeFurnishLounge_00597D47:
        push eax
    InteriorHomeFurnishLounge_00597D48:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597D4A:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597D4F:
        call InteriorHomeCall00821B1E
    InteriorHomeFurnishLounge_00597D54:
        and eax,0FFFFh
    InteriorHomeFurnishLounge_00597D59:
        mov dword ptr [esp+1Ch],eax
    InteriorHomeFurnishLounge_00597D5D:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597D61:
        fmul dword ptr ds:[00858B14h]
    InteriorHomeFurnishLounge_00597D67:
        fmul dword ptr ds:[00858628h]
    InteriorHomeFurnishLounge_00597D6D:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597D72:
        cmp eax,edi
    InteriorHomeFurnishLounge_00597D74:
        jge InteriorHomeFurnishLounge_00597DF0
    InteriorHomeFurnishLounge_00597D76:
        lea ecx,[esp+1Ch]
    InteriorHomeFurnishLounge_00597D7A:
        push ecx
    InteriorHomeFurnishLounge_00597D7B:
        lea edx,[esp+1Ch]
    InteriorHomeFurnishLounge_00597D7F:
        push edx
    InteriorHomeFurnishLounge_00597D80:
        push 1
    InteriorHomeFurnishLounge_00597D82:
        push 1
    InteriorHomeFurnishLounge_00597D84:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597D86:
        call InteriorHomeCall00591C50
    InteriorHomeFurnishLounge_00597D8B:
        test al,al
    InteriorHomeFurnishLounge_00597D8D:
        je InteriorHomeFurnishLounge_00597DF0
    InteriorHomeFurnishLounge_00597D8F:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597D92:
        xor ecx,ecx
    InteriorHomeFurnishLounge_00597D94:
        mov cl,byte ptr [eax+1Fh]
    InteriorHomeFurnishLounge_00597D97:
        push ecx
    InteriorHomeFurnishLounge_00597D98:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597D9A:
        push 4
    InteriorHomeFurnishLounge_00597D9C:
        push 8
    InteriorHomeFurnishLounge_00597D9E:
        mov ecx,0BAB380h
    InteriorHomeFurnishLounge_00597DA3:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishLounge_00597DA8:
        fild dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597DAC:
        push 0
    InteriorHomeFurnishLounge_00597DAE:
        push 3D4CCCCDh
    InteriorHomeFurnishLounge_00597DB3:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597DB9:
        sub esp,8
    InteriorHomeFurnishLounge_00597DBC:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597DBE:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishLounge_00597DC2:
        fild dword ptr [esp+28h]
    InteriorHomeFurnishLounge_00597DC6:
        fadd dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597DCC:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597DCF:
        push eax
    InteriorHomeFurnishLounge_00597DD0:
        push 0
    InteriorHomeFurnishLounge_00597DD2:
        call InteriorHomeCall005934E0
    InteriorHomeFurnishLounge_00597DD7:
        mov edx,dword ptr [esp+1Ch]
    InteriorHomeFurnishLounge_00597DDB:
        mov eax,dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597DDF:
        push 0
    InteriorHomeFurnishLounge_00597DE1:
        push 2
    InteriorHomeFurnishLounge_00597DE3:
        push 1
    InteriorHomeFurnishLounge_00597DE5:
        push 1
    InteriorHomeFurnishLounge_00597DE7:
        push edx
    InteriorHomeFurnishLounge_00597DE8:
        push eax
    InteriorHomeFurnishLounge_00597DE9:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597DEB:
        call InteriorHomeCall00591700
    InteriorHomeFurnishLounge_00597DF0:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597DF3:
        xor edx,edx
    InteriorHomeFurnishLounge_00597DF5:
        mov dl,byte ptr [ecx+1Fh]
    InteriorHomeFurnishLounge_00597DF8:
        mov ecx,0BAB380h
    InteriorHomeFurnishLounge_00597DFD:
        push edx
    InteriorHomeFurnishLounge_00597DFE:
        push 0FFFFFFFFh
    InteriorHomeFurnishLounge_00597E00:
        push 1
    InteriorHomeFurnishLounge_00597E02:
        push 8
    InteriorHomeFurnishLounge_00597E04:
        call InteriorHomeCall005911E0
    InteriorHomeFurnishLounge_00597E09:
        mov ebx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597E0C:
        movzx edx,byte ptr [ebx+3]
    InteriorHomeFurnishLounge_00597E10:
        push 0
    InteriorHomeFurnishLounge_00597E12:
        mov edi,eax
    InteriorHomeFurnishLounge_00597E14:
        lea eax,[esp+34h]
    InteriorHomeFurnishLounge_00597E18:
        push eax
    InteriorHomeFurnishLounge_00597E19:
        movzx eax,byte ptr [edi+0Dh]
    InteriorHomeFurnishLounge_00597E1D:
        mov dword ptr [esp+24h],edx
    InteriorHomeFurnishLounge_00597E21:
        lea ecx,[esp+3Ch]
    InteriorHomeFurnishLounge_00597E25:
        fild dword ptr [esp+24h]
    InteriorHomeFurnishLounge_00597E29:
        mov dword ptr [esp+24h],eax
    InteriorHomeFurnishLounge_00597E2D:
        push ecx
    InteriorHomeFurnishLounge_00597E2E:
        push 0
    InteriorHomeFurnishLounge_00597E30:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597E36:
        push 0
    InteriorHomeFurnishLounge_00597E38:
        fild dword ptr [esp+30h]
    InteriorHomeFurnishLounge_00597E3C:
        push 0
    InteriorHomeFurnishLounge_00597E3E:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597E44:
        fsubp st(1),st(0)
    InteriorHomeFurnishLounge_00597E46:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597E4B:
        movzx ecx,byte ptr [ebx+2]
    InteriorHomeFurnishLounge_00597E4F:
        movzx edx,byte ptr [edi+0Ch]
    InteriorHomeFurnishLounge_00597E53:
        mov dword ptr [esp+34h],ecx
    InteriorHomeFurnishLounge_00597E57:
        push eax
    InteriorHomeFurnishLounge_00597E58:
        fild dword ptr [esp+38h]
    InteriorHomeFurnishLounge_00597E5C:
        mov dword ptr [esp+38h],edx
    InteriorHomeFurnishLounge_00597E60:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597E66:
        fild dword ptr [esp+38h]
    InteriorHomeFurnishLounge_00597E6A:
        fmul dword ptr ds:[00858B8Ch]
    InteriorHomeFurnishLounge_00597E70:
        fsubp st(1),st(0)
    InteriorHomeFurnishLounge_00597E72:
        call InteriorHomeCall00821B40
    InteriorHomeFurnishLounge_00597E77:
        push eax
    InteriorHomeFurnishLounge_00597E78:
        push edi
    InteriorHomeFurnishLounge_00597E79:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597E7B:
        call InteriorHomeCall00592AA0
    InteriorHomeFurnishLounge_00597E80:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597E83:
        movzx ecx,byte ptr [eax+2]
    InteriorHomeFurnishLounge_00597E87:
        xor edi,edi
    InteriorHomeFurnishLounge_00597E89:
        dec ecx
    InteriorHomeFurnishLounge_00597E8A:
        test ecx,ecx
    InteriorHomeFurnishLounge_00597E8C:
        mov dword ptr [esp+18h],edi
    InteriorHomeFurnishLounge_00597E90:
        jle InteriorHomeFurnishLounge_00597F1E
    InteriorHomeFurnishLounge_00597E96:
        push 0
    InteriorHomeFurnishLounge_00597E98:
        push edi
    InteriorHomeFurnishLounge_00597E99:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597E9B:
        call InteriorHomeCall005918E0
    InteriorHomeFurnishLounge_00597EA0:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597EA3:
        mov ebx,eax
    InteriorHomeFurnishLounge_00597EA5:
        movzx eax,byte ptr [edx+3]
    InteriorHomeFurnishLounge_00597EA9:
        dec eax
    InteriorHomeFurnishLounge_00597EAA:
        push eax
    InteriorHomeFurnishLounge_00597EAB:
        push edi
    InteriorHomeFurnishLounge_00597EAC:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597EAE:
        call InteriorHomeCall005918E0
    InteriorHomeFurnishLounge_00597EB3:
        test ebx,ebx
    InteriorHomeFurnishLounge_00597EB5:
        mov ebp,eax
    InteriorHomeFurnishLounge_00597EB7:
        je InteriorHomeFurnishLounge_00597EBE
    InteriorHomeFurnishLounge_00597EB9:
        cmp ebx,2
    InteriorHomeFurnishLounge_00597EBC:
        jne InteriorHomeFurnishLounge_00597ED5
    InteriorHomeFurnishLounge_00597EBE:
        fild dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597EC2:
        push 0
    InteriorHomeFurnishLounge_00597EC4:
        push 2
    InteriorHomeFurnishLounge_00597EC6:
        push 0
    InteriorHomeFurnishLounge_00597EC8:
        push ecx
    InteriorHomeFurnishLounge_00597EC9:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597ECC:
        push 2
    InteriorHomeFurnishLounge_00597ECE:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597ED0:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishLounge_00597ED5:
        test ebp,ebp
    InteriorHomeFurnishLounge_00597ED7:
        je InteriorHomeFurnishLounge_00597EDE
    InteriorHomeFurnishLounge_00597ED9:
        cmp ebp,2
    InteriorHomeFurnishLounge_00597EDC:
        jne InteriorHomeFurnishLounge_00597F09
    InteriorHomeFurnishLounge_00597EDE:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597EE1:
        movzx edx,byte ptr [ecx+3]
    InteriorHomeFurnishLounge_00597EE5:
        push 0
    InteriorHomeFurnishLounge_00597EE7:
        dec edx
    InteriorHomeFurnishLounge_00597EE8:
        push 0
    InteriorHomeFurnishLounge_00597EEA:
        mov dword ptr [esp+24h],edx
    InteriorHomeFurnishLounge_00597EEE:
        sub esp,8
    InteriorHomeFurnishLounge_00597EF1:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishLounge_00597EF5:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597EF7:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishLounge_00597EFB:
        fild dword ptr [esp+28h]
    InteriorHomeFurnishLounge_00597EFF:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597F02:
        push 2
    InteriorHomeFurnishLounge_00597F04:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishLounge_00597F09:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597F0C:
        movzx ecx,byte ptr [eax+2]
    InteriorHomeFurnishLounge_00597F10:
        inc edi
    InteriorHomeFurnishLounge_00597F11:
        dec ecx
    InteriorHomeFurnishLounge_00597F12:
        cmp edi,ecx
    InteriorHomeFurnishLounge_00597F14:
        mov dword ptr [esp+18h],edi
    InteriorHomeFurnishLounge_00597F18:
        jl InteriorHomeFurnishLounge_00597E96
    InteriorHomeFurnishLounge_00597F1E:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597F21:
        movzx eax,byte ptr [edx+3]
    InteriorHomeFurnishLounge_00597F25:
        mov ebx,1
    InteriorHomeFurnishLounge_00597F2A:
        sub eax,2
    InteriorHomeFurnishLounge_00597F2D:
        cmp eax,ebx
    InteriorHomeFurnishLounge_00597F2F:
        mov dword ptr [esp+18h],ebx
    InteriorHomeFurnishLounge_00597F33:
        jle InteriorHomeFurnishLounge_00597FCA
    InteriorHomeFurnishLounge_00597F39:
        lea esp,[esp+00000000h]
    InteriorHomeFurnishLounge_00597F40:
        push ebx
    InteriorHomeFurnishLounge_00597F41:
        push 0
    InteriorHomeFurnishLounge_00597F43:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597F45:
        call InteriorHomeCall005918E0
    InteriorHomeFurnishLounge_00597F4A:
        mov ecx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597F4D:
        movzx edx,byte ptr [ecx+2]
    InteriorHomeFurnishLounge_00597F51:
        push ebx
    InteriorHomeFurnishLounge_00597F52:
        dec edx
    InteriorHomeFurnishLounge_00597F53:
        push edx
    InteriorHomeFurnishLounge_00597F54:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597F56:
        mov edi,eax
    InteriorHomeFurnishLounge_00597F58:
        call InteriorHomeCall005918E0
    InteriorHomeFurnishLounge_00597F5D:
        test edi,edi
    InteriorHomeFurnishLounge_00597F5F:
        mov ebp,eax
    InteriorHomeFurnishLounge_00597F61:
        je InteriorHomeFurnishLounge_00597F68
    InteriorHomeFurnishLounge_00597F63:
        cmp edi,2
    InteriorHomeFurnishLounge_00597F66:
        jne InteriorHomeFurnishLounge_00597F7F
    InteriorHomeFurnishLounge_00597F68:
        fild dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597F6C:
        push 0
    InteriorHomeFurnishLounge_00597F6E:
        push 1
    InteriorHomeFurnishLounge_00597F70:
        push ecx
    InteriorHomeFurnishLounge_00597F71:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597F74:
        push 0
    InteriorHomeFurnishLounge_00597F76:
        push 2
    InteriorHomeFurnishLounge_00597F78:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597F7A:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishLounge_00597F7F:
        test ebp,ebp
    InteriorHomeFurnishLounge_00597F81:
        je InteriorHomeFurnishLounge_00597F88
    InteriorHomeFurnishLounge_00597F83:
        cmp ebp,2
    InteriorHomeFurnishLounge_00597F86:
        jne InteriorHomeFurnishLounge_00597FB3
    InteriorHomeFurnishLounge_00597F88:
        mov eax,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597F8B:
        fild dword ptr [esp+18h]
    InteriorHomeFurnishLounge_00597F8F:
        movzx ecx,byte ptr [eax+2]
    InteriorHomeFurnishLounge_00597F93:
        push 0
    InteriorHomeFurnishLounge_00597F95:
        push 3
    InteriorHomeFurnishLounge_00597F97:
        sub esp,8
    InteriorHomeFurnishLounge_00597F9A:
        dec ecx
    InteriorHomeFurnishLounge_00597F9B:
        mov dword ptr [esp+2Ch],ecx
    InteriorHomeFurnishLounge_00597F9F:
        fstp dword ptr [esp+4]
    InteriorHomeFurnishLounge_00597FA3:
        mov ecx,esi
    InteriorHomeFurnishLounge_00597FA5:
        fild dword ptr [esp+2Ch]
    InteriorHomeFurnishLounge_00597FA9:
        fstp dword ptr [esp]
    InteriorHomeFurnishLounge_00597FAC:
        push 2
    InteriorHomeFurnishLounge_00597FAE:
        call InteriorHomeCall00591E40
    InteriorHomeFurnishLounge_00597FB3:
        mov edx,dword ptr [esi+14h]
    InteriorHomeFurnishLounge_00597FB6:
        movzx eax,byte ptr [edx+3]
    InteriorHomeFurnishLounge_00597FBA:
        inc ebx
    InteriorHomeFurnishLounge_00597FBB:
        sub eax,2
    InteriorHomeFurnishLounge_00597FBE:
        cmp ebx,eax
    InteriorHomeFurnishLounge_00597FC0:
        mov dword ptr [esp+18h],ebx
    InteriorHomeFurnishLounge_00597FC4:
        jl InteriorHomeFurnishLounge_00597F40
    InteriorHomeFurnishLounge_00597FCA:
        pop edi
    InteriorHomeFurnishLounge_00597FCB:
        pop esi
    InteriorHomeFurnishLounge_00597FCC:
        pop ebp
    InteriorHomeFurnishLounge_00597FCD:
        pop ebx
    InteriorHomeFurnishLounge_00597FCE:
        add esp,28h
    InteriorHomeFurnishLounge_00597FD1:
        ret
    }
}

void Interior_c::FurnishLounge() {
    reinterpret_cast<void(__thiscall*)(Interior_c*)>(&InteriorHomeNativeFurnishLounge)(this);
}

// 0x599210
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Office_PlaceEdgeFillersCalls[] = {0x00407180,0x005911E0,0x00591920,0x00591E40,0x00592AA0,0x00821B1E,0x00821B40};
__declspec(naked) int32 Interior_c::Office_PlaceEdgeFillers(int32 arg0, int32 a2, int32 a3, int32 a6, int32 a7) {
    __asm {
        sub esp,0Ch
        push ebx
        push ebp
        push esi
        push edi
        mov edi,ecx
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+20]
        and eax,0FFFFh
        mov dword ptr [esp+10h],eax
        fild dword ptr [esp+10h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+24]
        mov ebx,dword ptr [esp+2Ch]
        cmp ebx,2
        mov dword ptr [esp+10h],eax
        je InteriorCommerce_Office_PlaceEdgeFillers_00599252
        test ebx,ebx
        mov eax,2
        jne InteriorCommerce_Office_PlaceEdgeFillers_00599257
    InteriorCommerce_Office_PlaceEdgeFillers_00599252:
        mov eax,1
    InteriorCommerce_Office_PlaceEdgeFillers_00599257:
        mov ebp,dword ptr [esp+28h]
        mov esi,dword ptr [esp+24h]
        push 1
        push eax
        push ebp
        push esi
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+8]
        cmp eax,1
        jl InteriorCommerce_Office_PlaceEdgeFillers_00599386
        cmp ebx,2
        mov dword ptr [esp+28h],esi
        mov dword ptr [esp+2Ch],ebp
        jne InteriorCommerce_Office_PlaceEdgeFillers_0059928A
        lea eax,[ebp+1]
        mov dword ptr [esp+2Ch],eax
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005992A9
    InteriorCommerce_Office_PlaceEdgeFillers_0059928A:
        test ebx,ebx
        jne InteriorCommerce_Office_PlaceEdgeFillers_00599297
        lea ecx,[ebp-1]
        mov dword ptr [esp+2Ch],ecx
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005992A9
    InteriorCommerce_Office_PlaceEdgeFillers_00599297:
        cmp ebx,3
        jne InteriorCommerce_Office_PlaceEdgeFillers_0059929F
        dec esi
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005992A5
    InteriorCommerce_Office_PlaceEdgeFillers_0059929F:
        cmp ebx,1
        jne InteriorCommerce_Office_PlaceEdgeFillers_005992A9
        inc esi
    InteriorCommerce_Office_PlaceEdgeFillers_005992A5:
        mov dword ptr [esp+28h],esi
    InteriorCommerce_Office_PlaceEdgeFillers_005992A9:
        mov eax,dword ptr [esp+20h]
        cmp eax,0FFFFFFFFh
        jne InteriorCommerce_Office_PlaceEdgeFillers_00599395
        mov eax,dword ptr [esp+10h]
        cmp eax,5Ah
        jle InteriorCommerce_Office_PlaceEdgeFillers_00599318
        mov cl,byte ptr ds:[00BB3DC8h]
        test cl,cl
        jne InteriorCommerce_Office_PlaceEdgeFillers_00599318
        mov edx,dword ptr [edi+14h]
        xor eax,eax
        mov al,byte ptr [edx+1Fh]
        mov ecx,0BAB380h
        push eax
        push 0FFFFFFFFh
        push 2
        push 1
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+4]
        mov esi,eax
        test esi,esi
        je InteriorCommerce_Office_PlaceEdgeFillers_0059930C
        fild dword ptr [esp+2Ch]
        push 0
        lea ecx,[ebx-2]
        and ecx,3
        push ecx
        sub esp,8
        fstp dword ptr [esp+4]
        mov ecx,edi
        fild dword ptr [esp+38h]
        fstp dword ptr [esp]
        push 7
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+12]
    InteriorCommerce_Office_PlaceEdgeFillers_0059930C:
        mov byte ptr ds:[00BB3DC8h],1
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005993AF
    InteriorCommerce_Office_PlaceEdgeFillers_00599318:
        cmp eax,4Bh
        jle InteriorCommerce_Office_PlaceEdgeFillers_0059933C
        push 5
        push 0
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+0]
        mov edx,dword ptr [edi+14h]
        add esp,8
        mov ebx,eax
        xor eax,eax
        mov al,byte ptr [edx+1Fh]
        push eax
        push 0FFFFFFFFh
        push 0
        push 8
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005993A3
    InteriorCommerce_Office_PlaceEdgeFillers_0059933C:
        cmp eax,19h
        jle InteriorCommerce_Office_PlaceEdgeFillers_00599386
        mov ecx,dword ptr [edi+14h]
        xor edx,edx
        mov dl,byte ptr [ecx+1Fh]
        mov ecx,0BAB380h
        push edx
        push 0FFFFFFFFh
        push 3
        push 1
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+4]
        mov esi,eax
        test esi,esi
        je InteriorCommerce_Office_PlaceEdgeFillers_005993AF
        fild dword ptr [esp+2Ch]
        push 0
        lea eax,[ebx-2]
        and eax,3
        push eax
        sub esp,8
        fstp dword ptr [esp+4]
        mov ecx,edi
        fild dword ptr [esp+38h]
        fstp dword ptr [esp]
        push 7
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+12]
        jmp InteriorCommerce_Office_PlaceEdgeFillers_005993AF
    InteriorCommerce_Office_PlaceEdgeFillers_00599386:
        pop edi
        pop esi
        pop ebp
        mov eax,1
        pop ebx
        add esp,0Ch
        ret 14h
    InteriorCommerce_Office_PlaceEdgeFillers_00599395:
        mov ecx,dword ptr [edi+14h]
        xor edx,edx
        mov dl,byte ptr [ecx+1Fh]
        push edx
        push 0FFFFFFFFh
        push eax
        push 1
    InteriorCommerce_Office_PlaceEdgeFillers_005993A3:
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+4]
        mov esi,eax
    InteriorCommerce_Office_PlaceEdgeFillers_005993AF:
        mov edx,dword ptr [esp+24h]
        push 0
        lea eax,[esp+1Ch]
        push eax
        lea ecx,[esp+1Ch]
        push ecx
        push ebx
        push 1
        push 0
        push ebp
        push edx
        push esi
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceEdgeFillersCalls+16]
        mov eax,dword ptr [esp+14h]
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,0Ch
        ret 14h
    }
}

// 0x5993E0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Office_PlaceDeskCalls[] = {0x005911E0,0x00591700,0x00591E40,0x00592AA0};
__declspec(naked) int32 Interior_c::Office_PlaceDesk(int32 a3, int32 arg4, int32 offsetY, int32 a5, uint8 a6, int32 b) {
    __asm {
        sub esp,0Ch
        mov eax,dword ptr [esp+18h]
        push ebx
        mov ebx,dword ptr [esp+14h]
        push ebp
        mov ebp,dword ptr [esp+1Ch]
        push esi
        lea esi,[eax-2]
        and esi,3
        cmp eax,2
        push edi
        mov edi,ecx
        mov dword ptr [esp+10h],ebx
        mov dword ptr [esp+20h],ebp
        jne InteriorCommerce_Office_PlaceDesk_00599411
        lea eax,[ebp+1]
        mov dword ptr [esp+20h],eax
        jmp InteriorCommerce_Office_PlaceDesk_0059941D
    InteriorCommerce_Office_PlaceDesk_00599411:
        cmp eax,1
        jne InteriorCommerce_Office_PlaceDesk_0059941D
        lea ecx,[ebx+1]
        mov dword ptr [esp+10h],ecx
    InteriorCommerce_Office_PlaceDesk_0059941D:
        mov edx,dword ptr [edi+14h]
        mov ecx,dword ptr [esp+34h]
        xor eax,eax
        mov al,byte ptr [edx+1Fh]
        push eax
        push ecx
        push 0
        push 1
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+0]
        push 0
        lea edx,[esp+18h]
        push edx
        mov edx,dword ptr [esp+28h]
        lea ecx,[esp+20h]
        push ecx
        mov ecx,dword ptr [esp+1Ch]
        push esi
        push 1
        push 0
        push edx
        push ecx
        push eax
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+12]
        test eax,eax
        je InteriorCommerce_Office_PlaceDesk_00599596
        mov eax,dword ptr [esp+28h]
        cmp eax,2
        mov dword ptr [esp+20h],ebx
        mov dword ptr [esp+24h],ebp
        mov dword ptr [esp+10h],ebx
        mov dword ptr [esp+34h],ebp
        jne InteriorCommerce_Office_PlaceDesk_00599484
        inc ebx
        mov dword ptr [esp+20h],ebx
        jmp InteriorCommerce_Office_PlaceDesk_005994BB
    InteriorCommerce_Office_PlaceDesk_00599484:
        test eax,eax
        jne InteriorCommerce_Office_PlaceDesk_0059949A
        inc ebp
        lea edx,[ebx+1]
        mov dword ptr [esp+24h],ebp
        mov dword ptr [esp+34h],ebp
        mov dword ptr [esp+10h],edx
        jmp InteriorCommerce_Office_PlaceDesk_005994BB
    InteriorCommerce_Office_PlaceDesk_0059949A:
        cmp eax,3
        jne InteriorCommerce_Office_PlaceDesk_005994AF
        inc ebp
        inc ebx
        mov dword ptr [esp+24h],ebp
        mov dword ptr [esp+20h],ebx
        mov dword ptr [esp+10h],ebx
        jmp InteriorCommerce_Office_PlaceDesk_005994BB
    InteriorCommerce_Office_PlaceDesk_005994AF:
        cmp eax,1
        jne InteriorCommerce_Office_PlaceDesk_005994BB
        lea eax,[ebp+1]
        mov dword ptr [esp+34h],eax
    InteriorCommerce_Office_PlaceDesk_005994BB:
        mov ecx,dword ptr [edi+14h]
        movsx ax,byte ptr [edi+00000792h]
        xor edx,edx
        mov dl,byte ptr [ecx+1Fh]
        mov ecx,0BAB380h
        push edx
        push eax
        push 1
        push 1
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+0]
        push 1
        lea ecx,[esp+18h]
        push ecx
        lea edx,[esp+20h]
        push edx
        push esi
        push 1
        push 0
        push ebp
        push ebx
        push eax
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+12]
        fild dword ptr [esp+20h]
        cmp esi,2
        fstp dword ptr [esp+20h]
        fild dword ptr [esp+24h]
        fstp dword ptr [esp+28h]
        jne InteriorCommerce_Office_PlaceDesk_0059951B
        fld dword ptr [esp+28h]
        fadd dword ptr ds:[00858B8Ch]
        fstp dword ptr [esp+28h]
        jmp InteriorCommerce_Office_PlaceDesk_00599553
    InteriorCommerce_Office_PlaceDesk_0059951B:
        test esi,esi
        jne InteriorCommerce_Office_PlaceDesk_0059952F
        fld dword ptr [esp+28h]
        fsub dword ptr ds:[00858B8Ch]
        fstp dword ptr [esp+28h]
        jmp InteriorCommerce_Office_PlaceDesk_00599553
    InteriorCommerce_Office_PlaceDesk_0059952F:
        cmp esi,3
        jne InteriorCommerce_Office_PlaceDesk_00599540
        fld dword ptr [esp+20h]
        fsub dword ptr ds:[00858B8Ch]
        jmp InteriorCommerce_Office_PlaceDesk_0059954F
    InteriorCommerce_Office_PlaceDesk_00599540:
        cmp esi,1
        jne InteriorCommerce_Office_PlaceDesk_00599553
        fld dword ptr [esp+20h]
        fadd dword ptr ds:[00858B8Ch]
    InteriorCommerce_Office_PlaceDesk_0059954F:
        fstp dword ptr [esp+20h]
    InteriorCommerce_Office_PlaceDesk_00599553:
        mov ecx,dword ptr [esp+20h]
        push eax
        mov eax,dword ptr [esp+2Ch]
        add esi,0FFFFFFFEh
        and esi,3
        push esi
        push eax
        push ecx
        push 6
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+8]
        mov edx,dword ptr [esp+34h]
        mov eax,dword ptr [esp+10h]
        push 1
        push 2
        push 1
        push 1
        push edx
        push eax
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceDeskCalls+4]
        pop edi
        pop esi
        pop ebp
        mov eax,2
        pop ebx
        add esp,0Ch
        ret 18h
    InteriorCommerce_Office_PlaceDesk_00599596:
        pop edi
        pop esi
        pop ebp
        mov eax,1
        pop ebx
        add esp,0Ch
        ret 18h
    }
}

// 0x5995B0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Office_PlaceEdgeDesksCalls[] = {0x00591920,0x005993E0,0x00821B1E,0x00821B40};
__declspec(naked) int32 Interior_c::Office_PlaceEdgeDesks(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    __asm {
        sub esp,8
        push ebx
        push ebp
        push edi
        mov edi,ecx
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+8]
        and eax,0FFFFh
        mov dword ptr [esp+0Ch],eax
        fild dword ptr [esp+0Ch]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+12]
        mov dword ptr [esp+10h],eax
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+8]
        and eax,0FFFFh
        mov dword ptr [esp+0Ch],eax
        fild dword ptr [esp+0Ch]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[0085A708h]
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+12]
        mov ebx,1Eh
        sub ebx,eax
        mov eax,dword ptr [esp+24h]
        cmp eax,2
        je InteriorCommerce_Office_PlaceEdgeDesks_0059961B
        test eax,eax
        mov eax,2
        jne InteriorCommerce_Office_PlaceEdgeDesks_00599620
    InteriorCommerce_Office_PlaceEdgeDesks_0059961B:
        mov eax,1
    InteriorCommerce_Office_PlaceEdgeDesks_00599620:
        mov ebp,dword ptr [esp+20h]
        push 1
        push eax
        mov eax,dword ptr [esp+24h]
        push ebp
        push eax
        mov ecx,edi
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+0]
        cmp eax,1
        jg InteriorCommerce_Office_PlaceEdgeDesks_00599647
        pop edi
        pop ebp
        mov eax,1
        pop ebx
        add esp,8
        ret 14h
    InteriorCommerce_Office_PlaceEdgeDesks_00599647:
        cdq
        push esi
        sub eax,edx
        mov esi,eax
        sar esi,1
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+8]
        and eax,0FFFFh
        mov dword ptr [esp+24h],eax
        fild dword ptr [esp+24h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858B18h]
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+12]
        mov ecx,2
        sub ecx,eax
        cmp esi,ecx
        jge InteriorCommerce_Office_PlaceEdgeDesks_00599681
        mov ecx,esi
        jmp InteriorCommerce_Office_PlaceEdgeDesks_005996AB
    InteriorCommerce_Office_PlaceEdgeDesks_00599681:
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+8]
        and eax,0FFFFh
        mov dword ptr [esp+24h],eax
        fild dword ptr [esp+24h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858B18h]
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+12]
        mov ecx,2
        sub ecx,eax
    InteriorCommerce_Office_PlaceEdgeDesks_005996AB:
        mov eax,dword ptr [esp+14h]
        xor esi,esi
        cmp eax,ebx
        jle InteriorCommerce_Office_PlaceEdgeDesks_005996C4
        pop esi
        pop edi
        pop ebp
        mov eax,1
        pop ebx
        add esp,8
        ret 14h
    InteriorCommerce_Office_PlaceEdgeDesks_005996C4:
        test ecx,ecx
        jle InteriorCommerce_Office_PlaceEdgeDesks_0059975D
        mov ebx,dword ptr [esp+28h]
        add ebx,0FFFFFFFEh
        and ebx,3
        mov dword ptr [esp+28h],ecx
        lea ebx,[ebx+00000000h]
    InteriorCommerce_Office_PlaceEdgeDesks_005996E0:
        mov eax,dword ptr [esp+2Ch]
        test eax,eax
        jne InteriorCommerce_Office_PlaceEdgeDesks_005996FA
        movsx edx,byte ptr [edi+00000791h]
        push edx
        push eax
        push 46h
        push ebx
        lea eax,[ebp-1]
        push eax
        jmp InteriorCommerce_Office_PlaceEdgeDesks_00599746
    InteriorCommerce_Office_PlaceEdgeDesks_005996FA:
        cmp eax,3
        jne InteriorCommerce_Office_PlaceEdgeDesks_00599717
        movsx eax,byte ptr [edi+00000791h]
        mov edx,dword ptr [esp+20h]
        push eax
        push 0
        push 46h
        push ebx
        lea ecx,[esi+ebp]
        push ecx
        dec edx
        jmp InteriorCommerce_Office_PlaceEdgeDesks_0059974D
    InteriorCommerce_Office_PlaceEdgeDesks_00599717:
        cmp eax,1
        jne InteriorCommerce_Office_PlaceEdgeDesks_00599733
        movsx eax,byte ptr [edi+00000791h]
        mov edx,dword ptr [esp+20h]
        push eax
        push 0
        push 46h
        push ebx
        lea ecx,[esi+ebp]
        push ecx
        jmp InteriorCommerce_Office_PlaceEdgeDesks_0059974D
    InteriorCommerce_Office_PlaceEdgeDesks_00599733:
        cmp eax,2
        jne InteriorCommerce_Office_PlaceEdgeDesks_00599757
        movsx eax,byte ptr [edi+00000791h]
        push eax
        push 0
        push 46h
        push ebx
        push ebp
    InteriorCommerce_Office_PlaceEdgeDesks_00599746:
        mov ecx,dword ptr [esp+34h]
        lea edx,[esi+ecx]
    InteriorCommerce_Office_PlaceEdgeDesks_0059974D:
        mov ecx,edi
        push edx
        call dword ptr [InteriorCommerce_Office_PlaceEdgeDesksCalls+4]
        add esi,eax
    InteriorCommerce_Office_PlaceEdgeDesks_00599757:
        dec dword ptr [esp+28h]
        jne InteriorCommerce_Office_PlaceEdgeDesks_005996E0
    InteriorCommerce_Office_PlaceEdgeDesks_0059975D:
        lea eax,[esi+1]
        pop esi
        pop edi
        pop ebp
        pop ebx
        add esp,8
        ret 14h
    }
}

// 0x599770
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Office_FurnishEdgesCalls[] = {0x00591700,0x00591D20,0x00599210,0x005995B0};
__declspec(naked) void Interior_c::Office_FurnishEdges() {
    __asm {
        push ebx
        push ebp
        push esi
        mov esi,ecx
        mov eax,dword ptr [esi+14h]
        movzx ebx,byte ptr [eax+3]
        push edi
        movzx edi,byte ptr [eax+2]
        sub edi,3
        mov ebp,2
        sub ebx,3
        cmp edi,ebp
        jl InteriorCommerce_Office_FurnishEdges_005997B8
    InteriorCommerce_Office_FurnishEdges_00599790:
        push 0
        push 3
        push 1
        push 1
        push ebx
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+0]
        push 0
        push 3
        push 1
        push 1
        push 2
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+0]
        inc ebp
        cmp ebp,edi
        jle InteriorCommerce_Office_FurnishEdges_00599790
    InteriorCommerce_Office_FurnishEdges_005997B8:
        mov ebp,2
        cmp ebx,ebp
        jl InteriorCommerce_Office_FurnishEdges_005997E9
    InteriorCommerce_Office_FurnishEdges_005997C1:
        push 0
        push 3
        push 1
        push 1
        push ebp
        push 2
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+0]
        push 0
        push 3
        push 1
        push 1
        push ebp
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+0]
        inc ebp
        cmp ebp,ebx
        jle InteriorCommerce_Office_FurnishEdges_005997C1
    InteriorCommerce_Office_FurnishEdges_005997E9:
        push 3F000000h
        push 3F000000h
        push 2
        push 2
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+4]
        push 0BF000000h
        push 3F000000h
        push ebx
        push 2
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+4]
        push 3F000000h
        push 0BF000000h
        push 2
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+4]
        push 0BF000000h
        push 0BF000000h
        push ebx
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+4]
        mov eax,dword ptr [esi+14h]
        movzx ebx,byte ptr [eax+2]
        movzx edi,byte ptr [eax+3]
        movsx eax,byte ptr [eax+5]
        push 0
        push 7
        push 2
        push 4
        push 0
        sub eax,2
        dec ebx
        push eax
        mov ecx,esi
        dec edi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+0]
        mov ebp,1
        cmp ebx,ebp
        jle InteriorCommerce_Office_FurnishEdges_0059987E
    InteriorCommerce_Office_FurnishEdges_00599868:
        push 2
        push 2
        push 0
        push ebp
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+12]
        add ebp,eax
        cmp ebp,ebx
        jl InteriorCommerce_Office_FurnishEdges_00599868
    InteriorCommerce_Office_FurnishEdges_0059987E:
        mov ebp,1
        cmp ebx,ebp
        jle InteriorCommerce_Office_FurnishEdges_0059989C
    InteriorCommerce_Office_FurnishEdges_00599887:
        push 0
        push 0
        push edi
        push ebp
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+12]
        add ebp,eax
        cmp ebp,ebx
        jl InteriorCommerce_Office_FurnishEdges_00599887
    InteriorCommerce_Office_FurnishEdges_0059989C:
        dec edi
        mov ebp,1
        cmp edi,ebp
        jl InteriorCommerce_Office_FurnishEdges_005998BC
    InteriorCommerce_Office_FurnishEdges_005998A6:
        push 1
        push 1
        push ebp
        push 0
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+12]
        add ebp,eax
        cmp ebp,edi
        jle InteriorCommerce_Office_FurnishEdges_005998A6
    InteriorCommerce_Office_FurnishEdges_005998BC:
        mov ebp,1
        cmp edi,ebp
        jl InteriorCommerce_Office_FurnishEdges_005998DA
    InteriorCommerce_Office_FurnishEdges_005998C5:
        push 3
        push 3
        push ebp
        push ebx
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+12]
        add ebp,eax
        cmp ebp,edi
        jle InteriorCommerce_Office_FurnishEdges_005998C5
    InteriorCommerce_Office_FurnishEdges_005998DA:
        mov ebp,1
        inc edi
        cmp ebx,ebp
        jle InteriorCommerce_Office_FurnishEdges_005998FA
    InteriorCommerce_Office_FurnishEdges_005998E4:
        push 2
        push 2
        push 0
        push ebp
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+8]
        add ebp,eax
        cmp ebp,ebx
        jl InteriorCommerce_Office_FurnishEdges_005998E4
    InteriorCommerce_Office_FurnishEdges_005998FA:
        mov ebp,1
        cmp ebx,ebp
        jle InteriorCommerce_Office_FurnishEdges_00599918
    InteriorCommerce_Office_FurnishEdges_00599903:
        push 0
        push 0
        push edi
        push ebp
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+8]
        add ebp,eax
        cmp ebp,ebx
        jl InteriorCommerce_Office_FurnishEdges_00599903
    InteriorCommerce_Office_FurnishEdges_00599918:
        dec edi
        mov ebp,1
        cmp edi,ebp
        jl InteriorCommerce_Office_FurnishEdges_00599938
    InteriorCommerce_Office_FurnishEdges_00599922:
        push 1
        push 1
        push ebp
        push 0
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+8]
        add ebp,eax
        cmp ebp,edi
        jle InteriorCommerce_Office_FurnishEdges_00599922
    InteriorCommerce_Office_FurnishEdges_00599938:
        mov ebp,1
        cmp edi,ebp
        jl InteriorCommerce_Office_FurnishEdges_00599956
    InteriorCommerce_Office_FurnishEdges_00599941:
        push 3
        push 3
        push ebp
        push ebx
        push 0FFFFFFFFh
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_FurnishEdgesCalls+8]
        add ebp,eax
        cmp ebp,edi
        jle InteriorCommerce_Office_FurnishEdges_00599941
    InteriorCommerce_Office_FurnishEdges_00599956:
        pop edi
        pop esi
        pop ebp
        pop ebx
        ret
    }
}

// 0x599960
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Office_PlaceDeskQuadCalls[] = {0x00591700,0x005993E0};
__declspec(naked) int32 Interior_c::Office_PlaceDeskQuad(int32 a2, int32 a3, int32 a4, int32 a5) {
    __asm {
        mov eax,dword ptr [esp+10h]
        push ebx
        push ebp
        mov ebp,dword ptr [esp+10h]
        push esi
        push edi
        mov edi,dword ptr [esp+1Ch]
        push eax
        push 0
        push 46h
        push 2
        lea ebx,[edi-2]
        push ebx
        push ebp
        mov esi,ecx
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+4]
        mov ecx,dword ptr [esp+20h]
        push ecx
        push 0
        push 46h
        push 0
        push edi
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+4]
        mov edx,dword ptr [esp+20h]
        push edx
        push 0
        push 46h
        push 0
        lea eax,[ebp-2]
        push edi
        push eax
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+4]
        mov eax,dword ptr [esp+20h]
        push eax
        push 0
        push 46h
        push 2
        push ebx
        lea eax,[ebp-2]
        push eax
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+4]
        push 0
        push 3
        push 1
        push 6
        lea ecx,[edi-3]
        push ecx
        lea eax,[ebp-3]
        push eax
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+0]
        push 0
        push 3
        push 1
        push 6
        add edi,2
        push edi
        lea edi,[ebp-3]
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+0]
        push 0
        push 3
        push 4
        push 1
        push ebx
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+0]
        push 0
        push 3
        push 4
        push 1
        push ebx
        add ebp,2
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Office_PlaceDeskQuadCalls+0]
        pop edi
        pop esi
        pop ebp
        mov eax,6
        pop ebx
        ret 10h
    }
}

// 0x599BB0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_Place3PieceUnitCalls[] = {0x005911E0,0x00592AA0};
__declspec(naked) int8 Interior_c::Shop_Place3PieceUnit(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    __asm {
        sub esp,8
        push ebx
        mov ebx,dword ptr [esp+1Ch]
        cmp ebx,2
        push ebp
        push esi
        push edi
        mov esi,ecx
        je InteriorCommerce_Shop_Place3PieceUnit_00599C09
        cmp ebx,3
        je InteriorCommerce_Shop_Place3PieceUnit_00599C09
        mov eax,dword ptr [esi+14h]
        mov edi,dword ptr [esp+1Ch]
        movsx edx,byte ptr [esi+00000790h]
        xor ecx,ecx
        mov cl,byte ptr [eax+1Fh]
        push ecx
        push 0FFFFFFFFh
        push edi
        push edx
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+0]
        mov ebp,eax
        mov eax,dword ptr [esi+14h]
        xor ecx,ecx
        mov cl,byte ptr [eax+1Fh]
        xor edx,edx
        mov dx,word ptr [ebp+0Ah]
        lea eax,[edi+1]
        push ecx
        movsx ecx,byte ptr [esi+00000790h]
        push edx
        push eax
        push ecx
        jmp InteriorCommerce_Shop_Place3PieceUnit_00599C49
    InteriorCommerce_Shop_Place3PieceUnit_00599C09:
        mov edx,dword ptr [esi+14h]
        mov edi,dword ptr [esp+1Ch]
        xor eax,eax
        mov al,byte ptr [edx+1Fh]
        movsx edx,byte ptr [esi+00000790h]
        lea ecx,[edi+1]
        push eax
        push 0FFFFFFFFh
        push ecx
        push edx
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+0]
        mov ebp,eax
        mov eax,dword ptr [esi+14h]
        xor ecx,ecx
        mov cl,byte ptr [eax+1Fh]
        movsx eax,byte ptr [esi+00000790h]
        xor edx,edx
        mov dx,word ptr [ebp+0Ah]
        push ecx
        push edx
        push edi
        push eax
    InteriorCommerce_Shop_Place3PieceUnit_00599C49:
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+0]
        mov ecx,dword ptr [esi+14h]
        xor edx,edx
        mov dl,byte ptr [ecx+1Fh]
        movsx ecx,byte ptr [esi+00000790h]
        mov dword ptr [esp+10h],eax
        xor eax,eax
        mov ax,word ptr [ebp+0Ah]
        push edx
        add edi,2
        push eax
        push edi
        push ecx
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+0]
        cmp ebx,2
        mov dword ptr [esp+14h],eax
        je InteriorCommerce_Shop_Place3PieceUnit_00599D29
        test ebx,ebx
        je InteriorCommerce_Shop_Place3PieceUnit_00599D29
        mov edi,dword ptr [esp+24h]
        mov ecx,dword ptr [esp+20h]
        push 0
        lea edx,[esp+20h]
        push edx
        lea eax,[esp+30h]
        push eax
        push ebx
        push 1
        push 0
        push edi
        push ecx
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        mov eax,dword ptr [esp+2Ch]
        mov edx,dword ptr [esp+1Ch]
        add eax,0FFFFFFFEh
        add edi,edx
        test eax,eax
        jle InteriorCommerce_Shop_Place3PieceUnit_00599CFA
        mov ebp,eax
        jmp InteriorCommerce_Shop_Place3PieceUnit_00599CD0
        lea ebx,[ebx+00000000h]
    InteriorCommerce_Shop_Place3PieceUnit_00599CD0:
        mov ecx,dword ptr [esp+20h]
        push 0
        lea edx,[esp+20h]
        push edx
        mov edx,dword ptr [esp+1Ch]
        lea eax,[esp+30h]
        push eax
        push ebx
        push 1
        push 0
        push edi
        push ecx
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        add edi,dword ptr [esp+1Ch]
        dec ebp
        jne InteriorCommerce_Shop_Place3PieceUnit_00599CD0
    InteriorCommerce_Shop_Place3PieceUnit_00599CFA:
        mov edx,dword ptr [esp+20h]
        push 0
        lea eax,[esp+20h]
        push eax
        mov eax,dword ptr [esp+18h]
        lea ecx,[esp+30h]
        push ecx
        push ebx
        push 1
        push 0
        push edi
        push edx
        push eax
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        pop edi
        pop esi
        pop ebp
        mov al,1
        pop ebx
        add esp,8
        ret 14h
    InteriorCommerce_Shop_Place3PieceUnit_00599D29:
        mov eax,dword ptr [esp+24h]
        mov edi,dword ptr [esp+20h]
        push 0
        lea ecx,[esp+20h]
        push ecx
        lea edx,[esp+30h]
        push edx
        push ebx
        push 1
        push 0
        push eax
        push edi
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        mov eax,dword ptr [esp+2Ch]
        mov edx,dword ptr [esp+28h]
        add eax,0FFFFFFFEh
        add edi,edx
        test eax,eax
        jle InteriorCommerce_Shop_Place3PieceUnit_00599D8A
        mov ebp,eax
    InteriorCommerce_Shop_Place3PieceUnit_00599D60:
        mov eax,dword ptr [esp+24h]
        push 0
        lea ecx,[esp+20h]
        push ecx
        mov ecx,dword ptr [esp+1Ch]
        lea edx,[esp+30h]
        push edx
        push ebx
        push 1
        push 0
        push eax
        push edi
        push ecx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        add edi,dword ptr [esp+28h]
        dec ebp
        jne InteriorCommerce_Shop_Place3PieceUnit_00599D60
    InteriorCommerce_Shop_Place3PieceUnit_00599D8A:
        mov ecx,dword ptr [esp+24h]
        push 0
        lea edx,[esp+20h]
        push edx
        mov edx,dword ptr [esp+18h]
        lea eax,[esp+30h]
        push eax
        push ebx
        push 1
        push 0
        push ecx
        push edi
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_Place3PieceUnitCalls+4]
        pop edi
        pop esi
        pop ebp
        mov al,1
        pop ebx
        add esp,8
        ret 14h
    }
}

// 0x599DC0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_PlaceEdgeUnitsCalls[] = {0x00591920,0x00599BB0,0x00821B1E,0x00821B40};
__declspec(naked) int32 Interior_c::Shop_PlaceEdgeUnits(int32 a2, int32 a3, int32 a4, int32 a5) {
    __asm {
        mov eax,dword ptr [esp+10h]
        cmp eax,2
        push ebx
        push ebp
        push edi
        mov edi,ecx
        je InteriorCommerce_Shop_PlaceEdgeUnits_00599DD7
        test eax,eax
        mov eax,2
        jne InteriorCommerce_Shop_PlaceEdgeUnits_00599DDC
    InteriorCommerce_Shop_PlaceEdgeUnits_00599DD7:
        mov eax,1
    InteriorCommerce_Shop_PlaceEdgeUnits_00599DDC:
        mov ebx,dword ptr [esp+18h]
        mov ebp,dword ptr [esp+14h]
        push 1
        push eax
        push ebx
        push ebp
        mov ecx,edi
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+0]
        cmp eax,1
        mov dword ptr [esp+14h],eax
        jg InteriorCommerce_Shop_PlaceEdgeUnits_00599E04
        pop edi
        pop ebp
        mov eax,1
        pop ebx
        ret 10h
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E04:
        push esi
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+8]
        and eax,0FFFFh
        mov dword ptr [esp+1Ch],eax
        fild dword ptr [esp+1Ch]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00859024h]
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+12]
        mov esi,2
        sub esi,eax
        mov eax,dword ptr [esp+18h]
        cmp eax,3
        jne InteriorCommerce_Shop_PlaceEdgeUnits_00599E3C
        mov esi,eax
        jmp InteriorCommerce_Shop_PlaceEdgeUnits_00599E4C
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E3C:
        mov ecx,eax
        sub ecx,esi
        jns InteriorCommerce_Shop_PlaceEdgeUnits_00599E46
        mov esi,eax
        jmp InteriorCommerce_Shop_PlaceEdgeUnits_00599E4C
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E46:
        cmp ecx,1
        jne InteriorCommerce_Shop_PlaceEdgeUnits_00599E4C
        dec esi
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E4C:
        mov eax,dword ptr [esp+14h]
        cmp eax,0FFFFFFFFh
        push esi
        jne InteriorCommerce_Shop_PlaceEdgeUnits_00599ECC
        mov eax,dword ptr ds:[00BB3DE4h]
        cmp eax,32h
        jle InteriorCommerce_Shop_PlaceEdgeUnits_00599E79
        mov eax,dword ptr [esp+24h]
        push eax
        push ebx
        push ebp
        push 0
        mov ecx,edi
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+4]
        mov eax,esi
        pop esi
        pop edi
        pop ebp
        pop ebx
        ret 10h
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E79:
        cmp eax,19h
        jle InteriorCommerce_Shop_PlaceEdgeUnits_00599E97
        mov ecx,dword ptr [esp+24h]
        push ecx
        push ebx
        push ebp
        push 3
        mov ecx,edi
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+4]
        mov eax,esi
        pop esi
        pop edi
        pop ebp
        pop ebx
        ret 10h
    InteriorCommerce_Shop_PlaceEdgeUnits_00599E97:
        cmp eax,0Ah
        mov ecx,edi
        jle InteriorCommerce_Shop_PlaceEdgeUnits_00599EB5
        mov edx,dword ptr [esp+24h]
        push edx
        push ebx
        push ebp
        push 6
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+4]
        mov eax,esi
        pop esi
        pop edi
        pop ebp
        pop ebx
        ret 10h
    InteriorCommerce_Shop_PlaceEdgeUnits_00599EB5:
        mov eax,dword ptr [esp+24h]
        push eax
        push ebx
        push ebp
        push 9
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+4]
        mov eax,esi
        pop esi
        pop edi
        pop ebp
        pop ebx
        ret 10h
    InteriorCommerce_Shop_PlaceEdgeUnits_00599ECC:
        mov ecx,dword ptr [esp+24h]
        push ecx
        push ebx
        push ebp
        push eax
        mov ecx,edi
        call dword ptr [InteriorCommerce_Shop_PlaceEdgeUnitsCalls+4]
        mov eax,esi
        pop esi
        pop edi
        pop ebp
        pop ebx
        ret 10h
    }
}

// 0x599EF0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_PlaceCounterCalls[] = {0x005911E0,0x00591700,0x00592AA0,0x00821B1E,0x00821B40};
__declspec(naked) int32 Interior_c::Shop_PlaceCounter(uint8 a2) {
    __asm {
        sub esp,0Ch
        push ebx
        push ebp
        push esi
        mov esi,ecx
        mov eax,dword ptr [esi+14h]
        xor ecx,ecx
        mov cl,byte ptr [eax+1Fh]
        push edi
        push ecx
        push 0FFFFFFFFh
        push 0Ch
        push 0
        mov ecx,0BAB380h
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+0]
        mov edx,dword ptr [esi+14h]
        mov ebx,eax
        xor eax,eax
        mov al,byte ptr [edx+1Fh]
        mov ecx,0BAB380h
        push eax
        push 0FFFFFFFFh
        push 0Dh
        push 0
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+0]
        mov ebp,eax
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+12]
        and eax,0FFFFh
        mov dword ptr [esp+18h],eax
        fild dword ptr [esp+18h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858B90h]
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+16]
        mov dword ptr [esp+18h],eax
        mov al,byte ptr [esp+20h]
        test al,al
        push 0
        je InteriorCommerce_Shop_PlaceCounter_00599FB4
        mov ecx,dword ptr [esi+14h]
        movsx edi,byte ptr [ecx+5]
        lea edx,[esp+18h]
        push edx
        lea eax,[esp+18h]
        push eax
        push 0
        push 1
        push 0
        push 1
        sub edi,5
        push edi
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+8]
        mov ecx,dword ptr [esp+10h]
        push 0
        push 2
        push 1
        inc ecx
        push ecx
        push 0
        lea edx,[edi-1]
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+4]
        push 1
        lea eax,[esp+18h]
        push eax
        mov eax,dword ptr [esi+14h]
        lea ecx,[esp+18h]
        push ecx
        movsx ecx,byte ptr [eax+5]
        inc ecx
        jmp InteriorCommerce_Shop_PlaceCounter_0059A005
    InteriorCommerce_Shop_PlaceCounter_00599FB4:
        mov edx,dword ptr [esi+14h]
        movsx edi,byte ptr [edx+5]
        lea eax,[esp+18h]
        push eax
        lea ecx,[esp+18h]
        push ecx
        push 0
        push 1
        push 0
        push 1
        add edi,2
        push edi
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+8]
        mov edx,dword ptr [esp+10h]
        push 0
        push 2
        push 1
        inc edx
        push edx
        push 0
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+4]
        push 1
        lea eax,[esp+18h]
        push eax
        mov eax,dword ptr [esi+14h]
        lea ecx,[esp+18h]
        push ecx
        movsx ecx,byte ptr [eax+5]
        sub ecx,2
    InteriorCommerce_Shop_PlaceCounter_0059A005:
        mov edx,dword ptr [esp+24h]
        push edx
        push 1
        push 0
        push 0
        push ecx
        mov ecx,esi
        push ebp
        call dword ptr [InteriorCommerce_Shop_PlaceCounterCalls+8]
        lea eax,[edi+2]
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,0Ch
        ret 4
    }
}

// 0x59A030
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_PlaceFixedUnitsCalls[] = {0x00591700,0x00591E40,0x00599EF0,0x00821B1E};
__declspec(naked) void Interior_c::Shop_PlaceFixedUnits() {
    __asm {
        push ecx
        push esi
        mov esi,ecx
        mov eax,dword ptr [esi+14h]
        mov al,byte ptr [eax+5]
        cmp al,0FFh
        je InteriorCommerce_Shop_PlaceFixedUnits_0059A0D4
        push 0
        push 7
        push 1
        movsx ecx,al
        push 2
        dec ecx
        push 0
        push ecx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceFixedUnitsCalls+0]
        mov eax,dword ptr [esi+14h]
        movsx ecx,byte ptr [eax+5]
        movzx eax,byte ptr [eax+2]
        sub eax,ecx
        sub eax,2
        cmp eax,6
        lea edx,[ecx-2]
        jl InteriorCommerce_Shop_PlaceFixedUnits_0059A091
        cmp edx,6
        jl InteriorCommerce_Shop_PlaceFixedUnits_0059A08D
        call dword ptr [InteriorCommerce_Shop_PlaceFixedUnitsCalls+12]
        cmp eax,3FFFh
        setl dl
        mov byte ptr [esp+4],dl
        mov eax,dword ptr [esp+4]
        push eax
        jmp InteriorCommerce_Shop_PlaceFixedUnits_0059A098
    InteriorCommerce_Shop_PlaceFixedUnits_0059A08D:
        push 0
        jmp InteriorCommerce_Shop_PlaceFixedUnits_0059A098
    InteriorCommerce_Shop_PlaceFixedUnits_0059A091:
        cmp edx,6
        jl InteriorCommerce_Shop_PlaceFixedUnits_0059A0A3
        push 1
    InteriorCommerce_Shop_PlaceFixedUnits_0059A098:
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceFixedUnitsCalls+8]
        mov dword ptr [esp+4],eax
    InteriorCommerce_Shop_PlaceFixedUnits_0059A0A3:
        fild dword ptr [esp+4]
        push edi
        push 0
        push 0
        fstp dword ptr [esp+10h]
        push 40000000h
        mov edi,dword ptr [esp+14h]
        push edi
        push 9
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceFixedUnitsCalls+4]
        push 0
        push 2
        push 0
        push edi
        push 0Ah
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_PlaceFixedUnitsCalls+4]
        pop edi
    InteriorCommerce_Shop_PlaceFixedUnits_0059A0D4:
        pop esi
        pop ecx
        ret
    }
}

// 0x59A1B0
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_FurnishEdgesCalls[] = {0x00591700,0x005918E0,0x00591E40,0x00599DC0,0x00821B1E,0x00821B40};
__declspec(naked) void Interior_c::Shop_FurnishEdges() {
    __asm {
        sub esp,10h
        push ebx
        push ebp
        push esi
        mov esi,ecx
        mov eax,dword ptr [esi+14h]
        movzx ebp,byte ptr [eax+2]
        movzx ebx,byte ptr [eax+3]
        push edi
        dec ebp
        dec ebx
        mov edi,1
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+10h],eax
        fild dword ptr [esp+10h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,32h
        jle InteriorCommerce_Shop_FurnishEdges_0059A1FD
        mov dword ptr [esp+10h],0
        jmp InteriorCommerce_Shop_FurnishEdges_0059A21F
    InteriorCommerce_Shop_FurnishEdges_0059A1FD:
        cmp eax,19h
        jle InteriorCommerce_Shop_FurnishEdges_0059A20C
        mov dword ptr [esp+10h],3
        jmp InteriorCommerce_Shop_FurnishEdges_0059A21F
    InteriorCommerce_Shop_FurnishEdges_0059A20C:
        xor ecx,ecx
        cmp eax,0Ah
        setle cl
        dec ecx
        and ecx,0FFFFFFFDh
        add ecx,9
        mov dword ptr [esp+10h],ecx
    InteriorCommerce_Shop_FurnishEdges_0059A21F:
        cmp ebp,1
        jle InteriorCommerce_Shop_FurnishEdges_0059A23A
    InteriorCommerce_Shop_FurnishEdges_0059A224:
        mov edx,dword ptr [esp+10h]
        push 0
        push ebx
        push edi
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+12]
        add edi,eax
        cmp edi,ebp
        jl InteriorCommerce_Shop_FurnishEdges_0059A224
    InteriorCommerce_Shop_FurnishEdges_0059A23A:
        dec ebx
        mov dword ptr [esp+18h],ebx
        mov edi,1
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+10h],eax
        fild dword ptr [esp+10h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,32h
        jle InteriorCommerce_Shop_FurnishEdges_0059A276
        mov dword ptr [esp+10h],0
        jmp InteriorCommerce_Shop_FurnishEdges_0059A298
    InteriorCommerce_Shop_FurnishEdges_0059A276:
        cmp eax,19h
        jle InteriorCommerce_Shop_FurnishEdges_0059A285
        mov dword ptr [esp+10h],3
        jmp InteriorCommerce_Shop_FurnishEdges_0059A298
    InteriorCommerce_Shop_FurnishEdges_0059A285:
        xor ecx,ecx
        cmp eax,0Ah
        setle cl
        dec ecx
        and ecx,0FFFFFFFDh
        add ecx,9
        mov dword ptr [esp+10h],ecx
    InteriorCommerce_Shop_FurnishEdges_0059A298:
        cmp ebx,1
        jl InteriorCommerce_Shop_FurnishEdges_0059A2B7
        lea ecx,[ecx]
    InteriorCommerce_Shop_FurnishEdges_0059A2A0:
        mov edx,dword ptr [esp+10h]
        push 1
        push edi
        push 0
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+12]
        add edi,eax
        cmp edi,ebx
        jle InteriorCommerce_Shop_FurnishEdges_0059A2A0
    InteriorCommerce_Shop_FurnishEdges_0059A2B7:
        mov edi,1
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+10h],eax
        fild dword ptr [esp+10h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,32h
        jle InteriorCommerce_Shop_FurnishEdges_0059A2EE
        mov dword ptr [esp+10h],0
        jmp InteriorCommerce_Shop_FurnishEdges_0059A310
    InteriorCommerce_Shop_FurnishEdges_0059A2EE:
        cmp eax,19h
        jle InteriorCommerce_Shop_FurnishEdges_0059A2FD
        mov dword ptr [esp+10h],3
        jmp InteriorCommerce_Shop_FurnishEdges_0059A310
    InteriorCommerce_Shop_FurnishEdges_0059A2FD:
        xor ecx,ecx
        cmp eax,0Ah
        setle cl
        dec ecx
        and ecx,0FFFFFFFDh
        add ecx,9
        mov dword ptr [esp+10h],ecx
    InteriorCommerce_Shop_FurnishEdges_0059A310:
        cmp ebx,1
        jl InteriorCommerce_Shop_FurnishEdges_0059A32B
    InteriorCommerce_Shop_FurnishEdges_0059A315:
        mov edx,dword ptr [esp+10h]
        push 3
        push edi
        push ebp
        push edx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+12]
        add edi,eax
        cmp edi,ebx
        jle InteriorCommerce_Shop_FurnishEdges_0059A315
    InteriorCommerce_Shop_FurnishEdges_0059A32B:
        dec ebp
        mov edi,1
        cmp ebp,edi
        mov dword ptr [esp+1Ch],ebp
        mov dword ptr [esp+10h],edi
        jl InteriorCommerce_Shop_FurnishEdges_0059A3E6
    InteriorCommerce_Shop_FurnishEdges_0059A341:
        mov ecx,dword ptr [esi+14h]
        mov al,byte ptr [ecx+0Ah]
        cmp al,0FFh
        je InteriorCommerce_Shop_FurnishEdges_0059A35A
        movsx eax,al
        cmp edi,eax
        jl InteriorCommerce_Shop_FurnishEdges_0059A35A
        movsx ecx,byte ptr [ecx+0Bh]
        cmp edi,ecx
        jle InteriorCommerce_Shop_FurnishEdges_0059A3BA
    InteriorCommerce_Shop_FurnishEdges_0059A35A:
        cmp dword ptr ds:[008D0948h],1
        jle InteriorCommerce_Shop_FurnishEdges_0059A3B4
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+14h],eax
        fild dword ptr [esp+14h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,3Ch
        jle InteriorCommerce_Shop_FurnishEdges_0059A3B4
        fild dword ptr [esp+18h]
        push 0
        push 2
        sub esp,8
        fstp dword ptr [esp+4]
        mov ecx,esi
        fild dword ptr [esp+20h]
        fstp dword ptr [esp]
        push 8
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+8]
        mov dword ptr ds:[008D0948h],0
    InteriorCommerce_Shop_FurnishEdges_0059A3B4:
        inc dword ptr ds:[008D0948h]
    InteriorCommerce_Shop_FurnishEdges_0059A3BA:
        push 1
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+4]
        test eax,eax
        jne InteriorCommerce_Shop_FurnishEdges_0059A3D9
        push eax
        push 2
        push 1
        push 1
        push 1
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
    InteriorCommerce_Shop_FurnishEdges_0059A3D9:
        inc edi
        cmp edi,ebp
        mov dword ptr [esp+10h],edi
        jle InteriorCommerce_Shop_FurnishEdges_0059A341
    InteriorCommerce_Shop_FurnishEdges_0059A3E6:
        mov edi,2
        dec ebx
        cmp ebx,edi
        mov dword ptr [esp+10h],edi
        jl InteriorCommerce_Shop_FurnishEdges_0059A4F4
        mov edx,dword ptr ds:[008D0948h]
        mov edi,edi
    InteriorCommerce_Shop_FurnishEdges_0059A400:
        mov ecx,dword ptr [esi+14h]
        mov al,byte ptr [ecx+6]
        cmp al,0FFh
        je InteriorCommerce_Shop_FurnishEdges_0059A419
        movsx eax,al
        cmp edi,eax
        jl InteriorCommerce_Shop_FurnishEdges_0059A419
        movsx ecx,byte ptr [ecx+7]
        cmp edi,ecx
        jle InteriorCommerce_Shop_FurnishEdges_0059A471
    InteriorCommerce_Shop_FurnishEdges_0059A419:
        cmp edx,1
        jle InteriorCommerce_Shop_FurnishEdges_0059A46A
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+18h],eax
        fild dword ptr [esp+18h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,3Ch
        jle InteriorCommerce_Shop_FurnishEdges_0059A464
        fild dword ptr [esp+10h]
        push 0
        push 3
        push ecx
        fstp dword ptr [esp]
        push 3F800000h
        push 8
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+8]
        xor edx,edx
        jmp InteriorCommerce_Shop_FurnishEdges_0059A46A
    InteriorCommerce_Shop_FurnishEdges_0059A464:
        mov edx,dword ptr ds:[008D0948h]
    InteriorCommerce_Shop_FurnishEdges_0059A46A:
        inc edx
        mov dword ptr ds:[008D0948h],edx
    InteriorCommerce_Shop_FurnishEdges_0059A471:
        mov ecx,dword ptr [esi+14h]
        mov al,byte ptr [ecx+8]
        cmp al,0FFh
        je InteriorCommerce_Shop_FurnishEdges_0059A48A
        movsx eax,al
        cmp edi,eax
        jl InteriorCommerce_Shop_FurnishEdges_0059A48A
        movsx ecx,byte ptr [ecx+9]
        cmp edi,ecx
        jle InteriorCommerce_Shop_FurnishEdges_0059A4E7
    InteriorCommerce_Shop_FurnishEdges_0059A48A:
        cmp edx,1
        jle InteriorCommerce_Shop_FurnishEdges_0059A4E0
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+18h],eax
        fild dword ptr [esp+18h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+20]
        cmp eax,3Ch
        jle InteriorCommerce_Shop_FurnishEdges_0059A4DA
        fild dword ptr [esp+10h]
        push 0
        push 1
        sub esp,8
        fstp dword ptr [esp+4]
        mov ecx,esi
        fild dword ptr [esp+2Ch]
        fstp dword ptr [esp]
        push 8
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+8]
        xor edx,edx
        jmp InteriorCommerce_Shop_FurnishEdges_0059A4E0
    InteriorCommerce_Shop_FurnishEdges_0059A4DA:
        mov edx,dword ptr ds:[008D0948h]
    InteriorCommerce_Shop_FurnishEdges_0059A4E0:
        inc edx
        mov dword ptr ds:[008D0948h],edx
    InteriorCommerce_Shop_FurnishEdges_0059A4E7:
        inc edi
        cmp edi,ebx
        mov dword ptr [esp+10h],edi
        jle InteriorCommerce_Shop_FurnishEdges_0059A400
    InteriorCommerce_Shop_FurnishEdges_0059A4F4:
        dec ebp
        mov edi,2
        cmp ebp,edi
        jl InteriorCommerce_Shop_FurnishEdges_0059A535
        mov edi,edi
    InteriorCommerce_Shop_FurnishEdges_0059A500:
        push 2
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+4]
        test eax,eax
        jne InteriorCommerce_Shop_FurnishEdges_0059A51F
        push eax
        push 2
        push 1
        push 1
        push 2
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
    InteriorCommerce_Shop_FurnishEdges_0059A51F:
        push 0
        push 3
        push 1
        push 1
        push ebx
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
        inc edi
        cmp edi,ebp
        jle InteriorCommerce_Shop_FurnishEdges_0059A500
    InteriorCommerce_Shop_FurnishEdges_0059A535:
        dec ebx
        mov edi,3
        cmp ebx,edi
        jl InteriorCommerce_Shop_FurnishEdges_0059A568
    InteriorCommerce_Shop_FurnishEdges_0059A540:
        push 0
        push 3
        push 1
        push 1
        push edi
        push 2
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
        push 0
        push 3
        push 1
        push 1
        push edi
        push ebp
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
        inc edi
        cmp edi,ebx
        jle InteriorCommerce_Shop_FurnishEdges_0059A540
    InteriorCommerce_Shop_FurnishEdges_0059A568:
        push 0
        push 3
        push 1
        add ebp,0FFFFFFFDh
        push ebp
        push 3
        push 3
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishEdgesCalls+0]
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,10h
        ret
    }
}

// 0x593DB0
bool Interior_c::GetBoundingBox(FurnitureEntity_c* entity, CVector* points) {
    const auto kind = *reinterpret_cast<const uint8*>(m_box);
    if (kind != 0 && kind != 1 && kind != 6) return false;
    int32 minX = entity->m_tileX, maxX = minX;
    int32 minY = entity->m_tileY, maxY = minY;
    std::array<int32, 900> visited{};
    visited[minX * 30 + minY] = 1;
    FindBoundingBox(minX, minY, &minX, &maxX, &minY, &maxY, visited.data());
    const float margin = StaticRef<float>(0x8D22B0);
    const auto lower = [margin](int32 value) { return static_cast<float>((static_cast<double>(value) - 0.5) - margin); };
    const auto upper = [margin](int32 value) { return static_cast<float>((static_cast<double>(value) + margin) + 0.5); };
    GetTileCentre(lower(minX), upper(maxY), points);
    GetTileCentre(lower(minX), lower(minY), points + 1);
    GetTileCentre(upper(maxX), lower(minY), points + 2);
    GetTileCentre(upper(maxX), upper(maxY), points + 3);
    return true;
}

// 0x593910
void Interior_c::ResetTiles() {
    std::memset(field_68, 0, sizeof(field_68));
    const auto* box = reinterpret_cast<const int8*>(m_box);
    const auto* dimensions = reinterpret_cast<const uint8*>(m_box);
    const auto span = [&](int32 start, int32 end, bool vertical, int32 edge, int32 status) {
        if (box[start] == -1 || box[end] - box[start] <= 0) return;
        if (vertical) SetTilesStatus(edge, box[start], 1, box[end] - box[start], status, 0);
        else SetTilesStatus(box[start], edge, box[end] - box[start], 1, status, 0);
    };
    span(6, 7, true, 0, 8);
    span(8, 9, true, dimensions[2] - 1, 8);
    span(10, 11, false, dimensions[3] - 1, 8);
    span(12, 13, true, 0, 9);
    span(14, 15, true, dimensions[2] - 1, 9);
    span(16, 17, false, dimensions[3] - 1, 9);
    for (int32 i = 0; i < 3; ++i) {
        const int32 x = box[0x12 + i], y = box[0x15 + i];
        if (x != -1 && y != -1) SetTilesStatus(x, y, box[0x18 + i], box[0x1B + i], 11, 0);
    }
}

// 0x5934E0
static float InteriorObjectZExact(float offsetZ,float boundMinZ,int32 negativeHeight) {
    float result;
    __asm {
        fild negativeHeight
        fmul dword ptr ds:[00858B8Ch]
        fadd offsetZ
        fsub boundMinZ
        fstp result
    }
    return result;
}

static void InteriorFurniturePositionExact(CVector* position,int32 negativeWidth,int32 negativeDepth,int32 boxHeight,int32 tileX,int32 tileY,int32 shiftX,int32 shiftY,float height,bool ceiling) {
    __asm {
        fild boxHeight
        fmul dword ptr ds:[00858B8Ch]
        cmp ceiling,0
        je interior_position_floor
        fsub height
        jmp interior_position_z_ready
    interior_position_floor:
        fchs
        fadd height
    interior_position_z_ready:
        fild negativeDepth
        fmul dword ptr ds:[00858B8Ch]
        fiadd shiftY
        fiadd tileY
        fadd dword ptr ds:[00858B8Ch]
        fild negativeWidth
        fmul dword ptr ds:[00858B8Ch]
        fiadd shiftX
        fiadd tileX
        fadd dword ptr ds:[00858B8Ch]
        mov ecx,position
        fld dword ptr [ecx]
        fadd st,st(1)
        fstp dword ptr [ecx]
        fstp st(0)
        fld dword ptr [ecx+4]
        fadd st,st(1)
        fstp dword ptr [ecx+4]
        fstp st(0)
        fld dword ptr [ecx+8]
        fadd st,st(1)
        fstp dword ptr [ecx+8]
        fstp st(0)
    }
}

CObject* Interior_c::PlaceObject(uint8 isStealable, Furniture_c* furniture, float offsetX, float offsetY, float offsetZ, float rotationZ) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    const CVector position{float(-int32(box[2])) * 0.5f + offsetX, float(-int32(box[3])) * 0.5f + offsetY,
        InteriorObjectZExact(offsetZ, CModelInfo::GetModelInfo(uint16(furniture->m_nModelId))->GetColModel()->GetBoundingBox().m_vecMin.z, -int32(box[4]))};
    auto& pool = StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC);
    if (int32(pool.GetNumItems()) < 1) return nullptr;
    CMatrix interior(&m_matrix, false), local, final;
    local.SetUnity(); local.RotateZ(rotationZ * 0.017453292f); local.GetPosition() += position;
    final = interior * local;
    auto* entry = pool.RemoveHead();
    if (!entry) return nullptr;
    auto* object = new CObject(uint16(furniture->m_nModelId), false);
    entry->m_entity = object;
    object->SetMatrix(final); object->SetAreaCode(eAreaCodes(uint8(m_areaCode)));
    object->m_bDontCastShadowsOn = true; object->m_nObjectType = OBJECT_TYPE_DECORATION;
    object->SetIsStatic(true); CWorld::Add(object);
    entry->m_tileX = uint16(int32(offsetX)); entry->m_tileY = uint16(int32(offsetY));
    m_list.AddItem(entry);
    if (!isStealable) return object;
    object->objectFlags.bIsLiftable = true;
    if (!g_interiorMan.HasInteriorHadStealDataSetup(this)) {
        auto& record = g_interiorMan.m_Objects[g_interiorMan.m_ObjectCount++];
        record.entity = object; record.modelId = uint16(furniture->m_nModelId); record.interiorId = m_interiorId; record.pos = position; record.wasStolen = false;
        return object;
    }
    const auto stolen = g_interiorMan.FindStealableObjectId(m_interiorId, uint16(furniture->m_nModelId), position);
    if (stolen >= 0 && g_interiorMan.m_Objects[stolen].wasStolen) {
        CWorld::Remove(object); delete object; entry->m_entity = nullptr;
        m_list.RemoveItem(entry); pool.AddItem(entry); return nullptr;
    }
    const auto existing = g_interiorMan.FindStealableObjectId(m_interiorId, uint16(furniture->m_nModelId), position);
    if (existing >= 0) g_interiorMan.m_Objects[existing].entity = object;
    return object;
}

// 0x5913B0
FurnitureEntity_c* Interior_c::GetFurnitureEntity(CEntity* entity) {
    for (auto* item = m_list.GetHead(); item; item = item->m_pNext) {
        if (item->m_entity == entity) return item;
    }
    return nullptr;
}

// 0x5913E0
bool Interior_c::IsPtInside(const CVector& pt, CVector bias) {
    const CVector delta{pt.x - m_matrix.pos.x, pt.y - m_matrix.pos.y, pt.z - m_matrix.pos.z};
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    return InteriorProjectionInside(&m_matrix.right, &delta, box[2], bias.x)
        && InteriorProjectionInside(&m_matrix.up, &delta, box[3], bias.y)
        && InteriorProjectionInside(&m_matrix.at, &delta, box[4], bias.z);
}

// 0x5914D0
void Interior_c::CalcMatrix(CVector* translation) {
    m_matrix.right = RwV3d{1.0f, 0.0f, 0.0f};
    m_matrix.up = RwV3d{0.0f, 1.0f, 0.0f};
    m_matrix.at = RwV3d{0.0f, 0.0f, 1.0f};
    m_matrix.pos = RwV3d{0.0f, 0.0f, 0.0f};
    m_matrix.flags |= 0x20003;
    const RwV3d axis{0.0f, 0.0f, 1.0f};
    const auto angle = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(m_box) + 0x20);
    RwMatrixRotate(&m_matrix, &axis, angle, rwCOMBINEREPLACE);
    RwMatrixTranslate(&m_matrix, reinterpret_cast<const RwV3d*>(translation), rwCOMBINEPOSTCONCAT);
    auto* entity = m_pGroup->GetEntity();
    const auto* rw = entity->GetRwObject();
    const auto* frame = rw ? reinterpret_cast<const RwFrame*>(rw->parent) : nullptr;
    RwMatrixMultiply(&m_matrix, &m_matrix, frame ? &frame->modelling : nullptr);
}

// 0x591590
void Interior_c::Furnish() {
    switch (*reinterpret_cast<const uint8*>(m_box)) {
    case 0: FurnishShop(0); break;
    case 1: FurnishOffice(); break;
    case 2: FurnishLounge(); break;
    case 3: FurnishBedroom(); break;
    case 4: FurnishKitchen(); break;
    }
}

// 0x5915D0
void Interior_c::Unfurnish() {
    for (auto* item = m_list.GetHead(); item;) {
        auto* next = item->m_pNext;
        auto* player = FindPlayerPed(-1);
        auto* held = player ? player->GetEntityThatThisPedIsHolding() : nullptr;
        auto* entity = item->m_entity;
        if (held && held == entity && held->GetType() == ENTITY_TYPE_OBJECT
            && (*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(held) + 0x140) & 0x2000)) {
            ++StaticRef<uint16>(0xBB4A70);
            auto* bytes = reinterpret_cast<uint8*>(held);
            bytes[0x13C] = 3;
            *reinterpret_cast<uint32*>(bytes + 0x150) = CTimer::GetTimeInMS() + 99999999u;
        } else {
            CWorld::Remove(entity);
            delete entity;
        }
        item->m_entity = nullptr;
        m_list.RemoveItem(item);
        StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC).AddItem(item);
        item = next;
    }
}

// 0x591680
int8 Interior_c::CheckTilesEmpty(int32 x, int32 y, int32 width, int32 height, uint8 allowWindows) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    if (x < 0 || y < 0 || x + width > box[2] || y + height > box[3]) return false;
    for (int32 i = 0; i < width; ++i) {
        for (int32 j = 0; j < height; ++j) {
            const auto tile = static_cast<uint8>(field_68[(x + i) * 30 + y + j]);
            if (tile != 0 && !(allowWindows && tile == 9)) return false;
        }
    }
    return true;
}

// 0x591700
void Interior_c::SetTilesStatus(int32 x, int32 y, int32 width, int32 height, int32 status, int8 force) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    if (x < 0 || y < 0 || x + width > box[2] || y + height > box[3]) return;
    for (int32 i = 0; i < width; ++i) {
        for (int32 j = 0; j < height; ++j) {
            auto& tile = field_68[(x + i) * 30 + y + j];
            if (tile == 9 && status == 5) tile = 10;
            else if (!force) {
                if (tile == 3) {
                    if (status == 3) return;
                    if (status == 4) tile = 4;
                } else if (tile == 0) tile = static_cast<char>(status);
            } else if (tile != 5 && tile != 7 && tile != 8) tile = static_cast<char>(status);
        }
    }
}

// 0x5917C0
void Interior_c::SetCornerTiles(int32 corner, int32 length, int32 status, uint8 force) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    switch (corner) {
    case 0:
        SetTilesStatus(0, box[3] - 1, length, 1, status, static_cast<int8>(force));
        SetTilesStatus(0, box[3] - length, 1, length, status, static_cast<int8>(force));
        break;
    case 1:
        SetTilesStatus(0, 0, length, 1, status, static_cast<int8>(force));
        SetTilesStatus(0, 0, 1, length, status, static_cast<int8>(force));
        break;
    case 2:
        SetTilesStatus(box[2] - length, 0, length, 1, status, static_cast<int8>(force));
        SetTilesStatus(box[2] - 1, 0, 1, length, status, static_cast<int8>(force));
        break;
    case 3:
        SetTilesStatus(box[2] - length, box[3] - 1, length, 1, status, static_cast<int8>(force));
        SetTilesStatus(box[2] - 1, box[3] - length, 1, length, status, static_cast<int8>(force));
        break;
    }
}

// 0x5918E0
int32 Interior_c::GetTileStatus(int32 x, int32 y) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    if (x < 0 || y < 0 || x >= box[2] || y >= box[3]) return 1;
    return static_cast<uint8>(field_68[x * 30 + y]);
}

// 0x591920
int32 Interior_c::GetNumEmptyTiles(int32 x, int32 y, int32 direction, int32 width) {
    const int32 step = direction == 0 || direction == 3 ? -1 : 1;
    int32 count = 0;
    for (;;) {
        for (int32 i = 0; i < width; ++i) {
            if (GetTileStatus(x + (direction == 1 || direction == 3 ? 0 : i),
                              y + (direction == 1 || direction == 3 ? i : 0)) != 0) return count;
        }
        if (direction == 1 || direction == 3) x += step;
        else y += step;
        ++count;
    }
}

// 0x591B20
int32 Interior_c::GetRandomTile(int32 status, int32* outX, int32* outY) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    for (;;) {
        const int32 x = InteriorRandomCoordinate(box[2]);
        const int32 y = InteriorRandomCoordinate(box[3]);
        if (GetTileStatus(x, y) != status) continue;
        *outX = x;
        *outY = y;
        return y;
    }
}

// 0x59A590
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_Shop_FurnishAislesCalls[] = {0x00591700,0x00591D20,0x00599DC0,0x0059A140,0x00821B1E,0x00821B40};
__declspec(naked) void Interior_c::Shop_FurnishAisles() {
    __asm {
        sub esp,14h
        push ebx
        push ebp
        push esi
        mov esi,ecx
        mov eax,dword ptr [esi+14h]
        movzx ebx,byte ptr [eax+2]
        push edi
        movzx edi,byte ptr [eax+3]
        sub edi,4
        sub ebx,6
        test ebx,ebx
        lea ebp,[edi-3]
        mov dword ptr [esp+18h],ebx
        mov dword ptr [esp+20h],ebp
        jle InteriorCommerce_Shop_FurnishAisles_0059A77F
        test ebp,ebp
        jle InteriorCommerce_Shop_FurnishAisles_0059A77F
        push 0BF000000h
        push 0BF000000h
        push 3
        push 2
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
        push 3F000000h
        push 0BF000000h
        inc edi
        push edi
        push 2
        mov ecx,esi
        mov dword ptr [esp+2Ch],edi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
        test ebx,ebx
        mov dword ptr [esp+10h],0
        jle InteriorCommerce_Shop_FurnishAisles_0059A751
    InteriorCommerce_Shop_FurnishAisles_0059A601:
        mov edi,4
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+16]
        and eax,0FFFFh
        mov dword ptr [esp+14h],eax
        fild dword ptr [esp+14h]
        fmul dword ptr ds:[00858B14h]
        fmul dword ptr ds:[00858628h]
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+20]
        cmp eax,32h
        jle InteriorCommerce_Shop_FurnishAisles_0059A632
        xor ebx,ebx
        jmp InteriorCommerce_Shop_FurnishAisles_0059A64D
    InteriorCommerce_Shop_FurnishAisles_0059A632:
        cmp eax,19h
        jle InteriorCommerce_Shop_FurnishAisles_0059A63E
        mov ebx,3
        jmp InteriorCommerce_Shop_FurnishAisles_0059A64D
    InteriorCommerce_Shop_FurnishAisles_0059A63E:
        xor ebx,ebx
        cmp eax,0Ah
        setle bl
        dec ebx
        and ebx,0FFFFFFFDh
        add ebx,9
    InteriorCommerce_Shop_FurnishAisles_0059A64D:
        mov ecx,dword ptr [esp+10h]
        mov eax,ecx
        and eax,80000003h
        jns InteriorCommerce_Shop_FurnishAisles_0059A65F
        dec eax
        or eax,0FFFFFFFCh
        inc eax
    InteriorCommerce_Shop_FurnishAisles_0059A65F:
        jne InteriorCommerce_Shop_FurnishAisles_0059A6A0
        mov eax,dword ptr [esp+18h]
        dec eax
        cmp ecx,eax
        je InteriorCommerce_Shop_FurnishAisles_0059A738
        test ebp,ebp
        jle InteriorCommerce_Shop_FurnishAisles_0059A738
        mov dword ptr [esp+14h],ebp
        lea ebx,[ebx+00000000h]
    InteriorCommerce_Shop_FurnishAisles_0059A680:
        mov eax,dword ptr [esp+10h]
        push 3
        push edi
        add eax,3
        push eax
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+8]
        add edi,eax
        dec dword ptr [esp+14h]
        jne InteriorCommerce_Shop_FurnishAisles_0059A680
        jmp InteriorCommerce_Shop_FurnishAisles_0059A738
    InteriorCommerce_Shop_FurnishAisles_0059A6A0:
        cmp eax,1
        jne InteriorCommerce_Shop_FurnishAisles_0059A6CE
        test ebp,ebp
        jle InteriorCommerce_Shop_FurnishAisles_0059A738
        lea ecx,[ecx]
    InteriorCommerce_Shop_FurnishAisles_0059A6B0:
        mov eax,dword ptr [esp+10h]
        push 1
        push edi
        add eax,3
        push eax
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+8]
        add edi,eax
        dec ebp
        jne InteriorCommerce_Shop_FurnishAisles_0059A6B0
        mov ebp,dword ptr [esp+20h]
        jmp InteriorCommerce_Shop_FurnishAisles_0059A738
    InteriorCommerce_Shop_FurnishAisles_0059A6CE:
        cmp eax,2
        jne InteriorCommerce_Shop_FurnishAisles_0059A6F5
        xor edi,edi
        test ebp,ebp
        jle InteriorCommerce_Shop_FurnishAisles_0059A738
        lea ebx,[ecx+3]
        lea esp,[esp]
    InteriorCommerce_Shop_FurnishAisles_0059A6E0:
        push 3
        lea ecx,[edi+4]
        push ecx
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+12]
        inc edi
        cmp edi,ebp
        jl InteriorCommerce_Shop_FurnishAisles_0059A6E0
        jmp InteriorCommerce_Shop_FurnishAisles_0059A738
    InteriorCommerce_Shop_FurnishAisles_0059A6F5:
        cmp eax,3
        jne InteriorCommerce_Shop_FurnishAisles_0059A738
        push 0
        push eax
        push ebp
        push 1
        lea edi,[ecx+3]
        push 4
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+0]
        push 0BF000000h
        push 0BF000000h
        push 3
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
        mov edx,dword ptr [esp+1Ch]
        push 3F000000h
        push 0BF000000h
        push edx
        push edi
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
    InteriorCommerce_Shop_FurnishAisles_0059A738:
        mov eax,dword ptr [esp+10h]
        mov ecx,dword ptr [esp+18h]
        inc eax
        cmp eax,ecx
        mov dword ptr [esp+10h],eax
        jl InteriorCommerce_Shop_FurnishAisles_0059A601
        mov edi,dword ptr [esp+1Ch]
    InteriorCommerce_Shop_FurnishAisles_0059A751:
        mov ebx,dword ptr [esp+10h]
        push 0BF000000h
        push 3F000000h
        push 3
        add ebx,3
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
        push 3F000000h
        push 3F000000h
        push edi
        push ebx
        mov ecx,esi
        call dword ptr [InteriorCommerce_Shop_FurnishAislesCalls+4]
    InteriorCommerce_Shop_FurnishAisles_0059A77F:
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp,14h
        ret
    }
}

// 0x591BD0
CVector* Interior_c::GetTileCentre(float offsetX, float offsetY, CVector* pointsIn) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    InteriorLocalTileCentre(-int32(box[2]), -int32(box[3]), -int32(box[4]), offsetX, offsetY, pointsIn);
    return reinterpret_cast<CVector*>(RwV3dTransformPoints(reinterpret_cast<RwV3d*>(pointsIn), reinterpret_cast<const RwV3d*>(pointsIn), 1, &m_matrix));
}

// 0x591D20
void Interior_c::AddGotoPt(int32 x, int32 y, float offsetX, float offsetY) {
    if (field_40C >= 16) return;
    const auto status = GetTileStatus(x, y);
    if (status != 3 && status != 7) return;
    CVector position;
    GetTileCentre(static_cast<float>(x) + offsetX, static_cast<float>(y) + offsetY, &position);
    auto* entry = reinterpret_cast<uint8*>(this) + 0x410 + int32(field_40C) * 0x10;
    entry[0] = static_cast<uint8>(x);
    entry[1] = static_cast<uint8>(y);
    *reinterpret_cast<CVector*>(entry + 4) = position;
    SetTilesStatus(x, y, 1, 1, 4, 0);
    ++field_40C;
}

// 0x591E40
bool Interior_c::AddInteriorInfo(int32 actionType, float offsetX, float offsetY, int32 direction, CEntity* entityIgnoredCollision) {
    if (m_interiorInfosCount >= 16) return false;
    CVector position;
    GetTileCentre(offsetX, offsetY, &position);
    position.z += 0.8f;
    CVector facing{};
    if (direction != -1) {
        if (direction == 3) facing.x = -1.0f;
        else if (direction == 1) facing.x = 1.0f;
        else if (direction == 2) facing.y = 1.0f;
        else if (direction == 0) facing.y = -1.0f;
        RwV3dTransformVectors(reinterpret_cast<RwV3d*>(&facing), reinterpret_cast<const RwV3d*>(&facing), 1, &m_matrix);
    }
    auto& info = m_interiorInfos[m_interiorInfosCount];
    *reinterpret_cast<uint8*>(&info.Type) = static_cast<uint8>(actionType);
    info.Pos = position;
    info.Dir = facing;
    info.IsInUse = false;
    info.EntityIgnoredCollision = entityIgnoredCollision;
    ++m_interiorInfosCount;
    return true;
}

// 0x591F90
void Interior_c::AddPickups() {
    if (uint32(CTimer::GetTimeInMS() - StaticRef<uint32>(0xBB3DC4)) < 180000u) return;
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    for (int32 attempt = 0; attempt < 100; ++attempt) {
        const int32 x = InteriorRandomCoordinate(box[2] - 1);
        const int32 y = InteriorRandomCoordinate(box[3] - 1);
        const auto status = GetTileStatus(x, y);
        if (status != 0 && status != 3 && status != 4) continue;
        CVector position;
        GetTileCentre(static_cast<float>(x), static_cast<float>(y), &position);
        if (InteriorRandomCoordinate(100) < 75) {
            const auto amount = uint32(10 - InteriorRandomCoordinate(-40));
            CPickups::GenerateNewOne(position, StaticRef<uint16>(0x8CD59C), static_cast<ePickupType>(8), amount, 0, false, nullptr);
        } else {
            position.z += 0.5f;
            const int32 choice = InteriorRandomCoordinate(100);
            const auto weapon = static_cast<eWeaponType>(choice < 40 ? 5 : choice < 80 ? 22 : choice < 90 ? 4 : 25);
            const auto amount = uint32(3 - InteriorRandomCoordinate(-15));
            CPickups::GenerateNewOne_WeaponType(position, weapon, static_cast<ePickupType>(3), amount, false, nullptr);
        }
        return;
    }
}

// 0x5922C0
void Interior_c::FindBoundingBox(int32 x, int32 y, int32* minX, int32* maxX, int32* minY, int32* maxY, int32* visited) {
    for (;;) {
        if (x > 0 && GetTileStatus(x - 1, y) == 5 && !visited[(x - 1) * 30 + y]) {
            visited[(x - 1) * 30 + y] = 1;
            if (x - 1 < *minX) *minX = x - 1;
            FindBoundingBox(x - 1, y, minX, maxX, minY, maxY, visited);
        }
        if (y < 29 && GetTileStatus(x, y + 1) == 5 && !visited[x * 30 + y + 1]) {
            visited[x * 30 + y + 1] = 1;
            if (y + 1 > *maxY) *maxY = y + 1;
            FindBoundingBox(x, y + 1, minX, maxX, minY, maxY, visited);
        }
        if (x < 29 && GetTileStatus(x + 1, y) == 5 && !visited[(x + 1) * 30 + y]) {
            visited[(x + 1) * 30 + y] = 1;
            if (x + 1 > *maxX) *maxX = x + 1;
            FindBoundingBox(x + 1, y, minX, maxX, minY, maxY, visited);
        }
        if (y < 1 || GetTileStatus(x, y - 1) != 5 || visited[x * 30 + y - 1]) return;
        visited[x * 30 + y - 1] = 1;
        if (y - 1 < *minY) *minY = y - 1;
        --y;
    }
}

// 0x5924A0
void Interior_c::CalcExitPts() {
    const auto* box = reinterpret_cast<const int8*>(m_box);
    const auto* dimensions = reinterpret_cast<const uint8*>(m_box);
    auto* bytes = reinterpret_cast<uint8*>(this);
    const int32 count = field_40C;
    const auto tile = [bytes](int32 index, int32 component) {
        return reinterpret_cast<const int8*>(bytes)[0x410 + index * 0x10 + component];
    };
    const auto links = [bytes](int32 offset, int32 first, int32 second) {
        bytes[offset] = static_cast<uint8>(first);
        bytes[offset + 1] = static_cast<uint8>(second);
    };
    const auto point = [this, bytes](int32 offset, float x, float y) {
        CVector position;
        GetTileCentre(x, y, &position);
        *reinterpret_cast<CVector*>(bytes + offset) = position;
    };
    const auto midpoint = [](int32 start, int32 end) {
        return static_cast<float>((static_cast<double>(end - start) * 0.5 + start) - 0.5);
    };
    if (box[5] >= 0) {
        const float along = static_cast<float>(box[5]) - 0.5f;
        float outward = 0.0f;
        if (count < 3) links(0x512, -1, -1);
        else {
            int32 index = 0;
            while (index < count && !(along < static_cast<float>(tile(index, 0)))) index += 2;
            if (index == 0) links(0x512, 0, -1);
            else if (index == count) links(0x512, count - 2, -1);
            else links(0x512, index, index - 2);
            outward = static_cast<float>(tile(0, 1)) - 0.25f;
        }
        point(0x514, along, outward);
        point(0x524, along, -0.25f);
    }
    if (box[6] >= 0) {
        const float along = midpoint(box[6], box[7]);
        float outward = 0.0f;
        if (count < 3) links(0x532, -1, -1);
        else {
            if (static_cast<float>(tile(0, 1)) <= along) {
                if (along <= static_cast<float>(tile(1, 1))) links(0x532, 0, 1);
                else links(0x532, 1, -1);
            } else links(0x532, 0, -1);
            outward = static_cast<float>(tile(0, 0)) - 0.25f;
        }
        point(0x534, outward, along);
        point(0x544, -0.25f, along);
    }
    if (box[10] >= 0) {
        const float along = midpoint(box[10], box[11]);
        float outward = static_cast<float>(dimensions[3] - 1);
        if (count < 3) links(0x552, -1, -1);
        else {
            int32 index = 1;
            while (index < count && !(along < static_cast<float>(tile(index, 0)))) index += 2;
            if (index == count) links(0x552, count - 1, -1);
            else links(0x552, index, index - 2);
            outward = static_cast<float>(tile(1, 1)) + 0.25f;
        }
        point(0x554, along, outward);
        point(0x564, along, static_cast<float>(dimensions[3] - 1) + 0.25f);
    }
    if (box[8] >= 0) {
        const float along = midpoint(box[8], box[9]);
        float outward = static_cast<float>(dimensions[2] - 1);
        if (count < 3) links(0x572, -1, -1);
        else {
            if (static_cast<float>(tile(count - 2, 1)) <= along) {
                if (along <= static_cast<float>(tile(count - 1, 1))) links(0x572, count - 1, count - 2);
                else links(0x572, count - 1, -1);
            } else links(0x572, count - 2, -1);
            outward = static_cast<float>(tile(count - 2, 0)) + 0.25f;
        }
        point(0x574, outward, along);
        point(0x584, static_cast<float>(dimensions[2] - 1) + 0.25f, along);
    }
}

// 0x5929F0
bool Interior_c::IsVisible() {
    const CVector camera = TheCamera.GetPosition();
    if (IsPtInside(camera, CVector{5.0f, 5.0f, 0.0f})) return true;
    return reinterpret_cast<const int8*>(m_box)[5] > 0 && InteriorNearEntrance(&camera, &m_position);
}

// 0x592AA0
CEntity* Interior_c::PlaceFurniture(Furniture_c* furniture, int32 tileX, int32 tileY, float height, int32 placement, int32 direction, int32* width, int32* depth, uint8 unchecked) {
    auto& pool = StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC);
    const auto fail = [&]() -> CEntity* { *width = *depth = 0; return nullptr; };
    if (int32(pool.GetNumItems()) < 1) return fail();
    const int32 fw = uint8(furniture->m_nWidthX), fd = uint8(furniture->m_nWidthY);
    const int32 w = direction == 1 || direction == 3 ? fd : fw;
    const int32 d = direction == 1 || direction == 3 ? fw : fd;
    if (placement == 1 && !unchecked && !CheckTilesEmpty(tileX, tileY, w, d, furniture->m_bCanPlaceInFrontOfWindow)) return fail();
    CMatrix jitter;
    jitter.SetUnity();
    if (const auto maxAngle = uint8(furniture->m_nMaxAng)) {
        CMatrix toOrigin, rotation, fromOrigin;
        const float x = 0.5f - float(fw) * 0.5f, y = 0.5f - float(fd) * 0.5f;
        fromOrigin.SetUnity(); fromOrigin.SetTranslate(CVector{x, y, 0.0f});
        rotation.SetUnity(); rotation.RotateZ(float(CGeneral::GetRandomIntInRange(-int32(maxAngle), int32(maxAngle))) * 0.017453292f);
        toOrigin.SetUnity(); toOrigin.SetTranslate(CVector{-x, -y, 0.0f});
        jitter = toOrigin * rotation * fromOrigin;
    }
    CMatrix interior(&m_matrix, false), local, final;
    local.SetUnity(); local.RotateZ(float(direction) * 1.5707964f);
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    const int32 dx = direction == 1 || direction == 2 ? w - 1 : 0;
    const int32 dy = direction == 2 || direction == 3 ? d - 1 : 0;
    if (placement == 0 || placement == 1 || placement == 2) {
        InteriorFurniturePositionExact(&local.GetPosition(), -int32(box[2]), -int32(box[3]), int32(box[4]), tileX, tileY, dx, dy, height, placement == 0);
    }
    final = interior * local * jitter;
    auto* entry = pool.RemoveHead();
    if (!entry) return fail();
    auto* entity = new CBuilding;
    entry->m_entity = entity;
    entity->SetModelIndexNoCreate(uint16(furniture->m_nModelId));
    entity->SetMatrix(final);
    entity->SetAreaCode(eAreaCodes(uint8(m_areaCode)));
    entity->m_bDontCastShadowsOn = true;
    entity->m_bIsTempBuilding = true;
    CWorld::Add(entity);
    entry->m_tileX = uint16(tileX); entry->m_tileY = uint16(tileY);
    m_list.AddItem(entry);
    if (placement == 1) SetTilesStatus(tileX, tileY, w, d, furniture->m_bIsTall ? 6 : 5, 1);
    *width = w; *depth = d;
    return entity;
}

// 0x593120
CEntity* Interior_c::PlaceFurnitureOnWall(int32 group, int32 subgroup, int32 id, float height, int32 placement, int32 direction, int32 alongWall, int32 inset, int32* outDirection, int32* outAlongWall, int32* outX, int32* outY, int32* outWidth, int32* outDepth) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    auto* furniture = g_furnitureMan.GetFurniture(group, subgroup, int16(id), box[31]);
    if (!furniture) return nullptr;
    const int32 originalDirection = direction, originalAlong = alongWall;
    const int32 attempts = direction != -1 && alongWall != -1 ? 1 : 100;
    for (int32 attempt = 0; attempt < attempts; ++attempt) {
        const auto randomScaled = [](int32 range) { return int32(double(CGeneral::GetRandomNumber() & 0xFFFF) * double(StaticRef<float>(0x858B14)) * double(range)); };
        if (originalDirection == -1) direction = randomScaled(4);
        int32 x, y;
        if (direction == 1 || direction == 3) {
            if (originalAlong == -1) alongWall = randomScaled(int32(box[3]) - uint8(furniture->m_nWidthX));
            x = direction == 1 ? inset : int32(box[2]) - uint8(furniture->m_nWidthY) - inset;
            y = alongWall;
        } else {
            if (originalAlong == -1) alongWall = randomScaled(int32(box[2]) - uint8(furniture->m_nWidthX));
            x = alongWall;
            y = direction == 2 ? inset : int32(box[3]) - uint8(furniture->m_nWidthY) - inset;
        }
        int32 width, depth;
        if (auto* entity = PlaceFurniture(furniture, x, y, height, placement, direction, &width, &depth, 0)) {
            if (outDirection) *outDirection = direction;
            if (outAlongWall) *outAlongWall = alongWall;
            if (outX) *outX = x;
            if (outY) *outY = y;
            if (outWidth) *outWidth = width;
            if (outDepth) *outDepth = depth;
            return entity;
        }
    }
    return nullptr;
}

// 0x593340
CEntity* Interior_c::PlaceFurnitureInCorner(int32 group, int32 subgroup, int32 id, float height, int32 placement, int32 direction, int32 inset, int32* outDirection, int32* outX, int32* outY, int32* outWidth, int32* outDepth) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    auto* furniture = g_furnitureMan.GetFurniture(group, subgroup, int16(id), box[31]);
    if (!furniture) return nullptr;
    const int32 originalDirection = direction, attempts = direction == -1 ? 20 : 1;
    int32 x = direction, y = direction;
    for (int32 attempt = 0; attempt < attempts; ++attempt) {
        if (originalDirection == -1) direction = int32(double(CGeneral::GetRandomNumber() & 0xFFFF) * double(StaticRef<float>(0x858B14)) * 4.0);
        switch (direction) {
        case 1: x = inset; y = 0; break;
        case 3: x = int32(box[2]) - uint8(furniture->m_nWidthY) - inset; y = int32(box[3]) - uint8(furniture->m_nWidthX); break;
        case 2: x = int32(box[2]) - uint8(furniture->m_nWidthX); y = inset; break;
        case 0: x = 0; y = int32(box[3]) - uint8(furniture->m_nWidthY) - inset; break;
        }
        int32 width, depth;
        if (auto* entity = PlaceFurniture(furniture, x, y, height, placement, direction, &width, &depth, 0)) {
            if (outDirection) *outDirection = direction;
            if (outX) *outX = x;
            if (outY) *outY = y;
            if (outWidth) *outWidth = width;
            if (outDepth) *outDepth = depth;
            return entity;
        }
    }
    return nullptr;
}

// 0x591C50
bool Interior_c::FindEmptyTiles(int32 width, int32 height, int32* outX, int32* outY) {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    for (int32 attempt = 0; attempt < 100; ++attempt) {
        const int32 x = InteriorRandomCoordinate(box[2] - width);
        const int32 y = InteriorRandomCoordinate(box[3] - height);
        if (!CheckTilesEmpty(x, y, width, height, 1)) continue;
        *outX = x;
        *outY = y;
        return true;
    }
    return false;
}

// 0x59A790
// Canonical EXE transcription; internal branches reside in this DLL.
static const uintptr_t InteriorCommerce_FurnishShopCalls[] = {0x00591700,0x0059A030,0x0059A1B0,0x0059A590};
__declspec(naked) void Interior_c::FurnishShop(int32 a2) {
    __asm {
        mov al,byte ptr [esp+4]
        push esi
        mov esi,ecx
        mov byte ptr [esi+00000790h],al
        mov eax,dword ptr [esi+14h]
        movsx ecx,byte ptr [eax+5]
        lea edx,[ecx-1]
        cmp edx,6
        jge InteriorCommerce_FurnishShop_0059A7B7
        movzx eax,byte ptr [eax+2]
        sub eax,ecx
        cmp eax,6
        jl InteriorCommerce_FurnishShop_0059A831
    InteriorCommerce_FurnishShop_0059A7B7:
        push 0
        push 2
        push 1
        push 1
        push 0
        push 0
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+0]
        mov ecx,dword ptr [esi+14h]
        movzx edx,byte ptr [ecx+3]
        push 0
        push 2
        push 1
        push 1
        dec edx
        push edx
        push 0
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+0]
        mov eax,dword ptr [esi+14h]
        movzx ecx,byte ptr [eax+2]
        push 0
        push 2
        push 1
        push 1
        dec ecx
        push 0
        push ecx
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+0]
        mov eax,dword ptr [esi+14h]
        movzx edx,byte ptr [eax+3]
        movzx eax,byte ptr [eax+2]
        push 0
        push 2
        push 1
        push 1
        dec edx
        push edx
        dec eax
        push eax
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+0]
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+4]
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+8]
        mov ecx,esi
        call dword ptr [InteriorCommerce_FurnishShopCalls+12]
    InteriorCommerce_FurnishShop_0059A831:
        pop esi
        ret 4
    }
}

#pragma warning(pop)

Interior_c* Interior_c::Constructor() {
    m_pPrev = nullptr;
    m_pNext = nullptr;
    m_list = {};
    field_3EC = -1;
    m_nodeAddress.m_wAreaId = UINT16_MAX;
    return this;
}

void Interior_c::Destructor() {}

// --- Funciones reconstruidas con el cuerpo exacto del EXE (antes en InteriorCpp.cpp e InteriorShelfCpp.cpp) ---
// Interior_c.cpp se compila completo con /Zi /arch:IA32 (sin /ZI, PCH ni unity); no con /O2, porque sus transcripciones naked
// escriben EBX/ESI/EDI como el EXE. Sólo Shop_AddShelfInfo se optimiza con pragmas (ver source/CMakeLists.txt).

// 0x599A30
// Integer layout and return value match the EXE, including negative remainders.
int32 Interior_c::Office_FurnishCenter() {
    const auto* box = reinterpret_cast<const uint8*>(m_box);
    const int32 width = int32(box[2]) - 6;
    const int32 height = int32(box[3]) - 6;
    const int32 columns = width / 6;
    const int32 rows = height / 6;
    const int32 insetX = (width % 6) / 2;
    const int32 insetY = (height % 6) / 2;
    if (width > 0 && height > 0 && columns > 0) {
        for (int32 column = 0; column < columns; ++column) {
            for (int32 row = 0; row < rows; ++row) {
                Office_PlaceDeskQuad(-1, insetX + 6 + column * 6,
                    insetY + 6 + row * 6, int32(m_furnitureId));
            }
        }
    }
    // 0x599ACB reloads this value after each populated column. Empty paths
    // retain the same EAX computed at 0x599A8C.
    return insetY;
}

// 0x59A130
void Interior_c::Shop_FurnishCeiling() {
}


// 0x599AF0
void Interior_c::FurnishOffice() {
    const auto dimension = [this](unsigned offset) {
        return int32(reinterpret_cast<const uint8*>(m_box)[offset]);
    };
    SetTilesStatus(0, 0, 2, 2, 2, 0);
    SetTilesStatus(0, dimension(3) - 2, 2, 2, 2, 0);
    SetTilesStatus(dimension(2) - 2, 0, 2, 2, 2, 0);
    SetTilesStatus(dimension(2) - 2, dimension(3) - 2, 2, 2, 2, 0);
    m_furnitureId = int8(g_furnitureMan.GetRandomId(1, 0, uint8(dimension(0x1F))));
    field_792 = int8(g_furnitureMan.GetRandomId(1, 1, uint8(dimension(0x1F))));
    Office_FurnishEdges();
    Office_FurnishCenter();
    Shop_FurnishCeiling();
}

// 0x59A140
#pragma optimize("gt", on)
#pragma float_control(precise, off, push)
void Interior_c::Shop_AddShelfInfo(int32 x, int32 y, int32 direction) {
    auto& counter = *reinterpret_cast<uint32*>(0x8D0948);
    if (int32(counter) > 1) {
        // This is the external CRT rand collaborator, not a game method body.
        const uint32 draw = uint32(reinterpret_cast<int(__cdecl*)()>(0x821B1E)()) & 0xFFFF;
        const float chance = float(draw) * (1.0f / 32768.0f) * 100.0f;
        if (int32(chance) > 60) {
            AddInteriorInfo(8, float(x), float(y), direction, nullptr);
            counter = 1;
            return;
        }
    }
    // EXE INC wraps all bit patterns, including INT_MAX. Unsigned avoids UB.
    ++counter;
}
#pragma float_control(pop)
#pragma optimize("", on)
