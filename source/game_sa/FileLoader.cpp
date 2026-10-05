/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CFileLoader
 * Reconstrucción: COMPLETA — 47/47 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/47 entradas.
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

#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "Occlusion.h"
#include "PedType.h"
#include "ColHelpers.h"
#include "TempColModels.h"
#include "PlantMgr.h"
#include "StuntJumpManager.h"
#include "EntryExitManager.h"
#include "PedStats.h"
#include "LoadingScreen.h"
#include "Garages.h"
#include "Glass.h"

#define CHECK_ARG_COUNT(_l, _expected, _n) \
    do { \
        if (_n < _expected) { \
            NOTSA_LOG_WARN("Expected {} values to be read, got {}. Default initialized values will be used instead", _expected, _n); \
            NOTSA_LOG_WARN("Line: {:?}", _l); \
        } \
    } while (0)
auto& gAtomicModelId = StaticRef<uint32>(0xB71840);

void LinkLods(int32 a1);

void CFileLoader::InjectHooks() {
    RH_ScopedClass(CFileLoader);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(AddTexDictionaries, 0x5B3910);
    RH_ScopedInstall(LoadTexDictionary, 0x5B3860);
    RH_ScopedOverloadedInstall(LoadAtomicFile, "stream", 0x5371F0, bool(*)(RwStream*, unsigned));
    RH_ScopedOverloadedInstall(LoadAtomicFile, "", 0x5B39D0, void(*)(const char*));
    RH_ScopedInstall(LoadAudioZone, 0x5B4D70);
    RH_ScopedOverloadedInstall(LoadCarGenerator, "0", 0x537990, void(*)(CFileCarGenerator*, int32));
    RH_ScopedOverloadedInstall(LoadCarGenerator, "1", 0x5B4740, void(*)(const char*, int32));
    RH_ScopedInstall(StartLoadClumpFile, 0x5373F0);
    RH_ScopedInstall(FinishLoadClumpFile, 0x537450);
    RH_ScopedOverloadedInstall(LoadClumpFile, "", 0x5B3A30, void (*)(const char*));
    RH_ScopedOverloadedInstall(LoadClumpFile, "1", 0x5372D0, bool (*)(RwStream*, uint32));
    RH_ScopedInstall(LoadClumpObject, 0x5B4040);
    RH_ScopedInstall(LoadCullZone, 0x5B4B40);
    RH_ScopedInstall(LoadObject, 0x5B3C60);
    RH_ScopedOverloadedInstall(LoadObjectInstance, "inst", 0x538090, CEntity * (*)(CFileObjectInstance*, const char*));
    RH_ScopedOverloadedInstall(LoadObjectInstance, "file", 0x538690, CEntity * (*)(const char*));
    RH_ScopedInstall(LoadOcclusionVolume, 0x5B4C80);
    RH_ScopedInstall(LoadPathHeader, 0x5B41C0);
    RH_ScopedInstall(LoadStuntJump, 0x5B45D0);
    RH_ScopedInstall(LoadTXDParent, 0x5B75E0);
    RH_ScopedInstall(LoadTimeCyclesModifier, 0x5B81D0);
    RH_ScopedInstall(LoadTimeObject, 0x5B3DE0);
    RH_ScopedInstall(LoadWeaponObject, 0x5B3FB0);
    RH_ScopedInstall(LoadZone, 0x5B4AB0);
    RH_ScopedInstall(FindRelatedModelInfoCB, 0x5B3930);
    RH_ScopedInstall(SetRelatedModelInfoCB, 0x537150);

    RH_ScopedOverloadedInstall(LoadCollisionFile, "Buffer", 0x538440, bool(*)(uint8*, uint32, uint8));
    RH_ScopedOverloadedInstall(LoadCollisionFile, "File", 0x5B4E60, void(*)(const char*, uint8));
    RH_ScopedInstall(LoadCollisionFileFirstTime, 0x5B5000);
    RH_ScopedInstall(LoadCollisionModel, 0x537580);
    RH_ScopedInstall(LoadCollisionModelVer2, 0x537EE0);
    RH_ScopedInstall(LoadCollisionModelVer3, 0x537CE0);
    RH_ScopedInstall(LoadCollisionModelVer4, 0x537AE0);

    RH_ScopedInstall(LoadAnimatedClumpObject, 0x5B40C0);
    RH_ScopedOverloadedInstall(LoadLine, "File", 0x536F80, char* (*)(FILESTREAM));
    RH_ScopedOverloadedInstall(LoadLine, "Bufer", 0x536FE0, char* (*)(char*&, int32&));
    RH_ScopedInstall(LoadCarPathNode, 0x5B4380);
    RH_ScopedInstall(LoadPedPathNode, 0x5B41F0);
    RH_ScopedInstall(LoadVehicleObject, 0x5B6F30);
    RH_ScopedInstall(LoadPedObject, 0x5B7420);
    RH_ScopedInstall(LoadPickup, 0x5B47B0);
    RH_ScopedInstall(LoadEntryExit, 0x5B8030);
    RH_ScopedInstall(LoadGarage, 0x5B4530);
    RH_ScopedInstall(LoadLevel, 0x5B9030);
    RH_ScopedInstall(LoadScene, 0x5B8700);
    RH_ScopedInstall(LoadObjectTypes, 0x5B8400);

    RH_ScopedGlobalInstall(LinkLods, 0x5B51E0); 
}

// copy textures from dictionary to baseDictionary
// 0x5B3910
void CFileLoader::AddTexDictionaries(RwTexDictionary* dictionary, RwTexDictionary* baseDictionary) {
    RwTexDictionaryForAllTextures(baseDictionary, AddTextureCB, dictionary);
}

// save txd to file
// unused
// 0x5B38C0
void CFileLoader::SaveTexDictionary(RwTexDictionary* dictionary, const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, filename);
    if (stream) {
        RwTexDictionaryStreamWrite(dictionary, stream);
        RwStreamClose(stream, nullptr);
    }
}

// load txd from file
// 0x5B3860
RwTexDictionary* CFileLoader::LoadTexDictionary(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (stream) {
        // Como el original: el stream se cierra una sola vez, haya o no diccionario
        RwTexDictionary* txd = nullptr;
        if (RwStreamFindChunk(stream, rwID_TEXDICTIONARY, nullptr, nullptr)) {
            txd = RwTexDictionaryGtaStreamRead(stream);
        }
        RwStreamClose(stream, nullptr);
        if (txd)
            return txd;
    }
    return RwTexDictionaryCreate();
}

// 0x5B40C0
int32 CFileLoader::LoadAnimatedClumpObject(const char* line) {
    int32  objID{ MODEL_INVALID };
    char   modelName[24]{};
    char   txdName[24]{};
    char   animName[16]{ "null" };
    float  drawDist{ 2000.f };
    uint32 flags{};

    if (sscanf(line, "%d %s %s %s %f %d", &objID, modelName, txdName, animName, &drawDist, &flags) != 6)
        return -1;

    auto mi = CModelInfo::AddClumpModel(objID);

    mi->m_nKey = CKeyGen::GetUppercaseKey(modelName);
    mi->SetTexDictionary(txdName);
    mi->m_fDrawDistance = drawDist; // if you forgot to add this line here, you will meet with UB on closing game, have a nice day
    mi->SetAnimFile(animName);
    SetClumpModelInfoFlags(mi, flags);

    if (std::string_view{ animName } != "null")
        mi->bHasAnimBlend = true;

    return objID;
}

// 0x5371F0
bool CFileLoader::LoadAtomicFile(RwStream* stream, uint32 modelId) {
    auto mi = CModelInfo::GetModelInfo(modelId)->AsAtomicModelInfoPtr();
    bool bUseCommonVehicleTexDictionary = false;
    if (mi && mi->bUseCommonVehicleDictionary) {
        bUseCommonVehicleTexDictionary = true;
        CVehicleModelInfo::UseCommonVehicleTexDicationary();
    }

    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
        RpClump* pReadClump = RpClumpStreamRead(stream);
        if (!pReadClump) {
            if (bUseCommonVehicleTexDictionary) {
                CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();
            }
            return false;
        }
        gAtomicModelId = modelId;
        RpClumpForAllAtomics(pReadClump, (RpAtomicCallBack)SetRelatedModelInfoCB, pReadClump);
        RpClumpDestroy(pReadClump);
    }

    if (!mi->m_pRwObject) // Como el original: lee +0x1C en crudo, sin comprobar mi ni el tipo del objeto RW
        return false;

    if (bUseCommonVehicleTexDictionary)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    return true;
}

// 0x5B39D0
void CFileLoader::LoadAtomicFile(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
    {
        if (RpClump* clump = RpClumpStreamRead(stream)) {
            RpClumpForAllAtomics(clump, FindRelatedModelInfoCB, clump);
            RpClumpDestroy(clump);
        }
    }
    RwStreamClose(stream, nullptr);
}

