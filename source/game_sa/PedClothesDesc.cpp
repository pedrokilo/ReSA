/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedClothesDesc
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedClothesDesc.h"

void CPedClothesDesc::InjectHooks() {
    RH_ScopedClass(CPedClothesDesc);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5A8020);
    RH_ScopedInstall(Initialise, 0x5A78F0);
    RH_ScopedInstall(GetIsWearingBalaclava, 0x5A7950);
    RH_ScopedInstall(HasVisibleNewHairCut, 0x5A7970);
    RH_ScopedInstall(HasVisibleTattoo, 0x5A79D0);

    RH_ScopedOverloadedInstall(SetModel, "modelHash", 0x5A7910, void(CPedClothesDesc::*)(uint32, eClothesModelPart));
    RH_ScopedOverloadedInstall(SetModel, "modelName", 0x5A7920, void(CPedClothesDesc::*)(const char*, eClothesModelPart));
    // Lote 109A (banco Fn109A): las dos sobrecargas de SetTextureAndModel (0x5A8050 / 0x5A8080). Ambas toman texturePart de la PILA (el EXE solo lo deja ademas en EDX/ECX porque
    // CClothes::GetTextureDependency 0x5A7EA0 conserva esos registros); se pueden enganchar como funciones thiscall normales.
    RH_ScopedOverloadedInstall(SetTextureAndModel, "hash", 0x5A8050, void(CPedClothesDesc::*)(uint32, uint32, eClothesTexturePart));
    RH_ScopedOverloadedInstall(SetTextureAndModel, "name", 0x5A8080, void(CPedClothesDesc::*)(const char*, const char*, eClothesTexturePart));
}

CPedClothesDesc::CPedClothesDesc() {
    Initialise();
}

CPedClothesDesc* CPedClothesDesc::Constructor() {
    this->CPedClothesDesc::CPedClothesDesc();
    return this;
}

// 0x5A78F0
void CPedClothesDesc::Initialise() {
    std::ranges::fill(m_anModelKeys, 0);
    std::ranges::fill(m_anTextureKeys, 0);
    m_fFatStat = 0.0f;
    m_fMuscleStat = 0.0f;
}

// 0x5A7910
void CPedClothesDesc::SetModel(uint32 modelId, eClothesModelPart modelPart) {
    m_anModelKeys[modelPart] = modelId;
}

// 0x5A7920
void CPedClothesDesc::SetModel(const char* model, eClothesModelPart modelPart) {
    if (model)
        m_anModelKeys[modelPart] = CKeyGen::GetUppercaseKey(model);
    else
        m_anModelKeys[modelPart] = 0;
}

// 0x5A7950
bool CPedClothesDesc::GetIsWearingBalaclava() {
    return IsWearingModel("balaclava", CLOTHES_MODEL_SPECIAL);
}

// 0x5A7970
bool CPedClothesDesc::HasVisibleNewHairCut(int32 type) {
    /* Balaclava hides the hair */
    if (m_anModelKeys[CLOTHES_MODEL_SPECIAL] == CKeyGen::GetUppercaseKey("balaclava")) {
        return false;
    }

    /* Hats hide the hair */
    if (m_anModelKeys[CLOTHES_MODEL_HATS] != 0) {
        return false;
    }

    if (m_anModelKeys[CLOTHES_MODEL_HEAD] == CKeyGen::GetUppercaseKey("head")) {
        return false;
    }

    return type != 1 || m_anModelKeys[CLOTHES_MODEL_HEAD] == CKeyGen::GetUppercaseKey("afro");
}

// 0x5A79D0
bool CPedClothesDesc::HasVisibleTattoo() {
    // NOTE: Android: CLOTHES_TEX_TATTOOS1 = 4, CLOTHES_TEX_TATTOOS9 = 12
    for (int i = eClothesTexturePart::CLOTHES_TEXTURE_LOWER_LEFT_ARM; i <= eClothesTexturePart::CLOTHES_TEXTURE_UPPER_BACK; ++i) {
        if (m_anTextureKeys[i]) return true;
    }

    return false;
}

// 0x5A8050
void CPedClothesDesc::SetTextureAndModel(uint32 texture, uint32 model, eClothesTexturePart texturePart) {
    // Como el original: sin comprobar el rango de texturePart (la escritura de la textura es m_anTextureKeys + texturePart sin limite) y con la dependencia
    // consultada ANTES de escribir la textura; el modelo solo se escribe si hay dependencia (distinta de CLOTHES_MODEL_UNAVAILABLE).
    const auto modelPart = CClothes::GetTextureDependency(texturePart);
    m_anTextureKeys.data()[texturePart] = texture;
    if (modelPart != CLOTHES_MODEL_UNAVAILABLE) {
        m_anModelKeys.data()[modelPart] = model;
    }
}

// 0x5A8080
void CPedClothesDesc::SetTextureAndModel(const char* textureName, const char* modelName, eClothesTexturePart texturePart) {
    // Como el original: ambos hashes se calculan primero (cada uno solo si su nombre no es nulo; si lo es, 0), el texture primero; despues la dependencia, la textura y,
    // si hay dependencia, el modelo (aunque la dependencia no exista el hash del modelo ya se calculo). Sin comprobar el rango de texturePart.
    const uint32 textureKey = textureName ? CKeyGen::GetUppercaseKey(textureName) : 0u;
    const uint32 modelKey   = modelName ? CKeyGen::GetUppercaseKey(modelName) : 0u;
    const auto modelPart    = CClothes::GetTextureDependency(texturePart);
    m_anTextureKeys.data()[texturePart] = textureKey;
    if (modelPart != CLOTHES_MODEL_UNAVAILABLE) {
        m_anModelKeys.data()[modelPart] = modelKey;
    }
}

