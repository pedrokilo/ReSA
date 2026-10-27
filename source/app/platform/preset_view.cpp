#include "StdInc.h"

#include "platform.h"
#include "X87Math.h"

struct PresetView {
    CVector m_Translation;
    float m_RotX;
    float m_RotY;
    float m_NearClip;
    float m_FarClip;
    char* m_Description;
    PresetView* m_Next;
};

// 3x3 identity matrix
constexpr CVector PresetViewAxisX = {1.0f, 0.0f, 0.0f}; // 0x8D2E00
constexpr CVector PresetViewAxisY = {0.0f, 1.0f, 0.0f}; // 0x8D2E0C
constexpr CVector PresetViewAxisZ = {0.0f, 0.0f, 1.0f}; // 0x8D2E18

constexpr auto ViewsFileName = "./views.txt"; // 0x8D2E24;

auto& PresetViews = StaticRef<PresetView*>(0xC1707C);
auto& NumPresetViews = StaticRef<int32>(0xC17080);
auto& CurrentPresetView = StaticRef<int32>(0x8D2E30); // -1

// 0x619780
bool RsSetPresetView(RwCamera* camera, int32 viewNum) {
    if (!camera || !NumPresetViews || viewNum >= NumPresetViews || viewNum < 0)
        return false;

    auto* pv = PresetViews;
    int v3 = NumPresetViews - viewNum - 1u;
    for (CurrentPresetView = viewNum; v3 > 0; --v3) {
        if (!pv) {
            NOTSA_UNREACHABLE();
            break;
        }

        pv = pv->m_Next;
    }

    RwFrame* parent = RwCameraGetFrame(camera);
    RwFrameSetIdentity(parent);
    RwFrameRotate(parent, &PresetViewAxisX, -pv->m_RotX, rwCOMBINEREPLACE);
    RwFrameRotate(parent, &PresetViewAxisY, pv->m_RotY, rwCOMBINEPOSTCONCAT);
    RwFrameTranslate(parent, &pv->m_Translation, rwCOMBINEPOSTCONCAT);
    RwFrameUpdateObjects(parent);
    RwCameraSetNearClipPlane(camera, pv->m_NearClip);
    RwCameraSetFarClipPlane(camera, pv->m_FarClip);

    return true;
}

// 0x619840
void RsSetNextPresetView(RwCamera* camera) {
    if (!camera)
        return;

    if (!NumPresetViews)
        return;

    auto viewNum = ++CurrentPresetView;
    if (CurrentPresetView >= NumPresetViews) {
        viewNum = 0;
        CurrentPresetView = 0;
    }

    RsSetPresetView(camera, viewNum);
}

// 0x619880
void RsSetPreviousPresetView(RwCamera* camera) {
    if (!camera || !NumPresetViews)
        return;

    if (--CurrentPresetView < 0)
        CurrentPresetView = NumPresetViews - 1;

    RsSetPresetView(camera, CurrentPresetView);
}

// 0x6198C0
void RsDestroyPresetViews() {
    auto pv = PresetViews;
    if (PresetViews) {
        PresetView* m_Next;
        do {
            m_Next = pv->m_Next;
            if (pv->m_Description) {
                CMemoryMgr::Free(pv->m_Description);
            }
            CMemoryMgr::Free(pv);
            pv = m_Next;
        } while (m_Next);
    }
    PresetViews = nullptr;
    NumPresetViews = 0;
}

// 0x619AB0
void* RsGetPresetViewDescription() {
    if (!NumPresetViews || CurrentPresetView == -1) {
        return nullptr;
    }

    auto* pv = PresetViews;
    for (auto i = NumPresetViews - CurrentPresetView - 1; i > 0; --i) {
        if (!pv) {
            break;
        }
        pv = pv->m_Next;
    }
    if (!pv) {
        NOTSA_UNREACHABLE();
    }

    return pv->m_Description;
}

// Funciones de fichero de RenderWare (RwFileFunctions, 11 punteros en RwEngineInstance + 0xC4: RwOsGetFileInterface, 0x804130)
struct PresetViewFileFunctions {
    void*  fexist;
    void*  (__cdecl* fopen)(const char* path, const char* mode); // +4
    int32  (__cdecl* fclose)(void* file);                        // +8
    void*  fread;
    void*  fwrite;
    char*  (__cdecl* fgets)(char* buffer, int32 size, void* file); // +0x14
    int32  (__cdecl* fputs)(const char* text, void* file);       // +0x18
    void*  rest[4];
};
static_assert(sizeof(PresetViewFileFunctions) == sizeof(RwGlobals::fileFuncs));
static_assert(offsetof(RwCamera, nearPlane) == 0x80 && offsetof(RwCamera, farPlane) == 0x84);

