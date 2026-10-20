/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPathFind
 * Reconstrucción: COMPLETA — 66/66 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 66/66 entradas.
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
#include "X87Math.h"

#include <reversiblebugfixes/Bugs.hpp>

// 0x44EFC0
// Como el original (130 instrucciones, 0x44EFC0..0x44F186): recorre las 64 areas del mapa y, por cada nodo de vehiculos a menos de 80 unidades
// de (x, y) (|p - c| < 80 estricto y ordenado: un NaN lo descarta), suma la longitud de cada enlace por los carriles propios (bits 0..2 del byte
// +0xB del CCarPathLink) y despues por los del sentido contrario (bits 3..5); el total se multiplica por 0.0004 (0x858FC0).
// Todos los valores son finitos y acotados (int16 * 0.125, diferencias de hasta 8192), sin subnormales ni desbordes: el float del SSE de la DLL
// redondea operacion a operacion igual que el x87 de 24 bits del EXE (fild/fmul/fsubp/fsqrt/faddp), sin ayudantes x87.
// Registros de salida del EXE: EAX = 0 (contador de areas), ECX = this (no se escribe) y EDX = el ultimo valor que escribe el recorrido
// (puntero de nodos del area 63, o lo que sigue: y del nodo, byte de enlaces, area del enlace, area de navegacion, carriles propios). Los deja
// PathFindResidueFloatCpp (X87Math.cpp), ultima llamada, junto con el ST0 de retorno (sin comprobaciones de ejecucion ni cookie de /GS;
// lo vigila contrib/audit_register_residue.py, objeto PathFind).
#pragma runtime_checks("", off)
__declspec(safebuffers) float CPathFind::CalcRoadDensity(float x, float y) {
    const uint32 ecxValue = static_cast<uint32>(reinterpret_cast<uintptr_t>(this));
    uint32       edxValue = 0;
    float        density  = 0.f; // fld dword [0x858B50]

    for (int32 area = 0; area < NUM_PATH_MAP_AREAS; area++) {
        const uint8* node = reinterpret_cast<const uint8*>(m_pPathNodes[area]);
        edxValue          = static_cast<uint32>(reinterpret_cast<uintptr_t>(node)); // 0x44EFE0: mov edx,[ebp]
        if (!node) {
            continue;
        }
        int32 numNodes = static_cast<int32>(m_anNumVehicleNodes[area]); // 0x44EFEB: contador con signo (jle)
        if (numNodes <= 0) {
            continue;
        }
        for (; numNodes > 0; numNodes--, node += sizeof(CPathNode)) {
            const int16 rawX  = *reinterpret_cast<const int16*>(node + 0x8);
            const float nodeX = static_cast<float>(rawX) * 0.125f;
            if (!(std::fabs(nodeX - x) < 80.f)) { // fsub; fabs; fcomp 80.0; test ah,5; jp
                continue;
            }
            const int16 rawY = *reinterpret_cast<const int16*>(node + 0xA);
            edxValue         = static_cast<uint32>(static_cast<int32>(rawY)); // 0x44F031: movsx edx,[esi]
            const float nodeY = static_cast<float>(rawY) * 0.125f;
            if (!(std::fabs(nodeY - y) < 80.f)) {
                continue;
            }
            const uint8 flags = node[0x18];
            edxValue          = flags; // 0x44F05D: xor edx,edx; mov dl,[esi+0Eh]
            if ((flags & 0xF) == 0) {
                continue;
            }
            const uint32 numLinks = static_cast<uint32>(flags & 0xF);
            edxValue              = numLinks; // 0x44F06B: and edx,0Fh
            const int16         baseLink  = *reinterpret_cast<const int16*>(node + 0x10);
            const uint32* const nodeLinks = reinterpret_cast<const uint32*>(m_pNodeLinks[area]) + baseLink; // this+0xA44
            const uint16* const naviLinks = reinterpret_cast<const uint16*>(m_pNaviLinks[area]) + baseLink; // this+0xDA4

            for (uint32 i = 0; i < numLinks; i++) {
                const uint32 link = nodeLinks[i];
                // El indice de area llega a 0xFFFF sin comprobar, como en el EXE (movzx eax,ax)
                const uint8* const linkedNodes = reinterpret_cast<const uint8*>(m_pPathNodes[link & 0xFFFF]);
                edxValue                       = static_cast<uint32>(reinterpret_cast<uintptr_t>(linkedNodes)); // 0x44F08F
                if (!linkedNodes) {
                    continue;
                }
                const uint8* const linked     = linkedNodes + (link >> 16) * sizeof(CPathNode);
                const int16        linkedRawX = *reinterpret_cast<const int16*>(linked + 0x8);
                const int16        linkedRawY = *reinterpret_cast<const int16*>(linked + 0xA);
                const uint32       naviLink   = naviLinks[i];
                const uint32       naviArea   = naviLink >> 10;
                edxValue                      = naviArea; // 0x44F0D4: mov edx,eax; shr edx,0Ah
                const float dx     = nodeX - static_cast<float>(linkedRawX) * 0.125f;
                const float dy     = nodeY - static_cast<float>(linkedRawY) * 0.125f;
                const float length = std::sqrt(dy * dy + dx * dx); // fsqrt: 24 bits, correctamente redondeada
                if (!m_pPathNodes[naviArea]) {
                    continue;
                }
                const uint8* const naviNodes = reinterpret_cast<const uint8*>(m_pNaviNodes[naviArea]);
                const uint8        lanes     = naviNodes[(naviLink & 0x3FF) * sizeof(CCarPathLink) + 0xB];
                const uint32       ownLanes  = static_cast<uint32>(lanes & 7);
                edxValue                     = ownLanes; // 0x44F116..0x44F11B: mov dl,al; and dl,7; movzx edx,dl
                density                      = density + static_cast<float>(ownLanes) * length;
                density                      = density + static_cast<float>((lanes >> 3) & 7) * length;
            }
        }
    }

    return PathFindResidueFloatCpp(ecxValue, edxValue, density * std::bit_cast<float>(0x39D1B717u), 0); // fmul [0x858FC0] (0.0004f); EAX = 0
}
#pragma runtime_checks("", restore)

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
auto& s_parkingNodeCursor = StaticRef<int32>(0x977B9C); // 0x4513F0: indice del nodo de aparcamiento que se devuelve

