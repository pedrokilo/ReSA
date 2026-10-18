<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8585 funciones reconstruidas; 8585 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Hilos y callers de los tres locks del audio del vehículo

Revisión estática del EXE 1.0 US y fuentes actuales. No ejecuta GTA ni modifica instrumentación. EXE SHA-256: `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.

## Llamadas directas del catálogo

| Destino | Caller | Instrucción call |
|---|---|---|
| ProcessPlayerVehicleEngine | 0x00501e10 | 0x00501fd6 |
| ProcessPlayerVehicleEngine | 0x00501e10 | 0x005021c2 |
| ProcessVehicle | 0x00502280 | 0x005023be |
| Service | 0x006b1880 | 0x006b18f1 |
| Service | 0x006b9250 | 0x006b9298 |
| Service | 0x006f1770 | 0x006f1793 |
| Service | 0x006f86a0 | 0x006f86b6 |

Se desensamblaron todas las instrucciones call y sus siguientes instrucciones:

```asm
00501FD6: call 004FBB10
00501FDB: jmp 00501FE9
00501FDD: lea edx,[esp+0Ch]
```

```asm
005021C2: call 004FBB10
005021C7: jmp 005021D5
005021C9: lea ecx,[esp+0Ch]
```

```asm
005023BE: call 00501E10
005023C3: pop edi
005023C4: pop ebx
```

```asm
006B18F1: call 00502280
006B18F6: mov eax,dword ptr [esi+00000594h]
006B18FC: cmp eax,4
```

```asm
006B9298: call 00502280
006B929D: mov bl,byte ptr [esi+00000429h]
006B92A3: mov dl,byte ptr [esi+00000614h]
```

```asm
006F1793: call 00502280
006F1798: mov ecx,esi
006F179A: call 006F0E20
```

```asm
006F86B6: call 00502280
006F86BB: mov al,byte ptr ds:[00BA6728h]
006F86C0: xor ebp,ebp
```

## Referencias de puntero crudo y vtables

- ProcessPlayerVehicleEngine `0x004fbb10`: 0 ocurrencias del DWORD absoluto en todos los bytes del EXE; posiciones []; 0 ranuras en binary_slots.csv.
- ProcessVehicle `0x00501e10`: 0 ocurrencias del DWORD absoluto en todos los bytes del EXE; posiciones []; 0 ranuras en binary_slots.csv.
- Service `0x00502280`: 0 ocurrencias del DWORD absoluto en todos los bytes del EXE; posiciones []; 0 ranuras en binary_slots.csv.

La búsqueda del DWORD absoluto incluye secciones ejecutables y de datos, sin presuponer alineación. Su ausencia descarta punteros absolutos literales presentes en ese archivo; no descarta punteros calculados o almacenados en ejecución ni llamadas indirectas no resueltas. Los CALL relativos se acreditan por el inventario y asm, no por buscar DWORD absolutos.

## Cadena de ejecución observada en fuentes

Los cuatro callers EXE de Service son CAutomobile::ProcessControl0x6B1880, CBike::ProcessControl0x6B9250, CBoat::ProcessControl0x6F1770 y CTrain::ProcessControl0x6F86A0. Fuentes Automobile.cpp574, Bike.cpp1193 (dependency15=0x00502280 en transcripción nativa con llamadas relocadas), Boat.cpp602, Train.cpp1069 invocan audio de vehículo; World.cpp2458/2490 despacha ProcessControl desde procesamiento del mundo. Service fuente1238 llama ProcessVehicle; ProcessVehicle fuente5382/5421 llama ProcessPlayerVehicleEngine en coche/moto con conductor jugador. Las tres funciones se registran como hooks simples, no ranuras virtuales.

Game::Idle/FrontendIdle llaman AudioEngine.Service en source/app/app_game.cpp352/463. AudioEngine.cpp778..832 atiende radio, ambient, script, speech y AESoundManager. AESoundManager.cpp254..259 llama CAESound::UpdateParameters, que en AESound.cpp380 llama virtualmente m_AudioEntity->UpdateParameters(this,playPosition). Esa callback es CAEVehicleAudioEntity::UpdateParameters, otro método distinto de los tres locks. No se encuentra llamada de esa callback a Service, ProcessVehicle o ProcessPlayerVehicleEngine en la fuente examinada.

## Workers reales

CAEStreamThread.cpp83 CreateThread(MainLoop); MainLoop0x4F15C0 líneas179..215 llama CAEStreamThread::Service y CAEStreamingChannel::Service, procesa tiempo/foreground/sleep. CAEStreamThread::Service0x4F1290 administra peticiones de tracks bajo critical section; no llama a los tres cuerpos de vehículo. CAESmoothFadeThread.cpp82 crea SmoothFadeProc; bucle llama Service0x4EED10 y atiende canales/buffers DirectSound. No hay referencias a los tres métodos en ese worker.

Conclusión limitada: el grafo directo del EXE y los callers fuente sitúan estos cuerpos en la cadena ProcessControl del mundo y en las llamadas explícitas de la sonda sobre hilo principal. Se observa concurrencia real en streaming/fades, que comparten hardware, pero NO hay evidencia aquí de que ejecuten los tres cuerpos. Este análisis no sustituye registro exhaustivo de thread IDs en runtime ni demuestra ausencia de llamadas indirectas futuras, mods u otro EXE.

Conmutar desde el punto de actualización principal fuera de esos cuerpos evita cambiar código actualmente en pila de la cadena conocida. La escritura de cinco bytes no es atómica; cualquier afirmación de seguridad queda limitada al EXE, fuentes y escenarios ejecutados, sin extenderse a futuros callers o modificaciones externas.

## Instrucciones de los workers

### StreamThread MainLoop

```asm
004F1614: call 004F1290
004F1619: mov ecx,dword ptr [edi+8]
004F161C: mov edx,dword ptr [ecx]
004F161E: call dword ptr [edx+4]
004F1621: call 004D9E80
004F1626: mov ecx,eax
```
### SmoothFadeProc

```asm
004EEE90: push esi
004EEE91: mov esi,dword ptr [esp+8]
004EEE95: mov al,byte ptr [esi+00000809h]
004EEE9B: test al,al
004EEE9D: je 004EEEBC
004EEE9F: push edi
004EEEA0: mov edi,dword ptr ds:[008580C4h]
004EEEA6: mov ecx,esi
004EEEA8: call 004EED10
004EEEAD: push 1
004EEEAF: call edi
004EEEB1: mov al,byte ptr [esi+00000809h]
004EEEB7: test al,al
004EEEB9: jne 004EEEA6
004EEEBB: pop edi
004EEEBC: xor eax,eax
004EEEBE: pop esi
004EEEBF: ret
004EEEC0: push ebx
004EEEC1: push esi
004EEEC2: mov esi,ecx
004EEEC4: lea eax,[esi+8]
004EEEC7: mov ecx,40h
004EEECC: xor ebx,ebx
004EEECE: mov edi,edi
004EEED0: mov dword ptr [eax+18h],ebx
004EEED3: mov dword ptr [eax],ebx
004EEED5: add eax,20h
004EEED8: dec ecx
004EEED9: jne 004EEED0
004EEEDB: lea eax,[esi+4]
004EEEDE: push eax
004EEEDF: push 4
```

StreamThread0x4F1614 llama Service0x4F1290; a0x4F161E hay llamada indirecta a la ranura Service del canal de streaming. SmoothFadeProc0x4EEEA8 llama Service0x4EED10. Las llamadas indirectas se conservan como límite explícito del grafo; su objetivo de clase se identifica por la fuente y no se atribuye a CAEVehicleAudioEntity.

## Confirmación runtime: crash ABI de ECX

El run fallido165406-602582 pasó19.840pares aislados y después sufrió acceso0xB0 en IsAccInhibitedForLowSpeed+18 conthisECX=0 yreturn4FC2AE. EXE4FC299 movecx,esi;4FC29B call4F4FF0;4FC2A9 call4F4FF0 sinrecargarECX. El original4F4FF0 noescribeECX yret4 en4F5010/4F5015. Se corrigió el hookABI constackArguments1 conservandoECX, sinalterar fórmula. Run165826-989653 pasó960frames/2240calls/32contextos y19840pares0diff sin firstchanceAV. No se atribuye este fallo a workers; es dependencia real de registro volátil del caller EXE hacia el hookDLL.
