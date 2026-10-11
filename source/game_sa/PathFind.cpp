/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPathFind
 * Reconstrucción: COMPLETA — 56/56 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 56/56 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CPathNode
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "PathFind.h"

#include <reversiblebugfixes/Bugs.hpp>

// 0x44EFC0
__declspec(naked) float CPathFind::CalcRoadDensity(float x, float y) {
    __asm {
        // 0x44EFC0
        sub esp,18h
        // 0x44EFC3
        fld dword ptr ds:[00858B50h]
        // 0x44EFC9
        push ebx
        // 0x44EFCA
        push ebp
        // 0x44EFCB
        push esi
        // 0x44EFCC
        push edi
        // 0x44EFCD
        lea ebp,[ecx+00000804h]
        // 0x44EFD3
        mov dword ptr [esp+20h],40h
        // 0x44EFDB
        jmp PathDensity13_44EFE0
        // 0x44EFDD
        lea ecx,[ecx]
PathDensity13_44EFE0:
        mov edx,dword ptr [ebp]
        // 0x44EFE3
        test edx,edx
        // 0x44EFE5
        je PathDensity13_44F167
        // 0x44EFEB
        mov eax,dword ptr [ebp+000008C0h]
        // 0x44EFF1
        test eax,eax
        // 0x44EFF3
        jle PathDensity13_44F167
        // 0x44EFF9
        lea esi,[edx+0Ah]
        // 0x44EFFC
        mov dword ptr [esp+18h],esi
        // 0x44F000
        mov dword ptr [esp+1Ch],eax
PathDensity13_44F004:
        movsx eax,word ptr [esi-2]
        // 0x44F008
        mov dword ptr [esp+14h],eax
        // 0x44F00C
        fild dword ptr [esp+14h]
        // 0x44F010
        fmul dword ptr ds:[00858C48h]
        // 0x44F016
        fst dword ptr [esp+10h]
        // 0x44F01A
        fsub dword ptr [esp+2Ch]
        // 0x44F01E
        fabs
        // 0x44F020
        fcomp dword ptr ds:[00858A14h]
        // 0x44F026
        fnstsw ax
        // 0x44F028
        test ah,5
        // 0x44F02B
        jp PathDensity13_44F151
        // 0x44F031
        movsx edx,word ptr [esi]
        // 0x44F034
        mov dword ptr [esp+14h],edx
        // 0x44F038
        fild dword ptr [esp+14h]
        // 0x44F03C
        fmul dword ptr ds:[00858C48h]
        // 0x44F042
        fst dword ptr [esp+14h]
        // 0x44F046
        fsub dword ptr [esp+30h]
        // 0x44F04A
        fabs
        // 0x44F04C
        fcomp dword ptr ds:[00858A14h]
        // 0x44F052
        fnstsw ax
        // 0x44F054
        test ah,5
        // 0x44F057
        jp PathDensity13_44F151
        // 0x44F05D
        xor edx,edx
        // 0x44F05F
        mov dl,byte ptr [esi+0Eh]
        // 0x44F062
        test dl,0Fh
        // 0x44F065
        je PathDensity13_44F151
        // 0x44F06B
        and edx,0Fh
        // 0x44F06E
        jle PathDensity13_44F151
        // 0x44F074
        movsx eax,word ptr [esi+6]
        // 0x44F078
        mov edi,dword ptr [ebp+00000240h]
        // 0x44F07E
        lea esi,[eax+eax]
        // 0x44F081
        lea edi,[edi+eax*4]
        // 0x44F084
        mov ebx,edx
PathDensity13_44F086:
        mov eax,dword ptr [edi]
        // 0x44F088
        mov dword ptr [esp+24h],eax
        // 0x44F08C
        movzx eax,ax
        // 0x44F08F
        mov edx,dword ptr [ecx+eax*4+00000804h]
        // 0x44F096
        test edx,edx
        // 0x44F098
        je PathDensity13_44F140
        // 0x44F09E
        mov eax,dword ptr [esp+24h]
        // 0x44F0A2
        fld dword ptr [esp+10h]
        // 0x44F0A6
        shr eax,10h
        // 0x44F0A9
        imul eax,eax,1Ch
        // 0x44F0AC
        add eax,edx
        // 0x44F0AE
        movsx edx,word ptr [eax+8]
        // 0x44F0B2
        movsx eax,word ptr [eax+0Ah]
        // 0x44F0B6
        mov dword ptr [esp+24h],edx
        // 0x44F0BA
        mov edx,dword ptr [ebp+000005A0h]
        // 0x44F0C0
        fild dword ptr [esp+24h]
        // 0x44F0C4
        mov dword ptr [esp+24h],eax
        // 0x44F0C8
        movzx eax,word ptr [esi+edx]
        // 0x44F0CC
        fmul dword ptr ds:[00858C48h]
        // 0x44F0D2
        fsubp st(1),st
        // 0x44F0D4
        mov edx,eax
        // 0x44F0D6
        fld dword ptr [esp+14h]
        // 0x44F0DA
        shr edx,0Ah
        // 0x44F0DD
        cmp dword ptr [ecx+edx*4+00000804h],0
        // 0x44F0E5
        fild dword ptr [esp+24h]
        // 0x44F0E9
        fmul dword ptr ds:[00858C48h]
        // 0x44F0EF
        fsubp st(1),st
        // 0x44F0F1
        fld st(0)
        // 0x44F0F3
        fmul st,st(1)
        // 0x44F0F5
        fld st(2)
        // 0x44F0F7
        fmul st,st(3)
        // 0x44F0F9
        faddp st(1),st
        // 0x44F0FB
        fsqrt
        // 0x44F0FD
        fstp st(2)
        // 0x44F0FF
        fstp st(0)
        // 0x44F101
        je PathDensity13_44F13E
        // 0x44F103
        mov edx,dword ptr [ecx+edx*4+00000924h]
        // 0x44F10A
        and eax,3FFh
        // 0x44F10F
        imul eax,eax,0Eh
        // 0x44F112
        mov al,byte ptr [edx+eax+0Bh]
        // 0x44F116
        mov dl,al
        // 0x44F118
        and dl,7
        // 0x44F11B
        movzx edx,dl
        // 0x44F11E
        mov dword ptr [esp+24h],edx
        // 0x44F122
        shr al,3
        // 0x44F125
        fild dword ptr [esp+24h]
        // 0x44F129
        and al,7
        // 0x44F12B
        movzx eax,al
        // 0x44F12E
        fmul st,st(1)
        // 0x44F130
        mov dword ptr [esp+24h],eax
        // 0x44F134
        faddp st(2),st
        // 0x44F136
        fild dword ptr [esp+24h]
        // 0x44F13A
        fmul st,st(1)
        // 0x44F13C
        faddp st(2),st
PathDensity13_44F13E:
        fstp st(0)
PathDensity13_44F140:
        add edi,4
        // 0x44F143
        add esi,2
        // 0x44F146
        dec ebx
        // 0x44F147
        jne PathDensity13_44F086
        // 0x44F14D
        mov esi,dword ptr [esp+18h]
PathDensity13_44F151:
        mov eax,dword ptr [esp+1Ch]
        // 0x44F155
        add esi,1Ch
        // 0x44F158
        dec eax
        // 0x44F159
        mov dword ptr [esp+18h],esi
        // 0x44F15D
        mov dword ptr [esp+1Ch],eax
        // 0x44F161
        jne PathDensity13_44F004
PathDensity13_44F167:
        mov eax,dword ptr [esp+20h]
        // 0x44F16B
        add ebp,4
        // 0x44F16E
        dec eax
        // 0x44F16F
        mov dword ptr [esp+20h],eax
        // 0x44F173
        jne PathDensity13_44EFE0
        // 0x44F179
        fmul dword ptr ds:[00858FC0h]
        // 0x44F17F
        pop edi
        // 0x44F180
        pop esi
        // 0x44F181
        pop ebp
        // 0x44F182
        pop ebx
        // 0x44F183
        add esp,18h
        // 0x44F186
        ret 8
    }
}

// TODO: Move into the class itself
auto& s_pathsNeededPosn = StaticRef<CVector>(0x977B70);
auto& s_bLoadPathsNeeded = StaticRef<bool>(0x96F030);

// Como el original (0x44D080..): ToBeStreamed y ToBeStreamedForScript son dos globales de 64 bytes que se indexan por columnas
// ([x * 8 + y]); el id del area (nodes%d.dat) es x + y * 8.
auto& ToBeStreamed = StaticRef<std::array<bool, NUM_PATH_MAP_AREAS>>(0x96EFD0);
auto& ToBeStreamedForScript = StaticRef<std::array<bool, NUM_PATH_MAP_AREAS>>(0x96EF90);
auto& NumTempExternalNodes = StaticRef<int32>(0x96F02C); // Unused, pero Init la pone a cero

auto& XCoorGiven = StaticRef<std::array<float, 64>>(0x96EE80);
auto& YCoorGiven = StaticRef<std::array<float, 64>>(0x96ED80);
auto& ZCoorGiven = StaticRef<std::array<float, 64>>(0x96EC80);
auto& ConnectsToGiven = StaticRef<std::array<std::array<int8, 6>, 64>>(0x96EAC0); // Filas de 6 bytes (0x450E90: ecx * 6)
auto& DontWanderGiven = StaticRef<std::array<uint8, 64>>(0x96EC40);                // 0x450E90: mov [ecx+0096EC40h], bDontWander

auto& aInteriorNodeLinkedToExterior = StaticRef<std::array<int32, NUM_PATH_INTERIOR_AREAS>>(0x96EA98);
auto& aExteriorNodeLinkedTo = StaticRef<std::array<CNodeAddress, NUM_PATH_INTERIOR_AREAS>>(0x977B7C);

auto& aNodesToBeCleared = StaticRef<std::array<CNodeAddress, 5000>>(0x972CD0);

namespace detail {
/*!
* @brief Region (0..7) de una coordenada del mundo, como el original (0x44D830, 0x44D890, 0x44D8C0, 0x44DB60...):
*        (p - (-3000.0)) * 0.0013333333 (0x3AAEC33E, no 1/750 exacto), _ftol2 (trunca) y acotado con signo a [0, 7].
*/
inline int32 RegionValueOf(float p) {
    const auto scaled = (p - std::bit_cast<float>(0xC53B8000u)) * std::bit_cast<float>(0x3AAEC33Eu);
    const auto region = (int32)scaled;
    return region <= 0 ? 0 : (region < 7 ? region : 7);
}
}; // namespace detail

