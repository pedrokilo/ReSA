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
#include "X87Math.h"
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// Toda la aritmetica float de este archivo es x87 a 24 bits (el archivo se compila con /arch:IA32): una operacion float de C++ equivale a una
// instruccion x87 del EXE. precise impide reasociar o contraer (a * b + c) en las compilaciones /fp:fast. Se cierra antes de Shop_AddShelfInfo.
#pragma float_control(precise, on, push)

namespace {
// Acceso por desplazamiento a los campos del interior, de la caja de efectos (m_box) y de los muebles, como los mov del EXE.
template<class T>
T& InteriorField(const void* base, ptrdiff_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}
template<class T>
T& InteriorField(uintptr_t base, ptrdiff_t offset) {
    return *reinterpret_cast<T*>(base + offset);
}
// Patron de bits de un float guardado en una variable de pila del EXE (fstp dword) y viceversa.
uint32 InteriorFloatBits(float value) {
    uint32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}
float InteriorBitsFloat(uint32 bits) {
    float value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
// rand() del CRT del EXE (0x821B1E): srand(0x821B11) se hace en Init con la semilla del interior, asi que no sirve el rand de la DLL.
int32 InteriorRandCpp() {
    return reinterpret_cast<int(__cdecl*)()>(0x821B1E)();
}
}
namespace {
// 0x821B40 (_ftol) de: fild random; fmul factor; fimul extent. random < 65536 y el factor es 2^-15: el primer producto es exacto, el segundo se redondea una vez.
int32 InteriorRandomCoordinate(int32 extent) {
    const int32 random = CGeneral::GetRandomNumber() & 0xFFFF;
    const float factor = StaticRef<float>(0x858B14);
    return int32(IntTimesFloatCpp(extent, float(random) * factor));
}
// fild * 0,5 + offset + 0,5 para x e y; z es fild * 0,5 (cada paso redondeado a 24 bits; el entero entra exacto en fild).
void InteriorLocalTileCentre(int32 negativeWidth, int32 negativeHeight, int32 negativeDepth, float offsetX, float offsetY, CVector* output) {
    const float half = 0.5f;
    output->x = IntTimesFloatCpp(negativeWidth, half) + offsetX + half;
    output->y = IntTimesFloatCpp(negativeHeight, half) + offsetY + half;
    output->z = IntTimesFloatCpp(negativeDepth, half);
}
}
namespace {
// |(y*y' + x*x') + z*z'| frente a dimension * 0,5 + bias con fcompp: C0 solo cuando el limite es menor que la proyeccion; el igual (C3) y el
// no ordenado (C0 | C2) dejan pasar, asi que el resultado es !(limite < |proyeccion|).
bool InteriorProjectionInside(const RwV3d* axis, const CVector* delta, int32 dimension, float bias) {
    const float half = 0.5f;
    const float dot = (axis->y * delta->y + axis->x * delta->x) + axis->z * delta->z;
    const float magnitude = dot < 0.0f ? -dot : dot;
    const float limit = IntTimesFloatCpp(dimension, half) + bias;
    return !(limit < magnitude);
}
}
namespace {
// fld; call _ftol: truncamiento a entero del float (tambien para valores fuera de rango, como el CRT del EXE).
uint32 InteriorTruncate(float value) {
    return uint32(int32(value));
}
// ((z * y) * x) + extra (fiadd, entero exacto) y _ftol.
uint32 InteriorPositionSeed(const CVector* position, int32 extra) {
    return uint32(int32(FloatPlusIntCpp(position->z * position->y * position->x, extra)));
}
// (dy * dy + dx * dx) < 100 con fcomp: C0 solo cuando es menor; el no ordenado da C0 | C2 (5) y no cuenta.
bool InteriorNearEntrance(const CVector* camera, const CVector* centre) {
    const float hundred = 100.0f;
    const float dx = camera->x - centre->x;
    const float dy = camera->y - centre->y;
    return dy * dy + dx * dx < hundred;
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
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
CObject* Interior_c::Bedroom_AddTableItem(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6, int32 a7) {
    float f1{};
    float f2{};
    Furniture_c* furniture3{};
    CObject* object4{};
    f1 = float(a5);
    f2 = float(a6);
    if ((a4 == 0) || (a4 == 2)) {
        f1 = (f1 + 0.5f);
    } else if ((a4 == 1) || (a4 == 3)) {
        f2 = (f2 + 0.5f);
    }
    furniture3 = g_furnitureMan.GetFurniture(a2, a3, -1, InteriorField<uint8>(m_box, 0x1F));
    object4 = PlaceObject(1, furniture3, (f1 + 0.5f), (f2 + 0.5f), 0.5f, IntTimesFloatCpp(a7, 90.0f));
    return object4;
}

// 0x593FC0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::FurnishBedroom() {
    CEntity* entity18{};
    CEntity* entity29{};
    CEntity* entity36{};
    CEntity* entity38{};
    CEntity* entity5{};
    CEntity* entity6{};
    CEntity* entity8{};
    Furniture_c* furniture49{};
    Furniture_c* furniture53{};
    Furniture_c* furniture57{};
    Furniture_c* furniture61{};
    Furniture_c* furniture65{};
    Furniture_c* furniture66{};
    Furniture_c* furniture68{};
    int32 n1{};
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n19{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n25{};
    int32 n26{};
    int32 n27{};
    int32 n28{};
    int32 n3{};
    int32 n30{};
    int32 n31{};
    int32 n32{};
    int32 n33{};
    int32 n34{};
    int32 n35{};
    int32 n37{};
    int32 n39{};
    int32 n4{};
    int32 n40{};
    int32 n41{};
    int32 n42{};
    int32 n43{};
    int32 n44{};
    int32 n45{};
    int32 n46{};
    int32 n47{};
    int32 n50{};
    int32 n51{};
    int32 n54{};
    int32 n55{};
    int32 n58{};
    int32 n59{};
    int32 n62{};
    int32 n63{};
    int32 n69{};
    int32 n7{};
    int32 n70{};
    int32 n71{};
    int32 n72{};
    int32 n73{};
    int32 n74{};
    int32 n9{};
    bool ok48{};
    bool ok52{};
    bool ok56{};
    bool ok60{};
    bool ok64{};
    uintptr_t p2{};
    uintptr_t p67{};
    n1 = g_furnitureMan.GetRandomId(3, 1, InteriorField<uint8>(m_box, 0x1F));
    p2 = reinterpret_cast<uintptr_t>(m_box);
    m_furnitureId = uint8(n1);
    SetTilesStatus((InteriorField<int8>(p2, 0x5) - 1), 0, 2, 2, 7, 0);
    entity5 = PlaceFurnitureOnWall(3, 0, -1, 0.0f, 1, -1, -1, 0, &n3, &n4, nullptr, nullptr, nullptr, nullptr);
    entity6 = entity5;
    n7 = n4;
    if (n7 > 0) {
        entity8 = PlaceFurnitureOnWall(3, 1, m_furnitureId, 0.0f, 1, n3, (n7 - 1), 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        if (entity8 != nullptr) {
            n9 = n3;
            if (n9 != 1) {
                if (n9 != 3) {
                    if (n9 != 0) {
                        if (n9 != 2) {
                            n10 = n11;
                            n12 = n11;
                            n13 = n11;
                            n14 = n11;
                        } else {
                            n13 = (n4 - 1);
                            n15 = 1;
                            n12 = 1;
                            n10 = 4;
                            n11 = n13;
                            n14 = 2;
                        }
                    } else {
                        n16 = InteriorField<uint8>(m_box, 0x3);
                        n13 = (n4 - 1);
                        n15 = (n16 - 2);
                        n12 = 1;
                        n10 = 3;
                        n11 = n13;
                        n14 = (n16 - 3);
                    }
                } else {
                    n17 = InteriorField<uint8>(m_box, 0x2);
                    n14 = (n4 - 1);
                    n11 = (n17 - 2);
                    n10 = 4;
                    n15 = n14;
                    n12 = 2;
                    n13 = (n17 - 3);
                }
            } else {
                n11 = 1;
                n14 = (n4 - 1);
                n12 = 2;
                n10 = 3;
                n15 = n14;
                n13 = n12;
            }
            AddInteriorInfo(n10, float(n11), float(n15), n12, entity6);
            SetTilesStatus(n13, n14, 1, 1, 2, 0);
        }
    }
    entity18 = PlaceFurnitureOnWall(3, 1, m_furnitureId, 0.0f, 1, n3, (n4 + 2), 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    if (entity18 != nullptr) {
        n19 = n3;
        if (n19 != 1) {
            if (n19 != 3) {
                if (n19 != 0) {
                    if (n19 != 2) {
                        n19 = n11;
                        n20 = n11;
                        n21 = n11;
                        n22 = n11;
                    } else {
                        n19 = 3;
                        n21 = (n4 + 2);
                        n11 = 1;
                        n15 = n21;
                        n20 = n19;
                        n22 = 2;
                    }
                } else {
                    n23 = InteriorField<uint8>(m_box, 0x3);
                    n21 = (n4 + 2);
                    n11 = (n23 - 2);
                    n19 = 4;
                    n15 = n21;
                    n20 = 3;
                    n22 = (n23 - 3);
                }
            } else {
                n24 = InteriorField<uint8>(m_box, 0x2);
                n22 = (n4 + 2);
                n15 = (n24 - 2);
                n11 = n22;
                n20 = 0;
                n21 = (n24 - 3);
            }
        } else {
            n15 = 1;
            n22 = (n4 + 2);
            n19 = 4;
            n11 = n22;
            n20 = 0;
            n21 = 2;
        }
        AddInteriorInfo(n19, float(n15), float(n11), n20, entity6);
        SetTilesStatus(n21, n22, 1, 1, 2, 0);
    }
    PlaceFurnitureOnWall(3, 3, m_furnitureId, 0.0f, 1, -1, -1, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    PlaceFurnitureOnWall(3, 2, m_furnitureId, 0.0f, 1, -1, -1, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    n25 = InteriorRandCpp();
    n11 = (n25 & 65535);
    n26 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 100.0f));
    if (n26 >= 25) {
        if (n26 < 50) {
            entity29 = PlaceFurnitureOnWall(2, 6, -1, 0.0f, 1, -1, -1, 0, &n3, nullptr, &n27, &n28, nullptr, nullptr);
            if (entity29 != nullptr) {
                n30 = n3;
                n31 = n30;
                n32 = n28;
                n33 = n27;
                n34 = n30;
                n35 = 3;
                Bedroom_AddTableItem(2, n35, n34, n33, n32, n31);
            }
        } else {
            if (n26 < 75) {
                entity36 = PlaceFurnitureOnWall(2, 6, -1, 0.0f, 1, -1, -1, 0, &n3, nullptr, &n27, &n28, nullptr, nullptr);
                if (entity36 != nullptr) {
                    n37 = n3;
                    Bedroom_AddTableItem(2, 8, n37, n27, n28, n37);
                }
                entity29 = PlaceFurnitureOnWall(2, 6, -1, 0.0f, 1, -1, -1, 0, &n3, nullptr, &n27, &n28, nullptr, nullptr);
                if (entity29 != nullptr) {
                    n30 = n3;
                    n31 = n30;
                    n32 = n28;
                    n33 = n27;
                    n34 = n30;
                    n35 = 3;
                    Bedroom_AddTableItem(2, n35, n34, n33, n32, n31);
                }
            }
        }
    } else {
        entity38 = PlaceFurnitureOnWall(2, 6, -1, 0.0f, 1, -1, -1, 0, &n3, nullptr, &n27, &n28, nullptr, nullptr);
        if (entity38 != nullptr) {
            n39 = n3;
            n31 = n39;
            n32 = n28;
            n33 = n27;
            n34 = n39;
            n35 = 8;
            Bedroom_AddTableItem(2, n35, n34, n33, n32, n31);
        }
    }
    n40 = InteriorField<uint8>(m_box, 0x1F);
    if (uint32(uint8(n40)) < uint32(75)) {
        if (uint32(uint8(n40)) < uint32(50)) {
            n41 = CGeneral::GetRandomIntInRange(50, 100);
            n42 = n41;
        } else {
            n43 = InteriorRandCpp();
            n11 = (n43 & 65535);
            n44 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * -30.0f));
            n42 = (20 - n44);
        }
    } else {
        n45 = InteriorRandCpp();
        n11 = (n45 & 65535);
        n41 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 20.0f));
        n42 = n41;
    }
    n46 = InteriorRandCpp();
    n11 = (n46 & 65535);
    n47 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 60.0f));
    if (n47 < n42) {
        ok48 = FindEmptyTiles(2, 2, &n15, &n11);
        if (ok48) {
            furniture49 = g_furnitureMan.GetFurniture(8, 2, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture49, FloatPlusIntCpp(0.5f, n15), FloatPlusIntCpp(0.5f, n11), 0.05f, 0.0f);
            SetTilesStatus(n15, n11, 2, 2, 2, 0);
        }
    }
    n50 = InteriorRandCpp();
    n11 = (n50 & 65535);
    n51 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 100.0f));
    if (n51 < n42) {
        ok52 = FindEmptyTiles(1, 1, &n15, &n11);
        if (ok52) {
            furniture53 = g_furnitureMan.GetFurniture(8, 5, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture53, FloatPlusIntCpp(0.5f, n15), FloatPlusIntCpp(0.5f, n11), 0.05f, 0.0f);
            SetTilesStatus(n15, n11, 1, 1, 2, 0);
        }
    }
    n54 = InteriorRandCpp();
    n11 = (n54 & 65535);
    n55 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 100.0f));
    if (n55 < n42) {
        ok56 = FindEmptyTiles(1, 1, &n15, &n11);
        if (ok56) {
            furniture57 = g_furnitureMan.GetFurniture(8, 4, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture57, FloatPlusIntCpp(0.5f, n15), FloatPlusIntCpp(0.5f, n11), 0.05f, 0.0f);
            SetTilesStatus(n15, n11, 1, 1, 2, 0);
        }
    }
    n58 = InteriorRandCpp();
    n11 = (n58 & 65535);
    n59 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 100.0f));
    if (n59 < n42) {
        ok60 = FindEmptyTiles(1, 1, &n15, &n11);
        if (ok60) {
            furniture61 = g_furnitureMan.GetFurniture(8, 3, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture61, FloatPlusIntCpp(0.5f, n15), FloatPlusIntCpp(0.5f, n11), 0.05f, 0.0f);
            SetTilesStatus(n15, n11, 1, 1, 2, 0);
        }
    }
    n62 = InteriorRandCpp();
    n11 = (n62 & 65535);
    n63 = int32((IntTimesFloatCpp(n11, 0.000030517578f) * 100.0f));
    if (n63 < n42) {
        ok64 = FindEmptyTiles(2, 2, &n15, &n11);
        if (ok64) {
            furniture65 = g_furnitureMan.GetFurniture(8, 6, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture65, FloatPlusIntCpp(0.5f, n15), FloatPlusIntCpp(0.5f, n11), 0.05f, 0.0f);
            SetTilesStatus(n15, n11, 1, 1, 2, 0);
        }
    }
    furniture66 = g_furnitureMan.GetFurniture(8, 1, -1, InteriorField<uint8>(m_box, 0x1F));
    p67 = reinterpret_cast<uintptr_t>(m_box);
    furniture68 = furniture66;
    n11 = InteriorField<uint8>(p67, 0x3);
    n69 = n11;
    n11 = InteriorField<uint8>(furniture68, 0xD);
    n70 = int32((IntTimesFloatCpp(n69, 0.5f) - IntTimesFloatCpp(n11, 0.5f)));
    n11 = InteriorField<uint8>(p67, 0x2);
    n71 = n11;
    n11 = InteriorField<uint8>(furniture68, 0xC);
    n72 = int32((IntTimesFloatCpp(n71, 0.5f) - IntTimesFloatCpp(n11, 0.5f)));
    PlaceFurniture(furniture68, n72, n70, 0.0f, 0, 0, &n73, &n74, 0);
    return;
}