namespace detail {
/*!
* @brief Region (0..7) de una coordenada del mundo, como el original (0x44D830, 0x44D890, 0x44D8C0, 0x44DB60...):
*        (p - (-3000.0)) * 0.0013333333 (0x3AAEC33E, no 1/750 exacto), _ftol2 (trunca) y acotado con signo a [0, 7].
*/
inline int32 RegionValueOf(float p) {
    const auto scaled = (p - std::bit_cast<float>(0xC53B8000u)) * std::bit_cast<float>(0x3AAEC33Eu);
    const auto region = (int32)(int64)scaled; // _ftol (0x821B40): 32 bits bajos del entero de 64 bits truncado (NaN y |v| >= 2^63 dan 0)
    return region <= 0 ? 0 : (region < 7 ? region : 7);
}

// rand() del CRT del EXE (0x821B1E): mantiene su propia semilla (el rand de la DLL es otro generador)
inline int32 ExeRand() {
    return reinterpret_cast<int(__cdecl*)()>(0x821B1E)();
}
}; // namespace detail

void CPathFind::InjectHooks() {
    RH_ScopedClass(CPathFind);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(AddRandomOffsetToNodePosition, 0x44DA30);
    RH_ScopedInstall(GeneratePedCreationCoors_Interior, 0x44ECA0);

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
    RH_ScopedInstall(FindNodeClosestToCoors, 0x44F460);
    RH_ScopedInstall(MarkRoadNodeAsDontWander, 0x450560);
    //RH_ScopedInstall(AddDynamicLinkBetween2Nodes, 0x4512D0);
    RH_ScopedOverloadedInstall(LoadPathFindData, "Area", 0x452F40, void(CPathFind::*)(int32));
    RH_ScopedOverloadedInstall(LoadPathFindData, "FromStream", 0x4529F0, void(CPathFind::*)(RwStream*, int32));
    RH_ScopedInstall(SwitchPedRoadsOffInArea, 0x452F00);
    RH_ScopedInstall(SwitchRoadsOffInArea, 0x452C80);
    RH_ScopedInstall(SwitchRoadsOffInAreaForOneRegion, 0x452820);
    RH_ScopedInstall(ComputeRoute, 0x452760);
    RH_ScopedInstall(CompleteNewInterior, 0x452270);
    RH_ScopedInstall(SwitchOffNodeAndNeighbours, 0x452160);
    //RH_ScopedInstall(Find2NodesForCarCreation, 0x452090);
    //RH_ScopedInstall(TestCoorsCloseness, 0x452000);
    RH_ScopedInstall(FindNextNodeWandering, 0x451B70);
    RH_ScopedInstall(DoPathSearch, 0x4515D0);
    RH_ScopedInstall(FindParkingNodeInArea, 0x4513F0);
    RH_ScopedInstall(FindLinkBetweenNodes, 0x451350);
    RH_ScopedInstall(ReturnInteriorNodeIndex, 0x451300);
    RH_ScopedInstall(FindNthNodeClosestToCoors, 0x44F8C0);
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

// 0x44ECA0 (verificada contra el desensamblado, lote 62)
// busca (hasta 200 intentos) un nodo de peaton de la region de (x, y) que este a menos de 50 m (en 2D), con z > 900 (interior) y con enlaces, y genera una
// posicion aleatoria entre ese nodo y uno de sus vecinos del mapa (areas < 64). Hasta 5 intentos por enlace: se acepta si hay suelo a menos de 3 m de la posicion.
// Como el original: outOrientation recibe en cada intento la fraccion aleatoria t = (rand & 255) / 256 (no una orientacion) y el rand es el del CRT del EXE (0x821B1E).
// unused1/unused2 no se usan. Los productos y sumas se redondean a float en cada paso.
bool CPathFind::GeneratePedCreationCoors_Interior(float x, float y, CVector* outCoords, CNodeAddress* unused1, CNodeAddress* unused2, float* outOrientation) {
    const auto Comp = [](const CPathNode* node, size_t off) { return (float)*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(node) + off) * 0.125f; };

    const int32 areaId = (int32)FindRegionForCoors({ x, y });
    for (int32 attempt = 0; attempt < 200; attempt++) {
        if (!m_pPathNodes[areaId] || m_anNumPedNodes[areaId] == 0) {
            continue;
        }

        const int32 r       = detail::ExeRand() >> 6;
        const int32 nodeIdx = (int32)m_anNumVehicleNodes[areaId] + r % (int32)m_anNumPedNodes[areaId];
        const CPathNode* const node = &m_pPathNodes[areaId][nodeIdx];

        const float nx = Comp(node, 0x8);
        const float ny = Comp(node, 0xA);
        const float dy = ny - y;
        const float dx = nx - x;
        if (!(dy * dy + dx * dx < 2500.0f)) {
            continue;
        }
        if (!(Comp(node, 0xC) > 900.0f)) {
            continue;
        }

        const int32 numLinks = reinterpret_cast<const uint8*>(node)[0x18] & 0xF;
        for (int32 link = 0; link < numLinks; link++) {
            const CNodeAddress linked = m_pNodeLinks[areaId][(int32)node->m_wBaseLinkId + link];
            if (linked.m_wAreaId >= 0x40 || !m_pPathNodes[linked.m_wAreaId]) {
                continue;
            }
            const CPathNode* const other = &m_pPathNodes[linked.m_wAreaId][linked.m_wNodeId];

            for (int32 tries = 0; tries < 5; tries++) {
                const float t = (float)(detail::ExeRand() & 0xFF) * 0.00390625f;
                *outOrientation = t;

                const float a1  = Comp(other, 0x8) * t;
                const float a2  = Comp(other, 0xA) * t;
                const float a3  = Comp(other, 0xC) * t;
                const float omt = 1.0f - t;
                const float b1  = Comp(node, 0x8) * omt;
                const float b2  = Comp(node, 0xA) * omt;
                const float b3  = Comp(node, 0xC) * omt;
                outCoords->x = b1 + a1;
                outCoords->y = b2 + a2;
                outCoords->z = b3 + a3;

                bool        found  = false;
                const float groundZ = CWorld::FindGroundZFor3DCoord({ outCoords->x, outCoords->y, outCoords->z + 2.0f }, &found, nullptr);
                if (found && std::fabs(groundZ - outCoords->z) < 3.0f) {
                    outCoords->z = groundZ;
                    return true;
                }
            }
        }
    }
    return false;
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

// 0x44F8C0 (verificada contra el desensamblado, lote 62): devuelve el nodo n-esimo mas cercano a pos. Primero limpia en todas las areas (0..71) la marca (bit 1 del byte +0x19) de los nodos del tipo pedido; despues
// busca el mas cercano (0x44F460) nthNode veces marcando cada hallado, y devuelve el resultado de una ultima busqueda con interior incluido (unk6 = 0). outNode recibe
// cada resultado del bucle, tambien el invalido (que corta y se devuelve). Con nodeType > 1 no se limpia nada (el EXE compara dos registros con el mismo valor, nthNode: cmp edi,ebp; jge).
CNodeAddress CPathFind::FindNthNodeClosestToCoors(CVector pos, uint8 nodeType, float maxDistance, bool bLowTraffic, bool bUnkn, int32 nthNode, bool bBoatsOnly,
                                                  bool bIgnoreInterior, CNodeAddress* outNode) {
    for (int32 areaId = 0; areaId < NUM_PATH_MAP_AREAS + NUM_PATH_INTERIOR_AREAS; areaId++) {
        if (!m_pPathNodes[areaId]) {
            continue;
        }
        int32 first, last;
        if (nodeType == PATH_TYPE_VEH) {
            first = 0;
            last  = (int32)m_anNumVehicleNodes[areaId];
        } else if (nodeType == PATH_TYPE_PED) {
            first = (int32)m_anNumVehicleNodes[areaId];
            last  = (int32)m_anNumNodes[areaId];
        } else {
            continue;
        }
        for (int32 i = first; i < last; i++) {
            reinterpret_cast<uint8*>(&m_pPathNodes[areaId][i])[0x19] &= 0xFD;
        }
    }

    if (nthNode > 0) {
        do {
            const CNodeAddress found = FindNodeClosestToCoors(pos, (ePathType)nodeType, maxDistance, (uint16)bLowTraffic, (int32)bUnkn, 1, (uint16)bBoatsOnly, (int32)bIgnoreInterior);
            if (outNode) {
                *outNode = found;
            }
            if (found.m_wAreaId == 0xFFFF) {
                return found;
            }
            reinterpret_cast<uint8*>(&m_pPathNodes[found.m_wAreaId][found.m_wNodeId])[0x19] |= 2;
        } while (--nthNode > 0);
    }

    return FindNodeClosestToCoors(pos, (ePathType)nodeType, maxDistance, (uint16)bLowTraffic, (int32)bUnkn, 1, (uint16)bBoatsOnly, 0);
}

// 0x451B70 (verificada contra el desensamblado, lote 62): elige el siguiente nodo al que ir "deambulando" desde originAddress (si no es valido, o si pos esta a mas de max(7, anchura del nodo) de el, se busca el nodo mas
// cercano a pos, 0x44F460). Entre los vecinos cargados del nodo (sin los apagados salvo que el nodo lo este y sin los DontWander salvo que el nodo lo sea) se queda con el de
// mayor producto escalar entre su direccion normalizada y (sin, cos)(dir * pi/4), y outDir (0..7) clasifica esa direccion. Si el elegido es el propio origen original, se toma el
// primer vecino con su area cargada. Como el original: los pasos de x87 van a 24 bits; sin/cos son float(fsin/fcos) del angulo ya redondeado a float; *originAddress
// se escribe siempre que haya nodo de partida (el EXE no comprueba el puntero nulo en ese punto).
void CPathFind::FindNextNodeWandering(uint8 nodeType, CVector vecPos, CNodeAddress* originAddress, CNodeAddress* targetAddress, uint8 dir, uint8* outDir) {
    const auto Raw    = [](const CPathNode* node) { return reinterpret_cast<const uint8*>(node); };
    const auto CompXY = [](const CPathNode* node, size_t off) { return (float)*reinterpret_cast<const int16*>(reinterpret_cast<const uint8*>(node) + off) * 0.125f; };

    CNodeAddress origin; // copia del origen (el EXE deja sin inicializar el nodo cuando no hay origen; solo se compara con origin.m_wAreaId != 0xFFFF)
    origin.m_wAreaId = 0xFFFF;
    origin.m_wNodeId = 0xFFFF;
    CNodeAddress cur; // [esp+30h]: nodo de partida
    bool         search = false;

    if (!originAddress || ((origin = *originAddress).m_wAreaId == 0xFFFF)) {
        search = true;
    } else {
        if (!m_pPathNodes[origin.m_wAreaId]) {
            targetAddress->m_wAreaId = 0xFFFF;
            *outDir                  = 0;
            return;
        }
        const CPathNode* const originNode = &m_pPathNodes[origin.m_wAreaId][origin.m_wNodeId];
        const float            dx         = vecPos.x - CompXY(originNode, 0x8);
        const float            dy         = vecPos.y - CompXY(originNode, 0xA);
        const float            dz         = vecPos.z - CompXY(originNode, 0xC);
        const float            dist2      = (dz * dz + dy * dy) + dx * dx;
        float                  radius     = (float)Raw(originNode)[0x16] * 0.125f;
        if (7.0f > radius) {
            radius = 7.0f;
        }
        if (std::sqrt(dist2) > radius) {
            search = true;
        } else {
            cur = origin;
        }
    }
    if (search) {
        cur = FindNodeClosestToCoors(vecPos, (ePathType)nodeType, std::bit_cast<float>(0x497423FEu), 0, 0, 0, 0, 0);
    }

    if (cur.m_wAreaId == 0xFFFF || !m_pPathNodes[cur.m_wAreaId]) {
        targetAddress->m_wAreaId = 0xFFFF;
        cur.m_wAreaId            = 0xFFFF;
        *outDir                  = 0;
        *targetAddress           = cur;
    } else {
        const CPathNode* const node = &m_pPathNodes[cur.m_wAreaId][cur.m_wNodeId];
        targetAddress->m_wAreaId    = 0xFFFF;

        const float angle = (float)dir * 0.7853982f;
        const float sinA  = (float)std::sin((double)angle);
        const float cosA  = (float)std::cos((double)angle);
        float       best  = -999999.0f;

        const int32 numLinks = Raw(node)[0x18] & 0xF;
        for (int32 i = 0; i < numLinks; i++) {
            const CNodeAddress linked = m_pNodeLinks[node->m_wAreaId][(int32)node->m_wBaseLinkId + i];
            if (!m_pPathNodes[linked.m_wAreaId]) {
                continue;
            }
            const CPathNode* const other = &m_pPathNodes[linked.m_wAreaId][linked.m_wNodeId];
            if (!(Raw(node)[0x18] & 0x20) && (Raw(other)[0x18] & 0x20)) {
                continue;
            }
            if (!(Raw(node)[0x19] & 4) && (Raw(other)[0x19] & 4)) {
                continue;
            }

            const float dx  = CompXY(other, 0x8) - CompXY(node, 0x8);
            const float dy  = CompXY(other, 0xA) - CompXY(node, 0xA);
            const float inv = 1.0f / std::sqrt(dy * dy + dx * dx);
            const float ndx = inv * dx;
            const float ndy = inv * dy;
            const float dot = ndy * cosA + ndx * sinA;
            if (!(dot >= best)) { // fcom + test ah,1 (tambien salta con NaN)
                continue;
            }
            best           = dot;
            *targetAddress = linked;

            // Clasificacion de la direccion (cada rama replica la comparacion y el salto del EXE)
            if (ndx >= 0.0f) {
                if (2.0f * std::fabs(ndy) < ndx) {
                    *outDir = 2;
                } else if (2.0f * ndx < ndy) {
                    *outDir = 0;
                } else if (ndx * -2.0f > ndy) {
                    *outDir = 4;
                } else {
                    *outDir = ndy > 0.0f ? 1 : 3;
                }
            } else {
                if (2.0f * std::fabs(ndy) < -ndx) {
                    *outDir = 6;
                } else if (ndx * -2.0f < ndy) {
                    *outDir = 0;
                } else if (2.0f * ndx > ndy) {
                    *outDir = 4;
                } else {
                    *outDir = ndy > 0.0f ? 7 : 5;
                }
            }
        }

        *originAddress = cur;
        if (targetAddress->m_wAreaId == 0xFFFF) {
            *outDir        = 0;
            *targetAddress = cur;
        }
    }

    // Si el destino coincide con el origen original, se prefiere el primer vecino cuya area esta cargada
    if (targetAddress->m_wAreaId == origin.m_wAreaId && targetAddress->m_wNodeId == origin.m_wNodeId && origin.m_wAreaId != 0xFFFF && m_pPathNodes[origin.m_wAreaId]) {
        const CPathNode* const node     = &m_pPathNodes[origin.m_wAreaId][origin.m_wNodeId];
        const int32            numLinks = Raw(node)[0x18] & 0xF;
        if (numLinks > 0) {
            const CNodeAddress* links = m_pNodeLinks[node->m_wAreaId] + node->m_wBaseLinkId;
            for (int32 i = 0; i < numLinks; i++) {
                if (m_pPathNodes[links[i].m_wAreaId]) {
                    *targetAddress = links[i];
                    break;
                }
            }
        }
    }
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
    // sin normalizar (antes se normalizaba y se llamaba a std::atan2). WeaponHeadingDegreesCpp (X87Math) es float(atan2(-dx, dy) * 57.29578f).
    outHeadingDeg = WeaponHeadingDegreesCpp(dx, dy);

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

// 0x44F460 (verificada contra el desensamblado, lote 62): busca el nodo mas cercano a pos recorriendo la region (x, y) y hasta 4 anillos de regiones alrededor, y despues (si byte bajo de unk6 == 0) las 8 regiones de
// interior (0x40..0x47). La busqueda por region es 0x44F2C0 (FindNodeClosestInRegion, sin reconstruir): su distancia (entrada/salida) arranca en maxDistance.
// Se para cuando la distancia hallada es <= (primera region) / < (anillos) que la distancia minima al borde de la region + 750 por anillo.
// unk3 no se usa. Las banderas se pasan al colaborador como dwords (el EXE empuja los registros completos).
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
    CNodeAddress result;
    result.m_wAreaId = 0xFFFF;
    float dist = maxDistance;

    const auto Search = [&](int32 areaId, int32 unusedArg) {
        plugin::CallMethod<0x44F2C0, CPathFind*, CNodeAddress*, int32, CVector, int32, float*, int32, int32, int32, int32>(
            this, &result, areaId, pos, (int32)nodeType, &dist, (int32)unk2, (int32)unk4, (int32)bBoatsOnly, unusedArg);
    };

    const int32 rx = detail::RegionValueOf(pos.x);
    const int32 ry = detail::RegionValueOf(pos.y);

    // Distancia minima de pos a los cuatro bordes de su region (cada comparacion: se conserva el anterior solo si es estrictamente menor). Las restas van por el x87
    // (PathFindSubCpp): el SSE del juego (MXCSR con FTZ) vaciaria a cero un resultado denormal, el fsub del EXE lo conserva.
    float minDist = PathFindSubCpp(pos.x, (float)rx * 750.0f - 3000.0f);
    {
        const float east = PathFindSubCpp((float)(rx + 1) * 750.0f - 3000.0f, pos.x);
        if (!(minDist < east)) { minDist = east; }
        const float south = PathFindSubCpp(pos.y, (float)ry * 750.0f - 3000.0f);
        if (!(minDist < south)) { minDist = south; }
        const float north = PathFindSubCpp((float)(ry + 1) * 750.0f - 3000.0f, pos.y);
        if (!(minDist < north)) { minDist = north; }
    }

    Search(rx + ry * 8, unk6);
    if (dist > minDist) {
        int32 xmin = rx - 1;
        int32 xmax = rx + 1;
        int32 ymin = ry - 1;
        for (int32 ring = 1; ring < 5; ring++) {
            const int32 ymax = ry + ring;

            if (xmin >= 0 && xmin < 8) { // columna oeste
                for (int32 j = ymin; j <= ymax; j++) {
                    if (j >= 0 && j < 8) {
                        Search(xmin + j * 8, unk6);
                    }
                }
            }
            if (xmax >= 0 && xmax < 8) { // columna este
                for (int32 j = ymin; j <= ymax; j++) {
                    if (j >= 0 && j < 8) {
                        Search(xmax + j * 8, unk6);
                    }
                }
            }
            if (ymin >= 0 && ymin < 8) { // fila sur (sin esquinas)
                for (int32 i = xmin + 1; i < xmax; i++) {
                    if (i >= 0 && i < 8) {
                        Search(i + ymin * 8, unk6);
                    }
                }
            }
            if (ymax >= 0 && ymax < 8) { // fila norte (sin esquinas)
                for (int32 i = xmin + 1; i < xmax; i++) {
                    if (i >= 0 && i < 8) {
                        Search(i + ymax * 8, unk6);
                    }
                }
            }

            minDist += 750.0f;
            if (dist < minDist) {
                break;
            }
            xmin--;
            xmax++;
            ymin--;
        }
    }

    if ((uint8)unk6 == 0) {
        for (uint16 areaId = 0x40; areaId < 0x48; areaId++) {
            Search(areaId, 0);
        }
    }
    return result;
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

// 0x452270 (verificada contra el desensamblado, lote 62): cierra el interior que se estaba construyendo (StartNewInterior + AddNodeToNewInterior): reserva en el area 64 + NewInteriorSlot los nodos, enlaces,
// longitudes e intersecciones, enlaza los nodos entre si (las tablas de conexiones se hacen simetricas) y registra los enlaces con nodos exteriores.
// outAddress (opcional): solo se le escribe el area -1 (el EXE no rellena nada mas).
void CPathFind::CompleteNewInterior(CNodeAddress* outAddress) {
    if (outAddress) {
        outAddress->m_wAreaId = 0xFFFF;
    }

    if (NumNodesGiven != 0) {
        const int32 numNodes = (int32)NumNodesGiven;
        const int32 slot     = NewInteriorSlot;
        const int32 areaId   = NUM_PATH_MAP_AREAS + slot;

        // 0x45229A: slot + 100, o la inundacion del primer nodo exterior enlazado
        uint8 floodFill = (uint8)(slot + 100);
        if (NumLinksToExteriorNodes > 0) {
            const auto ext = aExteriorNodeLinkedTo[0];
            floodFill = m_pPathNodes[ext.m_wAreaId][ext.m_wNodeId].m_nFloodFill;
        }

        m_interiorIDs[slot] = InteriorIDBeingBuilt;
        m_pPathNodes[areaId] = static_cast<CPathNode*>(CMemoryMgr::Malloc(numNodes * sizeof(CPathNode), 0));

        // Nodos: posicion comprimida (x8, _ftol2) y banderas fijas
        for (int32 i = 0; i < numNodes; i++) {
            auto* const n = reinterpret_cast<uint8*>(&m_pPathNodes[areaId][i]);
            *reinterpret_cast<uint16*>(n + 0x8)  = (uint16)(int64)(XCoorGiven[i] * 8.0f);
            *reinterpret_cast<uint16*>(n + 0xA)  = (uint16)(int64)(YCoorGiven[i] * 8.0f);
            *reinterpret_cast<uint16*>(n + 0xC)  = (uint16)(int64)(ZCoorGiven[i] * 8.0f);
            *reinterpret_cast<uint16*>(n + 0x14) = (uint16)i;           // m_wNodeId
            *reinterpret_cast<uint16*>(n + 0x12) = (uint16)(slot + 0x40); // m_wAreaId
            n[0x16] = 0;                                                // m_nPathWidth
            n[0x17] = floodFill;                                        // m_nFloodFill
            n[0x18] &= 0xEF;                                            // m_onDeadEnd = 0
            n[0x18] |= 0x20;                                            // m_isSwitchedOff = 1
            n[0x19] |= 0x01;                                            // m_isSwitchedOffOriginal = 1
            n[0x18] &= 0xBF;                                            // m_bRoadBlocks = 0
            n[0x18] &= 0x7F;                                            // m_bWaterNode = 0
            n[0x19] &= 0xFD;
            n[0x19] = (uint8)((n[0x19] & ~4) | ((DontWanderGiven[i] << 2) & 4)); // m_bDontWander
            n[0x19] |= 0x08;
            n[0x19] &= 0xCF;                                            // m_bNotHighway = m_bHighway = 0
            n[0x1A] |= 0x0F;
            n[0x1A] &= 0x0F;                                            // m_nSpawnProbability = 15, m_nBehaviourType = 0
            *reinterpret_cast<uint16*>(n + 0xE) = 0x7FFE;               // m_totalDistFromOrigin
        }

        // Simetria de las conexiones: si i apunta a c y c no apunta a i, se anade i al primer hueco (byte negativo) de la fila de c
        for (int32 i = 0; i < numNodes; i++) {
            auto* const rows = reinterpret_cast<int8*>(ConnectsToGiven.data());
            for (int32 j = 0; j < 6; j++) {
                const int8 c = rows[i * 6 + j];
                if (c < 0) {
                    continue;
                }
                int32 k = 0;
                for (; k < 6; k++) {
                    if ((uint8)rows[c * 6 + k] == (uint8)i) {
                        break;
                    }
                }
                if (k < 6) {
                    continue;
                }
                for (k = 0; k < 6; k++) {
                    if (rows[c * 6 + k] < 0) {
                        rows[c * 6 + k] = (int8)i;
                        break;
                    }
                }
            }
        }

        // Numero de conexiones (bytes >= 0 en las filas de los nodos)
        int32 numLinks = 0;
        for (int32 i = 0; i < numNodes; i++) {
            const auto* const row = reinterpret_cast<const int8*>(ConnectsToGiven.data()) + i * 6;
            for (int32 j = 0; j < 6; j++) {
                if (row[j] >= 0) {
                    numLinks++;
                }
            }
        }

        const int32 linkCapacity = numLinks + 0xC0;
        m_pNodeLinks[areaId]         = static_cast<CNodeAddress*>(CMemoryMgr::Malloc(linkCapacity * sizeof(CNodeAddress), 0));
        m_pLinkLengths[areaId]       = static_cast<uint8*>(CMemoryMgr::Malloc(linkCapacity, 0));
        m_pPathIntersections[areaId] = static_cast<CPathIntersectionInfo*>(CMemoryMgr::Malloc(linkCapacity, 0));

        // 0x450FB0 (cdecl, sin reconstruir): area, nodo, area, vecino, ranura, &contador de enlaces
        int32 linkCounter = 0;
        for (int32 i = 0; i < numNodes; i++) {
            auto* const n = reinterpret_cast<uint8*>(&m_pPathNodes[areaId][i]);
            *reinterpret_cast<uint16*>(n + 0x10) = (uint16)linkCounter; // m_wBaseLinkId
            n[0x18] &= 0xF0;                                            // m_nNumLinks = 0
            for (int32 j = 0; j < 6; j++) {
                const int8 c = reinterpret_cast<const int8*>(ConnectsToGiven.data())[i * 6 + j];
                if (c >= 0) {
                    plugin::Call<0x450FB0, int32, int32, int32, int32, int32, int32*>(slot + 0x40, i, slot + 0x40, (int32)c, slot, &linkCounter);
                }
            }
        }

        for (int32 k = numLinks; k < linkCapacity; k++) {
            m_pNodeLinks[areaId][k].m_wAreaId = 0xFFFF;
        }

        m_anNumNodes[areaId]        = NumNodesGiven;
        m_anNumVehicleNodes[areaId] = 0;
        m_anNumPedNodes[areaId]     = NumNodesGiven;
        m_anNumCarPathLinks[areaId] = 0;
        m_anNumAddresses[areaId]    = (uint32)numLinks;

        for (int32 i = 0; NumLinksToExteriorNodes > i; i++) {
            const CNodeAddress interior{ (uint16)(NewInteriorSlot + 0x40), (uint16)aInteriorNodeLinkedToExterior[i] };
            const CNodeAddress exterior = aExteriorNodeLinkedTo[i];
            AddDynamicLinkBetween2Nodes_For1Node(interior, exterior);
            AddDynamicLinkBetween2Nodes_For1Node(exterior, interior);
        }
    }

    bInteriorBeingBuilt = false;
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

// 0x4513F0 (verificada contra el desensamblado, lote 62): devuelve la posicion de un nodo de aparcamiento (tipo de comportamiento 2) de las 64 areas del mapa, entre los nodos de vehiculo cuya posicion cae
// estrictamente dentro de la caja. Se elige el de indice s_parkingNodeCursor (0x977B9C) entre los hallados; si no hay tantos, el primero hallado; sin ninguno, (0, 0, 0).
// El cursor avanza al final (vuelve a 0 al llegar al numero de hallados).
CVector CPathFind::FindParkingNodeInArea(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
    CVector first{};
    CVector selected{};
    bool    anyFound   = false;
    bool    isSelected = false;
    int32   numFound   = 0;

    for (int32 areaId = 0; areaId < NUM_PATH_MAP_AREAS; areaId++) {
        const CPathNode* const nodes = m_pPathNodes[areaId];
        if (!nodes) {
            continue;
        }
        const int32 numNodes = (int32)m_anNumVehicleNodes[areaId];
        if (numNodes <= 0) {
            continue;
        }
        for (int32 i = 0; i < numNodes; i++) {
            const auto* const raw = reinterpret_cast<const uint8*>(&nodes[i]);
            const float       x   = (float)*reinterpret_cast<const int16*>(raw + 0x8) * 0.125f;
            if (!(x > minX && x < maxX)) {
                continue;
            }
            const float y = (float)*reinterpret_cast<const int16*>(raw + 0xA) * 0.125f;
            if (!(y > minY && y < maxY)) {
                continue;
            }
            const float z = (float)*reinterpret_cast<const int16*>(raw + 0xC) * 0.125f;
            if (!(z > minZ && z < maxZ)) {
                continue;
            }
            if ((raw[0x1A] & 0xF0) != 0x20) {
                continue;
            }
            if (numFound == 0) {
                first    = CVector{ x, y, z };
                anyFound = true;
            }
            if (numFound == s_parkingNodeCursor) {
                selected   = CVector{ x, y, z };
                isSelected = true;
            }
            numFound++;
        }
    }

    s_parkingNodeCursor = s_parkingNodeCursor + 1 < numFound ? s_parkingNodeCursor + 1 : 0;
    if (!anyFound) {
        return { 0.0f, 0.0f, 0.0f };
    }
    return isSelected ? selected : first;
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
// Como el original (0x4504F0..0x450559): cuenta los enlaces del nodo cuya area esta cargada y cuyo tipo de comportamiento (byte +0x1A >> 4)
// no es 1 ni 2. El EXE no toca ECX (SwitchOffNodeAndNeighbours del EXE lo sigue usando tras la llamada), deja EAX = cuenta y EDX = el ultimo
// valor que escribe: puntero del nodo (sin enlaces), area del enlace (area sin cargar) o (puntero del nodo enlazado con el byte bajo = tipo).
// ResidueRegsCpp (X87Math.cpp) es la ultima llamada (contrib/audit_register_residue.py, objeto PathFind).
#pragma runtime_checks("", off)
__declspec(safebuffers) size_t CPathFind::CountNeighboursToBeSwitchedOff(const CPathNode& node) {
    const uint32       ecxValue = static_cast<uint32>(reinterpret_cast<uintptr_t>(this));
    const uint8* const raw      = reinterpret_cast<const uint8*>(&node);
    uint32             edxValue = static_cast<uint32>(reinterpret_cast<uintptr_t>(raw)); // mov edx,[esp+4]
    uint32             count    = 0;
    const int32        numLinks = raw[0x18] & 0xF;
    if (numLinks > 0) {
        const int16         baseLink = *reinterpret_cast<const int16*>(raw + 0x10);
        const uint32* const links    = reinterpret_cast<const uint32*>(m_pNodeLinks[*reinterpret_cast<const uint16*>(raw + 0x12)]) + baseLink;
        for (int32 i = 0; i < numLinks; i++) {
            const uint32 link = links[i];
            edxValue          = link & 0xFFFF; // movzx edx,dx
            const uint8* const areaNodes = reinterpret_cast<const uint8*>(m_pPathNodes[link & 0xFFFF]);
            if (!areaNodes) {
                continue;
            }
            const uint8* const linked = areaNodes + (link >> 16) * sizeof(CPathNode);
            const uint32       type   = static_cast<uint32>(linked[0x1A] >> 4);
            edxValue                  = (static_cast<uint32>(reinterpret_cast<uintptr_t>(linked)) & 0xFFFFFF00u) | type; // mov dl,[edx+1Ah]; shr dl,4
            if (type != 1 && type != 2) {
                count++;
            }
        }
    }
    return ResidueRegsCpp(ecxValue, edxValue, count);
}
#pragma runtime_checks("", restore)

// 0x450320
// Como el original (126 instrucciones, 0x450320..0x4504DE; la normalizacion es el CVector::Normalise de 0x59C910, que reproduce
// PathFindNormaliseStatusCpp de X87Math.cpp). Recorre los primeros enlaces-1 enlaces de navegacion del nodo y elige el primero cuyo
// CCarPathLink tenga carriles (bits 3..5 si el CCarPathLink es de este mismo nodo, bits 0..2 si no), o el ultimo enlace; con la diferencia
// XY de los dos nodos (z = 0) normalizada devuelve float(atan2(-x, y) * 57.29578).
// Efectos observables que el banco compara y que se reproducen:
//  - 0x4503E3: mov [esp+38h],eax sobrescribe el argumento fisico con el nodeLink elegido (antes de comprobar que su area este cargada); en
//    la ruta con calculo los fild usan ese mismo hueco de pila como temporal y el ultimo valor que queda es la z entera del nodo ENLAZADO
//    (0x45046D: movsx edx,[eax+0Ch]; mov [esp+30h],edx).
//  - Registros de salida (EAX/ECX/EDX), por ruta: sin area -> (0, this, area); sin enlaces -> ((nodos & ~FF) | byte, this, area); destino sin
//    cargar -> (area del enlace, 0, m_pNodeLinks[area]); calculo -> ((bits de dx & FFFF0000) | fnstsw de Normalise, ESP de entrada - 0x24 =
//    &vector local, bits de dz).
// El estado x87 lo calcula PathFindNormaliseStatusCpp. TOP: la compuerta siembra un ST0 centinela (ResetTestFP) y Normalise lee fnstsw con dos
// valores en la pila (TOP = 6); con la pila vacia de una llamada normal seria 7 y ningun llamador del EXE lee ese EAX.
// PathFindResidueFloatCpp es la ultima llamada de cada salida (sin comprobaciones de ejecucion ni cookie de /GS; contrib/audit_register_residue.py).
#pragma runtime_checks("", off)
__declspec(safebuffers) float CPathFind::FindNodeOrientationForCarPlacement(CNodeAddress nodeInfo) {
    constexpr uint32 kNormaliseTop = 6u << 11; // bits 11..13 del fnstsw
    const uint32     entryEsp      = static_cast<uint32>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()));
    const uint32     self          = static_cast<uint32>(reinterpret_cast<uintptr_t>(this));
    const uint32     areaId        = nodeInfo.m_wAreaId;
    const uint32     nodeId        = nodeInfo.m_wNodeId;

    const uint8* const areaNodes = reinterpret_cast<const uint8*>(m_pPathNodes[areaId]);
    if (!areaNodes) {
        return PathFindResidueFloatCpp(self, areaId, 0.f, 0);
    }
    const uint8* const node  = areaNodes + nodeId * sizeof(CPathNode);
    const uint8        flags = node[0x18];
    if ((flags & 0xF) == 0) {
        return PathFindResidueFloatCpp(self, areaId, 0.f, (static_cast<uint32>(reinterpret_cast<uintptr_t>(areaNodes)) & 0xFFFFFF00u) | flags);
    }

    const int16 baseLink = *reinterpret_cast<const int16*>(node + 0x10);
    int32       chosen   = 0;
    for (const int32 last = static_cast<int32>(flags & 0xF) - 1; chosen < last; chosen++) {
        const uint32 naviLink = *reinterpret_cast<const uint16*>(&m_pNaviLinks[areaId][baseLink + chosen]); // this+0xDA4
        const uint32 naviArea = naviLink >> 10;
        if (!m_pPathNodes[naviArea]) {
            continue;
        }
        const uint8* const naviNode = reinterpret_cast<const uint8*>(m_pNaviNodes[naviArea]) + (naviLink & 0x3FF) * sizeof(CCarPathLink);
        const bool         sameNode = *reinterpret_cast<const uint16*>(naviNode + 4) == nodeInfo.m_wAreaId
                                   && *reinterpret_cast<const uint16*>(naviNode + 6) == nodeInfo.m_wNodeId;
        if (naviNode[0xB] & (sameNode ? 0x38 : 0x07)) {
            break;
        }
    }

    const uint32 nodeLinksPtr = static_cast<uint32>(reinterpret_cast<uintptr_t>(m_pNodeLinks[areaId]));
    const uint32 linkAddress  = *reinterpret_cast<const uint32*>(&m_pNodeLinks[areaId][baseLink + chosen]);
    *reinterpret_cast<volatile uint32*>(&nodeInfo) = linkAddress; // 0x4503E3
    const uint32       linkArea      = linkAddress & 0xFFFF;
    const uint8* const linkAreaNodes = reinterpret_cast<const uint8*>(m_pPathNodes[linkArea]);
    if (!linkAreaNodes) {
        return PathFindResidueFloatCpp(0, nodeLinksPtr, 0.f, linkArea);
    }
    const uint8* const linked = linkAreaNodes + (linkAddress >> 16) * sizeof(CPathNode);

    // Coordenadas comprimidas int16 * 0.125 (0x858C48); las diferencias son exactas en float
    const int32 rawX       = *reinterpret_cast<const int16*>(node + 0x8);
    const int32 rawY       = *reinterpret_cast<const int16*>(node + 0xA);
    const int32 rawZ       = *reinterpret_cast<const int16*>(node + 0xC);
    const int32 linkedRawX = *reinterpret_cast<const int16*>(linked + 0x8);
    const int32 linkedRawY = *reinterpret_cast<const int16*>(linked + 0xA);
    const int32 linkedRawZ = *reinterpret_cast<const int16*>(linked + 0xC);
    CVector     diff;
    diff.x         = static_cast<float>(linkedRawX) * 0.125f - static_cast<float>(rawX) * 0.125f;
    diff.y         = static_cast<float>(linkedRawY) * 0.125f - static_cast<float>(rawY) * 0.125f;
    const float dz = static_cast<float>(linkedRawZ) * 0.125f - static_cast<float>(rawZ) * 0.125f;
    diff.z         = 0.f; // 0x4504BB: mov [esp+10h],0
    const uint32 dxBits = std::bit_cast<uint32>(diff.x);
    const uint32 dzBits = std::bit_cast<uint32>(dz);
    *reinterpret_cast<volatile uint32*>(&nodeInfo) = static_cast<uint32>(linkedRawZ); // ultimo temporal de fild en el hueco del argumento

    const uint32 status  = PathFindNormaliseStatusCpp(&diff.x); // 0x4504C3: call 0x59C910; deja fnstsw en AX
    const float  heading = WeaponHeadingDegreesCpp(diff.x, diff.y);
    return PathFindResidueFloatCpp(entryEsp - 0x24, dzBits, heading, (dxBits & 0xFFFF0000u) | kNormaliseTop | status);
}
#pragma runtime_checks("", restore)

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

// 0x44DF60: pone a -1 las seis casillas de la fila `intId` y todas las referencias con signo a `intId` en las filas activas (NumNodesGiven).
// Como el original (0x44DF60..0x44DF9D): el contador y los indices se comparan con signo y no se limita a 64 filas; `this` no se usa. Registros
// de salida del EXE: sin filas (contador <= 0) conserva el EAX y el ECX de entrada y pone EDX = 0; con filas EAX = 6, ECX = tabla + 6 * filas
// (0x96EAC0) y EDX = filas. EntryEaxCpp tiene que ser lo primero que se ejecuta; ResidueRegsCpp es la ultima llamada de cada salida
// (contrib/audit_register_residue.py, objeto PathFind; sin comprobaciones de ejecucion ni cookie de /GS).
#pragma runtime_checks("", off)
__declspec(safebuffers) void CPathFind::RemoveInteriorLinks(uint32 intId) {
    const uint32 entryEax = EntryEaxCpp();
    const uint32 entryEcx = static_cast<uint32>(reinterpret_cast<uintptr_t>(this));
    const int32  numRows  = static_cast<int32>(NumNodesGiven);
    if (numRows <= 0) {
        (void)ResidueRegsCpp(entryEcx, 0, entryEax);
        return;
    }
    int8* const table = reinterpret_cast<int8*>(ConnectsToGiven.data());
    for (int32 row = 0; row < numRows; row++) {
        int8* const entries = table + row * 6;
        for (int32 slot = 0; slot < 6; slot++) {
            if (row == static_cast<int32>(intId) || static_cast<int32>(entries[slot]) == static_cast<int32>(intId)) {
                entries[slot] = static_cast<int8>(-1);
            }
        }
    }
    (void)ResidueRegsCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(table)) + static_cast<uint32>(numRows) * 6u, static_cast<uint32>(numRows), 6);
}
#pragma runtime_checks("", restore)

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
// Como el original (0x44DA97..0x44DAF1): fild desplazamiento; fmul 0x3BFDF3B6; fadd [x]; fstp [x], a 24 bits y sin redondeo intermedio a float del
// producto; RpNanMulAddCpp (X87Math) es float(a * k + c) con el mismo orden. Los dos anchos se leen otra vez tras escribir x (puede solaparse).
void CPathFind::AddRandomOffsetToNodePosition(CNodeAddress first, CNodeAddress second, int16 random, float* x, float* y) {
    if (first.m_wAreaId == 0xFFFF || !m_pPathNodes[first.m_wAreaId]) return;
    if (second.m_wAreaId == 0xFFFF || !m_pPathNodes[second.m_wAreaId]) return;
    const auto* a = reinterpret_cast<const uint8*>(&m_pPathNodes[first.m_wAreaId][first.m_wNodeId]);
    const auto* b = reinterpret_cast<const uint8*>(&m_pPathNodes[second.m_wAreaId][second.m_wNodeId]);
    const int32 xFactor = (uint16(random) & 15) - 7;
    const int32 yFactor = ((uint16(random) >> 4) & 15) - 7;
    const float scale = std::bit_cast<float>(0x3BFDF3B6u);
    const auto  minWidth = [a, b] { return static_cast<int32>(std::min(a[0x16], b[0x16])); };
    const int32 xDisplacement = xFactor * minWidth();
    *x = RpNanMulAddCpp(static_cast<float>(xDisplacement), scale, *x);
    // Original reloads both widths after writing x, including possible aliasing.
    const int32 yDisplacement = yFactor * minWidth();
    *y = RpNanMulAddCpp(static_cast<float>(yDisplacement), scale, *y);
}