void CPathFind::InjectHooks() {
    RH_ScopedClass(CPathFind);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(AddRandomOffsetToNodePosition, 0x44DA30);

    // Hooks commented out because most of the functions have no definitions
    // thus it's not possible to take their address
    // And I'm lazy to add stubs

    RH_ScopedInstall(AddNodeToNewInterior, 0x450E90);
    RH_ScopedInstall(FindNearestExteriorNodeToInteriorNode, 0x450F30);
    RH_ScopedInstall(ThisNodeHasToBeSwitchedOff, 0x44D3E0);
    RH_ScopedInstall(These2NodesAreAdjacent, 0x44D230);
    RH_ScopedInstall(FindRegionForCoors, 0x44D830);
    RH_ScopedInstall(FindYRegionForCoors, 0x44D8C0);
    RH_ScopedInstall(FindXRegionForCoors, 0x44D890);
    RH_ScopedInstall(AddInteriorLinkToExternalNode, 0x44DF30);
    RH_ScopedInstall(AddInteriorLink, 0x44DED0);
    RH_ScopedInstall(RemoveInteriorLinks, 0x44DF60);
    RH_ScopedInstall(MarkRegionsForCoors, 0x44DB60);
    RH_ScopedOverloadedInstall(FindStartPointOfRegion, "", 0x44D930, void(CPathFind::*)(size_t, size_t, float&, float&));
    RH_ScopedInstall(FindYCoorsForRegion, 0x44D910);
    RH_ScopedInstall(FindXCoorsForRegion, 0x44D8F0);
    RH_ScopedInstall(IsAreaNodesAvailable, 0x420AA0);
    RH_ScopedInstall(HaveRequestedNodesBeenLoaded, 0x450DB0);
    RH_ScopedInstall(MakeRequestForNodesToBeLoaded, 0x450D70);
    RH_ScopedInstall(UpdateStreaming, 0x450A60);
    RH_ScopedInstall(TakeWidthIntoAccountForWandering, 0x4509A0);
    RH_ScopedInstall(Shutdown, 0x450950);
    RH_ScopedOverloadedInstall(FindNodeCoorsForScript, "TwoNodes", 0x450780, CVector(CPathFind::*)(CNodeAddress, CNodeAddress, float&, bool*));
    RH_ScopedOverloadedInstall(FindNodeCoorsForScript, "LinkedNode", 0x4505E0, CVector(CPathFind::*)(CNodeAddress, bool*));
    RH_ScopedInstall(IsWaterNodeNearby, 0x450DE0);
    RH_ScopedInstall(CountNeighboursToBeSwitchedOff, 0x4504F0);
    RH_ScopedInstall(FindNodeOrientationForCarPlacement, 0x450320);
    //RH_ScopedInstall(FindNodePairClosestToCoors, 0x44FEE0);
    RH_ScopedInstall(FindNodeClosestToCoorsFavourDirection, 0x44FCE0);
    //RH_ScopedInstall(FindNodeClosestToCoors, 0x44FA30);
    RH_ScopedInstall(MarkRoadNodeAsDontWander, 0x450560);
    //RH_ScopedInstall(AddDynamicLinkBetween2Nodes, 0x4512D0);
    RH_ScopedOverloadedInstall(LoadPathFindData, "Area", 0x452F40, void(CPathFind::*)(int32));
    RH_ScopedOverloadedInstall(LoadPathFindData, "FromStream", 0x4529F0, void(CPathFind::*)(RwStream*, int32));
    RH_ScopedInstall(SwitchPedRoadsOffInArea, 0x452F00);
    RH_ScopedInstall(SwitchRoadsOffInArea, 0x452C80);
    RH_ScopedInstall(SwitchRoadsOffInAreaForOneRegion, 0x452820);
    RH_ScopedInstall(ComputeRoute, 0x452760);
    //RH_ScopedInstall(CompleteNewInterior, 0x452270);
    RH_ScopedInstall(SwitchOffNodeAndNeighbours, 0x452160);
    //RH_ScopedInstall(Find2NodesForCarCreation, 0x452090);
    //RH_ScopedInstall(TestCoorsCloseness, 0x452000);
    //RH_ScopedInstall(FindNextNodeWandering, 0x451B70);
    RH_ScopedInstall(DoPathSearch, 0x4515D0);
    //RH_ScopedInstall(FindParkingNodeInArea, 0x4513F0);
    RH_ScopedInstall(FindLinkBetweenNodes, 0x451350);
    RH_ScopedInstall(ReturnInteriorNodeIndex, 0x451300);
    //RH_ScopedInstall(FindNthNodeClosestToCoors, 0x44F8C0);
    //RH_ScopedInstall(FindNodeClosestInRegion, 0x44F2C0);
    //RH_ScopedInstall(CalcDistToAnyConnectingLinks, 0x44F190);
    RH_ScopedInstall(CalcRoadDensity, 0x44EFC0);
    RH_ScopedInstall(TestForPedTrafficLight, 0x44D480);
    RH_ScopedInstall(UnMarkAllRoadNodesAsDontWander, 0x44D400);
    RH_ScopedInstall(TidyUpNodeSwitchesAfterMission, 0x44D3B0);
    RH_ScopedInstall(ThisNodeWillLeadIntoADeadEnd, 0x44D310);
    RH_ScopedInstall(AddNodeToList, 0x44D1E0);
    RH_ScopedInstall(RemoveNodeFromList, 0x44D1B0);
    RH_ScopedInstall(UnLoadPathFindData, 0x44D0F0);
    RH_ScopedInstall(Init, 0x44D080);
    RH_ScopedInstall(GetPathNode, 0x420AC0);
    RH_ScopedInstall(TestCrossesRoad, 0x44D790);
    RH_ScopedInstall(ReInit, 0x44E4E0);
    RH_ScopedInstall(RemoveInterior, 0x44E1A0);
    RH_ScopedInstall(AddDynamicLinkBetween2Nodes_For1Node, 0x44E000);
    RH_ScopedInstall(StartNewInterior, 0x44DE80);
    RH_ScopedInstall(LoadSceneForPathNodes, 0x44DE00);
    RH_ScopedInstall(AreNodesLoadedForArea, 0x44DD10);
    RH_ScopedInstall(ReleaseRequestedNodes, 0x44DD00);
    RH_ScopedInstall(SetPathsNeededAtPosition, 0x44DCD0);
    RH_ScopedInstall(SetLinksBridgeLights, 0x44D960);
    RH_ScopedInstall(Save, 0x5D34C0);
    RH_ScopedInstall(Load, 0x5D3500);
}

void CPathNode::InjectHooks() {
    RH_ScopedClass(CPathNode);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(GetPosition, 0x420A10);
}

// 0x44D080
void CPathFind::Init() {
    ZoneScoped;

    // Como el original (0x44D080: mov [0096F02Ch], edx): es la global 0x96F02C, no una variable estatica local.
    NumTempExternalNodes      = 0; // Unused
    m_nNumNodeSwitches        = 0;
    m_loadAreaRequestPending = false;

    for (auto i = 0u; i < NUM_TOTAL_PATH_NODE_AREAS; ++i) {
        m_pPathNodes[i] = nullptr;
        m_pNaviNodes[i] = nullptr;
        m_pNodeLinks[i] = nullptr;
        m_pLinkLengths[i] = nullptr;
        m_pPathIntersections[i] = nullptr;
        m_pNaviLinks[i] = nullptr; // BUG: Out of array bounds write, same as in original code
        m_aTempNodes[i] = nullptr;    // BUG: Out of array bounds write, same as in original code
    }

    rng::fill(m_interiorIDs, (uint32)-1);
}

// 0x44E4E0
void CPathFind::ReInit() {
    m_nNumNodeSwitches       = 0;
    m_loadAreaRequestPending = false;
}

// 0x44DD00
void CPathFind::MakeRequestForNodesToBeLoaded(float minX, float maxX, float minY, float maxY) {
    m_loadAreaRequestPending = true;
    m_loadAreaRequestMinX = minX;
    m_loadAreaRequestMaxX = maxX;
    m_loadAreaRequestMinY = minY;
    m_loadAreaRequestMaxY = maxY;
    UpdateStreaming(true);
}

// 0x44DD10
bool CPathFind::AreNodesLoadedForArea(float minX, float maxX, float minY, float maxY) {
    // Como el original: las cuatro regiones se calculan sin construir un CRect (que en la DLL aborta con limites invertidos)
    // y se recorre x por fuera e y por dentro sobre el area x + y * 8.
    const auto xMin = detail::RegionValueOf(minX);
    const auto yMin = detail::RegionValueOf(minY);
    const auto xMax = detail::RegionValueOf(maxX);
    const auto yMax = detail::RegionValueOf(maxY);
    for (auto x = xMin; x <= xMax; x++) {
        for (auto y = yMin; y <= yMax; y++) {
            if (!m_pPathNodes[x + y * NUM_PATH_MAP_AREA_X]) {
                return false;
            }
        }
    }
    return true;
}

// 0x450DB0
bool CPathFind::HaveRequestedNodesBeenLoaded() {
    return AreNodesLoadedForArea(
        m_loadAreaRequestMinX,
        m_loadAreaRequestMaxX,
        m_loadAreaRequestMinY,
        m_loadAreaRequestMaxY
    );
}

// 0x450950
void CPathFind::Shutdown() {
    for (auto x = 0u; x < NUM_PATH_MAP_AREA_X; ++x) {
        for (auto y = 0u; y < NUM_PATH_MAP_AREA_Y; ++y) {
            auto relativeId = x + y * NUM_PATH_MAP_AREA_X;
            if (m_pPathNodes[relativeId]) {
                CStreaming::RemoveModel(DATToModelId(relativeId));
            }
        }
    }
}

bool CPathFind::ThisNodeWillLeadIntoADeadEnd(CPathNode* startNode, CPathNode* endNode) {
    auto curr = startNode, prev = endNode;
    while (true) {
        CPathNode* next{}; // If node has no links (or neither links area is loaded) this will be nullptr
        for (auto& linked : GetNodeLinkedNodes(*curr)) {
            if (&linked == prev) { // Obviously don't count the previous node
                continue;
            }
            // Como el original (0x44D36B..0x44D378): shr dl,4; test dl,dl; jbe -> cuenta; cmp dl,0Ah; jbe -> no cuenta.
            // Es decir, cuenta el comportamiento 0 y los mayores que 10 (antes decia == 4).
            if (linked.m_nBehaviourType == 0u || linked.m_nBehaviourType > 10u) { // TODO: Enum?
                // I'm unsure what's happening here
                // I think, since this function isn't recursive, they just
                // consider having 2 appropriate links as a non-deadend
                if (next) {
                    return false;
                }
                next = &linked;
            }
        }
        if (!next) { 
            return true;
        }
        prev = curr;
        curr = next;
    }
}

// 0x44D3B0
void CPathFind::TidyUpNodeSwitchesAfterMission() {
    // Como el original (0x44D3B6: cmp eax,36h; jl): comparacion con signo
    m_nNumNodeSwitches = (uint32)std::min<int32>(54, (int32)m_nNumNodeSwitches); // todo: magic number
}

// 0x44D400
void CPathFind::UnMarkAllRoadNodesAsDontWander() {
    for (auto i = 0u; i < NUM_PATH_MAP_AREAS; ++i) {
        if (!m_pPathNodes[i])
            continue;

        for (auto nodeInd = 0u; nodeInd < m_anNumVehicleNodes[i]; ++nodeInd) {
            m_pPathNodes[i][nodeInd].m_bDontWander = false;
        }
    }
}

// 0x44DD00
void CPathFind::ReleaseRequestedNodes() {
    m_loadAreaRequestPending = false;
}

/*!
* @brief Find intersection info between 2 nodes
* @addr notsa 100% inlined
*/
auto CPathFind::FindIntersection(const CNodeAddress& startNodeAddress, const CNodeAddress& targetNodeAddress) -> CPathIntersectionInfo* {
    // Make sure both nodes areas are loaded
    if (!AreNodeAreasLoaded({ targetNodeAddress, startNodeAddress })) {
        return nullptr;
    }

    const auto& startNode = *GetPathNode(startNodeAddress);
    const auto& nodeLinks = m_pNodeLinks[startNodeAddress.m_wAreaId];
    for (auto i = 0u; i < startNode.m_nNumLinks; i++) {
        const auto linkedNodeIdx = startNode.m_wBaseLinkId + i;
        if (nodeLinks[linkedNodeIdx] == targetNodeAddress) {
            return &m_pPathIntersections[startNodeAddress.m_wAreaId][linkedNodeIdx];
        }
    }

    return nullptr;
}

// 0x44D790
bool CPathFind::TestCrossesRoad(CNodeAddress startNodeAddress, CNodeAddress targetNodeAddress) {
    const auto intersect = FindIntersection(startNodeAddress, targetNodeAddress);
    return intersect && intersect->m_bRoadCross;
}

// 0x44ECA0
bool CPathFind::GeneratePedCreationCoors_Interior(float x, float y, CVector* outCoords, CNodeAddress* unused1, CNodeAddress* unused2, float* outOrientation) {
    return plugin::CallMethodAndReturn<bool, 0x44ECA0>(this, x, y, outCoords, unused1, unused2, outOrientation);
}

// 0x44D480
bool CPathFind::TestForPedTrafficLight(CNodeAddress startNodeAddress, CNodeAddress targetNodeAddress) {
    const auto intersect = FindIntersection(startNodeAddress, targetNodeAddress);
    return intersect && intersect->m_bPedTrafficLight;
}