// 0x596930
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
CObject* Interior_c::Kitchen_FurnishEdges() {
    CEntity* entity12{};
    CEntity* entity13{};
    CEntity* entity34{};
    CEntity* entity41{};
    CEntity* entity46{};
    CEntity* entity53{};
    float f60{};
    float f61{};
    float f62{};
    float f64{};
    float f71{};
    Furniture_c* furniture29{};
    Furniture_c* furniture30{};
    Furniture_c* furniture35{};
    Furniture_c* furniture36{};
    Furniture_c* furniture56{};
    Furniture_c* furniture57{};
    Furniture_c* furniture8{};
    Furniture_c* furniture89{};
    Furniture_c* furniture9{};
    Furniture_c* furniture91{};
    Furniture_c* furniture92{};
    int32 n10{};
    int32 n11{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n25{};
    int32 n26{};
    int32 n27{};
    int32 n28{};
    int32 n3{};
    int32 n31{};
    int32 n32{};
    int32 n33{};
    int32 n37{};
    int32 n38{};
    int32 n39{};
    int32 n4{};
    int32 n40{};
    int32 n42{};
    int32 n43{};
    int32 n44{};
    int32 n45{};
    int32 n47{};
    int32 n48{};
    int32 n49{};
    int32 n5{};
    int32 n50{};
    int32 n51{};
    int32 n52{};
    int32 n54{};
    int32 n55{};
    int32 n58{};
    int32 n6{};
    int32 n65{};
    int32 n66{};
    int32 n67{};
    int32 n68{};
    int32 n69{};
    int32 n7{};
    int32 n70{};
    int32 n72{};
    int32 n73{};
    int32 n74{};
    int32 n75{};
    int32 n76{};
    int32 n77{};
    int32 n78{};
    int32 n79{};
    int32 n80{};
    int32 n81{};
    int32 n82{};
    int32 n83{};
    int32 n84{};
    int32 n85{};
    int32 n86{};
    int32 n87{};
    int32 n88{};
    int32 n90{};
    CObject* object63{};
    uintptr_t p1{};
    uintptr_t p14{};
    uintptr_t p59{};
    float fr[130]{};
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = (InteriorField<uint8>(p1, 0x2) - 1);
    n3 = (InteriorField<uint8>(p1, 0x3) - 1);
    n4 = n2;
    n5 = n3;
    n6 = 0;
    n7 = 0;
    furniture8 = g_furnitureMan.GetFurniture(4, 7, int16(m_furnitureId), InteriorField<uint8>(p1, 0x1F));
    furniture9 = furniture8;
    entity12 = PlaceFurniture(furniture9, 0, n3, 0.0f, 1, 1, &n10, &n11, 0);
    if (entity12 != nullptr) {
        n6 = 1;
    }
    entity13 = PlaceFurniture(furniture9, n2, n3, 0.0f, 1, 0, &n10, &n11, 0);
    if (entity13 != nullptr) {
        n7 = 1;
    }
    p14 = reinterpret_cast<uintptr_t>(m_box);
    n15 = InteriorField<uint8>(p14, 0x7);
    n16 = InteriorField<uint8>(p14, 0xD);
    if (int8(uint8(n15)) <= int8(uint8(n16))) {
        n15 = uint8(n16);
    }
    n17 = InteriorField<uint8>(p14, 0xF);
    n18 = int8(uint8(n15));
    n19 = ((int32((n18 <= 0)) - 1) & n18);
    n20 = InteriorField<uint8>(p14, 0x9);
    if (int8(uint8(n20)) <= int8(uint8(n17))) {
        n20 = uint8(n17);
    }
    n21 = int8(uint8(n20));
    n22 = (int32((n21 <= 0)) - 1);
    n23 = (n22 & n21);
    if ((n22 & n21) <= 0) {
        if (n19 <= 0) {
            n24 = CGeneral::GetRandomIntInRange(0, n3);
            n25 = n24;
            n26 = CGeneral::GetRandomIntInRange(0, n3);
            n27 = n26;
        } else {
            n25 = n19;
            n27 = 0;
        }
        if (n25 > 0) {
            SetTilesStatus(0, 0, 1, n25, 2, 0);
        }
    } else {
        n27 = n23;
        if (n19 <= 0) {
            n25 = 0;
        } else {
            n25 = n19;
            if (n25 > 0) {
                SetTilesStatus(0, 0, 1, n25, 2, 0);
            }
        }
    }
    n28 = n27;
    if (n28 > 0) {
        SetTilesStatus(n2, 0, 1, n28, 2, 0);
    }
    furniture29 = g_furnitureMan.GetFurniture(4, 0, int16(m_furnitureId), InteriorField<uint8>(m_box, 0x1F));
    PlaceFurniture(furniture29, 0, n25, 0.0f, 1, 1, &n10, &n11, 0);
    furniture30 = g_furnitureMan.GetFurniture(4, 2, int16(m_furnitureId), InteriorField<uint8>(m_box, 0x1F));
    PlaceFurniture(furniture30, n2, n28, 0.0f, 1, 3, &n10, &n11, 0);
    n31 = InteriorField<uint8>(m_box, 0x10);
    n32 = -1;
    if (uint8(n31) != 255) {
        n32 = int8(uint8(n31));
    }
    entity34 = PlaceFurnitureOnWall(4, 3, m_furnitureId, 0.0f, 1, 0, n32, 0, reinterpret_cast<int32*>(&fr[0]), &n33, nullptr, nullptr, nullptr, nullptr);
    if (entity34 != nullptr) {
        n33 = (n33 + 1);
        AddInteriorInfo(5, float(n33), FloatPlusIntCpp(-1.5f, InteriorField<uint8>(m_box, 0x3)), 2, nullptr);
    }
    PlaceFurnitureOnWall(4, 5, m_furnitureId, 0.0f, 1, -1, -1, 0, reinterpret_cast<int32*>(&fr[0]), &n33, nullptr, nullptr, nullptr, nullptr);
    PlaceFurnitureOnWall(4, 4, m_furnitureId, 0.0f, 1, -1, -1, 0, reinterpret_cast<int32*>(&fr[0]), &n33, nullptr, nullptr, nullptr, nullptr);
    PlaceFurnitureOnWall(4, 6, m_furnitureId, 0.0f, 1, -1, -1, 0, reinterpret_cast<int32*>(&fr[0]), &n33, nullptr, nullptr, nullptr, nullptr);
    furniture35 = g_furnitureMan.GetFurniture(4, 1, int16(m_furnitureId), InteriorField<uint8>(m_box, 0x1F));
    furniture36 = furniture35;
    for (int32 i = 0; i < 32; ++i) fr[97 + i] = 0.0f;
    n37 = (n25 + 1);
    n38 = n5;
    n39 = 0;
    n40 = n37;
    if (n37 < n38) {
        do {
            entity41 = PlaceFurniture(furniture36, 0, n37, 0.0f, 1, 1, &n10, &n11, 0);
            if (entity41 != nullptr) {
                n42 = n39;
                fr[n42 + 33] = 0.5f;
                fr[n42 + 65] = 90.0f;
                n43 = (n42 + 1);
                n39 = n43;
                fr[n43] = FloatPlusIntCpp(0.5f, n40);
            }
            n37 = (n37 + 1);
            n40 = n37;
        } while (n37 < n38);
    }
    n44 = 1;
    n45 = n44;
    if (n4 > n44) {
        do {
            entity46 = PlaceFurniture(furniture36, n44, n38, 0.0f, 1, 0, &n10, &n11, 0);
            if (entity46 != nullptr) {
                n47 = n39;
                fr[n47 + 65] = 0.0f;
                n48 = (n47 + 1);
                n39 = n48;
                fr[n48 + 32] = FloatPlusIntCpp(0.5f, n45);
                fr[n48] = FloatPlusIntCpp(0.5f, n5);
            }
            n44 = (n44 + 1);
            n45 = n44;
        } while (n44 < n4);
    }
    n49 = (n38 - 1);
    n50 = (n27 + 1);
    n51 = n50;
    if (n50 <= n49) {
        n52 = n50;
        do {
            entity53 = PlaceFurniture(furniture36, n4, n52, 0.0f, 1, 3, &n10, &n11, 0);
            if (entity53 != nullptr) {
                n54 = n39;
                fr[n54 + 65] = 270.0f;
                n55 = (n54 + 1);
                n39 = n55;
                fr[n55 + 32] = FloatPlusIntCpp(0.5f, n4);
                fr[n55] = FloatPlusIntCpp(0.5f, n51);
            }
            n52 = (n52 + 1);
            n51 = n52;
        } while (n52 <= n49);
    }
    furniture56 = g_furnitureMan.GetFurniture(4, 10, -1, InteriorField<uint8>(m_box, 0x1F));
    furniture57 = furniture56;
    n58 = InteriorRandCpp();
    if (n58 >= 16383) {
        if (uint8(n7) != 0) {
            p59 = reinterpret_cast<uintptr_t>(m_box);
            f60 = 315.0f;
            f61 = FloatPlusIntCpp(-0.5f, InteriorField<uint8>(p59, 0x3));
            f62 = FloatPlusIntCpp(-0.5f, InteriorField<uint8>(p59, 0x2));
            PlaceObject(1, furniture57, f62, f61, 1.05f, f60);
        }
    } else {
        n58 = ((n58 & (-256)) | uint8(n6));
        if (uint8(n58) == 0) {
            if (uint8(n7) != 0) {
                p59 = reinterpret_cast<uintptr_t>(m_box);
                f60 = 315.0f;
                f61 = FloatPlusIntCpp(-0.5f, InteriorField<uint8>(p59, 0x3));
                f62 = FloatPlusIntCpp(-0.5f, InteriorField<uint8>(p59, 0x2));
                PlaceObject(1, furniture57, f62, f61, 1.05f, f60);
            }
        } else {
            f60 = 45.0f;
            f61 = FloatPlusIntCpp(-0.5f, InteriorField<uint8>(m_box, 0x3));
            f62 = 0.5f;
            PlaceObject(1, furniture57, f62, f61, 1.05f, f60);
        }
    }
    object63 = reinterpret_cast<CObject*>(n39);
    if (int32(reinterpret_cast<uintptr_t>(object63)) > 0) {
        f64 = float(n39);
        n65 = InteriorRandCpp();
        n66 = int32((IntTimesFloatCpp((n65 & 65535), 0.000030517578f) * f64));
        n67 = n66;
        fr[n66 + 97] = 1.0f;
        n68 = InteriorRandCpp();
        n69 = int32((IntTimesFloatCpp((n68 & 65535), 0.000030517578f) * f64));
        n70 = n69;
        f71 = fr[n70 + 97];
        n72 = 0;
        if (InteriorFloatBits(f71) == 0x3F800000) {
            for (;;) {
                if (n72 >= 30) {
                    break;
                }
                n73 = InteriorRandCpp();
                n74 = int32((IntTimesFloatCpp((n73 & 65535), 0.000030517578f) * f64));
                n70 = n74;
                f71 = fr[n70 + 97];
                n72 = (n72 + 1);
                if (InteriorFloatBits(f71) == 0x3F800000) {
                    continue;
                }
                break;
            }
            if (n72 == 30) {
                n70 = -1;
            }
        }
        n75 = InteriorField<uint8>(m_box, 0x1F);
        if (uint32(uint8(n75)) < uint32(75)) {
            if (uint32(uint8(n75)) < uint32(50)) {
                n76 = InteriorRandCpp();
                n77 = int32((IntTimesFloatCpp((n76 & 65535), 0.000030517578f) * -50.0f));
                n78 = 50;
            } else {
                n79 = InteriorRandCpp();
                n77 = int32((IntTimesFloatCpp((n79 & 65535), 0.000030517578f) * -30.0f));
                n78 = 20;
            }
            n80 = (n78 - n77);
        } else {
            n81 = InteriorRandCpp();
            n82 = int32((IntTimesFloatCpp((n81 & 65535), 0.000030517578f) * 20.0f));
            n80 = n82;
        }
        object63 = reinterpret_cast<CObject*>(n39);
        n83 = 0;
        if (int32(reinterpret_cast<uintptr_t>(object63)) > 0) {
            do {
                if (!((fr[n83 + 97] != 0.0f))) {
                    n84 = InteriorRandCpp();
                    n85 = int32((IntTimesFloatCpp((n84 & 65535), 0.000030517578f) * 100.0f));
                    if (n85 < n80) {
                        n86 = InteriorRandCpp();
                        if (n86 >= 16383) {
                            n87 = InteriorField<uint8>(m_box, 0x1F);
                            n88 = 4;
                        } else {
                            n87 = InteriorField<uint8>(m_box, 0x1F);
                            n88 = 3;
                        }
                        furniture89 = g_furnitureMan.GetFurniture(8, n88, -1, uint8(n87));
                        if (furniture89 != nullptr) {
                            PlaceObject(0, furniture89, fr[n83 + 33], fr[n83 + 1], 1.05f, fr[n83 + 65]);
                        }
                    }
                }
                object63 = reinterpret_cast<CObject*>(n39);
                n83 = (n83 + 1);
            } while (n83 < int32(reinterpret_cast<uintptr_t>(object63)));
        }
        n90 = n67;
        if (n90 != (-1)) {
            furniture91 = g_furnitureMan.GetFurniture(4, 8, -1, InteriorField<uint8>(m_box, 0x1F));
            object63 = PlaceObject(1, furniture91, fr[n90 + 33], fr[n90 + 1], 1.05f, fr[n90 + 65]);
        }
        if (n70 != (-1)) {
            furniture92 = g_furnitureMan.GetFurniture(4, 9, -1, InteriorField<uint8>(m_box, 0x1F));
            object63 = PlaceObject(1, furniture92, fr[n70 + 33], fr[n70 + 1], 1.05f, fr[n70 + 65]);
        }
    }
    return object63;
}

// 0x5970B0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::FurnishKitchen() {
    Furniture_c* furniture7{};
    Furniture_c* furniture9{};
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n2{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    uintptr_t p1{};
    uintptr_t p8{};
    SetTilesStatus((InteriorField<int8>(m_box, 0x5) - 1), 0, 2, 1, 7, 0);
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = (InteriorField<uint8>(p1, 0x2) - 2);
    n3 = 1;
    n4 = (InteriorField<uint8>(p1, 0x3) - 2);
    if (n2 >= n3) {
        do {
            SetTilesStatus(n3, n4, 1, 1, 3, 0);
            SetTilesStatus(n3, 0, 1, 1, 3, 0);
            n3 = (n3 + 1);
        } while (n3 <= n2);
    }
    n5 = 0;
    if (n4 >= 0) {
        do {
            SetTilesStatus(1, n5, 1, 1, 3, 0);
            SetTilesStatus(n2, n5, 1, 1, 3, 0);
            n5 = (n5 + 1);
        } while (n5 <= n4);
    }
    AddGotoPt(1, 1, 0.0f, 0.0f);
    AddGotoPt(1, n4, 0.0f, 0.0f);
    AddGotoPt(n2, 1, 0.0f, 0.0f);
    AddGotoPt(n2, n4, 0.0f, 0.0f);
    n6 = g_furnitureMan.GetRandomId(4, 0, InteriorField<uint8>(m_box, 0x1F));
    m_furnitureId = uint8(n6);
    Kitchen_FurnishEdges();
    furniture7 = g_furnitureMan.GetFurniture(8, 1, -1, InteriorField<uint8>(m_box, 0x1F));
    p8 = reinterpret_cast<uintptr_t>(m_box);
    furniture9 = furniture7;
    n10 = int32((IntTimesFloatCpp(InteriorField<uint8>(p8, 0x3), 0.5f) - IntTimesFloatCpp(InteriorField<uint8>(furniture9, 0xD), 0.5f)));
    n11 = int32((IntTimesFloatCpp(InteriorField<uint8>(p8, 0x2), 0.5f) - IntTimesFloatCpp(InteriorField<uint8>(furniture9, 0xC), 0.5f)));
    PlaceFurniture(furniture9, n11, n10, 0.0f, 0, 0, &n12, &n13, 0);
    return;
}

// 0x597240
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
CObject* Interior_c::Lounge_AddTV(int32 a2, int32 a3, int32 a4, int32 a5) {
    float f10{};
    float f12{};
    float f17{};
    float f6{};
    float f7{};
    float f9{};
    Furniture_c* furniture16{};
    Furniture_c* furniture22{};
    int32 n11{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n18{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n4{};
    int32 n5{};
    int32 n8{};
    CObject* object23{};
    uintptr_t p19{};
    uintptr_t p3{};
    float f1 = InteriorBitsFloat(uint32(a2));
    n2 = (a2 & 3);
    if (a2 != 0) {
        if (a2 != 2) {
            if (a2 != 1) {
                if (a2 == 3) {
                    p3 = reinterpret_cast<uintptr_t>(m_box);
                    n4 = InteriorField<uint8>(p3, 0x2);
                    n5 = InteriorField<uint8>(p3, 0x3);
                    f6 = FloatPlusIntCpp(-0.5f, n4);
                    f7 = FloatPlusIntCpp(-0.5f, n4);
                    n8 = n5;
                    f1 = FloatPlusIntCpp(-0.5f, n5);
                    f9 = (FloatPlusIntCpp(-0.5f, n8) - 1.0f);
                    f10 = FloatPlusIntCpp(-2.0f, n8);
                    n11 = n4;
                    f12 = FloatPlusIntCpp(-2.0f, n11);
                    AddInteriorInfo(0, f12, f10, -1, nullptr);
                }
            } else {
                f10 = 1.0f;
                f6 = 0.5f;
                f7 = 0.5f;
                f1 = 0.5f;
                f9 = 1.5f;
                f12 = 1.0f;
                AddInteriorInfo(0, f12, f10, -1, nullptr);
            }
        } else {
            f10 = 1.0f;
            n13 = InteriorField<uint8>(m_box, 0x2);
            f1 = 0.5f;
            f9 = 0.5f;
            f6 = FloatPlusIntCpp(-0.5f, n13);
            f7 = (f6 - 1.0f);
            n11 = n13;
            f12 = FloatPlusIntCpp(-2.0f, n11);
            AddInteriorInfo(0, f12, f10, -1, nullptr);
        }
    } else {
        n14 = InteriorField<uint8>(m_box, 0x3);
        f6 = 0.5f;
        f7 = 1.5f;
        n15 = n14;
        f1 = FloatPlusIntCpp(-0.5f, n14);
        f9 = FloatPlusIntCpp(-0.5f, n15);
        f10 = FloatPlusIntCpp(-2.0f, n15);
        f12 = 1.0f;
        AddInteriorInfo(0, f12, f10, -1, nullptr);
    }
    furniture16 = g_furnitureMan.GetFurniture(2, 3, -1, InteriorField<uint8>(m_box, 0x1F));
    f17 = IntTimesFloatCpp(n2, 90.0f);
    PlaceObject(1, furniture16, f6, f1, 0.5f, (IntTimesFloatCpp(n2, 90.0f) + 45.0f));
    n18 = InteriorRandCpp();
    p19 = reinterpret_cast<uintptr_t>(m_box);
    if (n18 >= 16383) {
        n20 = InteriorField<uint8>(p19, 0x1F);
        n21 = 9;
    } else {
        n20 = InteriorField<uint8>(p19, 0x1F);
        n21 = 7;
    }
    furniture22 = g_furnitureMan.GetFurniture(2, n21, -1, uint8(n20));
    object23 = PlaceObject(1, furniture22, f7, f9, 0.5f, f17);
    return object23;
}

// 0x597430
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
CObject* Interior_c::Lounge_AddHifi(int32 a2, int32 a3, int32 a4, int32 a5) {
    float f1{};
    float f3{};
    Furniture_c* furniture4{};
    int32 n2{};
    CObject* object5{};
    f1 = float(a3);
    n2 = (a2 & 3);
    f3 = float(a4);
    if ((a2 == 0) || (a2 == 2)) {
        f1 = (f1 + 0.5f);
    } else if ((a2 == 1) || (a2 == 3)) {
        f3 = (f3 + 0.5f);
    }
    furniture4 = g_furnitureMan.GetFurniture(2, 8, -1, InteriorField<uint8>(m_box, 0x1F));
    object5 = PlaceObject(1, furniture4, (f1 + 0.5f), (f3 + 0.5f), 0.5f, IntTimesFloatCpp(n2, 90.0f));
    return object5;
}

// 0x5974E0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Lounge_AddChairInfo(int32 a2, int32 a3, CEntity* entityIgnoredCollision) {
    int32 n1{};
    n1 = ((a2 - 2) & 3);
    if (a2 != 0) {
        if (a2 != 2) {
            if (a2 != 1) {
                if (a2 == 3) {
                    AddInteriorInfo(1, FloatPlusIntCpp(-1.0f, (InteriorField<uint8>(m_box, 0x2) - 1)), FloatPlusIntCpp(0.5f, a3), n1, entityIgnoredCollision);
                }
                return;
            }
            AddInteriorInfo(1, 1.0f, FloatPlusIntCpp(0.5f, a3), n1, entityIgnoredCollision);
            return;
        } else {
            AddInteriorInfo(1, FloatPlusIntCpp(0.5f, a3), 1.0f, n1, entityIgnoredCollision);
            return;
        }
    } else {
        AddInteriorInfo(1, FloatPlusIntCpp(0.5f, a3), FloatPlusIntCpp(-1.0f, (InteriorField<uint8>(m_box, 0x3) - 1)), n1, entityIgnoredCollision);
        return;
    }
}

// 0x5975C0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Lounge_AddSofaInfo(int32 sitType, int32 offsetX, CEntity* entityIgnoredCollision) {
    float f2{};
    float f3{};
    float f4{};
    float f5{};
    float f6{};
    float f7{};
    int32 n1{};
    n1 = ((sitType - 2) & 3);
    if (sitType != 0) {
        if (sitType != 2) {
            if (sitType != 1) {
                if (sitType == 3) {
                    f2 = FloatPlusIntCpp(-1.0f, (InteriorField<uint8>(m_box, 0x2) - 1));
                    f3 = FloatPlusIntCpp(0.5f, offsetX);
                    AddInteriorInfo(1, f2, f3, n1, entityIgnoredCollision);
                    AddInteriorInfo(1, f2, (f3 + 1.0f), n1, entityIgnoredCollision);
                }
                return;
            }
            f4 = FloatPlusIntCpp(0.5f, offsetX);
            AddInteriorInfo(sitType, 1.0f, f4, n1, entityIgnoredCollision);
            AddInteriorInfo(1, 1.0f, (f4 + 1.0f), n1, entityIgnoredCollision);
            return;
        } else {
            f5 = FloatPlusIntCpp(0.5f, offsetX);
            AddInteriorInfo(1, f5, 1.0f, n1, entityIgnoredCollision);
            AddInteriorInfo(1, (f5 + 1.0f), 1.0f, n1, entityIgnoredCollision);
            return;
        }
    } else {
        f6 = FloatPlusIntCpp(0.5f, offsetX);
        f7 = FloatPlusIntCpp(-1.0f, (InteriorField<uint8>(m_box, 0x3) - 1));
        AddInteriorInfo(1, f6, f7, n1, entityIgnoredCollision);
        AddInteriorInfo(1, (f6 + 1.0f), f7, n1, entityIgnoredCollision);
        return;
    }
}

// 0x597740
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::FurnishLounge() {
    CEntity* entity17{};
    CEntity* entity18{};
    CEntity* entity19{};
    CEntity* entity20{};
    CEntity* entity21{};
    CEntity* entity25{};
    CEntity* entity9{};
    Furniture_c* furniture37{};
    Furniture_c* furniture41{};
    Furniture_c* furniture45{};
    Furniture_c* furniture46{};
    Furniture_c* furniture48{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n2{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n26{};
    int32 n27{};
    int32 n28{};
    int32 n29{};
    int32 n3{};
    int32 n30{};
    int32 n31{};
    int32 n32{};
    int32 n33{};
    int32 n34{};
    int32 n35{};
    int32 n38{};
    int32 n39{};
    int32 n4{};
    int32 n42{};
    int32 n43{};
    int32 n49{};
    int32 n5{};
    int32 n50{};
    int32 n51{};
    int32 n52{};
    int32 n53{};
    int32 n54{};
    int32 n55{};
    int32 n56{};
    int32 n57{};
    int32 n58{};
    int32 n59{};
    int32 n6{};
    int32 n60{};
    int32 n61{};
    int32 n62{};
    int32 n7{};
    int32 n8{};
    bool ok36{};
    bool ok40{};
    bool ok44{};
    uintptr_t p1{};
    uintptr_t p10{};
    uintptr_t p47{};
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = ((InteriorField<uint8>(p1, 0x3) + InteriorField<uint8>(p1, 0x2)) << 1);
    SetTilesStatus((InteriorField<int8>(p1, 0x5) - 1), 0, 2, 1, 7, 0);
    SetTilesStatus((InteriorField<int8>(m_box, 0x5) - 2), 0, 1, 1, 2, 0);
    SetTilesStatus((InteriorField<int8>(m_box, 0x5) + 1), 0, 1, 1, 2, 0);
    n3 = -1;
    entity9 = PlaceFurnitureInCorner(2, 2, -1, 0.0f, 1, -1, 0, &n4, &n5, &n6, &n7, &n8);
    if (entity9 != nullptr) {
        Lounge_AddTV(n4, n5, n6, 0);
        n3 = n4;
    }
    SetCornerTiles(n4, 2, 2, 1);
    p10 = reinterpret_cast<uintptr_t>(m_box);
    n11 = (InteriorField<uint8>(p10, 0x2) - 2);
    n12 = 1;
    n13 = (InteriorField<uint8>(p10, 0x3) - 2);
    if (n11 >= n12) {
        do {
            SetTilesStatus(n12, n13, 1, 1, 3, 0);
            SetTilesStatus(n12, 1, 1, 1, 3, 0);
            n12 = (n12 + 1);
        } while (n12 <= n11);
    }
    n14 = 1;
    if (n13 >= n14) {
        do {
            SetTilesStatus(1, n14, 1, 1, 3, 0);
            SetTilesStatus(n11, n14, 1, 1, 3, 0);
            n14 = (n14 + 1);
        } while (n14 <= n13);
    }
    AddGotoPt(1, 1, 0.0f, 0.0f);
    AddGotoPt(1, n13, 0.0f, 0.0f);
    AddGotoPt(n11, 1, 0.0f, 0.0f);
    AddGotoPt(n11, n13, 0.0f, 0.0f);
    SetCornerTiles(0, 2, 2, 0);
    SetCornerTiles(2, 2, 2, 0);
    SetCornerTiles(1, 2, 2, 0);
    SetCornerTiles(3, 2, 2, 0);
    n15 = g_furnitureMan.GetRandomId(2, 0, InteriorField<uint8>(m_box, 0x1F));
    m_furnitureId = uint8(n15);
    entity17 = PlaceFurnitureOnWall(2, 0, int8(uint8(n15)), 0.0f, 1, -1, -1, 0, &n4, &n16, nullptr, nullptr, nullptr, nullptr);
    if (entity17 != nullptr) {
        Lounge_AddSofaInfo(n4, n16, entity17);
        entity18 = PlaceFurnitureOnWall(2, 4, -1, 0.0f, 1, n4, n16, 2, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        if (entity18 == nullptr) {
            PlaceFurnitureOnWall(2, 4, -1, 0.0f, 1, n4, n16, 3, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
        }
    }
    entity19 = PlaceFurnitureOnWall(2, 1, m_furnitureId, 0.0f, 1, -1, -1, 0, &n4, &n16, nullptr, nullptr, nullptr, nullptr);
    if (entity19 != nullptr) {
        Lounge_AddChairInfo(n4, n16, entity19);
    }
    entity20 = PlaceFurnitureOnWall(2, 1, m_furnitureId, 0.0f, 1, -1, -1, 0, &n4, &n16, nullptr, nullptr, nullptr, nullptr);
    if (entity20 != nullptr) {
        Lounge_AddChairInfo(n4, n16, entity20);
    }
    if (n2 > 28) {
        entity21 = PlaceFurnitureOnWall(2, 1, m_furnitureId, 0.0f, 1, -1, -1, 0, &n4, &n16, nullptr, nullptr, nullptr, nullptr);
        if (entity21 != nullptr) {
            Lounge_AddChairInfo(n4, n16, entity21);
        }
    }
    n22 = n3;
    if (n22 != 0) {
        SetCornerTiles(0, 2, 0, 0);
    }
    if (n22 != 2) {
        SetCornerTiles(2, 2, 0, 0);
    }
    if (n22 != 1) {
        SetCornerTiles(1, 2, 0, 0);
    }
    if (n22 != 3) {
        SetCornerTiles(3, 2, 0, 0);
    }
    entity25 = PlaceFurnitureOnWall(2, 6, -1, 0.0f, 1, -1, -1, 0, &n4, nullptr, &n23, &n24, nullptr, nullptr);
    if (entity25 != nullptr) {
        Lounge_AddHifi(n4, n23, n24, 0);
    }
    PlaceFurnitureOnWall(2, 5, -1, 0.0f, 1, -1, -1, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    PlaceFurnitureOnWall(8, 0, -1, 0.0f, 1, -1, -1, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    PlaceFurnitureOnWall(8, 0, -1, 0.0f, 1, -1, -1, 0, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
    n26 = InteriorField<uint8>(m_box, 0x1F);
    if (uint32(uint8(n26)) < uint32(75)) {
        if (uint32(uint8(n26)) < uint32(50)) {
            n27 = InteriorRandCpp();
            n2 = (n27 & 65535);
            n28 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * -50.0f));
            n29 = 50;
        } else {
            n30 = InteriorRandCpp();
            n2 = (n30 & 65535);
            n28 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * -30.0f));
            n29 = 20;
        }
        n31 = (n29 - n28);
    } else {
        n32 = InteriorRandCpp();
        n2 = (n32 & 65535);
        n33 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * 20.0f));
        n31 = n33;
    }
    n34 = InteriorRandCpp();
    n2 = (n34 & 65535);
    n35 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * 60.0f));
    if (n35 < n31) {
        ok36 = FindEmptyTiles(2, 2, &n2, &n3);
        if (ok36) {
            furniture37 = g_furnitureMan.GetFurniture(8, 2, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture37, FloatPlusIntCpp(0.5f, n2), FloatPlusIntCpp(0.5f, n3), 0.05f, 0.0f);
            SetTilesStatus(n2, n3, 2, 2, 2, 0);
        }
    }
    n38 = InteriorRandCpp();
    n2 = (n38 & 65535);
    n39 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * 100.0f));
    if (n39 < n31) {
        ok40 = FindEmptyTiles(1, 1, &n3, &n2);
        if (ok40) {
            furniture41 = g_furnitureMan.GetFurniture(8, 5, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture41, FloatPlusIntCpp(0.5f, n3), FloatPlusIntCpp(0.5f, n2), 0.05f, 0.0f);
            SetTilesStatus(n3, n2, 1, 1, 2, 0);
        }
    }
    n42 = InteriorRandCpp();
    n2 = (n42 & 65535);
    n43 = int32((IntTimesFloatCpp(n2, 0.000030517578f) * 100.0f));
    if (n43 < n31) {
        ok44 = FindEmptyTiles(1, 1, &n3, &n2);
        if (ok44) {
            furniture45 = g_furnitureMan.GetFurniture(8, 4, -1, InteriorField<uint8>(m_box, 0x1F));
            PlaceObject(0, furniture45, FloatPlusIntCpp(0.5f, n3), FloatPlusIntCpp(0.5f, n2), 0.05f, 0.0f);
            SetTilesStatus(n3, n2, 1, 1, 2, 0);
        }
    }
    furniture46 = g_furnitureMan.GetFurniture(8, 1, -1, InteriorField<uint8>(m_box, 0x1F));
    p47 = reinterpret_cast<uintptr_t>(m_box);
    furniture48 = furniture46;
    n2 = InteriorField<uint8>(p47, 0x3);
    n49 = n2;
    n2 = InteriorField<uint8>(furniture48, 0xD);
    n50 = int32((IntTimesFloatCpp(n49, 0.5f) - IntTimesFloatCpp(n2, 0.5f)));
    n2 = InteriorField<uint8>(p47, 0x2);
    n51 = n2;
    n2 = InteriorField<uint8>(furniture48, 0xC);
    n52 = int32((IntTimesFloatCpp(n51, 0.5f) - IntTimesFloatCpp(n2, 0.5f)));
    PlaceFurniture(furniture48, n52, n50, 0.0f, 0, 0, &n7, &n8, 0);
    n53 = 0;
    n3 = n53;
    if ((InteriorField<uint8>(m_box, 0x2) - 1) > 0) {
        do {
            n54 = GetTileStatus(n53, 0);
            n55 = n54;
            n56 = GetTileStatus(n53, (InteriorField<uint8>(m_box, 0x3) - 1));
            n57 = n56;
            if ((n55 == 0) || (n55 == 2)) {
                AddInteriorInfo(2, float(n3), 0.0f, 2, nullptr);
            }
            if ((n57 == 0) || (n57 == 2)) {
                n2 = (InteriorField<uint8>(m_box, 0x3) - 1);
                AddInteriorInfo(2, float(n3), float(n2), 0, nullptr);
            }
            n53 = (n53 + 1);
            n3 = n53;
        } while (n53 < (InteriorField<uint8>(m_box, 0x2) - 1));
    }
    n58 = 1;
    n3 = n58;
    if ((InteriorField<uint8>(m_box, 0x3) - 2) > n58) {
        do {
            n59 = GetTileStatus(0, n58);
            n60 = n59;
            n61 = GetTileStatus((InteriorField<uint8>(m_box, 0x2) - 1), n58);
            n62 = n61;
            if ((n60 == 0) || (n60 == 2)) {
                AddInteriorInfo(2, 0.0f, float(n3), 1, nullptr);
            }
            if ((n62 == 0) || (n62 == 2)) {
                n2 = (InteriorField<uint8>(m_box, 0x2) - 1);
                AddInteriorInfo(2, float(n2), float(n3), 3, nullptr);
            }
            n58 = (n58 + 1);
            n3 = n58;
        } while (n58 < (InteriorField<uint8>(m_box, 0x3) - 2));
    }
    return;
}

