#pragma once

#include "List_c.h"
#include "Interior_c.h"
#include "InteriorGroup_c.h"
#include "InteriorObject.h"
#include "InteriorEffectInfo_t.h"

/**
 * Gestor de interiores dinámicos generados a partir de efectos 2DFX del mundo.
 *
 * Mantiene pools fijos de Interior_c e InteriorGroup_c, activa sólo los efectos
 * próximos/visibles, relaciona peatones y objetos robables con su interior y evita
 * asignaciones continuas devolviendo estructuras a listas libres al dejar de usarse.
 */
class InteriorManager_c {
    friend class Interior_c; // Furniture placement updates the original fixed steal-data store.
public:
    static void InjectHooks();

    InteriorManager_c() = default;  // 0x5984C0
    ~InteriorManager_c() = default; // 0x598580

    bool Init(); // Como el original: devuelve true (al = 1)
    bool Update();
    void Exit();

    // Ambas son __thiscall en el original (el llamador carga ecx=&g_interiorMan y la
    // función termina con `ret 4`/`ret 8`). Declararlas `static` (cdecl, `ret` sin
    // limpiar argumentos) desplazaba la pila del llamador cuando un código original
    // las invocaba a través del hook. No usan `this`.
    void AddSameGroupEffectInfos(InteriorEffectInfo_t* intFxInfo, int32 a2);
    bool AreAnimsLoaded(int32 animBlock) const;

    void PruneVisibleEffects(InteriorEffectInfo_t* pInteriorEffectInfos, size_t numInfos, size_t reqdNumInfos, float maxDist);
    size_t GetVisibleEffects(InteriorEffectInfo_t* effectInfo, uint32 totalEffects);
    bool IsInteriorEffectVisible(C2dEffectInterior* effect, CEntity* entity);

    /*!
    * @addr 0x5984B0
    * 
    * Devuelve un Interior_c a la lista libre; no destruye su almacenamiento.
    */
    void ReturnInteriorToPool(Interior_c* interior);

    /*!
    * @addr 0x5984A0
    * @returns Una entrada libre del pool, o nullptr si no quedan disponibles.
    */
    Interior_c* GetInteriorFromPool();

    /*!
    * @addr 0x5983D0
    *
    * @returns El Interior_c que contiene pt, o nullptr si ninguno lo contiene.
    */
    Interior_c* GetVectorsInterior(const CVector& pt);

    /*!
    * @addr 0x598620
    *
    * @returns El Interior_c que contiene la posición del ped, o nullptr.
    */
    Interior_c* GetPedsInterior(const CPed* ped);

    void SetStealableObjectStolen(CEntity* entity, uint8 a3);
    int32 FindStealableObjectId(CEntity* entity) const;
    int32 FindStealableObjectId(int32 interiorId, int32 modelId, CVector point) const;
    bool HasInteriorHadStealDataSetup(Interior_c* interior) const;
    int8 IsGroupActive(int32 group) const;
    InteriorGroup_c* GetPedsInteriorGroup(const CPed* ped);
    void SetEntryExitPtr(CEntryExit* exit);
    bool GetBoundingBox(CEntity* entity, CVector* pos); // Como el original: busca primero el FurnitureEntity_c de la entidad
    void ActivatePeds(bool enable);

    auto GetInteriorIds() const { return m_InteriorIds | rng::views::take(m_InteriorCount); }
    auto GetObjects() const { return m_Objects | rng::views::take(m_ObjectCount); }

private:
    Interior_c               m_Interiors[8]{};               // Store fijo del pool de interiores.               // 0x0
    TList_c<Interior_c>      m_InteriorPool{};               // 0x3CA0
    InteriorGroup_c          m_InteriorGroups[8]{};          // Store fijo de grupos de interior.          // 0x3CAC
    TList_c<InteriorGroup_c> m_InteriorGroupList{};          // 0x428C
    TList_c<InteriorGroup_c> m_InteriorGroupPool{};          // 0x4298
    size_t                   m_InteriorCount{};              // 0x42A4
    int32                    m_InteriorIds[64]{};            // 0x42A8
    int32                    m_ObjectCount{};                // 0x43A8
    InteriorObject           m_Objects[32]{};                // 0x43AC
    bool                     m_InteriorPedsAliveState[16]{}; // 0x472C
    CEntryExit*              m_EnEx{};                       // 0x473C
    CRect                    m_EnExRect{};                   // 0x4740
    bool                     m_IsFrozen{};                   // 0x4750
    bool                     m_IsActive{};                   // 0x4751
    bool                     m_ArePedsEnabled{};             // 0x4752
    int32                    m_TimeLastPickupsGenerated{};   // 0x4754
};
VALIDATE_SIZE(InteriorManager_c, 0x4758);

static inline auto& g_interiorMan = StaticRef<InteriorManager_c>(0xBAF670);