// 0x4509A0
CVector CPathFind::TakeWidthIntoAccountForWandering(CNodeAddress nodeAddress, int16 randomSeed) {
    // Como el original (0x4509A9..0x4509BF): solo comprueba el area (cmp ax,0FFFFh) y que este cargada, no el nodo
    if (!nodeAddress.IsAreaValid() || !IsAreaNodesAvailable(nodeAddress)) {
        return CVector{ 0.f, 0.f, 0.f };
    }

    const auto node         = &m_pPathNodes[nodeAddress.m_wAreaId][nodeAddress.m_wNodeId];
    const auto basePosition = node->GetPosition();
    // Como el original (0x4509DE..0x4509FC): movsx de la semilla, `& 0Fh` y `sar eax,4; & 0Fh` (con `%` y `/` de C++ las semillas
    // negativas daban otro resultado), producto entero con la anchura y fild + fmul 0.00775 (0x3BFDF3B6) + fadd.
    const int32 seed    = randomSeed;
    const int32 offsetX = ((seed & 0xF) - 7) * node->m_nPathWidth;        // bottom 4 bits remapped to [-7 : +8]
    const int32 offsetY = (((seed >> 4) & 0xF) - 7) * node->m_nPathWidth; // next 4 bits remapped to [-7 : +8]
    const float scale   = std::bit_cast<float>(0x3BFDF3B6u);
    return CVector{ (float)offsetX * scale + basePosition.x, (float)offsetY * scale + basePosition.y, basePosition.z };
}

//  0x44F8C0
CNodeAddress CPathFind::FindNthNodeClosestToCoors(CVector pos, uint8 nodeType, float maxDistance, bool bLowTraffic, bool bUnkn, int32 nthNode, bool bBoatsOnly,
                                                  bool bIgnoreInterior, CNodeAddress* outNode) {
    CNodeAddress outAddress;
    plugin::CallMethod<0x44F8C0, CPathFind*, CNodeAddress*, CVector, uint8, float, bool, bool, int32, bool, bool, CNodeAddress*>(
        this, &outAddress, pos, nodeType, maxDistance, bLowTraffic, bUnkn, nthNode, bBoatsOnly, bIgnoreInterior, outNode);
    return outAddress;
}

// 0x451B70
void CPathFind::FindNextNodeWandering(uint8 nodeType, CVector vecPos, CNodeAddress* originAddress, CNodeAddress* targetAddress, uint8 dir, uint8* outDir) {
    plugin::CallMethod<0x451B70, CPathFind*, uint8, CVector, CNodeAddress*, CNodeAddress*, uint8, uint8*>(this, nodeType, vecPos, originAddress, targetAddress, dir, outDir);
}

// 0x4515D0
void CPathFind::DoPathSearch(
    ePathType pathType,
    CVector originPos,
    CNodeAddress originAddrAddrHint, // If invalid/area not loaded the closest node to `originPos` is used.
    CVector targetPos,
    CNodeAddress* outResultNodes,
    int16& outNodesCount,
    int32 maxNodesToFind,
    float* outDistance,
    float maxSearchDistance,
    CNodeAddress* targetNodeAddrHint, // If null/invalid/area not loaded the closest node to `targetPos` is used.
    float maxSearchDepth,
    bool sameLaneOnly,
    CNodeAddress forbiddenNodeAddr,
    bool bAllowWaterNodeTransitions,
    bool forBoats
) {
    // Como el original: resolver primero el destino y reiniciar solo el contador de la lista.
    m_totalNumNodesInPathFindHashTable = 0;
    const auto resolve = [&](CVector pos, CNodeAddress* hint) {
        if (hint && hint->IsAreaValid() && IsAreaNodesAvailable(*hint)) {
            return *hint;
        }
        return FindNodeClosestToCoors(pos, pathType, maxSearchDistance, 0, 0, 0, forBoats, 0);
    };
    const auto targetAddr = resolve(targetPos, targetNodeAddrHint);
    const auto fail = [&] {
        outNodesCount = 0;
        if (outDistance) *outDistance = 100000.0f;
    };
    if (!targetAddr.IsAreaValid()) { fail(); return; }
    const auto originAddr = resolve(originPos, &originAddrAddrHint);
    if (!originAddr.IsAreaValid()) { fail(); return; }
    if (originAddr == targetAddr) {
        outNodesCount = 0;
        if (outDistance) *outDistance = 0.0f;
        return;
    }
    auto* origin = GetPathNode(originAddr);
    auto* target = GetPathNode(targetAddr);
    if (origin->m_nFloodFill != target->m_nFloodFill) { fail(); return; }
    rng::fill(m_pathFindHashTable, nullptr);
    // Como el original: operaciones de lista en linea y bucket con mascara de 9 bits.
    const auto remove = [&](CPathNode* node) {
        node->m_prev->m_next = node->m_next;
        if (node->m_next) node->m_next->m_prev = node->m_prev;
        --m_totalNumNodesInPathFindHashTable;
    };
    const auto add = [&](CPathNode* node, int32 distance) {
        auto& head = m_pathFindHashTable[distance & 511];
        node->m_next = head;
        node->m_prev = reinterpret_cast<CPathNode*>(&head);
        if (head) head->m_prev = node;
        head = node;
        node->m_totalDistFromOrigin = static_cast<int16>(distance);
        ++m_totalNumNodesInPathFindHashTable;
    };
    add(target, 0);
    int32 clearCount = 1;
    aNodesToBeCleared[0] = target->GetAddress();
    int32 depth = 0;
    bool finished = false;
    for (;;) {
        for (auto* node = m_pathFindHashTable[depth & 511]; node; node = node->m_next) {
            if (node == origin) finished = true;
            for (int32 linkNum = 0; linkNum < static_cast<int32>(node->m_nNumLinks); ++linkNum) {
                const auto linkIdx = node->m_wBaseLinkId + linkNum;
                const auto linkedAddr = m_pNodeLinks[node->m_wAreaId][linkIdx];
                if (!IsAreaNodesAvailable(linkedAddr)) continue;
                auto* linked = GetPathNode(linkedAddr);
                bool forbidden = false;
                if (sameLaneOnly) {
                    const auto naviAddr = m_pNaviLinks[node->m_wAreaId][linkIdx];
                    if (IsAreaLoaded(naviAddr.m_wAreaId)) {
                        const auto& navi = GetCarPathLink(naviAddr);
                        forbidden = navi.m_attachedTo == linkedAddr
                            ? navi.m_numOppositeDirLanes == 0
                            : navi.m_numSameDirLanes == 0;
                    }
                }
                if (forbiddenNodeAddr == linkedAddr) forbidden = true;
                if ((node->m_bWaterNode != linked->m_bWaterNode && !bAllowWaterNodeTransitions) || forbidden) continue;
                const int32 distance = node->m_totalDistFromOrigin + m_pLinkLengths[node->m_wAreaId][linkIdx];
                if (distance >= linked->m_totalDistFromOrigin) continue;
                if (linked->m_totalDistFromOrigin != 32766) remove(linked);
                if (linked->m_totalDistFromOrigin == 32766 && clearCount < 5000) {
                    aNodesToBeCleared[clearCount++] = linked->GetAddress();
                }
                add(linked, distance);
            }
            remove(node);
        }
        ++depth;
        // Como el original: un fallo despues de explorar deja outDistance intacto.
        if (!m_totalNumNodesInPathFindHashTable || static_cast<float>(depth) > maxSearchDepth || clearCount >= 4950) {
            outNodesCount = 0;
            break;
        }
        if (!finished) continue;
        outNodesCount = 0;
        if (outDistance) *outDistance = static_cast<float>(origin->m_totalDistFromOrigin);
        if (outResultNodes) outResultNodes[outNodesCount++] = origin->GetAddress();
        auto* node = origin;
        while (outNodesCount < maxNodesToFind && node != target) {
            for (int32 linkNum = 0; linkNum < static_cast<int32>(node->m_nNumLinks); ++linkNum) {
                const auto linkIdx = node->m_wBaseLinkId + linkNum;
                const auto linkedAddr = m_pNodeLinks[node->m_wAreaId][linkIdx];
                if (!IsAreaNodesAvailable(linkedAddr)) continue;
                auto* linked = GetPathNode(linkedAddr);
                if (node->m_totalDistFromOrigin - m_pLinkLengths[node->m_wAreaId][linkIdx] == linked->m_totalDistFromOrigin) {
                    outResultNodes[outNodesCount++] = linked->GetAddress();
                    node = linked;
                    break;
                }
            }
        }
        break;
    }
    for (int32 i = 0; i < clearCount; ++i) GetPathNode(aNodesToBeCleared[i])->m_totalDistFromOrigin = 32766;
}

// 0x452760
void CPathFind::ComputeRoute(uint8 nodeType, const CVector& vecStart, const CVector& vecEnd, const CNodeAddress& startAddress, CNodeRoute* route) {
    CNodeAddress nodes[8];
    int16 count = 0;
    DoPathSearch(static_cast<ePathType>(nodeType), vecStart, startAddress, vecEnd, nodes, count, 8,
        nullptr, std::bit_cast<float>(0x497423FEu), nullptr, std::bit_cast<float>(0x497423FEu),
        false, *reinterpret_cast<const CNodeAddress*>(0x8A5F44), false, false);
    route->Clear();
    for (int32 i = 0; i < count; ++i) {
        if (!route->IsFull()) route->Add(nodes[i]);
    }
}

// 0x44D960
void CPathFind::SetLinksBridgeLights(float fXMin, float fXMax, float fYMin, float fYMax, bool value) {
    // Como el original (0x44D9B0..0x44D9F6): sin CRect (abortaria con limites invertidos) y con comparaciones estrictas y
    // ordenadas (fcom + test ah,41h / test ah,5): xMin < x < xMax e yMin < y < yMax; un NaN en un limite excluye el enlace.
    // El indice del enlace se usa tal cual (con CCarPathLinkAddress se truncaba a 10 bits).
    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        if (!IsAreaLoaded(areaId)) {
            continue;
        }

        for (auto n = 0; n < (int32)m_anNumCarPathLinks[areaId]; n++) {
            auto& node = m_pNaviNodes[areaId][n];
            const auto raw = reinterpret_cast<const int16*>(&node); // m_posn: x, y (int16, 1/8)
            const float x = (float)raw[0] * 0.125f;
            const float y = (float)raw[1] * 0.125f;
            if (x > fXMin && x < fXMax && y > fYMin && y < fYMax) {
                node.m_bridgeLights = value;
            }
        }
    }
}

namespace detail {
// Constantes del EXE (0x858620 = 0.0625, 0x859AA8 = 0x402CCCCD, 0x859878 = 0x42652EE0)
constexpr float kScriptWidthStep   = 0.0625f;
inline float ScriptWidthScale(uint8 width) { return (float)width * kScriptWidthStep + std::bit_cast<float>(0x402CCCCDu); }

// Como el original: v * escala (0x40FEC0) y suma de vectores (0x40FE30), componente a componente en float.
inline CVector ScriptOffsetPos(const CVector& nodePos, float px, float py, float scale) {
    return CVector{ nodePos.x + px * scale, nodePos.y + py * scale, nodePos.z + 0.f * scale };
}
}; // namespace detail