// 0x599210
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Office_PlaceEdgeFillers(int32 arg0, int32 a2, int32 a3, int32 a6, int32 a7) {
    Furniture_c* furniture13{};
    Furniture_c* furniture14{};
    Furniture_c* furniture18{};
    Furniture_c* furniture20{};
    int32 n1{};
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n19{};
    int32 n2{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    n1 = InteriorRandCpp();
    n2 = int32((IntTimesFloatCpp((n1 & 65535), 0.000030517578f) * 100.0f));
    n3 = a6;
    n4 = n2;
    if (n3 == 2) {
        n5 = 1;
    } else {
        n5 = 2;
        if (n3 == 0) {
            n5 = 1;
        }
    }
    n6 = GetNumEmptyTiles(a2, a3, n5, 1);
    if (n6 < 1) {
        return 1;
    }
    n7 = a2;
    n8 = a3;
    if (n3 != 2) {
        if (n3 != 0) {
            if (n3 != 3) {
                if (n3 == 1) {
                    n9 = (a2 + 1);
                    n7 = n9;
                }
            } else {
                n9 = (a2 - 1);
                n7 = n9;
            }
        } else {
            n8 = (a3 - 1);
        }
    } else {
        n8 = (a3 + 1);
    }
    if (arg0 != (-1)) {
        n10 = InteriorField<uint8>(m_box, 0x1F);
        n11 = arg0;
        n12 = 1;
        furniture13 = g_furnitureMan.GetFurniture(n12, n11, -1, uint8(n10));
        furniture14 = furniture13;
        PlaceFurniture(furniture14, a2, a3, 0.0f, 1, n3, &n15, &n16, 0);
        return n15;
    }
    n17 = n4;
    if ((n17 <= 90) || (StaticRef<uint8>(0xBB3DC8) != 0)) {
        if (n17 <= 75) {
            if (n17 <= 25) {
                return 1;
            }
            furniture18 = g_furnitureMan.GetFurniture(1, 3, -1, InteriorField<uint8>(m_box, 0x1F));
            furniture14 = furniture18;
            if (furniture14 != nullptr) {
                AddInteriorInfo(7, float(n7), float(n8), ((n3 - 2) & 3), nullptr);
            }
            PlaceFurniture(furniture14, a2, a3, 0.0f, 1, n3, &n15, &n16, 0);
            return n15;
        } else {
            n19 = CGeneral::GetRandomIntInRange(0, 5);
            n3 = n19;
            n10 = InteriorField<uint8>(m_box, 0x1F);
            n11 = 0;
            n12 = 8;
            furniture13 = g_furnitureMan.GetFurniture(n12, n11, -1, uint8(n10));
            furniture14 = furniture13;
            PlaceFurniture(furniture14, a2, a3, 0.0f, 1, n3, &n15, &n16, 0);
            return n15;
        }
    } else {
        furniture20 = g_furnitureMan.GetFurniture(1, 2, -1, InteriorField<uint8>(m_box, 0x1F));
        furniture14 = furniture20;
        if (furniture14 != nullptr) {
            AddInteriorInfo(7, float(n7), float(n8), ((n3 - 2) & 3), nullptr);
        }
        StaticRef<uint8>(0xBB3DC8) = uint8(1);
        PlaceFurniture(furniture14, a2, a3, 0.0f, 1, n3, &n15, &n16, 0);
        return n15;
    }
}

// 0x5993E0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Office_PlaceDesk(int32 a3, int32 arg4, int32 offsetY, int32 a5, uint8 a6, int32 b) {
    CEntity* entity16{};
    CEntity* entity9{};
    float f17{};
    float f18{};
    float f19{};
    Furniture_c* furniture15{};
    Furniture_c* furniture6{};
    int32 n1{};
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n2{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n7{};
    int32 n8{};
    n1 = a3;
    n2 = arg4;
    n3 = ((offsetY - 2) & 3);
    n4 = n1;
    n5 = n2;
    if (offsetY != 2) {
        if (offsetY == 1) {
            n4 = (n1 + 1);
        }
    } else {
        n5 = (n2 + 1);
    }
    furniture6 = g_furnitureMan.GetFurniture(1, 0, int16(b), InteriorField<uint8>(m_box, 0x1F));
    entity9 = PlaceFurniture(furniture6, n4, n5, 0.0f, 1, n3, &n7, &n8, 0);
    if (entity9 == nullptr) {
        return 1;
    }
    n10 = offsetY;
    n11 = n1;
    n12 = n2;
    n13 = n1;
    n14 = n2;
    if (n10 != 2) {
        if (n10 != 0) {
            if (n10 != 3) {
                if (n10 == 1) {
                    n10 = (n2 + 1);
                    n14 = n10;
                }
            } else {
                n2 = (n2 + 1);
                n1 = (n1 + 1);
                n12 = n2;
                n11 = n1;
                n13 = n1;
            }
        } else {
            n2 = (n2 + 1);
            n12 = n2;
            n14 = n2;
            n13 = (n1 + 1);
        }
    } else {
        n1 = (n1 + 1);
        n11 = n1;
    }
    furniture15 = g_furnitureMan.GetFurniture(1, 1, int16(field_792), InteriorField<uint8>(m_box, 0x1F));
    entity16 = PlaceFurniture(furniture15, n1, n2, 0.0f, 1, n3, &n7, &n8, 1);
    f17 = float(n11);
    f18 = float(n12);
    if (n3 != 2) {
        if (n3 != 0) {
            if (n3 != 3) {
                if (n3 == 1) {
                    f19 = (f17 + 0.5f);
                    f17 = f19;
                }
            } else {
                f19 = (f17 - 0.5f);
                f17 = f19;
            }
        } else {
            f18 = (f18 - 0.5f);
        }
    } else {
        f18 = (f18 + 0.5f);
    }
    AddInteriorInfo(6, f17, f18, ((n3 - 2) & 3), entity16);
    SetTilesStatus(n13, n14, 1, 1, 2, 1);
    return 2;
}

// 0x5995B0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Office_PlaceEdgeDesks(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    int32 n1{};
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    n1 = InteriorRandCpp();
    n2 = int32((IntTimesFloatCpp((n1 & 65535), 0.000030517578f) * 100.0f));
    n3 = n2;
    n4 = InteriorRandCpp();
    n5 = int32((IntTimesFloatCpp((n4 & 65535), 0.000030517578f) * -40.0f));
    n6 = (30 - n5);
    if (a5 == 2) {
        n7 = 1;
    } else {
        n7 = 2;
        if (a5 == 0) {
            n7 = 1;
        }
    }
    n8 = GetNumEmptyTiles(a3, a4, n7, 1);
    if (n8 > 1) {
        n9 = ((n8 - (n8 >> 31)) >> 1);
        n10 = InteriorRandCpp();
        n11 = int32((IntTimesFloatCpp((n10 & 65535), 0.000030517578f) * -2.0f));
        if (n9 >= (2 - n11)) {
            n12 = InteriorRandCpp();
            n13 = int32((IntTimesFloatCpp((n12 & 65535), 0.000030517578f) * -2.0f));
            n14 = (2 - n13);
        } else {
            n14 = n9;
        }
        n15 = 0;
        if (n3 <= n6) {
            if (n14 > 0) {
                n16 = ((a5 - 2) & 3);
                n17 = n14;
                do {
                    if (a6 != 0) {
                        if (a6 != 3) {
                            if (a6 != 1) {
                                if (a6 == 2) {
                                    n18 = m_furnitureId;
                                    n19 = 0;
                                    n20 = n16;
                                    n21 = a4;
                                    n22 = (n15 + a3);
                                    n23 = Office_PlaceDesk(n22, n21, n20, 70, uint8(n19), n18);
                                    n15 = (n15 + n23);
                                }
                            } else {
                                n22 = a3;
                                n18 = m_furnitureId;
                                n19 = 0;
                                n20 = n16;
                                n21 = (n15 + a4);
                                n23 = Office_PlaceDesk(n22, n21, n20, 70, uint8(n19), n18);
                                n15 = (n15 + n23);
                            }
                        } else {
                            n18 = m_furnitureId;
                            n19 = 0;
                            n20 = n16;
                            n21 = (n15 + a4);
                            n22 = (a3 - 1);
                            n23 = Office_PlaceDesk(n22, n21, n20, 70, uint8(n19), n18);
                            n15 = (n15 + n23);
                        }
                    } else {
                        n18 = m_furnitureId;
                        n19 = a6;
                        n20 = n16;
                        n21 = (a4 - 1);
                        n22 = (n15 + a3);
                        n23 = Office_PlaceDesk(n22, n21, n20, 70, uint8(n19), n18);
                        n15 = (n15 + n23);
                    }
                    n24 = n17;
                    n17 = (n17 - 1);
                } while (n24 != 1);
            }
            return (n15 + 1);
        }
        return 1;
    } else {
        return 1;
    }
}

