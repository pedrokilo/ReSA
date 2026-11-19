<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 19/11/2026 (Ciudad de México):** 998 grupos / 12441 funciones reconstruidas; 11781 con evidencia histórica y 660 pendientes de validación en 18 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Funciones originales ejecutadas hasta el menu (sin hooks: solo el original sobre la cascara)

2832 funciones originales distintas (primera entrada). Por region: CRT y bibliotecas (0x820000+): 1839, RenderWare/D3D (0x750000-0x81FFFF): 292, juego (0x401000-0x74FFFF): 701

| n | funcion original | region | llamada desde | quien llama | ms |
|---:|---|---|---|---|---:|
| 1 | `0x00822070` | CRT y bibliotecas | `0x00824588` | original (CRT) | 63 |
| 2 | `0x0082C471` | CRT y bibliotecas | `0x0082463E` | original (CRT) | 63 |
| 3 | `0x0082AC74` | CRT y bibliotecas | `0x00827D2A` | original (CRT) | 63 |
| 4 | `0x00833BA6` | CRT y bibliotecas | `0x0082AC9F` | original (CRT) | 63 |
| 5 | `0x0082871B` | CRT y bibliotecas | `0x00833C30` | original (CRT) | 63 |
| 6 | `0x00824416` | CRT y bibliotecas | `0x00827D53` | original (CRT) | 63 |
| 7 | `0x0082BEDA` | CRT y bibliotecas | `0x00824661` | original (CRT) | 63 |
| 8 | `0x0082C0B9` | CRT y bibliotecas | `0x00824669` | original (CRT) | 63 |
| 9 | `0x00824257` | CRT y bibliotecas | `0x0082C0C8` | original (CRT) | 63 |
| 10 | `0x0082422B` | CRT y bibliotecas | `0x00824266` | original (CRT) | 63 |
| 11 | `0x008241B0` | CRT y bibliotecas | `0x0082423B` | original (CRT) | 63 |
| 12 | `0x0082DBC8` | CRT y bibliotecas | `0x00824685` | original (CRT) | 63 |
| 13 | `0x0082DB26` | CRT y bibliotecas | `0x0082468F` | original (CRT) | 63 |
| 14 | `0x00833494` | CRT y bibliotecas | `0x0082DB3D` | original (CRT) | 63 |
| 15 | `0x00833344` | CRT y bibliotecas | `0x008334A4` | original (CRT) | 63 |
| 16 | `0x0082ADBE` | CRT y bibliotecas | `0x0083335B` | original (CRT) | 63 |
| 17 | `0x0083319E` | CRT y bibliotecas | `0x008333E8` | original (CRT) | 63 |
| 18 | `0x00832F9D` | CRT y bibliotecas | `0x0083331D` | original (CRT) | 63 |
| 19 | `0x00830FB8` | CRT y bibliotecas | `0x00833046` | original (CRT) | 63 |
| 20 | `0x0082D4C0` | CRT y bibliotecas | `0x00831088` | original (CRT) | 63 |
| 21 | `0x0082BB1E` | CRT y bibliotecas | `0x0083306A` | original (CRT) | 63 |
| 22 | `0x0082AAD4` | CRT y bibliotecas | `0x0083312D` | original (CRT) | 63 |
| 23 | `0x0083348B` | CRT y bibliotecas | `0x00833482` | original (CRT) | 63 |
| 24 | `0x0082AD12` | CRT y bibliotecas | `0x00833492` | original (CRT) | 63 |
| 25 | `0x0082D8F3` | CRT y bibliotecas | `0x008246A0` | original (CRT) | 63 |
| 26 | `0x00826330` | CRT y bibliotecas | `0x0082D91E` | original (CRT) | 63 |
| 27 | `0x00826590` | CRT y bibliotecas | `0x0082D974` | original (CRT) | 63 |
| 28 | `0x0082413F` | CRT y bibliotecas | `0x0082D98A` | original (CRT) | 63 |
| 29 | `0x00823B76` | CRT y bibliotecas | `0x008246B1` | original (CRT) | 63 |
| 30 | `0x00821397` | CRT y bibliotecas | `0x00823B81` | original (CRT) | 63 |
| 31 | `0x0082134F` | CRT y bibliotecas | `0x0082139C` | original (CRT) | 63 |
| 32 | `0x00826F04` | CRT y bibliotecas | `0x008213A1` | original (CRT) | 63 |
| 33 | `0x00826EB2` | CRT y bibliotecas | `0x008213AB` | original (CRT) | 63 |
| 34 | `0x00825136` | CRT y bibliotecas | `0x00826EC1` | original (CRT) | 63 |
| 35 | `0x00825104` | CRT y bibliotecas | `0x00825149` | original (CRT) | 63 |
| 36 | `0x00824F86` | CRT y bibliotecas | `0x00825115` | original (CRT) | 63 |
| 37 | `0x00825018` | CRT y bibliotecas | `0x0082512B` | original (CRT) | 63 |
| 38 | `0x00826A72` | CRT y bibliotecas | `0x00823BA1` | original (CRT) | 63 |
| 39 | `0x00821CBE` | CRT y bibliotecas | `0x00823BA1` | original (CRT) | 63 |
| 40 | `0x00823EE5` | CRT y bibliotecas | `0x00823BA1` | original (CRT) | 63 |
| 41 | `0x008289F5` | CRT y bibliotecas | `0x00823BA1` | original (CRT) | 63 |
| 42 | `0x008289C0` | CRT y bibliotecas | `0x00828A34` | original (CRT) | 63 |
| 43 | `0x0082AC54` | CRT y bibliotecas | `0x00823BA1` | original (CRT) | 63 |
| 44 | `0x00821D1E` | CRT y bibliotecas | `0x00823BB6` | original (CRT) | 63 |
| 45 | `0x00821CE6` | CRT y bibliotecas | `0x00821D27` | original (CRT) | 63 |
| 46 | `0x00823B2E` | CRT y bibliotecas | `0x00821CF7` | original (CRT) | 63 |
| 47 | `0x00821C3E` | CRT y bibliotecas | `0x00821D03` | original (CRT) | 63 |
| 48 | `0x00828C4A` | CRT y bibliotecas | `0x00821C4A` | original (CRT) | 63 |
| 49 | `0x00821D18` | CRT y bibliotecas | `0x00821D0F` | original (CRT) | 63 |
| 50 | `0x00823B37` | CRT y bibliotecas | `0x00821D1D` | original (CRT) | 63 |
| 51 | `0x008339CA` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 52 | `0x00848F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 53 | `0x00848F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 54 | `0x00848F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 55 | `0x00848F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 56 | `0x00848FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 57 | `0x00848FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 58 | `0x00848FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 59 | `0x00849010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 60 | `0x00849030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 61 | `0x00849050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 62 | `0x00849070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 63 | `0x00849090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 64 | `0x008490B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 65 | `0x008490D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 66 | `0x008490F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 67 | `0x00849110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 68 | `0x00849130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 69 | `0x00849150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 70 | `0x00849170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 71 | `0x00849190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 72 | `0x008491B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 73 | `0x008491D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 74 | `0x008491F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 75 | `0x00849210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 76 | `0x00849230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 77 | `0x00849250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 78 | `0x00849270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 79 | `0x00849290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 80 | `0x008492B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 81 | `0x008492D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 82 | `0x008492F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 83 | `0x00849310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 84 | `0x00849330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 85 | `0x00849350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 86 | `0x00849370` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 87 | `0x00849390` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 88 | `0x008493B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 89 | `0x008493D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 90 | `0x008493F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 91 | `0x00849410` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 92 | `0x00849430` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 93 | `0x00849450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 94 | `0x00849470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 95 | `0x0040FB60` | juego | `0x0084947A` | original (CRT) | 63 |
| 96 | `0x0040FAF0` | juego | `0x0040FB68` | original (juego) | 63 |
| 97 | `0x00849490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 98 | `0x00821D30` | CRT y bibliotecas | `0x008494A8` | original (CRT) | 63 |
| 99 | `0x00821D7A` | CRT y bibliotecas | `0x00821D72` | original (CRT) | 63 |
| 100 | `0x008494C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 101 | `0x008494E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 102 | `0x00849500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 103 | `0x00849520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 104 | `0x00849540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 105 | `0x00849560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 106 | `0x00849580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 107 | `0x008495A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 108 | `0x008495C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 109 | `0x008495E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 110 | `0x00849600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 111 | `0x00849620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 112 | `0x00849640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 113 | `0x00849660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 114 | `0x00849680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 115 | `0x008496A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 116 | `0x008496C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 117 | `0x008496E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 118 | `0x00849700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 119 | `0x00849720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 120 | `0x00849740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 121 | `0x00849760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 122 | `0x00849780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 123 | `0x008497A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 124 | `0x008497C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 125 | `0x008497E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 126 | `0x00849800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 127 | `0x00849820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 128 | `0x00849840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 129 | `0x00849860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 130 | `0x00849880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 131 | `0x008498A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 132 | `0x008498C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 133 | `0x008498E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 134 | `0x00849900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 135 | `0x00849920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 136 | `0x00849940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 137 | `0x00849960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 138 | `0x00849980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 139 | `0x008499A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 140 | `0x008499C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 141 | `0x008499E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 142 | `0x00849A00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 143 | `0x00849A20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 144 | `0x00849A40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 145 | `0x00849A60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 146 | `0x00849A80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 147 | `0x00849AA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 148 | `0x00849AC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 149 | `0x00849AE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 150 | `0x00849B00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 151 | `0x00849B20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 152 | `0x00849B40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 153 | `0x00849B60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 154 | `0x00849B80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 155 | `0x00849BA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 156 | `0x00849BC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 157 | `0x00849BE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 158 | `0x00849C00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 159 | `0x00849C20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 160 | `0x00849C40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 161 | `0x00849C60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 162 | `0x00849C80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 163 | `0x00849CA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 164 | `0x00849CC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 165 | `0x00849CE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 166 | `0x00849D10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 167 | `0x00849D30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 168 | `0x00849D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 169 | `0x00849D70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 170 | `0x00849D90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 171 | `0x00849DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 172 | `0x00849DD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 173 | `0x00849DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 174 | `0x00849E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 175 | `0x00849E30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 176 | `0x00849E50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 177 | `0x00849E70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 178 | `0x00849E90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 179 | `0x00849EC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 180 | `0x00849EE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 181 | `0x00849F00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 182 | `0x00849F20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 183 | `0x00849F40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 184 | `0x00849F60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 185 | `0x00849F80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 186 | `0x00849FA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 187 | `0x00849FC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 188 | `0x00849FE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 189 | `0x0084A000` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 190 | `0x0084A020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 191 | `0x0084A040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 192 | `0x0084A060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 193 | `0x0084A080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 194 | `0x0084A0A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 195 | `0x0084A0C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 196 | `0x0084A0E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 197 | `0x0084A100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 198 | `0x0084A120` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 199 | `0x0084A140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 200 | `0x0084A160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 201 | `0x0084A180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 202 | `0x0084A1A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 203 | `0x0084A1C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 204 | `0x0084A1E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 205 | `0x0084A200` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 206 | `0x0084A220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 63 |
| 207 | `0x004541E0` | juego | `0x0084A22A` | original (CRT) | 78 |
| 208 | `0x0084A240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 209 | `0x0084A260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 210 | `0x0084A280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 211 | `0x0084A2A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 212 | `0x0084A2C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 213 | `0x0084A2E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 214 | `0x0084A300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 215 | `0x0084A320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 216 | `0x0084A340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 217 | `0x0084A360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 218 | `0x0084A380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 219 | `0x0084A3A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 220 | `0x0084A3C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 221 | `0x0084A3E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 222 | `0x0084A400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 223 | `0x0084A420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 224 | `0x0084A440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 225 | `0x0084A460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 226 | `0x0084A480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 227 | `0x0084A4A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 228 | `0x0084A4C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 229 | `0x0084A4E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 230 | `0x0084A500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 231 | `0x0084A520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 232 | `0x0084A540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 233 | `0x00571920` | juego | `0x0084A54A` | original (CRT) | 78 |
| 234 | `0x0056F210` | juego | `0x0057193C` | original (juego) | 78 |
| 235 | `0x00559AF0` | juego | `0x0056F2C3` | original (juego) | 78 |
| 236 | `0x0084A560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 237 | `0x0084A580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 238 | `0x0084A5A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 239 | `0x0084A5C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 240 | `0x0084A5E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 241 | `0x0084A600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 242 | `0x0084A620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 243 | `0x0084A640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 244 | `0x0084A660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 245 | `0x0084A680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 246 | `0x0084A6A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 247 | `0x0084A6C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 248 | `0x0084A6E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 249 | `0x0084A700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 250 | `0x0084A720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 251 | `0x0084A740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 252 | `0x0046B260` | juego | `0x00823BCF` | original (CRT) | 78 |
| 253 | `0x0046A8C0` | juego | `0x0046B268` | original (juego) | 78 |
| 254 | `0x0084A750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 255 | `0x0084A780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 256 | `0x0084A7B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 257 | `0x00727230` | juego | `0x00821D57` | original (CRT) | 78 |
| 258 | `0x0084A7E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 259 | `0x0084A800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 260 | `0x0084A820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 261 | `0x0084A840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 262 | `0x0084A860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 263 | `0x0084A880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 264 | `0x0084A8A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 265 | `0x0084A8C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 266 | `0x0084A8E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 267 | `0x0084A900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 268 | `0x0084A920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 269 | `0x0084A940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 270 | `0x0084A960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 271 | `0x0084A980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 272 | `0x0084A9A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 273 | `0x0084A9C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 274 | `0x0084A9E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 275 | `0x0084AA00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 276 | `0x0084AA20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 277 | `0x0084AA40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 278 | `0x0084AA60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 279 | `0x0084AA80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 280 | `0x0084AAA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 281 | `0x0084AAC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 282 | `0x0084AAE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 283 | `0x0084AB00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 284 | `0x0084AB20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 285 | `0x0084AB40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 286 | `0x0084AB60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 287 | `0x0084AB80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 288 | `0x0084ABA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 289 | `0x0084ABC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 290 | `0x0084ABE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 291 | `0x0084AC00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 292 | `0x0084AC20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 293 | `0x0084AC40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 294 | `0x0084AC60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 295 | `0x0084AC80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 296 | `0x0084ACA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 297 | `0x0084ACC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 298 | `0x0084ACE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 299 | `0x0084AD00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 300 | `0x0084AD20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 301 | `0x0084AD40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 302 | `0x0084AD60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 303 | `0x0084AD80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 304 | `0x0084ADA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 305 | `0x0084ADC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 306 | `0x0084ADE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 307 | `0x0084AE00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 308 | `0x0084AE20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 309 | `0x0084AE40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 310 | `0x0084AE60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 311 | `0x0084AE80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 312 | `0x0084AEA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 313 | `0x0084AEC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 314 | `0x0084AEE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 315 | `0x0084AF00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 316 | `0x0084AF20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 317 | `0x0084AF40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 318 | `0x0084AF60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 319 | `0x0084AF80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 320 | `0x0084AFA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 321 | `0x0084AFC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 322 | `0x0084AFE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 323 | `0x0084B000` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 324 | `0x0084B020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 325 | `0x0084B040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 326 | `0x0084B060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 327 | `0x0084B080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 328 | `0x0084B0A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 329 | `0x0084B0C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 330 | `0x0084B0E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 331 | `0x0084B100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 332 | `0x0084B120` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 333 | `0x0084B140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 334 | `0x0084B160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 335 | `0x0084B180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 336 | `0x0084B1A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 337 | `0x0084B1C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 338 | `0x0084B1E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 339 | `0x0084B200` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 340 | `0x0084B220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 341 | `0x0084B240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 342 | `0x0084B260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 343 | `0x0084B280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 344 | `0x0084B2A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 345 | `0x005A8020` | juego | `0x00823BCF` | original (CRT) | 78 |
| 346 | `0x0084B2B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 347 | `0x0084B2D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 348 | `0x0084B2F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 349 | `0x0084B310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 350 | `0x0084B330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 351 | `0x0084B350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 352 | `0x0084B370` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 353 | `0x0084B390` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 354 | `0x0084B3B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 355 | `0x0084B3D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 356 | `0x0084B3F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 357 | `0x0084B410` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 358 | `0x0084B430` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 359 | `0x0084B450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 360 | `0x0084B470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 361 | `0x0084B490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 362 | `0x0084B4B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 363 | `0x0084B4D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 364 | `0x0084B4F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 365 | `0x0084B510` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 366 | `0x0084B530` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 367 | `0x0084B550` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 368 | `0x0084B570` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 369 | `0x0084B590` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 370 | `0x0084B5B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 371 | `0x0084B5D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 372 | `0x0049E620` | juego | `0x0084B5DA` | original (CRT) | 78 |
| 373 | `0x004A8DD0` | juego | `0x0049E62B` | original (juego) | 78 |
| 374 | `0x0084B5F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 375 | `0x0084B610` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 376 | `0x0084B630` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 377 | `0x0084B650` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 378 | `0x004A9470` | juego | `0x0084B65A` | original (CRT) | 78 |
| 379 | `0x004A9F70` | juego | `0x004A94BC` | original (juego) | 78 |
| 380 | `0x004A9C10` | juego | `0x004A94C7` | original (juego) | 78 |
| 381 | `0x0084B670` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 382 | `0x0084B690` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 383 | `0x0084B6B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 384 | `0x0084B6D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 385 | `0x0084B6F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 386 | `0x0084B710` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 387 | `0x0084B730` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 388 | `0x0084B750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 389 | `0x0084B770` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 390 | `0x0084B790` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 391 | `0x0084B7B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 392 | `0x0084B7D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 393 | `0x0084B7F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 394 | `0x0084B810` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 395 | `0x0084B830` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 396 | `0x0084B850` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 397 | `0x0084B870` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 398 | `0x0084B890` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 399 | `0x0084B8B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 400 | `0x0084B8D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 401 | `0x0084B8F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 402 | `0x0084B910` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 403 | `0x0084B930` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 404 | `0x0084B950` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 405 | `0x0084B970` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 406 | `0x0084B990` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 407 | `0x0084B9B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 408 | `0x0084B9D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 409 | `0x0084B9F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 410 | `0x0084BA10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 411 | `0x0084BA30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 412 | `0x0084BA60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 413 | `0x004AC350` | juego | `0x00821D57` | original (CRT) | 78 |
| 414 | `0x0084BA90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 415 | `0x0084BAB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 416 | `0x0084BAD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 417 | `0x0084BAF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 418 | `0x0084BB10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 419 | `0x0084BB30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 420 | `0x0084BB50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 421 | `0x0084BB70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 422 | `0x0084BB90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 423 | `0x0084BBB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 424 | `0x0084BBD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 425 | `0x0084BBF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 426 | `0x004C5CB0` | juego | `0x0084BBFA` | original (CRT) | 78 |
| 427 | `0x004C5540` | juego | `0x00821D57` | original (CRT) | 78 |
| 428 | `0x004C4A60` | juego | `0x004C5548` | original (juego) | 78 |
| 429 | `0x0084BC10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 430 | `0x004C5D10` | juego | `0x0084BC1A` | original (CRT) | 78 |
| 431 | `0x004C55A0` | juego | `0x00821D57` | original (CRT) | 78 |
| 432 | `0x0084BC30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 433 | `0x004C5D70` | juego | `0x0084BC3A` | original (CRT) | 78 |
| 434 | `0x004C55E0` | juego | `0x00821D57` | original (CRT) | 78 |
| 435 | `0x0084BC50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 436 | `0x004C5DD0` | juego | `0x0084BC5A` | original (CRT) | 78 |
| 437 | `0x004C5640` | juego | `0x00821D57` | original (CRT) | 78 |
| 438 | `0x0084BC70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 439 | `0x004C5E30` | juego | `0x0084BC7A` | original (CRT) | 78 |
| 440 | `0x004C5690` | juego | `0x00821D57` | original (CRT) | 78 |
| 441 | `0x0084BC90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 442 | `0x004C5E90` | juego | `0x0084BC9A` | original (CRT) | 78 |
| 443 | `0x004C5760` | juego | `0x00821D57` | original (CRT) | 78 |
| 444 | `0x0084BCB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 445 | `0x004C5EF0` | juego | `0x0084BCBA` | original (CRT) | 78 |
| 446 | `0x004C56F0` | juego | `0x00821D57` | original (CRT) | 78 |
| 447 | `0x0084BCD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 448 | `0x004C5F50` | juego | `0x0084BCDA` | original (CRT) | 78 |
| 449 | `0x004C75E0` | juego | `0x00821D57` | original (CRT) | 78 |
| 450 | `0x00824269` | CRT y bibliotecas | `0x00821C77` | original (CRT) | 78 |
| 451 | `0x0084BCF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 452 | `0x004C67D0` | juego | `0x0084BCFA` | original (CRT) | 78 |
| 453 | `0x004C57A0` | juego | `0x00821D57` | original (CRT) | 78 |
| 454 | `0x0084BD10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 455 | `0x0084BD30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 456 | `0x0084BD50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 457 | `0x0084BD70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 458 | `0x0084BD90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 459 | `0x0084BDB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 460 | `0x0084BDD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 461 | `0x0084BDF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 462 | `0x0084BE10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 463 | `0x0084BE30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 464 | `0x0084BE50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 465 | `0x0084BE70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 466 | `0x0084BE90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 467 | `0x0084BEB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 468 | `0x0084BED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 469 | `0x0084BEF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 470 | `0x0084BF10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 471 | `0x0084BF30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 472 | `0x0084BF50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 473 | `0x0084BF70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 474 | `0x0084BF90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 475 | `0x0084BFB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 476 | `0x0084BFD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 477 | `0x004CF270` | juego | `0x00821D57` | original (CRT) | 78 |
| 478 | `0x0084C000` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 479 | `0x0084C020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 480 | `0x0084C040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 481 | `0x0084C060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 482 | `0x0084C080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 483 | `0x0084C0A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 484 | `0x0084C0C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 485 | `0x0084C0E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 486 | `0x0084C100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 487 | `0x0084C120` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 488 | `0x0084C140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 489 | `0x0084C160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 490 | `0x0084C180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 491 | `0x0084C1A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 492 | `0x0084C1B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 493 | `0x0084C1D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 494 | `0x0084C1F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 495 | `0x0084C210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 496 | `0x0084C230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 497 | `0x0084C250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 498 | `0x0084C270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 499 | `0x004D83E0` | juego | `0x0084C27A` | original (CRT) | 78 |
| 500 | `0x004F11D0` | juego | `0x004D83EF` | original (juego) | 78 |
| 501 | `0x0084C290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 502 | `0x004EE9A0` | juego | `0x0084C29A` | original (CRT) | 78 |
| 503 | `0x0084C2B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 504 | `0x0084C2D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 505 | `0x0084C2F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 506 | `0x0084C310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 507 | `0x0084C330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 508 | `0x0084C350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 509 | `0x0084C370` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 510 | `0x0084C390` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 511 | `0x0084C3B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 512 | `0x0084C3D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 513 | `0x0084C3F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 514 | `0x0084C410` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 515 | `0x0084C430` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 516 | `0x0084C450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 517 | `0x0084C470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 518 | `0x0084C490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 519 | `0x0084C4B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 520 | `0x0084C4D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 521 | `0x0084C4F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 522 | `0x0084C500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 523 | `0x004F1750` | juego | `0x0084C50A` | original (CRT) | 78 |
| 524 | `0x0084C520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 525 | `0x0084C540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 526 | `0x0084C560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 527 | `0x0084C580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 528 | `0x0084C5A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 529 | `0x0084C5C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 530 | `0x0084C5E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 531 | `0x0084C600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 532 | `0x0084C620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 533 | `0x0084C640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 534 | `0x0084C660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 535 | `0x0084C680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 536 | `0x0084C6A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 537 | `0x0084C6C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 538 | `0x0084C6E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 539 | `0x0084C700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 540 | `0x0084C720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 541 | `0x0084C740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 542 | `0x0084C760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 543 | `0x0084C780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 544 | `0x0084C7A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 545 | `0x0084C7C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 546 | `0x0084C7E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 547 | `0x0084C800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 548 | `0x0084C820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 549 | `0x0084C840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 550 | `0x0084C860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 551 | `0x0084C880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 552 | `0x0084C8A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 553 | `0x0084C8C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 554 | `0x0084C8E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 555 | `0x0084C900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 556 | `0x0084C920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 557 | `0x0084C940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 558 | `0x0084C960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 559 | `0x0084C980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 560 | `0x0084C9A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 561 | `0x0084C9D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 562 | `0x0084C9F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 563 | `0x0084CA10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 564 | `0x0084CA30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 565 | `0x0084CA50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 566 | `0x0084CA70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 567 | `0x0084CA90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 568 | `0x0084CAB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 569 | `0x0084CAD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 570 | `0x0084CAF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 571 | `0x0084CB10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 572 | `0x0084CB30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 573 | `0x0084CB50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 574 | `0x0084CB70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 575 | `0x0084CB90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 576 | `0x0084CBB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 577 | `0x0084CBD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 578 | `0x0084CBF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 579 | `0x0084CC10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 580 | `0x0084CC30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 581 | `0x0084CC40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 582 | `0x0084CC60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 583 | `0x0084CC80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 584 | `0x0084CCA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 585 | `0x0084CCC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 586 | `0x0084CCE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 587 | `0x0084CD00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 588 | `0x0084CD20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 589 | `0x004EFD30` | juego | `0x0084CD2A` | original (CRT) | 78 |
| 590 | `0x0084CD40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 591 | `0x0084CD60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 592 | `0x0084CD70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 593 | `0x0084CD90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 594 | `0x0084CDB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 595 | `0x0084CDD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 596 | `0x0084CDF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 597 | `0x0084CE10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 598 | `0x0084CE30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 599 | `0x0084CE50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 600 | `0x0084CE70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 601 | `0x0084CE90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 602 | `0x0084CEB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 603 | `0x0084CED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 604 | `0x0084CEF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 605 | `0x0084CF10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 606 | `0x0084CF20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 607 | `0x0084CF40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 608 | `0x0084CF60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 609 | `0x0084CF80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 610 | `0x0084CFA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 611 | `0x0084CFC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 612 | `0x0084CFE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 613 | `0x0084CFF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 614 | `0x0084D010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 615 | `0x0084D030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 616 | `0x0084D050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 617 | `0x0084D070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 618 | `0x0084D090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 619 | `0x0084D0B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 620 | `0x0084D0D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 621 | `0x0084D0F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 622 | `0x0084D110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 623 | `0x0084D130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 624 | `0x0084D150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 625 | `0x0084D170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 626 | `0x0084D190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 627 | `0x0084D1B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 628 | `0x0084D1D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 629 | `0x0084D1F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 630 | `0x0084D210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 631 | `0x0084D230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 632 | `0x0084D250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 633 | `0x0084D270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 634 | `0x0084D290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 635 | `0x0084D2B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 636 | `0x0084D2D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 637 | `0x0084D2F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 638 | `0x0084D310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 639 | `0x0084D330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 640 | `0x0084D350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 641 | `0x0084D370` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 642 | `0x0084D390` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 643 | `0x0084D3B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 644 | `0x0084D3D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 645 | `0x0084D3F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 646 | `0x0084D410` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 647 | `0x0084D430` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 648 | `0x0084D450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 649 | `0x0084D470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 650 | `0x0084D490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 651 | `0x0084D4B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 652 | `0x0084D4D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 653 | `0x004F63B0` | juego | `0x0084D4DA` | original (CRT) | 78 |
| 654 | `0x0084D4F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 655 | `0x0084D510` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 656 | `0x0084D530` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 657 | `0x0084D550` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 658 | `0x0084D570` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 659 | `0x0084D590` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 660 | `0x0084D5B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 661 | `0x0084D5D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 662 | `0x00507670` | juego | `0x0084D5DA` | original (CRT) | 78 |
| 663 | `0x005074D0` | juego | `0x005076F2` | original (juego) | 78 |
| 664 | `0x004E6070` | juego | `0x00507712` | original (juego) | 78 |
| 665 | `0x004E4F10` | juego | `0x004E6077` | original (juego) | 78 |
| 666 | `0x0084D5F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 667 | `0x0084D610` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 668 | `0x0084D630` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 669 | `0x0084D650` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 670 | `0x0084D670` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 671 | `0x0084D690` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 672 | `0x0084D6B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 673 | `0x0084D6D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 674 | `0x00517740` | juego | `0x00821D57` | original (CRT) | 78 |
| 675 | `0x00517330` | juego | `0x00517753` | original (juego) | 78 |
| 676 | `0x0050D860` | juego | `0x00517753` | original (juego) | 78 |
| 677 | `0x00821B1E` | CRT y bibliotecas | `0x0050D8A0` | original (juego) | 78 |
| 678 | `0x00827B3D` | CRT y bibliotecas | `0x00821B23` | original (CRT) | 78 |
| 679 | `0x0084D700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 680 | `0x0051A450` | juego | `0x0084D70A` | original (CRT) | 78 |
| 681 | `0x0054F1E0` | juego | `0x0051A474` | original (juego) | 78 |
| 682 | `0x00517730` | juego | `0x00821D57` | original (CRT) | 78 |
| 683 | `0x0050E490` | juego | `0x00517738` | original (juego) | 78 |
| 684 | `0x0050A160` | juego | `0x0050E4A9` | original (juego) | 78 |
| 685 | `0x0053FB70` | juego | `0x0050A1E8` | original (juego) | 78 |
| 686 | `0x0050D2D0` | juego | `0x0051A546` | original (juego) | 78 |
| 687 | `0x0084D720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 688 | `0x0050E6D0` | juego | `0x0084D72A` | original (CRT) | 78 |
| 689 | `0x0084D740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 690 | `0x0084D750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 691 | `0x0084D770` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 692 | `0x0050E180` | juego | `0x005173F5` | original (juego) | 78 |
| 693 | `0x0084D7A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 694 | `0x0084D7C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 695 | `0x0084D7E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 696 | `0x0084D800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 697 | `0x0084D820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 698 | `0x0084D840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 699 | `0x0084D860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 700 | `0x0084D880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 701 | `0x0084D8A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 702 | `0x0084D8C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 703 | `0x0084D8E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 704 | `0x0084D900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 705 | `0x0084D920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 706 | `0x00531EE0` | juego | `0x00823BCF` | original (CRT) | 78 |
| 707 | `0x00530500` | juego | `0x00531F0A` | original (juego) | 78 |
| 708 | `0x0052FD70` | juego | `0x00530519` | original (juego) | 78 |
| 709 | `0x0052F5F0` | juego | `0x0052FDA7` | original (juego) | 78 |
| 710 | `0x00530640` | juego | `0x00531F11` | original (juego) | 78 |
| 711 | `0x00530490` | juego | `0x00530653` | original (juego) | 78 |
| 712 | `0x0052D260` | juego | `0x00531F18` | original (juego) | 78 |
| 713 | `0x00718600` | juego | `0x0052D276` | original (juego) | 78 |
| 714 | `0x0069DB70` | juego | `0x0052D289` | original (juego) | 78 |
| 715 | `0x0084D930` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 716 | `0x0084D950` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 717 | `0x0084D970` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 718 | `0x0084D990` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 719 | `0x0084D9B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 720 | `0x0084D9D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 721 | `0x0084D9F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 722 | `0x0084DA10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 723 | `0x0084DA30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 724 | `0x0084DA50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 725 | `0x0084DA70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 726 | `0x0084DA90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 727 | `0x0084DAB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 728 | `0x0084DAD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 729 | `0x0084DAF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 730 | `0x0084DB10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 731 | `0x0084DB30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 732 | `0x0084DB50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 733 | `0x0084DB70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 734 | `0x0084DB90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 735 | `0x0084DBB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 736 | `0x0084DBD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 737 | `0x0084DBF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 738 | `0x0084DC10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 739 | `0x0084DC30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 740 | `0x0084DC50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 741 | `0x0084DC70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 742 | `0x0084DC90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 743 | `0x0084DCB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 744 | `0x0084DCD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 745 | `0x0084DCF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 746 | `0x0084DD10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 747 | `0x0084DD30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 748 | `0x0084DD50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 749 | `0x0084DD70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 750 | `0x0084DD90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 751 | `0x0084DDB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 752 | `0x0084DDD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 753 | `0x0084DDF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 754 | `0x0084DE10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 755 | `0x0084DE30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 756 | `0x0084DE50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 757 | `0x0084DE70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 758 | `0x00539DA0` | juego | `0x0084DE7A` | original (CRT) | 78 |
| 759 | `0x00539D90` | juego | `0x00821D57` | original (CRT) | 78 |
| 760 | `0x00538B30` | juego | `0x00539D95` | original (juego) | 78 |
| 761 | `0x00538BC0` | juego | `0x00539DBE` | original (juego) | 78 |
| 762 | `0x0084DE90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 763 | `0x0084DEB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 764 | `0x0084DED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 765 | `0x0084DEF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 766 | `0x0084DF10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 767 | `0x0084DF30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 768 | `0x0084DF50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 769 | `0x0084DF70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 770 | `0x0084DF90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 771 | `0x0084DFB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 772 | `0x0084DFD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 773 | `0x0084DFF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 774 | `0x0084E010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 775 | `0x0053CFA0` | juego | `0x0084E01A` | original (CRT) | 78 |
| 776 | `0x0084E030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 777 | `0x0084E050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 778 | `0x0084E070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 779 | `0x0084E090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 780 | `0x0084E0B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 781 | `0x0084E0D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 782 | `0x0084E0F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 783 | `0x0084E100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 784 | `0x0084E110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 785 | `0x0084E130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 786 | `0x0084E150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 787 | `0x0084E170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 788 | `0x0084E190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 789 | `0x0084E1B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 790 | `0x0084E1D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 791 | `0x0084E1F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 792 | `0x00541A70` | juego | `0x00541D8C` | original (juego) | 78 |
| 793 | `0x0053EF80` | juego | `0x00541A78` | original (juego) | 78 |
| 794 | `0x0053F090` | juego | `0x00541AA8` | original (juego) | 78 |
| 795 | `0x0084E220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 796 | `0x0084E240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 797 | `0x0084E260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 798 | `0x0084E280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 799 | `0x0084E2A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 800 | `0x0084E2C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 801 | `0x0084E2E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 802 | `0x0059AED0` | juego | `0x0084E2EF` | original (CRT) | 78 |
| 803 | `0x0084E300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 804 | `0x0054F440` | juego | `0x0084E30A` | original (CRT) | 78 |
| 805 | `0x0084E320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 806 | `0x0084E340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 807 | `0x0084E360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 808 | `0x0084E380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 809 | `0x0084E3A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 810 | `0x0084E3C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 811 | `0x0084E3E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 812 | `0x0084E400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 813 | `0x0084E420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 814 | `0x0084E440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 815 | `0x0084E460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 816 | `0x0084E480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 817 | `0x0084E4A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 818 | `0x0084E4C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 819 | `0x0084E4E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 820 | `0x0084E500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 821 | `0x0084E520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 822 | `0x0084E540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 823 | `0x0084E560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 824 | `0x0084E580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 825 | `0x0084E5A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 826 | `0x0084E5C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 827 | `0x0084E5E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 828 | `0x0084E600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 829 | `0x0084E620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 830 | `0x0084E640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 831 | `0x0084E660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 832 | `0x0084E680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 833 | `0x0084E6A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 834 | `0x0084E6C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 835 | `0x0084E6E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 836 | `0x0084E6F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 837 | `0x0084E710` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 838 | `0x0084E730` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 839 | `0x0084E750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 840 | `0x0084E770` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 841 | `0x0084E790` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 842 | `0x0084E7B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 843 | `0x007170C0` | juego | `0x0084E7C5` | original (CRT) | 78 |
| 844 | `0x0084E7E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 845 | `0x0084E7F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 846 | `0x0084E800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 847 | `0x0084E820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 848 | `0x0084E840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 849 | `0x0084E860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 850 | `0x0084E880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 851 | `0x0084E8A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 852 | `0x0084E8C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 853 | `0x0084E8E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 854 | `0x0084E900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 855 | `0x0084E920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 856 | `0x0084E940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 857 | `0x0084E960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 858 | `0x0084E980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 859 | `0x0084E9B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 860 | `0x0084E9E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 861 | `0x0084EA10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 862 | `0x0084EA40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 863 | `0x0084EA50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 864 | `0x0084EA60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 865 | `0x0084EA80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 866 | `0x0084EAA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 867 | `0x0084EAC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 868 | `0x0084EAE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 869 | `0x0084EB00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 870 | `0x0084EB20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 871 | `0x0084EB40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 872 | `0x0084EB60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 873 | `0x0084EB80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 874 | `0x0084EBA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 875 | `0x0084EBC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 876 | `0x0084EBE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 877 | `0x0084EC00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 878 | `0x0084EC20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 879 | `0x0084EC40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 880 | `0x0084EC60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 881 | `0x0084EC80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 882 | `0x0084ECA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 883 | `0x0084ECC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 884 | `0x0084ECE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 885 | `0x0084ED00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 886 | `0x0084ED20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 887 | `0x0084ED40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 888 | `0x0084ED60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 889 | `0x0084ED80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 890 | `0x0084EDA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 891 | `0x0084EDC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 892 | `0x0084EDE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 893 | `0x0084EE00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 894 | `0x0084EE20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 895 | `0x0084EE40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 896 | `0x00574350` | juego | `0x0084EE4A` | original (CRT) | 78 |
| 897 | `0x00573AE0` | juego | `0x005743B5` | original (juego) | 78 |
| 898 | `0x0049EA40` | juego | `0x00573B83` | original (juego) | 78 |
| 899 | `0x00747200` | juego | `0x00573B9F` | original (juego) | 78 |
| 900 | `0x00506DE0` | juego | `0x00573B3A` | original (juego) | 78 |
| 901 | `0x004D9530` | juego | `0x00506E01` | original (juego) | 78 |
| 902 | `0x00506E10` | juego | `0x00573B4D` | original (juego) | 78 |
| 903 | `0x004D9540` | juego | `0x00506E31` | original (juego) | 78 |
| 904 | `0x00506F90` | juego | `0x00573B5F` | original (juego) | 78 |
| 905 | `0x004E9DB0` | juego | `0x00573B5F` | original (juego) | 78 |
| 906 | `0x00506F80` | juego | `0x0057443A` | original (juego) | 78 |
| 907 | `0x004E82E0` | juego | `0x0057443A` | original (juego) | 78 |
| 908 | `0x004D9C10` | juego | `0x0057444C` | original (juego) | 78 |
| 909 | `0x00821B40` | CRT y bibliotecas | `0x004D9C42` | original (juego) | 78 |
| 910 | `0x0084EE60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 911 | `0x0084EE80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 912 | `0x0084EEA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 913 | `0x0084EEC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 914 | `0x0084EEE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 915 | `0x0084EF00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 916 | `0x0084EF20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 917 | `0x0084EF40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 918 | `0x0084EF60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 919 | `0x0084EF80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 920 | `0x0084EFA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 921 | `0x0084EFC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 922 | `0x0084EFE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 923 | `0x0084F000` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 924 | `0x0084F020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 925 | `0x0084F040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 926 | `0x0084F060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 927 | `0x0084F080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 928 | `0x0084F0A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 929 | `0x0084F0D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 930 | `0x0084F100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 931 | `0x0084F120` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 932 | `0x0084F140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 933 | `0x0084F160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 934 | `0x0084F180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 935 | `0x0084F1A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 936 | `0x0084F1C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 937 | `0x0084F1F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 938 | `0x0058FDA0` | juego | `0x0084F1FA` | original (CRT) | 78 |
| 939 | `0x0084F210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 940 | `0x0084F230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 941 | `0x0084F250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 942 | `0x0084F270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 943 | `0x0084F290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 944 | `0x0084F2B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 945 | `0x0084F2D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 946 | `0x0084F300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 947 | `0x00591260` | juego | `0x0084F30A` | original (CRT) | 78 |
| 948 | `0x00590DE0` | juego | `0x00821D57` | original (CRT) | 78 |
| 949 | `0x00590E00` | juego | `0x00821D57` | original (CRT) | 78 |
| 950 | `0x004A8DB0` | juego | `0x00590E08` | original (juego) | 78 |
| 951 | `0x0084F320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 952 | `0x00590E20` | juego | `0x00821D57` | original (CRT) | 78 |
| 953 | `0x0084F350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 954 | `0x00590EC0` | juego | `0x00821D57` | original (CRT) | 78 |
| 955 | `0x0084F380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 956 | `0x0084F3A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 957 | `0x0084F3C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 958 | `0x0084F3E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 959 | `0x0084F400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 960 | `0x0084F420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 961 | `0x0084F440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 962 | `0x0084F460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 963 | `0x0084F480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 964 | `0x0084F4A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 965 | `0x0084F4C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 966 | `0x0084F4E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 967 | `0x0084F500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 968 | `0x0084F520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 969 | `0x0084F540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 970 | `0x0084F560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 971 | `0x0084F580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 972 | `0x0084F5A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 973 | `0x0084F5C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 974 | `0x0084F5E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 975 | `0x0084F600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 976 | `0x0084F620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 977 | `0x0084F640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 978 | `0x0084F660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 979 | `0x0084F680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 980 | `0x0084F6A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 981 | `0x0084F6C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 982 | `0x0084F6E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 983 | `0x0084F700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 984 | `0x0084F720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 985 | `0x0084F740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 986 | `0x005984C0` | juego | `0x0084F74A` | original (CRT) | 78 |
| 987 | `0x005921D0` | juego | `0x00821D57` | original (CRT) | 78 |
| 988 | `0x00597FE0` | juego | `0x00821D57` | original (CRT) | 78 |
| 989 | `0x0084F760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 990 | `0x0084F780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 991 | `0x0084F7A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 992 | `0x0084F7C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 993 | `0x0084F7E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 994 | `0x0084F800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 995 | `0x0084F820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 996 | `0x0084F840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 997 | `0x0084F860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 998 | `0x0084F880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 999 | `0x0084F8A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1000 | `0x0084F8C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1001 | `0x0084F8E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1002 | `0x0084F900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1003 | `0x0084F920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1004 | `0x0084F940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1005 | `0x0084F960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1006 | `0x0084F980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1007 | `0x0084F9A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1008 | `0x0084F9C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1009 | `0x0084F9E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1010 | `0x0084FA00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1011 | `0x0084FA20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1012 | `0x0084FA40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1013 | `0x0084FA60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1014 | `0x0084FA80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1015 | `0x0059E620` | juego | `0x0084FA8A` | original (CRT) | 78 |
| 1016 | `0x0084FAA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1017 | `0x0084FAC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1018 | `0x0084FAE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1019 | `0x0084FB00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1020 | `0x0084FB20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1021 | `0x0084FB40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1022 | `0x0084FB60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1023 | `0x0084FB80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1024 | `0x0084FBA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1025 | `0x0084FBC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1026 | `0x0084FBE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1027 | `0x0084FC00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1028 | `0x0084FC20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1029 | `0x0084FC40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1030 | `0x0084FC60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1031 | `0x0084FC80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1032 | `0x0084FCA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1033 | `0x0084FCC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1034 | `0x0084FCE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1035 | `0x0084FD00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1036 | `0x0084FD20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1037 | `0x0084FD40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1038 | `0x0084FD60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1039 | `0x0084FD80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1040 | `0x0084FDA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1041 | `0x0084FDC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1042 | `0x005A3E20` | juego | `0x0084FDCA` | original (CRT) | 78 |
| 1043 | `0x005A2E90` | juego | `0x00821D57` | original (CRT) | 78 |
| 1044 | `0x005A3070` | juego | `0x00821D57` | original (CRT) | 78 |
| 1045 | `0x0084FDE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1046 | `0x0084FE00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1047 | `0x0084FE20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1048 | `0x005A7570` | juego | `0x0084FE2A` | original (CRT) | 78 |
| 1049 | `0x0084FE40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1050 | `0x00532290` | juego | `0x0084FE4A` | original (CRT) | 78 |
| 1051 | `0x0084FE60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1052 | `0x0084FE70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1053 | `0x0084FE90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1054 | `0x0084FEB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1055 | `0x0084FED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1056 | `0x0084FEF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1057 | `0x0084FF10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1058 | `0x0084FF30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1059 | `0x0084FF60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1060 | `0x0084FF80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1061 | `0x0084FFA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1062 | `0x0084FFC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1063 | `0x0084FFE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1064 | `0x00850000` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1065 | `0x00850020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1066 | `0x00850040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1067 | `0x004CDE70` | juego | `0x0085004A` | original (CRT) | 78 |
| 1068 | `0x00850060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1069 | `0x00850080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1070 | `0x008500A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1071 | `0x008500C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1072 | `0x008500E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1073 | `0x00850100` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1074 | `0x00850120` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1075 | `0x00850140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1076 | `0x00850160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1077 | `0x00850180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1078 | `0x008501A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1079 | `0x008501C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1080 | `0x008501E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1081 | `0x00850200` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1082 | `0x00850220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1083 | `0x00850240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1084 | `0x00850260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1085 | `0x00850280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1086 | `0x008502A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1087 | `0x008502C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1088 | `0x008502E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1089 | `0x00850300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1090 | `0x00850320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1091 | `0x00850340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1092 | `0x00850360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1093 | `0x00850380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1094 | `0x008503A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1095 | `0x008503C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1096 | `0x008503E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1097 | `0x00850400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1098 | `0x00850420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1099 | `0x00850440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1100 | `0x00850460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1101 | `0x00850480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1102 | `0x008504A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1103 | `0x008504C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1104 | `0x008504E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1105 | `0x00850500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1106 | `0x00850520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1107 | `0x00850540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1108 | `0x00850560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1109 | `0x00850580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1110 | `0x008505A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1111 | `0x008505C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1112 | `0x008505E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1113 | `0x00850600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1114 | `0x00850620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1115 | `0x00850640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1116 | `0x00850660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1117 | `0x00850680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1118 | `0x008506A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1119 | `0x008506C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1120 | `0x008506E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1121 | `0x00850700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1122 | `0x00850720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1123 | `0x00850740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1124 | `0x00850760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1125 | `0x00850780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1126 | `0x008507A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1127 | `0x008507C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1128 | `0x008507E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1129 | `0x00850800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1130 | `0x00850820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1131 | `0x00850840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1132 | `0x00850860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1133 | `0x00850880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1134 | `0x008508A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1135 | `0x008508C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1136 | `0x008508F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1137 | `0x00850910` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1138 | `0x00850940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1139 | `0x00850970` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1140 | `0x008509A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1141 | `0x008509C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1142 | `0x008509E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1143 | `0x00850A00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1144 | `0x00850A20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1145 | `0x00850A40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1146 | `0x00850A60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1147 | `0x00850A80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1148 | `0x00850AA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1149 | `0x00850AC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1150 | `0x00850AE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1151 | `0x00850B00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1152 | `0x00850B20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1153 | `0x00850B40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1154 | `0x00850B60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1155 | `0x00850B80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1156 | `0x00850BA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1157 | `0x00850BC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1158 | `0x00850BE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1159 | `0x00850C00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1160 | `0x00850C20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1161 | `0x00850C40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1162 | `0x00850C60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1163 | `0x00850C80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1164 | `0x00850CA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1165 | `0x00850CC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1166 | `0x00850CE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1167 | `0x00850D10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1168 | `0x00850D30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1169 | `0x00850D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1170 | `0x00850D70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1171 | `0x00850D90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1172 | `0x00850DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1173 | `0x00850DD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1174 | `0x00850DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1175 | `0x00850E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1176 | `0x00850E30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1177 | `0x00850E50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1178 | `0x00850E70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1179 | `0x00850E90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1180 | `0x00850EB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1181 | `0x00850ED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1182 | `0x00850EF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1183 | `0x00850F10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1184 | `0x00850F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1185 | `0x00850F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1186 | `0x00850F60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1187 | `0x00850F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1188 | `0x00850F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1189 | `0x00850FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1190 | `0x00850FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1191 | `0x00850FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1192 | `0x00851010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1193 | `0x00851030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1194 | `0x00851050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1195 | `0x00851070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1196 | `0x00851090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1197 | `0x008510B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1198 | `0x008510D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1199 | `0x008510F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1200 | `0x00851110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1201 | `0x00851130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1202 | `0x00851150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1203 | `0x00851170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1204 | `0x00851190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1205 | `0x008511B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1206 | `0x008511D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1207 | `0x008511F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1208 | `0x00851210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1209 | `0x00851230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1210 | `0x00851250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1211 | `0x00851270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1212 | `0x005F6450` | juego | `0x0085127A` | original (CRT) | 78 |
| 1213 | `0x00851290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1214 | `0x005F6480` | juego | `0x0085129A` | original (CRT) | 78 |
| 1215 | `0x008512B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1216 | `0x005F64B0` | juego | `0x008512BA` | original (CRT) | 78 |
| 1217 | `0x008512D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1218 | `0x005F64E0` | juego | `0x008512DA` | original (CRT) | 78 |
| 1219 | `0x008512F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1220 | `0x005F6510` | juego | `0x008512FA` | original (CRT) | 78 |
| 1221 | `0x00851310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1222 | `0x005F6540` | juego | `0x0085131A` | original (CRT) | 78 |
| 1223 | `0x00851330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1224 | `0x005FC150` | juego | `0x00821D57` | original (CRT) | 78 |
| 1225 | `0x00851360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1226 | `0x00851380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1227 | `0x008513A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1228 | `0x008513C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1229 | `0x008513E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1230 | `0x00851400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1231 | `0x00851420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1232 | `0x00851440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1233 | `0x00851460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1234 | `0x00851480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1235 | `0x008514A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1236 | `0x008514C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1237 | `0x008514E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1238 | `0x006023A0` | juego | `0x008514EA` | original (CRT) | 78 |
| 1239 | `0x00851500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1240 | `0x00851520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1241 | `0x00851540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1242 | `0x00851560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1243 | `0x00851580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1244 | `0x008515A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1245 | `0x008515C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1246 | `0x008515E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1247 | `0x00851600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1248 | `0x00851620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1249 | `0x00851640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1250 | `0x00851660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1251 | `0x00851680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1252 | `0x00608330` | juego | `0x00821D57` | original (CRT) | 78 |
| 1253 | `0x008516B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1254 | `0x008516D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1255 | `0x008516F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1256 | `0x00851710` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1257 | `0x00851730` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1258 | `0x00851750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1259 | `0x00851770` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1260 | `0x00851790` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1261 | `0x008517B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1262 | `0x008517D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1263 | `0x008517F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1264 | `0x00851810` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1265 | `0x00851830` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1266 | `0x00851850` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1267 | `0x00851870` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1268 | `0x00851890` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1269 | `0x008518B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1270 | `0x008518D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1271 | `0x008518F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1272 | `0x00851910` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1273 | `0x00851930` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1274 | `0x00851950` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1275 | `0x00851970` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1276 | `0x00851990` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1277 | `0x008519B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1278 | `0x008519D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1279 | `0x008519F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1280 | `0x00851A10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1281 | `0x00851A30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1282 | `0x00851A50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1283 | `0x00851A70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1284 | `0x00617330` | juego | `0x00851A7A` | original (CRT) | 78 |
| 1285 | `0x00616B30` | juego | `0x00821D57` | original (CRT) | 78 |
| 1286 | `0x00851A90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1287 | `0x00851AB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1288 | `0x00851AD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1289 | `0x00851AF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1290 | `0x00851B10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1291 | `0x00851B30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1292 | `0x00851B50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1293 | `0x00617FC0` | juego | `0x00851B5A` | original (CRT) | 78 |
| 1294 | `0x00617850` | juego | `0x00821D57` | original (CRT) | 78 |
| 1295 | `0x00851B70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1296 | `0x00851B90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1297 | `0x00851BB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1298 | `0x00851BD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1299 | `0x00851BF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1300 | `0x00851C10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1301 | `0x00851C30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1302 | `0x00851C50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1303 | `0x00851C70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1304 | `0x00851C90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1305 | `0x00851CB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1306 | `0x00851CD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1307 | `0x00851CF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1308 | `0x00851D10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1309 | `0x00851D30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1310 | `0x00851D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1311 | `0x00851D70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1312 | `0x00851D90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1313 | `0x00851DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1314 | `0x00851DD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1315 | `0x00851DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1316 | `0x00851E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1317 | `0x00851E30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1318 | `0x00851E50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1319 | `0x00851E70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1320 | `0x00851E90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1321 | `0x00851EB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1322 | `0x00851ED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1323 | `0x00851EF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1324 | `0x00851F10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1325 | `0x00851F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1326 | `0x00851F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1327 | `0x00851F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1328 | `0x00851F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1329 | `0x00851FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1330 | `0x00851FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1331 | `0x00851FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1332 | `0x00852010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1333 | `0x0040F030` | juego | `0x00823BCF` | original (CRT) | 78 |
| 1334 | `0x00852020` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1335 | `0x00852040` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1336 | `0x00852060` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1337 | `0x00852080` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1338 | `0x008520A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1339 | `0x008520C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1340 | `0x008520E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1341 | `0x00632BD0` | juego | `0x00821D57` | original (CRT) | 78 |
| 1342 | `0x0061A3B0` | juego | `0x00632BD9` | original (juego) | 78 |
| 1343 | `0x00852110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1344 | `0x0062EC40` | juego | `0x00821D57` | original (CRT) | 78 |
| 1345 | `0x00852140` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1346 | `0x00852160` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1347 | `0x00852180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1348 | `0x008521A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1349 | `0x008521C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1350 | `0x008521E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1351 | `0x00852200` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1352 | `0x00852220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1353 | `0x00852240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1354 | `0x00852260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1355 | `0x00852280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1356 | `0x008522A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1357 | `0x008522C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1358 | `0x008522E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1359 | `0x00852300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1360 | `0x00852320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1361 | `0x00852340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1362 | `0x00852360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1363 | `0x00852380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1364 | `0x008523A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1365 | `0x008523C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1366 | `0x008523E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1367 | `0x00852400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1368 | `0x00852420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1369 | `0x00852440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1370 | `0x00852460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1371 | `0x00852480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1372 | `0x008524A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1373 | `0x008524C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1374 | `0x008524E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1375 | `0x00852500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1376 | `0x00852520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1377 | `0x00852540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1378 | `0x00852560` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1379 | `0x00852580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1380 | `0x008525A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1381 | `0x008525C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1382 | `0x008525E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1383 | `0x00852600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1384 | `0x00852620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1385 | `0x00852640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1386 | `0x00852660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1387 | `0x00852680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1388 | `0x00852690` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1389 | `0x008526A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1390 | `0x008526B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1391 | `0x008526C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1392 | `0x008526E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1393 | `0x00852700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1394 | `0x00852720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1395 | `0x00852740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1396 | `0x00852760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1397 | `0x00852780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1398 | `0x008527A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1399 | `0x008527C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1400 | `0x008527E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1401 | `0x00852800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1402 | `0x00852820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1403 | `0x00852840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1404 | `0x00852850` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1405 | `0x00852860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1406 | `0x00852870` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1407 | `0x0066D440` | juego | `0x0085287A` | original (CRT) | 78 |
| 1408 | `0x00852890` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1409 | `0x008528B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1410 | `0x008528D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1411 | `0x008528F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1412 | `0x00852910` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1413 | `0x00852930` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1414 | `0x00852950` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1415 | `0x00852970` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1416 | `0x00852990` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1417 | `0x008529B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1418 | `0x008529D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1419 | `0x008529F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1420 | `0x00852A10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1421 | `0x00852A30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1422 | `0x00852A50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1423 | `0x00852A70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1424 | `0x00852A90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1425 | `0x00852AB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1426 | `0x00852AD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1427 | `0x00852AF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1428 | `0x00852B10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1429 | `0x00852B30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1430 | `0x00852B50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1431 | `0x00852B70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1432 | `0x00852B90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1433 | `0x00852BB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1434 | `0x00852BD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1435 | `0x00852BF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1436 | `0x00852C10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1437 | `0x00852C30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1438 | `0x00852C50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1439 | `0x00852C70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1440 | `0x00852C90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1441 | `0x00852CB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1442 | `0x00852CD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1443 | `0x00852CF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1444 | `0x00852D10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1445 | `0x00852D30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1446 | `0x00852D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1447 | `0x00852D70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1448 | `0x00852D90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1449 | `0x00852DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1450 | `0x00852DD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1451 | `0x00852DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1452 | `0x00852E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1453 | `0x00852E30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1454 | `0x00852E50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1455 | `0x00852E70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1456 | `0x00852E90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1457 | `0x00852EB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1458 | `0x00852ED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1459 | `0x00852EF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1460 | `0x00852F10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1461 | `0x00694850` | juego | `0x00852F1A` | original (CRT) | 78 |
| 1462 | `0x00852F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1463 | `0x00852F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1464 | `0x00852F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1465 | `0x00852F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1466 | `0x00852FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1467 | `0x00852FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1468 | `0x00852FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1469 | `0x00853010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1470 | `0x00853030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1471 | `0x00853050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1472 | `0x00853070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1473 | `0x00853090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1474 | `0x008530B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1475 | `0x008530D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1476 | `0x008530F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1477 | `0x00853110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1478 | `0x00853130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1479 | `0x00853150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1480 | `0x00853170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1481 | `0x00853180` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1482 | `0x008531A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1483 | `0x008531C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1484 | `0x008531E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1485 | `0x00853200` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1486 | `0x00853220` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1487 | `0x00853240` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1488 | `0x00853260` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1489 | `0x00853280` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1490 | `0x008532A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1491 | `0x008532C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1492 | `0x008532E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1493 | `0x00853300` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1494 | `0x00853320` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1495 | `0x00853340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1496 | `0x00853360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1497 | `0x00853380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1498 | `0x008533A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1499 | `0x008533C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1500 | `0x008533E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1501 | `0x00853400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1502 | `0x00853420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1503 | `0x00853440` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1504 | `0x00853460` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1505 | `0x006A00F0` | juego | `0x0085346A` | original (CRT) | 78 |
| 1506 | `0x00853480` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1507 | `0x008534A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1508 | `0x008534C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1509 | `0x008534E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1510 | `0x00853500` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1511 | `0x00853520` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1512 | `0x00853540` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1513 | `0x00853580` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1514 | `0x008535A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1515 | `0x008535C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1516 | `0x008535E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1517 | `0x00853600` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1518 | `0x00853620` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1519 | `0x00853640` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1520 | `0x00853660` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1521 | `0x00853680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1522 | `0x008536A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1523 | `0x008536C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1524 | `0x008536E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1525 | `0x00853700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1526 | `0x00853720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1527 | `0x00853740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1528 | `0x00853760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1529 | `0x00853780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1530 | `0x008537A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1531 | `0x008537C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1532 | `0x008537E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1533 | `0x00853800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1534 | `0x00853820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1535 | `0x00853840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1536 | `0x00853860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1537 | `0x00853880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1538 | `0x008538A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1539 | `0x008538C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1540 | `0x008538E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1541 | `0x00853900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1542 | `0x00853920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1543 | `0x00853940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1544 | `0x00853960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1545 | `0x00853980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1546 | `0x008539A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1547 | `0x008539C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1548 | `0x008539E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1549 | `0x00853A00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1550 | `0x00853A20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1551 | `0x00853A40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1552 | `0x00853A60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1553 | `0x00853A80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1554 | `0x00853AA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1555 | `0x00853AC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1556 | `0x00853AE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1557 | `0x00853B00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1558 | `0x00853B20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1559 | `0x00853B40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1560 | `0x00853B60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1561 | `0x00853B80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1562 | `0x00853BA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1563 | `0x00853BC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1564 | `0x00853BE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1565 | `0x00853C00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1566 | `0x00853C20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1567 | `0x00853C40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1568 | `0x00853C60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1569 | `0x00853C80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1570 | `0x00853CA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1571 | `0x00853CC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1572 | `0x00853CE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1573 | `0x00853D00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1574 | `0x00853D20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1575 | `0x00853D40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1576 | `0x00853D60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1577 | `0x00853D80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1578 | `0x00853DA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1579 | `0x00853DC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1580 | `0x00853DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1581 | `0x00853E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1582 | `0x00853E20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1583 | `0x005BD420` | juego | `0x00821D57` | original (CRT) | 78 |
| 1584 | `0x00853E50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1585 | `0x00853E70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1586 | `0x00853E90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1587 | `0x00853EB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1588 | `0x00853ED0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1589 | `0x00853EF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1590 | `0x00853F10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1591 | `0x006E3EB0` | juego | `0x00853F1A` | original (CRT) | 78 |
| 1592 | `0x006E3E30` | juego | `0x00821D57` | original (CRT) | 78 |
| 1593 | `0x00853F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1594 | `0x00853F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1595 | `0x00853F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1596 | `0x00853F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1597 | `0x00853FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1598 | `0x00853FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1599 | `0x00853FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1600 | `0x00854010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1601 | `0x00854030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1602 | `0x00854050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1603 | `0x00854070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1604 | `0x00854090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1605 | `0x008540B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1606 | `0x008540D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1607 | `0x008540F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1608 | `0x00854110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1609 | `0x00854130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1610 | `0x00854150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1611 | `0x00854170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1612 | `0x00854190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1613 | `0x008541B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1614 | `0x008541D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1615 | `0x008541F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1616 | `0x00854210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1617 | `0x00854230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1618 | `0x00854250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1619 | `0x00854270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1620 | `0x00854290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1621 | `0x008542B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1622 | `0x008542D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1623 | `0x008542F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1624 | `0x00854310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1625 | `0x00854330` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1626 | `0x00854350` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1627 | `0x00854370` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1628 | `0x00854390` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1629 | `0x008543B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1630 | `0x008543D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1631 | `0x006D0450` | juego | `0x008543E7` | original (CRT) | 78 |
| 1632 | `0x00854410` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1633 | `0x00854430` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1634 | `0x00854450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1635 | `0x00854470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1636 | `0x00854490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1637 | `0x008544B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1638 | `0x008544D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1639 | `0x008544F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1640 | `0x00854510` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1641 | `0x00854530` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1642 | `0x00854550` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1643 | `0x00854570` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1644 | `0x00854590` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1645 | `0x008545B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1646 | `0x008545E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1647 | `0x006F9EB0` | juego | `0x006FA7B1` | original (juego) | 78 |
| 1648 | `0x00854610` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 78 |
| 1649 | `0x00854630` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1650 | `0x00854650` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1651 | `0x00854670` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1652 | `0x00854690` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1653 | `0x008546B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1654 | `0x008546D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1655 | `0x008546F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1656 | `0x00854710` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1657 | `0x00854730` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1658 | `0x00854750` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1659 | `0x00854770` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1660 | `0x00854790` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1661 | `0x008547B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1662 | `0x008547D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1663 | `0x008547F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1664 | `0x00854810` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1665 | `0x00854820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1666 | `0x00854840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1667 | `0x00854860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1668 | `0x00854880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1669 | `0x008548A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1670 | `0x008548B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1671 | `0x008548C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1672 | `0x008548E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1673 | `0x00854900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1674 | `0x00854920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1675 | `0x00854940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1676 | `0x00854960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1677 | `0x00854980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1678 | `0x00706770` | juego | `0x0085498A` | original (CRT) | 94 |
| 1679 | `0x008549A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1680 | `0x008549C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1681 | `0x008549E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1682 | `0x00854A00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1683 | `0x00854A20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1684 | `0x00854A40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1685 | `0x00854A60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1686 | `0x00854A80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1687 | `0x00854AA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1688 | `0x00854AC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1689 | `0x00854AE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1690 | `0x00854B00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1691 | `0x00854B20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1692 | `0x00854B40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1693 | `0x00854B60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1694 | `0x0070F8D0` | juego | `0x00821D57` | original (CRT) | 94 |
| 1695 | `0x00854B90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1696 | `0x00854BB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1697 | `0x00854BD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1698 | `0x00854BF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1699 | `0x00854C10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1700 | `0x00854C30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1701 | `0x00854C50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1702 | `0x00854C70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1703 | `0x00854C80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1704 | `0x00854C90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1705 | `0x00854CB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1706 | `0x00854CD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1707 | `0x00854CF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1708 | `0x00854D10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1709 | `0x00854D30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1710 | `0x00854D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1711 | `0x00717110` | juego | `0x00821D57` | original (CRT) | 94 |
| 1712 | `0x00854D80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1713 | `0x00854DA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1714 | `0x00854DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1715 | `0x00854DC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1716 | `0x00854DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1717 | `0x00854E20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1718 | `0x00854E40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1719 | `0x00854E60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1720 | `0x00854E80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1721 | `0x00854EA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1722 | `0x00854EC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1723 | `0x00854EE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1724 | `0x00854F10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1725 | `0x00854F30` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1726 | `0x00854F50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1727 | `0x00854F70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1728 | `0x00854F90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1729 | `0x00854FB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1730 | `0x00854FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1731 | `0x00854FF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1732 | `0x00855010` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1733 | `0x00855030` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1734 | `0x00855050` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1735 | `0x00855070` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1736 | `0x00855090` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1737 | `0x008550B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1738 | `0x008550D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1739 | `0x008550F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1740 | `0x00855110` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1741 | `0x00855130` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1742 | `0x00855150` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1743 | `0x00855170` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1744 | `0x00855190` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1745 | `0x008551B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1746 | `0x008551D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1747 | `0x008551F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1748 | `0x00855210` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1749 | `0x00855230` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1750 | `0x00855250` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1751 | `0x00855270` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1752 | `0x00855290` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1753 | `0x008552B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1754 | `0x008552D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1755 | `0x008552F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1756 | `0x00855310` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1757 | `0x00855340` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1758 | `0x00855360` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1759 | `0x00855380` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1760 | `0x008553A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1761 | `0x008553C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1762 | `0x008553E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1763 | `0x00855400` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1764 | `0x00855420` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1765 | `0x00855450` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1766 | `0x00855470` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1767 | `0x00855490` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1768 | `0x008554B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1769 | `0x008554D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1770 | `0x008554F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1771 | `0x00855510` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1772 | `0x0072A620` | juego | `0x0085551A` | original (CRT) | 94 |
| 1773 | `0x00855530` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1774 | `0x00855550` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1775 | `0x00855570` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1776 | `0x00855590` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1777 | `0x008555B0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1778 | `0x008555D0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1779 | `0x008555F0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1780 | `0x00855610` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1781 | `0x00855630` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1782 | `0x00855650` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1783 | `0x00855670` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1784 | `0x0072E860` | juego | `0x00823BCF` | original (CRT) | 94 |
| 1785 | `0x00855680` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1786 | `0x00855690` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1787 | `0x008556A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1788 | `0x008556C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1789 | `0x008556E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1790 | `0x00855700` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1791 | `0x00855720` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1792 | `0x00855740` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1793 | `0x00855760` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1794 | `0x00855780` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1795 | `0x008557A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1796 | `0x008557C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1797 | `0x008557E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1798 | `0x00855800` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1799 | `0x00855820` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1800 | `0x00855840` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1801 | `0x00855860` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1802 | `0x00855880` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1803 | `0x008558A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1804 | `0x008558C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1805 | `0x008558E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1806 | `0x00855900` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1807 | `0x00855920` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1808 | `0x00855940` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1809 | `0x00855960` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1810 | `0x00855980` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1811 | `0x008559A0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1812 | `0x008559C0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1813 | `0x008559E0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1814 | `0x005074B0` | juego | `0x008559EA` | original (CRT) | 94 |
| 1815 | `0x00855A00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1816 | `0x00855A20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1817 | `0x00855A40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1818 | `0x00855A60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1819 | `0x00855A80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1820 | `0x00855AA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1821 | `0x00855AC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1822 | `0x00855AE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1823 | `0x00855B00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1824 | `0x00855B20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1825 | `0x00855B40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1826 | `0x00855B60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1827 | `0x00855B80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1828 | `0x00855BA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1829 | `0x00855BC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1830 | `0x00855BE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1831 | `0x00855C00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1832 | `0x00855C20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1833 | `0x00855C40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1834 | `0x00855C60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1835 | `0x00855C80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1836 | `0x00855CA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1837 | `0x00855CC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1838 | `0x00855CE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1839 | `0x00855D00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1840 | `0x00855D20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1841 | `0x00855D50` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1842 | `0x00855D70` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1843 | `0x00855D90` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1844 | `0x00855DB0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1845 | `0x00855DD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1846 | `0x00855DF0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1847 | `0x00855E10` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1848 | `0x00855E40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1849 | `0x00855E60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1850 | `0x00855E80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1851 | `0x00855EA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1852 | `0x00855EC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1853 | `0x00855EE0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1854 | `0x00855F00` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1855 | `0x00855F20` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1856 | `0x00855F40` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1857 | `0x00855F60` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1858 | `0x00855F80` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1859 | `0x00855FA0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1860 | `0x00855FC0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1861 | `0x00855FD0` | CRT y bibliotecas | `0x00823BCF` | original (CRT) | 94 |
| 1862 | `0x0082D88A` | CRT y bibliotecas | `0x008246D1` | original (CRT) | 94 |
| 1863 | `0x00835335` | CRT y bibliotecas | `0x0082D8C4` | original (CRT) | 94 |
| 1864 | `0x00835270` | CRT y bibliotecas | `0x00835342` | original (CRT) | 94 |
| 1865 | `0x00748710` | juego | `0x008246F1` | original (CRT) | 94 |
| 1866 | `0x007468E0` | juego | `0x00748732` | original (juego) | 94 |
| 1867 | `0x00619B60` | juego | `0x0074873E` | original (juego) | 94 |
| 1868 | `0x0053EC10` | juego | `0x00619B71` | original (juego) | 94 |
| 1869 | `0x0053BB50` | juego | `0x0053EC30` | original (juego) | 94 |
| 1870 | `0x0072F3B0` | juego | `0x0053BB55` | original (juego) | 94 |
| 1871 | `0x0056D180` | juego | `0x0053BB5A` | original (juego) | 94 |
| 1872 | `0x005386F0` | juego | `0x0053BB5F` | original (juego) | 94 |
| 1873 | `0x00836DA3` | CRT y bibliotecas | `0x00836EB6` | original (CRT) | 94 |
| 1874 | `0x00836ECE` | CRT y bibliotecas | `0x00836EC5` | original (CRT) | 94 |
| 1875 | `0x00406B70` | juego | `0x0053BB66` | original (juego) | 94 |
| 1876 | `0x0072F4C0` | juego | `0x00406BF9` | original (juego) | 94 |
| 1877 | `0x004067B0` | juego | `0x00406C39` | original (juego) | 94 |
| 1878 | `0x00406A20` | juego | `0x00406C45` | original (juego) | 94 |
| 1879 | `0x00406690` | juego | `0x00406C4F` | original (juego) | 94 |
| 1880 | `0x00406460` | juego | `0x004066A6` | original (juego) | 94 |
| 1881 | `0x004068F0` | juego | `0x00406C7D` | original (juego) | 94 |
| 1882 | `0x0072F4F0` | juego | `0x00406C83` | original (juego) | 94 |
| 1883 | `0x00541D90` | juego | `0x0053BB6E` | original (juego) | 94 |
| 1884 | `0x00619600` | juego | `0x0053EC35` | original (juego) | 94 |
| 1885 | `0x00747420` | juego | `0x0053EC35` | original (juego) | 94 |
| 1886 | `0x00744FB0` | juego | `0x00747475` | original (juego) | 94 |
| 1887 | `0x00406560` | juego | `0x75EB5D49` | otro | 94 |
| 1888 | `0x00406570` | juego | `0x00406560` | original (juego) | 94 |
| 1889 | `0x00619040` | juego | `0x0074747B` | original (juego) | 94 |
| 1890 | `0x00821BB5` | CRT y bibliotecas | `0x00619059` | original (juego) | 94 |
| 1891 | `0x00827F4B` | CRT y bibliotecas | `0x00821BE4` | original (CRT) | 94 |
| 1892 | `0x00827ED1` | CRT y bibliotecas | `0x008285F2` | original (CRT) | 94 |
| 1893 | `0x00827EF5` | CRT y bibliotecas | `0x00828606` | original (CRT) | 94 |
| 1894 | `0x00827E9E` | CRT y bibliotecas | `0x00827F1C` | original (CRT) | 94 |
| 1895 | `0x007465B0` | juego | `0x00747483` | original (juego) | 94 |
| 1896 | `0x0069EDC0` | juego | `0x0069FF2C` | original (juego) | 94 |
| 1897 | `0x0069DCD0` | juego | `0x0069EDCB` | original (juego) | 94 |
| 1898 | `0x00588A50` | juego | `0x0069EDF1` | original (juego) | 94 |
| 1899 | `0x006A01A0` | juego | `0x007466AE` | original (juego) | 94 |
| 1900 | `0x005387D0` | juego | `0x006A01C8` | original (juego) | 94 |
| 1901 | `0x00836F1E` | CRT y bibliotecas | `0x006A01C8` | original (juego) | 94 |
| 1902 | `0x008370D0` | CRT y bibliotecas | `0x00836F78` | original (CRT) | 94 |
| 1903 | `0x0083312F` | CRT y bibliotecas | `0x008370EA` | original (CRT) | 94 |
| 1904 | `0x00833195` | CRT y bibliotecas | `0x0083318A` | original (CRT) | 94 |
| 1905 | `0x00538900` | juego | `0x006A022D` | original (juego) | 94 |
| 1906 | `0x008232D8` | CRT y bibliotecas | `0x006A022D` | original (juego) | 94 |
| 1907 | `0x0082327C` | CRT y bibliotecas | `0x008232E7` | original (CRT) | 94 |
| 1908 | `0x0082B206` | CRT y bibliotecas | `0x0082328D` | original (CRT) | 94 |
| 1909 | `0x0082AD3F` | CRT y bibliotecas | `0x0082B25A` | original (CRT) | 94 |
| 1910 | `0x00823FD1` | CRT y bibliotecas | `0x0082B271` | original (CRT) | 94 |
| 1911 | `0x0082B31F` | CRT y bibliotecas | `0x0082B314` | original (CRT) | 94 |
| 1912 | `0x0082B019` | CRT y bibliotecas | `0x008232B6` | original (CRT) | 94 |
| 1913 | `0x00834334` | CRT y bibliotecas | `0x0082B155` | original (CRT) | 94 |
| 1914 | `0x00833FF8` | CRT y bibliotecas | `0x00834361` | original (CRT) | 94 |
| 1915 | `0x00833E02` | CRT y bibliotecas | `0x00834191` | original (CRT) | 94 |
| 1916 | `0x00833C31` | CRT y bibliotecas | `0x0083421D` | original (CRT) | 94 |
| 1917 | `0x00834379` | CRT y bibliotecas | `0x00834370` | original (CRT) | 94 |
| 1918 | `0x00833DE0` | CRT y bibliotecas | `0x00834387` | original (CRT) | 94 |
| 1919 | `0x008232CE` | CRT y bibliotecas | `0x008232C5` | original (CRT) | 94 |
| 1920 | `0x00823FF4` | CRT y bibliotecas | `0x008232D6` | original (CRT) | 94 |
| 1921 | `0x00538950` | juego | `0x006A023C` | original (juego) | 94 |
| 1922 | `0x00823521` | CRT y bibliotecas | `0x00538966` | original (juego) | 94 |
| 1923 | `0x00823FA2` | CRT y bibliotecas | `0x00823535` | original (CRT) | 94 |
| 1924 | `0x00823438` | CRT y bibliotecas | `0x0082354B` | original (CRT) | 94 |
| 1925 | `0x0082AF38` | CRT y bibliotecas | `0x008234E7` | original (CRT) | 94 |
| 1926 | `0x0083361B` | CRT y bibliotecas | `0x0082AF71` | original (CRT) | 94 |
| 1927 | `0x0082B617` | CRT y bibliotecas | `0x0082AF87` | original (CRT) | 94 |
| 1928 | `0x00833D6D` | CRT y bibliotecas | `0x0082B654` | original (CRT) | 94 |
| 1929 | `0x0082B44A` | CRT y bibliotecas | `0x0082B66E` | original (CRT) | 94 |
| 1930 | `0x0082B69E` | CRT y bibliotecas | `0x0082B696` | original (CRT) | 94 |
| 1931 | `0x008266F0` | CRT y bibliotecas | `0x0082349C` | original (CRT) | 94 |
| 1932 | `0x00823563` | CRT y bibliotecas | `0x0082355A` | original (CRT) | 94 |
| 1933 | `0x008214D0` | CRT y bibliotecas | `0x006A02B5` | original (juego) | 94 |
| 1934 | `0x0069F670` | juego | `0x006A02D7` | original (juego) | 94 |
| 1935 | `0x0069F490` | juego | `0x006A030F` | original (juego) | 94 |
| 1936 | `0x00821195` | CRT y bibliotecas | `0x0069F4AB` | original (juego) | 94 |
| 1937 | `0x0082119A` | CRT y bibliotecas | `0x0069F4AB` | original (juego) | 94 |
| 1938 | `0x0069F5D0` | juego | `0x006A0349` | original (juego) | 94 |
| 1939 | `0x005389D0` | juego | `0x006A03A8` | original (juego) | 94 |
| 1940 | `0x0082318B` | CRT y bibliotecas | `0x006A03A8` | original (juego) | 94 |
| 1941 | `0x0082313F` | CRT y bibliotecas | `0x008231C2` | original (CRT) | 94 |
| 1942 | `0x00823D26` | CRT y bibliotecas | `0x00823154` | original (CRT) | 94 |
| 1943 | `0x0082AF0D` | CRT y bibliotecas | `0x0082315C` | original (CRT) | 94 |
| 1944 | `0x0082AE72` | CRT y bibliotecas | `0x00823164` | original (CRT) | 94 |
| 1945 | `0x0082ADEF` | CRT y bibliotecas | `0x0082AEC3` | original (CRT) | 94 |
| 1946 | `0x00833D2C` | CRT y bibliotecas | `0x0082ADFB` | original (CRT) | 94 |
| 1947 | `0x00833CAD` | CRT y bibliotecas | `0x0082AE45` | original (CRT) | 94 |
| 1948 | `0x0082AEE9` | CRT y bibliotecas | `0x0082AEE1` | original (CRT) | 94 |
| 1949 | `0x008231D4` | CRT y bibliotecas | `0x008231CF` | original (CRT) | 94 |
| 1950 | `0x006A0050` | juego | `0x006A03BC` | original (juego) | 94 |
| 1951 | `0x006A0000` | juego | `0x006A0075` | original (juego) | 94 |
| 1952 | `0x0053CF30` | juego | `0x006A000D` | original (juego) | 94 |
| 1953 | `0x00823A0E` | CRT y bibliotecas | `0x0053CF49` | original (juego) | 94 |
| 1954 | `0x00825456` | CRT y bibliotecas | `0x00823A23` | original (CRT) | 94 |
| 1955 | `0x0082524D` | CRT y bibliotecas | `0x00825473` | original (CRT) | 94 |
| 1956 | `0x00825488` | CRT y bibliotecas | `0x0082547F` | original (CRT) | 94 |
| 1957 | `0x00823945` | CRT y bibliotecas | `0x00823A2D` | original (CRT) | 94 |
| 1958 | `0x0069F570` | juego | `0x006A0026` | original (juego) | 94 |
| 1959 | `0x0069F7E0` | juego | `0x006A03C2` | original (juego) | 94 |
| 1960 | `0x0057C8F0` | juego | `0x0074754A` | original (juego) | 94 |
| 1961 | `0x00538860` | juego | `0x0057C8FC` | original (juego) | 94 |
| 1962 | `0x00538990` | juego | `0x0057C96D` | original (juego) | 94 |
| 1963 | `0x0082374F` | CRT y bibliotecas | `0x005389A4` | original (juego) | 94 |
| 1964 | `0x008236C0` | CRT y bibliotecas | `0x00823776` | original (CRT) | 94 |
| 1965 | `0x0082B986` | CRT y bibliotecas | `0x0082372E` | original (CRT) | 94 |
| 1966 | `0x0082B912` | CRT y bibliotecas | `0x0082B9DD` | original (CRT) | 94 |
| 1967 | `0x0082BA0D` | CRT y bibliotecas | `0x0082BA05` | original (CRT) | 94 |
| 1968 | `0x0082378E` | CRT y bibliotecas | `0x00823785` | original (CRT) | 94 |
| 1969 | `0x00530530` | juego | `0x0057C995` | original (juego) | 94 |
| 1970 | `0x008260FF` | CRT y bibliotecas | `0x008236F0` | original (CRT) | 94 |
| 1971 | `0x00507E10` | juego | `0x0057CC71` | original (juego) | 94 |
| 1972 | `0x004DBEC0` | juego | `0x00507E1A` | original (juego) | 94 |
| 1973 | `0x00745650` | juego | `0x0074754F` | original (juego) | 94 |
| 1974 | `0x0074754F` | juego | `0x00000000` | otro | 235 |
| 1975 | `0x007455E0` | juego | `0x00747605` | original (juego) | 235 |
| 1976 | `0x0081805C` | RenderWare/D3D | `0x007455F6` | original (juego) | 235 |
| 1977 | `0x00818041` | RenderWare/D3D | `0x007455F6` | original (juego) | 235 |
| 1978 | `0x00820FFA` | CRT y bibliotecas | `0x00000018` | otro | 235 |
| 1979 | `0x00745840` | juego | `0x0074760D` | original (juego) | 282 |
| 1980 | `0x0081803B` | RenderWare/D3D | `0x0074585F` | original (juego) | 297 |
| 1981 | `0x008213AE` | CRT y bibliotecas | `0x0069FF3B` | original (juego) | 313 |
| 1982 | `0x008214BD` | CRT y bibliotecas | `0x0069FF3B` | original (juego) | 313 |
| 1983 | `0x007486A0` | juego | `0x00748760` | original (juego) | 313 |
| 1984 | `0x00746480` | juego | `0x00748778` | original (juego) | 313 |
| 1985 | `0x00745560` | juego | `0x007487AD` | original (juego) | 313 |
| 1986 | `0x00747EB0` | juego | `0x7722A33F` | otro | 313 |
| 1987 | `0x007487CF` | juego | `0x75EBD640` | otro | 313 |
| 1988 | `0x00817523` | RenderWare/D3D | `0x00748806` | original (juego) | 313 |
| 1989 | `0x00817508` | RenderWare/D3D | `0x00748806` | original (juego) | 313 |
| 1990 | `0x007469A0` | juego | `0x00748810` | original (juego) | 328 |
| 1991 | `0x007485C0` | juego | `0x00748818` | original (juego) | 328 |
| 1992 | `0x007477C0` | juego | `0x007485EA` | original (juego) | 328 |
| 1993 | `0x0053F2D0` | juego | `0x00748831` | original (juego) | 360 |
| 1994 | `0x0052F6F0` | juego | `0x0074883C` | original (juego) | 360 |
| 1995 | `0x0052F590` | juego | `0x0052F70D` | original (juego) | 360 |
| 1996 | `0x005BF390` | juego | `0x0053EC79` | original (juego) | 360 |
| 1997 | `0x00619C90` | juego | `0x005BF39A` | original (juego) | 360 |
| 1998 | `0x00745510` | juego | `0x00619C9D` | original (juego) | 360 |
| 1999 | `0x007F3170` | RenderWare/D3D | `0x00619CA3` | original (juego) | 360 |
| 2000 | `0x00801970` | RenderWare/D3D | `0x007F31BA` | original (RenderWare/D3D) | 360 |
| 2001 | `0x0080A240` | RenderWare/D3D | `0x007F31E5` | original (RenderWare/D3D) | 360 |
| 2002 | `0x00801FD0` | RenderWare/D3D | `0x007F31F9` | original (RenderWare/D3D) | 360 |
| 2003 | `0x008019B0` | RenderWare/D3D | `0x00802000` | original (RenderWare/D3D) | 360 |
| 2004 | `0x00804140` | RenderWare/D3D | `0x007F320B` | original (RenderWare/D3D) | 360 |
| 2005 | `0x008082E0` | RenderWare/D3D | `0x007F321A` | original (RenderWare/D3D) | 360 |
| 2006 | `0x00801B70` | RenderWare/D3D | `0x00808300` | original (RenderWare/D3D) | 360 |
| 2007 | `0x0072F420` | juego | `0x00801A8A` | original (RenderWare/D3D) | 360 |
| 2008 | `0x008084A0` | RenderWare/D3D | `0x007F3244` | original (RenderWare/D3D) | 360 |
| 2009 | `0x007F2B90` | RenderWare/D3D | `0x008084B5` | original (RenderWare/D3D) | 360 |
| 2010 | `0x00801C30` | RenderWare/D3D | `0x008085E2` | original (RenderWare/D3D) | 360 |
| 2011 | `0x00807C70` | RenderWare/D3D | `0x007F33C1` | original (RenderWare/D3D) | 360 |
| 2012 | `0x007F5F60` | RenderWare/D3D | `0x007F3469` | original (RenderWare/D3D) | 360 |
| 2013 | `0x004C9AB0` | juego | `0x007F3469` | original (RenderWare/D3D) | 360 |
| 2014 | `0x007FB0B0` | RenderWare/D3D | `0x004C9ACA` | original (juego) | 360 |
| 2015 | `0x00745520` | juego | `0x00619CB8` | original (juego) | 360 |
| 2016 | `0x0053D870` | juego | `0x0053ECA6` | original (juego) | 360 |
| 2017 | `0x0074FDA0` | juego | `0x0053D875` | original (juego) | 360 |
| 2018 | `0x007F2BB0` | RenderWare/D3D | `0x0074FDB7` | original (juego) | 360 |
| 2019 | `0x00750050` | RenderWare/D3D | `0x0074FE72` | original (juego) | 360 |
| 2020 | `0x007EE450` | RenderWare/D3D | `0x00750084` | original (RenderWare/D3D) | 360 |
| 2021 | `0x0074BDA0` | juego | `0x007500A6` | original (RenderWare/D3D) | 360 |
| 2022 | `0x0074BDD0` | juego | `0x007500C8` | original (RenderWare/D3D) | 360 |
| 2023 | `0x00751D60` | RenderWare/D3D | `0x007500ED` | original (RenderWare/D3D) | 360 |
| 2024 | `0x0074CD70` | juego | `0x00750106` | original (RenderWare/D3D) | 360 |
| 2025 | `0x00761C90` | RenderWare/D3D | `0x0075011A` | original (RenderWare/D3D) | 360 |
| 2026 | `0x0074CDA0` | juego | `0x00750135` | original (RenderWare/D3D) | 360 |
| 2027 | `0x008088E0` | RenderWare/D3D | `0x0074CDBE` | original (juego) | 360 |
| 2028 | `0x00761CC0` | RenderWare/D3D | `0x00750153` | original (RenderWare/D3D) | 360 |
| 2029 | `0x0074BE00` | juego | `0x007501DD` | original (RenderWare/D3D) | 360 |
| 2030 | `0x0074FCD0` | juego | `0x007501EE` | original (RenderWare/D3D) | 360 |
| 2031 | `0x0074FD00` | juego | `0x00750209` | original (RenderWare/D3D) | 360 |
| 2032 | `0x0074DBF0` | juego | `0x00750243` | original (RenderWare/D3D) | 360 |
| 2033 | `0x0074DC20` | juego | `0x0075025E` | original (RenderWare/D3D) | 360 |
| 2034 | `0x0075E3E0` | RenderWare/D3D | `0x0075026E` | original (RenderWare/D3D) | 360 |
| 2035 | `0x00749830` | juego | `0x0074FE7C` | original (juego) | 360 |
| 2036 | `0x0074EA40` | juego | `0x0074FE86` | original (juego) | 360 |
| 2037 | `0x00755B90` | RenderWare/D3D | `0x0074EA46` | original (juego) | 360 |
| 2038 | `0x007587D0` | RenderWare/D3D | `0x0074EA4F` | original (juego) | 360 |
| 2039 | `0x007FAE30` | RenderWare/D3D | `0x0074EA61` | original (juego) | 360 |
| 2040 | `0x007FAE20` | RenderWare/D3D | `0x0074EA70` | original (juego) | 360 |
| 2041 | `0x007C6820` | RenderWare/D3D | `0x0053D881` | original (juego) | 360 |
| 2042 | `0x0074BE30` | juego | `0x007C6883` | original (RenderWare/D3D) | 360 |
| 2043 | `0x00808920` | RenderWare/D3D | `0x0074BE44` | original (juego) | 360 |
| 2044 | `0x0074BE50` | juego | `0x007C6892` | original (RenderWare/D3D) | 360 |
| 2045 | `0x00808950` | RenderWare/D3D | `0x0074BE64` | original (juego) | 360 |
| 2046 | `0x007CCCA0` | RenderWare/D3D | `0x0053D88A` | original (juego) | 360 |
| 2047 | `0x007C4600` | RenderWare/D3D | `0x0053D893` | original (juego) | 360 |
| 2048 | `0x007F1260` | RenderWare/D3D | `0x007C4638` | original (RenderWare/D3D) | 360 |
| 2049 | `0x00807580` | RenderWare/D3D | `0x007C4656` | original (RenderWare/D3D) | 360 |
| 2050 | `0x0072FAB0` | juego | `0x0053D89C` | original (juego) | 360 |
| 2051 | `0x00732E30` | juego | `0x0053D8A5` | original (juego) | 360 |
| 2052 | `0x004D6150` | juego | `0x0053D8AE` | original (juego) | 360 |
| 2053 | `0x007CCD40` | RenderWare/D3D | `0x004D61EA` | original (juego) | 360 |
| 2054 | `0x00731650` | juego | `0x0053D8B7` | original (juego) | 360 |
| 2055 | `0x007F3C10` | RenderWare/D3D | `0x0073166B` | original (juego) | 360 |
| 2056 | `0x00810AA0` | RenderWare/D3D | `0x0053D8C0` | original (juego) | 360 |
| 2057 | `0x007CB940` | RenderWare/D3D | `0x0053D8C9` | original (juego) | 360 |
| 2058 | `0x005D7EF0` | juego | `0x0053D8D2` | original (juego) | 360 |
| 2059 | `0x005D72E0` | juego | `0x005D7EF5` | original (juego) | 360 |
| 2060 | `0x005D5B00` | juego | `0x0053D8DB` | original (juego) | 360 |
| 2061 | `0x005DA450` | juego | `0x005D5B05` | original (juego) | 360 |
| 2062 | `0x00748F70` | juego | `0x0053D8E4` | original (juego) | 360 |
| 2063 | `0x007F3BB0` | RenderWare/D3D | `0x00748FAB` | original (juego) | 360 |
| 2064 | `0x00804550` | RenderWare/D3D | `0x00748FD3` | original (juego) | 360 |
| 2065 | `0x007FDE60` | RenderWare/D3D | `0x00748FED` | original (juego) | 360 |
| 2066 | `0x0059D100` | juego | `0x0053D8E9` | original (juego) | 360 |
| 2067 | `0x0041B310` | juego | `0x0053D8F2` | original (juego) | 360 |
| 2068 | `0x0074BE70` | juego | `0x0041B349` | original (juego) | 360 |
| 2069 | `0x006FA970` | juego | `0x0053D8FB` | original (juego) | 360 |
| 2070 | `0x0072FBD0` | juego | `0x0053D908` | original (juego) | 360 |
| 2071 | `0x00744A20` | juego | `0x0053ECB2` | original (juego) | 360 |
| 2072 | `0x006194C0` | juego | `0x00744A2C` | original (juego) | 360 |
| 2073 | `0x007F2F70` | RenderWare/D3D | `0x00619CF9` | original (juego) | 360 |
| 2074 | `0x007F9C20` | RenderWare/D3D | `0x007F2FC0` | original (RenderWare/D3D) | 360 |
| 2075 | `0x007F2AB0` | RenderWare/D3D | `0x007F3022` | original (RenderWare/D3D) | 360 |
| 2076 | `0x007F5F70` | RenderWare/D3D | `0x007F2AD2` | original (RenderWare/D3D) | 360 |
| 2077 | `0x00807C2B` | RenderWare/D3D | `0x007F6310` | original (RenderWare/D3D) | 360 |
| 2078 | `0x00807C10` | RenderWare/D3D | `0x007F6310` | original (RenderWare/D3D) | 360 |
| 2079 | `0x007F6BF0` | RenderWare/D3D | `0x007F646D` | original (RenderWare/D3D) | 438 |
| 2080 | `0x0080DBE0` | RenderWare/D3D | `0x007F6472` | original (RenderWare/D3D) | 438 |
| 2081 | `0x0080DC40` | RenderWare/D3D | `0x007F647C` | original (RenderWare/D3D) | 438 |
| 2082 | `0x0080DCA0` | RenderWare/D3D | `0x007F6486` | original (RenderWare/D3D) | 438 |
| 2083 | `0x0080DD00` | RenderWare/D3D | `0x007F6490` | original (RenderWare/D3D) | 438 |
| 2084 | `0x007F72E0` | RenderWare/D3D | `0x007F68B8` | original (RenderWare/D3D) | 438 |
| 2085 | `0x00746190` | juego | `0x00619D15` | original (juego) | 438 |
| 2086 | `0x007F2C00` | RenderWare/D3D | `0x007461AF` | original (juego) | 438 |
| 2087 | `0x007F2C30` | RenderWare/D3D | `0x007461DD` | original (juego) | 438 |
| 2088 | `0x007F2C60` | RenderWare/D3D | `0x007461F2` | original (juego) | 438 |
| 2089 | `0x007F2C90` | RenderWare/D3D | `0x00746288` | original (juego) | 438 |
| 2090 | `0x007F7540` | RenderWare/D3D | `0x007F65DA` | original (RenderWare/D3D) | 438 |
| 2091 | `0x0072F440` | juego | `0x007F76FB` | original (RenderWare/D3D) | 438 |
| 2092 | `0x00745AF0` | juego | `0x007462A1` | original (juego) | 438 |
| 2093 | `0x007F2CC0` | RenderWare/D3D | `0x00745B23` | original (juego) | 438 |
| 2094 | `0x0072F460` | juego | `0x00745B2F` | original (juego) | 438 |
| 2095 | `0x007F2CF0` | RenderWare/D3D | `0x00745B4C` | original (juego) | 438 |
| 2096 | `0x00833570` | CRT y bibliotecas | `0x0082855D` | original (CRT) | 438 |
| 2097 | `0x008088D0` | RenderWare/D3D | `0x007F2B54` | original (RenderWare/D3D) | 438 |
| 2098 | `0x00808820` | RenderWare/D3D | `0x007F2B62` | original (RenderWare/D3D) | 438 |
| 2099 | `0x007F2D50` | RenderWare/D3D | `0x007462EF` | original (juego) | 438 |
| 2100 | `0x007460A0` | juego | `0x00746315` | original (juego) | 438 |
| 2101 | `0x00746163` | juego | `0x00000001` | otro | 438 |
| 2102 | `0x007F8580` | RenderWare/D3D | `0x00746323` | original (juego) | 438 |
| 2103 | `0x007F84F0` | RenderWare/D3D | `0x00746355` | original (juego) | 438 |
| 2104 | `0x007F2E70` | RenderWare/D3D | `0x00619D33` | original (juego) | 438 |
| 2105 | `0x007F6CB0` | RenderWare/D3D | `0x007F66D5` | original (RenderWare/D3D) | 438 |
| 2106 | `0x007F6781` | RenderWare/D3D | `0x065F4B00` | otro | 438 |
| 2107 | `0x007F6786` | RenderWare/D3D | `0x00000000` | otro | 1782 |
| 2108 | `0x007F6B00` | RenderWare/D3D | `0x007F681F` | original (RenderWare/D3D) | 1782 |
| 2109 | `0x007F6B40` | RenderWare/D3D | `0x007F6824` | original (RenderWare/D3D) | 1782 |
| 2110 | `0x00801980` | RenderWare/D3D | `0x007F6BA2` | original (RenderWare/D3D) | 1782 |
| 2111 | `0x004CC150` | juego | `0x007F684E` | original (RenderWare/D3D) | 1782 |
| 2112 | `0x007FB480` | RenderWare/D3D | `0x007F6853` | original (RenderWare/D3D) | 1782 |
| 2113 | `0x007FAA30` | RenderWare/D3D | `0x007FB593` | original (RenderWare/D3D) | 1782 |
| 2114 | `0x007FCAC0` | RenderWare/D3D | `0x007F6858` | original (RenderWare/D3D) | 1782 |
| 2115 | `0x007FC2D0` | RenderWare/D3D | `0x007FCEC3` | original (RenderWare/D3D) | 1782 |
| 2116 | `0x007FC3C0` | RenderWare/D3D | `0x007FCF57` | original (RenderWare/D3D) | 1782 |
| 2117 | `0x007FC340` | RenderWare/D3D | `0x007FCFCD` | original (RenderWare/D3D) | 1782 |
| 2118 | `0x007FC200` | RenderWare/D3D | `0x007F6858` | original (RenderWare/D3D) | 1782 |
| 2119 | `0x007F5CB0` | RenderWare/D3D | `0x007F685D` | original (RenderWare/D3D) | 1782 |
| 2120 | `0x007F5D20` | RenderWare/D3D | `0x007F5CB5` | original (RenderWare/D3D) | 1782 |
| 2121 | `0x007F5940` | RenderWare/D3D | `0x007F5D11` | original (RenderWare/D3D) | 1782 |
| 2122 | `0x007F5A00` | RenderWare/D3D | `0x007F5979` | original (RenderWare/D3D) | 1782 |
| 2123 | `0x008086E0` | RenderWare/D3D | `0x007F2E9F` | original (RenderWare/D3D) | 1782 |
| 2124 | `0x008087D0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2125 | `0x007EDE90` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2126 | `0x0080AA40` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2127 | `0x007F16C0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2128 | `0x007EFEF0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2129 | `0x007EC780` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2130 | `0x007EE110` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2131 | `0x00802280` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2132 | `0x00803FE0` | RenderWare/D3D | `0x008023EC` | original (RenderWare/D3D) | 1782 |
| 2133 | `0x008220F0` | CRT y bibliotecas | `0x0080405F` | original (RenderWare/D3D) | 1782 |
| 2134 | `0x00829B99` | CRT y bibliotecas | `0x0080405F` | original (RenderWare/D3D) | 1782 |
| 2135 | `0x007FB370` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2136 | `0x007F3EA0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2137 | `0x007F3600` | RenderWare/D3D | `0x007F3F86` | original (RenderWare/D3D) | 1782 |
| 2138 | `0x00807C40` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2139 | `0x00804FE0` | RenderWare/D3D | `0x00807C4E` | original (RenderWare/D3D) | 1782 |
| 2140 | `0x00809F90` | RenderWare/D3D | `0x00804FF4` | original (RenderWare/D3D) | 1782 |
| 2141 | `0x00809BF0` | RenderWare/D3D | `0x0080A05A` | original (RenderWare/D3D) | 1782 |
| 2142 | `0x008099F0` | RenderWare/D3D | `0x00809BFB` | original (RenderWare/D3D) | 1782 |
| 2143 | `0x0080EA40` | RenderWare/D3D | `0x0080507E` | original (RenderWare/D3D) | 1782 |
| 2144 | `0x0080A780` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2145 | `0x007EFE20` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2146 | `0x0080A0A0` | RenderWare/D3D | `0x007EFE62` | original (RenderWare/D3D) | 1782 |
| 2147 | `0x008057B0` | RenderWare/D3D | `0x0080A0A7` | original (RenderWare/D3D) | 1782 |
| 2148 | `0x00806990` | RenderWare/D3D | `0x0080A0BA` | original (RenderWare/D3D) | 1782 |
| 2149 | `0x0080E990` | RenderWare/D3D | `0x0080A0CA` | original (RenderWare/D3D) | 1782 |
| 2150 | `0x00806BE0` | RenderWare/D3D | `0x0080A0D3` | original (RenderWare/D3D) | 1782 |
| 2151 | `0x00806DE0` | RenderWare/D3D | `0x00806C72` | original (RenderWare/D3D) | 1782 |
| 2152 | `0x00805F40` | RenderWare/D3D | `0x0080A0D9` | original (RenderWare/D3D) | 1782 |
| 2153 | `0x008067B0` | RenderWare/D3D | `0x00806159` | original (RenderWare/D3D) | 1782 |
| 2154 | `0x0080F0D0` | RenderWare/D3D | `0x008062E2` | original (RenderWare/D3D) | 1782 |
| 2155 | `0x00805950` | RenderWare/D3D | `0x0080F160` | original (RenderWare/D3D) | 1782 |
| 2156 | `0x0080F460` | RenderWare/D3D | `0x0080F167` | original (RenderWare/D3D) | 1782 |
| 2157 | `0x00805890` | RenderWare/D3D | `0x0080F469` | original (RenderWare/D3D) | 1782 |
| 2158 | `0x0080F690` | RenderWare/D3D | `0x0080F37D` | original (RenderWare/D3D) | 1782 |
| 2159 | `0x0080F970` | RenderWare/D3D | `0x0080F38F` | original (RenderWare/D3D) | 1782 |
| 2160 | `0x0080FBA0` | RenderWare/D3D | `0x0080F3A1` | original (RenderWare/D3D) | 1782 |
| 2161 | `0x0080FC30` | RenderWare/D3D | `0x0080F3B3` | original (RenderWare/D3D) | 1782 |
| 2162 | `0x008058F0` | RenderWare/D3D | `0x0080FCC4` | original (RenderWare/D3D) | 1782 |
| 2163 | `0x008103D0` | RenderWare/D3D | `0x0080FF97` | original (RenderWare/D3D) | 1782 |
| 2164 | `0x008064D0` | RenderWare/D3D | `0x008062FE` | original (RenderWare/D3D) | 1782 |
| 2165 | `0x007EFAC0` | RenderWare/D3D | `0x0080A0EE` | original (RenderWare/D3D) | 1782 |
| 2166 | `0x0080A1A0` | RenderWare/D3D | `0x007EFE7F` | original (RenderWare/D3D) | 1782 |
| 2167 | `0x0080E9A0` | RenderWare/D3D | `0x0080A1CE` | original (RenderWare/D3D) | 1782 |
| 2168 | `0x007EFB10` | RenderWare/D3D | `0x0080A200` | original (RenderWare/D3D) | 1782 |
| 2169 | `0x0080E020` | RenderWare/D3D | `0x007EFE7F` | original (RenderWare/D3D) | 1782 |
| 2170 | `0x00807C90` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2171 | `0x0080FFE0` | RenderWare/D3D | `0x00807CFD` | original (RenderWare/D3D) | 1782 |
| 2172 | `0x0080FFFF` | RenderWare/D3D | `0x00807CFD` | original (RenderWare/D3D) | 1782 |
| 2173 | `0x0074D8E0` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2174 | `0x00758970` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2175 | `0x0074BFA0` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2176 | `0x00749760` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2177 | `0x00752270` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2178 | `0x00761C30` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2179 | `0x0074FEE0` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2180 | `0x0075E310` | RenderWare/D3D | `0x0074FEEE` | original (juego) | 1782 |
| 2181 | `0x0074EAB0` | juego | `0x0075E3A2` | original (RenderWare/D3D) | 1782 |
| 2182 | `0x0074EB80` | juego | `0x0075E3AB` | original (RenderWare/D3D) | 1782 |
| 2183 | `0x007582E0` | RenderWare/D3D | `0x0074EBAA` | original (juego) | 1782 |
| 2184 | `0x00757890` | RenderWare/D3D | `0x0080636B` | original (RenderWare/D3D) | 1782 |
| 2185 | `0x0075E2A0` | RenderWare/D3D | `0x0074EBD5` | original (juego) | 1782 |
| 2186 | `0x00755D80` | RenderWare/D3D | `0x0074EBDD` | original (juego) | 1782 |
| 2187 | `0x007FAD20` | RenderWare/D3D | `0x00755E0E` | original (RenderWare/D3D) | 1782 |
| 2188 | `0x00760CF0` | RenderWare/D3D | `0x0074EBE6` | original (juego) | 1782 |
| 2189 | `0x0074EAD0` | juego | `0x0075E3B4` | original (RenderWare/D3D) | 1782 |
| 2190 | `0x0075E9F0` | RenderWare/D3D | `0x0074EAFA` | original (juego) | 1782 |
| 2191 | `0x0075E540` | RenderWare/D3D | `0x0080636B` | original (RenderWare/D3D) | 1782 |
| 2192 | `0x0075E270` | RenderWare/D3D | `0x0074EB25` | original (juego) | 1782 |
| 2193 | `0x007637E0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2194 | `0x00750280` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2195 | `0x00758890` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2196 | `0x007C68E0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2197 | `0x007F2BE0` | RenderWare/D3D | `0x007C68FE` | original (RenderWare/D3D) | 1782 |
| 2198 | `0x00808470` | RenderWare/D3D | `0x007F2BEF` | original (RenderWare/D3D) | 1782 |
| 2199 | `0x007C8980` | RenderWare/D3D | `0x007C6926` | original (RenderWare/D3D) | 1782 |
| 2200 | `0x007CB2B0` | RenderWare/D3D | `0x007C8993` | original (RenderWare/D3D) | 1782 |
| 2201 | `0x007CB240` | RenderWare/D3D | `0x0080636B` | original (RenderWare/D3D) | 1782 |
| 2202 | `0x007CCCD0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2203 | `0x007C4670` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2204 | `0x00810BF0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2205 | `0x00815E60` | RenderWare/D3D | `0x00810C2B` | original (RenderWare/D3D) | 1782 |
| 2206 | `0x00806B30` | RenderWare/D3D | `0x00815EC6` | original (RenderWare/D3D) | 1782 |
| 2207 | `0x008263C0` | CRT y bibliotecas | `0x00806B84` | original (RenderWare/D3D) | 1782 |
| 2208 | `0x00757390` | RenderWare/D3D | `0x00815ECE` | original (RenderWare/D3D) | 1782 |
| 2209 | `0x00757380` | RenderWare/D3D | `0x00815EDE` | original (RenderWare/D3D) | 1782 |
| 2210 | `0x007573E0` | RenderWare/D3D | `0x00815EE9` | original (RenderWare/D3D) | 1782 |
| 2211 | `0x007FACC0` | RenderWare/D3D | `0x0081600C` | original (RenderWare/D3D) | 1782 |
| 2212 | `0x007CB9B0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2213 | `0x007CED40` | RenderWare/D3D | `0x007CB9F1` | original (RenderWare/D3D) | 1782 |
| 2214 | `0x00809160` | RenderWare/D3D | `0x007CED55` | original (RenderWare/D3D) | 1782 |
| 2215 | `0x00749000` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2216 | `0x007F12C0` | RenderWare/D3D | `0x007F6919` | original (RenderWare/D3D) | 1782 |
| 2217 | `0x007ED5F0` | RenderWare/D3D | `0x007F694E` | original (RenderWare/D3D) | 1782 |
| 2218 | `0x00745530` | juego | `0x00619D45` | original (juego) | 1782 |
| 2219 | `0x007F9C30` | RenderWare/D3D | `0x00619D45` | original (juego) | 1782 |
| 2220 | `0x007F3530` | RenderWare/D3D | `0x00619D4C` | original (juego) | 1782 |
| 2221 | `0x007F3560` | RenderWare/D3D | `0x00619D53` | original (juego) | 1782 |
| 2222 | `0x005BD600` | juego | `0x005BF3A6` | original (juego) | 1782 |
| 2223 | `0x005BA060` | juego | `0x005BD60B` | original (juego) | 1782 |
| 2224 | `0x00821F40` | CRT y bibliotecas | `0x005BA0E8` | original (juego) | 1782 |
| 2225 | `0x00730830` | juego | `0x005BD610` | original (juego) | 1782 |
| 2226 | `0x00731F20` | juego | `0x005BD615` | original (juego) | 1782 |
| 2227 | `0x00731AD0` | juego | `0x00731F6B` | original (juego) | 1782 |
| 2228 | `0x00731B80` | juego | `0x00731F8B` | original (juego) | 1782 |
| 2229 | `0x007F34D0` | RenderWare/D3D | `0x00731FDF` | original (juego) | 1782 |
| 2230 | `0x007F3500` | RenderWare/D3D | `0x00731FE9` | original (juego) | 1782 |
| 2231 | `0x00733A20` | juego | `0x005BD61A` | original (juego) | 1782 |
| 2232 | `0x007EE4F0` | RenderWare/D3D | `0x005BD62B` | original (juego) | 1782 |
| 2233 | `0x007EE200` | RenderWare/D3D | `0x007EE586` | original (RenderWare/D3D) | 1782 |
| 2234 | `0x007503E0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2235 | `0x00749880` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2236 | `0x007F0410` | RenderWare/D3D | `0x005BD636` | original (juego) | 1782 |
| 2237 | `0x007F0360` | RenderWare/D3D | `0x007F043C` | original (RenderWare/D3D) | 1782 |
| 2238 | `0x007C4750` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2239 | `0x007321B0` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2240 | `0x00804EF0` | RenderWare/D3D | `0x005BD63D` | original (juego) | 1782 |
| 2241 | `0x007F0910` | RenderWare/D3D | `0x00804F36` | original (RenderWare/D3D) | 1782 |
| 2242 | `0x007FB230` | RenderWare/D3D | `0x005BD648` | original (juego) | 1782 |
| 2243 | `0x004CCE60` | juego | `0x007FB294` | original (RenderWare/D3D) | 1782 |
| 2244 | `0x004CC5C0` | juego | `0x004CCEAB` | original (juego) | 1782 |
| 2245 | `0x004CB530` | juego | `0x004CCFCA` | original (juego) | 1782 |
| 2246 | `0x004C9A60` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2247 | `0x004CCD70` | juego | `0x004CCF31` | original (juego) | 1782 |
| 2248 | `0x004CBD80` | juego | `0x004CCD9F` | original (juego) | 1782 |
| 2249 | `0x005BC520` | juego | `0x005BD69E` | original (juego) | 1782 |
| 2250 | `0x0050AB10` | juego | `0x005BC60F` | original (juego) | 1782 |
| 2251 | `0x0056E0D0` | juego | `0x005BC72E` | original (juego) | 1782 |
| 2252 | `0x0050BF40` | juego | `0x005BC85C` | original (juego) | 1782 |
| 2253 | `0x0050C100` | juego | `0x005BD6AE` | original (juego) | 1782 |
| 2254 | `0x0059BD10` | juego | `0x0050C11E` | original (juego) | 1782 |
| 2255 | `0x0071D710` | juego | `0x0050C12A` | original (juego) | 1782 |
| 2256 | `0x007EE2A0` | RenderWare/D3D | `0x005BD6BF` | original (juego) | 1782 |
| 2257 | `0x007EE1D0` | RenderWare/D3D | `0x005BD6D0` | original (juego) | 1782 |
| 2258 | `0x0072FC70` | juego | `0x005BD6E7` | original (juego) | 1782 |
| 2259 | `0x007F2D20` | RenderWare/D3D | `0x0072FC87` | original (juego) | 1782 |
| 2260 | `0x007EE410` | RenderWare/D3D | `0x0072FD65` | original (juego) | 1782 |
| 2261 | `0x0074F760` | juego | `0x005BD721` | original (juego) | 1782 |
| 2262 | `0x0074E1B0` | juego | `0x0074F7DB` | original (juego) | 1782 |
| 2263 | `0x008086B0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2264 | `0x00758850` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2265 | `0x00811B00` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2266 | `0x0074F210` | juego | `0x0074F9D7` | original (juego) | 1782 |
| 2267 | `0x00758A90` | RenderWare/D3D | `0x0074F275` | original (juego) | 1782 |
| 2268 | `0x0075D970` | RenderWare/D3D | `0x0074F485` | original (juego) | 1782 |
| 2269 | `0x00758B80` | RenderWare/D3D | `0x0075D987` | original (RenderWare/D3D) | 1782 |
| 2270 | `0x00801D50` | RenderWare/D3D | `0x00758BB6` | original (RenderWare/D3D) | 1782 |
| 2271 | `0x0072F430` | juego | `0x00801DEF` | original (RenderWare/D3D) | 1782 |
| 2272 | `0x00750F20` | RenderWare/D3D | `0x005BD759` | original (juego) | 1782 |
| 2273 | `0x005BA520` | juego | `0x005BD765` | original (juego) | 1782 |
| 2274 | `0x00752110` | RenderWare/D3D | `0x005BA539` | original (juego) | 1782 |
| 2275 | `0x00750AA0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2276 | `0x007498B0` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2277 | `0x00755BC0` | RenderWare/D3D | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2278 | `0x00751A90` | RenderWare/D3D | `0x005BA56B` | original (juego) | 1782 |
| 2279 | `0x00751A70` | RenderWare/D3D | `0x005BA5B4` | original (juego) | 1782 |
| 2280 | `0x007F1010` | RenderWare/D3D | `0x005BA5F2` | original (juego) | 1782 |
| 2281 | `0x007F1FD0` | RenderWare/D3D | `0x007F102D` | original (RenderWare/D3D) | 1782 |
| 2282 | `0x007EDB90` | RenderWare/D3D | `0x007F2012` | original (RenderWare/D3D) | 1782 |
| 2283 | `0x007F1D00` | RenderWare/D3D | `0x007F2061` | original (RenderWare/D3D) | 1782 |
| 2284 | `0x00751910` | RenderWare/D3D | `0x005BA5FF` | original (juego) | 1782 |
| 2285 | `0x00734610` | juego | `0x005BD76A` | original (juego) | 1782 |
| 2286 | `0x005BA690` | juego | `0x005BD76F` | original (juego) | 1782 |
| 2287 | `0x00731C80` | juego | `0x005BA69C` | original (juego) | 1782 |
| 2288 | `0x007320B0` | juego | `0x005BA6A9` | original (juego) | 1782 |
| 2289 | `0x00731DD0` | juego | `0x007320F5` | original (juego) | 1782 |
| 2290 | `0x007ED2D0` | RenderWare/D3D | `0x00731E01` | original (juego) | 1782 |
| 2291 | `0x007ED0F0` | RenderWare/D3D | `0x007ED2F0` | original (RenderWare/D3D) | 1782 |
| 2292 | `0x007ED1E0` | RenderWare/D3D | `0x007ED193` | original (RenderWare/D3D) | 1782 |
| 2293 | `0x00730FC0` | juego | `0x00731E0E` | original (juego) | 1782 |
| 2294 | `0x00730E60` | juego | `0x0073102B` | original (juego) | 1782 |
| 2295 | `0x00561A80` | juego | `0x00730E8A` | original (juego) | 1782 |
| 2296 | `0x00561A40` | juego | `0x00730EA5` | original (juego) | 1782 |
| 2297 | `0x004CBDE0` | juego | `0x004CD8B7` | original (juego) | 1782 |
| 2298 | `0x00730510` | juego | `0x004CDA03` | original (juego) | 1782 |
| 2299 | `0x0072FF60` | juego | `0x00730581` | original (juego) | 1782 |
| 2300 | `0x007FB2D0` | RenderWare/D3D | `0x004CDCA9` | original (juego) | 1782 |
| 2301 | `0x004C9F90` | juego | `0x007FB2F8` | original (RenderWare/D3D) | 1782 |
| 2302 | `0x007FAEC0` | RenderWare/D3D | `0x004CDCF3` | original (juego) | 1782 |
| 2303 | `0x007F37C0` | RenderWare/D3D | `0x004CDD30` | original (juego) | 1782 |
| 2304 | `0x00749020` | juego | `0x008086FD` | original (RenderWare/D3D) | 1782 |
| 2305 | `0x007F38A0` | RenderWare/D3D | `0x004CDD84` | original (juego) | 1782 |
| 2306 | `0x007F3910` | RenderWare/D3D | `0x004CDD8F` | original (juego) | 1782 |
| 2307 | `0x007F3980` | RenderWare/D3D | `0x00731039` | original (juego) | 1782 |
| 2308 | `0x007ECD00` | RenderWare/D3D | `0x007ED30C` | original (RenderWare/D3D) | 1782 |
| 2309 | `0x00731D50` | juego | `0x00731E1D` | original (juego) | 1782 |
| 2310 | `0x007ECE20` | RenderWare/D3D | `0x007320FF` | original (juego) | 1782 |
| 2311 | `0x00731A00` | juego | `0x005BA6AF` | original (juego) | 1782 |
| 2312 | `0x007316A0` | juego | `0x005BA6B4` | original (juego) | 1782 |
| 2313 | `0x007F3A90` | RenderWare/D3D | `0x007316A5` | original (juego) | 1782 |
| 2314 | `0x007319C0` | juego | `0x005BA6BA` | original (juego) | 1782 |
| 2315 | `0x007F3A70` | RenderWare/D3D | `0x005BA6BA` | original (juego) | 1782 |
| 2316 | `0x007272B0` | juego | `0x005BA6D1` | original (juego) | 1782 |
| 2317 | `0x007F3AC0` | RenderWare/D3D | `0x007272CA` | original (juego) | 1782 |
| 2318 | `0x00731720` | juego | `0x007F3AD9` | original (RenderWare/D3D) | 1782 |
| 2319 | `0x007F39F0` | RenderWare/D3D | `0x00731738` | original (juego) | 1782 |
| 2320 | `0x007187C0` | juego | `0x005BA6EA` | original (juego) | 1782 |
| 2321 | `0x00536F80` | juego | `0x007187E8` | original (juego) | 1782 |
| 2322 | `0x005389B0` | juego | `0x00536F96` | original (juego) | 1782 |
| 2323 | `0x00823798` | CRT y bibliotecas | `0x005389C4` | original (juego) | 1782 |
| 2324 | `0x00823816` | CRT y bibliotecas | `0x0082380A` | original (CRT) | 1782 |
| 2325 | `0x008220AD` | CRT y bibliotecas | `0x00718818` | original (juego) | 1782 |
| 2326 | `0x008290D4` | CRT y bibliotecas | `0x008220DC` | original (CRT) | 1782 |
| 2327 | `0x00822802` | CRT y bibliotecas | `0x00829118` | original (CRT) | 1782 |
| 2328 | `0x00822789` | CRT y bibliotecas | `0x008291A8` | original (CRT) | 1782 |
| 2329 | `0x00829094` | CRT y bibliotecas | `0x008292C8` | original (CRT) | 1782 |
| 2330 | `0x0083375D` | CRT y bibliotecas | `0x0082969E` | original (CRT) | 1782 |
| 2331 | `0x00719380` | juego | `0x005BA6F9` | original (juego) | 1782 |
| 2332 | `0x00719400` | juego | `0x005BA70D` | original (juego) | 1782 |
| 2333 | `0x00719420` | juego | `0x005BA714` | original (juego) | 1782 |
| 2334 | `0x00719430` | juego | `0x005BA739` | original (juego) | 1782 |
| 2335 | `0x00719610` | juego | `0x005BA740` | original (juego) | 1782 |
| 2336 | `0x00719600` | juego | `0x005BA747` | original (juego) | 1782 |
| 2337 | `0x007194D0` | juego | `0x005BA758` | original (juego) | 1782 |
| 2338 | `0x007194E0` | juego | `0x005BA766` | original (juego) | 1782 |
| 2339 | `0x007195C0` | juego | `0x005BA76F` | original (juego) | 1782 |
| 2340 | `0x007195E0` | juego | `0x005BA797` | original (juego) | 1782 |
| 2341 | `0x007195B0` | juego | `0x005BA79E` | original (juego) | 1782 |
| 2342 | `0x00719490` | juego | `0x005BA7A5` | original (juego) | 1782 |
| 2343 | `0x007194F0` | juego | `0x005BA7AC` | original (juego) | 1782 |
| 2344 | `0x00719500` | juego | `0x005BA7B6` | original (juego) | 1782 |
| 2345 | `0x00719570` | juego | `0x005BA7BD` | original (juego) | 1782 |
| 2346 | `0x007316B0` | juego | `0x005BA7C2` | original (juego) | 1782 |
| 2347 | `0x005BA850` | juego | `0x005BD774` | original (juego) | 1782 |
| 2348 | `0x00588880` | juego | `0x005BD774` | original (juego) | 1782 |
| 2349 | `0x006FF8A0` | juego | `0x005BD779` | original (juego) | 1782 |
| 2350 | `0x00731990` | juego | `0x006FF8B5` | original (juego) | 1782 |
| 2351 | `0x00704630` | juego | `0x005BD77E` | original (juego) | 1782 |
| 2352 | `0x007043D0` | juego | `0x00704635` | original (juego) | 1782 |
| 2353 | `0x004CBE20` | juego | `0x004CC7C6` | original (juego) | 1782 |
| 2354 | `0x004CB9C0` | juego | `0x004CCF0F` | original (juego) | 1782 |
| 2355 | `0x007FED70` | RenderWare/D3D | `0x004CBA09` | original (juego) | 1782 |
| 2356 | `0x00703CC0` | juego | `0x00704621` | original (juego) | 1782 |
| 2357 | `0x00701450` | juego | `0x00704626` | original (juego) | 1782 |
| 2358 | `0x004CB7C0` | juego | `0x004CCEEC` | original (juego) | 1782 |
| 2359 | `0x007F2A50` | RenderWare/D3D | `0x005BD783` | original (juego) | 1782 |
| 2360 | `0x00507440` | juego | `0x00747FE9` | original (juego) | 3047 |
| 2361 | `0x004D9630` | juego | `0x00747FE9` | original (juego) | 3047 |
| 2362 | `0x0053BC60` | juego | `0x00748068` | original (juego) | 3125 |
| 2363 | `0x007489F0` | juego | `0x75EBD640` | otro | 3172 |
| 2364 | `0x006194A0` | juego | `0x00748A20` | original (juego) | 3172 |
| 2365 | `0x007453F0` | juego | `0x00748A20` | original (juego) | 3172 |
| 2366 | `0x00747180` | juego | `0x00748A35` | original (juego) | 3172 |
| 2367 | `0x007F9D50` | RenderWare/D3D | `0x007471AA` | original (juego) | 3172 |
| 2368 | `0x00801E00` | RenderWare/D3D | `0x007F9DF6` | original (RenderWare/D3D) | 3172 |
| 2369 | `0x005900B0` | juego | `0x00748AAF` | original (juego) | 3219 |
| 2370 | `0x00821B11` | CRT y bibliotecas | `0x0059010F` | original (juego) | 3219 |
| 2371 | `0x00727270` | juego | `0x005901F6` | original (juego) | 3219 |
| 2372 | `0x005902B0` | juego | `0x00748AB6` | original (juego) | 3219 |
| 2373 | `0x00619410` | juego | `0x005902DD` | original (juego) | 3219 |
| 2374 | `0x00745270` | juego | `0x005902DD` | original (juego) | 3219 |
| 2375 | `0x00590860` | juego | `0x00748ABB` | original (juego) | 3219 |
| 2376 | `0x007EE190` | RenderWare/D3D | `0x005908AD` | original (juego) | 3219 |
| 2377 | `0x007EF370` | RenderWare/D3D | `0x005908AD` | original (juego) | 3219 |
| 2378 | `0x00809550` | RenderWare/D3D | `0x007EF381` | original (RenderWare/D3D) | 3219 |
| 2379 | `0x007521A0` | RenderWare/D3D | `0x00750ADB` | original (RenderWare/D3D) | 3219 |
| 2380 | `0x007EE5A0` | RenderWare/D3D | `0x0075049B` | original (RenderWare/D3D) | 3219 |
| 2381 | `0x007F17E0` | RenderWare/D3D | `0x007EE6F8` | original (RenderWare/D3D) | 3219 |
| 2382 | `0x007F1500` | RenderWare/D3D | `0x007F1805` | original (RenderWare/D3D) | 3219 |
| 2383 | `0x007F14A0` | RenderWare/D3D | `0x007F1820` | original (RenderWare/D3D) | 3219 |
| 2384 | `0x007EE740` | RenderWare/D3D | `0x007EE6FE` | original (RenderWare/D3D) | 3219 |
| 2385 | `0x00808F60` | RenderWare/D3D | `0x007EE727` | original (RenderWare/D3D) | 3219 |
| 2386 | `0x007F0990` | RenderWare/D3D | `0x007504DE` | original (RenderWare/D3D) | 3219 |
| 2387 | `0x008097D0` | RenderWare/D3D | `0x007F09A7` | original (RenderWare/D3D) | 3219 |
| 2388 | `0x007F7530` | RenderWare/D3D | `0x007506BB` | original (RenderWare/D3D) | 3219 |
| 2389 | `0x007F8F20` | RenderWare/D3D | `0x007EF391` | original (RenderWare/D3D) | 3219 |
| 2390 | `0x007F2070` | RenderWare/D3D | `0x007F8F4A` | original (RenderWare/D3D) | 3219 |
| 2391 | `0x007FA390` | RenderWare/D3D | `0x007F8FB6` | original (RenderWare/D3D) | 3219 |
| 2392 | `0x007F7F70` | RenderWare/D3D | `0x007F9218` | original (RenderWare/D3D) | 3219 |
| 2393 | `0x007FB5F0` | RenderWare/D3D | `0x007F813E` | original (RenderWare/D3D) | 3219 |
| 2394 | `0x007FAC10` | RenderWare/D3D | `0x007FB5FF` | original (RenderWare/D3D) | 3219 |
| 2395 | `0x0080DFB0` | RenderWare/D3D | `0x007F8143` | original (RenderWare/D3D) | 3219 |
| 2396 | `0x007F5840` | RenderWare/D3D | `0x007F8148` | original (RenderWare/D3D) | 3219 |
| 2397 | `0x004CB640` | juego | `0x007F814D` | original (RenderWare/D3D) | 3219 |
| 2398 | `0x00801F90` | RenderWare/D3D | `0x007F8152` | original (RenderWare/D3D) | 3219 |
| 2399 | `0x004CC970` | juego | `0x007F929E` | original (RenderWare/D3D) | 4594 |
| 2400 | `0x007F58D0` | RenderWare/D3D | `0x007F92A7` | original (RenderWare/D3D) | 4594 |
| 2401 | `0x007FD100` | RenderWare/D3D | `0x007F92B0` | original (RenderWare/D3D) | 4594 |
| 2402 | `0x005BA120` | juego | `0x007F9327` | original (RenderWare/D3D) | 4594 |
| 2403 | `0x0074EA80` | juego | `0x005BA12B` | original (juego) | 4594 |
| 2404 | `0x00807C80` | RenderWare/D3D | `0x007EF39E` | original (RenderWare/D3D) | 4594 |
| 2405 | `0x00734750` | juego | `0x005908B9` | original (juego) | 4594 |
| 2406 | `0x007FE420` | RenderWare/D3D | `0x0073475C` | original (juego) | 4594 |
| 2407 | `0x007FEB70` | RenderWare/D3D | `0x007FE7FC` | original (RenderWare/D3D) | 4594 |
| 2408 | `0x007FEC70` | RenderWare/D3D | `0x007FE8A9` | original (RenderWare/D3D) | 4594 |
| 2409 | `0x007FDB80` | RenderWare/D3D | `0x007FE7CB` | original (RenderWare/D3D) | 4594 |
| 2410 | `0x007FE0A0` | RenderWare/D3D | `0x007FE8B9` | original (RenderWare/D3D) | 4594 |
| 2411 | `0x007FECC0` | RenderWare/D3D | `0x007FE8C9` | original (RenderWare/D3D) | 4594 |
| 2412 | `0x007FED00` | RenderWare/D3D | `0x007FE8E7` | original (RenderWare/D3D) | 4594 |
| 2413 | `0x0058FF60` | juego | `0x005908CE` | original (juego) | 4594 |
| 2414 | `0x00727350` | juego | `0x0058FF68` | original (juego) | 4594 |
| 2415 | `0x00728350` | juego | `0x00590020` | original (juego) | 4594 |
| 2416 | `0x00727420` | juego | `0x00728365` | original (juego) | 4594 |
| 2417 | `0x007FDCD0` | RenderWare/D3D | `0x007FE7DC` | original (RenderWare/D3D) | 4594 |
| 2418 | `0x00734E90` | juego | `0x0072839B` | original (juego) | 4594 |
| 2419 | `0x007FBB00` | RenderWare/D3D | `0x0072839B` | original (juego) | 4594 |
| 2420 | `0x007F5B10` | RenderWare/D3D | `0x007FBBEC` | original (RenderWare/D3D) | 4594 |
| 2421 | `0x007FB810` | RenderWare/D3D | `0x007FBCCB` | original (RenderWare/D3D) | 4594 |
| 2422 | `0x007F9F70` | RenderWare/D3D | `0x007FB81B` | original (RenderWare/D3D) | 4594 |
| 2423 | `0x007F9FB0` | RenderWare/D3D | `0x007FB822` | original (RenderWare/D3D) | 4594 |
| 2424 | `0x007F9FF0` | RenderWare/D3D | `0x007FB829` | original (RenderWare/D3D) | 4594 |
| 2425 | `0x007FDE50` | RenderWare/D3D | `0x007FB848` | original (RenderWare/D3D) | 4594 |
| 2426 | `0x007FA030` | RenderWare/D3D | `0x007FB8D6` | original (RenderWare/D3D) | 4594 |
| 2427 | `0x00727B60` | juego | `0x0059009E` | original (juego) | 4594 |
| 2428 | `0x007EE180` | RenderWare/D3D | `0x005908F7` | original (juego) | 4594 |
| 2429 | `0x007EF340` | RenderWare/D3D | `0x005908F7` | original (juego) | 4594 |
| 2430 | `0x00619440` | juego | `0x00590902` | original (juego) | 4594 |
| 2431 | `0x00745240` | juego | `0x00590902` | original (juego) | 4594 |
| 2432 | `0x007EE370` | RenderWare/D3D | `0x0074526A` | original (juego) | 4594 |
| 2433 | `0x007FB1A0` | RenderWare/D3D | `0x007EE388` | original (RenderWare/D3D) | 4594 |
| 2434 | `0x00807E50` | RenderWare/D3D | `0x007FB1B2` | original (RenderWare/D3D) | 4594 |
| 2435 | `0x007F5800` | RenderWare/D3D | `0x007F9C12` | original (RenderWare/D3D) | 4719 |
| 2436 | `0x00590990` | juego | `0x00748AC0` | original (juego) | 5563 |
| 2437 | `0x0058FF10` | juego | `0x00748AC6` | original (juego) | 9719 |
| 2438 | `0x00727240` | juego | `0x0058FF2C` | original (juego) | 9719 |
| 2439 | `0x007F3820` | RenderWare/D3D | `0x0072724F` | original (juego) | 9719 |
| 2440 | `0x00731850` | juego | `0x0058FF41` | original (juego) | 9719 |
| 2441 | `0x00731E90` | juego | `0x0058FF51` | original (juego) | 9719 |
| 2442 | `0x007F3730` | RenderWare/D3D | `0x00731EC3` | original (juego) | 9719 |
| 2443 | `0x00731680` | juego | `0x007F3753` | original (RenderWare/D3D) | 9719 |
| 2444 | `0x007F36A0` | RenderWare/D3D | `0x00731ECB` | original (juego) | 9719 |
| 2445 | `0x00808740` | RenderWare/D3D | `0x007F3841` | original (RenderWare/D3D) | 9719 |
| 2446 | `0x00749030` | juego | `0x0080875D` | original (RenderWare/D3D) | 9719 |
| 2447 | `0x007FB020` | RenderWare/D3D | `0x007F3868` | original (RenderWare/D3D) | 9719 |
| 2448 | `0x004C9A80` | juego | `0x0080875D` | original (RenderWare/D3D) | 9719 |
| 2449 | `0x004CBB00` | juego | `0x007FB03D` | original (RenderWare/D3D) | 9719 |
| 2450 | `0x00730B70` | juego | `0x004CBC22` | original (juego) | 9719 |
| 2451 | `0x00731CD0` | juego | `0x0058FF57` | original (juego) | 9735 |
| 2452 | `0x00541DD0` | juego | `0x00748B1C` | original (juego) | 14750 |
| 2453 | `0x00746060` | juego | `0x0053F3CB` | original (juego) | 14750 |
| 2454 | `0x00746ED0` | juego | `0x0053F41C` | original (juego) | 14750 |
| 2455 | `0x00746A10` | juego | `0x00541DE3` | original (juego) | 14750 |
| 2456 | `0x0052DD90` | juego | `0x00541DF0` | original (juego) | 14750 |
| 2457 | `0x00531140` | juego | `0x00541DFA` | original (juego) | 14750 |
| 2458 | `0x00747CD0` | juego | `0x0053115A` | original (juego) | 14750 |
| 2459 | `0x00619560` | juego | `0x00747D6A` | original (juego) | 14750 |
| 2460 | `0x00744880` | juego | `0x00747D6A` | original (juego) | 14750 |
| 2461 | `0x0052DDB0` | juego | `0x00531199` | original (juego) | 14750 |
| 2462 | `0x005314A0` | juego | `0x00541E04` | original (juego) | 14750 |
| 2463 | `0x0052EF30` | juego | `0x005314E5` | original (juego) | 14750 |
| 2464 | `0x0052F020` | juego | `0x005315F7` | original (juego) | 14750 |
| 2465 | `0x00541C40` | juego | `0x00541E10` | original (juego) | 14750 |
| 2466 | `0x0053F530` | juego | `0x00541C6C` | original (juego) | 14750 |
| 2467 | `0x0053EFF0` | juego | `0x00541CB7` | original (juego) | 14750 |
| 2468 | `0x00746740` | juego | `0x00748BDD` | original (juego) | 14750 |
| 2469 | `0x00747660` | juego | `0x00748BFE` | original (juego) | 14750 |
| 2470 | `0x007466D0` | juego | `0x00747799` | original (juego) | 14860 |
| 2471 | `0x00745FB0` | juego | `0x00748446` | original (juego) | 14875 |
| 2472 | `0x005BA160` | juego | `0x00748C35` | original (juego) | 15688 |
| 2473 | `0x005D5AC0` | juego | `0x005BA171` | original (juego) | 15688 |
| 2474 | `0x005DA020` | juego | `0x005D5AC5` | original (juego) | 15688 |
| 2475 | `0x005D9F80` | juego | `0x005DA03B` | original (juego) | 15688 |
| 2476 | `0x007573A0` | RenderWare/D3D | `0x005D9FE3` | original (juego) | 15688 |
| 2477 | `0x005D8980` | juego | `0x005DA049` | original (juego) | 15688 |
| 2478 | `0x005D9060` | juego | `0x005DA099` | original (juego) | 15688 |
| 2479 | `0x005D91B0` | juego | `0x005DA0D5` | original (juego) | 15688 |
| 2480 | `0x005D9300` | juego | `0x005DA111` | original (juego) | 15688 |
| 2481 | `0x005DD6B0` | juego | `0x005BA17A` | original (juego) | 15688 |
| 2482 | `0x005D7EC0` | juego | `0x005BA183` | original (juego) | 15688 |
| 2483 | `0x005D7D90` | juego | `0x005D7EC5` | original (juego) | 15688 |
| 2484 | `0x005D7AA0` | juego | `0x005D7D95` | original (juego) | 15688 |
| 2485 | `0x005D7100` | juego | `0x005D7ECF` | original (juego) | 15688 |
| 2486 | `0x005D6750` | juego | `0x005D7105` | original (juego) | 15688 |
| 2487 | `0x007573B0` | RenderWare/D3D | `0x005D67E9` | original (juego) | 15688 |
| 2488 | `0x005617E0` | juego | `0x005BA18F` | original (juego) | 15688 |
| 2489 | `0x005617A0` | juego | `0x005618A6` | original (juego) | 15688 |
| 2490 | `0x005BFA90` | juego | `0x00748C44` | original (juego) | 15688 |
| 2491 | `0x005BA8C0` | juego | `0x005BFA95` | original (juego) | 15688 |
| 2492 | `0x0040FCF0` | juego | `0x005BA8F1` | original (juego) | 15688 |
| 2493 | `0x0040EDE0` | juego | `0x005BA935` | original (juego) | 15688 |
| 2494 | `0x0040F870` | juego | `0x005BAB61` | original (juego) | 15688 |
| 2495 | `0x0040FD10` | juego | `0x005BAB91` | original (juego) | 15688 |
| 2496 | `0x0040EF10` | juego | `0x005BAC1B` | original (juego) | 15688 |
| 2497 | `0x005BD830` | juego | `0x005BFA9F` | original (juego) | 15688 |
| 2498 | `0x0082244B` | CRT y bibliotecas | `0x005BE1B1` | original (juego) | 15688 |
| 2499 | `0x006F4F30` | juego | `0x005BE1E7` | original (juego) | 15688 |
| 2500 | `0x00823CEE` | CRT y bibliotecas | `0x005BE201` | original (juego) | 15688 |
| 2501 | `0x0082BF62` | CRT y bibliotecas | `0x00823D1D` | original (CRT) | 15688 |
| 2502 | `0x008346E2` | CRT y bibliotecas | `0x0082BF8E` | original (CRT) | 15688 |
| 2503 | `0x00835E89` | CRT y bibliotecas | `0x00834A49` | original (CRT) | 15688 |
| 2504 | `0x00835E2E` | CRT y bibliotecas | `0x00835EC9` | original (CRT) | 15688 |
| 2505 | `0x00835DD0` | CRT y bibliotecas | `0x00835ED9` | original (CRT) | 15688 |
| 2506 | `0x00835DAF` | CRT y bibliotecas | `0x00835DE4` | original (CRT) | 15688 |
| 2507 | `0x0083660B` | CRT y bibliotecas | `0x00834A86` | original (CRT) | 15688 |
| 2508 | `0x008363D3` | CRT y bibliotecas | `0x0083667E` | original (CRT) | 15688 |
| 2509 | `0x00832B79` | CRT y bibliotecas | `0x0082BFB2` | original (CRT) | 15688 |
| 2510 | `0x00832A21` | CRT y bibliotecas | `0x00832B8B` | original (CRT) | 15688 |
| 2511 | `0x00832966` | CRT y bibliotecas | `0x00832A93` | original (CRT) | 15688 |
| 2512 | `0x008328F4` | CRT y bibliotecas | `0x00832A9F` | original (CRT) | 15688 |
| 2513 | `0x008329A6` | CRT y bibliotecas | `0x00832B37` | original (CRT) | 15688 |
| 2514 | `0x00832875` | CRT y bibliotecas | `0x0083292C` | original (CRT) | 15688 |
| 2515 | `0x008328A7` | CRT y bibliotecas | `0x0083293B` | original (CRT) | 15688 |
| 2516 | `0x0083298D` | CRT y bibliotecas | `0x00832A73` | original (CRT) | 15688 |
| 2517 | `0x0082258E` | CRT y bibliotecas | `0x005BE255` | original (juego) | 15688 |
| 2518 | `0x00822506` | CRT y bibliotecas | `0x005BE255` | original (juego) | 15688 |
| 2519 | `0x008227C3` | CRT y bibliotecas | `0x008299FB` | original (CRT) | 15688 |
| 2520 | `0x006F5080` | juego | `0x005BE451` | original (juego) | 15688 |
| 2521 | `0x006D0460` | juego | `0x006F5231` | original (juego) | 15688 |
| 2522 | `0x006F5300` | juego | `0x005BDBF6` | original (juego) | 15688 |
| 2523 | `0x006F5290` | juego | `0x005BD9F7` | original (juego) | 15688 |
| 2524 | `0x005B9EB0` | juego | `0x005BE17E` | original (juego) | 15688 |
| 2525 | `0x005B9E40` | juego | `0x005BE198` | original (juego) | 15688 |
| 2526 | `0x00470000` | juego | `0x005BDFE0` | original (juego) | 15688 |
| 2527 | `0x00822130` | CRT y bibliotecas | `0x0047000D` | original (juego) | 15688 |
| 2528 | `0x00829B80` | CRT y bibliotecas | `0x0047000D` | original (juego) | 15688 |
| 2529 | `0x0055F420` | juego | `0x005BFADB` | original (juego) | 15688 |
| 2530 | `0x0055D0E0` | juego | `0x0055F428` | original (juego) | 15688 |
| 2531 | `0x00832CA8` | CRT y bibliotecas | `0x00826B72` | original (CRT) | 15688 |
| 2532 | `0x00832B8F` | CRT y bibliotecas | `0x00832CDB` | original (CRT) | 15688 |
| 2533 | `0x0055EB90` | juego | `0x0055F42F` | original (juego) | 15688 |
| 2534 | `0x0055D220` | juego | `0x0055ECEE` | original (juego) | 15688 |
| 2535 | `0x0055F2B0` | juego | `0x005BFADB` | original (juego) | 15688 |
| 2536 | `0x005BF9D0` | juego | `0x005BFAE0` | original (juego) | 15688 |
| 2537 | `0x005BB890` | juego | `0x005BFA81` | original (juego) | 15688 |
| 2538 | `0x005BF400` | juego | `0x005BFAE0` | original (juego) | 15688 |
| 2539 | `0x004684F0` | juego | `0x005BF40E` | original (juego) | 15688 |
| 2540 | `0x004650F0` | juego | `0x00468533` | original (juego) | 15688 |
| 2541 | `0x004650A0` | juego | `0x00821D57` | original (CRT) | 15688 |
| 2542 | `0x006040C0` | juego | `0x004650B6` | original (juego) | 15688 |
| 2543 | `0x00600530` | juego | `0x006040C5` | original (juego) | 15688 |
| 2544 | `0x00600840` | juego | `0x005BF415` | original (juego) | 15688 |
| 2545 | `0x005BB9F0` | juego | `0x00600851` | original (juego) | 15688 |
| 2546 | `0x00607A70` | juego | `0x005BF426` | original (juego) | 15688 |
| 2547 | `0x006043A0` | juego | `0x00607A81` | original (juego) | 15688 |
| 2548 | `0x006076B0` | juego | `0x005BF482` | original (juego) | 15688 |
| 2549 | `0x00600570` | juego | `0x00607A0C` | original (juego) | 15688 |
| 2550 | `0x006006B0` | juego | `0x00607A2E` | original (juego) | 15688 |
| 2551 | `0x00607D30` | juego | `0x005BF616` | original (juego) | 15688 |
| 2552 | `0x00607050` | juego | `0x00607D75` | original (juego) | 15688 |
| 2553 | `0x005BBAC0` | juego | `0x005BFAE7` | original (juego) | 15703 |
| 2554 | `0x0059C910` | juego | `0x005BC06B` | original (juego) | 15703 |
| 2555 | `0x005BC090` | juego | `0x005BFAEF` | original (juego) | 15703 |
| 2556 | `0x0045A1B0` | juego | `0x005BFAF4` | original (juego) | 15703 |
| 2557 | `0x00459390` | juego | `0x005BFAF4` | original (juego) | 15703 |
| 2558 | `0x005B9C60` | juego | `0x005BFAFE` | original (juego) | 15703 |
| 2559 | `0x00590310` | juego | `0x005B9C68` | original (juego) | 15703 |
| 2560 | `0x004D9930` | juego | `0x005B9C72` | original (juego) | 15703 |
| 2561 | `0x004DFB10` | juego | `0x004D996D` | original (juego) | 15703 |
| 2562 | `0x004E0930` | juego | `0x004D999F` | original (juego) | 15703 |
| 2563 | `0x004E08F0` | juego | `0x004D99B8` | original (juego) | 15703 |
| 2564 | `0x004DFB60` | juego | `0x004E08F8` | original (juego) | 15703 |
| 2565 | `0x004E0590` | juego | `0x004E08FD` | original (juego) | 15703 |
| 2566 | `0x005389E0` | juego | `0x004E05B9` | original (juego) | 15703 |
| 2567 | `0x00804130` | RenderWare/D3D | `0x005389E8` | original (juego) | 15703 |
| 2568 | `0x00826261` | CRT y bibliotecas | `0x005389F0` | original (juego) | 15703 |
| 2569 | `0x00826298` | CRT y bibliotecas | `0x0082628F` | original (CRT) | 15703 |
| 2570 | `0x004DFBA0` | juego | `0x004E0639` | original (juego) | 15703 |
| 2571 | `0x004DFBD0` | juego | `0x004E0908` | original (juego) | 15703 |
| 2572 | `0x004DFC70` | juego | `0x004E0913` | original (juego) | 15703 |
| 2573 | `0x004E0C50` | juego | `0x004D99C7` | original (juego) | 15703 |
| 2574 | `0x004E0970` | juego | `0x004E0C58` | original (juego) | 15703 |
| 2575 | `0x004E09F0` | juego | `0x004E0C63` | original (juego) | 15703 |
| 2576 | `0x004D9640` | juego | `0x004D9A4D` | original (juego) | 15703 |
| 2577 | `0x004D8490` | juego | `0x004D9791` | original (juego) | 15703 |
| 2578 | `0x004F1800` | juego | `0x004D9A8A` | original (juego) | 15719 |
| 2579 | `0x004D7890` | juego | `0x004F181B` | original (juego) | 15719 |
| 2580 | `0x004F22F0` | juego | `0x004D9A9F` | original (juego) | 15719 |
| 2581 | `0x004D7A70` | juego | `0x004F23AC` | original (juego) | 15719 |
| 2582 | `0x004F1C70` | juego | `0x004F23BC` | original (juego) | 15719 |
| 2583 | `0x004F0B10` | juego | `0x004D9B36` | original (juego) | 16438 |
| 2584 | `0x004D7C60` | juego | `0x004D9B6B` | original (juego) | 16438 |
| 2585 | `0x004EEC80` | juego | `0x004D7CEE` | original (juego) | 16438 |
| 2586 | `0x004F4BA0` | juego | `0x004D9B84` | original (juego) | 16438 |
| 2587 | `0x004F4A20` | juego | `0x75EB5D49` | otro | 16438 |
| 2588 | `0x004F4690` | juego | `0x004F4AD5` | original (juego) | 16438 |
| 2589 | `0x004F35B0` | juego | `0x004D9BBA` | original (juego) | 16532 |
| 2590 | `0x00502B80` | juego | `0x004F35D3` | original (juego) | 16532 |
| 2591 | `0x004E7C70` | juego | `0x004F35DB` | original (juego) | 16547 |
| 2592 | `0x008195C0` | RenderWare/D3D | `0x004E7C80` | original (juego) | 16547 |
| 2593 | `0x0081DAE0` | RenderWare/D3D | `0x0081961E` | original (RenderWare/D3D) | 16547 |
| 2594 | `0x008194B0` | RenderWare/D3D | `0x00819692` | original (RenderWare/D3D) | 16563 |
| 2595 | `0x00818650` | RenderWare/D3D | `0x00819692` | original (RenderWare/D3D) | 16563 |
| 2596 | `0x00819820` | RenderWare/D3D | `0x004E7C8D` | original (juego) | 17735 |
| 2597 | `0x004EEEC0` | juego | `0x004D9BC4` | original (juego) | 17766 |
| 2598 | `0x004EEA10` | juego | `0x004D9BCE` | original (juego) | 17766 |
| 2599 | `0x004F1680` | juego | `0x004D9BE2` | original (juego) | 17766 |
| 2600 | `0x004F1A60` | juego | `0x004F16B7` | original (juego) | 17766 |
| 2601 | `0x004F19E0` | juego | `0x004F16C1` | original (juego) | 17766 |
| 2602 | `0x004F1A40` | juego | `0x004F16CB` | original (juego) | 17766 |
| 2603 | `0x004F1880` | juego | `0x004F16D5` | original (juego) | 17766 |
| 2604 | `0x004F11F0` | juego | `0x004D9BE9` | original (juego) | 17766 |
| 2605 | `0x005B9390` | juego | `0x005B9CD2` | original (juego) | 17766 |
| 2606 | `0x00558F90` | juego | `0x005B93B4` | original (juego) | 17766 |
| 2607 | `0x004F3330` | juego | `0x005B961C` | original (juego) | 17766 |
| 2608 | `0x005B9690` | juego | `0x005B9D55` | original (juego) | 17766 |
| 2609 | `0x004D8810` | juego | `0x005B96B3` | original (juego) | 17766 |
| 2610 | `0x005B9340` | juego | `0x005B96D5` | original (juego) | 17766 |
| 2611 | `0x00821982` | CRT y bibliotecas | `0x005B97CE` | original (juego) | 17766 |
| 2612 | `0x00827DEC` | CRT y bibliotecas | `0x008219A7` | original (CRT) | 17766 |
| 2613 | `0x008334B2` | CRT y bibliotecas | `0x00827DF9` | original (CRT) | 17766 |
| 2614 | `0x00828B31` | CRT y bibliotecas | `0x00827EC4` | original (CRT) | 17766 |
| 2615 | `0x00827E74` | CRT y bibliotecas | `0x008219C3` | original (CRT) | 17766 |
| 2616 | `0x00824023` | CRT y bibliotecas | `0x008219E5` | original (CRT) | 17766 |
| 2617 | `0x005B9AB0` | juego | `0x005B9DBE` | original (juego) | 17766 |
| 2618 | `0x004D88A0` | juego | `0x005B9B01` | original (juego) | 17766 |
| 2619 | `0x004E0670` | juego | `0x005B9B01` | original (juego) | 17766 |
| 2620 | `0x004E0220` | juego | `0x004E06B2` | original (juego) | 17766 |
| 2621 | `0x004E01B0` | juego | `0x004E06E2` | original (juego) | 17766 |
| 2622 | `0x004D8ED0` | juego | `0x005B9B3F` | original (juego) | 17766 |
| 2623 | `0x004E07A0` | juego | `0x005B9B3F` | original (juego) | 17766 |
| 2624 | `0x004E03B0` | juego | `0x004E07F4` | original (juego) | 17766 |
| 2625 | `0x00506E50` | juego | `0x005B9DC6` | original (juego) | 17766 |
| 2626 | `0x004D9560` | juego | `0x005B9DC6` | original (juego) | 17766 |
| 2627 | `0x005B97F0` | juego | `0x005B9DCB` | original (juego) | 17766 |
| 2628 | `0x004D9E80` | juego | `0x005B988B` | original (juego) | 17766 |
| 2629 | `0x005B98A0` | juego | `0x005B9DD0` | original (juego) | 17766 |
| 2630 | `0x005B98C0` | juego | `0x005B9DD5` | original (juego) | 17766 |
| 2631 | `0x004E37B0` | juego | `0x005B992B` | original (juego) | 17766 |
| 2632 | `0x005B99F0` | juego | `0x005B9DDA` | original (juego) | 17766 |
| 2633 | `0x005B9A60` | juego | `0x005B9DDF` | original (juego) | 17766 |
| 2634 | `0x005B9A70` | juego | `0x005B9DE4` | original (juego) | 17766 |
| 2635 | `0x005B9A80` | juego | `0x005B9DE9` | original (juego) | 17766 |
| 2636 | `0x005B9A90` | juego | `0x005B9DEE` | original (juego) | 17766 |
| 2637 | `0x005B9C30` | juego | `0x005B9DF3` | original (juego) | 17766 |
| 2638 | `0x005B9B60` | juego | `0x005B9DFE` | original (juego) | 17766 |
| 2639 | `0x004E4E10` | juego | `0x005B9E09` | original (juego) | 17766 |
| 2640 | `0x005B9BD0` | juego | `0x005B9E14` | original (juego) | 17766 |
| 2641 | `0x00590320` | juego | `0x005B9E22` | original (juego) | 17766 |
| 2642 | `0x0059E650` | juego | `0x005BFB0F` | original (juego) | 17766 |
| 2643 | `0x006173F0` | juego | `0x005BFB1D` | original (juego) | 17766 |
| 2644 | `0x004A8DF0` | juego | `0x00617408` | original (juego) | 17766 |
| 2645 | `0x006180A0` | juego | `0x005BFB2B` | original (juego) | 17766 |
| 2646 | `0x004EEE90` | juego | `0x75EB5D49` | otro | 17766 |
| 2647 | `0x004EED10` | juego | `0x004EEEAD` | original (juego) | 17782 |
| 2648 | `0x004F15C0` | juego | `0x75EB5D49` | otro | 17782 |
| 2649 | `0x004F1290` | juego | `0x004F1619` | original (juego) | 17782 |
| 2650 | `0x004F18A0` | juego | `0x004F12B6` | original (juego) | 17782 |
| 2651 | `0x004F2550` | juego | `0x004F1621` | original (juego) | 17782 |
| 2652 | `0x004D7BD0` | juego | `0x004F255E` | original (juego) | 17782 |
| 2653 | `0x0053E770` | juego | `0x0053ECD0` | original (juego) | 21875 |
| 2654 | `0x006FF420` | juego | `0x0053E775` | original (juego) | 21875 |
| 2655 | `0x00561B10` | juego | `0x0053E77C` | original (juego) | 21875 |
| 2656 | `0x005618D0` | juego | `0x00561C5D` | original (juego) | 21875 |
| 2657 | `0x00727260` | juego | `0x0053E781` | original (juego) | 21875 |
| 2658 | `0x00719800` | juego | `0x0053E78B` | original (juego) | 21875 |
| 2659 | `0x0070CFB0` | juego | `0x0053E78B` | original (juego) | 21875 |
| 2660 | `0x0057B440` | juego | `0x0053E7AA` | original (juego) | 21875 |
| 2661 | `0x00573CF0` | juego | `0x0057B455` | original (juego) | 21875 |
| 2662 | `0x0057FD70` | juego | `0x0057B45C` | original (juego) | 21875 |
| 2663 | `0x0071A0E0` | juego | `0x0057FE3A` | original (juego) | 21875 |
| 2664 | `0x0069DB50` | juego | `0x0071A104` | original (juego) | 21875 |
| 2665 | `0x0069E160` | juego | `0x0071A142` | original (juego) | 21875 |
| 2666 | `0x00719750` | juego | `0x0071A1CC` | original (juego) | 21875 |
| 2667 | `0x00573410` | juego | `0x0057FE70` | original (juego) | 21875 |
| 2668 | `0x00573E70` | juego | `0x0058007D` | original (juego) | 21875 |
| 2669 | `0x005773D0` | juego | `0x00580092` | original (juego) | 21875 |
| 2670 | `0x005738B0` | juego | `0x00580099` | original (juego) | 21875 |
| 2671 | `0x00540980` | juego | `0x005738EE` | original (juego) | 21875 |
| 2672 | `0x00573840` | juego | `0x005800AB` | original (juego) | 21875 |
| 2673 | `0x00540950` | juego | `0x0057387E` | original (juego) | 21875 |
| 2674 | `0x004410E0` | juego | `0x005802A7` | original (juego) | 21875 |
| 2675 | `0x005409B0` | juego | `0x005802BC` | original (juego) | 21875 |
| 2676 | `0x005409E0` | juego | `0x005802D1` | original (juego) | 21875 |
| 2677 | `0x0057C400` | juego | `0x005802E6` | original (juego) | 21875 |
| 2678 | `0x0057C420` | juego | `0x005802FB` | original (juego) | 21875 |
| 2679 | `0x0053FC90` | juego | `0x005803FF` | original (juego) | 21875 |
| 2680 | `0x00573990` | juego | `0x0058056F` | original (juego) | 21875 |
| 2681 | `0x00573920` | juego | `0x00580588` | original (juego) | 21875 |
| 2682 | `0x0057B480` | juego | `0x00580620` | original (juego) | 21875 |
| 2683 | `0x0057E4D0` | juego | `0x0057B495` | original (juego) | 21875 |
| 2684 | `0x00578D60` | juego | `0x0057B463` | original (juego) | 21891 |
| 2685 | `0x00730740` | juego | `0x0057B46A` | original (juego) | 21891 |
| 2686 | `0x007305E0` | juego | `0x0057B471` | original (juego) | 21891 |
| 2687 | `0x00576B70` | juego | `0x0053E7AA` | original (juego) | 21891 |
| 2688 | `0x00506FF0` | juego | `0x00576BDF` | original (juego) | 21891 |
| 2689 | `0x004DD480` | juego | `0x00576BDF` | original (juego) | 21891 |
| 2690 | `0x00506EA0` | juego | `0x00576C1D` | original (juego) | 21891 |
| 2691 | `0x004DD4A0` | juego | `0x00576C1D` | original (juego) | 21891 |
| 2692 | `0x004D88C0` | juego | `0x004DD5E8` | original (juego) | 21891 |
| 2693 | `0x004EF660` | juego | `0x004DEC85` | original (juego) | 21891 |
| 2694 | `0x00507750` | juego | `0x00576C27` | original (juego) | 21891 |
| 2695 | `0x004D8F60` | juego | `0x00507796` | original (juego) | 21891 |
| 2696 | `0x004F1530` | juego | `0x00507796` | original (juego) | 21891 |
| 2697 | `0x004D8820` | juego | `0x005077AB` | original (juego) | 21891 |
| 2698 | `0x004DBFB0` | juego | `0x005077F4` | original (juego) | 21891 |
| 2699 | `0x004D8F70` | juego | `0x004DBFCC` | original (juego) | 21891 |
| 2700 | `0x004F1550` | juego | `0x004DBFCC` | original (juego) | 21891 |
| 2701 | `0x004D8F90` | juego | `0x004DBFD9` | original (juego) | 21891 |
| 2702 | `0x004F1570` | juego | `0x004DBFD9` | original (juego) | 21891 |
| 2703 | `0x00507150` | juego | `0x004D76CE` | original (juego) | 21891 |
| 2704 | `0x004DBEB0` | juego | `0x004D76CE` | original (juego) | 21891 |
| 2705 | `0x004EB9A0` | juego | `0x0050780A` | original (juego) | 21891 |
| 2706 | `0x004E8410` | juego | `0x004EB9F2` | original (juego) | 21891 |
| 2707 | `0x00558E40` | juego | `0x004E8477` | original (juego) | 21891 |
| 2708 | `0x004EB660` | juego | `0x004EB9F9` | original (juego) | 21891 |
| 2709 | `0x0056E210` | juego | `0x00507811` | original (juego) | 21891 |
| 2710 | `0x004DA2C0` | juego | `0x0050782E` | original (juego) | 21891 |
| 2711 | `0x004D95B0` | juego | `0x0050787F` | original (juego) | 21891 |
| 2712 | `0x004EC900` | juego | `0x0050788A` | original (juego) | 21891 |
| 2713 | `0x004E3710` | juego | `0x0050788F` | original (juego) | 21891 |
| 2714 | `0x004F0000` | juego | `0x00507899` | original (juego) | 21891 |
| 2715 | `0x004F0F70` | juego | `0x004D8857` | original (juego) | 21891 |
| 2716 | `0x004F0F40` | juego | `0x004F0F78` | original (juego) | 21891 |
| 2717 | `0x004D8E90` | juego | `0x004F010B` | original (juego) | 21891 |
| 2718 | `0x004D8EB0` | juego | `0x004F011C` | original (juego) | 21891 |
| 2719 | `0x004D9870` | juego | `0x004F0ABD` | original (juego) | 21891 |
| 2720 | `0x004D8990` | juego | `0x004D988D` | original (juego) | 21891 |
| 2721 | `0x004F2060` | juego | `0x004D8D7B` | original (juego) | 21891 |
| 2722 | `0x004D7990` | juego | `0x004F2099` | original (juego) | 21891 |
| 2723 | `0x004D7D00` | juego | `0x004D8D7B` | original (juego) | 21891 |
| 2724 | `0x004D8DA0` | juego | `0x004D989E` | original (juego) | 21891 |
| 2725 | `0x004D8010` | juego | `0x004D8DB5` | original (juego) | 21891 |
| 2726 | `0x004D7AA0` | juego | `0x004D8DF7` | original (juego) | 21891 |
| 2727 | `0x004F1040` | juego | `0x004D98B9` | original (juego) | 21891 |
| 2728 | `0x004F10D0` | juego | `0x004D98E7` | original (juego) | 21891 |
| 2729 | `0x004E0450` | juego | `0x004D9916` | original (juego) | 21891 |
| 2730 | `0x004DFE30` | juego | `0x004F0ABD` | original (juego) | 21891 |
| 2731 | `0x004063E0` | juego | `0x004E011C` | original (juego) | 21891 |
| 2732 | `0x004E7630` | juego | `0x00576C27` | original (juego) | 21891 |
| 2733 | `0x0053D690` | juego | `0x00576E23` | original (juego) | 21891 |
| 2734 | `0x007328C0` | juego | `0x0053D719` | original (juego) | 21891 |
| 2735 | `0x007EE340` | RenderWare/D3D | `0x0053D72C` | original (juego) | 21891 |
| 2736 | `0x007F7730` | RenderWare/D3D | `0x007EE35D` | original (RenderWare/D3D) | 21891 |
| 2737 | `0x00619450` | juego | `0x0053D738` | original (juego) | 21891 |
| 2738 | `0x00745210` | juego | `0x0053D738` | original (juego) | 21891 |
| 2739 | `0x00727C10` | juego | `0x0053D792` | original (juego) | 21891 |
| 2740 | `0x0053D840` | juego | `0x00576E28` | original (juego) | 21891 |
| 2741 | `0x00532260` | juego | `0x0053D845` | original (juego) | 21891 |
| 2742 | `0x00734640` | juego | `0x0053D84A` | original (juego) | 21891 |
| 2743 | `0x0053F1E0` | juego | `0x00576E9C` | original (juego) | 21907 |
| 2744 | `0x00541BD0` | juego | `0x00576EB3` | original (juego) | 21907 |
| 2745 | `0x00745CA0` | juego | `0x00576EB8` | original (juego) | 21907 |
| 2746 | `0x00746F70` | juego | `0x00576EC1` | original (juego) | 21907 |
| 2747 | `0x005744D0` | juego | `0x00576ED1` | original (juego) | 21907 |
| 2748 | `0x007F8A90` | RenderWare/D3D | `0x00574502` | original (juego) | 21907 |
| 2749 | `0x00745C70` | juego | `0x0057450E` | original (juego) | 21922 |
| 2750 | `0x007F8640` | RenderWare/D3D | `0x00745C7A` | original (juego) | 21922 |
| 2751 | `0x0057C520` | juego | `0x0057451E` | original (juego) | 21922 |
| 2752 | `0x00507040` | juego | `0x00574604` | original (juego) | 21922 |
| 2753 | `0x004E83F0` | juego | `0x00574604` | original (juego) | 21922 |
| 2754 | `0x00619140` | juego | `0x00574622` | original (juego) | 21922 |
| 2755 | `0x0082B193` | CRT y bibliotecas | `0x008341FF` | original (CRT) | 21922 |
| 2756 | `0x005D1170` | juego | `0x00619261` | original (juego) | 21922 |
| 2757 | `0x005D0D20` | juego | `0x005D1190` | original (juego) | 21922 |
| 2758 | `0x005D10B0` | juego | `0x005D11A1` | original (juego) | 21922 |
| 2759 | `0x00538A50` | juego | `0x005D1108` | original (juego) | 21922 |
| 2760 | `0x0069F8C0` | juego | `0x00619330` | original (juego) | 21922 |
| 2761 | `0x00561AF0` | juego | `0x00576ED1` | original (juego) | 21922 |
| 2762 | `0x00572EC0` | juego | `0x00576ED8` | original (juego) | 21922 |
| 2763 | `0x00561AA0` | juego | `0x00572ED8` | original (juego) | 21922 |
| 2764 | `0x0040E120` | juego | `0x00572EE2` | original (juego) | 21922 |
| 2765 | `0x00407BE0` | juego | `0x00572EE7` | original (juego) | 21922 |
| 2766 | `0x0053C500` | juego | `0x00572EF0` | original (juego) | 21922 |
| 2767 | `0x00748F50` | juego | `0x00730F89` | original (juego) | 21922 |
| 2768 | `0x0049EA50` | juego | `0x00730F9A` | original (juego) | 21922 |
| 2769 | `0x00748F30` | juego | `0x00730FAF` | original (juego) | 21922 |
| 2770 | `0x00727320` | juego | `0x00572F57` | original (juego) | 21922 |
| 2771 | `0x00407BF0` | juego | `0x00572F6F` | original (juego) | 21922 |
| 2772 | `0x004EB890` | juego | `0x004EBA02` | original (juego) | 21938 |
| 2773 | `0x004D9550` | juego | `0x00507861` | original (juego) | 21938 |
| 2774 | `0x004D9E50` | juego | `0x004D8D51` | original (juego) | 21938 |
| 2775 | `0x00734650` | juego | `0x0053E81F` | original (juego) | 21938 |
| 2776 | `0x00573A60` | juego | `0x0057C2C1` | original (juego) | 21938 |
| 2777 | `0x0057B750` | juego | `0x0053E832` | original (juego) | 21938 |
| 2778 | `0x005733E0` | juego | `0x0057B926` | original (juego) | 21938 |
| 2779 | `0x005794A0` | juego | `0x0057BA5D` | original (juego) | 21938 |
| 2780 | `0x00719590` | juego | `0x005795EF` | original (juego) | 21938 |
| 2781 | `0x0058FEA0` | juego | `0x00579603` | original (juego) | 21938 |
| 2782 | `0x00719510` | juego | `0x00579627` | original (juego) | 21938 |
| 2783 | `0x0071A700` | juego | `0x0057969D` | original (juego) | 21938 |
| 2784 | `0x0071A220` | juego | `0x0071A807` | original (juego) | 21938 |
| 2785 | `0x00719B40` | juego | `0x0071A56A` | original (juego) | 21938 |
| 2786 | `0x00719840` | juego | `0x00719B62` | original (juego) | 21938 |
| 2787 | `0x00727B30` | juego | `0x00719867` | original (juego) | 21938 |
| 2788 | `0x00718E50` | juego | `0x00719885` | original (juego) | 21938 |
| 2789 | `0x00718A10` | juego | `0x00719A35` | original (juego) | 21938 |
| 2790 | `0x00718770` | juego | `0x00718B16` | original (juego) | 21938 |
| 2791 | `0x00728200` | juego | `0x00718D83` | original (juego) | 21938 |
| 2792 | `0x00727A00` | juego | `0x0072824C` | original (juego) | 21938 |
| 2793 | `0x0070CF20` | juego | `0x00719B2B` | original (juego) | 21938 |
| 2794 | `0x007273D0` | juego | `0x00719B30` | original (juego) | 21938 |
| 2795 | `0x00734EA0` | juego | `0x00727404` | original (juego) | 21938 |
| 2796 | `0x007FBD30` | RenderWare/D3D | `0x00727404` | original (juego) | 21938 |
| 2797 | `0x007FA1C0` | RenderWare/D3D | `0x007FC181` | original (RenderWare/D3D) | 21938 |
| 2798 | `0x007192C0` | juego | `0x00719775` | original (juego) | 21938 |
| 2799 | `0x0071A5E0` | juego | `0x0057B24B` | original (juego) | 21938 |
| 2800 | `0x00744AE0` | juego | `0x0053E837` | original (juego) | 21938 |
| 2801 | `0x0053E600` | juego | `0x0053E83C` | original (juego) | 21938 |
| 2802 | `0x0058D490` | juego | `0x0053E841` | original (juego) | 21938 |
| 2803 | `0x0069EFC0` | juego | `0x0053E848` | original (juego) | 21938 |
| 2804 | `0x0069DE90` | juego | `0x0069F07C` | original (juego) | 21938 |
| 2805 | `0x00588F60` | juego | `0x0069F0A1` | original (juego) | 21938 |
| 2806 | `0x0071A210` | juego | `0x0053E850` | original (juego) | 21938 |
| 2807 | `0x004EFE50` | juego | `0x004DE236` | original (juego) | 21953 |
| 2808 | `0x004EF880` | juego | `0x004EFEFE` | original (juego) | 21953 |
| 2809 | `0x004EFB10` | juego | `0x004DE25A` | original (juego) | 21953 |
| 2810 | `0x004EF680` | juego | `0x004EFB4B` | original (juego) | 21953 |
| 2811 | `0x004EF7A0` | juego | `0x004EFB68` | original (juego) | 21953 |
| 2812 | `0x004D8E30` | juego | `0x004EF7CE` | original (juego) | 21953 |
| 2813 | `0x004E01E0` | juego | `0x004EF7CE` | original (juego) | 21953 |
| 2814 | `0x004D8E60` | juego | `0x004EFB80` | original (juego) | 21953 |
| 2815 | `0x004EFF50` | juego | `0x004F0404` | original (juego) | 21953 |
| 2816 | `0x004DEDA0` | juego | `0x004EFFAB` | original (juego) | 21953 |
| 2817 | `0x004EFA10` | juego | `0x004F041F` | original (juego) | 21953 |
| 2818 | `0x004D86B0` | juego | `0x004F07FD` | original (juego) | 21953 |
| 2819 | `0x004E0280` | juego | `0x004D86FD` | original (juego) | 21953 |
| 2820 | `0x004E0380` | juego | `0x004D8716` | original (juego) | 21953 |
| 2821 | `0x004F0C40` | juego | `0x004D874E` | original (juego) | 21953 |
| 2822 | `0x004F0FA0` | juego | `0x004D8768` | original (juego) | 21953 |
| 2823 | `0x004D79D0` | juego | `0x004F0FA9` | original (juego) | 21953 |
| 2824 | `0x004D9EF0` | juego | `0x004D79E5` | original (juego) | 21953 |
| 2825 | `0x008219F0` | CRT y bibliotecas | `0x004D9F31` | original (juego) | 21953 |
| 2826 | `0x004D7B50` | juego | `0x004D87DC` | original (juego) | 21953 |
| 2827 | `0x004F0BD0` | juego | `0x004D87EA` | original (juego) | 21953 |
| 2828 | `0x004D8870` | juego | `0x004F0818` | original (juego) | 21953 |
| 2829 | `0x004D8920` | juego | `0x004F085F` | original (juego) | 21953 |
| 2830 | `0x004D7950` | juego | `0x004D8956` | original (juego) | 21953 |
| 2831 | `0x004D8960` | juego | `0x004F0876` | original (juego) | 21953 |
| 2832 | `0x004D79A0` | juego | `0x004F0F88` | original (juego) | 21953 |
