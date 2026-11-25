<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 24/11/2026 (Ciudad de México):** 1007 grupos / 13511 funciones reconstruidas; 11800 con evidencia histórica y 1711 pendientes de validación en 52 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnS2Aresto: resto del lote S2A (21/11/2026)

Worktree `wv-23`, rama `s2aresto` sobre `integracion-fase7`. Sin push. Regla: reconstruir, enganchar, compilar; sin bancos ni juego. Todas las filas nuevas de `docs/hooks.csv` con validated=0.
Desensamblado: `python reversing/FnS2A5A6A/exedis.py <dir hex> <bytes>` (capstone sobre la copia del EXE de CVehicle Test).

| Funcion | Direccion | Estado | Verificacion |
|---|---|---|---|
| CVehicle::DoSunGlare | 0x6DD6F0 | reconstruida y enganchada (`RH_ScopedInstall`) | lectura del desensamblado completo (542 instrucciones, pila anotada con `[esp+X]` -> `[E-Y]`); compila y enlaza. Sin emulacion ni juego |
| CPhysical::ApplyFrictionTurnForce | 0x543100 | reconstruida y enganchada | idem; es la mitad de giro de ApplyFrictionForce (argumentos reales: fuerza, punto) |
| CPhysical::ApplySpringDampeningOld | 0x544100 | reconstruida y enganchada | idem; comparaciones de float segun el salto (NaN) |
| CPhysical::ApplyScriptCollision | 0x546ED0 | reconstruida y enganchada | idem |
| WinInput::diMouseInit | 0x7469A0 | ya estaba reconstruida y enganchada (activa; `locked` solo congela la GUI). Bloqueo explicado, ver abajo | comparada con 7469A0..746A0C |
| CPad::UpdatePads | 0x541DD0 | ya enganchada con HookInstall; fila anadida a hooks.csv | n/a |

## Notas de DoSunGlare
- Gates: bit 29 de las banderas fisicas, up.z < 0 (solo "menor"), apariencia != 1, SunGlare <= 0/NaN. Despues dist, C = D*(2/dist) (Veh83D*), S = C + vector al sol (indice sin comprobar, puntero crudo), P normalizado con CVector::Normalise.
- Las normalizaciones 2D usan `VehENormalise2D` (float, ya existente en Vehicle.cpp); el EXE la hace en doble sin redondear: difiere solo con magnitudes extremas.
- Colores: los dwords que el EXE empuja a RegisterCorona llevan basura de pila en los 3 bytes altos; la corona solo usa el byte bajo (uint8).
- Se anade una guarda de `m_pColData` nulo (el EXE lo desreferencia sin comprobar y caeria).
- Registra la corona con el RegisterCorona (0x6FC580, sobrecarga con eCoronaType) propio; los argumentos se reconstruyeron de las 21 pilas empujadas (alcance 90, tipo 0, 1.5, 15.0).
- Riesgo visual: es el camino caliente de render de cada coche; no se ha visto en juego. Si hubiera artefactos, desactivar con RH_ScopedInstall -> comentar la linea de DoSunGlare en Vehicle.cpp.

## diMouseInit (0x7469A0)
No hay nada que arreglar: el hook se instala activo en las dos variantes (SDL3 `InjectPadHooks` y DirectInput `InjectHooks`; solo uno de los dos se compila segun la variante). `locked = true` solo impide cambiarlo desde la GUI. El texto "ACCESS DENIED y se cae" viene del proyecto original, donde WIN_FCHECK abortaba; aqui solo registra y vuelve (WinPlatform.h). El EXE hace lo mismo: ante un HRESULT negativo sale sin tocar el resto y no comprueba el Acquire final. Evidencia: desensamblado 7469A0..746A0C vs `WinInput.cpp:143`. Falta (no hecho): una prueba con DirectInput real que provoque E_ACCESSDENIED (requiere juego).

## No hecho
- Emulacion Unicorn de DoSunGlare y de las tres de CPhysical (necesitan modelar CMatrix, colision comprimida, CCoronas y GetSpeed/ApplyForce): no se hizo.
- Regeneracion de docs/ReversedClasses.md y de los bloques de estado generados en los .cpp (contrib/hooks2md.py): no ejecutada.
