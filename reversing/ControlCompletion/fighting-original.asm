Microsoft (R) COFF/PE Dumper Version 14.51.36257.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file D:\Grand Theft Auto San Andreas\gta-sa.exe

File Type: EXECUTABLE IMAGE

  0062A0A0: 64 A1 00 00 00 00  mov         eax,dword ptr fs:[00000000h]
  0062A0A6: 6A FF              push        0FFFFFFFFh
  0062A0A8: 68 7B 00 84 00     push        84007Bh
  0062A0AD: 50                 push        eax
  0062A0AE: 64 89 25 00 00 00  mov         dword ptr fs:[00000000h],esp
            00
  0062A0B5: 83 EC 08           sub         esp,8
  0062A0B8: 53                 push        ebx
  0062A0B9: 56                 push        esi
  0062A0BA: 8B F1              mov         esi,ecx
  0062A0BC: 8B 46 0C           mov         eax,dword ptr [esi+0Ch]
  0062A0BF: 33 DB              xor         ebx,ebx
  0062A0C1: 3B C3              cmp         eax,ebx
  0062A0C3: 74 09              je          0062A0CE
  0062A0C5: 38 5E 08           cmp         byte ptr [esi+8],bl
  0062A0C8: 89 5C 24 08        mov         dword ptr [esp+8],ebx
  0062A0CC: 74 15              je          0062A0E3
  0062A0CE: 5E                 pop         esi
  0062A0CF: B0 01              mov         al,1
  0062A0D1: 5B                 pop         ebx
  0062A0D2: 8B 4C 24 08        mov         ecx,dword ptr [esp+8]
  0062A0D6: 64 89 0D 00 00 00  mov         dword ptr fs:[00000000h],ecx
            00
  0062A0DD: 83 C4 14           add         esp,14h
  0062A0E0: C2 04 00           ret         4
  0062A0E3: 55                 push        ebp
  0062A0E4: 57                 push        edi
  0062A0E5: 8B 7C 24 28        mov         edi,dword ptr [esp+28h]
  0062A0E9: 8A 87 19 07 00 00  mov         al,byte ptr [edi+00000719h]
  0062A0EF: 0F B6 C8           movzx       ecx,al
  0062A0F2: 89 4C 24 28        mov         dword ptr [esp+28h],ecx
  0062A0F6: 8A 8F 84 04 00 00  mov         cl,byte ptr [edi+00000484h]
  0062A0FC: 80 F9 02           cmp         cl,2
  0062A0FF: DB 44 24 28        fild        dword ptr [esp+28h]
  0062A103: D9 5C 24 28        fstp        dword ptr [esp+28h]
  0062A107: 74 23              je          0062A12C
  0062A109: 83 BF 98 05 00 00  cmp         dword ptr [edi+00000598h],6
            06
  0062A110: 74 1A              je          0062A12C
  0062A112: 3C 28              cmp         al,28h
  0062A114: 75 16              jne         0062A12C
  0062A116: 8B 97 9C 05 00 00  mov         edx,dword ptr [edi+0000059Ch]
  0062A11C: 0F B7 42 30        movzx       eax,word ptr [edx+30h]
  0062A120: 89 44 24 28        mov         dword ptr [esp+28h],eax
  0062A124: DB 44 24 28        fild        dword ptr [esp+28h]
  0062A128: D9 5C 24 28        fstp        dword ptr [esp+28h]
  0062A12C: 8B CF              mov         ecx,edi
  0062A12E: E8 7D E9 FB FF     call        005E8AB0
  0062A133: 8B 87 7C 04 00 00  mov         eax,dword ptr [edi+0000047Ch]
  0062A139: 53                 push        ebx
  0062A13A: 8D 48 04           lea         ecx,[eax+4]
  0062A13D: E8 CE 76 05 00     call        00681810
  0062A142: 85 C0              test        eax,eax
  0062A144: 75 54              jne         0062A19A
  0062A146: 6A 28              push        28h
  0062A148: E8 53 04 FF FF     call        0061A5A0
  0062A14D: 8B E8              mov         ebp,eax
  0062A14F: 83 C4 04           add         esp,4
  0062A152: 89 6C 24 14        mov         dword ptr [esp+14h],ebp
  0062A156: 3B EB              cmp         ebp,ebx
  0062A158: 89 5C 24 20        mov         dword ptr [esp+20h],ebx
  0062A15C: 74 1A              je          0062A178
  0062A15E: D9 05 50 2E 8D 00  fld         dword ptr ds:[008D2E50h]
  0062A164: E8 D7 79 1F 00     call        00821B40
  0062A169: 8B 4E 0C           mov         ecx,dword ptr [esi+0Ch]
  0062A16C: 50                 push        eax
  0062A16D: 53                 push        ebx
  0062A16E: 51                 push        ecx
  0062A16F: 8B CD              mov         ecx,ebp
  0062A171: E8 FA 22 FF FF     call        0061C470
  0062A176: EB 02              jmp         0062A17A
  0062A178: 33 C0              xor         eax,eax
  0062A17A: 8B 8F 7C 04 00 00  mov         ecx,dword ptr [edi+0000047Ch]
  0062A180: 53                 push        ebx
  0062A181: 50                 push        eax
  0062A182: 83 C1 04           add         ecx,4
  0062A185: C7 44 24 28 FF FF  mov         dword ptr [esp+28h],0FFFFFFFFh
            FF FF
  0062A18D: E8 CE 79 05 00     call        00681B60
  0062A192: 89 5E 1C           mov         dword ptr [esi+1Ch],ebx
  0062A195: E9 0A 01 00 00     jmp         0062A2A4
  0062A19A: 8B 87 7C 04 00 00  mov         eax,dword ptr [edi+0000047Ch]
  0062A1A0: 53                 push        ebx
  0062A1A1: 8D 48 04           lea         ecx,[eax+4]
  0062A1A4: E8 67 76 05 00     call        00681810
  0062A1A9: 8B 10              mov         edx,dword ptr [eax]
  0062A1AB: 8B C8              mov         ecx,eax
  0062A1AD: FF 52 10           call        dword ptr [edx+10h]
  0062A1B0: 3D F8 03 00 00     cmp         eax,3F8h
  0062A1B5: 74 1F              je          0062A1D6
  0062A1B7: 8B 87 7C 04 00 00  mov         eax,dword ptr [edi+0000047Ch]
  0062A1BD: 53                 push        ebx
  0062A1BE: 8D 48 04           lea         ecx,[eax+4]
  0062A1C1: E8 4A 76 05 00     call        00681810
  0062A1C6: 8B 10              mov         edx,dword ptr [eax]
  0062A1C8: 53                 push        ebx
  0062A1C9: 6A 01              push        1
  0062A1CB: 57                 push        edi
  0062A1CC: 8B C8              mov         ecx,eax
  0062A1CE: FF 52 18           call        dword ptr [edx+18h]
  0062A1D1: E9 8E 01 00 00     jmp         0062A364
  0062A1D6: 8B 46 1C           mov         eax,dword ptr [esi+1Ch]
  0062A1D9: 3B 05 84 CB B7 00  cmp         eax,dword ptr ds:[00B7CB84h]
  0062A1DF: 77 3C              ja          0062A21D
  0062A1E1: 89 5E 1C           mov         dword ptr [esi+1Ch],ebx
  0062A1E4: 80 BF 2D 07 00 00  cmp         byte ptr [edi+0000072Dh],4
            04
  0062A1EB: C7 44 24 10 0B 00  mov         dword ptr [esp+10h],0Bh
            00 00
  0062A1F3: 0F 84 AB 00 00 00  je          0062A2A4
  0062A1F9: 0F BE 8F 18 07 00  movsx       ecx,byte ptr [edi+00000718h]
            00
  0062A200: 6B C9 1C           imul        ecx,ecx,1Ch
  0062A203: 39 9C 39 A0 05 00  cmp         dword ptr [ecx+edi+000005A0h],ebx
            00
  0062A20A: 0F 85 94 00 00 00  jne         0062A2A4
  0062A210: C7 44 24 10 0C 00  mov         dword ptr [esp+10h],0Ch
            00 00
  0062A218: E9 87 00 00 00     jmp         0062A2A4
  0062A21D: 8B 56 0C           mov         edx,dword ptr [esi+0Ch]
  0062A220: 8A 42 36           mov         al,byte ptr [edx+36h]
  0062A223: 24 07              and         al,7
  0062A225: 3C 03              cmp         al,3
  0062A227: 75 47              jne         0062A270
  0062A229: 39 5E 20           cmp         dword ptr [esi+20h],ebx
  0062A22C: 75 42              jne         0062A270
  0062A22E: 68 00 00 C8 42     push        42C80000h
  0062A233: 53                 push        ebx
  0062A234: E8 57 1B DF FF     call        0041BD90
  0062A239: D9 05 54 2E 8D 00  fld         dword ptr ds:[008D2E54h]
  0062A23F: D8 4C 24 30        fmul        dword ptr [esp+30h]
  0062A243: 83 C4 08           add         esp,8
  0062A246: D8 0D 38 90 85 00  fmul        dword ptr ds:[00859038h]
  0062A24C: DE D9              fcompp
  0062A24E: DF E0              fnstsw      ax
  0062A250: F6 C4 41           test        ah,41h
  0062A253: 75 1B              jne         0062A270
  0062A255: 8B 0D 5C 2E 8D 00  mov         ecx,dword ptr ds:[008D2E5Ch]
  0062A25B: 8B 15 58 2E 8D 00  mov         edx,dword ptr ds:[008D2E58h]
  0062A261: 51                 push        ecx
  0062A262: 52                 push        edx
  0062A263: E8 18 CF DD FF     call        00407180
  0062A268: 83 C4 08           add         esp,8
  0062A26B: 89 46 20           mov         dword ptr [esi+20h],eax
  0062A26E: EB 2C              jmp         0062A29C
  0062A270: 8B 6E 20           mov         ebp,dword ptr [esi+20h]
  0062A273: 3B EB              cmp         ebp,ebx
  0062A275: 76 2D              jbe         0062A2A4
  0062A277: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  0062A27D: D8 0D 38 8B 85 00  fmul        dword ptr ds:[00858B38h]
  0062A283: D8 0D 4C 8C 85 00  fmul        dword ptr ds:[00858C4Ch]
  0062A289: E8 B2 78 1F 00     call        00821B40
  0062A28E: 3B E8              cmp         ebp,eax
  0062A290: 76 07              jbe         0062A299
  0062A292: 2B E8              sub         ebp,eax
  0062A294: 89 6E 20           mov         dword ptr [esi+20h],ebp
  0062A297: EB 03              jmp         0062A29C
  0062A299: 89 5E 20           mov         dword ptr [esi+20h],ebx
  0062A29C: C7 44 24 10 02 00  mov         dword ptr [esp+10h],2
            00 00
  0062A2A4: 8B 87 7C 04 00 00  mov         eax,dword ptr [edi+0000047Ch]
  0062A2AA: 53                 push        ebx
  0062A2AB: 8D 48 04           lea         ecx,[eax+4]
  0062A2AE: E8 5D 75 05 00     call        00681810
  0062A2B3: 8B E8              mov         ebp,eax
  0062A2B5: 0F BE 45 24        movsx       eax,byte ptr [ebp+24h]
  0062A2B9: 83 C0 FC           add         eax,0FFFFFFFCh
  0062A2BC: 33 C9              xor         ecx,ecx
  0062A2BE: 3B C3              cmp         eax,ebx
  0062A2C0: 0F 9C C1           setl        cl
  0062A2C3: 49                 dec         ecx
  0062A2C4: 23 C1              and         eax,ecx
  0062A2C6: 69 C0 88 00 00 00  imul        eax,eax,88h
  0062A2CC: D9 80 D4 70 C1 00  fld         dword ptr [eax+00C170D4h]
  0062A2D2: 8B 46 1C           mov         eax,dword ptr [esi+1Ch]
  0062A2D5: 3B C3              cmp         eax,ebx
  0062A2D7: D9 5E 14           fstp        dword ptr [esi+14h]
  0062A2DA: 75 61              jne         0062A33D
  0062A2DC: E8 3D 78 1F 00     call        00821B1E
  0062A2E1: 89 44 24 14        mov         dword ptr [esp+14h],eax
  0062A2E5: DB 44 24 14        fild        dword ptr [esp+14h]
  0062A2E9: 8B 15 4C 2E 8D 00  mov         edx,dword ptr ds:[008D2E4Ch]
  0062A2EF: 8B C2              mov         eax,edx
  0062A2F1: 85 C0              test        eax,eax
  0062A2F3: D8 0D 7C 8C 85 00  fmul        dword ptr ds:[00858C7Ch]
  0062A2F9: 89 56 1C           mov         dword ptr [esi+1Ch],edx
  0062A2FC: D8 05 84 8C 85 00  fadd        dword ptr ds:[00858C84h]
  0062A302: D9 44 24 28        fld         dword ptr [esp+28h]
  0062A306: D8 4E 18           fmul        dword ptr [esi+18h]
  0062A309: D8 0D 38 90 85 00  fmul        dword ptr ds:[00859038h]
  0062A30F: D8 0D B0 8C 85 00  fmul        dword ptr ds:[00858CB0h]
  0062A315: D8 05 24 8C 85 00  fadd        dword ptr ds:[00858C24h]
  0062A31B: DE F9              fdivp       st(1),st
  0062A31D: DB 46 1C           fild        dword ptr [esi+1Ch]
  0062A320: 7D 06              jge         0062A328
  0062A322: D8 05 54 8C 85 00  fadd        dword ptr ds:[00858C54h]
  0062A328: DE C9              fmulp       st(1),st
  0062A32A: E8 11 78 1F 00     call        00821B40
  0062A32F: 89 46 1C           mov         dword ptr [esi+1Ch],eax
  0062A332: 8B 0D 84 CB B7 00  mov         ecx,dword ptr ds:[00B7CB84h]
  0062A338: 03 C1              add         eax,ecx
  0062A33A: 89 46 1C           mov         dword ptr [esi+1Ch],eax
  0062A33D: 80 7D 24 01        cmp         byte ptr [ebp+24h],1
  0062A341: 7F 11              jg          0062A354
  0062A343: 57                 push        edi
  0062A344: 8B CE              mov         ecx,esi
  0062A346: E8 05 A8 FF FF     call        00624B50
  0062A34B: 66 3D FF FF        cmp         ax,0FFFFh
  0062A34F: 0F BF C0           movsx       eax,ax
  0062A352: 7F 04              jg          0062A358
  0062A354: 8B 44 24 10        mov         eax,dword ptr [esp+10h]
  0062A358: 8B 56 0C           mov         edx,dword ptr [esi+0Ch]
  0062A35B: 50                 push        eax
  0062A35C: 52                 push        edx
  0062A35D: 8B CD              mov         ecx,ebp
  0062A35F: E8 7C 22 FF FF     call        0061C5E0
  0062A364: 8B 4C 24 18        mov         ecx,dword ptr [esp+18h]
  0062A368: 5F                 pop         edi
  0062A369: 5D                 pop         ebp
  0062A36A: 5E                 pop         esi
  0062A36B: 32 C0              xor         al,al
  0062A36D: 5B                 pop         ebx
  0062A36E: 64 89 0D 00 00 00  mov         dword ptr fs:[00000000h],ecx
            00
  0062A375: 83 C4 14           add         esp,14h
  0062A378: C2 04 00           ret         4
  0062A37B: 90                 nop
  0062A37C: 90                 nop
  0062A37D: 90                 nop
  0062A37E: 90                 nop
  0062A37F: 90                 nop
  0062A380: 6A FF              push        0FFFFFFFFh
  0062A382: 64 A1 00 00 00 00  mov         eax,dword ptr fs:[00000000h]
  0062A388: 68 98 00 84 00     push        840098h
  0062A38D: 50                 push        eax
  0062A38E: 64 89 25 00 00 00  mov         dword ptr fs:[00000000h],esp
            00
  0062A395: 83 EC 74           sub         esp,74h
  0062A398: 53                 push        ebx
  0062A399: 55                 push        ebp
  0062A39A: 56                 push        esi
  0062A39B: 8B F1              mov         esi,ecx
  0062A39D: 8A 46 3A           mov         al,byte ptr [esi+3Ah]
  0062A3A0: 84 C0              test        al,al
  0062A3A2: 57                 push        edi
  0062A3A3: 8B BC 24 94 00 00  mov         edi,dword ptr [esp+00000094h]
            00
  0062A3AA: 74 12              je          0062A3BE
  0062A3AC: 57                 push        edi
  0062A3AD: B9 48 54 C1 00     mov         ecx,0C15448h
  0062A3B2: E8 E9 DD FE FF     call        006181A0
  0062A3B7: 84 C0              test        al,al
  0062A3B9: 75 03              jne         0062A3BE
  0062A3BB: 88 46 3A           mov         byte ptr [esi+3Ah],al
  0062A3BE: 8A 46 39           mov         al,byte ptr [esi+39h]
  0062A3C1: 33 DB              xor         ebx,ebx
  0062A3C3: 84 C0              test        al,al
  0062A3C5: 74 13              je          0062A3DA
  0062A3C7: 57                 push        edi
  0062A3C8: 53                 push        ebx
  0062A3C9: B9 48 54 C1 00     mov         ecx,0C15448h
  0062A3CE: E8 DD DE FE FF     call        006182B0
  0062A3D3: 84 C0              test        al,al
  0062A3D5: 75 03              jne         0062A3DA
  0062A3D7: 88 46 39           mov         byte ptr [esi+39h],al
  0062A3DA: 39 5E 30           cmp         dword ptr [esi+30h],ebx
  0062A3DD: C6 46 0D 00        mov         byte ptr [esi+0Dh],0
  0062A3E1: 75 6A              jne         0062A44D
  0062A3E3: 8B 87 80 04 00 00  mov         eax,dword ptr [edi+00000480h]
  0062A3E9: 3B C3              cmp         eax,ebx
  0062A3EB: 74 0F              je          0062A3FC
  0062A3ED: 8A 40 20           mov         al,byte ptr [eax+20h]
  0062A3F0: 3A 87 18 07 00 00  cmp         al,byte ptr [edi+00000718h]
  0062A3F6: 0F 85 B7 01 00 00  jne         0062A5B3
  0062A3FC: 0F BE 8F 18 07 00  movsx       ecx,byte ptr [edi+00000718h]
            00
  0062A403: 6B C9 1C           imul        ecx,ecx,1Ch
  0062A406: 8B AC 39 A0 05 00  mov         ebp,dword ptr [ecx+edi+000005A0h]
            00
  0062A40D: 8D 84 39 A0 05 00  lea         eax,[ecx+edi+000005A0h]
            00
  0062A414: 8B CF              mov         ecx,edi
  0062A416: E8 65 C1 FB FF     call        005E6580
  0062A41B: 50                 push        eax
  0062A41C: 55                 push        ebp
  0062A41D: E8 3E 98 11 00     call        00743C60
  0062A422: 89 46 30           mov         dword ptr [esi+30h],eax
  0062A425: 8B 08              mov         ecx,dword ptr [eax]
  0062A427: 83 C4 08           add         esp,8
  0062A42A: 3B CB              cmp         ecx,ebx
  0062A42C: 74 13              je          0062A441
  0062A42E: 8B 50 18           mov         edx,dword ptr [eax+18h]
  0062A431: C1 EA 08           shr         edx,8
  0062A434: F6 C2 01           test        dl,1
  0062A437: 75 08              jne         0062A441
  0062A439: 89 5E 14           mov         dword ptr [esi+14h],ebx
  0062A43C: 89 5E 18           mov         dword ptr [esi+18h],ebx
  0062A43F: EB 47              jmp         0062A488
  0062A441: C6 46 08 01        mov         byte ptr [esi+8],1
  0062A445: 89 5E 30           mov         dword ptr [esi+30h],ebx
  0062A448: E9 DA 0A 00 00     jmp         0062AF27
  0062A44D: 0F BE 87 18 07 00  movsx       eax,byte ptr [edi+00000718h]
            00
  0062A454: 6B C0 1C           imul        eax,eax,1Ch
  0062A457: 8B AC 38 A0 05 00  mov         ebp,dword ptr [eax+edi+000005A0h]
            00
  0062A45E: 8D 84 38 A0 05 00  lea         eax,[eax+edi+000005A0h]
            00
  0062A465: 8B CF              mov         ecx,edi
  0062A467: E8 14 C1 FB FF     call        005E6580
  0062A46C: 50                 push        eax
  0062A46D: 55                 push        ebp
  0062A46E: E8 ED 97 11 00     call        00743C60
  0062A473: 8B 4E 30           mov         ecx,dword ptr [esi+30h]
  0062A476: 83 C4 08           add         esp,8
  0062A479: 3B C8              cmp         ecx,eax
  0062A47B: 74 0B              je          0062A488
  0062A47D: 8B 16              mov         edx,dword ptr [esi]
  0062A47F: 53                 push        ebx
  0062A480: 6A 01              push        1
  0062A482: 57                 push        edi
  0062A483: 8B CE              mov         ecx,esi
  0062A485: FF 52 18           call        dword ptr [edx+18h]
  0062A488: 8B 46 30           mov         eax,dword ptr [esi+30h]
  0062A48B: 8B 48 18           mov         ecx,dword ptr [eax+18h]
  0062A48E: C1 E9 02           shr         ecx,2
  0062A491: F6 C1 01           test        cl,1
  0062A494: 74 08              je          0062A49E
  0062A496: 39 9F 80 04 00 00  cmp         dword ptr [edi+00000480h],ebx
  0062A49C: 75 0A              jne         0062A4A8
  0062A49E: 81 8F 74 04 00 00  or          dword ptr [edi+00000474h],8000h
            00 80 00 00
  0062A4A8: 8A 46 08           mov         al,byte ptr [esi+8]
  0062A4AB: 84 C0              test        al,al
  0062A4AD: 0F 85 67 0A 00 00  jne         0062AF1A
  0062A4B3: 8B AF 80 04 00 00  mov         ebp,dword ptr [edi+00000480h]
  0062A4B9: 3B EB              cmp         ebp,ebx
  0062A4BB: 74 27              je          0062A4E4
  0062A4BD: D9 45 2C           fld         dword ptr [ebp+2Ch]
  0062A4C0: DC 1D F8 9E 85 00  fcomp       qword ptr ds:[00859EF8h]
  0062A4C6: DF E0              fnstsw      ax
  0062A4C8: F6 C4 41           test        ah,41h
  0062A4CB: 75 17              jne         0062A4E4
  0062A4CD: D9 05 64 2E 8D 00  fld         dword ptr ds:[008D2E64h]
  0062A4D3: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  0062A4D9: E8 52 7C 1F 00     call        00822130
  0062A4DE: D8 4D 2C           fmul        dword ptr [ebp+2Ch]
  0062A4E1: D9 5D 2C           fstp        dword ptr [ebp+2Ch]
  0062A4E4: 8A 46 09           mov         al,byte ptr [esi+9]
  0062A4E7: 84 C0              test        al,al
  0062A4E9: 0F 84 0B 0A 00 00  je          0062AEFA
  0062A4EF: C6 46 38 FF        mov         byte ptr [esi+38h],0FFh
  0062A4F3: 8B 87 6C 04 00 00  mov         eax,dword ptr [edi+0000046Ch]
  0062A4F9: C1 E8 1A           shr         eax,1Ah
  0062A4FC: 83 E0 01           and         eax,1
  0062A4FF: 74 0C              je          0062A50D
  0062A501: F7 87 74 04 00 00  test        dword ptr [edi+00000474h],40000h
            00 00 04 00
  0062A50B: 75 25              jne         0062A532
  0062A50D: 3B C3              cmp         eax,ebx
  0062A50F: 0F 85 BB 00 00 00  jne         0062A5D0
  0062A515: F6 87 76 04 00 00  test        byte ptr [edi+00000476h],1
            01
  0062A51C: 0F 84 AE 00 00 00  je          0062A5D0
  0062A522: 8B 56 30           mov         edx,dword ptr [esi+30h]
  0062A525: 8B 42 18           mov         eax,dword ptr [edx+18h]
  0062A528: D1 E8              shr         eax,1
  0062A52A: A8 01              test        al,1
  0062A52C: 0F 85 9E 00 00 00  jne         0062A5D0
  0062A532: 80 7E 10 05        cmp         byte ptr [esi+10h],5
  0062A536: 74 15              je          0062A54D
  0062A538: 8A 4E 0F           mov         cl,byte ptr [esi+0Fh]
  0062A53B: 80 F9 05           cmp         cl,5
  0062A53E: 0F 85 C2 00 00 00  jne         0062A606
  0062A544: 39 5E 2C           cmp         dword ptr [esi+2Ch],ebx
  0062A547: 0F 85 B9 00 00 00  jne         0062A606
  0062A54D: 8B 6E 2C           mov         ebp,dword ptr [esi+2Ch]
  0062A550: 3B EB              cmp         ebp,ebx
  0062A552: 0F 85 FF 00 00 00  jne         0062A657
  0062A558: 57                 push        edi
  0062A559: 8B CE              mov         ecx,esi
  0062A55B: E8 D0 A9 FF FF     call        00624F30
  0062A560: 39 5E 2C           cmp         dword ptr [esi+2Ch],ebx
  0062A563: 75 08              jne         0062A56D
  0062A565: 8B CE              mov         ecx,esi
  0062A567: 57                 push        edi
  0062A568: E8 33 3A FF FF     call        0061DFA0
  0062A56D: 8B 4E 30           mov         ecx,dword ptr [esi+30h]
  0062A570: 8B 51 18           mov         edx,dword ptr [ecx+18h]
  0062A573: D1 EA              shr         edx,1
  0062A575: F6 C2 01           test        dl,1
  0062A578: 74 13              je          0062A58D
  0062A57A: F7 87 6C 04 00 00  test        dword ptr [edi+0000046Ch],4000000h
            00 00 00 04
  0062A584: 75 07              jne         0062A58D
  0062A586: 8A 46 0A           mov         al,byte ptr [esi+0Ah]
  0062A589: 84 C0              test        al,al
  0062A58B: 74 08              je          0062A595
  0062A58D: 57                 push        edi
  0062A58E: 8B CE              mov         ecx,esi
  0062A590: E8 5B 3E FF FF     call        0061E3F0
  0062A595: 8A 46 0F           mov         al,byte ptr [esi+0Fh]
  0062A598: 3C 06              cmp         al,6
  0062A59A: 7D 13              jge         0062A5AF
  0062A59C: 3C 04              cmp         al,4
  0062A59E: 75 0B              jne         0062A5AB
  0062A5A0: 8A 46 10           mov         al,byte ptr [esi+10h]
  0062A5A3: 3C 02              cmp         al,2
  0062A5A5: 74 08              je          0062A5AF
  0062A5A7: 3C 03              cmp         al,3
  0062A5A9: 74 04              je          0062A5AF
  0062A5AB: C6 46 0F 00        mov         byte ptr [esi+0Fh],0
  0062A5AF: C6 46 09 00        mov         byte ptr [esi+9],0
  0062A5B3: 32 C0              xor         al,al
  0062A5B5: 8B 8C 24 84 00 00  mov         ecx,dword ptr [esp+00000084h]
            00
  0062A5BC: 5F                 pop         edi
  0062A5BD: 5E                 pop         esi
  0062A5BE: 5D                 pop         ebp
  0062A5BF: 64 89 0D 00 00 00  mov         dword ptr fs:[00000000h],ecx
            00
  0062A5C6: 5B                 pop         ebx
  0062A5C7: 81 C4 80 00 00 00  add         esp,80h
  0062A5CD: C2 04 00           ret         4
  0062A5D0: 80 7E 0F 05        cmp         byte ptr [esi+0Fh],5
  0062A5D4: 0F 85 73 FF FF FF  jne         0062A54D
  0062A5DA: 80 7E 10 03        cmp         byte ptr [esi+10h],3
  0062A5DE: 0F 8F 69 FF FF FF  jg          0062A54D
  0062A5E4: 8B 4E 2C           mov         ecx,dword ptr [esi+2Ch]
  0062A5E7: 3B CB              cmp         ecx,ebx
  0062A5E9: 74 0B              je          0062A5F6
  0062A5EB: 53                 push        ebx
  0062A5EC: 68 90 13 4D 00     push        4D1390h
  0062A5F1: E8 CA 45 EA FF     call        004CEBC0
  0062A5F6: 57                 push        edi
  0062A5F7: 8B CE              mov         ecx,esi
  0062A5F9: 89 5E 2C           mov         dword ptr [esi+2Ch],ebx
  0062A5FC: E8 9F 39 FF FF     call        0061DFA0
  0062A601: E9 47 FF FF FF     jmp         0062A54D
  0062A606: 8B 46 2C           mov         eax,dword ptr [esi+2Ch]
  0062A609: 3B C3              cmp         eax,ebx
  0062A60B: 75 16              jne         0062A623
  0062A60D: 80 F9 01           cmp         cl,1
  0062A610: 0F

  Summary

      456000 .text