// 0x4505E0
CVector CPathFind::FindNodeCoorsForScript(CNodeAddress address, bool* bFound) {
    const auto SetFound = [&](bool found) {
        if (bFound) {
            *bFound = found;
        }
    };
    // Como el original (0x4505EF..0x450603): solo se comprueba el area (cmp ax,0FFFFh) y que este cargada
    if (!address.IsAreaValid() || !IsAreaNodesAvailable(address)) {
        SetFound(false);
        return CVector{ 0.f, 0.f, 0.f };
    }
    SetFound(true);

    const auto node    = &m_pPathNodes[address.m_wAreaId][address.m_wNodeId];
    const auto nodePos = node->GetPosition();

    // Como el original (0x45062B..0x450675): sin anchura o sin enlaces, o con el area del primer enlace sin cargar, devuelve la posicion
    if (node->m_nPathWidth != 0 && node->m_nNumLinks != 0) {
        // Antes se indexaba m_pNodeLinks (el array de punteros por area) con el id del enlace base; el original lee el primer
        // enlace del array de enlaces del area del nodo (mov eax,[edx+eax*4], edx = m_pNodeLinks[area]).
        const auto firstLink = m_pNodeLinks[address.m_wAreaId][node->m_wBaseLinkId]; // el area de la direccion, no el campo del nodo
        if (IsAreaNodesAvailable(firstLink)) {
            const auto linkedPos = (&m_pPathNodes[firstLink.m_wAreaId][firstLink.m_wNodeId])->GetPosition();

            // Normalise inline (0x44E480): x*x + y*y; si es <= 0, x = 1.0 e y se queda; si no, * 1/sqrt
            float dirX = linkedPos.x - nodePos.x;
            float dirY = linkedPos.y - nodePos.y;
            const float sqLen = dirX * dirX + dirY * dirY;
            if (sqLen <= 0.f) {
                dirX = 1.f;
            } else {
                const float inv = 1.f / std::sqrt(sqLen);
                dirX = inv * dirX;
                dirY = inv * dirY;
            }

            // fcom 0.0 + test ah,5 + jp: con x < 0 se niega el vector (un NaN cuenta como no negativo); despues el perpendicular izquierdo
            float perpX, perpY;
            if (dirX < 0.f) {
                perpX = dirY;
                perpY = -dirX;
            } else {
                perpX = -dirY;
                perpY = dirX;
            }
            return detail::ScriptOffsetPos(nodePos, perpX, perpY, detail::ScriptWidthScale(node->m_nPathWidth));
        }
    }

    // Otherwise just return this node's position
    return nodePos;
}

// 0x450780
CVector CPathFind::FindNodeCoorsForScript(CNodeAddress nodeAddrA, CNodeAddress nodeAddrB, float& outHeadingDeg, bool* outFound) {
    const auto SetFound = [&](bool found) {
        if (outFound) {
            *outFound = found;
        }
    };
    // Como el original (0x450784..0x4507C3): se comprueban las areas (cmp ax,0FFFFh) y que esten cargadas
    if (!nodeAddrA.IsAreaValid() || !nodeAddrB.IsAreaValid() || !AreNodeAreasLoaded({ nodeAddrA, nodeAddrB })) {
        SetFound(false);
        return CVector{ 0.f, 0.f, 0.f };
    }
    SetFound(true);

    const auto nodeA = &m_pPathNodes[nodeAddrA.m_wAreaId][nodeAddrA.m_wNodeId];
    const auto nodeB = &m_pPathNodes[nodeAddrB.m_wAreaId][nodeAddrB.m_wNodeId];
    const auto posA  = nodeA->GetPosition();
    const auto posB  = nodeB->GetPosition();
    const float dx   = posB.x - posA.x;
    const float dy   = posB.y - posA.y;

    // Como el original (0x450833..0x45084C): fchs; fpatan; fmul 57.29578 (0x42652EE0); fstp. Un solo redondeo, con la direccion
    // sin normalizar (antes se normalizaba y se llamaba a std::atan2).
    {
        const float headNegDx = -dx;
        const float headDy    = dy;
        const float headDeg   = std::bit_cast<float>(0x42652EE0u);
        float       headOut = 0.f;
        __asm {
            fld  dword ptr [headNegDx]
            fld  dword ptr [headDy]
            fpatan
            fmul dword ptr [headDeg]
            fstp dword ptr [headOut]
        }
        outHeadingDeg = headOut;
    }

    if (nodeA->m_nPathWidth == 0) {
        return posA;
    }

    // Como el original (0x4508B4: call 0x59C910 = CVector::Normalise de 3 componentes): el vector normalizado es (dx, dy, posA.z); la z
    // es la del nodo A (se quedo en la pila al leer su posicion). Si el cuadrado es <= 0, x = 1.0 y el resto se queda.
    float dirX = dx;
    float dirY = dy;
    {
        const float sqLen = dirX * dirX + dirY * dirY + posA.z * posA.z;
        if (sqLen <= 0.f) {
            dirX = 1.f;
        } else {
            const float inv = 1.f / std::sqrt(sqLen);
            dirX = inv * dirX;
            dirY = inv * dirY;
        }
    }
    // Perpendicular derecho (dir.y, -dir.x, 0), sin invertir el sentido como en la otra variante
    return detail::ScriptOffsetPos(posA, dirY, -dirX, detail::ScriptWidthScale(nodeA->m_nPathWidth));
}

// 0x450560
void CPathFind::MarkRoadNodeAsDontWander(float x, float y, float z) {
    CVector pos = {x, y, z};
    auto node = FindNodeClosestToCoors(pos, PATH_TYPE_VEH, 999999.88f, 0, 0, 0, 0, 0);
    // Como el original (0x4505B8: cmp ax,0FFFFh; 0x4505D4: or byte [eax],4): solo se comprueba el area
    if (node.IsAreaValid()) {
        m_pPathNodes[node.m_wAreaId][node.m_wNodeId].m_bDontWander = true;
    }
}

// 0x452820
void CPathFind::SwitchRoadsOffInAreaForOneRegion(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bSwitchOff, bool bCars, int areaId, bool bBackToOriginal) {
    assert(areaId >= 0 && areaId < NUM_PATH_MAP_AREAS);

    if (!IsAreaLoaded(areaId)) {
        return;
    }

    auto start = bCars ? 0 : m_anNumVehicleNodes[areaId];
    auto end   = bCars ? m_anNumVehicleNodes[areaId] : m_anNumNodes[areaId];

    for (auto nodeIdx = start; nodeIdx < end; ++nodeIdx) {
        CPathNode& node     = m_pPathNodes[areaId][nodeIdx];
        const auto position = node.GetPosition();

        // Como el original (0x4528A0..0x452920): comparaciones ordenadas (fcom + test ah,1 / 41h); un NaN en un limite excluye el nodo
        if (!(position.x >= xMin && position.x <= xMax && position.y >= yMin && position.y <= yMax && position.z >= zMin && position.z <= zMax)) {
            continue;
        }
        if (!ThisNodeHasToBeSwitchedOff(&node) || node.m_isSwitchedOff == (bBackToOriginal ? node.m_isSwitchedOffOriginal : bSwitchOff)) {
            continue;
        }
        CPathNode* next1{};
        CPathNode* next2{};

        SwitchOffNodeAndNeighbours(&node, next1, &next2, bSwitchOff, bBackToOriginal);

        for (auto iter = next1; iter;) {
            SwitchOffNodeAndNeighbours(iter, iter, nullptr, bSwitchOff, bBackToOriginal);
        }
        for (auto iter = next2; iter;) {
            SwitchOffNodeAndNeighbours(iter, iter, nullptr, bSwitchOff, bBackToOriginal);
        }
    }
}

// NOTSA
CPathNode* CPathFind::GetPathNode(CNodeAddress address) {
    assert(address.IsValid());
    assert(IsAreaNodesAvailable(address));
    return &m_pPathNodes[address.m_wAreaId][address.m_wNodeId];
}

// notsa
CCarPathLinkAddress CPathFind::GetNaviLink(uint16 area, uint16 linkId) const {
    assert(area < NUM_PATH_MAP_AREAS);
    assert(linkId < m_anNumAddresses[area]);
    return m_pNaviLinks[area][linkId];
}

// NOTSA
bool CPathFind::FindNodeCoorsForScript(CVector& outPos, CNodeAddress addr) {
    bool valid{};
    outPos = FindNodeCoorsForScript(addr, &valid);
    return valid;
}

// 0x452F40
void CPathFind::LoadPathFindData(int32 areaId) {
    CTimer::Suspend();
    // Como el original (0x452F59: sprintf del CRT): sprintf_s rellenaba el resto de gString con 0xFE en depuracion
    sprintf(gString, "data\\paths\\nodes%d.dat", areaId);
    auto* stream = RwStreamOpen(RwStreamType::rwSTREAMFILENAME, RwStreamAccessType::rwSTREAMREAD, gString);
    LoadPathFindData(stream, areaId);
    // Como el original (0x452F7D: RwStreamClose(stream, 0)): antes no se cerraba el flujo
    RwStreamClose(stream, nullptr);
    CTimer::Resume();
}

// 0x4529F0
void CPathFind::LoadPathFindData(RwStream* stream, int32 areaId) {
    RwStreamRead(stream, &m_anNumNodes[areaId],        sizeof(m_anNumNodes[areaId]));
    RwStreamRead(stream, &m_anNumVehicleNodes[areaId], sizeof(m_anNumVehicleNodes[areaId]));
    RwStreamRead(stream, &m_anNumPedNodes[areaId],     sizeof(m_anNumPedNodes[areaId]));
    RwStreamRead(stream, &m_anNumCarPathLinks[areaId], sizeof(m_anNumCarPathLinks[areaId]));
    RwStreamRead(stream, &m_anNumAddresses[areaId],    sizeof(m_anNumAddresses[areaId]));

    assert(m_anNumNodes[areaId] == m_anNumVehicleNodes[areaId] + m_anNumPedNodes[areaId]);

    // Como el original (0x452A5A..0x452BA0): la memoria sale de CMemoryMgr::Malloc (0x72F420, sin constructores ni relleno) y se
    // libera con CMemoryMgr::Free (0x72F430); antes se usaba new[] con constructores. Un area sin nodos reserva un CPathNode.
    auto numNodes = m_anNumNodes[areaId];
    if (numNodes) {
        m_pPathNodes[areaId] = static_cast<CPathNode*>(CMemoryMgr::Malloc(numNodes * sizeof(CPathNode), 0));
        RwStreamRead(stream, m_pPathNodes[areaId], sizeof(CPathNode) * numNodes);
    } else {
        m_pPathNodes[areaId] = static_cast<CPathNode*>(CMemoryMgr::Malloc(sizeof(CPathNode), 0));
    }

    auto numCarPathLinks = m_anNumCarPathLinks[areaId];
    if (numCarPathLinks) {
        m_pNaviNodes[areaId] = static_cast<CCarPathLink*>(CMemoryMgr::Malloc(numCarPathLinks * sizeof(CCarPathLink), 0));
        RwStreamRead(stream, m_pNaviNodes[areaId], sizeof(CCarPathLink) * numCarPathLinks);
    } else {
        m_pNaviNodes[areaId] = nullptr;
    }

    auto numAddresses = m_anNumAddresses[areaId];
    if (numAddresses) {
        auto numToAdd = numAddresses + NUM_DYNAMIC_LINKS_PER_AREA * 12;
        m_pNodeLinks[areaId] = static_cast<CNodeAddress*>(CMemoryMgr::Malloc(numToAdd * sizeof(CNodeAddress), 0));
        m_pNaviLinks[areaId] = static_cast<CCarPathLinkAddress*>(CMemoryMgr::Malloc(numAddresses * sizeof(CCarPathLinkAddress), 0));
        m_pLinkLengths[areaId] = static_cast<uint8*>(CMemoryMgr::Malloc(numToAdd * sizeof(uint8), 0));
        m_pPathIntersections[areaId] = static_cast<CPathIntersectionInfo*>(CMemoryMgr::Malloc(numToAdd * sizeof(CPathIntersectionInfo), 0));
        RwStreamRead(stream, m_pNodeLinks[areaId], sizeof(CNodeAddress) * numToAdd);
        RwStreamRead(stream, m_pNaviLinks[areaId], sizeof(CCarPathLinkAddress) * numAddresses);
        RwStreamRead(stream, m_pLinkLengths[areaId], sizeof(uint8) * numToAdd);
        RwStreamRead(stream, m_pPathIntersections[areaId], sizeof(CPathIntersectionInfo) * numToAdd);
    } else {
        m_pNodeLinks[areaId] = nullptr;
        m_pNaviLinks[areaId] = nullptr;
        m_pLinkLengths[areaId] = nullptr;
        m_pPathIntersections[areaId] = nullptr;
    }

    for (auto i = 0u; i < numNodes; ++i) {
        auto& node = m_pPathNodes[areaId][i];
        node.m_isSwitchedOffOriginal = node.m_isSwitchedOff;
    }

    for (auto i = 0u; i < m_nNumNodeSwitches; ++i) {
        auto& area = m_aNodeSwitches[i];
        SwitchRoadsOffInAreaForOneRegion(area.xMin, area.xMax, area.yMin, area.yMax, area.zMin, area.zMax, area.isOff, area.isCars, areaId, false);
    }
    // Como el original (0x452C4B..0x452C6D): solo se reinician los 16 enlaces dinamicos de ESTA area
    // (antes se recorrian las filas 0..15, es decir, otras 16 areas)
    rng::fill(m_aDynamicLinksBaseIds[areaId], -1);
    rng::fill(m_aDynamicLinksIds[areaId], -1);
}