// unused
// 0x537060
RpClump* CFileLoader::LoadAtomicFile2Return(const char* filename) {
    RpClump* clump = nullptr;
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    if (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        clump = RpClumpStreamRead(stream);
    RwStreamClose(stream, nullptr);

    return clump;
}

// NOTSA
char* CFileLoader::FindFirstNonNullOrWS(char* it) {
    // Have to cast to uint8, because signed ASCII is retarded
    for (; *it && (uint8)*it <= (uint8)' '; it++);
    return it;
}

// NOTSA
char* CFileLoader::FindFirstNullOrWS(char* it) {
    for (; *it && *it > ' '; it++);
    return it;
}

// 0x536F80
// Load line into static buffer (`ms_line`)
char* CFileLoader::LoadLine(auto file) {
    if (!CFileMgr::ReadLine(file, ms_line, sizeof(ms_line)))
        return nullptr;

    // Sanitize it (otherwise random crashes appear)
    for (char* it = ms_line; *it; it++) {
        // Have to cast to uint8, because signed ASCII is retarded
        if ((uint8)*it < (uint8)' ' || *it == ',')
            *it = ' ';
    }

    return FindFirstNonNullOrWS(ms_line);
}

/*!
* Load line from a text buffer with sanitization (replaces chars < 32 (space) with a space)
* @param bufferIt Iterator into buffer. It is modified by this function to point after the last character of this line
* @param buffSize Size of buffer. It is modified to represent the size of the buffer remaining after the end of this line
* @returns The beginning of the line - Note, this isn't a pointer into the passed in buffer!
* @addr 0x536FE0
*/
char* CFileLoader::LoadLine(char*& bufferIt, int32& buffSize) {
    if (buffSize <= 0 || !*bufferIt)
        return nullptr;

    // Copy with sanitization (Otherwise random crashes appear)
    char* copyIt = s_MemoryHeapBuffer;
    for (; *bufferIt && *bufferIt != '\n' && buffSize != 0; bufferIt++, buffSize--) {
        // Como el original: '\r' (< ' ') se copia como espacio (LoadCutSceneFile solo usa starts_with/strtok/sscanf)
        // Have to cast to uint8, because signed ASCII is retarded
        *copyIt++ = ((uint8)*bufferIt < (uint8)' ' || *bufferIt == ',') ? ' ' : *bufferIt; // Replace chars before space and ',' (comma) by space, otherwise copy
    }

    bufferIt++;    // Make it point after end of line
    *copyIt = 0;   // Null terminate the copy

    return FindFirstNonNullOrWS(s_MemoryHeapBuffer);
}

// IPL -> AUZO
// 0x5B4D70
void CFileLoader::LoadAudioZone(const char* line) {
    char name[16]; // Como el original: 16 bytes en la pila
    int32 id;
    int32 flags;
    float x1, y1, z1;
    float x2, y2, z2;
    float radius;

    // Como el original: activo = flags != 0 (setne) y el recuento de la esfera no se comprueba
    if (sscanf(line, "%s %d %d %f %f %f %f %f %f", name, &id, &flags, &x1, &y1, &z1, &x2, &y2, &z2) == 9) {
        CAudioZones::RegisterAudioBox(name, id, flags != 0, {x1, y1, z1}, {x2, y2, z2});
    } else {
        sscanf(line, "%s %d %d %f %f %f %f", name, &id, &flags, &x1, &y1, &z1, &radius);
        CAudioZones::RegisterAudioSphere(name, id, flags != 0, {x1, y1, z1}, radius);
    }
}

// unused?
// 0x0
void CFileLoader::LoadBoundingBox(uint8* data, CBoundingBox& outBoundBox) {

}

// 0x537990
void CFileLoader::LoadCarGenerator(CFileCarGenerator* carGen, int32 iplId) {
    // Como el original: un solo fmul por 57.295776 (0x42652EE0) y el ultimo argumento es el byte
    // `flags & 2` sin normalizar (Setup lo enmascara con & 1, asi que el EXE nunca ignora el limite)
    using CreateFn = int32(__cdecl*)(CVector, float, int32, int16, int16, uint8, uint8, uint8, uint16, uint16, uint8, uint8);
    auto index = reinterpret_cast<CreateFn>(&CTheCarGenerators::CreateCarGenerator)(
        carGen->m_vecPosn,
        carGen->m_fAngle * std::bit_cast<float>(0x42652EE0u),
        carGen->m_nModelId,
        (int16)carGen->m_nPrimaryColor,
        (int16)carGen->m_nSecondaryColor,
        (uint8)(carGen->m_nFlags & 1),
        (uint8)carGen->m_nAlarmChance,
        (uint8)carGen->m_nDoorLockChance,
        (uint16)carGen->m_nMinDelay,
        (uint16)carGen->m_nMaxDelay,
        (uint8)iplId,
        (uint8)(carGen->m_nFlags & 2)
    );
    if (index >= 0)
        CTheCarGenerators::Get(index)->SwitchOn();
}

// IPL -> CARS
// 0x5B4740
void CFileLoader::LoadCarGenerator(const char* line, int32 iplId) {
    CFileCarGenerator carGen; // Como el original: sin inicializar (con 12 campos se escriben todos)
    if (sscanf(
        line,
        "%f %f %f %f %d %d %d %d %d %d %d %d",
        &carGen.m_vecPosn.x,
        &carGen.m_vecPosn.y,
        &carGen.m_vecPosn.z,
        &carGen.m_fAngle,
        &carGen.m_nModelId,
        &carGen.m_nPrimaryColor,
        &carGen.m_nSecondaryColor,
        &carGen.m_nFlags,
        &carGen.m_nAlarmChance,
        &carGen.m_nDoorLockChance,
        &carGen.m_nMinDelay,
        &carGen.m_nMaxDelay
    ) == 12) {
        LoadCarGenerator(&carGen, iplId);
    }
}

// 0x5B4380
void CFileLoader::LoadCarPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex, bool a4) {
    // Loads some data from the line, and calls a function which does nothing, so the whole function.. does nothing.
    // Leftover from VC
    // See code in re3
}

// 0x5373F0
bool CFileLoader::StartLoadClumpFile(RwStream* stream, uint32 modelIndex) {
    // Como el original: el tipo se consulta antes de buscar el chunk
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(modelIndex);
    bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE;

    if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        return false;

    if (isVehicle)
        CVehicleModelInfo::UseCommonVehicleTexDicationary();

    auto clumpReaded = RpClumpGtaStreamRead1(stream);

    if (isVehicle)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    return clumpReaded;
}

// 0x537450
bool CFileLoader::FinishLoadClumpFile(RwStream* stream, uint32 modelIndex) {
    auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(modelIndex));
    bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE;

    if (isVehicle)
        CVehicleModelInfo::UseCommonVehicleTexDicationary();

    RpClump* clump = RpClumpGtaStreamRead2(stream);

    if (isVehicle)
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();

    if (!clump)
        return false;

    mi->SetClump(clump);
    return true;
}

// 0x5372D0
bool CFileLoader::LoadClumpFile(RwStream* stream, uint32 modelIndex) {
    auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(modelIndex));
    const bool isVehicle = mi->GetModelType() == MODEL_INFO_VEHICLE; // Como el original: se consulta siempre, al principio

    if (mi->bHasComplexHierarchy) {
        RpClump* parentClump = RpClumpCreate();
        RwFrame* parentFrame = RwFrameCreate();
        RpClumpSetFrame(parentClump, parentFrame);
        if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
            mi->SetClump(parentClump);
            return true;
        }

        while (true) {
            RpClump* childClump = RpClumpStreamRead(stream);
            if (!childClump)
                return false;

            RwFrame* childFrame = _rwFrameCloneAndLinkClones(RpClumpGetFrame(childClump));
            RwFrameAddChild(parentFrame, childFrame);
            RpClumpForAllAtomics(childClump, CloneAtomicToClumpCB, parentClump);
            RpClumpDestroy(childClump);

            if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
                mi->SetClump(parentClump);
                return true;
            }
        }
    }

    if (!RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
        return false;

    if (isVehicle) {
        CCollisionPlugin::SetModelInfo(mi);
        CVehicleModelInfo::UseCommonVehicleTexDicationary();
    }

    RpClump* clump = RpClumpStreamRead(stream);

    if (isVehicle) {
        CCollisionPlugin::SetModelInfo(nullptr);
        CVehicleModelInfo::StopUsingCommonVehicleTexDicationary();
    }

    if (!clump)
        return false;

    mi->SetClump(clump);

    if (modelIndex == MODEL_JOURNEY)
        mi->AsVehicleModelInfoPtr()->m_nNumDoors = 2;

    return true;
}

// 0x5B3A30
void CFileLoader::LoadClumpFile(const char* filename) {
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, filename);
    while (RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr))
    {
        if (RpClump* clump = RpClumpStreamRead(stream)) {
            const char* nodeName = GetFrameNodeName(RpClumpGetFrame(clump));
            if (auto mi = static_cast<CClumpModelInfo*>(CModelInfo::GetModelInfo(nodeName, nullptr)))
                mi->SetClump(clump);
            else
                RpClumpDestroy(clump);
        }
    }
    RwStreamClose(stream, nullptr);
}

// 0x5B4040
int32 CFileLoader::LoadClumpObject(const char* line) {
    int32 objId{ MODEL_INVALID };
    char  modelName[24];
    char  texName[24];

    if (sscanf(line, "%d %s %s", &objId, modelName, texName) != 3)
        return MODEL_INVALID;

    auto mi = static_cast<CVehicleModelInfo*>(CModelInfo::AddClumpModel(objId));
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    mi->SetColModel(&CTempColModels::ms_colModelBBox, false);
    return objId;
}

// 0x538440
// Load one, or multiple, collision models from the given buffer
// Como el original: sigue mientras queden mas de 8 bytes (sin signo), el model info se conserva entre
// registros (si el id >= 20000 se reutiliza el del registro anterior), sin asserts y con COL4.
bool CFileLoader::LoadCollisionFile(uint8* buff, uint32 buffSize, uint8 colId) {
    using namespace ColHelpers;

    CBaseModelInfo* mi = nullptr;
    auto remaining = buffSize;
    auto it = buff;
    while (remaining > 8) {
        uint32 fourcc, fileSize;
        memcpy(&fourcc, it, 4);
        memcpy(&fileSize, it + 4, 4);
        remaining -= 8;
        it += 8;

        int32 version;
        switch (fourcc) {
        case MakeFourCC("COLL"): version = 0; break;
        case MakeFourCC("COL2"): version = 1; break;
        case MakeFourCC("COL3"): version = 2; break;
        case MakeFourCC("COL4"): version = 3; break;
        default:                 return true;
        }

        char name[22];
        memcpy(name, it, sizeof(name));
        uint16 modelId;
        memcpy(&modelId, it + 22, 2);
        it += 24;
        remaining -= 24;

        if (modelId < 20000) {
            mi = CModelInfo::ms_modelInfoPtrs[modelId];
        }
        if (!mi || mi->m_nKey != CKeyGen::GetUppercaseKey(name)) {
            auto colDef = CColStore::GetInSlot(colId);
            mi = CModelInfo::GetModelInfo(name, colDef->m_nModelIdStart, colDef->m_nModelIdEnd);
        }

        if (mi && mi->bIsLod) {
            auto cm = mi->GetColModel();
            if (!cm) {
                cm = new CColModel;
                mi->SetColModel(cm, true);
            }

            switch (version) {
            case 0: LoadCollisionModel(it, *cm); break;
            case 1: LoadCollisionModelVer2(it, fileSize - 24, *cm, name); break;
            case 2: LoadCollisionModelVer3(it, fileSize - 24, *cm, name); break;
            case 3: LoadCollisionModelVer4(it, fileSize - 24, *cm, name); break;
            }

            cm->m_nColSlot = colId;
            if (mi->GetModelType() == MODEL_INFO_ATOMIC) {
                CPlantMgr::SetPlantFriendlyFlagInAtomicMI(static_cast<CAtomicModelInfo*>(mi));
            }
        }

        it += fileSize - 24;
        remaining += 24 - fileSize;
    }
    return true;
}

// 0x5B4E60
// Como el original: COL4 (o un fourcc desconocido) conserva la version del registro anterior y, si el
// model info no tenia modelo de colision, se le asigna uno nuevo vacio pero los datos se cargan en el del
// registro anterior (el EXE no actualiza el puntero). Sin registro anterior el EXE usa basura de la pila;
// aqui no se carga nada.
void CFileLoader::LoadCollisionFile(const char* filename, uint8 colId) {
    auto& buffer = StaticRef<uint8[0x8000]>(0xBC40D8); // 32 kB

    using namespace ColHelpers;

    FileHeader header;
    int32 version = -1;
    CColModel* cm = nullptr;

    auto f = CFileMgr::OpenFile(filename, "rb");
    while (CFileMgr::Read(f, &header.info, sizeof(header.info))) {
        switch (MakeFourCC(header.info.fourcc)) {
        case MakeFourCC("COL2"): version = 1; break;
        case MakeFourCC("COL3"): version = 2; break;
        case MakeFourCC("COLL"): version = 0; break;
        default: break;
        }

        // Read remaining header info. This is stupid, no idea why it's read separately like this.
        CFileMgr::Read(f, &header.info + 1, sizeof(FileHeader) - sizeof(FileHeader::FileInfo));

        // Read actual data
        CFileMgr::Read(f, buffer, header.info.size - 24);

        const auto mi = CModelInfo::GetModelInfo(header.modelName, nullptr);
        if (!mi || !mi->bIsLod)
            continue;

        if (mi->GetColModel()) {
            cm = mi->GetColModel();
        } else {
            mi->SetColModel(new CColModel, true);
        }

        if (!cm) {
            continue;
        }

        switch (version) {
        case 0: LoadCollisionModel(buffer, *cm); break;
        case 1: LoadCollisionModelVer2(buffer, header.info.size - 24, *cm, header.modelName); break;
        case 2: LoadCollisionModelVer3(buffer, header.info.size - 24, *cm, header.modelName); break;
        }

        cm->m_nColSlot = colId;
    }
    CFileMgr::CloseFile(f);
}

