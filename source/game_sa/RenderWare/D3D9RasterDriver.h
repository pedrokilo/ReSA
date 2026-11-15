#pragma once

/**
 * Rutinas del controlador de rasters D3D9 de RenderWare dentro del EXE (zona 0x4CB530..0x4CC170): lista de rasters con recursos D3D
 * vivos, destruccion de rasters y consultas de formatos a IDirect3D9. El listado las atribuia a CWeaponModelInfo por cercania de
 * direcciones; no pertenecen a ninguna clase del juego.
 */
class RwD3D9RasterDriver {
public:
    static void InjectHooks();
};