// 0x44D0F0
void CPathFind::UnLoadPathFindData(int32 index) {
    // Como el original (0x44D0F9..0x44D170): CMemoryMgr::Free (0x72F430) solo de los punteros no nulos, en este orden
    for (void* ptr : { (void*)m_pPathNodes[index], (void*)m_pNaviNodes[index], (void*)m_pNodeLinks[index], (void*)m_pNaviLinks[index],
                       (void*)m_pLinkLengths[index], (void*)m_pPathIntersections[index] }) {
        if (ptr) {
            CMemoryMgr::Free(ptr);
        }
    }

    m_pPathNodes[index] = nullptr;
    m_pNaviNodes[index] = nullptr;
    m_pNodeLinks[index] = nullptr;
    m_pNaviLinks[index] = nullptr;
    m_pLinkLengths[index] = nullptr;
    m_pPathIntersections[index] = nullptr;
}

// 0x44DE00
void CPathFind::LoadSceneForPathNodes(CVector point) {
    rng::fill(ToBeStreamed, false);
    MarkRegionsForCoors(point, 350.f);
    // Como el original (0x44DE3B..0x44DE60): ToBeStreamed se indexa por columnas, [x * 8 + y], y se recorre x por fuera;
    // el modelo es el del area x + y * 8
    for (auto x = 0; x < NUM_PATH_MAP_AREA_X; x++) {
        for (auto y = 0; y < NUM_PATH_MAP_AREA_Y; y++) {
            if (ToBeStreamed.data()[x * NUM_PATH_MAP_AREA_Y + y]) {
                CStreaming::RequestModel(DATToModelId(x + y * NUM_PATH_MAP_AREA_X), STREAMING_DEFAULT);
            }
        }
    }
}

// 0x450DE0
bool CPathFind::IsWaterNodeNearby(CVector position, float radius) {
    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        if (!m_pPathNodes[areaId]) {
            continue;
        }
        for (auto nodeIdx = 0; nodeIdx < (int32)m_anNumVehicleNodes[areaId]; nodeIdx++) {
            const auto& node = m_pPathNodes[areaId][nodeIdx];
            if (!node.m_bWaterNode) {
                continue;
            }
            // Como el original (0x450E1E..0x450E5B): distancia 2D (la z no cuenta), sqrt(dy*dy + dx*dx) y comparacion estricta
            // `dist < radius` (fcomp + test ah,5 + jnp). Antes se comparaba el cuadrado 3D con `<=`.
            const auto  nodePos = node.GetPosition();
            const float dx      = position.x - nodePos.x;
            const float dy      = position.y - nodePos.y;
            if (std::sqrt(dy * dy + dx * dx) < radius) {
                return true;
            }
        }
    }
    return false;
}

// 0x44F460
CNodeAddress CPathFind::FindNodeClosestToCoors(
    CVector pos,
    ePathType nodeType,
    float maxDistance,
    uint16 unk2,
    int32 unk3,
    uint16 unk4,
    uint16 bBoatsOnly,
    int32 unk6
) {
    CNodeAddress tempAddress;
    plugin::CallMethodAndReturn<CNodeAddress*, 0x44F460, CPathFind*, CNodeAddress*, CVector, ePathType, float, uint16, int32, uint16, uint16, int32>(
        this, &tempAddress, pos, nodeType, maxDistance, unk2, unk3, unk4, bBoatsOnly, unk6);
    return tempAddress;
}

// 0x450A60
void CPathFind::UpdateStreaming(bool bForceStreaming) {
    ZoneScoped;

    // The time thingy I think is some kind of `% 512`, not sure yet, will have to figure it out.
    if (!s_bLoadPathsNeeded && !bForceStreaming && (CTimer::m_snTimeInMilliseconds ^ CTimer::m_snPreviousTimeInMilliseconds) < 512) {
        return;
    }

    // Como el original (0x450A94..0x450AAE): se ponen a cero ToBeStreamed (0x96EFD0) y ToBeStreamedForScript (0x96EF90), dos globales
    rng::fill(ToBeStreamed, false);
    rng::fill(ToBeStreamedForScript, false);

    // Mark areas around the player
    if (FindPlayerPed()) {
        MarkRegionsForCoors(FindPlayerCoors(), 350.f);
    }

    // Mark areas requested by `SetPathsNeededAtPosition`
    if (s_bLoadPathsNeeded) {
        MarkRegionsForCoors(s_pathsNeededPosn, 300.f);
        s_bLoadPathsNeeded = false;
    }

    // Mark paths around some specific mission vehicles
    // Como el original (0x450B26..0x450BB2): el pool se recorre de la ultima ranura a la primera
    auto* const vehiclePool = GetVehiclePool();
    for (auto slot = (int32)vehiclePool->GetSize() - 1; slot >= 0; slot--) {
        const auto* veh = vehiclePool->GetAt(slot);
        if (!veh) {
            continue;
        }

        switch (veh->m_nVehicleSubType) {
        case VEHICLE_TYPE_HELI:
        case VEHICLE_TYPE_PLANE:
        case VEHICLE_TYPE_BOAT:
        case VEHICLE_TYPE_TRAIN:
        case VEHICLE_TYPE_FPLANE:
            continue;
        default:
            break;
        }
        if (!veh->IsMissionVehicle()) {
            continue;
        }
        MarkRegionsForCoors(veh->GetPosition(), 300.f);
    }

    // Mark areas inside load request rect
    // Como el original (0x450BB4..0x450CDC): cuatro regiones sueltas y rellenado por columnas ([x * 8 + y]) de ambos arrays
    if (m_loadAreaRequestPending) {
        const auto xMin = detail::RegionValueOf(m_loadAreaRequestMinX);
        const auto xMax = detail::RegionValueOf(m_loadAreaRequestMaxX);
        const auto yMin = detail::RegionValueOf(m_loadAreaRequestMinY);
        const auto yMax = detail::RegionValueOf(m_loadAreaRequestMaxY);
        for (auto x = xMin; x <= xMax; x++) {
            for (auto y = yMin; y <= yMax; y++) {
                ToBeStreamedForScript.data()[x * NUM_PATH_MAP_AREA_Y + y] = true;
                ToBeStreamed.data()[x * NUM_PATH_MAP_AREA_Y + y] = true;
            }
        }
    }

    // Load/unload areas as per `ToBeStreamed`
    // Como el original (0x450CE0..0x450D5E): x por fuera e y por dentro; el modelo es el del area x + y * 8 y los avisos de
    // depuracion de la DLL no existen en el EXE
    for (auto x = 0; x < NUM_PATH_MAP_AREA_X; x++) {
        for (auto y = 0; y < NUM_PATH_MAP_AREA_Y; y++) {
            const auto areaId      = x + y * NUM_PATH_MAP_AREA_X;
            const auto colMajorIdx = x * NUM_PATH_MAP_AREA_Y + y;
            if (ToBeStreamed.data()[colMajorIdx]) {
                if (!IsAreaLoaded(areaId)) {
                    CStreaming::RequestModel(
                        DATToModelId(areaId),
                        ToBeStreamedForScript.data()[colMajorIdx]
                            ? STREAMING_MISSION_REQUIRED
                            : STREAMING_KEEP_IN_MEMORY
                    );
                }
            } else if (IsAreaLoaded(areaId)) {
                CStreaming::RemoveModel(DATToModelId(areaId));
            }
        }
    }
}

// 0x44DE80
void CPathFind::StartNewInterior(int32 interiorNum) {
    InteriorIDBeingBuilt = interiorNum;
    bInteriorBeingBuilt = true;
    NumNodesGiven = 0;
    NumLinksToExteriorNodes = 0;

    // BUG: Possible endless loop if 8 interiors are loaded i think
    NewInteriorSlot = 0;
    while (m_interiorIDs[NewInteriorSlot] != (uint32)-1) {
        NewInteriorSlot++;
        assert(NewInteriorSlot < 8);
    }
}

// 0x450E90
CNodeAddress CPathFind::AddNodeToNewInterior(
    float x,
    float y,
    float z,
    bool bDontWander,
    int8 con0,
    int8 con1,
    int8 con2,
    int8 con3,
    int8 con4,
    int8 con5
) {
    const auto idx = NumNodesGiven++;
    XCoorGiven[idx] = x;
    YCoorGiven[idx] = y;
    ZCoorGiven[idx] = z;
    // Como el original (0x450EA9..0x450EDA): el parametro bDontWander se guarda en el array de bytes 0x96EC40 (antes se ignoraba)
    DontWanderGiven[idx] = bDontWander;
    rng::copy(std::array{ con0, con1, con2, con3, con4, con5 }, ConnectsToGiven[idx].begin());
    return { (uint16)(NUM_PATH_MAP_AREAS + NewInteriorSlot), (uint16)idx };
}

// 0x451300 unused
// Como el original (ret 0Ch: puntero de retorno + intId + nodeId): no tiene el parametro `unkn` que tenia la DLL (descuadraba la pila).
CNodeAddress CPathFind::ReturnInteriorNodeIndex(uint32 intId, int16 nodeId) {
    for (auto i = 0; i < NUM_PATH_INTERIOR_AREAS; ++i) {
        if (m_interiorIDs[i] == intId) {
            return CNodeAddress((uint16)(NUM_PATH_MAP_AREAS + i), (uint16)nodeId);
        }
    }
    // Como el original (0x45131D..0x45132E): area -1 y, en el nodo, la mitad alta del identificador (mov word [esp+8],0FFFFh)
    return CNodeAddress((uint16)-1, (uint16)(intId >> 16));
}

// 0x451350
CCarPathLinkAddress CPathFind::FindLinkBetweenNodes(CNodeAddress nodeAddrA, CNodeAddress nodeAddrB) {
    if (AreNodeAreasLoaded({ nodeAddrA, nodeAddrB })) {
        // Como el original (0x451392..0x4513D6): los arrays de enlaces son los del area de la DIRECCION A, no los del campo del nodo.
        // (El original busca sin limite de numLinks; aqui se limita a los enlaces del nodo, que es el uso valido.)
        const auto nodeA = &m_pPathNodes[nodeAddrA.m_wAreaId][nodeAddrA.m_wNodeId];
        for (auto i = 0u; i < nodeA->m_nNumLinks; i++) {
            const auto linkIdx = nodeA->m_wBaseLinkId + i;
            if (m_pNodeLinks[nodeAddrA.m_wAreaId][linkIdx] == nodeAddrB) {
                return m_pNaviLinks[nodeAddrA.m_wAreaId][linkIdx];
            }
        }
    }
    return {};
}

// 0x4513F0
CVector CPathFind::FindParkingNodeInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    CVector vecOut;
    plugin::CallMethod<0x4513F0, CPathFind*, CVector*, float, float, float, float, float, float>(this, &vecOut, minX, maxX, minY, maxY, minZ, maxZ);
    return vecOut;
}

// 0x450F30
CNodeAddress CPathFind::FindNearestExteriorNodeToInteriorNode(int32 interiorId) {
    // Como el original (0x450F87: mov ecx,96F050h): la busqueda se hace sobre ThePaths, no sobre `this`
    return ThePaths.FindNodeClosestToCoors(
        { XCoorGiven[interiorId], YCoorGiven[interiorId], ZCoorGiven[interiorId] },
        ePathType::PATH_TYPE_PED,
        3.f,
        0,
        0,
        0,
        0,
        true
    );
}

