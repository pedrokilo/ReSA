# Funciones originales ejecutadas hasta el menu (con hooks)

148 funciones originales distintas (primera entrada). Por region: CRT y bibliotecas (0x820000+): 78, RenderWare/D3D (0x750000-0x81FFFF): 53, juego (0x401000-0x74FFFF): 17

| n | funcion original | region | llamada desde | quien llama | ms |
|---:|---|---|---|---|---:|
| 1 | `0x0082119A` | CRT y bibliotecas | `0x01355564` | propio: `??$CallAndReturn@PAX$0ICBBJK@I@plugin@@YAPAXI@Z` | 688 |
| 2 | `0x0082422B` | CRT y bibliotecas | `0x019658A4` | propio: `?CrtMalloc@@YAPAXI@Z` | 688 |
| 3 | `0x008241B0` | CRT y bibliotecas | `0x0082423B` | original (CRT) | 688 |
| 4 | `0x008286E0` | CRT y bibliotecas | `0x008241BC` | original (CRT) | 688 |
| 5 | `0x0082871B` | CRT y bibliotecas | `0x0082421E` | original (CRT) | 688 |
| 6 | `0x008214BD` | CRT y bibliotecas | `0x01355610` | propio: `??3@YAXPAX@Z` | 688 |
| 7 | `0x0082413F` | CRT y bibliotecas | `0x01355610` | propio: `??3@YAXPAX@Z` | 688 |
| 8 | `0x008213AE` | CRT y bibliotecas | `0x013554C4` | propio: `??$Call@$0ICBDKO@PAX@plugin@@YAXPAX@Z` | 891 |
| 9 | `0x00821195` | CRT y bibliotecas | `0x00DDA8E4` | propio: `??$CallAndReturn@PAX$0ICBBJF@I@plugin@@YAPAXI@Z` | 985 |
| 10 | `0x00801C30` | RenderWare/D3D | `0x0162259F` | propio: `__rwPluginRegistryAddPlugin` | 1344 |
| 11 | `0x007F5F70` | RenderWare/D3D | `0x0161CD2F` | propio: `?_rwDeviceSystemRequest@@YAHPAURwDevice@@HPAX1H@Z` | 1438 |
| 12 | `0x007F62CB` | RenderWare/D3D | `0x001AF680` | otro | 1532 |
| 13 | `0x00807C2B` | RenderWare/D3D | `0x007F6310` | original (RenderWare/D3D) | 1625 |
| 14 | `0x00807C10` | RenderWare/D3D | `0x007F6310` | original (RenderWare/D3D) | 1735 |
| 15 | `0x00820DBD` | CRT y bibliotecas | `0x00807C27` | original (RenderWare/D3D) | 1828 |
| 16 | `0x00820FFA` | CRT y bibliotecas | `0x001AF680` | otro | 1922 |
| 17 | `0x00826590` | CRT y bibliotecas | `0x007F633E` | original (RenderWare/D3D) | 2078 |
| 18 | `0x00824269` | CRT y bibliotecas | `0x01356A08` | propio: `?Realloc@CMemoryMgr@@SAPAEPAXII@Z` | 2188 |
| 19 | `0x00824416` | CRT y bibliotecas | `0x01355788` | propio: `?Calloc@CMemoryMgr@@SAPAEIII@Z` | 2297 |
| 20 | `0x00821BB5` | CRT y bibliotecas | `0x0196BBD8` | propio: `?GetVideoModeList@@YAPAPADXZ` | 2391 |
| 21 | `0x00827F4B` | CRT y bibliotecas | `0x00821BE4` | original (CRT) | 2485 |
| 22 | `0x00833570` | CRT y bibliotecas | `0x0082855D` | original (CRT) | 2610 |
| 23 | `0x00827ED1` | CRT y bibliotecas | `0x008285F2` | original (CRT) | 2719 |
| 24 | `0x00827EF5` | CRT y bibliotecas | `0x00828606` | original (CRT) | 2813 |
| 25 | `0x00827E9E` | CRT y bibliotecas | `0x00827F1C` | original (CRT) | 2922 |
| 26 | `0x0082AAD4` | CRT y bibliotecas | `0x008286BE` | original (CRT) | 3032 |
| 27 | `0x007F6786` | RenderWare/D3D | `0x001AF698` | otro | 3250 |
| 28 | `0x008087D0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 3360 |
| 29 | `0x007EDE90` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 3469 |
| 30 | `0x007EE004` | RenderWare/D3D | `0x40800000` | otro | 3563 |
| 31 | `0x0080AA40` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 3672 |
| 32 | `0x007F16C0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 3782 |
| 33 | `0x007EFEF0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 3891 |
| 34 | `0x007EC780` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4000 |
| 35 | `0x007EE110` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4110 |
| 36 | `0x00802280` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4203 |
| 37 | `0x008220F0` | CRT y bibliotecas | `0x01A08AFF` | propio: `?ImageGammaTablesCpp@@YAXMPAE0P6ANNN@Z@Z` | 4313 |
| 38 | `0x00829B99` | CRT y bibliotecas | `0x01A08AFF` | propio: `?ImageGammaTablesCpp@@YAXMPAE0P6ANNN@Z@Z` | 4422 |
| 39 | `0x007FB370` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4532 |
| 40 | `0x007F3EA0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4641 |
| 41 | `0x007F4000` | RenderWare/D3D | `0x001AF5BC` | otro | 4750 |
| 42 | `0x00807C40` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4844 |
| 43 | `0x0080A780` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 4953 |
| 44 | `0x007EFE20` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5063 |
| 45 | `0x00807C90` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5172 |
| 46 | `0x0074D8E0` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5282 |
| 47 | `0x00758970` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5391 |
| 48 | `0x0074BFA0` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5485 |
| 49 | `0x00749760` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5594 |
| 50 | `0x00752270` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5703 |
| 51 | `0x00761C30` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5813 |
| 52 | `0x0074FEE0` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 5922 |
| 53 | `0x00757890` | RenderWare/D3D | `0x01608CD7` | propio: `_RxLockedPipeUnlock` | 6032 |
| 54 | `0x0075E540` | RenderWare/D3D | `0x01608CD7` | propio: `_RxLockedPipeUnlock` | 6141 |
| 55 | `0x007637E0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6250 |
| 56 | `0x00750280` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6344 |
| 57 | `0x00758890` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6453 |
| 58 | `0x007C68E0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6563 |
| 59 | `0x007CB240` | RenderWare/D3D | `0x01608CD7` | propio: `_RxLockedPipeUnlock` | 6672 |
| 60 | `0x007CCCD0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6782 |
| 61 | `0x007C4670` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 6891 |
| 62 | `0x00810BF0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7000 |
| 63 | `0x008263C0` | CRT y bibliotecas | `0x016092F9` | propio: `_RxPipelineFindNodeByName` | 7110 |
| 64 | `0x007CB9B0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7219 |
| 65 | `0x00749000` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7328 |
| 66 | `0x007503E0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7438 |
| 67 | `0x00749880` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7547 |
| 68 | `0x007C4750` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7641 |
| 69 | `0x004CCE60` | juego | `0x016060C1` | propio: `_RwRasterCreate` | 7750 |
| 70 | `0x004CC5C0` | juego | `0x004CCEAB` | original (juego) | 7860 |
| 71 | `0x004C9A60` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 7953 |
| 72 | `0x004CCD70` | juego | `0x004CCF31` | original (juego) | 8063 |
| 73 | `0x008086B0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8172 |
| 74 | `0x00758850` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8266 |
| 75 | `0x00811B00` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8391 |
| 76 | `0x00750AA0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8500 |
| 77 | `0x007498B0` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8625 |
| 78 | `0x00755BC0` | RenderWare/D3D | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 8766 |
| 79 | `0x008232D8` | CRT y bibliotecas | `0x01610D0B` | propio: `?R12AStreamSetup@?A0x99da385b@@YAPAURwStream@@PAU2@W4RwStreamType@@W4RwStreamAccessType@@PBX@Z` | 8875 |
| 80 | `0x0082327C` | CRT y bibliotecas | `0x008232E7` | original (CRT) | 8969 |
| 81 | `0x0082B206` | CRT y bibliotecas | `0x0082328D` | original (CRT) | 9094 |
| 82 | `0x0082ADBE` | CRT y bibliotecas | `0x0082B220` | original (CRT) | 9203 |
| 83 | `0x0082AD3F` | CRT y bibliotecas | `0x0082B25A` | original (CRT) | 9313 |
| 84 | `0x00833BA6` | CRT y bibliotecas | `0x0082AD8D` | original (CRT) | 9422 |
| 85 | `0x0082AD12` | CRT y bibliotecas | `0x0082ADB6` | original (CRT) | 9532 |
| 86 | `0x0082B31F` | CRT y bibliotecas | `0x0082B314` | original (CRT) | 9641 |
| 87 | `0x0082B019` | CRT y bibliotecas | `0x008232B6` | original (CRT) | 9735 |
| 88 | `0x00834334` | CRT y bibliotecas | `0x0082B155` | original (CRT) | 9844 |
| 89 | `0x00833FF8` | CRT y bibliotecas | `0x00834361` | original (CRT) | 9953 |
| 90 | `0x00833E02` | CRT y bibliotecas | `0x00834191` | original (CRT) | 10063 |
| 91 | `0x00833C31` | CRT y bibliotecas | `0x0083421D` | original (CRT) | 10188 |
| 92 | `0x00834379` | CRT y bibliotecas | `0x00834370` | original (CRT) | 10313 |
| 93 | `0x00833DE0` | CRT y bibliotecas | `0x00834387` | original (CRT) | 10422 |
| 94 | `0x008232CE` | CRT y bibliotecas | `0x008232C5` | original (CRT) | 10516 |
| 95 | `0x00823FF4` | CRT y bibliotecas | `0x008232D6` | original (CRT) | 10625 |
| 96 | `0x00823521` | CRT y bibliotecas | `0x0162146A` | propio: `_RwStreamRead` | 10735 |
| 97 | `0x00823FA2` | CRT y bibliotecas | `0x00823535` | original (CRT) | 10860 |
| 98 | `0x00823438` | CRT y bibliotecas | `0x0082354B` | original (CRT) | 10953 |
| 99 | `0x0082AF38` | CRT y bibliotecas | `0x008234E7` | original (CRT) | 11078 |
| 100 | `0x0083361B` | CRT y bibliotecas | `0x0082AF71` | original (CRT) | 11172 |
| 101 | `0x0082B617` | CRT y bibliotecas | `0x0082AF87` | original (CRT) | 11282 |
| 102 | `0x00833D6D` | CRT y bibliotecas | `0x0082B654` | original (CRT) | 11391 |
| 103 | `0x0082B44A` | CRT y bibliotecas | `0x0082B66E` | original (CRT) | 11500 |
| 104 | `0x0082B69E` | CRT y bibliotecas | `0x0082B696` | original (CRT) | 11610 |
| 105 | `0x008266F0` | CRT y bibliotecas | `0x0082349C` | original (CRT) | 11719 |
| 106 | `0x00823563` | CRT y bibliotecas | `0x0082355A` | original (CRT) | 11828 |
| 107 | `0x004CD820` | juego | `0x016252D3` | propio: `?RwTextureGtaStreamRead@@YAPAURwTexture@@PAURwStream@@@Z` | 11922 |
| 108 | `0x00749020` | juego | `0x0162297E` | propio: `__rwPluginRegistryInitObject` | 12032 |
| 109 | `0x00821F40` | CRT y bibliotecas | `0x015D2332` | propio: `?CopyStringChecked@?A0x3ca24ac8@@YAXPADPBDI@Z` | 12125 |
| 110 | `0x00826330` | CRT y bibliotecas | `0x015D235C` | propio: `?CopyStringChecked@?A0x3ca24ac8@@YAXPADPBDI@Z` | 12219 |
| 111 | `0x0082318B` | CRT y bibliotecas | `0x016210E5` | propio: `_RwStreamClose` | 12313 |
| 112 | `0x0082313F` | CRT y bibliotecas | `0x008231C2` | original (CRT) | 12422 |
| 113 | `0x00823D26` | CRT y bibliotecas | `0x00823154` | original (CRT) | 12532 |
| 114 | `0x0082AF0D` | CRT y bibliotecas | `0x0082315C` | original (CRT) | 12641 |
| 115 | `0x0082AE72` | CRT y bibliotecas | `0x00823164` | original (CRT) | 12735 |
| 116 | `0x0082ADEF` | CRT y bibliotecas | `0x0082AEC3` | original (CRT) | 12844 |
| 117 | `0x00833D2C` | CRT y bibliotecas | `0x0082ADFB` | original (CRT) | 12938 |
| 118 | `0x00833CAD` | CRT y bibliotecas | `0x0082AE45` | original (CRT) | 13032 |
| 119 | `0x0082AEE9` | CRT y bibliotecas | `0x0082AEE1` | original (CRT) | 13141 |
| 120 | `0x008231D4` | CRT y bibliotecas | `0x008231CF` | original (CRT) | 13250 |
| 121 | `0x004CB9C0` | juego | `0x004CCF0F` | original (juego) | 13360 |
| 122 | `0x004CB7C0` | juego | `0x004CCEEC` | original (juego) | 13469 |
| 123 | `0x007EF370` | RenderWare/D3D | `0x015FEC73` | propio: `_RwCameraBeginUpdate` | 13594 |
| 124 | `0x007521A0` | RenderWare/D3D | `0x00750ADB` | original (RenderWare/D3D) | 13688 |
| 125 | `0x007EE5A0` | RenderWare/D3D | `0x0075049B` | original (RenderWare/D3D) | 13797 |
| 126 | `0x007F7530` | RenderWare/D3D | `0x007506BB` | original (RenderWare/D3D) | 13891 |
| 127 | `0x007EF340` | RenderWare/D3D | `0x015FEFE3` | propio: `_RwCameraEndUpdate` | 13985 |
| 128 | `0x007F98D0` | RenderWare/D3D | `0x007EF354` | original (RenderWare/D3D) | 14078 |
| 129 | `0x00749030` | juego | `0x0162286A` | propio: `__rwPluginRegistryDeInitObject` | 16266 |
| 130 | `0x004C9A80` | juego | `0x0162286A` | propio: `__rwPluginRegistryDeInitObject` | 16360 |
| 131 | `0x008220AD` | CRT y bibliotecas | `0x0110DAFA` | propio: `?ReadEventIndices@CDecisionMakerTypes@@SGXPAHPBD@Z` | 124375 |
| 132 | `0x008290D4` | CRT y bibliotecas | `0x008220DC` | original (CRT) | 124485 |
| 133 | `0x00822802` | CRT y bibliotecas | `0x00829118` | original (CRT) | 124578 |
| 134 | `0x00827B3D` | CRT y bibliotecas | `0x00822807` | original (CRT) | 124688 |
| 135 | `0x00825456` | CRT y bibliotecas | `0x00822817` | original (CRT) | 124797 |
| 136 | `0x0082524D` | CRT y bibliotecas | `0x00825473` | original (CRT) | 124907 |
| 137 | `0x00825488` | CRT y bibliotecas | `0x0082547F` | original (CRT) | 125016 |
| 138 | `0x00822789` | CRT y bibliotecas | `0x008291A8` | original (CRT) | 125125 |
| 139 | `0x00829094` | CRT y bibliotecas | `0x008292C8` | original (CRT) | 125219 |
| 140 | `0x0083375D` | CRT y bibliotecas | `0x0082969E` | original (CRT) | 125328 |
| 141 | `0x00818650` | RenderWare/D3D | `0x01513B14` | propio: `?QtmlInitializeThunk@@YAFH@Z` | 125828 |
| 142 | `0x00819820` | RenderWare/D3D | `0x00D8A9A0` | propio: `??$CallAndReturn@F$0IBJICA@$$V@plugin@@YAFXZ` | 127157 |
| 143 | `0x00827DEC` | CRT y bibliotecas | `0x0196594E` | propio: `?CrtPrintf@@YAHPBDZZ` | 127391 |
| 144 | `0x008334B2` | CRT y bibliotecas | `0x00827DF9` | original (CRT) | 127563 |
| 145 | `0x00828B31` | CRT y bibliotecas | `0x00827EC4` | original (CRT) | 127735 |
| 146 | `0x00827E74` | CRT y bibliotecas | `0x0196599B` | propio: `?CrtPrintf@@YAHPBDZZ` | 127907 |
| 147 | `0x007F2BA0` | RenderWare/D3D | `0x0161EF20` | propio: `_RwEngineGetVersion` | 129813 |
| 148 | `0x007FBD30` | RenderWare/D3D | `0x01670809` | propio: `?RenderVertexBuffer@CSprite2d@@SAXXZ` | 130000 |
