<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 795 grupos / 8669 funciones reconstruidas; 8669 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Revisión de los 19 destinos inicialmente desconocidos

Se revisaron referencias de fuente y se desensamblaron entradas seleccionadas con dis.sh sobre el EXE original de investigación. No se compiló ni ejecutó GTA y no se cambiaron hooks ni fuentes. Las nuevas categorías describen identidad estática, no equivalencia ni aprobación de bancos.

| Dirección | Identidad actual | Categoría | Implementación |
| --- | --- | --- | --- |
| 0x0072F430 | CMemoryMgr::Free (CRT free thunk) | library | wrapper |
| 0x006E2D50 | CVehicle::operator new / vehicle pool allocation | game | implemented |
| 0x004B0A50 | CEventScriptCommand::~CEventScriptCommand | game | implemented |
| 0x00821982 | printf-like CRT formatted output (FID conflict) | unknown | wrapper |
| 0x007491C0 | AtomicDefaultRenderCallBack | library | wrapper |
| 0x005A1D70 | CObject::CObject(int32,bool) | game | implemented |
| 0x006217D0 | FUN_006217d0 | unknown | unknown |
| 0x00821B11 | srand (CRT per-thread random seed) | library | wrapper |
| 0x005404A0 | CPad unnamed camera-cycle input helper | game | wrapper |
| 0x00514030 | CCamera unnamed geometry helper | game | wrapper |
| 0x0065DA70 | CPhysical force adapter (unnamed) | game | wrapper |
| 0x00734940 | GetFirstTexture | unknown | unknown |
| 0x004BC470 | SeekEntity distance setter helper | game | unknown |
| 0x00515D80 | CCam unnamed arrest cop-source helper | game | wrapper |
| 0x0054EEF0 | TransformPoints | game | implemented |
| 0x005B2090 | Camera vector path interpolation helper (unnamed) | game | wrapper |
| 0x005B2330 | Camera scalar path interpolation helper (unnamed) | game | wrapper |
| 0x00424100 | Constant-false game helper (unnamed) | game | unknown |
| 0x00449FF0 | CGarage unnamed camera door helper | game | wrapper |

## Implementaciones C++ que ya existen

- 0x5A1D70: CObject::CObject(int32,bool), Object.cpp:247. Population conserva llamada explícita original.
- 0x54EEF0: TransformPoints, common.cpp:120. StencilShadows llama la entrada original aunque existe cuerpo C++.
- 0x4B0A50: destructor normal CEventScriptCommand, EventScriptCommand.cpp:55; EXE elimina m_task vía destructor virtual y establece vtable de base. No confundir con deleting destructor.
- 0x6E2D50: allocador del pool de vehículos; Vehicle.cpp:549 operator new usa GetVehiclePool()->New(); EXE carga el mismo pool B74494 y salta al allocador. El tamaño solicitado no cambia la identidad del pool; ABI/equivalencia requieren banco.
- 0x5F7E80: CPedGroups::GetPedsGroup reconstruido durante esta campaña; banco inicial de 1.024 pares aprobado comunicado por el coordinador, pendiente registro posterior.

CMemoryMgr::Free tiene un cuerpo propio sólo bajo MEMORY_MGR_USE_MEMORY_HEAP; la rama alternativa vuelve al EXE. No contar ese cuerpo condicional como reemplazo probado en el build actual. SetEntityMinDist2D inline existe, pero el EXE 4BC470 compara antes de escribir, por lo que no se etiqueta como implementación equivalente. El helper 424100 devuelve false y algunas llamadas se redujeron en sus callers: eso no crea una función reconstruida independiente.

Se mantienen tres categorías desconocidas: 821982 (conflicto printf/wprintf y ABI), 6217D0 (constructor de tarea no identificado por firma completa) y 734940 (utilidad GetFirstTexture sobre RW, sin evidencia suficiente para atribuirla a biblioteca o juego). El CSV conserva el detalle de instrucciones y referencias para las 19 filas revisadas.

La cifra anterior de 237 destinos game pertenece a una clasificación global previa, no al subconjunto de esta revisión. No modificar sus denominadores a mano: volver a fusionar classifications a/b/c y calcular el conjunto por dirección. Las implementaciones identificadas arriba son candidatas a reducir dependencia, no validaciones nuevas.