// 0x44E000
void CPathFind::AddDynamicLinkBetween2Nodes_For1Node(CNodeAddress first, CNodeAddress second) {
    assert(IsAreaNodesAvailable(first));

    auto& firstPathInfo = m_pPathNodes[first.m_wAreaId][first.m_wNodeId];
    auto numAddresses = m_anNumAddresses[first.m_wAreaId];

    int32 firstLinkId;
    // Como el original (0x44E02B: cmp ebx,ebp; jge): comparacion con signo
    if ((int32)firstPathInfo.m_wBaseLinkId >= (int32)numAddresses)
        firstLinkId = firstPathInfo.m_wBaseLinkId;
    else {
        auto* nodeLink = &m_pNodeLinks[first.m_wAreaId][numAddresses];
        auto linkCounter = 0;
        // Como el original (0x44E043: cmp word [esi],0FFFFh; je): busca el primer bloque libre, es decir, el primero cuyo
        // area es -1 (la DLL buscaba el primer bloque con un enlace valido)
        while (nodeLink->IsAreaValid()) {
            nodeLink += 12; // No clue why we jump 12 objects each time (Search: MAGIC_NUM_12)
            ++linkCounter;
        }

        firstLinkId = numAddresses + 12 * linkCounter;
        for (auto i = 0u; i < firstPathInfo.m_nNumLinks; ++i) {
            m_pNodeLinks[first.m_wAreaId][firstLinkId + i] = m_pNodeLinks[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
            m_pLinkLengths[first.m_wAreaId][firstLinkId + i] = m_pLinkLengths[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
            m_pPathIntersections[first.m_wAreaId][firstLinkId + i] = m_pPathIntersections[first.m_wAreaId][firstPathInfo.m_wBaseLinkId + i];
        }

        if (first.m_wAreaId < NUM_PATH_MAP_AREAS) {
            auto& linkInfo = m_aDynamicLinksBaseIds[first.m_wAreaId];
            for (auto i = 0u; i < NUM_DYNAMIC_LINKS_PER_AREA; ++i) {
                // Como el original (0x44E101: cmp dword [esi],0; jl): el primer hueco es el primero negativo
                if (linkInfo[i] < 0) {
                    m_aDynamicLinksBaseIds[first.m_wAreaId][i] = firstPathInfo.m_wBaseLinkId;
                    m_aDynamicLinksIds[first.m_wAreaId][i] = firstLinkId;
                    break;
                }
            }
        }
    }

    m_pNodeLinks[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks] = second;
    m_pLinkLengths[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks] = 5;
    m_pPathIntersections[first.m_wAreaId][firstLinkId + firstPathInfo.m_nNumLinks].Clear();
    firstPathInfo.m_nNumLinks++;
    firstPathInfo.m_wBaseLinkId = firstLinkId;
}

// 0x44D230
bool CPathFind::These2NodesAreAdjacent(CNodeAddress nodeAddress1, CNodeAddress nodeAddress2) {
    // Como el original (0x44D230..0x44D26F): el array de enlaces es el del area de la DIRECCION (shl ecx,2 sobre su word bajo),
    // no el del campo m_wAreaId del nodo; sin comprobar que el area este cargada.
    const auto node1 = &m_pPathNodes[nodeAddress1.m_wAreaId][nodeAddress1.m_wNodeId];
    for (auto i = 0u; i < node1->m_nNumLinks; i++) {
        if (m_pNodeLinks[nodeAddress1.m_wAreaId][node1->m_wBaseLinkId + i] == nodeAddress2) {
            return true;
        }
    }
    return false;
}

// 0x44FCE0
CNodeAddress CPathFind::FindNodeClosestToCoorsFavourDirection(CVector pos, ePathType nodeType, CVector2D dir) {
    // Normalizacion inline del original (0x44FCE0..0x44FD2A): len = sqrt(x*x + y*y); si len == 0 el x pasa a ser 1.0 y la y se queda;
    // si no, inv = 1.0 / len y x, y se multiplican por inv (un NaN va por la rama de normalizar).
    float dirX = dir.x;
    float dirY = dir.y;
    {
        const float len = std::sqrt(dirX * dirX + dirY * dirY);
        if (len == 0.f) {
            dirX = 1.f;
        } else {
            const float inv = 1.f / len;
            dirX = inv * dirX;
            dirY = inv * dirY;
        }
    }

    // Como el original (0x44FD3C..0x44FD43): la mejor puntuacion inicial es 10000.0 (0x461C4000), no FLT_MAX
    CNodeAddress closest{};
    float        scoreOfClosest{ std::bit_cast<float>(0x461C4000u) };
    for (auto areaId{ 0u }; areaId < NUM_TOTAL_PATH_NODE_AREAS; areaId++) {
        const auto allNodes = m_pPathNodes[areaId];
        if (!allNodes) {
            continue;
        }
        int32 first{}, last{};
        switch (nodeType) {
        case PATH_TYPE_VEH: first = 0; last = (int32)m_anNumVehicleNodes[areaId]; break;
        case PATH_TYPE_PED: first = (int32)m_anNumVehicleNodes[areaId]; last = (int32)m_anNumNodes[areaId]; break;
        default:            first = 0; last = (int32)m_anNumNodes[areaId]; break; // PATH_TYPE_ALL no existe en el original
        }
        for (auto nodeIdx = first; nodeIdx < last; nodeIdx++) { // Como el original: se devuelve el indice de recorrido, no m_wNodeId
            const auto nodePos = allNodes[nodeIdx].GetPosition();

            // Como el original (0x44FDD7..0x44FE0B): ((|dz| * 3 + |dy|) + |dx|), orden de suma incluido
            const float dzAbs3   = std::fabs(nodePos.z - pos.z) * 3.f;
            const float dyAbs    = std::fabs(nodePos.y - pos.y);
            const float dxAbs    = std::fabs(nodePos.x - pos.x);
            const float dotScore = (dzAbs3 + dyAbs) + dxAbs;
            if (!(dotScore < scoreOfClosest)) { // fcomp + test ah,5 + jp: solo continua con estrictamente menor (un NaN no)
                continue;
            }

            const float dx = nodePos.x - pos.x;
            const float dy = nodePos.y - pos.y;
            float nx, ny;
            const float len2 = std::sqrt(dy * dy + dx * dx);
            if (len2 == 0.f) {
                nx = 1.f;
                ny = dy;
            } else {
                const float inv = 1.f / len2;
                nx = inv * dx;
                ny = dy * inv;
            }
            const float dot   = nx * dirX + ny * dirY;
            const float score = dotScore - (dot - 1.f) * 20.f;
            if (!(score < scoreOfClosest)) {
                continue;
            }

            scoreOfClosest = score;
            closest = CNodeAddress{ (uint16)areaId, (uint16)nodeIdx };
        }
    }
    return closest;
}

// 0x5D34C0
bool CPathFind::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(m_nNumNodeSwitches);
    for (auto& area : std::span{ m_aNodeSwitches, m_nNumNodeSwitches }) {
        CGenericGameStorage::SaveDataToWorkBuffer(area);
    }
    return true;
}

// 0x5D3500
bool CPathFind::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer(m_nNumNodeSwitches);
    for (auto& area : std::span{ m_aNodeSwitches, m_nNumNodeSwitches }) {
        CGenericGameStorage::LoadDataFromWorkBuffer(area);
    }
    return true;
}

bool CPathFind::AreNodeAreasLoaded(const std::initializer_list<CNodeAddress>& addrs) const {
    return rng::all_of(addrs, [this](auto&& addr) { return IsAreaNodesAvailable(addr); });
}

// 0x44DCD0
void CPathFind::SetPathsNeededAtPosition(const CVector& posn) {
    s_pathsNeededPosn = posn;
    s_bLoadPathsNeeded = true;
}

// 0x44D890
size_t CPathFind::FindXRegionForCoors(float x) const {
    return (size_t)detail::RegionValueOf(x);
}

// 0x44D8C0
size_t CPathFind::FindYRegionForCoors(float y) const {
    return (size_t)detail::RegionValueOf(y);
}

// 0x44DB60
void CPathFind::MarkRegionsForCoors(CVector pos, float radius) {
    // Como el original: las cuatro regiones se calculan sueltas (x - r, x + r, y - r, y + r), sin construir un CRect, y ToBeStreamed
    // se rellena por columnas ([x * 8 + y], rep stos de yMax - yMin + 1 bytes por cada x); antes se usaba el indice y * 8 + x.
    const auto xMin = detail::RegionValueOf(pos.x - radius);
    const auto xMax = detail::RegionValueOf(pos.x + radius);
    const auto yMin = detail::RegionValueOf(pos.y - radius);
    const auto yMax = detail::RegionValueOf(pos.y + radius);
    for (auto x = xMin; x <= xMax; x++) {
        for (auto y = yMin; y <= yMax; y++) {
            ToBeStreamed.data()[x * NUM_PATH_MAP_AREA_Y + y] = true;
        }
    }
}

// 0x44D3E0 - Moved to CPathNode
bool CPathFind::ThisNodeHasToBeSwitchedOff(CPathNode* node) {
    return node->HasToBeSwitchedOff();
}

// 0x4504F0
// This function is only called from `SwitchOffNodeAndNeighbours` but when unhooked
// it doesn't spoil `eax` which makes the former crash
// so hopefully the `__asm mov eax, this` fixes it
// If not just lock both :D
size_t CPathFind::CountNeighboursToBeSwitchedOff(const CPathNode& node) {
    const auto ret = (size_t)rng::count_if(GetNodeLinkedNodes(node), &CPathNode::HasToBeSwitchedOff);
    __asm mov eax, this // It has to be `this`
    return ret;
}

// 0x450320
// Complete EXE helper 0x59C910..0x59C960; local reconstruction, no original delegation.
__declspec(naked) void PathOrientation14Normalise() {
    __asm {
    // 0x59C910
    fld dword ptr [ecx+8]
    // 0x59C913
    fld dword ptr [ecx+4]
    // 0x59C916
    fld dword ptr [ecx]
    // 0x59C918
    fld st(0)
    // 0x59C91A
    fmul st,st(1)
    // 0x59C91C
    fld st(2)
    // 0x59C91E
    fmul st,st(3)
    // 0x59C920
    faddp st(1),st
    // 0x59C922
    fld st(3)
    // 0x59C924
    fmul st,st(4)
    // 0x59C926
    faddp st(1),st
    // 0x59C928
    fstp st(3)
    // 0x59C92A
    fstp st(0)
    // 0x59C92C
    fstp st(0)
    // 0x59C92E
    fcom dword ptr ds:[000858B50h]
    // 0x59C934
    fnstsw ax
    // 0x59C936
    test ah,041h
    // 0x59C939
    jp PathOrientation14Norm_59C944
    // 0x59C93B
    fstp st(0)
    // 0x59C93D
    mov dword ptr [ecx],03F800000h
    // 0x59C943
    ret
    PathOrientation14Norm_59C944:
    fsqrt
    // 0x59C946
    fdivr dword ptr ds:[000858624h]
    // 0x59C94C
    fld st(0)
    // 0x59C94E
    fmul dword ptr [ecx]
    // 0x59C950
    fstp dword ptr [ecx]
    // 0x59C952
    fld st(0)
    // 0x59C954
    fmul dword ptr [ecx+4]
    // 0x59C957
    fstp dword ptr [ecx+4]
    // 0x59C95A
    fmul dword ptr [ecx+8]
    // 0x59C95D
    fstp dword ptr [ecx+8]
    // 0x59C960
    ret
    }
}