// 0x5B5000
// Como el original: solo COLL/COL2/COL3 (otro fourcc termina), el model info y su id se conservan entre
// registros, se cuenta mientras queden mas de 8 bytes y sin asserts.
bool CFileLoader::LoadCollisionFileFirstTime(uint8* buff, uint32 buffSize, uint8 colId) {
    using namespace ColHelpers;

    CBaseModelInfo* mi = nullptr;
    int32 modelId;
    auto remaining = buffSize;
    auto it = buff;
    while (remaining > 8) {
        uint32 fourcc, fileSize;
        memcpy(&fourcc, it, 4);
        memcpy(&fileSize, it + 4, 4);
        it += 8;
        remaining -= 8;

        int32 version;
        switch (fourcc) {
        case MakeFourCC("COL2"): version = 1; break;
        case MakeFourCC("COL3"): version = 2; break;
        case MakeFourCC("COLL"): version = 0; break;
        default:                 return true;
        }

        char name[22];
        memcpy(name, it, sizeof(name));
        uint16 id16;
        memcpy(&id16, it + 22, 2);
        it += 24;
        remaining -= 24;

        modelId = id16;
        if (modelId < 20000) {
            mi = CModelInfo::ms_modelInfoPtrs[modelId];
        }
        if (!mi || mi->m_nKey != CKeyGen::GetUppercaseKey(name)) {
            mi = CModelInfo::GetModelInfo(name, &modelId);
        }

        if (mi) {
            CColStore::IncludeModelIndex(colId, modelId);

            if (mi->bIsLod) {
                auto cm = new CColModel;
                switch (version) {
                case 0: LoadCollisionModel(it, *cm); break;
                case 1: LoadCollisionModelVer2(it, fileSize - 24, *cm, name); break;
                case 2: LoadCollisionModelVer3(it, fileSize - 24, *cm, name); break;
                }

                cm->m_nColSlot = colId;
                mi->SetColModel(cm, true);
                CColAccel::addCacheCol(PackedModelStartEnd{ .modelId = modelId }, *cm);
            }
        }

        it += fileSize - 24;
        remaining += 24 - fileSize;
    }
    return true;
}

// 0x537580
// Load collision V1 file from buffer. Just note that `buffer` is pointing to after `FileHeader`.
// Como el original: lectura byte a byte del formato COLL (contadores con signo, lineas descartadas con
// m_nNumLines = 0, esferas/cajas escritas campo a campo como CColSphere::Set/CColBox::Set sin tocar el
// byte de relleno, vertices comprimidos *128 con truncamiento de _ftol2).
void CFileLoader::LoadCollisionModel(uint8* buffer, CColModel& cm) {
    auto it = buffer;
    const auto ReadU32 = [&]() { uint32 v; memcpy(&v, it, 4); it += 4; return v; };
    const auto ReadF   = [&](const uint8* p) { float f; memcpy(&f, p, 4); return f; };

    // Cabecera: radio, centro, minimo y maximo de la caja
    cm.m_boundSphere.m_fRadius = ReadF(it);
    memcpy(&cm.m_boundSphere.m_vecCenter, it + 4, sizeof(CVector));
    memcpy(&cm.m_boundBox.m_vecMin, it + 16, sizeof(CVector));
    memcpy(&cm.m_boundBox.m_vecMax, it + 28, sizeof(CVector));
    it += 40;

    auto cd = new CCollisionData;
    cm.m_pColData = cd;

    // Esferas: TSphere {radio, centro, material, flag, brillo, luz} -> CColSphere::Set(radio, centro, material, flag, luz)
    cd->m_nNumSpheres = (uint16)ReadU32();
    if ((int16)cd->m_nNumSpheres > 0) {
        cd->m_pSpheres = (CColSphere*)CMemoryMgr::Malloc((int16)cd->m_nNumSpheres * 0x14, 0);
        for (int32 i = 0; i < (int16)cd->m_nNumSpheres; i++, it += 0x14) {
            auto* d = reinterpret_cast<uint8*>(cd->m_pSpheres) + i * 0x14;
            memcpy(d + 0xC, it, 4);       // radio
            memcpy(d, it + 4, 12);        // centro
            d[0x10] = it[0x10];           // material
            d[0x11] = it[0x11];           // flag
            d[0x12] = it[0x13];           // luz
        }
    } else {
        cd->m_pSpheres = nullptr;
    }

    // Lineas: se saltan (solo si el contador con signo es positivo) y se descartan
    cd->m_nNumLines = *it;
    it += 4;
    if ((int8)cd->m_nNumLines > 0) {
        it += (int8)cd->m_nNumLines * 24;
    } else {
        cd->m_pLines = nullptr;
    }
    cd->m_nNumLines = 0;
    cd->m_pLines = nullptr;

    // Cajas: TBox {min, max, material, flag, brillo, luz} -> CColBox::Set(min, max, material, flag, luz)
    cd->m_nNumBoxes = (uint16)ReadU32();
    if ((int16)cd->m_nNumBoxes > 0) {
        cd->m_pBoxes = (CColBox*)CMemoryMgr::Malloc((int16)cd->m_nNumBoxes * 0x1C, 0);
        for (int32 i = 0; i < (int16)cd->m_nNumBoxes; i++, it += 0x1C) {
            auto* d = reinterpret_cast<uint8*>(cd->m_pBoxes) + i * 0x1C;
            memcpy(d, it, 24);            // minimo y maximo
            d[0x18] = it[0x18];           // material
            d[0x19] = it[0x19];           // flag
            d[0x1A] = it[0x1B];           // luz
        }
    } else {
        cd->m_pBoxes = nullptr;
    }

    // Vertices: float * 128 truncado (_ftol2) a int16
    const auto nVertices = (int32)ReadU32();
    if (nVertices > 0) {
        cd->m_pVertices = static_cast<decltype(cd->m_pVertices)>(CMemoryMgr::Malloc(nVertices * 6, 0));
        auto* d = reinterpret_cast<int16*>(cd->m_pVertices);
        for (int32 i = 0; i < nVertices * 3; i++, it += 4) {
            d[i] = (int16)(int64)(ReadF(it) * 128.0f);
        }
    } else {
        cd->m_pVertices = nullptr;
    }

    // Triangulos: TFace {a, b, c (uint32), material, flag, brillo, luz}
    cd->m_nNumTriangles = (uint16)ReadU32();
    if ((int16)cd->m_nNumTriangles > 0) {
        cd->m_pTriangles = (CColTriangle*)CMemoryMgr::Malloc((int16)cd->m_nNumTriangles * 8, 0);
        int32 i = 0;
        for (; i < (int16)cd->m_nNumTriangles; i++, it += 0x10) {
            auto* d = reinterpret_cast<uint8*>(cd->m_pTriangles) + i * 8;
            memcpy(d + 0, it + 0, 2);
            memcpy(d + 2, it + 4, 2);
            memcpy(d + 4, it + 8, 2);
            d[6] = it[0xC];               // material
            d[7] = it[0xF];               // luz
        }
        cd->m_nNumTriangles = (uint16)i;
    } else {
        cd->m_pTriangles = nullptr;
    }

    cd->bHasShadowInfo = false;
    cd->m_pShadowTriangles = nullptr;
    cd->m_pShadowVertices = nullptr;
    cm.m_pColData->m_nNumShadowVertices = 0;

    if (cd->m_nNumSpheres || cd->m_nNumBoxes || cd->m_nNumTriangles)
        cm.m_bHasCollisionVolumes = true;
}

// 0x537EE0
// Load collision V2 file from buffer. Just note that `data` is pointing to after `FileHeader`
// `dataSize` - <`size` parameter in the header> - 24 (That is, basically `<total size of file> - sizeof(FileHeader)`)
// `buffer`   - Pointer to data after `FileHeader` (So that's an offset of 32)
void CFileLoader::LoadCollisionModelVer2(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V2;

    auto& h = *reinterpret_cast<Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(Header);
    if (!dataSizeAfterHeader) {
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData), 0);
    cm.m_pColData = reinterpret_cast<CCollisionData*>(p); // Como el original: sin construir (el EXE solo escribe los campos de abajo)
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;

    cd->bUsesDisks = false;
    cd->bHasShadowInfo = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                  p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);

    cd->m_pTrianglePlanes = nullptr;

    // Como el original: m_nNumShadowTriangles (+0x20) no se escribe en V2
    cd->m_pShadowVertices = nullptr;
    cd->m_pShadowTriangles = nullptr;
    cd->m_nNumShadowVertices = 0;

    cm.m_bIsSingleColDataAlloc = true;

}

// 0x537CE0
// Same arguments as above function, but for V3
void CFileLoader::LoadCollisionModelVer3(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V3;

    auto& h = *reinterpret_cast<V3::Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(V3::Header);
    if (!dataSizeAfterHeader) {
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData), 0);
    cm.m_pColData = reinterpret_cast<CCollisionData*>(p); // Como el original: sin construir (el EXE solo escribe los campos de abajo)
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(V3::Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;
    cd->m_nNumShadowTriangles = h.nShdwFaces;

    cd->bUsesDisks = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compnensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(V3::Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);
    SetColDataPtr(cd->m_pShadowVertices, h.offShdwVerts);
    SetColDataPtr(cd->m_pShadowTriangles, h.offShdwFaces);

    cd->bHasShadowInfo = h.offShdwFaces && h.offShdwVerts && (int32)h.nShdwFaces > 0; // Como el original: jle (con signo)
    cd->m_nNumShadowVertices = cd->bHasShadowInfo ? h.GetNoOfShdwVerts(cd) : 0;

    cd->m_pTrianglePlanes = nullptr;

    cm.m_bIsSingleColDataAlloc = true;

}

