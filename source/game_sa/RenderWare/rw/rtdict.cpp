/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RtDict
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rtdict.h"

void RtDictInjectHooks() {
    RH_ScopedNamespaceName("RtDict");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtDictSchemaDestruct, 0x7CED70);
    RH_ScopedGlobalInstall(RtDictSchemaGetCurrentDict, 0x7CEEE0);
}

RtDictSchema* RtDictSchemaInit(RtDictSchema* schema) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*))0x7CED40)(schema);
}

// 0x7CED70: destruye la lista de diccionarios (schema->dictionaries, +0x10) con _rwSListDestroy (0x809440), ignora su resultado y devuelve TRUE.
RwBool RtDictSchemaDestruct(RtDictSchema* schema) {
    _rwSListDestroy(schema->dictionaries);
    return TRUE;
}

RtDict* RtDictSchemaStreamReadDict(RtDictSchema* schema, RwStream* stream) {
    return ((RtDict*(__cdecl *)(RtDictSchema*, RwStream*))0x7CF240)(schema, stream);
}

RtDict* RtDictSchemaCreateDict(RtDictSchema* schema) {
    return ((RtDict*(__cdecl *)(RtDictSchema*))0x7CED90)(schema);
}

RtDictSchema* RtDictSchemaAddDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEE50)(schema, dict);
}

RtDictSchema* RtDictSchemaRemoveDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEE80)(schema, dict);
}

RwBool RtDictSchemaForAllDictionaries(RtDictSchema* schema, RtDictCallBack* callback, RtDictEntryType data) {
    return ((RwBool(__cdecl *)(RtDictSchema*, RtDictCallBack*, RtDictEntryType))0x7CF5B0)(schema, callback, data);
}

// 0x7CEEE0: devuelve schema->current (+0x14).
RtDict* RtDictSchemaGetCurrentDict(RtDictSchema* schema) {
    return schema->current;
}

RtDictSchema* RtDictSchemaSetCurrentDict(RtDictSchema* schema, RtDict* dict) {
    return ((RtDictSchema*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEEF0)(schema, dict);
}

RwBool RtDictDestroy(RtDict* dictionary) {
    return ((RwBool(__cdecl *)(RtDict*))0x7CF130)(dictionary);
}

RtDictEntryType RtDictAddEntry(RtDict* dictionary, RtDictEntryType entry) {
    return ((RtDictEntryType(__cdecl *)(RtDict*, RtDictEntryType))0x7CEFB0)(dictionary, entry);
}

RtDictEntryType RtDictFindNamedEntry(RtDict* dictionary, const RwChar* name) {
    return ((RtDictEntryType(__cdecl *)(RtDict*, const RwChar*))0x7CEFE0)(dictionary, name);
}

const RtDict* RtDictForAllEntries(const RtDict* dictionary, RtDictEntryCallBack* callBack, RtDictEntryType data) {
    return ((const RtDict*(__cdecl *)(const RtDict*, RtDictEntryCallBack*, RtDictEntryType))0x7CF060)(dictionary, callBack, data);
}

RtDict* RtDictRemoveEntry(RtDict* dictionary, RtDictEntryType entry) {
    return ((RtDict*(__cdecl *)(RtDict*, RtDictEntryType))0x7CF0C0)(dictionary, entry);
}

const RtDict* RtDictStreamWrite(const RtDict* dictionary, RwStream* stream) {
    return ((const RtDict*(__cdecl *)(const RtDict*, RwStream*))0x7CF490)(dictionary, stream);
}

RwUInt32 RtDictStreamGetSize(const RtDict* dictionary) {
    return ((RwUInt32(__cdecl *)(const RtDict*))0x7CF1F0)(dictionary);
}

RtDict* _rtDictSchemaInitDict(RtDictSchema* schema, RtDict* dictionary) {
    return ((RtDict*(__cdecl *)(RtDictSchema*, RtDict*))0x7CEF00)(schema, dictionary);
}

RwBool _rtDictDestruct(RtDict* dictionary) {
    return ((RwBool(__cdecl *)(RtDict*))0x7CEF60)(dictionary);
}