// 0x619D60 (devuelve NumPresetViews != 0 en EAX: 0 o 1)
// Vuelve a leer ./views.txt: una vista por linea "x y z rotX rotY near far [nombre]"; las lineas con menos de 7 numeros se ignoran y sin nombre se llama "View<N>".
// Como el original: si falta memoria devuelve false SIN cerrar el fichero.
bool RsLoadPresetViews() {
    RsDestroyPresetViews();
    CurrentPresetView = -1;
    auto* const files = reinterpret_cast<PresetViewFileFunctions*>(&RwEngineInstance->fileFuncs[0]);
    if (files) {
        char* const path = psPathnameCreate(ViewsFileName);
        void* const file = files->fopen(path, "r"); // 0x85A53C
        psPathnameDestroy(path);
        if (file) {
            char line[0x400];
            while (files->fgets(line, 0x3FF, file)) {
                (void)rwstrlen(line); // el EXE llama a strlen y no usa el resultado
                // Se descartan los caracteres no imprimibles (isprint del EXE, 0x8228B5)
                char* out = line;
                if (line[0]) {
                    for (const char* in = line;;) {
                        if (reinterpret_cast<int(__cdecl*)(int)>(0x8228B5)(uint8(*in))) {
                            *out++ = *in;
                        }
                        ++in;
                        if (!*in) {
                            break;
                        }
                    }
                }
                *out = '\0';

                float v[7];
                char name[0x400 - 0x30]{};
                const int32 count = rwsscanf(line, "%f%f%f%f%f%f%f %[^\0]", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], name); // 0x86D3E4: el literal del EXE es "...%[^\0]" (el conjunto queda sin cerrar tras el cero)
                if (count < 7) {
                    continue;
                }
                if (count == 7) {
                    rwsprintf(name, "%s%d", "View", NumPresetViews); // 0x86D3D4 / 0x86D3DC
                }
                auto* const view = static_cast<PresetView*>(CMemoryMgr::Malloc(sizeof(PresetView), 0));
                if (!view) {
                    RsErrorMessage("Out of memory - not all preset views loaded"); // 0x86D3A8
                    return false;
                }
                view->m_Translation.x = v[0];
                view->m_Translation.y = v[1];
                view->m_Translation.z = v[2];
                view->m_RotX = v[3];
                view->m_RotY = v[4];
                view->m_NearClip = v[5];
                view->m_FarClip = v[6];
                const auto length = uint32(rwstrlen(name));
                view->m_Description = static_cast<char*>(CMemoryMgr::Malloc(length + 1, 0));
                if (!view->m_Description) {
                    RsErrorMessage("Out of memory - not all preset views loaded");
                    CMemoryMgr::Free(view);
                    return false;
                }
                rwstrcpy(view->m_Description, name);
                view->m_Next = PresetViews;
                PresetViews = view;
                ++NumPresetViews;
            }
            files->fclose(file);
        }
    }
    return NumPresetViews != 0;
}

// 0x619FA0
// Anade al fichero ./views.txt la vista actual de la camara (posicion, rotX, rotY, plano cercano y lejano y el nombre "View<N>"), recarga las vistas y selecciona la ultima.
// Como el original: ningun llamador del EXE (la funcion no tiene llamadas directas ni punteros); devuelve lo que devuelve RsLoadPresetViews (0 si falla algo).
bool RsSavePresetView(RwCamera* camera) {
    // RwOsGetFileInterface (0x804130) = RwEngineInstance + 0xC4 (nunca nulo)
    auto* const files = reinterpret_cast<PresetViewFileFunctions*>(&RwEngineInstance->fileFuncs[0]);
    if (!files) {
        return false;
    }

    char* const path = psPathnameCreate(ViewsFileName);
    void* const file = files->fopen(path, "a"); // 0x863A2C
    psPathnameDestroy(path);
    if (!file) {
        RsErrorMessage("Cannot open preset view file"); // el EXE lo tiene expandido en linea (aviso de teclado 0x420 + psErrorMessage)
        return false;
    }

    // Ayudante usercall 0x619910 (X87Math.cpp)
    float pos[3], rotX, rotY;
    Hp66PresetViewAnglesCpp(reinterpret_cast<const float*>(RwFrameGetLTM(RwCameraGetFrame(camera))), reinterpret_cast<float(__cdecl*)(void*, const void*)>(&RwV3dNormalize), pos, &rotX, &rotY);

    char line[0x400];
    rwsprintf(line, "%0.6f %0.6f %0.6f  %0.6f %0.6f  %0.6f %0.6f %s%d\n", // 0x86D440
              double(pos[0]), double(pos[1]), double(pos[2]), double(rotX), double(rotY), double(camera->nearPlane), double(camera->farPlane), "View", NumPresetViews);
    (void)rwstrlen(line); // el EXE llama a strlen y no usa el resultado

    bool loaded = false;
    if (files->fputs(line, file) <= 0) {
        RsErrorMessage("Cannot write to preset view file");
    } else {
        loaded = RsLoadPresetViews();
        if (loaded) {
            RsSetPresetView(camera, NumPresetViews - 1);
        }
    }
    files->fclose(file);
    return loaded;
}