// 0x537AE0
// Same shit different packaing of V3, but using V4 header
void CFileLoader::LoadCollisionModelVer4(uint8* buffer, uint32 dataSize, CColModel& cm, const char* modelName) {
    using namespace ColHelpers;
    using namespace ColHelpers::V4;

    auto& h = *reinterpret_cast<V4::Header*>(buffer);
    cm.m_boundBox = h.bounds.box;
    cm.m_boundSphere = h.bounds.sphere;
    cm.m_bHasCollisionVolumes = !h.IsEmpty();

    auto dataSizeAfterHeader = dataSize - sizeof(V4::Header);
    if (!dataSizeAfterHeader) {
        return; // No data present, other than the header
    }

    // Here's the meat. We allocate some memory to hold both the file's contents and the CCollisionData struct.
    // So, memory layout is as follows:
    //                [              DATA FROM FILE COPIED                  ]
    // CCollisionData | Spheres | Boxes | Suspension Lines | Vertices | Faces

    auto p = (uint8*)CMemoryMgr::Malloc(dataSizeAfterHeader + sizeof(CCollisionData), 0);
    cm.m_pColData = reinterpret_cast<CCollisionData*>(p); // Como el original: sin construir (el EXE solo escribe los campos de abajo)
    memcpy(p + sizeof(CCollisionData), buffer + sizeof(V4::Header), dataSizeAfterHeader); // Copy actual data into allocated memory after CCollisionData

    auto cd = cm.m_pColData;
    cd->m_nNumSpheres = h.nSpheres;
    cd->m_nNumBoxes = h.nBoxes;
    cd->m_nNumLines = (uint8)h.nLines;
    cd->m_nNumTriangles = h.nFaces;
    cd->m_nNumShadowTriangles = h.nShdwFaces;

    cd->bUsesDisks = false;
    cd->bHasFaceGroups = h.HasFaceGroups();

    // Set given field in `CCollisionData` based on offset in file.
    // If it's 0 then nullptr, otherwise a pointer to where the data is in memory.
    const auto SetColDataPtr = [&]<typename T>(T& colDataPtr, auto fileOffset) {
        // Return pointer for offset in allocated memory (relative to where it was in the file)
        const auto GetDataPtr = [&]() {
            return reinterpret_cast<T>(
                p
                + sizeof(CCollisionData)                // Must offset by this (See memory layout above)
                + fileOffset
                + sizeof(FileHeader::FileInfo::fourcc)  // All offsets are relative to this, but since it is already included in the header's size, so we gotta compnensate for it.
                - sizeof(FileHeader)                    // Offset includes these headers, but we haven't copied them into our memory
                - sizeof(V4::Header)
            );
        };
        colDataPtr = fileOffset ? GetDataPtr() : nullptr;
    };

    SetColDataPtr(cd->m_pSpheres, h.offSpheres);
    SetColDataPtr(cd->m_pBoxes, h.offBoxes);
    SetColDataPtr(cd->m_pLines, h.offLines);
    SetColDataPtr(cd->m_pVertices, h.offVerts);
    SetColDataPtr(cd->m_pTriangles, h.offFaces);
    SetColDataPtr(cd->m_pShadowVertices, h.offShdwVerts);
    SetColDataPtr(cd->m_pShadowTriangles, h.offShdwFaces);

    cd->bHasShadowInfo = h.offShdwFaces && h.offShdwVerts && (int32)h.nShdwFaces > 0; // Como el original: jle (con signo)
    cd->m_nNumShadowVertices = cd->bHasShadowInfo ? h.GetNoOfShdwVerts(cd) : 0;

    cd->m_pTrianglePlanes = nullptr;

    cm.m_bIsSingleColDataAlloc = true;

}

// 0x5B3C60
int32 CFileLoader::LoadObject(const char* line) {
    int32  modelId{ MODEL_INVALID};
    char   modelName[24];
    char   texName[24];
    float  fDrawDist;
    uint32 nFlags;

    auto iNumRead = sscanf(line, "%d %s %s %f %d", &modelId, modelName, texName, &fDrawDist, &nFlags);
    if (iNumRead != 5 || fDrawDist < 4.0f)
    {
        int32 objType;
        float fDrawDist2_unused, fDrawDist3_unused;
        iNumRead = sscanf(line, "%d %s %s %d", &modelId, modelName, texName, &objType);
        if (iNumRead != 4)
            return -1;

        switch (objType)
        {
        case 1:
            sscanf(line, "%d %s %s %d %f %d", &modelId, modelName, texName, &objType, &fDrawDist, &nFlags);
            break;
        case 2:
            sscanf(line, "%d %s %s %d %f %f %d", &modelId, modelName, texName, &objType, &fDrawDist, &fDrawDist2_unused, &nFlags);
            break;
        case 3:
            sscanf(line, "%d %s %s %d %f %f %f %d", &modelId, modelName, texName, &objType, &fDrawDist, &fDrawDist2_unused, &fDrawDist3_unused, &nFlags);
            break;
        }
    }

    sItemDefinitionFlags flags(nFlags);
    const auto mi = flags.bIsDamageable ? CModelInfo::AddDamageAtomicModel(modelId) : CModelInfo::AddAtomicModel(modelId);
    mi->m_fDrawDistance = fDrawDist;
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    SetAtomicModelInfoFlags(mi, nFlags);

    return modelId;
}

// 0x5B7670
void CFileLoader::Load2dEffect(const char* line) {
    return plugin::Call<0x5B7670, const char*>(line);

    // todo:
    auto modelId{ MODEL_INVALID };
    CVector pos{};
    int32 type;
    sscanf(line, "%d %f %f %f %d", &modelId, &pos.x, &pos.y, &pos.z, &type);

    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    auto& effect = CModelInfo::Get2dEffectStore()->AddItem();
    CModelInfo::GetModelInfo(modelId)->Add2dEffect(&effect);
    effect.m_Pos = pos;
    effect.m_Type = *reinterpret_cast<e2dEffectType*>(type);

    switch (type) {
    case EFFECT_LIGHT:
        break;
    case EFFECT_PARTICLE:
        break;
    case EFFECT_ATTRACTOR:
        break;
    case EFFECT_INTERIOR:
        break;
    case EFFECT_ENEX:
        break;
    case EFFECT_ROADSIGN:
        break;
    case EFFECT_TRIGGER_POINT:
        break;
    case EFFECT_COVER_POINT:
        break;
    case EFFECT_ESCALATOR:
        break;
    default:
        break;
    }
    CTxdStore::PopCurrentTxd();
}

// Como el original: acos del CRT del EXE (_CIacos, 0x822380: x87 con la precision del EXE) sobre el float
static float ExeCIacos(float x) {
    float result;
    __asm {
        fld   x
        mov   eax, 0x822380
        call  eax
        fstp  result
    }
    return result;
}

// 0x538090
CEntity* CFileLoader::LoadObjectInstance(CFileObjectInstance* objInstance, const char* modelName) {
    auto* mi = CModelInfo::GetModelInfo(objInstance->m_nModelId);
    if (!mi)
        return nullptr;

    CEntity* newEntity = nullptr;
    if (mi->m_nObjectInfoIndex == -1)
    {
        if (mi->GetModelType() == ModelInfoType::MODEL_INFO_CLUMP && mi->bHasAnimBlend)
            newEntity = new CAnimatedBuilding();
        else
            newEntity = new CBuilding();

        newEntity->SetModelIndexNoCreate(objInstance->m_nModelId);
        if (mi->bDontCastShadowsOn)
            newEntity->m_bDontCastShadowsOn = true;

        if (mi->m_fDrawDistance < 2.0F)
            newEntity->SetIsVisible(false);
    }
    else
    {
        newEntity = new CDummyObject();
        newEntity->SetModelIndexNoCreate(objInstance->m_nModelId);
        if (CGlass::IsObjectGlass(newEntity) && !CModelInfo::GetModelInfo(newEntity->m_nModelIndex)->IsGlassType2()) {
            newEntity->SetIsVisible(false);
        }
    }

    const auto& rot = objInstance->m_qRotation.imag;
    if (std::fabs(rot.x) > 0.05F || std::fabs(rot.y) > 0.05F ||
        (
            objInstance->m_bDontStream &&
            rot.x != 0.0f && rot.y != 0.0f
        )
    ) {
        objInstance->m_qRotation.imag = -rot;
        newEntity->AllocateStaticMatrix();
        const CQuaternion q = objInstance->m_qRotation;
        newEntity->GetMatrix().SetRotate(q);
    }
    else
    {
        // Como el original: acos(w) * 2 si z < 0 y * -2 en otro caso (tambien NaN)
        const auto a = ExeCIacos(objInstance->m_qRotation.real);
        const auto fHeading = rot.z < 0.0f ? a * 2.0f : a * -2.0f;
        newEntity->SetHeading(fHeading);
    }

    newEntity->SetPosn(objInstance->m_vecPosition);

    newEntity->m_bUnderwater        |= objInstance->m_bUnderwater;
    newEntity->m_bTunnel            |= objInstance->m_bTunnel;
    newEntity->m_bTunnelTransition  |= objInstance->m_bTunnelTransition;
    newEntity->SetIsUnimportantStream(objInstance->m_bRedundantStream);
    newEntity->SetAreaCode(static_cast<eAreaCodes>(objInstance->m_nAreaCode));
    newEntity->SetLodIndex(objInstance->m_nLodInstanceIndex);

    if (objInstance->m_nModelId == ModelIndices::MI_TRAINCROSSING)
    {
        newEntity->GetMatrix();
        newEntity->AllocateStaticMatrix();
        CObject::SetMatrixForTrainCrossing(&newEntity->GetMatrix(), std::bit_cast<float>(0x3FACE9CCu)); // Como el original: constante del EXE
    }

    auto* cm = CModelInfo::GetModelInfo(newEntity->m_nModelIndex)->GetColModel();
    if (cm) {
        if (cm->m_bHasCollisionVolumes)
        {
            if (cm->m_nColSlot)
            {
                const auto rect = newEntity->GetBoundRect();
                CColStore::GetInSlot(cm->m_nColSlot)->m_Area.Restrict(rect);
            }
        }
        else
        {
            newEntity->SetUsesCollision(false);
        }

        if (mi->GetColModel()->GetBoundingBox().m_vecMin.z + newEntity->GetPosition().z < 0.0f)
            newEntity->m_bUnderwater = true;
    }

    return newEntity;
}

// 0x538690
CEntity* CFileLoader::LoadObjectInstance(const char* line) {
    char modelName[24];
    CFileObjectInstance instance;
    sscanf(
        line,
        "%d %s %d %f %f %f %f %f %f %f %d",
        &instance.m_nModelId,
        modelName,
        &instance.m_nInstanceType,
        &instance.m_vecPosition.x,
        &instance.m_vecPosition.y,
        &instance.m_vecPosition.z,
        &instance.m_qRotation.imag.x,
        &instance.m_qRotation.imag.y,
        &instance.m_qRotation.imag.z,
        &instance.m_qRotation.real,
        &instance.m_nLodInstanceIndex
    );
    return LoadObjectInstance(&instance, modelName);
}

