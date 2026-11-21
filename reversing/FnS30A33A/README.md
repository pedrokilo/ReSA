# FnS30A33A: lotes S30A, S31A, S32A y S33A del plan de sustitucion + RwD3D9RasterStreamReadDDS (0x81F360)

Hooks activos, `validated=0` en docs/hooks.csv. Verificacion: solo compilacion (EXIT=0); no se emulo ni se ejecuto el juego.

Archivos: `source/game_sa/RenderWare/rw/rpworld_s30a31a.cpp` (S30A y S31A, `RpWorldInjectHooksS30A31A`), `rwcore.cpp` final (S32A y S33A, `RwD3D9InjectHooksS32A33A`), `rwcore_dds.cpp` (DDS, `RwD3D9InjectHooksDDS`).
Herramientas: `desens.py` (capstone), `nombres.py` (anota llamadas con nombres propios), `rdconst.py`, `dis_*.txt`, `chk.bat/chk.ps1` (comprobacion de sintaxis `cl /Zs`), `genhooks.py`.

Hechas: S30A 24/24, S31A 15/18, S32A 14/14, S33A 13/14, DDS.
No hechas: 0x75B780 (RpTriStripMeshTunnel), 0x75C380 (RpBuildMeshGenerateTriStrip), 0x75DAE0 (RpGeometrySortByMaterial) quedan como envolturas del EXE; 0x7F6786 (S33A) no es funcion sino interior de la creacion del dispositivo (retorno del parche naked de WindowedMode.cpp).
Siguen por direccion (no raices): 0x74F590, 0x75A960, 0x74A770, 0x74A880, 0x74AE40, 0x74AEA0, 0x74AF20, 0x74AFA0, 0x74AFD0, 0x74B000, 0x74CE60, 0x762040/0x762440/0x7624A0/0x762740/0x762F30/0x763310, 0x4CBEE0 y _rpD3D9GetVertexShader (0x75EED0); RwCameraForAllSectorsInFrustum (0x751660, lote de otro agente) se llama por su funcion.
