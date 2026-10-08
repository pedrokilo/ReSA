<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6822 con evidencia histórica, 0 con vigencia acreditada y 1624 pendientes en 64 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Impactos de bala sobre BMX: acceso fuera de la tabla de superficies

Corrección del cierre observado el 23 de septiembre de 2026 a las 18:43:19 en la copia `GTA San Andreas - CVehicle Test`.

## Causa y cambio

La pila registrada sigue `CWeapon::DoBulletImpact` → `CAudioEngine::ReportBulletHit` → `CAECollisionAudioEntity::ReportBulletHit` → `PlayBulletHitCollisionSound` → `SurfaceInfos_c::IsAudioWater` → aserción de `std::array<SurfaceInfo_c,179>`.

`ReportBulletHit` convierte los impactos sobre BMX al identificador especial de audio 188. La función de reproducción aceptaba valores menores que 195 y después consultaba una tabla de 179 entradas. El log no capturó el índice concreto, pero el banco cubre directamente esta ruta defectuosa.

En [AECollisionAudioEntity.cpp](../../source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp), `PlayBulletHitCollisionSound` convierte BMX a `SURFACE_CAR`, aplicando el tratamiento de impactos sobre vehículos. Rechaza los demás valores fuera de las 179 superficies antes de consultar la tabla. No aumenta estructuras, no cambia ABI, no desactiva aserciones y no cambia flags de hooks. La asociación BMX → vehículo es la decisión correctiva; no se atribuye al original, cuya lectura estaba fuera de límites.

## Validación realizada

Resultado: **APROBADO**, ejecutado dentro de GTA SA con la sonda compilada en Debug y sin exclusiones de hooks por argumentos.

| Comprobación | Casos | Resultado |
| --- | ---: | --- |
| Superficies 0–178 contra la función del EXE | 8.234 | Misma familia de sonido y parámetros comparados |
| Todos los identificadores representables por un byte | 256 | Superficies válidas y BMX generan sonido; los demás no acceden a la tabla ni generan sonido |
| `ReportBulletHit`: BMX/coche, banco disponible/no disponible | 4 | Mismo tratamiento de vehículo; sin reproducción cuando falta el banco |

Las comparaciones normales combinan 179 superficies, dos ángulos (−90° y 90°) y 23 valores del sonido anterior (−1 a 21). Verifican que no se repita el sonido anterior y que la versión candidata se ejecute solo en su mitad de la comparación. Los ángulos fuerzan probabilidades extremas para comparar las ramas de metal sin depender del azar.

Se comparan familia de sonido, banco, volumen, distancia de atenuación, posición, flags y variación de velocidad. El identificador aleatorio exacto dentro de una familia puede diferir. Se capturan las solicitudes en `RequestNewSound` y se devuelve `nullptr` para no llenar los canales. La disponibilidad del banco se controla durante la prueba de `ReportBulletHit`; el vehículo es una estructura mínima con tipo y subtipo. La clasificación de superficies y el código de impactos sí se ejecutan dentro del motor. Esto no es una comprobación auditiva, una comparación de todos los campos de `CAESound` ni una repetición visual de la persecución.

El ejecutor confirmó restauración de hooks, ASI/PDB e INI después del banco y hashes de partidas intactos. A continuación se compiló e instaló el ASI normal sin instrumentación. No se acredita toda la clase ni se cambian los contadores de validación globales por este banco limitado.

## Evidencia e instalación

- [Resumen de validación](validation-summary.json).
- Informe local: `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-bullet-hit/20260923-185138-856781/summary.json`.
- Comparaciones: mismo directorio, `comparisons.jsonl`; SHA-256 `734f3c553a9dfb3c93c8309cc615e90c6978dd5eafbc497823f15e8b816f4ec4`.
- Sonda: SHA-256 `d4e6f27883ea2b759cd37c088f4cd0f177d66a4fe674e3ba22ab45b997f5ae21`.
- ASI normal instalado: SHA-256 `d8b39a395abfed3acae54a480df9db7368dbdbbaff94caeed817dcc121f903ec`.
- Respaldo anterior: `D:/JUEGOS/GTA San Andreas - CVehicle Test/_baseline/before-bullet-hit-fix-20260923-185319/`.
- Configuración de inicio y partidas conservadas. El rechazo del guardado `v1 Modified` y los vehículos que atraviesan el mapa son problemas separados de este cambio.

## Repetir el banco

Con GTA cerrado y la sonda de `artifacts/` copiada a `_baseline/` en la instalación de pruebas:

```powershell
python reversing/run.py --case bullet-hit
```

`build_probe.py` reconstruye la sonda y restaura los archivos fuente incluso si falla. Los binarios producidos en `bin/Debug` por ese comando están instrumentados: antes de instalar una versión normal hay que volver a compilar tras la restauración de fuentes. El manifiesto local y el ejecutor verifican el hash de la sonda. Logs de compilación, manifiesto e informe de instalación quedan en `artifacts/`, ignorado por Git.