// 0x5B4B40
void CFileLoader::LoadCullZone(const char* line) {
    CVector center{}, mirrorDirection{};
    float unknown, length, bottom, width, unknown2, zTop, cm;
    int32 flags, flags2 = 0;

    if (sscanf(
        line,
        "%f %f %f %f %f %f %f %f %f %d %f %f %f %f",
        &center.x, &center.y, &center.z,
        &unknown,
        &length,
        &bottom,
        &width,
        &unknown2,
        &zTop,
        &flags,
        &mirrorDirection.x, &mirrorDirection.y, &mirrorDirection.z,
        &cm
    ) == 14) {
        CCullZones::AddMirrorAttributeZone(
            center,
            unknown,
            length,
            bottom,
            width,
            unknown2,
            zTop,
            static_cast<eZoneAttributes>(flags),
            cm,
            mirrorDirection.x, mirrorDirection.y, mirrorDirection.z
        );
        return;
    }

    sscanf(
        line,
        "%f %f %f %f %f %f %f %f %f %d %d",
        &center.x, &center.y, &center.z,
        &unknown,
        &length,
        &bottom,
        &width,
        &unknown2,
        &zTop,
        &flags,
        &flags2
    );
    CCullZones::AddCullZone(center, unknown, length, bottom, width, unknown2, zTop, flags);
}

// IPL -> ENEX
// 0x5B8030
void CFileLoader::LoadEntryExit(const char* line) {
    uint32 numOfPeds = 2;
    uint32 timeOn = 0, timeOff = 24;
    CVector enter{}, exit{};
    CVector2D range;
    float entranceAngle;
    float unused;
    float exitAngle;
    int32 area;
    char interiorName[64]{};
    uint32 skyColor;
    uint32 flags;

    sscanf(
        line,
        "%f %f %f %f %f %f %f %f %f %f %f %d %d %s %d %d %d %d",
        &enter.x, &enter.y, &enter.z,
        &entranceAngle,
        &range.x, &range.y,
        &unused,
        &exit.x, &exit.y, &exit.z,
        &exitAngle,
        &area,
        &flags,
        interiorName,
        &skyColor,
        &numOfPeds,
        &timeOn,
        &timeOff
    );

    auto name = strrchr(interiorName, '"');
    if (name) {
        *name = 0;
        name = &interiorName[1];
    }

    const auto enexPoolIdx = CEntryExitManager::AddOne(
        enter.x, enter.y, enter.z,
        entranceAngle,
        range.x, range.y,
        unused,
        exit.x, exit.y, exit.z,
        exitAngle,
        static_cast<eAreaCodes>(area),
        (CEntryExit::eFlags)flags,
        skyColor,
        timeOn,
        timeOff,
        numOfPeds,
        name
    );
    auto enex = CEntryExitManager::GetInSlot(enexPoolIdx);

    enum Flags {
        UNKNOWN_INTERIOR      = 1 << 0,
        UNKNOWN_PAIRING       = 1 << 1,
        CREATE_LINKED_PAIR    = 1 << 2,
        REWARD_INTERIOR       = 1 << 3,
        USED_REWARD_ENTRANCE  = 1 << 4,
        CARS_AND_AIRCRAFT     = 1 << 5,
        BIKES_AND_MOTORCYCLES = 1 << 6,
        DISABLE_ONFOOT        = 1 << 7,
    };

    if (flags & UNKNOWN_INTERIOR)
        enex->bUnknownInterior = true;

    if (flags & UNKNOWN_PAIRING)
        enex->bUnknownPairing = true;

    if (flags & CREATE_LINKED_PAIR)
        enex->bCreateLinkedPair = true;

    if (flags & REWARD_INTERIOR)
        enex->bRewardInterior = true;

    if (flags & USED_REWARD_ENTRANCE)
        enex->bUsedRewardEntrance = true;

    if (flags & CARS_AND_AIRCRAFT)
        enex->bCarsAndAircraft = true;

    if (flags & BIKES_AND_MOTORCYCLES)
        enex->bBikesAndMotorcycles = true;

    if (flags & DISABLE_ONFOOT)
        enex->bDisableOnFoot = true;
}

// IPL -> GRGE
// 0x5B4530
void CFileLoader::LoadGarage(const char* line) {
    uint32 flags;
    uint32 type;
    CVector p1{}, p2{};
    float frontX, frontY;
    char name[128];

    if (sscanf(
        line,
        "%f %f %f %f %f %f %f %f %d %d %s",
        &p1.x, &p1.y, &p1.z,
        &frontX,
        &frontY,
        &p2.x, &p2.y, &p2.z,
        &flags,
        &type,
        name
    ) == 11) {
        CGarages::AddOne(p1.x, p1.y, p1.z, frontX, frontY, p2.x, p2.y, p2.z, (eGarageType)type, 0, name, flags);
    }
}

// 0x5B9030
// Como el original: comparaciones strncmp en el orden del EXE; TEXDICTION muestra la pantalla de carga en
// linea y copia las texturas al diccionario actual; IMG no muestra pantalla de carga; COLFILE la muestra
// con la ruta que empieza tras "COLFILE 0 ".
void CFileLoader::LoadLevel(const char* levelFileName) {
    ZoneScoped;

    auto txd = RwTexDictionaryGetCurrent();
    if (!txd) {
        txd = RwTexDictionaryCreate();
        RwTexDictionarySetCurrent(txd);
    }

    bool hasLoadedAnyIPLs{};
    char pathBuffer[MAX_PATH];

    const auto f = CFileMgr::OpenFile(levelFileName, "r");
    for (auto l = LoadLine(f); l; l = LoadLine(f)) {
        if (*l == '#')
            continue; // Skip comment

        if (!strncmp("EXIT", l, 4))
            break; // Done

        if (!strncmp("TEXDICTION", l, 10)) {
            strcpy(pathBuffer, l + 11);
            sprintf(gString, "Loading %s", GetFilename(pathBuffer));
            LoadingScreen("Loading the Game", gString, nullptr);

            const auto texDict = LoadTexDictionary(pathBuffer);
            RwTexDictionaryForAllTextures(texDict, AddTextureCB, txd);
            RwTexDictionaryDestroy(texDict);
        } else if (!strncmp("IMG", l, 3)) {
            const auto path = l + 4;
            if (memcmp(path, "MODELS\\GTA_INT.IMG", 19) != 0) {
                CStreaming::AddImageToList(path, true);
            }
        } else if (!strncmp("COLFILE", l, 7)) {
            const auto path = l + 10;
            LoadingScreenLoadingFile(path);
            LoadCollisionFile(path, 0);
        } else if (!strncmp("MODELFILE", l, 9)) {
            const auto path = l + 10;
            LoadingScreenLoadingFile(path);
            LoadAtomicFile(path);
        } else if (!strncmp("HIERFILE", l, 8)) {
            const auto path = l + 9;
            LoadingScreenLoadingFile(path);
            LoadClumpFile(path);
        } else if (!strncmp("IDE", l, 3)) {
            const auto path = l + 4;
            LoadingScreenLoadingFile(path);
            LoadObjectTypes(path);
        } else if (!strncmp("IPL", l, 3)) {
            if (!hasLoadedAnyIPLs) {
                MatchAllModelStrings();
            }

            // La ruta se copia porque las funciones de abajo pueden volver a llamar a LoadLine
            strcpy(pathBuffer, l + 4);

            if (!hasLoadedAnyIPLs) {
                LoadingScreenLoadingFile("Object Data");
                CObjectData::Initialise("DATA\\OBJECT.DAT");

                LoadingScreenLoadingFile("Setup vehicle info data");
                CVehicleModelInfo::SetupCommonData();

                LoadingScreenLoadingFile("Streaming Init");
                CStreaming::Init2();

                CLoadingScreen::NewChunkLoaded();

                LoadingScreenLoadingFile("Collision");
                CColStore::LoadAllBoundingBoxes();

                for (auto mi : CModelInfo::ms_modelInfoPtrs) {
                    if (mi) {
                        mi->ConvertAnimFileIndex();
                    }
                }

                hasLoadedAnyIPLs = true;
            }

            LoadingScreenLoadingFile(pathBuffer);
            LoadScene(pathBuffer);
        }
        // SPLASH: comparado pero sin efecto
    }
    CFileMgr::CloseFile(f);

    RwTexDictionarySetCurrent(txd);

    if (hasLoadedAnyIPLs) {
        CIplStore::LoadAllRemainingIpls();
        CColStore::BoundingBoxesPostProcess();
        CTrain::InitTrains();
        CColStore::RemoveAllCollision();
    }
}

// IPL -> OCCL
// 0x5B4C80
void CFileLoader::LoadOcclusionVolume(const char* line, const char* filename) {
    int32 flags = 0;
    float centerX, centerY, bottomZ, width, length, height, rotZ, rotY = 0.f, rotX = 0.f;
    sscanf(line, "%f %f %f %f %f %f %f %f %f %d ", &centerX, &centerY, &bottomZ, &width, &length, &height, &rotZ, &rotY, &rotX, &flags); // yes, it's z, y, x
    // Como el original: solo se comprueban los caracteres 'i','n','t' en las posiciones len-7..len-5
    const auto len = strlen(filename);
    const bool isInterior = filename[len - 7] == 'i' && filename[len - 6] == 'n' && filename[len - 5] == 't';
    COcclusion::AddOne(
        centerX, centerY, height * 0.5F + bottomZ,
        width, length, height,
        rotZ, rotY, rotX,
        flags,
        isInterior
    );
}

// 0x5B41C0
int32 CFileLoader::LoadPathHeader(const char* line, int32& outPathType) {
    int32 id;
    char unused[84]; // Como el original: "%d %d %s" sobre un buffer de la pila
    sscanf(line, "%d %d %s", &outPathType, &id, unused);
    return id;
}

// PEDS
// 0x5B7420
// Como el original: buffers de la pila del EXE, "%s" sin limite para el fichero de animaciones y el grupo de
// animaciones buscado en linea con strcmp (sin coincidencia queda el numero de grupos leido al empezar).
int32 CFileLoader::LoadPedObject(const char* line) {
    auto   modelId{ MODEL_INVALID };
    int32  radio1{}, radio2{};
    uint32 flags{};
    int32  carsCanDriveMask{};

    char animFile[16]{};
    char modelName[24]{};
    char pedVoiceType[20]{};
    char statName[24]{};
    char animGroup[24]{};
    char pedType[24]{};
    char texName[24]{};
    char voiceMin[60]{};
    char voiceMax[60]{};

    const auto numGroups = (int32)StaticRef<uint32>(0xB4EA28) /* CAnimManager::ms_numAnimAssocDefinitions */;

    sscanf(
        line,
        "%d %s %s %s %s %s %x %x %s %d %d %s %s %s",
        &modelId,
        modelName,
        texName,
        pedType,
        statName,
        animGroup,
        &carsCanDriveMask,
        &flags,
        animFile,
        &radio1,
        &radio2,
        pedVoiceType,
        voiceMin,
        voiceMax
    );

    const auto mi = CModelInfo::AddPedModel(modelId);

    mi->m_nKey = CKeyGen::GetUppercaseKey(modelName);
    mi->SetTexDictionary(texName);
    mi->SetAnimFile(animFile);
    mi->SetColModel(&CTempColModels::ms_colModelPed1, false);
    mi->m_nPedType  = CPedType::FindPedType(pedType);
    mi->m_nStatType = CPedStats::GetPedStatType(statName);

    auto animType = numGroups;
    for (int32 i = 0; i < (int32)StaticRef<uint32>(0xB4EA28) /* CAnimManager::ms_numAnimAssocDefinitions */; i++) {
        if (!strcmp(animGroup, CAnimManager::GetAnimGroupName((AssocGroupId)i))) {
            animType = i;
            break;
        }
    }
    mi->m_nAnimType         = (AssocGroupId)animType;
    mi->m_nCarsCanDriveMask = carsCanDriveMask;
    mi->m_nPedFlags         = flags;
    mi->m_nRadio2           = (eRadioID)(radio2 + 1);
    mi->m_nRadio1           = (eRadioID)(radio1 + 1);
    mi->m_nRace             = CPopulation::FindPedRaceFromName(modelName);
    mi->m_nPedAudioType     = CAEPedSpeechAudioEntity::GetAudioPedType(pedVoiceType);
    mi->m_nVoiceMin         = CAEPedSpeechAudioEntity::GetVoice(voiceMin, mi->m_nPedAudioType);
    mi->m_nVoiceMax         = CAEPedSpeechAudioEntity::GetVoice(voiceMax, mi->m_nPedAudioType);
    mi->m_nVoiceId          = mi->m_nVoiceMin;

    return modelId;
}