// 0x599770
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Office_FurnishEdges() {
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n25{};
    int32 n26{};
    int32 n27{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    uintptr_t p1{};
    uintptr_t p6{};
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = (InteriorField<uint8>(p1, 0x2) - 3);
    n3 = 2;
    n4 = (InteriorField<uint8>(p1, 0x3) - 3);
    if (n2 >= n3) {
        do {
            SetTilesStatus(n3, n4, 1, 1, 3, 0);
            SetTilesStatus(n3, 2, 1, 1, 3, 0);
            n3 = (n3 + 1);
        } while (n3 <= n2);
    }
    n5 = 2;
    if (n4 >= n5) {
        do {
            SetTilesStatus(2, n5, 1, 1, 3, 0);
            SetTilesStatus(n2, n5, 1, 1, 3, 0);
            n5 = (n5 + 1);
        } while (n5 <= n4);
    }
    AddGotoPt(2, 2, 0.5f, 0.5f);
    AddGotoPt(2, n4, 0.5f, -0.5f);
    AddGotoPt(n2, 2, -0.5f, 0.5f);
    AddGotoPt(n2, n4, -0.5f, -0.5f);
    p6 = reinterpret_cast<uintptr_t>(m_box);
    n7 = (InteriorField<uint8>(p6, 0x2) - 1);
    n8 = (InteriorField<uint8>(p6, 0x3) - 1);
    SetTilesStatus((InteriorField<int8>(p6, 0x5) - 2), 0, 4, 2, 7, 0);
    n9 = 1;
    if (n7 > n9) {
        do {
            n10 = Office_PlaceEdgeDesks(-1, n9, 0, 2, 2);
            n9 = (n9 + n10);
        } while (n9 < n7);
    }
    n11 = 1;
    if (n7 > n11) {
        do {
            n12 = Office_PlaceEdgeDesks(-1, n11, n8, 0, 0);
            n11 = (n11 + n12);
        } while (n11 < n7);
    }
    n13 = (n8 - 1);
    n14 = 1;
    if (n13 >= n14) {
        do {
            n15 = Office_PlaceEdgeDesks(-1, 0, n14, 1, 1);
            n14 = (n14 + n15);
        } while (n14 <= n13);
    }
    n16 = 1;
    if (n13 >= n16) {
        do {
            n17 = Office_PlaceEdgeDesks(-1, n7, n16, 3, 3);
            n16 = (n16 + n17);
        } while (n16 <= n13);
    }
    n18 = 1;
    n19 = (n13 + 1);
    if (n7 > n18) {
        do {
            n20 = Office_PlaceEdgeFillers(-1, n18, 0, 2, 2);
            n18 = (n18 + n20);
        } while (n18 < n7);
    }
    n21 = 1;
    if (n7 > n21) {
        do {
            n22 = Office_PlaceEdgeFillers(-1, n21, n19, 0, 0);
            n21 = (n21 + n22);
        } while (n21 < n7);
    }
    n23 = (n19 - 1);
    n24 = 1;
    if (n23 >= n24) {
        do {
            n25 = Office_PlaceEdgeFillers(-1, 0, n24, 1, 1);
            n24 = (n24 + n25);
        } while (n24 <= n23);
    }
    n26 = 1;
    if (n23 >= n26) {
        do {
            n27 = Office_PlaceEdgeFillers(-1, n7, n26, 3, 3);
            n26 = (n26 + n27);
        } while (n26 <= n23);
    }
    return;
}

// 0x599960
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Office_PlaceDeskQuad(int32 a2, int32 a3, int32 a4, int32 a5) {
    int32 n1{};
    int32 n2{};
    n1 = (a4 - 2);
    Office_PlaceDesk(a3, n1, 2, 70, 0, a5);
    Office_PlaceDesk(a3, a4, 0, 70, 0, a5);
    Office_PlaceDesk((a3 - 2), a4, 0, 70, 0, a5);
    Office_PlaceDesk((a3 - 2), n1, 2, 70, 0, a5);
    SetTilesStatus((a3 - 3), (a4 - 3), 6, 1, 3, 0);
    n2 = (a3 - 3);
    SetTilesStatus(n2, (a4 + 2), 6, 1, 3, 0);
    SetTilesStatus(n2, n1, 1, 4, 3, 0);
    SetTilesStatus((a3 + 2), n1, 1, 4, 3, 0);
    return 6;
}

// 0x599BB0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
// EAX = (CEntity* de la ultima PlaceFurniture & ~0xFF) | 1 como el EXE (mov al,1 en 0x599DB0): lo compara el banco; ResidueRegsCpp es lo ultimo que se ejecuta.
#pragma runtime_checks("", off)
uint32 Interior_c::Shop_Place3PieceUnit(int32 a2, int32 a3, int32 a4, int32 a5, int32 a6) {
    CEntity* entity20{};
    CEntity* entity25{};
    Furniture_c* furniture11{};
    Furniture_c* furniture12{};
    Furniture_c* furniture13{};
    Furniture_c* furniture14{};
    Furniture_c* furniture15{};
    Furniture_c* furniture5{};
    Furniture_c* furniture6{};
    int32 n10{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n3{};
    int32 n4{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    n3 = a5;
    if ((n3 == 2) || (n3 == 3)) {
        n4 = a2;
        furniture5 = g_furnitureMan.GetFurniture(m_furnitureGroupId, (n4 + 1), -1, InteriorField<uint8>(m_box, 0x1F));
        furniture6 = furniture5;
        n7 = InteriorField<uint8>(m_box, 0x1F);
        n8 = InteriorField<uint16>(furniture6, 0xA);
        n9 = n4;
        n10 = m_furnitureGroupId;
    } else {
        n4 = a2;
        furniture11 = g_furnitureMan.GetFurniture(m_furnitureGroupId, n4, -1, InteriorField<uint8>(m_box, 0x1F));
        furniture6 = furniture11;
        n7 = InteriorField<uint8>(m_box, 0x1F);
        n8 = InteriorField<uint16>(furniture6, 0xA);
        n9 = (n4 + 1);
        n10 = m_furnitureGroupId;
    }
    furniture12 = g_furnitureMan.GetFurniture(n10, n9, int16(n8), uint8(n7));
    furniture13 = furniture12;
    furniture14 = g_furnitureMan.GetFurniture(m_furnitureGroupId, (n4 + 2), int16(InteriorField<uint16>(furniture6, 0xA)), InteriorField<uint8>(m_box, 0x1F));
    furniture15 = furniture14;
    if ((n3 == 2) || (n3 == 0)) {
        PlaceFurniture(furniture6, a3, a4, 0.0f, 1, n3, &a5, &a2, 0);
        n16 = (a6 - 2);
        n17 = (a3 + a5);
        if (n16 > 0) {
            n18 = n16;
            do {
                PlaceFurniture(furniture15, n17, a4, 0.0f, 1, n3, &a5, &a2, 0);
                n17 = (n17 + a5);
                n19 = n18;
                n18 = (n18 - 1);
            } while (n19 != 1);
        }
        entity20 = PlaceFurniture(furniture13, n17, a4, 0.0f, 1, n3, &a5, &a2, 0);
        return ResidueRegsCpp(0, 0, uint32(((int32(reinterpret_cast<uintptr_t>(entity20)) & (-256)) | 1)));
    }
    PlaceFurniture(furniture6, a3, a4, 0.0f, 1, n3, &a5, &a2, 0);
    n21 = (a6 - 2);
    n22 = (a4 + a2);
    if (n21 > 0) {
        n23 = n21;
        do {
            PlaceFurniture(furniture15, a3, n22, 0.0f, 1, n3, &a5, &a2, 0);
            n22 = (n22 + a2);
            n24 = n23;
            n23 = (n23 - 1);
        } while (n24 != 1);
    }
    entity25 = PlaceFurniture(furniture13, a3, n22, 0.0f, 1, n3, &a5, &a2, 0);
    return ResidueRegsCpp(0, 0, uint32(((int32(reinterpret_cast<uintptr_t>(entity25)) & (-256)) | 1)));
}
#pragma runtime_checks("", restore)

// 0x599DC0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Shop_PlaceEdgeUnits(int32 a2, int32 a3, int32 a4, int32 a5) {
    int32 n1{};
    int32 n10{};
    int32 n11{};
    int32 n2{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    if (a5 == 2) {
        n1 = 1;
    } else {
        n1 = 2;
        if (a5 == 0) {
            n1 = 1;
        }
    }
    n2 = GetNumEmptyTiles(a3, a4, n1, 1);
    n3 = n2;
    if (n2 > 1) {
        n4 = InteriorRandCpp();
        n5 = int32((IntTimesFloatCpp((n4 & 65535), 0.000030517578f) * -3.0f));
        n6 = (2 - n5);
        n7 = n3;
        if (n7 != 3) {
            n8 = n7;
            n9 = (n8 - n6);
            if ((n8 - n6) >= 0) {
                if (n9 == 1) {
                    n6 = (n6 - 1);
                }
            } else {
                n6 = n7;
            }
        } else {
            n6 = 3;
        }
        n10 = n6;
        if (a2 != (-1)) {
            Shop_Place3PieceUnit(a2, a3, a4, a5, n10);
            return n6;
        }
        n11 = StaticRef<int32>(0xBB3DE4);
        if (n11 <= 50) {
            if (n11 <= 25) {
                if (n11 <= 10) {
                    Shop_Place3PieceUnit(9, a3, a4, a5, n10);
                    return n6;
                }
                Shop_Place3PieceUnit(6, a3, a4, a5, n10);
                return n6;
            } else {
                Shop_Place3PieceUnit(3, a3, a4, a5, n10);
                return n6;
            }
        } else {
            Shop_Place3PieceUnit(0, a3, a4, a5, n10);
            return n6;
        }
    } else {
        return 1;
    }
}

// 0x599EF0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
int32 Interior_c::Shop_PlaceCounter(uint32 a2) {
    Furniture_c* furniture1{};
    Furniture_c* furniture2{};
    Furniture_c* furniture3{};
    Furniture_c* furniture4{};
    int32 n10{};
    int32 n11{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    furniture1 = g_furnitureMan.GetFurniture(0, 12, -1, InteriorField<uint8>(m_box, 0x1F));
    furniture2 = furniture1;
    furniture3 = g_furnitureMan.GetFurniture(0, 13, -1, InteriorField<uint8>(m_box, 0x1F));
    furniture4 = furniture3;
    n5 = InteriorRandCpp();
    n6 = int32((IntTimesFloatCpp((n5 & 65535), 0.000030517578f) * 4.0f));
    n7 = n6;
    if (uint8(a2) == 0) {
        n8 = (InteriorField<int8>(m_box, 0x5) + 2);
        PlaceFurniture(furniture2, n8, 1, 0.0f, 1, 0, &n9, &n10, 0);
        SetTilesStatus(n8, 0, (n9 + 1), 1, 2, 0);
        n11 = (InteriorField<int8>(m_box, 0x5) - 2);
    } else {
        n8 = (InteriorField<int8>(m_box, 0x5) - 5);
        PlaceFurniture(furniture2, n8, 1, 0.0f, 1, 0, &n9, &n10, 0);
        SetTilesStatus((n8 - 1), 0, (n9 + 1), 1, 2, 0);
        n11 = (InteriorField<int8>(m_box, 0x5) + 1);
    }
    PlaceFurniture(furniture4, n11, 0, 0.0f, 1, n7, &n9, &n10, 1);
    return (n8 + 2);
}

// 0x59A030
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Shop_PlaceFixedUnits() {
    int32 n2{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    uintptr_t p3{};
    uint32 u1{};
    uint32 u9{};
    u1 = uint32(this);
    n2 = InteriorField<uint8>(m_box, 0x5);
    if (uint8(n2) != 255) {
        SetTilesStatus((int8(uint8(n2)) - 1), 0, 2, 1, 7, 0);
        p3 = reinterpret_cast<uintptr_t>(m_box);
        n4 = InteriorField<int8>(p3, 0x5);
        n5 = (n4 - 2);
        if (((InteriorField<uint8>(p3, 0x2) - n4) - 2) < 6) {
            if (n5 >= 6) {
                n6 = 1;
                n7 = Shop_PlaceCounter(uint32(n6));
                u1 = uint32(n7);
            }
        } else {
            if (n5 < 6) {
                n6 = 0;
            } else {
                n8 = InteriorRandCpp();
                // El EXE (0x59A07F..0x59A08A) escribe SETL en el byte bajo de la ranura [esp+4], que aun guarda ECX de entrada (this), y apila
                // la palabra entera: el argumento lleva los 24 bits altos de this. Shop_PlaceCounter solo lee el byte bajo, pero el banco compara la palabra.
                n6 = int32((u1 & 0xFFFFFF00u) | uint32(n8 < 16383));
            }
            n7 = Shop_PlaceCounter(uint32(n6));
            u1 = uint32(n7);
        }
        u1 = InteriorFloatBits(float(int32(u1)));
        u9 = uint32(int32(u1));
        AddInteriorInfo(9, InteriorBitsFloat(u9), 2.0f, 0, nullptr);
        AddInteriorInfo(10, InteriorBitsFloat(u9), 0.0f, 2, nullptr);
    }
    return;
}

// 0x59A1B0
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Shop_FurnishEdges() {
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n25{};
    int32 n26{};
    int32 n27{};
    int32 n28{};
    int32 n29{};
    int32 n3{};
    int32 n30{};
    int32 n31{};
    int32 n33{};
    int32 n34{};
    int32 n37{};
    int32 n39{};
    int32 n4{};
    int32 n40{};
    int32 n41{};
    int32 n42{};
    int32 n43{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    bool ok35{};
    bool ok38{};
    uintptr_t p1{};
    uintptr_t p24{};
    uintptr_t p32{};
    uintptr_t p36{};
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = (InteriorField<uint8>(p1, 0x2) - 1);
    n3 = (InteriorField<uint8>(p1, 0x3) - 1);
    n4 = 1;
    n5 = InteriorRandCpp();
    n6 = int32((IntTimesFloatCpp((n5 & 65535), 0.000030517578f) * 100.0f));
    if (n6 <= 50) {
        if (n6 <= 25) {
            n7 = (((int32((n6 <= 10)) - 1) & (-3)) + 9);
        } else {
            n7 = 3;
        }
    } else {
        n7 = 0;
    }
    if (n2 > 1) {
        do {
            n8 = Shop_PlaceEdgeUnits(n7, n4, n3, 0);
            n4 = (n4 + n8);
        } while (n4 < n2);
    }
    n9 = (n3 - 1);
    n10 = n9;
    n11 = 1;
    n12 = InteriorRandCpp();
    n13 = int32((IntTimesFloatCpp((n12 & 65535), 0.000030517578f) * 100.0f));
    if (n13 <= 50) {
        if (n13 <= 25) {
            n14 = (((int32((n13 <= 10)) - 1) & (-3)) + 9);
        } else {
            n14 = 3;
        }
    } else {
        n14 = 0;
    }
    if (n9 >= 1) {
        do {
            n15 = Shop_PlaceEdgeUnits(n14, 0, n11, 1);
            n11 = (n11 + n15);
        } while (n11 <= n9);
    }
    n16 = 1;
    n17 = InteriorRandCpp();
    n18 = int32((IntTimesFloatCpp((n17 & 65535), 0.000030517578f) * 100.0f));
    if (n18 <= 50) {
        if (n18 <= 25) {
            n19 = (((int32((n18 <= 10)) - 1) & (-3)) + 9);
        } else {
            n19 = 3;
        }
    } else {
        n19 = 0;
    }
    if (n9 >= 1) {
        do {
            n18 = Shop_PlaceEdgeUnits(n19, n2, n16, 3);
            n16 = (n16 + n18);
        } while (n16 <= n9);
    }
    n20 = (n2 - 1);
    n21 = 1;
    n22 = n20;
    n23 = n21;
    if (n20 >= n21) {
        do {
            p24 = reinterpret_cast<uintptr_t>(m_box);
            n25 = InteriorField<uint8>(p24, 0xA);
            if (((uint8(n25) == 255) || (n21 < int8(uint8(n25)))) || (n21 > InteriorField<int8>(p24, 0xB))) {
                if (StaticRef<int32>(0x8D0948) > 1) {
                    n26 = InteriorRandCpp();
                    n27 = int32((IntTimesFloatCpp((n26 & 65535), 0.000030517578f) * 100.0f));
                    if (n27 > 60) {
                        AddInteriorInfo(8, float(n23), float(n10), 2, nullptr);
                        StaticRef<int32>(0x8D0948) = 0;
                    }
                }
                StaticRef<int32>(0x8D0948) = (StaticRef<int32>(0x8D0948) + 1);
            }
            n18 = GetTileStatus(n21, 1);
            if (n18 == 0) {
                SetTilesStatus(n21, 1, 1, 1, 2, 0);
                n18 = 0;
            }
            n21 = (n21 + 1);
            n23 = n21;
        } while (n21 <= n20);
    }
    n28 = (n9 - 1);
    n29 = 2;
    if (n28 >= 2) {
        n30 = StaticRef<int32>(0x8D0948);
        n31 = 2;
        do {
            p32 = reinterpret_cast<uintptr_t>(m_box);
            n33 = ((n18 & (-256)) | InteriorField<uint8>(p32, 0x6));
            if (uint8(n33) == 255) {
                if (n30 > 1) {
                    n34 = InteriorRandCpp();
                    n33 = int32((IntTimesFloatCpp((n34 & 65535), 0.000030517578f) * 100.0f));
                    if (n33 <= 60) {
                        n30 = StaticRef<int32>(0x8D0948);
                    } else {
                        ok35 = AddInteriorInfo(8, 1.0f, float(n29), 3, nullptr);
                        n33 = uint8(int32(ok35));
                        n30 = 0;
                    }
                }
                n30 = (n30 + 1);
                StaticRef<int32>(0x8D0948) = n30;
            } else {
                n33 = int8(uint8(n33));
                if ((n31 < n33) || (n31 > InteriorField<int8>(p32, 0x7))) {
                    if (n30 > 1) {
                        n34 = InteriorRandCpp();
                        n33 = int32((IntTimesFloatCpp((n34 & 65535), 0.000030517578f) * 100.0f));
                        if (n33 <= 60) {
                            n30 = StaticRef<int32>(0x8D0948);
                        } else {
                            ok35 = AddInteriorInfo(8, 1.0f, float(n29), 3, nullptr);
                            n33 = uint8(int32(ok35));
                            n30 = 0;
                        }
                    }
                    n30 = (n30 + 1);
                    StaticRef<int32>(0x8D0948) = n30;
                }
            }
            p36 = reinterpret_cast<uintptr_t>(m_box);
            n18 = ((n33 & (-256)) | InteriorField<uint8>(p36, 0x8));
            if (uint8(n18) == 255) {
                if (n30 > 1) {
                    n37 = InteriorRandCpp();
                    n18 = int32((IntTimesFloatCpp((n37 & 65535), 0.000030517578f) * 100.0f));
                    if (n18 <= 60) {
                        n30 = StaticRef<int32>(0x8D0948);
                    } else {
                        ok38 = AddInteriorInfo(8, float(n22), float(n29), 1, nullptr);
                        n18 = uint8(int32(ok38));
                        n30 = 0;
                    }
                }
                n30 = (n30 + 1);
                StaticRef<int32>(0x8D0948) = n30;
            } else {
                n18 = int8(uint8(n18));
                if ((n31 < n18) || (n31 > InteriorField<int8>(p36, 0x9))) {
                    if (n30 > 1) {
                        n37 = InteriorRandCpp();
                        n18 = int32((IntTimesFloatCpp((n37 & 65535), 0.000030517578f) * 100.0f));
                        if (n18 <= 60) {
                            n30 = StaticRef<int32>(0x8D0948);
                        } else {
                            ok38 = AddInteriorInfo(8, float(n22), float(n29), 1, nullptr);
                            n18 = uint8(int32(ok38));
                            n30 = 0;
                        }
                    }
                    n30 = (n30 + 1);
                    StaticRef<int32>(0x8D0948) = n30;
                }
            }
            n31 = (n31 + 1);
            n29 = n31;
        } while (n31 <= n28);
    }
    n39 = (n20 - 1);
    if (n39 >= 2) {
        n40 = 2;
        do {
            n41 = GetTileStatus(n40, 2);
            if (n41 == 0) {
                SetTilesStatus(n40, 2, 1, 1, 2, 0);
            }
            SetTilesStatus(n40, n28, 1, 1, 3, 0);
            n40 = (n40 + 1);
        } while (n40 <= n39);
    }
    n42 = (n28 - 1);
    n43 = 3;
    if (n42 >= n43) {
        do {
            SetTilesStatus(2, n43, 1, 1, 3, 0);
            SetTilesStatus(n39, n43, 1, 1, 3, 0);
            n43 = (n43 + 1);
        } while (n43 <= n42);
    }
    SetTilesStatus(3, 3, (n39 - 3), 1, 3, 0);
    return;
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
    // fild * 0,5; fadd offsetZ; fsub boundMinZ; fstp
    return (IntTimesFloatCpp(negativeHeight, 0.5f) + offsetZ) - boundMinZ;
}

static void InteriorFurniturePositionExact(CVector* position,int32 negativeWidth,int32 negativeDepth,int32 boxHeight,int32 tileX,int32 tileY,int32 shiftX,int32 shiftY,float height,bool ceiling) {
    const float half = 0.5f;
    // z: fild * 0,5 y, segun el techo, - height o (fchs) + height; y, x: fild * 0,5 + fiadd shift + fiadd tile + 0,5 (cada paso a 24 bits).
    const float boxHalf = IntTimesFloatCpp(boxHeight, half);
    const float z = ceiling ? boxHalf - height : -boxHalf + height;
    const float y = FloatPlusIntCpp(FloatPlusIntCpp(IntTimesFloatCpp(negativeDepth, half), shiftY), tileY) + half;
    const float x = FloatPlusIntCpp(FloatPlusIntCpp(IntTimesFloatCpp(negativeWidth, half), shiftX), tileX) + half;
    position->x = position->x + x;
    position->y = position->y + y;
    position->z = position->z + z;
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
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::Shop_FurnishAisles() {
    int32 n10{};
    int32 n11{};
    int32 n12{};
    int32 n13{};
    int32 n14{};
    int32 n15{};
    int32 n16{};
    int32 n17{};
    int32 n18{};
    int32 n19{};
    int32 n2{};
    int32 n20{};
    int32 n21{};
    int32 n22{};
    int32 n23{};
    int32 n24{};
    int32 n25{};
    int32 n26{};
    int32 n3{};
    int32 n4{};
    int32 n5{};
    int32 n6{};
    int32 n7{};
    int32 n8{};
    int32 n9{};
    uintptr_t p1{};
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = (InteriorField<uint8>(p1, 0x3) - 4);
    n3 = (InteriorField<uint8>(p1, 0x2) - 6);
    n4 = (n2 - 3);
    n5 = n3;
    n6 = n4;
    if ((n3 > 0) && (n4 > 0)) {
        AddGotoPt(2, 3, -0.5f, -0.5f);
        n7 = (n2 + 1);
        n8 = n7;
        AddGotoPt(2, n7, -0.5f, 0.5f);
        n9 = 0;
        if (n3 > 0) {
            do {
                n10 = 4;
                n11 = InteriorRandCpp();
                n12 = int32((IntTimesFloatCpp((n11 & 65535), 0.000030517578f) * 100.0f));
                if (n12 <= 50) {
                    if (n12 <= 25) {
                        n13 = (((int32((n12 <= 10)) - 1) & (-3)) + 9);
                    } else {
                        n13 = 3;
                    }
                } else {
                    n13 = 0;
                }
                n14 = n9;
                n15 = n14;
                n16 = (n15 & (-2147483645));
                if ((n15 & (-2147483645)) < 0) {
                    n16 = (((n16 - 1) | (-4)) + 1);
                }
                if (n16 != 0) {
                    if (n16 != 1) {
                        if (n16 != 2) {
                            if (n16 == 3) {
                                n17 = (n14 + 3);
                                SetTilesStatus(n17, 4, 1, n4, 3, 0);
                                AddGotoPt(n17, 3, -0.5f, -0.5f);
                                AddGotoPt(n17, n8, -0.5f, 0.5f);
                            }
                        } else {
                            n18 = 0;
                            if (n4 > 0) {
                                n19 = (n14 + 3);
                                do {
                                    Shop_AddShelfInfo(n19, (n18 + 4), 3);
                                    n18 = (n18 + 1);
                                } while (n18 < n4);
                            }
                        }
                    } else {
                        if (n4 > 0) {
                            do {
                                n20 = Shop_PlaceEdgeUnits(n13, (n9 + 3), n10, 1);
                                n10 = (n10 + n20);
                                n21 = n4;
                                n4 = (n4 - 1);
                            } while (n21 != 1);
                            n4 = n6;
                        }
                    }
                } else {
                    if ((n14 != (n5 - 1)) && (n4 > 0)) {
                        n22 = n4;
                        do {
                            n23 = Shop_PlaceEdgeUnits(n13, (n9 + 3), n10, 3);
                            n10 = (n10 + n23);
                            n24 = n22;
                            n22 = (n22 - 1);
                        } while (n24 != 1);
                    }
                }
                n25 = (n9 + 1);
                n9 = n25;
            } while (n25 < n5);
            n7 = n8;
        }
        n26 = (n9 + 3);
        AddGotoPt(n26, 3, 0.5f, -0.5f);
        AddGotoPt(n26, n7, 0.5f, 0.5f);
    }
    return;
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
// Transcrito del EXE sin ensamblador: los registros pasan a variables, la aritmetica float conserva el redondeo a 24 bits por operacion
// (el archivo se compila con /arch:IA32) y las conversiones entero-flotante usan los ayudantes de X87Math (fild + operacion con un solo redondeo).
void Interior_c::FurnishShop(int32 a2) {
    int32 n2{};
    uintptr_t p1{};
    uintptr_t p3{};
    m_furnitureGroupId = uint8(a2);
    p1 = reinterpret_cast<uintptr_t>(m_box);
    n2 = InteriorField<int8>(p1, 0x5);
    if (((n2 - 1) >= 6) || ((InteriorField<uint8>(p1, 0x2) - n2) >= 6)) {
        SetTilesStatus(0, 0, 1, 1, 2, 0);
        SetTilesStatus(0, (InteriorField<uint8>(m_box, 0x3) - 1), 1, 1, 2, 0);
        SetTilesStatus((InteriorField<uint8>(m_box, 0x2) - 1), 0, 1, 1, 2, 0);
        p3 = reinterpret_cast<uintptr_t>(m_box);
        SetTilesStatus((InteriorField<uint8>(p3, 0x2) - 1), (InteriorField<uint8>(p3, 0x3) - 1), 1, 1, 2, 0);
        Shop_PlaceFixedUnits();
        Shop_FurnishEdges();
        Shop_FurnishAisles();
    }
    return;
}

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
// Interior_c.cpp se compila con /Zi /arch:IA32 (sin /ZI, PCH ni unity): ya sin ensamblador ni transcripciones desnudas, pero la aritmetica float
// del archivo tiene que seguir siendo x87 (redondeo a 24 bits por operacion, como el EXE). Shop_AddShelfInfo se optimiza con pragmas (ver source/CMakeLists.txt).

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

#pragma float_control(pop)

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
