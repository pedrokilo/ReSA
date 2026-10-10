# Revisión Process* (10/10/2026)

## Diferencias reales corregidas

- Familia de volúmenes (AddAudioEvent-1, ProcessMovingParts, ProcessVehicleSkidding, ProcessVehicleFlatTyre, ProcessVehicleRoadNoise, ProcessReverseGear, ProcessAircraft/Prop/Heli/SeaPlane/RCPlane/RCHeli, ProcessTrainTrackSound, ProcessDummyTrainEngine, ProcessPlayerTrainBrakes y auxiliares): la utilidad del EXE devuelve el resultado interno extendido de x87 (`004D9E61 fldlg2`, `004D9E67 fyl2x`, `004D9E69 ret`). La utilidad reconstruida almacena float antes de devolverlo. Ese redondeo adicional provoca las diferencias de 1–2 ULP observadas en `requests` +0x14 (volumen). Helper local AudioLog10ScaledExe reproduce fyl2x seguido de fmul factor antes del fstp float. Evidencia de consumidor: `004F8D0E call 004D9E50`, `004F8D13 fmul [00858BA4]`, `004F8D1C fadd [esp+14h]`, `004F8D20 fstp [esp+18h]`. El umbral respeta también NaN (`004D9E5C test ah,1`, `004D9E5F jne`) mediante !(input >= umbral).
- ProcessVehicleRoadNoise: la fuente descartaba el cálculo de distancia a cámara. En realidad `004F8C24 push eax` desplaza ESP; `004F8C30 fstp [esp+14h]` escribe la variable anterior [esp+10h], frecuencia, no volumen. Reintroducido sqrt((z*z+x*x)+y*y), /[008CBD10], mínimo 1, *0.5+0.75. Evidencia orden: `004F8BE4 fmul`, `004F8BEA faddp`, `004F8BF0 faddp`, `004F8BF2 fsqrt`, `004F8BFA fdiv`, `004F8C11 fld 1`, `004F8C17 fmul 0.5`, `004F8C25 fadd 0.75`.
- GetVolForPlayerEngineSound ABI: el EXE preserva ECX; `004FC9C1 call 004F5D00`, `004FC9CD call 004F8070` reutiliza this sin recargarlo. Envoltura naked conserva ECX y ret 8. Registro conserva nombre mediante RH_ScopedNamedGlobalInstall.

## Clasificación restante

Las diferencias `world` sin requests no pueden clasificarse por un hash agregado; el agente del banco desglosa campos para discriminar fuente frente a ruido de relleno/estado. No se cambió el banco ni se ejecutó compilación/juego. Verificación dinámica pendiente del responsable, en serie: `CMAKE_BUILD_PARALLEL_LEVEL=2 _CL_=-MP2 bash reversing/_handoff/bench.sh VehAudioFinal4 3`.

## Iteración posterior (corrida 20261010-150800-825349)

- ProcessDummyRCCar y ProcessSpecialVehicle (delegación): diferencias de volumen amplias, no ULP. Ejemplo caso 3, EventVolume=0: petición original -7.173489, candidata 0. El EXE lee las tablas vivas `.data` mediante `00500EED push 008CC0A4h` / `00500EFE push 008CBF8Ch`; la fuente usaba tablas constexpr con datos iniciales del archivo. Sustituida copia estática por lectura de ambas direcciones, como el EXE. Confirmación dinámica pendiente.
- ProcessPlayerVehicleEngine: eliminado assert adicional de std::array al indexar transmisión mediante `.data()[vp.RealGear]`; EXE `004FBBC6 lea ...` calcula desplazamiento de marcha sin verificar límite. Fixture con marcha 6 podía disparar comprobación añadida de la biblioteca de depuración. Resto de índices del método: sonidos tipos constantes válidos 1..5.

## Iteración de los cinco fallos finales G6

ProcessPlayerVehicleEngine: los casos 105/502/624/707/864 difieren en RealRevsRatio (NaN candidato frente a cero original), propagado a hash de parámetros y trazas. No se modifica la semántica NaN del clamp: el EXE sí la conserva. La marcha 6 lee MaxVelocity en +0x48, que solapa m_nNumberOfGears en +0x4A: el fixture produce un subnormal (bits numeroMarchas << 16), con ChangeDownVelocity=0. EXE `004FBBCD fsub [eax+8]`, `004FBBD2 fsub [eax+8]`, `004FBBD5 fdivp st(1),st` conserva el divisor subnormal y devuelve cero al dividir velocidad cero. SSE con FTZ/DAZ convierte ese divisor a cero y crea NaN. Reproducido cálculo de ratio con x87 inline; clamp posterior conservado, sin máscara ni ajuste de dominio.