// useless
// 0x5B41F0
void CFileLoader::LoadPedPathNode(const char* line, int32 objModelIndex, int32 pathEntryIndex) {
    // This function loads the file, reads some data
    // and calls `CPathFind::StoreDetachedNodeInfoPed` or `CPathFind::StoreNodeInfoPed`
    // but both functions are NOPs, so this function basically doesn't do anything useful.
}

// 0x5B47B0
// https://gta.fandom.com/wiki/PICK
void CFileLoader::LoadPickup(const char* line) {
    CVector pos{};
    int32   weaponType{};

    if (sscanf(line, "%d %f %f %f", &weaponType, &pos.x, &pos.y, &pos.z) != 4)
        return;

    const auto GetModel = [weaponType] {
        switch (weaponType) {
        case WEAPON_KNIFE:
            return MODEL_BRASSKNUCKLE;

        case WEAPON_BASEBALLBAT:
            return MODEL_NITESTICK;

        case WEAPON_SHOVEL:
            return MODEL_KNIFECUR;

        case WEAPON_CHAINSAW:
            return MODEL_GOLFCLUB;

        case WEAPON_DILDO1:
            return MODEL_BAT;

        case WEAPON_DILDO2:
            return MODEL_SHOVEL;

        case WEAPON_VIBE1:
            return MODEL_POOLCUE;

        case WEAPON_VIBE2:
            return MODEL_KATANA;

        case WEAPON_FLOWERS:
            return MODEL_CHNSAW;

        case WEAPON_CANE:
            return MODEL_MOLOTOV;

        case WEAPON_GRENADE:
            return MODEL_GRENADE;

        case WEAPON_TEARGAS:
            return MODEL_SATCHEL;

        case WEAPON_MOLOTOV:
            return MODEL_COLT45;

        case WEAPON_ROCKET:
            return MODEL_SILENCED;

        case WEAPON_ROCKET_HS:
            return MODEL_DESERT_EAGLE;

        case WEAPON_FREEFALL_BOMB:
            return MODEL_CHROMEGUN;

        case WEAPON_PISTOL:
        case WEAPON_INFRARED:
            return MODEL_SHOTGSPA;

        case WEAPON_PISTOL_SILENCED:
            return MODEL_TEC9;

        case WEAPON_DESERT_EAGLE:
            return MODEL_MICRO_UZI;

        case WEAPON_SHOTGUN:
            return MODEL_MP5LNG;

        case WEAPON_SAWNOFF_SHOTGUN:
            return MODEL_AK47;

        case WEAPON_SPAS12_SHOTGUN:
            return MODEL_M4;

        case WEAPON_MICRO_UZI:
            return MODEL_CUNTGUN;

        case WEAPON_MP5:
            return MODEL_SNIPER;

        case WEAPON_M4:
            return MODEL_FLAME;

        case WEAPON_TEC9:
        case WEAPON_NIGHTVISION:
            return MODEL_MINIGUN;

        case WEAPON_COUNTRYRIFLE:
            return MODEL_GUN_DILDO1;

        case WEAPON_SNIPERRIFLE:
            return MODEL_GUN_DILDO2;

        case WEAPON_RLAUNCHER:
            return MODEL_GUN_VIBE1;

        case WEAPON_RLAUNCHER_HS:
            return MODEL_GUN_VIBE2;

        case WEAPON_FLAMETHROWER:
            return MODEL_FLOWERA;

        case WEAPON_MINIGUN:
            return MODEL_GUN_CANE;

        case WEAPON_REMOTE_SATCHEL_CHARGE:
            return MODEL_GUN_BOXWEE;

        case WEAPON_DETONATOR:
            return MODEL_GUN_BOXBIG;

        case WEAPON_SPRAYCAN:
            return MODEL_CELLPHONE;

        case WEAPON_CAMERA:
            return MODEL_TEARGAS;

        case WEAPON_PARACHUTE:
            return MODEL_ROCKETLA;

        case WEAPON_LAST_WEAPON:
            return MODEL_HEATSEEK;

        case WEAPON_ARMOUR:
            return MODEL_BOMB;

        case WEAPON_RAMMEDBYCAR:
            return MODEL_SPRAYCAN;

        case WEAPON_RUNOVERBYCAR:
            return MODEL_FIRE_EX;

        case WEAPON_EXPLOSION:
            return MODEL_CAMERA;

        case WEAPON_UZI_DRIVEBY:
            return MODEL_NVGOGGLES;

        case WEAPON_DROWNING:
            return MODEL_IRGOGGLES;

        case WEAPON_FALL:
            return MODEL_JETPACK;

        case WEAPON_UNIDENTIFIED:
            return MODEL_GUN_PARA;

        default:
            return MODEL_INVALID;
        }
    };

    if (const auto model = GetModel(); model != MODEL_INVALID) {
        CPickups::GenerateNewOne(pos, model, PICKUP_ON_STREET, 0, 0, false, nullptr);
    }
}

// 0x5B45D0
void CFileLoader::LoadStuntJump(const char* line) {
    CBoundingBox start, target;
    CVector cameraPosn{};
    int32 reward;

    int32 iNumRead = sscanf(
        line,
        "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d",
        &start.m_vecMin.x,
        &start.m_vecMin.y,
        &start.m_vecMin.z,
        &start.m_vecMax.x,
        &start.m_vecMax.y,
        &start.m_vecMax.z,

        &target.m_vecMin.x,
        &target.m_vecMin.y,
        &target.m_vecMin.z,
        &target.m_vecMax.x,
        &target.m_vecMax.y,
        &target.m_vecMax.z,

        &cameraPosn.x,
        &cameraPosn.y,
        &cameraPosn.z,
        &reward
    );
    if (iNumRead == 16) {
         CStuntJumpManager::AddOne(start, target, cameraPosn, reward);
    }
}

// 0x5B75E0
int32 CFileLoader::LoadTXDParent(const char* line) {
    char name[32], parentName[32];
    sscanf(line, "%s %s", name, parentName);
    auto txdSlot = CTxdStore::FindOrAddTxdSlot(name);
    auto parentSlot = CTxdStore::FindOrAddTxdSlot(parentName);
    CTxdStore::ms_pTxdPool->GetAt(txdSlot)->m_wParentIndex = parentSlot;
    return parentSlot;
}

// IPL -> TCYC
// 0x5B81D0
void CFileLoader::LoadTimeCyclesModifier(const char* line) {
    CVector vec1{}, vec2{};
    int32 farClip;
    int32 extraColor;
    float extraColorIntensity;
    float falloffDist = 100.0f;
    float unused = 1.0f;
    float lodDistMult = 1.0f;

    auto iNumRead = sscanf(
        line,
        "%f %f %f %f %f %f %d %d %f %f %f %f",
        &vec1.x,
        &vec1.y,
        &vec1.z,
        &vec2.x,
        &vec2.y,
        &vec2.z,
        &farClip,
        &extraColor,
        &extraColorIntensity,
        &falloffDist,
        &unused,
        &lodDistMult
    );
    if (iNumRead < 12)
        lodDistMult = unused;

    CBox box{};
    box.Set(vec1, vec2);
    return CTimeCycle::AddOne(box, farClip, extraColor, extraColorIntensity, falloffDist, lodDistMult);
}

// 0x5B3DE0
int32 CFileLoader::LoadTimeObject(const char* line) {
    auto  modelId{ MODEL_INVALID };
    char  modelName[24];
    char  texName[24];
    float drawDistance[3]{};
    int32 flags;
    int32 timeOn;
    int32 timeOff;

    int32 numValuesRead = sscanf(line, "%d %s %s %f %d %d %d", &modelId, modelName, texName, &drawDistance[0], &flags, &timeOn, &timeOff);

    if (numValuesRead != 7 || drawDistance[0] < 4.0) {
        int32 numObjs;

        if (sscanf(line, "%d %s %s %d", &modelId, modelName, texName, &numObjs) != 4)
            return -1;

        switch (numObjs) {
        case 1:
            sscanf(line, "%d %s %s %d %f %d %d %d", &modelId, modelName, texName, &numObjs, &drawDistance[0], &flags, &timeOn, &timeOff);
            break;
        case 2:
            sscanf(line, "%d %s %s %d %f %f %d %d %d", &modelId, modelName, texName, &numObjs, &drawDistance[0], &drawDistance[1], &flags, &timeOn, &timeOff);
            break;
        case 3:
            sscanf(line, "%d %s %s %d %f %f %f %d %d %d", &modelId, modelName, texName, &numObjs, &drawDistance[0], &drawDistance[1], &drawDistance[2], &flags, &timeOn, &timeOff);
            break;
        default: // Como el original: sin relectura
            break;
        }
    }

    CTimeModelInfo* mi = CModelInfo::AddTimeModel(modelId);
    CTimeInfo* timeInfo = mi->GetTimeInfo(); // Como el original: justo despues de crear el model info
    mi->m_fDrawDistance = drawDistance[0];
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);

    timeInfo->SetTimes(timeOn, timeOff);

    SetAtomicModelInfoFlags(mi, flags);

    CTimeInfo* otherTimeInfo = timeInfo->FindOtherTimeModel(modelName);
    if (otherTimeInfo)
        otherTimeInfo->SetOtherTimeModel(modelId);

    return modelId;
}

