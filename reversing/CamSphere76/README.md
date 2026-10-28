<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8950 funciones reconstruidas; 8950 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamSphere76: CCamera::IsSphereVisible(const CVector&, float) (0x00420D40)

Estado: **APROBADO dentro del juego** (27/10/2026, worktree `D:\Original\wy-1`, HEAD a7266bf64 + `source.patch`). 18432 pares (16 familias de 1024 + 2 extra de 1024), cero diferencias EXE/DLL, compuertas de ABI/ruta/restauracion en ambos lados y salida de cada lado igual a la del oraculo independiente (`fixture_oracle.py`). Evidencia final: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-camsphere76\20261027-214100-904974`.

`source.patch`: el mismo de MatrixOps76 (Matrix.cpp, Matrix.h, InjectHooksMain.cpp, Camera.cpp; aqui solo importa Camera.cpp).

| Funcion | Direccion | Hook (clase,nombre) | Tipo |
|---|---|---|---|
| IsSphereVisible (sin matriz) | 0x00420D40 | CCamera,IsSphereVisible-mirror | S |

## Estado previo y cambios de produccion
Ya estaba en C++ puro y marcada «Function has no hook»; `CCamera::InjectHooks` esta activo (instala la sobrecarga con matriz 0x420C40), asi que bastaba añadir `RH_ScopedOverloadedInstall(IsSphereVisible, "mirror", 0x420D40, bool(CCamera::*)(const CVector&,float))`. Sin defectos de produccion. El nombre `-mirror` distingue la sobrecarga (la otra es `-matrix`) y describe la unica diferencia con ella: el espejo.

## Revision contra el EXE (0x420D40..0x420D8C)
`esi = ECX`; llama a 0x420C40(origen, radio, esi+0xA4C) con ECX = camara; si AL != 0 devuelve 1; si no, lee el byte `[esi+0x970]` (m_bMirrorActive) DESPUES de esa llamada y, si es 0, devuelve 0; si no, devuelve el AL de 0x420C40(origen, radio, esi+0xA94). Sin aritmetica: el origen (puntero) y el radio (dword de la pila) solo se reenvian. La DLL (`IsSphereVisible(...) || (m_bMirrorActive && IsSphereVisible(...))`) reproduce el orden de lecturas.

## Banco
Fixtures: 18432 casos, 18 familias: sin espejo (acierta/falla), con espejo (acierta, falla/acierta, falla/falla), byte del espejo con los 256 valores (mas de 1 incluido), primera llamada con AL entre 2 y 255, el falso de la primera llamada CAMBIA el byte del espejo (se comprueba que se relee despues), radio con ±0, desnormales, infinitos y NaN de señal y silenciosos con cargas utiles, origen con especiales, origen apuntando dentro de la propia camara (+0x0, +0x970, +0xA4C, +0xA94), basura en los bytes altos de EAX de cada llamada, registros y semillas de pila variados.
Arena propia: camara (0xB00 bytes con guarda de 0x40 a cada lado) y origen (12 bytes + guardas). El falso de 0x420C40 (fastcall con 3 argumentos en pila, `ret 0xC`, identico en la direccion del EXE y en el cuerpo de la DLL) registra: this, puntero de origen y sus 3 palabras en el momento de la llamada, radio crudo y puntero de la matriz (etiquetado: +0xA4C o +0xA94), y devuelve AL scripted con basura arriba. Se comparan los eventos EN ORDEN, el AL de salida, bytes cambiados de las dos arenas, compuertas de ABI (pila, no volatiles, DF, SEH), ruta, x87/MXCSR y restauracion. Cobertura de ramas medida y calibrada al 50 % en `coverage.json`.
Colaborador sustituido: CCamera::IsSphereVisible con matriz 0x420C40 (con hook `IsSphereVisible-matrix`): lo valida DependenciesWorldFxFont5 (4096); este banco no valida su correccion.
Aritmetica: ninguna, por lo que no hay microtest `__asm`. El radio llega a la sobrecarga con matriz bit a bit incluso siendo sNaN (2706 pares), prueba de que la DLL no lo pasa por la x87.

Prueba negativa (produccion mutada y restaurada; `git diff` igual al de `source.patch`): leer `m_bMirrorActive` ANTES de la primera llamada: **501/18432 diferencias de resultado**, sin caida: `E:\JUEGOS\GTA San Andreas - CVehicle Test\resultados-camsphere76\20261027-213935-324427`.

Hallazgo del BANCO (no de produccion): la primera corrida (`...\resultados-camsphere76\20261027-213233-244229`, FALLO por el oraculo) se debio a que el banco calculaba mal el desplazamiento de las arenas de «origen dentro de la camara» (`Obj(off)` es desde el inicio de la arena, no de la ventana); corregido en `LiveComparison.hpp`. La corrida `20261027-213403-342340` (APROBADA) es de antes de la prueba negativa; la final es posterior a la restauracion.

No validado: camara nula, el AL de la segunda llamada mayor que 1 (el EXE lo devuelve en bruto, la DLL lo normaliza a 1; la funcion real 0x420C40 devuelve 0/1), EAX alto/ECX/EDX de salida, la propia 0x420C40.

Comandos: `CMAKE_BUILD_PARALLEL_LEVEL=1 _CL_=-MP1 bash reversing/_handoff/bench_here.sh CamSphere76`; `python -B run_local.py --analyze <carpeta>/comparisons.jsonl`; `python -B fixture_oracle.py <carpeta>/comparisons.jsonl`; `register_hook.py <carpeta>` (solo el coordinador).
