#pragma once

/*
 * Correcciones de vehiculos tomadas de SilentPatch SA (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent); ver docs/THIRD_PARTY_NOTICES.md),
 * segunda tanda (rama silentpatch2-vehiculos). Todas son desviaciones deliberadas y opcionales del EXE; con SilentPatch apagado el codigo antiguo se conserva.
 * Documento: docs/CodigoVivoExe/silentpatch2-vehiculos.md.
 *
 * "SVF" de SilentPatch (caracteristicas especiales por modelo de vehiculo): aqui es una tabla estatica por id de modelo (sin lectura de ini).
 */
#include "Base.h"

class CAutomobile;
class CPlane;
class CTrailer;
class CVehicle;
class CVector;
class CVehicleModelInfo;
struct RwFrame;
struct RpClump;
struct RpMaterial;
struct RpAtomic;

namespace SilentPatchVehicles {

enum class Feature : uint8 {
    PHOENIX_FLUTTER, // capo del Phoenix
    SWEEPER_BRUSHES, // cepillos del Sweeper
    NEWSVAN_DISH,    // antena de la Newsvan
    EXTRA_AILERONS1, // como en el Beagle
    EXTRA_AILERONS2, // como en el Stuntplane
    DOUBLE_TRAILER,  // como el artict3 (remolque encadenable)
    VORTEX_EXHAUST,
    TOWTRUCK_HOOK,
    TRACTOR_HOOK,
    RHINO_WHEELS,
    FIRELA_LADDER,
};

//! Tabla estatica de SilentPatch (SVF.cpp, specialVehFeatures): id de modelo -> caracteristica
bool ModelHasFeature(int32 modelId, Feature feature);

//! Tabla de erratas de jerarquia de SilentPatch (HierarchyTypoFix): true si `dataName` (nombre pedido por el juego) y `nodeName` (nombre del marco del modelo) son una errata conocida
bool IsHierarchyTypo(const char* dataName, const char* nodeName);

//! CAutomobile::Fix: sustituye el reinicio de los marcos 8..24 (rotacion a identidad) por restaurar la matriz original del modelo
void ResetFramesAfterFix(CAutomobile& car);
//! CAutomobile::Fix / CPlane::Fix: reinicia los paneles que rebotan (salvo el de la grua/tractor/Vortex) y las ruedas del Rhino
void ResetBouncingPanelsAfterFix(CAutomobile& car);
void ResetBouncingPanelsAfterFix(CPlane& plane);

//! Paso de tiempo para los componentes del motor (BF Injection, Bandito, Hotknife): 0 con el motor apagado
float EngineComponentStep(const CAutomobile& car, float timeStep);

//! Animaciones extra tras CAutomobile::PreRender (Phoenix, Sweeper, Newsvan) y tras CPlane::PreRender (alerones extra)
void AfterAutomobilePreRender(CAutomobile& car);
void AfterPlanePreRender(CPlane& plane);

//! BlowUpCar: separa y oculta las ruedas que el DamageManager marca como perdidas (en lugar de la rueda delantera izquierda siempre)
void SpawnAndHideDestroyedWheels(CAutomobile& car, uint32 collisionType);

//! Barra de remolque: nodo 20 o, si no existe, el 21 (Tug)
RwFrame* GetTowBarFrame(const CAutomobile& car);
//! Remolque encadenable (artict3): posicion del enganche trasero (nodo 21); false si no aplica
bool GetDoubleTrailerTowBarPos(const CTrailer& trailer, CVector& outPos);

//! Rotores del helicoptero y helices del avion con efecto de desenfoque (SilentPatch, RenderRotors): alfa de los componentes estaticos/moviles segun la velocidad
//! (`speedDivisor`: 0.22 helicoptero, 0.31 avion); se llama justo antes de dibujar el vehiculo
void ApplyRotorBlur(CAutomobile& vehicle, float rotorSpeed, double speedDivisor);
//! Dibujo directo (sin lista ordenada) de un rotor/helice con la prueba alfa a 0 y el alfa de vertices activado (estado restaurado al salir)
void RenderRotorAtomic(RpAtomic* atomic);
//! Igual, pero solo si el nodo del atomico contiene "_prop" (helices de avion); si no, el dibujo normal
void RenderPropOrDefaultAtomic(RpAtomic* atomic);
//! true si SilentPatch asigna ademas el dibujo con alfa a las helices "_prop" del avion y a los rotores estaticos del helicoptero
bool RotorBlurEnabled();

//! Matriculas (SilentPatch, CustomCarPlate_*): cada vehiculo genera su propia matricula (texto aleatorio o el pedido por script/generador) y su diseno
//! (el 5 % de los coches no policiales usa uno al azar); se aplica a los materiales "carplate"/"carpback" del vehiculo solo al dibujarlo (coches y motos)
bool CreatePlate(CVehicle& vehicle, CVehicleModelInfo* modelInfo);
void BeforeRenderPlate(CVehicle& vehicle);
void AfterRenderPlate(CVehicle& vehicle);
//! Marca de "con matricula por vehiculo" que SilentPatch deja en CVehicleModelInfo::m_pPlateMaterial (no nulo para que sigan valiendo las comprobaciones del juego)
RpMaterial* PlateMaterialPlaceholder();

} // namespace SilentPatchVehicles