__declspec(naked) float CPathFind::FindNodeOrientationForCarPlacement(CNodeAddress nodeInfo) {
    __asm {
    // 0x450320
    sub esp,028h
    // 0x450323
    movzx edx,word ptr [esp+02Ch]
    // 0x450328
    mov eax,dword ptr [ecx+edx*4+000000804h]
    // 0x45032F
    test eax,eax
    // 0x450331
    mov dword ptr [esp],edx
    // 0x450335
    jne PathOrientation14_450343
    // 0x450337
    fld dword ptr ds:[000858B50h]
    // 0x45033D
    add esp,028h
    // 0x450340
    ret 4
    PathOrientation14_450343:
    push esi
    // 0x450344
    mov esi,dword ptr [esp+030h]
    // 0x450348
    shr esi,010h
    // 0x45034B
    imul esi,esi,01Ch
    // 0x45034E
    add esi,eax
    // 0x450350
    mov al,byte ptr [esi+018h]
    // 0x450353
    test al,00Fh
    // 0x450355
    jne PathOrientation14_450364
    PathOrientation14_450357:
    fld dword ptr ds:[000858B50h]
    // 0x45035D
    pop esi
    // 0x45035E
    add esp,028h
    // 0x450361
    ret 4
    PathOrientation14_450364:
    push ebx
    // 0x450365
    and eax,00Fh
    // 0x450368
    push ebp
    // 0x450369
    lea ebx,[eax-1]
    // 0x45036C
    xor ebp,ebp
    // 0x45036E
    test ebx,ebx
    // 0x450370
    jle PathOrientation14_4503D3
    // 0x450372
    movsx eax,word ptr [esi+010h]
    // 0x450376
    mov edx,dword ptr [ecx+edx*4+000000DA4h]
    // 0x45037D
    push edi
    // 0x45037E
    lea edi,[edx+eax*2]
    PathOrientation14_450381:
    movzx eax,word ptr [edi]
    // 0x450384
    mov edx,eax
    // 0x450386
    shr edx,00Ah
    // 0x450389
    cmp dword ptr [ecx+edx*4+000000804h],0
    // 0x450391
    je PathOrientation14_4503C6
    // 0x450393
    mov edx,dword ptr [ecx+edx*4+000000924h]
    // 0x45039A
    and eax,03FFh
    // 0x45039F
    imul eax,eax,00Eh
    // 0x4503A2
    add eax,edx
    // 0x4503A4
    mov dx,word ptr [esp+03Ch]
    // 0x4503A9
    cmp word ptr [eax+4],dx
    // 0x4503AD
    jne PathOrientation14_4503C0
    // 0x4503AF
    mov dx,word ptr [esp+03Eh]
    // 0x4503B4
    cmp word ptr [eax+6],dx
    // 0x4503B8
    jne PathOrientation14_4503C0
    // 0x4503BA
    test byte ptr [eax+00Bh],038h
    // 0x4503BE
    jmp PathOrientation14_4503C4
    PathOrientation14_4503C0:
    test byte ptr [eax+00Bh],7
    PathOrientation14_4503C4:
    jne PathOrientation14_4503CE
    PathOrientation14_4503C6:
    inc ebp
    // 0x4503C7
    add edi,2
    // 0x4503CA
    cmp ebp,ebx
    // 0x4503CC
    jl PathOrientation14_450381
    PathOrientation14_4503CE:
    mov edx,dword ptr [esp+010h]
    // 0x4503D2
    pop edi
    PathOrientation14_4503D3:
    movsx eax,word ptr [esi+010h]
    // 0x4503D7
    mov edx,dword ptr [ecx+edx*4+000000A44h]
    // 0x4503DE
    add eax,ebp
    // 0x4503E0
    mov eax,dword ptr [edx+eax*4]
    // 0x4503E3
    mov dword ptr [esp+038h],eax
    // 0x4503E7
    movzx eax,ax
    // 0x4503EA
    mov ecx,dword ptr [ecx+eax*4+000000804h]
    // 0x4503F1
    test ecx,ecx
    // 0x4503F3
    pop ebp
    // 0x4503F4
    pop ebx
    // 0x4503F5
    je PathOrientation14_450357
    // 0x4503FB
    mov eax,dword ptr [esp+030h]
    // 0x4503FF
    movsx edx,word ptr [esi+00Ah]
    // 0x450403
    shr eax,010h
    // 0x450406
    imul eax,eax,01Ch
    // 0x450409
    add eax,ecx
    // 0x45040B
    movsx ecx,word ptr [esi+8]
    // 0x45040F
    mov dword ptr [esp+030h],ecx
    // 0x450413
    movsx ecx,word ptr [esi+00Ch]
    // 0x450417
    fild dword ptr [esp+030h]
    // 0x45041B
    mov dword ptr [esp+030h],edx
    // 0x45041F
    fmul dword ptr ds:[000858C48h]
    // 0x450425
    movsx edx,word ptr [eax+8]
    // 0x450429
    fild dword ptr [esp+030h]
    // 0x45042D
    mov dword ptr [esp+030h],ecx
    // 0x450431
    fmul dword ptr ds:[000858C48h]
    // 0x450437
    movsx ecx,word ptr [eax+00Ah]
    // 0x45043B
    fild dword ptr [esp+030h]
    // 0x45043F
    mov dword ptr [esp+030h],edx
    // 0x450443
    fmul dword ptr ds:[000858C48h]
    // 0x450449
    fstp dword ptr [esp+028h]
    // 0x45044D
    fild dword ptr [esp+030h]
    // 0x450451
    mov dword ptr [esp+030h],ecx
    // 0x450455
    fmul dword ptr ds:[000858C48h]
    // 0x45045B
    fild dword ptr [esp+030h]
    // 0x45045F
    fmul dword ptr ds:[000858C48h]
    // 0x450465
    fstp dword ptr [esp+00Ch]
    // 0x450469
    movsx edx,word ptr [eax+00Ch]
    // 0x45046D
    mov dword ptr [esp+030h],edx
    // 0x450471
    fild dword ptr [esp+030h]
    // 0x450475
    fmul dword ptr ds:[000858C48h]
    // 0x45047B
    fstp dword ptr [esp+010h]
    // 0x45047F
    fsub st,st(2)
    // 0x450481
    fstp dword ptr [esp+014h]
    // 0x450485
    mov eax,dword ptr [esp+014h]
    // 0x450489
    fld dword ptr [esp+00Ch]
    // 0x45048D
    mov dword ptr [esp+8],eax
    // 0x450491
    fsub st,st(1)
    // 0x450493
    fstp dword ptr [esp+018h]
    // 0x450497
    mov ecx,dword ptr [esp+018h]
    // 0x45049B
    fstp st(0)
    // 0x45049D
    mov dword ptr [esp+00Ch],ecx
    // 0x4504A1
    fstp st(0)
    // 0x4504A3
    lea ecx,[esp+8]
    // 0x4504A7
    fld dword ptr [esp+010h]
    // 0x4504AB
    fsub dword ptr [esp+028h]
    // 0x4504AF
    fstp dword ptr [esp+01Ch]
    // 0x4504B3
    mov edx,dword ptr [esp+01Ch]
    // 0x4504B7
    mov dword ptr [esp+010h],edx
    // 0x4504BB
    mov dword ptr [esp+010h],0
    // 0x4504C3
    call PathOrientation14Normalise
    // 0x4504C8
    fld dword ptr [esp+8]
    // 0x4504CC
    fchs
    // 0x4504CE
    pop esi
    // 0x4504CF
    fld dword ptr [esp+8]
    // 0x4504D3
    fpatan
    // 0x4504D5
    fmul dword ptr ds:[000859878h]
    // 0x4504DB
    add esp,028h
    // 0x4504DE
    ret 4
    }
}

// 0x452160
void CPathFind::SwitchOffNodeAndNeighbours(CPathNode* node, CPathNode*& outNext1, CPathNode** outNext2, bool bWhatToSwitchTo, bool bBackToOriginal) {
    node->m_isSwitchedOff = bBackToOriginal ? node->m_isSwitchedOffOriginal : bWhatToSwitchTo;
   
    outNext1 = nullptr;
    if (outNext2) {
        *outNext2 = nullptr;
    }

    if (CountNeighboursToBeSwitchedOff(*node) > 2) {
        return;
    }

    for (auto& linked : GetNodeLinkedNodes(*node)) {
        if (!linked.HasToBeSwitchedOff()) {
            continue;
        }
        // Como el original (0x4521F4..0x452236): con bBackToOriginal se salta el vecino cuyo estado ya es el original
        // (m_isSwitchedOff == m_isSwitchedOffOriginal); si no, el que ya tiene el estado pedido (antes se ignoraba bBackToOriginal)
        if (bBackToOriginal ? (linked.m_isSwitchedOff == linked.m_isSwitchedOffOriginal) : (linked.m_isSwitchedOff == bWhatToSwitchTo)) {
            continue;
        }
        // Como el original (0x452238: push esi; call 0x4504F0): se cuentan los vecinos del VECINO, no los del nodo
        if (CountNeighboursToBeSwitchedOff(linked) > 2) {
            continue;
        }
        if (!outNext1) {
            outNext1 = &linked;
        }
#ifdef FIX_BUGS // Above it was checked whenever it's set so I assume this was a bug
        else if (outNext2) // Don't get confused, `outNext2` is a ptr to a ptr
#else
        else
#endif
        {
            *outNext2 = &linked;
        }
    }
}

// 0x44DF30
void CPathFind::AddInteriorLinkToExternalNode(int32 interiorNodeIdx, CNodeAddress externalNodeAddr) {
    const auto idx = NumLinksToExteriorNodes++;
    aInteriorNodeLinkedToExterior[idx] = interiorNodeIdx;
    aExteriorNodeLinkedTo[idx] = externalNodeAddr;
}

// 0x44E1A0
void CPathFind::RemoveInterior(uint32 intId) {
    for (auto intSlot = 0; intSlot < NUM_PATH_INTERIOR_AREAS; intSlot++) {
        // Como el original (0x44E1BF..0x44E1D5): el identificador se libera ANTES de recorrer las areas
        if (m_interiorIDs[intSlot] != intId) {
            continue;
        }
        m_interiorIDs[intSlot] = (uint32)-1;
        const int32 intSlotAreaId = NUM_PATH_MAP_AREAS + intSlot;

        // Como el original (0x44E1F1..0x44E3D2): solo las 64 areas del mapa (no las de interior) y solo sus nodos de peatones
        for (auto areaId = 0; areaId < NUM_PATH_MAP_AREAS; areaId++) {
            if (!m_pPathNodes[areaId]) {
                continue;
            }
            if ((int32)m_anNumVehicleNodes[areaId] >= (int32)m_anNumNodes[areaId]) {
                continue;
            }
            for (auto nodeIdx = (int32)m_anNumVehicleNodes[areaId]; nodeIdx < (int32)m_anNumNodes[areaId]; nodeIdx++) {
                auto& node = m_pPathNodes[areaId][nodeIdx];

                // Solo los nodos con enlaces dinamicos (fuera de los enlaces estaticos del area)
                if ((int32)node.m_wBaseLinkId < (int32)m_anNumAddresses[areaId]) {
                    continue;
                }
                if (node.m_nNumLinks == 0) {
                    continue;
                }

                bool removedOwnLink{}, hasOtherInteriorLink{};
                for (auto i = 0; i < (int32)node.m_nNumLinks; i++) {
                    const auto link = m_pNodeLinks[areaId][node.m_wBaseLinkId + i];
                    if (link.m_wAreaId == intSlotAreaId) {
                        // Como el original (0x44E26E..0x44E292): BUG de R*, el desplazamiento de los enlaces restantes indexa
                        // directamente a partir de `this + 0xA44` (el array de punteros m_pNodeLinks), no del array de enlaces del area.
                        auto* const raw = reinterpret_cast<uint32*>(&m_pNodeLinks[0]);
                        for (auto k = i; k < (int32)node.m_nNumLinks - 1; k++) {
                            raw[node.m_wBaseLinkId + k] = raw[node.m_wBaseLinkId + k + 1];
                        }
                        node.m_nNumLinks--;
                        i--;
                        removedOwnLink = true;
                    } else if (link.m_wAreaId >= NUM_PATH_MAP_AREAS) { // cmp cx,40h; jb (el area -1 tambien cuenta)
                        hasOtherInteriorLink = true;
                    }
                }

                // Si se quitaron enlaces a este interior y el nodo no enlaza con otro interior
                if (!removedOwnLink || hasOtherInteriorLink) {
                    continue;
                }

                // Como el original (0x44E2DF..0x44E347): solo se pone a -1 el area (la mitad baja) de los 12 enlaces del bloque
                for (auto j = 0; j < 12; j++) {
                    m_pNodeLinks[areaId][node.m_wBaseLinkId + j].m_wAreaId = (uint16)-1;
                }

                // Libera el registro de enlaces dinamicos de este nodo (0x44E34E..0x44E38D)
                for (auto j = 0; j < NUM_DYNAMIC_LINKS_PER_AREA; j++) {
                    if (m_aDynamicLinksIds[areaId][j] == (int32)node.m_wBaseLinkId) {
                        node.m_wBaseLinkId = (int16)m_aDynamicLinksBaseIds[areaId][j];
                        m_aDynamicLinksBaseIds[areaId][j] = -1;
                        m_aDynamicLinksIds[areaId][j] = -1;
                        break;
                    }
                }
            }
        }

        // Como el original (0x44E3DC..0x44E439): CMemoryMgr::Free (0x72F430) sin comprobar nulos de nodos, enlaces, longitudes e
        // intersecciones, en este orden (antes se liberaban dos veces las longitudes y nunca los enlaces); no hay m_pNaviNodes.
        CMemoryMgr::Free(m_pPathNodes[intSlotAreaId]);
        m_pPathNodes[intSlotAreaId] = nullptr;
        CMemoryMgr::Free(m_pNodeLinks[intSlotAreaId]);
        m_pNodeLinks[intSlotAreaId] = nullptr;
        CMemoryMgr::Free(m_pLinkLengths[intSlotAreaId]);
        m_pLinkLengths[intSlotAreaId] = nullptr;
        CMemoryMgr::Free(m_pPathIntersections[intSlotAreaId]);
        m_pPathIntersections[intSlotAreaId] = nullptr;

        m_anNumNodes[intSlotAreaId] = 0;
        m_anNumVehicleNodes[intSlotAreaId] = 0;
        m_anNumPedNodes[intSlotAreaId] = 0;
        m_anNumCarPathLinks[intSlotAreaId] = 0;
        m_anNumAddresses[intSlotAreaId] = 0;
    }
}

