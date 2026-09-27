/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <span>

#include "Base.h"

#include "AnimBlendStaticAssociation.h"
#include "Enums/AnimationEnums.h"

class CAnimBlock;
struct RpClump;

/*!
 * @brief Grupo de asociaciones asociado normalmente a un bloque de animaciones.
 *
 * Representa conjuntos como ped o int_house y mantiene el mapeo entre nombres,
 * AnimationId y CAnimBlendStaticAssociation. El grupo es propietario de m_Anims,
 * pero m_AnimBlock referencia el catálogo global gestionado por CAnimManager.
*/
class CAnimBlendAssocGroup {
public:
    static void InjectHooks();

    CAnimBlendAssocGroup() = default;
    ~CAnimBlendAssocGroup();

    /*!
     * @addr 0x4CE0B0
     * 
     * @brief Crea una asociación dinámica a partir de una definición estática.
     *
     * @param name Nombre de la animación dentro del grupo.
     * @return Nueva asociación reproducible, o nullptr si no existe la definición.
    */
    CAnimBlendAssociation* CopyAnimation(const char* name);

    //! @addr 0x4CE130
    CAnimBlendAssociation* CopyAnimation(AnimationId id);

    /*!
     * @addr 0x4CE220
     * 
     * @brief Crea las asociaciones predeterminadas del bloque indicado.
     *
     * @param blockName Nombre del bloque que aporta las jerarquías de animación.
    */
    void CreateAssociations(const char* blockName);

    /*!
     * @brief Crea asociaciones concretas contra un mismo RpClump.
     *
     * @param blockName Bloque del que se resuelven las jerarquías.
     * @param clump Clump cuya estructura de frames se utiliza para inicializarlas.
     * @param names Nombres de las animaciones; una cadena vacía reserva un slot sin ID.
     * @param cnt Número de entradas de names.
    */
    void CreateAssociations(const char* blockName, RpClump* clump, const char** names, uint32 cnt);

    /*!
     * @addr 0x4CE3B0
     * 
     * @brief Relaciona animaciones y modelos a partir de buffers paralelos.
     * 
     * @param blockName Bloque fuente.
     * @param animNames Buffer de nombres de animación.
     * @param modelNames Buffer paralelo de nombres de modelo.
     * @param strBufLen Longitud fija de cada campo en ambos buffers.
    */
    void CreateAssociations(const char* blockName, const char* animNames, const char* modelNames, uint32 strBufLen);

    //! @addr 0x4CDFF0
    void DestroyAssociations();

    //! @addr 0x4CE040
    CAnimBlendStaticAssociation* GetAnimation(const char* name);

    //! @addr 0x4CE090
    CAnimBlendStaticAssociation* GetAnimation(AnimationId id);

    //! 0x4CE1B0
    AnimationId GetAnimationId(const char* name);

    //! @addr 0x4CDFB0
    //! @unused
    void InitEmptyAssociations(RpClump* clump);

    //! @addr 0x4D37A0
    //! @unused
    auto IsCreated() const { return m_NumAnims != 0; }

    //! @addr 0x45B050
    //! @unused
    auto GetNumAnimations() const { return m_NumAnims; }

    //! @addr 0x45B060
    //! @unused
    auto GetAnimBlock() const { return m_AnimBlock; }

private:
    CAnimBlock* AllocateForBlock(const char* blockName, int32 numAnims);

    auto GetAssociations() const { return std::span{m_Anims, m_NumAnims}; }

    CAnimBlendAssociation* CopyAnimation(CAnimBlendStaticAssociation* def);
private:
    //! @notsa
    //! @brief Extract common code from `CreateAssociations`
    void CreateAssociation(CAnimBlendStaticAssociation* assoc, CAnimBlendHierarchy* anim, CBaseModelInfo* mi, size_t i);

private: // Wrappers for hooks
    //! @addr 0x4CDE70
    CAnimBlendAssocGroup* Constructor() {
        this->CAnimBlendAssocGroup::CAnimBlendAssocGroup();
        return this;
    }

    //! @addr 0x4CE1D0
    CAnimBlendAssocGroup* Destructor(uint32 flags) {
        std::destroy_at(this);
        return this;
    }

public:
    CAnimBlock*                  m_AnimBlock{};              ///< Bloque global del que proceden las animaciones; no owned.
    CAnimBlendStaticAssociation* m_Anims{};                  ///< Array propiedad del grupo.
    uint32                       m_NumAnims{};               ///< Número de entradas válidas de m_Anims.
    uint32                       m_IdOffset{};               ///< Primer AnimationId reservado para este grupo.
    AssocGroupId                 m_GroupID{ANIM_GROUP_NONE}; ///< Identificador lógico del grupo.
};
VALIDATE_SIZE(CAnimBlendAssocGroup, 0x14);
