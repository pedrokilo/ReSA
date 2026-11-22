#pragma once

/*
 * Lote DE05-F (docs/DatosExe): resto de variables de .bss del juego como objetos propios. Cada uno es un
 * almacen en cero que ConstructF() construye con el constructor de su tipo (como el _initterm del EXE) y cuyo destructor, si no es
 * trivial, se registra con el atexit del CRT del EXE. Se accede con b<DIR><T>() (devuelve T&; T es el tipo de la StaticRef original). Generado por
 * contrib/datos_exe_de05f.py; definidas en DataBssF.cpp. Se incluye despues de StdInc.h (los tipos ya estan declarados).
 */

namespace DatosExe::Bss {
void ConstructF(); // construye todos (se llama sola al cargar la DLL, antes de InjectHooks)

template <class T> T& b8E4004(); // 0x008E4004; se especializa en DataBssF.cpp para HANDLE
template <class T> T& b8E4008(); // 0x008E4008; se especializa en DataBssF.cpp para HANDLE
template <class T> T& b8E48AC(); // 0x008E48AC; se especializa en DataBssF.cpp para RwStream
template <class T> T& b965560(); // 0x00965560; se especializa en DataBssF.cpp para CColPool*
template <class T> T& b96A7D8(); // 0x0096A7D8; se especializa en DataBssF.cpp para CEntryExitsPool*
template <class T> T& b96EA98(); // 0x0096EA98; se especializa en DataBssF.cpp para std::array<int32, NUM_PATH_INTERIOR_AREAS>
template <class T> T& b96EF90(); // 0x0096EF90; se especializa en DataBssF.cpp para std::array<bool, NUM_PATH_MAP_AREAS>
template <class T> T& b96EFD0(); // 0x0096EFD0; se especializa en DataBssF.cpp para std::array<bool, NUM_PATH_MAP_AREAS>
template <class T> T& bA8B430(); // 0x00A8B430; se especializa en DataBssF.cpp para std::array<CRunningScript, MAX_NUM_SCRIPTS>
template <class T> T& bA951F8(); // 0x00A951F8; se especializa en DataBssF.cpp para std::array<CColPoint, 32>
template <class T> T& bB5F87C(); // 0x00B5F87C; se especializa en DataBssF.cpp para AnimBlendFrameData*
template <class T> T& bB610F0(); // 0x00B610F0; se especializa en DataBssF.cpp para LARGE_INTEGER
template <class T> T& bB6131C(); // 0x00B6131C; se especializa en DataBssF.cpp para eWeaponType
template <class T> T& bB73C98(); // 0x00B73C98; se especializa en DataBssF.cpp para std::array<CColPoint, 32>
template <class T> T& bBA82D8(); // 0x00BA82D8; se especializa en DataBssF.cpp para std::array<CMenuSystem::Menu*, 2>
template <class T> T& bBAA350(); // 0x00BAA350; se especializa en DataBssF.cpp para eRadarTraceHeight
template <class T> T& bBAD3EC(); // 0x00BAD3EC; se especializa en DataBssF.cpp para TList_c<FurnitureEntity_c>
template <class T> T& bBBCDC8(); // 0x00BBCDC8; se especializa en DataBssF.cpp para CDirectory::DirectoryInfo
template <class T> T& bBD0B74(); // 0x00BD0B74; se especializa en DataBssF.cpp para FILESTREAM
template <class T> T& bBD0B78(); // 0x00BD0B78; se especializa en DataBssF.cpp para std::array<uint8, 204'800>
template <class T> T& bC02C28(); // 0x00C02C28; se especializa en DataBssF.cpp para D3DMATRIX
template <class T> T& bC102A0(); // 0x00C102A0; se especializa en DataBssF.cpp para std::array<CColPoint, 32>
template <class T> T& bC18D50(); // 0x00C18D50; se especializa en DataBssF.cpp para CPointRoute
template <class T> T& bC1C26C(); // 0x00C1C26C; se especializa en DataBssF.cpp para tWheelState
template <class T> T& bC1C270(); // 0x00C1C270; se especializa en DataBssF.cpp para tWheelState
template <class T> T& bC1CB30(); // 0x00C1CB30; se especializa en DataBssF.cpp para tTransmissionGear*
template <class T> T& bC1CB34(); // 0x00C1CB34; se especializa en DataBssF.cpp para tTransmissionGear*
template <class T> T& bC1CD98(); // 0x00C1CD98; se especializa en DataBssF.cpp para CColSphere
template <class T> T& bC6A170(); // 0x00C6A170; se especializa en DataBssF.cpp para RxVertexIndex*
template <class T> T& bC718B0(); // 0x00C718B0; se especializa en DataBssF.cpp para std::array<tFontData, 2>
template <class T> T& bC80434(); // 0x00C80434; se especializa en DataBssF.cpp para RwRGBAReal
template <class T> T& bC80444(); // 0x00C80444; se especializa en DataBssF.cpp para RwRGBAReal

} // namespace DatosExe::Bss