// 0x44D930
void CPathFind::FindStartPointOfRegion(size_t x, size_t y, float& outX, float& outY) {
    const auto pos = FindStartPointOfRegion(x, y);
    outX = pos.x;
    outY = pos.y;
}

// notsa
CVector2D CPathFind::FindStartPointOfRegion(size_t x, size_t y) {
    return { FindXCoorsForRegion(x), FindYCoorsForRegion(y) };
}

namespace detail {
constexpr auto GetCoorsOfRegion(size_t p, size_t nareas) {
    return (6000.f / (float)nareas) * (float)p - 3000.f;
}
};

// 0x44D8F0
float CPathFind::FindXCoorsForRegion(size_t x) {
    return detail::GetCoorsOfRegion(x, NUM_PATH_MAP_AREA_X);
}

// 0x44D910
float CPathFind::FindYCoorsForRegion(size_t y) {
    return detail::GetCoorsOfRegion(y, NUM_PATH_MAP_AREA_Y);
}

// 0x44DF60: clears this node's six slots and signed references in all active rows.
__declspec(naked) void CPathFind::RemoveInteriorLinks(uint32) {
    __asm {
    push edi
    mov edi,dword ptr ds:[0096EF80h]
    xor edx,edx
    test edi,edi
    jle L0044DF9C
    push ebx
    push esi
    mov esi,dword ptr [esp+10h]
    mov ecx,96EAC0h
    L0044DF78: xor eax,eax
    lea ebx,[ebx]
    L0044DF7C: cmp edx,esi
    je L0044DF88
    movsx ebx,byte ptr [eax+ecx]
    cmp ebx,esi
    jne L0044DF8C
    L0044DF88: mov byte ptr [eax+ecx],0FFh
    L0044DF8C: inc eax
    cmp eax,6
    jl L0044DF7C
    inc edx
    add ecx,6
    cmp edx,edi
    jl L0044DF78
    pop esi
    pop ebx
    L0044DF9C: pop edi
    ret 4
    }
}

// 0x44DED0
void CPathFind::AddInteriorLink(int32 intNodeA, int32 intNodeB) {
    const auto AddLink = [](int32 intIdx, int32 linkTo) {
        // Como el original (0x44DED5..0x44DF24): filas de 6 bytes; se busca el primer byte negativo (con signo) y se guarda el enlace.
        // Si los seis primeros ya estan ocupados se escribe en row[6], que es el primer byte de la fila siguiente (sin limite).
        auto* const row = reinterpret_cast<int8*>(ConnectsToGiven.data()) + intIdx * 6;
        int32       k   = 0;
        if (row[0] >= 0) {
            for (;;) {
                if (k >= 6) {
                    break;
                }
                const int8 v = row[k + 1];
                k++;
                if (v < 0) {
                    break;
                }
            }
        }
        row[k] = (int8)linkTo;
    };
    AddLink(intNodeA, intNodeB);
    AddLink(intNodeB, intNodeA);
}

// notsa
std::span<CPathNode> CPathFind::GetPathNodesInArea(size_t areaId, ePathType ptype) const {
    if (const auto allNodes = m_pPathNodes[areaId]) {
        const auto numVehNodes = m_anNumVehicleNodes[areaId];
        switch (ptype) {
        case ePathType::PATH_TYPE_VEH: // Vehicles, boats, race tracks
            return std::span{ allNodes, m_anNumVehicleNodes[areaId] };
        case ePathType::PATH_TYPE_PED: // Peds only
            assert(m_anNumPedNodes[areaId] == m_anNumNodes[areaId] - numVehNodes); // Pirulax: I'm assuming this is true, so if this doesnt assert for a long time remove it
            return std::span{ allNodes + numVehNodes, m_anNumPedNodes[areaId] };
        case ePathType::PATH_TYPE_ALL: // All of the above
            return std::span{ allNodes, m_anNumNodes[areaId] };
        default:
            NOTSA_UNREACHABLE("Invalid pathType: {}", (int)ptype);
        }
    }
    return {}; // Area not loaded, return nothing.. Perhaps assert here instead?
}

// 0x44D1B0
void CPathFind::RemoveNodeFromList(CPathNode* node) {
    node->m_prev->m_next = node->m_next;
    if (node->m_next) {
        node->m_next->m_prev = node->m_prev;
    }

    m_totalNumNodesInPathFindHashTable--;
}

void CPathFind::AddNodeToList(CPathNode* node, int32 distFromOrigin) {
    // Insert the node as the head into it's bucket

    // Como el original (0x44D1E6..0x44D1F1: and eax,1FFh; movsx eax,ax): mascara de 9 bits (con `%` una distancia negativa daba indice negativo)
    auto& head = m_pathFindHashTable[distFromOrigin & 511];

    node->m_next = head;

    // Make this node's `next` point to the head in the hash table
    // I guess this works as long as you only access the `m_prev` variable
    // as that's at offset 0
    // This is a really bad hack to avoid having to do special handling
    // for the head in `RemoveNodeFromList`...
    node->m_prev = reinterpret_cast<CPathNode*>(&head);

    if (head) {
        head->m_prev = node;
    }

    head = node;

    node->m_totalDistFromOrigin = (int16)distFromOrigin; // Como el original (mov word [edx+0Eh],di): se trunca a 16 bits sin aviso

    m_totalNumNodesInPathFindHashTable++;
}

// 0x452C80
void CPathFind::SwitchRoadsOffInArea(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bSwitchOff, bool bCars, bool bBackToOriginal) {
    for (auto areaId = 0u; areaId < NUM_PATH_MAP_AREAS; ++areaId) {
        SwitchRoadsOffInAreaForOneRegion(xMin, xMax, yMin, yMax, zMin, zMax, bSwitchOff, bCars, areaId, bBackToOriginal);
    }

    for (auto i = 0u; i < m_nNumNodeSwitches; ++i) {
        auto* pArea = &m_aNodeSwitches[i];

        if (notsa::bugfixes::CPathFind_SwitchRoadsOffInArea_StrayAreas) {
            // some missions create both types of switches at the same area, so we store them separately
            if (pArea->isCars != bCars) {
                continue;
            }

            // avoid creating stray areas, potentially leaving no space for important areas later in game
            // ideally, the script would use SWITCH_ROADS_BACK_TO_ORIGINAL or SWITCH_PED_ROADS_BACK_TO_ORIGINAL but that's not always the case
            // so we consider toggling the same area as "back to original"
            if (pArea->xMin == xMin && pArea->yMin == yMin && pArea->zMin == zMin && pArea->xMax == xMax && pArea->yMax == yMax && pArea->zMax == zMax && pArea->isOff != bSwitchOff) {
                bBackToOriginal = true;
            }
        }

        // If the existing area is completely inside the area we are switching off, remove it
        // Como el original (0x452D18..0x452D84): comparaciones ordenadas (fcom + test ah,1 / 41h), un NaN en el area guardada la conserva
        if (!(pArea->xMin >= xMin && pArea->yMin >= yMin && pArea->zMin >= zMin && pArea->xMax <= xMax && pArea->yMax <= yMax && pArea->zMax <= zMax)) {
            continue;
        }

        for (auto j = i; j < m_nNumNodeSwitches - 1; ++j) {
            // Como el original (rep movs de 7 dwords, 0x452D9C): copia los 0x1C bytes, relleno incluido
            if (notsa::bugfixes::CPathFind_SwitchRoadsOffInArea_StrayAreas) {
                std::memcpy(&m_aNodeSwitches[j], &m_aNodeSwitches[j + 1], sizeof(CNodesSwitchedOnOrOff));
            } else {
                // R* bug, they messed up with the index (0x452D98: mov esi,ebx; mov edi,edx dentro del bucle)
                std::memcpy(&m_aNodeSwitches[i], &m_aNodeSwitches[i + 1], sizeof(CNodesSwitchedOnOrOff));
            }
        }

        --m_nNumNodeSwitches;
        --i;
    }

    if (!bBackToOriginal && m_nNumNodeSwitches < NUM_PATH_MAP_AREAS) {
        auto& area  = m_aNodeSwitches[m_nNumNodeSwitches];
        area.xMin   = xMin;
        area.xMax   = xMax;
        area.yMin   = yMin;
        area.yMax   = yMax;
        area.zMin   = zMin;
        area.zMax   = zMax;
        area.isOff  = bSwitchOff;
        area.isCars = bCars;
        m_nNumNodeSwitches++;
    }
}

// 0x452F00
void CPathFind::SwitchPedRoadsOffInArea(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax, bool bSwitchOff, bool bBackToOriginal) {
    SwitchRoadsOffInArea(xMin, xMax, yMin, yMax, zMin, zMax, bSwitchOff, false, bBackToOriginal);
}

// 0x44DA30: both offsets use the minimum raw width; preserve x-before-y alias order.
void CPathFind::AddRandomOffsetToNodePosition(CNodeAddress first, CNodeAddress second, int16 random, float* x, float* y) {
    if (first.m_wAreaId == 0xFFFF || !m_pPathNodes[first.m_wAreaId]) return;
    if (second.m_wAreaId == 0xFFFF || !m_pPathNodes[second.m_wAreaId]) return;
    const auto* a = reinterpret_cast<const uint8*>(&m_pPathNodes[first.m_wAreaId][first.m_wNodeId]);
    const auto* b = reinterpret_cast<const uint8*>(&m_pPathNodes[second.m_wAreaId][second.m_wNodeId]);
    const int32 xFactor = (uint16(random) & 15) - 7;
    const int32 yFactor = ((uint16(random) >> 4) & 15) - 7;
    const float scale = std::bit_cast<float>(0x3BFDF3B6u);
    int32 displacement;
    __asm {
        mov ecx, a
        mov edx, b
        movzx eax, byte ptr [ecx + 16h]
        movzx ebx, byte ptr [edx + 16h]
        cmp eax, ebx
        jbe firstMinimum
        mov eax, ebx
    firstMinimum:
        imul eax, xFactor
        mov displacement, eax
        mov eax, x
        fild displacement
        fmul scale
        fadd dword ptr [eax]
        fstp dword ptr [eax]
        // Original reloads both widths after writing x, including possible aliasing.
        movzx eax, byte ptr [ecx + 16h]
        movzx ebx, byte ptr [edx + 16h]
        cmp eax, ebx
        jbe secondMinimum
        mov eax, ebx
    secondMinimum:
        imul eax, yFactor
        mov displacement, eax
        mov eax, y
        fild displacement
        fmul scale
        fadd dword ptr [eax]
        fstp dword ptr [eax]
    }
}