// 0x5B6F30
// Como el original: el slot "vehicle" se busca/crea antes de leer la linea; los tipos y clases se comparan
// con strcmp (sin coincidencia no se escribe el campo); "f_heli" es un helicoptero sin datos de ruedas;
// el nombre de juego cambia '_' por ' ' desde el segundo caracter y se copia con strncpy(8); la frecuencia
// se escribe salvo en la clase "ignore".
int32 CFileLoader::LoadVehicleObject(const char* line) {
    auto               modelId{ MODEL_INVALID };
    char               modelName[24]{};
    char               texName[24]{};
    char               type[12]{};
    char               handlingName[16]{};
    char               gameName[32]{};
    char               anims[16]{};
    char               vehCls[16]{};
    uint32             frq{}, flags{};
    tVehicleCompsUnion vehComps{};
    int32              misc{ -1 };                                    // `m_fBikeSteerAngle` if model type is BMX/Bike, otherwise `m_nWheelModelIndex`
    float              wheelSizeFront{ 0.7f }, wheelSizeRear{ 0.7f }; // NOTSA/BUG: Properly default initialize this value (See https://cookieplmonster.github.io/2025/04/23/gta-san-andreas-win11-24h2-bug/)
    int32              wheelUpgradeCls{ -1 };

    int32 vehicleTxd = CTxdStore::FindTxdSlot("vehicle");
    if (vehicleTxd == -1) {
        vehicleTxd = CTxdStore::AddTxdSlot("vehicle");
    }

    sscanf(line, "%d %s %s %s %s %s %s %s %d %d %x %d %f %f %d",
        &modelId,                  // 1
        modelName,                 // 2
        texName,                   // 3
        type,                      // 4
        handlingName,              // 5
        gameName,                  // 6
        anims,                     // 7
        vehCls,                    // 8
        &frq,                      // 9
        &flags,                    // 10
        &vehComps.m_nComps,        // 11
        &misc,                     // 12 [optional]
        &wheelSizeFront,           // 13 [optional]
        &wheelSizeRear,            // 14 [optional]
        &wheelUpgradeCls           // 15 [optional]
    );

    auto mi = CModelInfo::AddVehicleModel(modelId);
    mi->SetModelName(modelName);
    mi->SetTexDictionary(texName);
    CTxdStore::ms_pTxdPool->GetAt(mi->m_nTxdIndex)->m_wParentIndex = (int16)vehicleTxd;
    mi->SetAnimFile(anims);

    // Replace `_` with ` ` (space), como el original desde el segundo caracter
    if (gameName[0]) {
        for (char* it = gameName + 1; *it; it++) {
            if (*it == '_') {
                *it = ' ';
            }
        }
    }
    strncpy(mi->m_szGameName, gameName, 8);

    mi->m_nFlags = (uint8)flags;
    mi->m_extraComps = vehComps;

    const auto SetWheels = [&] {
        mi->m_nWheelModelIndex = (int16)misc;
        mi->m_fWheelSizeFront  = wheelSizeFront;
        mi->m_fWheelSizeRear   = wheelSizeRear;
    };
    if (!strcmp(type, "car")) {
        mi->m_nVehicleType = VEHICLE_TYPE_AUTOMOBILE;
        SetWheels();
    } else if (!strcmp(type, "mtruck")) {
        mi->m_nVehicleType = VEHICLE_TYPE_MTRUCK;
        SetWheels();
    } else if (!strcmp(type, "quad")) {
        mi->m_nVehicleType = VEHICLE_TYPE_QUAD;
        SetWheels();
    } else if (!strcmp(type, "heli")) {
        mi->m_nVehicleType = VEHICLE_TYPE_HELI;
        SetWheels();
    } else if (!strcmp(type, "plane")) {
        mi->m_nVehicleType = VEHICLE_TYPE_PLANE;
        SetWheels();
    } else if (!strcmp(type, "boat")) {
        mi->m_nVehicleType = VEHICLE_TYPE_BOAT;
    } else if (!strcmp(type, "train")) {
        mi->m_nVehicleType = VEHICLE_TYPE_TRAIN;
    } else if (!strcmp(type, "f_heli")) {
        mi->m_nVehicleType = VEHICLE_TYPE_HELI; // Como el original: f_heli se registra como helicoptero
    } else if (!strcmp(type, "f_plane")) {
        mi->m_nWheelModelIndex = (int16)misc;
        mi->m_fWheelSizeFront  = 1.0f;
        mi->m_fWheelSizeRear   = 1.0f;
        mi->m_nVehicleType     = VEHICLE_TYPE_FPLANE;
    } else if (!strcmp(type, "bike") || !strcmp(type, "bmx")) {
        mi->m_nVehicleType     = type[1] == 'i' ? VEHICLE_TYPE_BIKE : VEHICLE_TYPE_BMX;
        mi->m_fBikeSteerAngle  = (float)misc;
        mi->m_fWheelSizeFront  = wheelSizeFront;
        mi->m_fWheelSizeRear   = wheelSizeRear;
    } else if (!strcmp(type, "trailer")) {
        mi->m_nVehicleType = VEHICLE_TYPE_TRAILER;
        SetWheels();
    }

    mi->SetHandlingId(handlingName);
    mi->m_nWheelUpgradeClass = (uint8)wheelUpgradeCls;

    static constexpr const char* classes[]{ "normal", "poorfamily", "richfamily", "executive", "worker", "big", "taxi",
                                            "moped", "motorbike", "leisureboat", "workerboat", "bicycle" };
    if (!strcmp(vehCls, "ignore")) {
        mi->m_nVehicleClass = VEHICLE_CLASS_IGNORE;
        return modelId;
    }
    for (int32 i = 0; i < (int32)std::size(classes); i++) {
        if (!strcmp(vehCls, classes[i])) {
            mi->m_nVehicleClass = (eVehicleClass)i;
            break;
        }
    }
    mi->m_nFrq = (uint16)frq;

    return modelId;
}

// 0x5B3FB0
int32 CFileLoader::LoadWeaponObject(const char* line) {
    auto objId{ MODEL_INVALID };
    char modelName[24];
    char texName[24];
    char animName[16];
    int32 weaponType;
    float drawDist;

    sscanf(line, "%d %s %s %s %d %f", &objId, modelName, texName, animName, &weaponType, &drawDist);
    CWeaponModelInfo* mi = CModelInfo::AddWeaponModel(objId);
    mi->SetModelName(modelName);
    mi->m_fDrawDistance = drawDist;
    mi->SetTexDictionary(texName);
    mi->SetAnimFile(animName);
    mi->SetColModel(&CTempColModels::ms_colModelWeapon, false);
    return objId;
}

// 0x5B4AB0
void CFileLoader::LoadZone(const char* line) {
    char    infoLabel[24];
    int32   type;
    CVector min{}, max{};
    int32   level;
    char    textLabel[12];

    // Como el original: la zona solo se crea si se leen los 10 campos
    if (sscanf(line, "%s %d %f %f %f %f %f %f %d %s", infoLabel, &type, &min.x, &min.y, &min.z, &max.x, &max.y, &max.z, &level, textLabel) == 10)
        CTheZones::CreateZone(infoLabel, static_cast<eZoneType>(type), min, max, static_cast<eLevelName>(level), textLabel);
}

// 0x5B51E0
// orig AddBuildingInstancesToWorld ?
void LinkLods(int32 numRelatedIPLs) {
    // Link LODs
    for (int32 i = 0; i < (int32)gNumLoadedBuildings; i++) {
        const auto building = gpLoadedBuildings.data()[i];
        const auto idx = (int32)building->GetLodIndex();
        if (idx != -1) {
            const auto lod = gpLoadedBuildings.data()[idx];
            building->SetLod(lod);
            lod->AddLodChildren();
        } else {
            building->SetLod(nullptr);
        }
    }

    // Load related IPLs (?)
    // I'm unsure how this exactly works, but in theory:
    // `SetupRelatedIPLs` returns how many IPLs were loaded, and that's what `numRelatedIPLs` is
    // Thus, everything above `gNumLoadedBuildings` is such an IPL instance
    // Here we process all buildings (and their related IPLs) + the (related) IPLs we've just loaded in `SetupRelatedIPLs`
    // Como el original: isCacheLoading se consulta en cada vuelta; con la cache cargando se saltan las
    // entradas relacionadas (i >= gNumLoadedBuildings) y sin ella se crean con addIPLEntity. El limite se
    // relee en cada vuelta (sin signo).
    for (uint32 i{}; i < (uint32)(gNumLoadedBuildings + numRelatedIPLs); i++) {
        const auto isIPL = i >= (uint32)gNumLoadedBuildings;
        if (CColAccel::isCacheLoading()) {
            if (isIPL) {
                continue;
            }
        } else if (isIPL) {
            CColAccel::addIPLEntity(gpLoadedBuildings.data(), gNumLoadedBuildings, i);
        }
        const auto building = gpLoadedBuildings.data()[i];

        // 0x5B5285
        if (building->GetNumLodChildren() || TheCamera.m_fLODDistMultiplier * building->GetModelInfo()->m_fDrawDistance > 300.f) {
            building->SetupBigBuilding();
        }

        // 0x5B5293 - Now handle the collision model for the building/lod
        if (const auto lod = building->GetLod()) {
            const auto mi = building->GetModelInfo(),
                lodMI = lod->GetModelInfo();
            if (lod->GetNumLodChildren() == 1) {
                lod->m_bUnderwater |= building->m_bUnderwater;
                if (const auto cm = mi->GetColModel()) {
                    if (cm != lodMI->GetColModel()) {
                        lodMI->DeleteCollisionModel();
                        lodMI->SetColModel(cm);
                    }
                }
            } else if (mi->bDoWeOwnTheColModel) {
                mi->m_fDrawDistance = 400.f;
            } else {
                lod->RemoveLodChildren();
                building->SetLod(nullptr);
            }
        }
    }

    // 0x5B5321
    CColAccel::cacheIPLSection(gpLoadedBuildings.data(), gNumLoadedBuildings);

    // 0x5B5358
    for (int32 i = 0; i < (int32)gNumLoadedBuildings; i++) {
        CWorld::Add(gpLoadedBuildings.data()[i]);
    }
}

