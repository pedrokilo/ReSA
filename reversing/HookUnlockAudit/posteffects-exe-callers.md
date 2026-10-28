<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Callers EXE de ImmediateModeRenderStatesStore

Inventario binary_calls.csv: 14 referencias directas (incluye thunk). Se desensamblaron 80 bytes desde cada call manteniendo alineacion; se conservan las dos instrucciones relevantes. En TODOS los14callers la siguiente instruccion es call00700D70: no consumen EAX entre Store y Set. Set comienza00700D70 mov eax,dword ptr ds:[00C97B24h], destruyendo EAX anterior. No se observa contrato especial de preservacion EAX en estos callers directos. La ausencia de usos en este inventario no prueba cobertura de callbacks indirectos o modificaciones externas.

Contrato Store original: diez callbacksGet por [engine+24h], argumentos caller-cleaned, add esp40h en00700D43 y add esp10h en00700D5F, ret en00700D62 (sin argumentos). EAXfinal es resultado del ultimo callback, no el EAXdeentrada.

### Caller 0x006e7760 call 0x006e78c6

```asm
006E78C6: call 00700CC0
006E78CB: call 00700D70
```

### Caller 0x006e7760 call 0x006e7922

```asm
006E7922: call 00700CC0
006E7927: call 00700D70
```

### Caller 0x007011c0 call 0x00701207

```asm
00701207: call 00700CC0
0070120C: call 00700D70
```

### Caller 0x007011c0 call 0x00701277

```asm
00701277: call 00700CC0
0070127C: call 00700D70
```

### Caller 0x00701780 call 0x007017cc

```asm
007017CC: call 00700CC0
007017D1: call 00700D70
```

### Caller 0x00702f00 call 0x00702f00

```asm
00702F00: call 00700CC0
00702F05: call 00700D70
```

### Caller 0x00702f40 call 0x00703028

```asm
00703028: call 00700CC0
0070302D: call 00700D70
```

### Caller 0x007030a0 call 0x0070314e

```asm
0070314E: call 00700CC0
00703153: call 00700D70
```

### Caller 0x007037c0 call 0x0070387b

```asm
0070387B: call 00700CC0
00703880: call 00700D70
```

### Caller 0x007039c0 call 0x007039fa

```asm
007039FA: call 00700CC0
007039FF: call 00700D70
```

### Caller 0x00703f80 call 0x00703f90

```asm
00703F90: call 00700CC0
00703F95: call 00700D70
```

### Caller 0x00704150 call 0x0070415e

```asm
0070415E: call 00700CC0
00704163: call 00700D70
```

### Caller 0x00716380 call 0x007164bf

```asm
007164BF: call 00700CC0
007164C4: call 00700D70
```

### Caller 0x00716c90 call 0x00716d94

```asm
00716D94: call 00700CC0
00716D99: call 00700D70
```
