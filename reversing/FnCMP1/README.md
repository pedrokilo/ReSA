<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnCMP1: auditoria de comparaciones de coma flotante con NaN (19/11/2026)

Motivo: `CVehicle::ProcessWheel` escribia `brake > adhesion` / `!(fwd <= brake)` donde el EXE hace `fcom; fnstsw; test ah,0x41; jp`, y con un operando NaN
el C++ propagaba el NaN al coche (ver `reversing/FnBIS1/RESULTADO.md`). Aqui se busca el mismo tipo de defecto en el resto de `source/game_sa`.

## Metodo

1. Patron de partida (el pedido): `!(x <= y)` / `!(x >= y)` en `*.cpp` (103 coincidencias en 48 archivos). Las comparaciones de enteros se descartan.
2. Para cada una: funcion y direccion (`docs/hooks.csv`, `RH_Scoped*Install`, comentario `// 0x...`), desensamblado de solo lectura del EXE con capstone y localizacion del
   `fcom/fcomp/fucom` que corresponde (por constante, por el salto y por el cuerpo de la rama).
3. Tabla de verdad de x87 (en `ah`: C0=1, C2=4, C3=0x40; mayor=0x00, menor=0x01, igual=0x40, NO ORDENADO=0x45): `fcmp.py` calcula, para cada
   `test ah,MASK`/`and eax,MASK<<8` + jcc, si el salto se toma con mayor, menor, igual y NaN (PF = paridad de `ah & MASK`; `jp` salta con PF=1).
   Se comprueba a mano a que rama del C++ va cada resultado (el destino del salto, no solo la bandera).
4. Veredicto: IGUAL (el C++ se comporta como el EXE con NaN), DIFERENTE, INDETERMINADO (ninguno quedo asi en la tabla revisada a mano).
5. Segunda pasada automatica (`escaneo.py`, salida `escaneo.csv`, 3363 comparaciones contra un literal en funciones con direccion conocida): compara cada
   predicado C++ con las comparaciones x87 del EXE que usan la misma constante. Sirve para ENCONTRAR candidatos, no para sentenciar (ver limites).

## Resultado (tabla revisada a mano: `auditoria.csv`, 107 filas)

* IGUAL: 89 (incluye `Automobile::ProcessControl`, `UpdateMovingCollision`, mapa del menu, ProcessWheel ya corregida, cuerpos x87 de agua, AirBoat, etc.)
* DIFERENTE: 17 filas en 12 funciones
* DESCARTADO: 1 (`World.cpp:1840`, enteros)
* INDETERMINADO: 0 en la tabla revisada; los pendientes estan en `escaneo.csv` (ver abajo).

DIFERENTE (todas con `!(a <= b)`/`>=` o `>=`/`<=` plano donde el EXE manda el NaN al otro lado):

| funcion | EXE con NaN | C++ con NaN |
|---|---|---|
| `CCam::Process_DW_PlaneSpotterCam` 0x51C250 (`planar>=5`) | NO falla | falla y sale |
| `CWorld::GetIsLineOfSightClear` 0x56A490 y `ProcessLineOfSight` 0x56BA00 (`!(target.x <= origin.x)`) | recorre de derecha a izquierda | de izquierda a derecha |
| `CCollision::IsStoredPolyStillValidVerticalLine` 0x414D70 (`!(ay <= az)`) | rama Z | rama Y |
| `CCarAI::EntitiesGoHeadOn` 0x41CD00 (`dot >= 0.8`) | devuelve true | false |
| `CTrain::ProcessControl` 0x6F86A0 (`gas >= 1`, `speed >= 1`: recorte) | NaN -> 1.0 | NaN se conserva |
| `CTrain::AddNearbyPedAsRandomPassenger` 0x6F8170 (distancia 625 y lado) | procesa el peaton | lo salta |
| `CGangWars::CreateAttackWave` 0x444810, `Update` 0x446610 (z>950, 30000, 70, 150) | rama contraria | rama contraria |
| `CPopulation::AddToPopulation/PopulationCouple` (`>= .75`), `ManageObject` 0x615DC0 (`<= 120`) | no sale / no elimina | sale / elimina |
| `FxEmitterBP_c::Render` 0x4A2C40 (rotacion `<= 0`) | no rota | rota con NaN (solo render) |

El patron del recorte (`if (x >= K) x = K` o `K < x ? K : x`) importa mas de lo que parece: el EXE lo escribe de forma que NaN SE SANEA (queda en K) y el C++
lo deja pasar; son los sitios por donde el EXE "se cura" solo de un NaN. Probable causa de que el EXE aguante NaN que nuestra DLL propaga.

## Parches (rama `cmp1` del worktree `D:\Original\wv-9`, desde origin/main cff02cea81; NO compilado, NO subido)

Solo C++ puro, un cambio por comparacion (se invierte el operador con su complemento ordenado: `a <= b` <-> `!(a > b)`, etc.). Bancos a repetir: `bancos_a_repetir.csv`
(12 funciones; todas ya tienen banco en `reversing/validation-registry/parte-00{1..4}.json`; hay que repetirlos con entradas NaN, los actuales no las cubren).

## Limites / lo que falta

* `escaneo.csv`: 3363 comparaciones: 2869 coherentes con alguna comparacion del EXE, 330 SIN_ANCLA (la constante no aparece en el EXE de esa funcion),
  162 `POSIBLE_DIFERENTE_NAN` y 2 `SIN_PATRON`. Los 162 NO estan confirmados: la tasa de falsos positivos es alta (ejemplo comprobado: `CarCtrl.cpp:4919`, la constante 4.0
  se compara en otro sitio de la funcion). Los que si se confirmaron a mano al revisarlos (CarAI 302, Train 1221/1230, GangWars 1090/1107/1149) ya estan en `auditoria.csv` y parcheados.
  Pendientes de revision manual, por prioridad: CarCtrl (971, 4694-4706, 4919), Automobile (969, 3373, 3625/3627, 5102, 5802), Boat (737, 1050, 1106), Vehicle (1199-1203, 2402, 7332, 7519),
  Physical 2476/2528, Camera 3163, Cam (1111, 2185, 2313, 3280, 3431), Streaming 838-928 y 3866, Weapon (758, 1097, 1927, 2030), Tasks (ArrestPed, KillCriminal, GoToPointFine, SimplePlayerOnFoot).
* No se auditaron las comparaciones planas `a > b`, `a < b` con operandos que no son un literal ni las de doble (solo cribadas las de literal float); el defecto de ProcessWheel era
  de ese tipo (`brake > adhesion` sin literal). Haria falta emparejar el CFG del C++ con el del EXE para cubrirlas.
* La orientacion del salto se verifico leyendo el cuerpo de cada rama en las filas de `auditoria.csv`; en las filas que dicen "IGUAL" basadas en el comentario del propio codigo se
  comprobo el salto y la tabla de NaN, y el destino solo en las criticas (fisica, camara, coche, IA).
* `Cam.cpp:2676` y los demas parches tocan funciones ya validadas: tras compilar, repetir los bancos antes de integrar.

## Herramientas

* `fcmp.py <dir> [bytes]`: lista cada comparacion x87 con la tabla de NaN. `chk.py <dir> <const>`: solo las que usan esa constante. `exedis.py <dir> <bytes>`: desensamblado.
* `escaneo.py`: regenera `escaneo.csv`. Todas leen el EXE de `E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe` en solo lectura (no lo ejecutan).