// 0x5B8700
void CFileLoader::LoadScene(const char* filename) {
    ZoneScoped;

    gNumLoadedBuildings = 0;

    enum class SectionID {
        NONE = 0, // NOTSA - Placeholder value

        PATH = 1,
        INST = 2,
        MULT = 3,
        ZONE = 4,
        CULL = 5,
        OCCL = 6,
        GRGE = 8,
        ENEX = 9,
        PICK = 10,
        CARS = 11,
        JUMP = 12,
        TCYC = 13,
        AUZO = 14,
    };

    auto sectionId{ SectionID::NONE };

    auto file = CFileMgr::OpenFile(filename, "rb");
    for (char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (!line[0]) // Emtpy line
            continue;

        const auto LineBeginsWith = [linesv = std::string_view{line}](auto what) {
            return linesv.starts_with(what);
        };

        if (LineBeginsWith("#"))
            continue; // Skip comment

        if (sectionId != SectionID::NONE) {
            if (LineBeginsWith("end")) {
                sectionId = SectionID::NONE;
                continue;
            }

            switch (sectionId) {
            case SectionID::INST: {
                gpLoadedBuildings[gNumLoadedBuildings++] = LoadObjectInstance(line);
                break;
            }
            case SectionID::ZONE:
                LoadZone(line);
                break;
            case SectionID::CULL:
                LoadCullZone(line);
                break;
            case SectionID::OCCL:
                LoadOcclusionVolume(line, filename);
                break;
            case SectionID::PATH:
                // Como el original: la tabla de saltos no procesa la seccion PATH (leftover de VC);
                // la primera linea de datos solo termina la lectura (ver abajo).
                break;
            case SectionID::GRGE:
                LoadGarage(line);
                break;
            case SectionID::ENEX:
                LoadEntryExit(line);
                break;
            case SectionID::PICK:
                LoadPickup(line);
                break;
            case SectionID::CARS:
                LoadCarGenerator(line, 0);
                break;
            case SectionID::JUMP:
                LoadStuntJump(line);
                break;
            case SectionID::TCYC:
                LoadTimeCyclesModifier(line);
                break;
            case SectionID::AUZO:
                LoadAudioZone(line);
                break;
            }

            if (sectionId == SectionID::PATH)
                break; // TODO: Unsure why it stops after a path section.

        } else {
            const auto FindSectionID = [&] {
                static const struct { std::string_view name; SectionID id; } mapping[]{
                    { "path", SectionID::PATH },
                    { "inst", SectionID::INST },
                    { "mult", SectionID::MULT },
                    { "zone", SectionID::ZONE },
                    { "cull", SectionID::CULL },
                    { "occl", SectionID::OCCL },
                    { "grge", SectionID::GRGE },
                    { "enex", SectionID::ENEX },
                    { "pick", SectionID::PICK },
                    { "cars", SectionID::CARS },
                    { "jump", SectionID::JUMP },
                    { "tcyc", SectionID::TCYC },
                    { "auzo", SectionID::AUZO },
                };

                for (const auto& [itname, id] : mapping) {
                    if (LineBeginsWith(itname)) {
                        return id;
                    }
                }

                return SectionID::NONE; // Possible if the line was empty, let's move on to the next line.
            };
            sectionId = FindSectionID();
        }
    }
    CFileMgr::CloseFile(file);

    // This really seems like should be in CIplStore...
    auto newIPLIndex{ -1 };
    if (gNumLoadedBuildings > 0) {
        newIPLIndex = CIplStore::GetNewIplEntityIndexArray(gNumLoadedBuildings);
        rng::copy(
            gpLoadedBuildings | std::views::take(gNumLoadedBuildings),
            CIplStore::GetIplEntityIndexArray(newIPLIndex)
        );
    }
    LinkLods(CIplStore::SetupRelatedIpls(filename, newIPLIndex, &gpLoadedBuildings[gNumLoadedBuildings]));
    CIplStore::RemoveRelatedIpls(newIPLIndex); // I mean this totally makes sense, doesn't it?
}

// todo: add original min/max check
// 0x5B8400
void CFileLoader::LoadObjectTypes(const char* filename) {
    /* Unused
    char filenameCopy[MAX_PATH]{};
    strcpy_s(filenameCopy, filename);
    */

    enum class SectionID {
        NONE = 0, // NOTSA - Placeholder value

        OBJS = 1,
        TOBJ = 3,
        WEAP = 4,
        HIER = 5,
        ANIM = 6,
        CARS = 7,
        PEDS = 8,
        PATH = 9,
        TDFX = 10, // 2DFX (but enum names can't start with a number, so..)
        TXDP = 11
    };

    auto sectionId{ SectionID::NONE };

    int32 nPathEntryIndex{ -1 }, pathHeaderId{};
    int32 pathType{};

    auto file = CFileMgr::OpenFile(filename, "rb");
    for (const char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (!line[0])
            continue;

        const auto LineBeginsWith = [linesv = std::string_view{ line }](auto what) {
            return linesv.starts_with(what);
        };

        if (LineBeginsWith("#"))
            continue;

        if (sectionId != SectionID::NONE) {
            // Process section

            if (LineBeginsWith("end")) {
                sectionId = SectionID::NONE;
                continue;
            }

            switch (sectionId) {
            case SectionID::OBJS:
                LoadObject(line);
                break;
            case SectionID::TOBJ:
                LoadTimeObject(line);
                break;
            case SectionID::WEAP:
                LoadWeaponObject(line);
                break;
            case SectionID::HIER:
                LoadClumpObject(line);
                break;
            case SectionID::ANIM:
                LoadAnimatedClumpObject(line);
                break;
            case SectionID::CARS:
                LoadVehicleObject(line);
                break;
            case SectionID::PEDS:
                LoadPedObject(line);
                break;
                // R* does something weird with the object IDs frm the above cases,
                // but it isn't used, so I wont put it in here, cause it would require jumps..
            case SectionID::PATH: {
                // Leftover from VC, as path's are loaded differently in SA.
                // That is, all this does nothing in the end.

                if (nPathEntryIndex == -1) {
                    pathHeaderId = LoadPathHeader(line, pathType);
                    nPathEntryIndex = 0; // Como el original: tras la cabecera vienen 12 nodos
                } else {
                    switch (pathType) {
                    case 0:
                        LoadPedPathNode(line, pathHeaderId, nPathEntryIndex);
                        break;
                    case 1:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, false);
                        break;
                    case 2:
                        LoadCarPathNode(line, pathHeaderId, nPathEntryIndex, true);
                        break;
                    }
                    if (++nPathEntryIndex == 12)
                        nPathEntryIndex = -1;
                }
                break;
            }
            case SectionID::TDFX:
                Load2dEffect(line);
                break;
            case SectionID::TXDP:
                LoadTXDParent(line);
                break;
            }
        } else {
            // Find out next section

            const auto FindSectionID = [&] {
                static const struct { std::string_view name; SectionID id; } mapping[]{
                    { "objs", SectionID::OBJS },
                    { "tobj", SectionID::TOBJ },
                    { "weap", SectionID::WEAP },
                    { "hier", SectionID::HIER },
                    { "anim", SectionID::ANIM },
                    { "cars", SectionID::CARS },
                    { "peds", SectionID::PEDS },
                    { "path", SectionID::PATH },
                    { "2dfx", SectionID::TDFX },
                    { "txdp", SectionID::TXDP },
                };

                for (const auto& [name, id] : mapping) {
                    if (LineBeginsWith(name)) {
                        return id;
                    }
                }

                return SectionID::NONE; // May happen if line was empty. It's fine.
            };
            sectionId = FindSectionID();
        }
    }
    CFileMgr::CloseFile(file);
}

// 0x5B3AC0
void CFileLoader::ReloadObjectTypes(const char* arg1) {
    // NOP
}

// Izzotop: Untested and may be wrong, see at your own risk
// unused
// 0x5B6E10
void CFileLoader::ReloadPaths(const char* filename) {
    int32 objModelIndex;
    int32 id;
    char  unused[4]{};

    bool pathAllocated = false;
    int32 pathEntryIndex = -1;
    auto file = CFileMgr::OpenFile(filename, "r");
    for (char* line = LoadLine(file); line; line = LoadLine(file)) {
        if (*line == '#' || !*line)
            continue;

        if (pathAllocated) {
            if (make_fourcc3(line, "end")) {
                pathAllocated = false;
            } else if (pathEntryIndex == -1) {
                sscanf(line, "%d %d %*s", &id, &objModelIndex);
                pathEntryIndex = 0;
            } else {
                if (id) {
                    if (id == 1) {
                        LoadCarPathNode(line, objModelIndex, pathEntryIndex, false);
                    } else if (id == 2) {
                        LoadCarPathNode(line, objModelIndex, pathEntryIndex, true);
                    }
                } else {
                    LoadPedPathNode(line, objModelIndex, pathEntryIndex);
                }

                if (++pathEntryIndex == 12)
                    pathEntryIndex = -1;
            }
        } else if (make_fourcc4(line, "path")) {
            pathAllocated = true;
            CPathFind::AllocatePathFindInfoMem();
        }
    }

    CFileMgr::CloseFile(file);
}

/**
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x5B3930
 */
RpAtomic* CFileLoader::FindRelatedModelInfoCB(RpAtomic* atomic, void* data) {
    char name[24] = {0};
    bool bDamage = false;

    const char* nodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));
    GetNameAndDamage(nodeName, name, bDamage);

    int32 modelId = MODEL_INVALID;
    CBaseModelInfo* mi = CModelInfo::GetModelInfo(name, &modelId);
    if (mi) {
        CAtomicModelInfo* ami = mi->AsAtomicModelInfoPtr();
        CVisibilityPlugins::SetAtomicRenderCallback(atomic, nullptr);
        if (bDamage) {
            ami->AsDamageAtomicModelInfoPtr()->SetDamagedAtomic(atomic);
        } else {
            ami->SetAtomic(atomic);
        }
        RpClumpRemoveAtomic(static_cast<RpClump*>(data), atomic);
        RwFrame* pRwFrame = RwFrameCreate();
        RpAtomicSetFrame(atomic, pRwFrame);
        CVisibilityPlugins::SetModelInfoIndex(atomic, modelId);
    }
    return atomic;
}

/**
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x537150
 */
RpAtomic* CFileLoader::SetRelatedModelInfoCB(RpAtomic* atomic, void* data) {
    char name[24] = {0};
    bool bDamage = false;

    auto mi = CModelInfo::GetModelInfo(gAtomicModelId)->AsAtomicModelInfoPtr();
    auto frameNodeName = GetFrameNodeName(RpAtomicGetFrame(atomic));

    GetNameAndDamage(frameNodeName, name, bDamage);

    CVisibilityPlugins::SetAtomicRenderCallback(atomic, nullptr);

    if (bDamage)
        mi->AsDamageAtomicModelInfoPtr()->SetDamagedAtomic(atomic);
    else
        mi->SetAtomic(atomic);

    RpClumpRemoveAtomic(static_cast<RpClump*>(data), atomic);

    RpAtomicSetFrame(atomic, RwFrameCreate()); // Just create a new empty frame for it

    CVisibilityPlugins::SetModelInfoIndex(atomic, gAtomicModelId);

    return atomic;
}

/**
 * Adds texture to the dictionary
 * @param texture callback texture
 * @param dict texture dictionary (RwTexDictionary*)
 * @return callback texture
 * @addr 0x5B38F0
 */
RwTexture* AddTextureCB(RwTexture* texture, void* dict) {
    RwTexDictionaryAddTexture((RwTexDictionary*)dict, texture);
    return texture;
}

/**
 * Makes a copy of atomic and adds it to clump
 * @param atomic callback atomic
 * @param data clump object (RpClump*)
 * @return callback atomic
 * @addr 0x537290
 */
RpAtomic* CloneAtomicToClumpCB(RpAtomic* atomic, void* data) {
    RpAtomic* clone = RpAtomicClone(atomic);
    auto frame = RpClumpGetFrame(atomic);
    RpAtomicSetFrame(clone, frame->root);
    RpClumpAddAtomic(static_cast<RpClump*>(data), clone);
    return atomic;
}

// Gets file name from a path
// 0x5B3660
const char* GetFilename(const char* filepath) {
    const char* pch = strrchr(filepath, '\\');
    return pch ? pch + 1 : filepath;
}

// 0x5B3680
void LoadingScreenLoadingFile(const char* str) {
    const char* screenName = GetFilename(str);
    sprintf_s(gString, "Loading %s", screenName);
    LoadingScreen("Loading the Game", gString);
}
