Microsoft (R) COFF/PE Dumper Version 14.51.36257.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file D:\Grand Theft Auto San Andreas\gta-sa.exe

File Type: EXECUTABLE IMAGE

  00687530: 83 EC 28           sub         esp,28h
  00687533: 53                 push        ebx
  00687534: 55                 push        ebp
  00687535: 56                 push        esi
  00687536: 8B 74 24 38        mov         esi,dword ptr [esp+38h]
  0068753A: 57                 push        edi
  0068753B: 8B F9              mov         edi,ecx
  0068753D: 8B 8E 7C 04 00 00  mov         ecx,dword ptr [esi+0000047Ch]
  00687543: E8 E8 99 F7 FF     call        00600F30
  00687548: 85 C0              test        eax,eax
  0068754A: 89 44 24 14        mov         dword ptr [esp+14h],eax
  0068754E: 0F 84 BA 06 00 00  je          00687C0E
  00687554: 0F BE 86 18 07 00  movsx       eax,byte ptr [esi+00000718h]
            00
  0068755B: 6B C0 1C           imul        eax,eax,1Ch
  0068755E: 8B 84 30 A0 05 00  mov         eax,dword ptr [eax+esi+000005A0h]
            00
  00687565: 6A 01              push        1
  00687567: 50                 push        eax
  00687568: 32 DB              xor         bl,bl
  0068756A: E8 F1 C6 0B 00     call        00743C60
  0068756F: 8B 08              mov         ecx,dword ptr [eax]
  00687571: 83 C4 08           add         esp,8
  00687574: 85 C9              test        ecx,ecx
  00687576: 74 02              je          0068757A
  00687578: B3 01              mov         bl,1
  0068757A: 8B CE              mov         ecx,esi
  0068757C: E8 DF 1F F8 FF     call        00609560
  00687581: 8B E8              mov         ebp,eax
  00687583: 8B CD              mov         ecx,ebp
  00687585: E8 06 87 EB FF     call        0053FC90
  0068758A: 0F BF C8           movsx       ecx,ax
  0068758D: 89 4C 24 3C        mov         dword ptr [esp+3Ch],ecx
  00687591: 8B CD              mov         ecx,ebp
  00687593: DB 44 24 3C        fild        dword ptr [esp+3Ch]
  00687597: D9 5C 24 1C        fstp        dword ptr [esp+1Ch]
  0068759B: E8 90 87 EB FF     call        0053FD30
  006875A0: 0F BF D0           movsx       edx,ax
  006875A3: 89 54 24 3C        mov         dword ptr [esp+3Ch],edx
  006875A7: 56                 push        esi
  006875A8: DB 44 24 40        fild        dword ptr [esp+40h]
  006875AC: D9 5C 24 1C        fstp        dword ptr [esp+1Ch]
  006875B0: D9 44 24 20        fld         dword ptr [esp+20h]
  006875B4: D8 0D 88 8B 85 00  fmul        dword ptr ds:[00858B88h]
  006875BA: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006875BE: D9 44 24 1C        fld         dword ptr [esp+1Ch]
  006875C2: D8 0D 88 8B 85 00  fmul        dword ptr ds:[00858B88h]
  006875C8: D9 5C 24 2C        fstp        dword ptr [esp+2Ch]
  006875CC: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  006875D2: D8 0D A8 8C 85 00  fmul        dword ptr ds:[00858CA8h]
  006875D8: D9 5C 24 14        fstp        dword ptr [esp+14h]
  006875DC: E8 3F AA DB FF     call        00442020
  006875E1: 83 C4 04           add         esp,4
  006875E4: 84 C0              test        al,al
  006875E6: 0F 84 E2 00 00 00  je          006876CE
  006875EC: D9 44 24 24        fld         dword ptr [esp+24h]
  006875F0: D8 4C 24 24        fmul        dword ptr [esp+24h]
  006875F4: D9 44 24 28        fld         dword ptr [esp+28h]
  006875F8: D8 4C 24 28        fmul        dword ptr [esp+28h]
  006875FC: DE C1              faddp       st(1),st
  006875FE: D9 FA              fsqrt
  00687600: D9 54 24 3C        fst         dword ptr [esp+3Ch]
  00687604: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  0068760A: DF E0              fnstsw      ax
  0068760C: F6 C4 41           test        ah,41h
  0068760F: 0F 85 9A 01 00 00  jne         006877AF
  00687615: 8B 44 24 28        mov         eax,dword ptr [esp+28h]
  00687619: D9 44 24 24        fld         dword ptr [esp+24h]
  0068761D: 50                 push        eax
  0068761E: D9 E0              fchs
  00687620: 51                 push        ecx
  00687621: D9 1C 24           fstp        dword ptr [esp]
  00687624: 6A 00              push        0
  00687626: 6A 00              push        0
  00687628: E8 B3 55 EB FF     call        0053CBE0
  0068762D: D8 25 78 F1 B6 00  fsub        dword ptr ds:[00B6F178h]
  00687633: D9 5C 24 30        fstp        dword ptr [esp+30h]
  00687637: 8B 4C 24 30        mov         ecx,dword ptr [esp+30h]
  0068763B: 51                 push        ecx
  0068763C: E8 0F 55 EB FF     call        0053CB50
  00687641: D9 C0              fld         st(0)
  00687643: D9 FE              fsin
  00687645: 83 C4 14           add         esp,14h
  00687648: 6A 00              push        0
  0068764A: 83 EC 0C           sub         esp,0Ch
  0068764D: 8B D4              mov         edx,esp
  0068764F: C7 44 24 44 00 00  mov         dword ptr [esp+44h],0
            00 00
  00687657: 56                 push        esi
  00687658: D9 E0              fchs
  0068765A: D9 5C 24 40        fstp        dword ptr [esp+40h]
  0068765E: 8B 44 24 40        mov         eax,dword ptr [esp+40h]
  00687662: 89 02              mov         dword ptr [edx],eax
  00687664: D9 FF              fcos
  00687666: 8B 44 24 48        mov         eax,dword ptr [esp+48h]
  0068766A: D9 5C 24 44        fstp        dword ptr [esp+44h]
  0068766E: 8B 4C 24 44        mov         ecx,dword ptr [esp+44h]
  00687672: 89 4A 04           mov         dword ptr [edx+4],ecx
  00687675: 89 42 08           mov         dword ptr [edx+8],eax
  00687678: E8 93 A7 DB FF     call        00441E10
  0068767D: 83 C4 14           add         esp,14h
  00687680: 84 C0              test        al,al
  00687682: 74 35              je          006876B9
  00687684: 8B 46 14           mov         eax,dword ptr [esi+14h]
  00687687: D9 44 24 30        fld         dword ptr [esp+30h]
  0068768B: D8 48 04           fmul        dword ptr [eax+4]
  0068768E: D9 44 24 2C        fld         dword ptr [esp+2Ch]
  00687692: D8 08              fmul        dword ptr [eax]
  00687694: DE C1              faddp       st(1),st
  00687696: D9 40 08           fld         dword ptr [eax+8]
  00687699: D8 0D 50 8B 85 00  fmul        dword ptr ds:[00858B50h]
  0068769F: DE C1              faddp       st(1),st
  006876A1: D8 4C 24 3C        fmul        dword ptr [esp+3Ch]
  006876A5: D9 5C 24 24        fstp        dword ptr [esp+24h]
  006876A9: D9 44 24 30        fld         dword ptr [esp+30h]
  006876AD: D8 48 14           fmul        dword ptr [eax+14h]
  006876B0: D9 44 24 2C        fld         dword ptr [esp+2Ch]
  006876B4: E9 DC 00 00 00     jmp         00687795
  006876B9: C7 44 24 24 00 00  mov         dword ptr [esp+24h],0
            00 00
  006876C1: C7 44 24 28 00 00  mov         dword ptr [esp+28h],0
            00 00
  006876C9: E9 E1 00 00 00     jmp         006877AF
  006876CE: 8B 86 1C 07 00 00  mov         eax,dword ptr [esi+0000071Ch]
  006876D4: 85 C0              test        eax,eax
  006876D6: 75 1C              jne         006876F4
  006876D8: A0 2E EC B6 00     mov         al,byte ptr ds:[00B6EC2Eh]
  006876DD: 84 C0              test        al,al
  006876DF: 0F 84 CA 00 00 00  je          006877AF
  006876E5: 8B CD              mov         ecx,ebp
  006876E7: E8 84 8F EB FF     call        00540670
  006876EC: 84 C0              test        al,al
  006876EE: 0F 84 BB 00 00 00  je          006877AF
  006876F4: D9 44 24 24        fld         dword ptr [esp+24h]
  006876F8: D8 4C 24 24        fmul        dword ptr [esp+24h]
  006876FC: D9 44 24 28        fld         dword ptr [esp+28h]
  00687700: D8 4C 24 28        fmul        dword ptr [esp+28h]
  00687704: DE C1              faddp       st(1),st
  00687706: D9 FA              fsqrt
  00687708: D9 54 24 3C        fst         dword ptr [esp+3Ch]
  0068770C: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687712: DF E0              fnstsw      ax
  00687714: F6 C4 41           test        ah,41h
  00687717: 0F 85 92 00 00 00  jne         006877AF
  0068771D: D9 44 24 3C        fld         dword ptr [esp+3Ch]
  00687721: D8 1D 24 86 85 00  fcomp       dword ptr ds:[00858624h]
  00687727: DF E0              fnstsw      ax
  00687729: F6 C4 41           test        ah,41h
  0068772C: 75 08              jne         00687736
  0068772E: C7 44 24 3C 00 00  mov         dword ptr [esp+3Ch],3F800000h
            80 3F
  00687736: 8B 4C 24 28        mov         ecx,dword ptr [esp+28h]
  0068773A: D9 44 24 24        fld         dword ptr [esp+24h]
  0068773E: 51                 push        ecx
  0068773F: D9 E0              fchs
  00687741: 51                 push        ecx
  00687742: D9 1C 24           fstp        dword ptr [esp]
  00687745: 6A 00              push        0
  00687747: 6A 00              push        0
  00687749: E8 92 54 EB FF     call        0053CBE0
  0068774E: D8 25 78 F1 B6 00  fsub        dword ptr ds:[00B6F178h]
  00687754: D9 5C 24 30        fstp        dword ptr [esp+30h]
  00687758: 8B 54 24 30        mov         edx,dword ptr [esp+30h]
  0068775C: 52                 push        edx
  0068775D: E8 EE 53 EB FF     call        0053CB50
  00687762: D9 C0              fld         st(0)
  00687764: D9 FE              fsin
  00687766: 8B 46 14           mov         eax,dword ptr [esi+14h]
  00687769: 83 C4 14           add         esp,14h
  0068776C: D9 E0              fchs
  0068776E: D9 C9              fxch        st(1)
  00687770: D9 FF              fcos
  00687772: D9 C0              fld         st(0)
  00687774: D8 48 04           fmul        dword ptr [eax+4]
  00687777: D9 C2              fld         st(2)
  00687779: D8 08              fmul        dword ptr [eax]
  0068777B: DE C1              faddp       st(1),st
  0068777D: D9 40 08           fld         dword ptr [eax+8]
  00687780: D8 0D 50 8B 85 00  fmul        dword ptr ds:[00858B50h]
  00687786: DE C1              faddp       st(1),st
  00687788: D8 4C 24 3C        fmul        dword ptr [esp+3Ch]
  0068778C: D9 5C 24 24        fstp        dword ptr [esp+24h]
  00687790: D8 48 14           fmul        dword ptr [eax+14h]
  00687793: D9 C9              fxch        st(1)
  00687795: D8 48 10           fmul        dword ptr [eax+10h]
  00687798: DE C1              faddp       st(1),st
  0068779A: D9 40 18           fld         dword ptr [eax+18h]
  0068779D: D8 0D 50 8B 85 00  fmul        dword ptr ds:[00858B50h]
  006877A3: DE C1              faddp       st(1),st
  006877A5: D8 4C 24 3C        fmul        dword ptr [esp+3Ch]
  006877A9: D9 E0              fchs
  006877AB: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006877AF: 8B 86 1C 07 00 00  mov         eax,dword ptr [esi+0000071Ch]
  006877B5: 85 C0              test        eax,eax
  006877B7: 0F 85 14 02 00 00  jne         006879D1
  006877BD: A0 2E EC B6 00     mov         al,byte ptr ds:[00B6EC2Eh]
  006877C2: 84 C0              test        al,al
  006877C4: 74 0F              je          006877D5
  006877C6: 8B CD              mov         ecx,ebp
  006877C8: E8 A3 8E EB FF     call        00540670
  006877CD: 84 C0              test        al,al
  006877CF: 0F 85 FC 01 00 00  jne         006879D1
  006877D5: 8B 44 24 18        mov         eax,dword ptr [esp+18h]
  006877D9: D9 44 24 1C        fld         dword ptr [esp+1Ch]
  006877DD: 50                 push        eax
  006877DE: D9 E0              fchs
  006877E0: 51                 push        ecx
  006877E1: D9 1C 24           fstp        dword ptr [esp]
  006877E4: 6A 00              push        0
  006877E6: 6A 00              push        0
  006877E8: E8 F3 53 EB FF     call        0053CBE0
  006877ED: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006877F1: D9 44 24 34        fld         dword ptr [esp+34h]
  006877F5: 83 C4 10           add         esp,10h
  006877F8: D8 4C 24 24        fmul        dword ptr [esp+24h]
  006877FC: D9 44 24 28        fld         dword ptr [esp+28h]
  00687800: D8 4C 24 28        fmul        dword ptr [esp+28h]
  00687804: DE C1              faddp       st(1),st
  00687806: D9 FA              fsqrt
  00687808: D9 54 24 3C        fst         dword ptr [esp+3Ch]
  0068780C: D8 1D 24 86 85 00  fcomp       dword ptr ds:[00858624h]
  00687812: DF E0              fnstsw      ax
  00687814: F6 C4 41           test        ah,41h
  00687817: 75 5B              jne         00687874
  00687819: C7 44 24 3C 00 00  mov         dword ptr [esp+3Ch],3F800000h
            80 3F
  00687821: D9 44 24 28        fld         dword ptr [esp+28h]
  00687825: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  0068782B: DF E0              fnstsw      ax
  0068782D: F6 C4 41           test        ah,41h
  00687830: 0F 8A 9D 00 00 00  jp          006878D3
  00687836: D9 44 24 3C        fld         dword ptr [esp+3Ch]
  0068783A: 8B 8E 80 04 00 00  mov         ecx,dword ptr [esi+00000480h]
  00687840: D9 E0              fchs
  00687842: D9 C0              fld         st(0)
  00687844: D8 61 10           fsub        dword ptr [ecx+10h]
  00687847: D9 54 24 1C        fst         dword ptr [esp+1Ch]
  0068784B: D9 E1              fabs
  0068784D: D8 5C 24 10        fcomp       dword ptr [esp+10h]
  00687851: DF E0              fnstsw      ax
  00687853: F6 C4 41           test        ah,41h
  00687856: 75 4A              jne         006878A2
  00687858: DD D8              fstp        st(0)
  0068785A: D9 44 24 1C        fld         dword ptr [esp+1Ch]
  0068785E: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687864: DF E0              fnstsw      ax
  00687866: F6 C4 41           test        ah,41h
  00687869: 75 30              jne         0068789B
  0068786B: D9 44 24 10        fld         dword ptr [esp+10h]
  0068786F: D8 41 10           fadd        dword ptr [ecx+10h]
  00687872: EB 2E              jmp         006878A2
  00687874: D9 44 24 3C        fld         dword ptr [esp+3Ch]
  00687878: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  0068787E: DF E0              fnstsw      ax
  00687880: F6 C4 44           test        ah,44h
  00687883: 7A 9C              jp          00687821
  00687885: 8B 8E 80 04 00 00  mov         ecx,dword ptr [esi+00000480h]
  0068788B: 33 C0              xor         eax,eax
  0068788D: 89 41 0C           mov         dword ptr [ecx+0Ch],eax
  00687890: 8B 96 80 04 00 00  mov         edx,dword ptr [esi+00000480h]
  00687896: 89 42 10           mov         dword ptr [edx+10h],eax
  00687899: EB 38              jmp         006878D3
  0068789B: D9 41 10           fld         dword ptr [ecx+10h]
  0068789E: D8 64 24 10        fsub        dword ptr [esp+10h]
  006878A2: D9 59 10           fstp        dword ptr [ecx+10h]
  006878A5: 8B 86 80 04 00 00  mov         eax,dword ptr [esi+00000480h]
  006878AB: D9 44 24 18        fld         dword ptr [esp+18h]
  006878AF: C7 40 0C 00 00 00  mov         dword ptr [eax+0Ch],0
            00
  006878B6: D8 25 78 F1 B6 00  fsub        dword ptr ds:[00B6F178h]
  006878BC: D9 5C 24 18        fstp        dword ptr [esp+18h]
  006878C0: 8B 4C 24 18        mov         ecx,dword ptr [esp+18h]
  006878C4: 51                 push        ecx
  006878C5: E8 86 52 EB FF     call        0053CB50
  006878CA: D9 9E 5C 05 00 00  fstp        dword ptr [esi+0000055Ch]
  006878D0: 83 C4 04           add         esp,4
  006878D3: 8B 86 1C 07 00 00  mov         eax,dword ptr [esi+0000071Ch]
  006878D9: 85 C0              test        eax,eax
  006878DB: 75 37              jne         00687914
  006878DD: D9 44 24 28        fld         dword ptr [esp+28h]
  006878E1: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  006878E7: DF E0              fnstsw      ax
  006878E9: F6 C4 41           test        ah,41h
  006878EC: 7A 26              jp          00687914
  006878EE: 8B 54 24 14        mov         edx,dword ptr [esp+14h]
  006878F2: 80 7A 27 0B        cmp         byte ptr [edx+27h],0Bh
  006878F6: 7D 1C              jge         00687914
  006878F8: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  006878FE: D8 0D 38 8B 85 00  fmul        dword ptr ds:[00858B38h]
  00687904: D8 0D 4C 8C 85 00  fmul        dword ptr ds:[00858C4Ch]
  0068790A: E8 31 A2 19 00     call        00821B40
  0068790F: 01 47 10           add         dword ptr [edi+10h],eax
  00687912: EB 07              jmp         0068791B
  00687914: C7 47 10 00 00 00  mov         dword ptr [edi+10h],0
            00
  0068791B: 8B CD              mov         ecx,ebp
  0068791D: E8 7E 8E EB FF     call        005407A0
  00687922: 84 C0              test        al,al
  00687924: 75 41              jne         00687967
  00687926: 8B CD              mov         ecx,ebp
  00687928: E8 D3 8D EB FF     call        00540700
  0068792D: 84 C0              test        al,al
  0068792F: 75 36              jne         00687967
  00687931: 84 DB              test        bl,bl
  00687933: 75 32              jne         00687967
  00687935: D9 44 24 28        fld         dword ptr [esp+28h]
  00687939: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  0068793F: DF E0              fnstsw      ax
  00687941: F6 C4 41           test        ah,41h
  00687944: 74 21              je          00687967
  00687946: 8B 47 10           mov         eax,dword ptr [edi+10h]
  00687949: DB 47 10           fild        dword ptr [edi+10h]
  0068794C: 85 C0              test        eax,eax
  0068794E: 7D 06              jge         00687956
  00687950: D8 05 54 8C 85 00  fadd        dword ptr ds:[00858C54h]
  00687956: D8 1D 70 08 87 00  fcomp       dword ptr ds:[00870870h]
  0068795C: DF E0              fnstsw      ax
  0068795E: F6 C4 01           test        ah,1
  00687961: 0F 85 01 02 00 00  jne         00687B68
  00687967: 8B CD              mov         ecx,ebp
  00687969: E8 32 8E EB FF     call        005407A0
  0068796E: D9 44 24 3C        fld         dword ptr [esp+3Ch]
  00687972: D8 1D 8C 8B 85 00  fcomp       dword ptr ds:[00858B8Ch]
  00687978: BF 10 00 00 00     mov         edi,10h
  0068797D: DF E0              fnstsw      ax
  0068797F: F6 C4 41           test        ah,41h
  00687982: 74 05              je          00687989
  00687984: BF 0F 00 00 00     mov         edi,0Fh
  00687989: 8B CD              mov         ecx,ebp
  0068798B: E8 90 8D EB FF     call        00540720
  00687990: 84 C0              test        al,al
  00687992: 0F 84 B6 01 00 00  je          00687B4E
  00687998: 56                 push        esi
  00687999: E8 72 AC 00 00     call        00692610
  0068799E: 83 C4 04           add         esp,4
  006879A1: 84 C0              test        al,al
  006879A3: 0F 84 A5 01 00 00  je          00687B4E
  006879A9: 8B 8E 7C 04 00 00  mov         ecx,dword ptr [esi+0000047Ch]
  006879AF: 6A 00              push        0
  006879B1: E8 7A 98 F7 FF     call        00601230
  006879B6: 8B 4C 24 14        mov         ecx,dword ptr [esp+14h]
  006879BA: BF 12 00 00 00     mov         edi,12h
  006879BF: 57                 push        edi
  006879C0: 6A 00              push        0
  006879C2: E8 19 4C F9 FF     call        0061C5E0
  006879C7: 5F                 pop         edi
  006879C8: 5E                 pop         esi
  006879C9: 5D                 pop         ebp
  006879CA: 5B                 pop         ebx
  006879CB: 83 C4 28           add         esp,28h
  006879CE: C2 04 00           ret         4
  006879D1: 8B 8E 80 04 00 00  mov         ecx,dword ptr [esi+00000480h]
  006879D7: D9 44 24 28        fld         dword ptr [esp+28h]
  006879DB: D8 61 10           fsub        dword ptr [ecx+10h]
  006879DE: D9 C0              fld         st(0)
  006879E0: D9 E1              fabs
  006879E2: D8 5C 24 10        fcomp       dword ptr [esp+10h]
  006879E6: DF E0              fnstsw      ax
  006879E8: F6 C4 41           test        ah,41h
  006879EB: 75 25              jne         00687A12
  006879ED: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  006879F3: DF E0              fnstsw      ax
  006879F5: F6 C4 41           test        ah,41h
  006879F8: 75 0C              jne         00687A06
  006879FA: D9 44 24 10        fld         dword ptr [esp+10h]
  006879FE: D8 41 10           fadd        dword ptr [ecx+10h]
  00687A01: D9 59 10           fstp        dword ptr [ecx+10h]
  00687A04: EB 15              jmp         00687A1B
  00687A06: D9 41 10           fld         dword ptr [ecx+10h]
  00687A09: D8 64 24 10        fsub        dword ptr [esp+10h]
  00687A0D: D9 59 10           fstp        dword ptr [ecx+10h]
  00687A10: EB 09              jmp         00687A1B
  00687A12: 8B 54 24 28        mov         edx,dword ptr [esp+28h]
  00687A16: DD D8              fstp        st(0)
  00687A18: 89 51 10           mov         dword ptr [ecx+10h],edx
  00687A1B: 8B 8E 80 04 00 00  mov         ecx,dword ptr [esi+00000480h]
  00687A21: D9 44 24 24        fld         dword ptr [esp+24h]
  00687A25: D8 61 0C           fsub        dword ptr [ecx+0Ch]
  00687A28: D9 C0              fld         st(0)
  00687A2A: D9 E1              fabs
  00687A2C: D8 5C 24 10        fcomp       dword ptr [esp+10h]
  00687A30: DF E0              fnstsw      ax
  00687A32: F6 C4 41           test        ah,41h
  00687A35: 75 25              jne         00687A5C
  00687A37: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687A3D: DF E0              fnstsw      ax
  00687A3F: F6 C4 41           test        ah,41h
  00687A42: 75 0C              jne         00687A50
  00687A44: D9 44 24 10        fld         dword ptr [esp+10h]
  00687A48: D8 41 0C           fadd        dword ptr [ecx+0Ch]
  00687A4B: D9 59 0C           fstp        dword ptr [ecx+0Ch]
  00687A4E: EB 15              jmp         00687A65
  00687A50: D9 41 0C           fld         dword ptr [ecx+0Ch]
  00687A53: D8 64 24 10        fsub        dword ptr [esp+10h]
  00687A57: D9 59 0C           fstp        dword ptr [ecx+0Ch]
  00687A5A: EB 09              jmp         00687A65
  00687A5C: 8B 44 24 24        mov         eax,dword ptr [esp+24h]
  00687A60: DD D8              fstp        st(0)
  00687A62: 89 41 0C           mov         dword ptr [ecx+0Ch],eax
  00687A65: 8B 8E 1C 07 00 00  mov         ecx,dword ptr [esi+0000071Ch]
  00687A6B: 85 C9              test        ecx,ecx
  00687A6D: 74 5B              je          00687ACA
  00687A6F: 8B 46 14           mov         eax,dword ptr [esi+14h]
  00687A72: 85 C0              test        eax,eax
  00687A74: 8D 50 30           lea         edx,[eax+30h]
  00687A77: 75 03              jne         00687A7C
  00687A79: 8D 56 04           lea         edx,[esi+4]
  00687A7C: 8B 41 14           mov         eax,dword ptr [ecx+14h]
  00687A7F: 85 C0              test        eax,eax
  00687A81: 74 05              je          00687A88
  00687A83: 83 C0 30           add         eax,30h
  00687A86: EB 03              jmp         00687A8B
  00687A88: 8D 41 04           lea         eax,[ecx+4]
  00687A8B: D9 00              fld         dword ptr [eax]
  00687A8D: D8 22              fsub        dword ptr [edx]
  00687A8F: D9 40 04           fld         dword ptr [eax+4]
  00687A92: D8 62 04           fsub        dword ptr [edx+4]
  00687A95: D9 40 08           fld         dword ptr [eax+8]
  00687A98: D8 62 08           fsub        dword ptr [edx+8]
  00687A9B: D9 C0              fld         st(0)
  00687A9D: D8 C9              fmul        st,st(1)
  00687A9F: D9 C2              fld         st(2)
  00687AA1: D8 CB              fmul        st,st(3)
  00687AA3: DE C1              faddp       st(1),st
  00687AA5: D9 C3              fld         st(3)
  00687AA7: D8 CC              fmul        st,st(4)
  00687AA9: DE C1              faddp       st(1),st
  00687AAB: D8 1D 6C 08 87 00  fcomp       dword ptr ds:[0087086Ch]
  00687AB1: DD D8              fstp        st(0)
  00687AB3: DF E0              fnstsw      ax
  00687AB5: DD D8              fstp        st(0)
  00687AB7: F6 C4 41           test        ah,41h
  00687ABA: DD D8              fstp        st(0)
  00687ABC: 75 0C              jne         00687ACA
  00687ABE: 8B 7C 24 14        mov         edi,dword ptr [esp+14h]
  00687AC2: 80 7F 27 0B        cmp         byte ptr [edi+27h],0Bh
  00687AC6: 7C 19              jl          00687AE1
  00687AC8: EB 04              jmp         00687ACE
  00687ACA: 8B 7C 24 14        mov         edi,dword ptr [esp+14h]
  00687ACE: 8B CD              mov         ecx,ebp
  00687AD0: E8 CB 8C EB FF     call        005407A0
  00687AD5: 84 C0              test        al,al
  00687AD7: 75 08              jne         00687AE1
  00687AD9: 84 DB              test        bl,bl
  00687ADB: 0F 84 8B 00 00 00  je          00687B6C
  00687AE1: 8B CD              mov         ecx,ebp
  00687AE3: E8 B8 8C EB FF     call        005407A0
  00687AE8: 84 C0              test        al,al
  00687AEA: 74 1B              je          00687B07
  00687AEC: 8B 4C 24 14        mov         ecx,dword ptr [esp+14h]
  00687AF0: BF 11 00 00 00     mov         edi,11h
  00687AF5: 57                 push        edi
  00687AF6: 6A 00              push        0
  00687AF8: E8 E3 4A F9 FF     call        0061C5E0
  00687AFD: 5F                 pop         edi
  00687AFE: 5E                 pop         esi
  00687AFF: 5D                 pop         ebp
  00687B00: 5B                 pop         ebx
  00687B01: 83 C4 28           add         esp,28h
  00687B04: C2 04 00           ret         4
  00687B07: D9 44 24 28        fld         dword ptr [esp+28h]
  00687B0B: D8 1D 40 8F 85 00  fcomp       dword ptr ds:[00858F40h]
  00687B11: DF E0              fnstsw      ax
  00687B13: F6 C4 05           test        ah,5
  00687B16: 7A 1B              jp          00687B33
  00687B18: 8B 4C 24 14        mov         ecx,dword ptr [esp+14h]
  00687B1C: BF 10 00 00 00     mov         edi,10h
  00687B21: 57                 push        edi
  00687B22: 6A 00              push        0
  00687B24: E8 B7 4A F9 FF     call        0061C5E0
  00687B29: 5F                 pop         edi
  00687B2A: 5E                 pop         esi
  00687B2B: 5D                 pop         ebp
  00687B2C: 5B                 pop         ebx
  00687B2D: 83 C4 28           add         esp,28h
  00687B30: C2 04 00           ret         4
  00687B33: 8B 4C 24 14        mov         ecx,dword ptr [esp+14h]
  00687B37: BF 0F 00 00 00     mov         edi,0Fh
  00687B3C: 57                 push        edi
  00687B3D: 6A 00              push        0
  00687B3F: E8 9C 4A F9 FF     call        0061C5E0
  00687B44: 5F                 pop         edi
  00687B45: 5E                 pop         esi
  00687B46: 5D                 pop         ebp
  00687B47: 5B                 pop         ebx
  00687B48: 83 C4 28           add         esp,28h
  00687B4B: C2 04 00           ret         4
  00687B4E: 85 FF              test        edi,edi
  00687B50: 76 16              jbe         00687B68
  00687B52: 8B 4C 24 14        mov         ecx,dword ptr [esp+14h]
  00687B56: 57                 push        edi
  00687B57: 6A 00              push        0
  00687B59: E8 82 4A F9 FF     call        0061C5E0
  00687B5E: 5F                 pop         edi
  00687B5F: 5E                 pop         esi
  00687B60: 5D                 pop         ebp
  00687B61: 5B                 pop         ebx
  00687B62: 83 C4 28           add         esp,28h
  00687B65: C2 04 00           ret         4
  00687B68: 8B 7C 24 14        mov         edi,dword ptr [esp+14h]
  00687B6C: 8B B6 80 04 00 00  mov         esi,dword ptr [esi+00000480h]
  00687B72: D9 46 10           fld         dword ptr [esi+10h]
  00687B75: D9 E1              fabs
  00687B77: D8 15 50 8B 85 00  fcom        dword ptr ds:[00858B50h]
  00687B7D: DF E0              fnstsw      ax
  00687B7F: F6 C4 41           test        ah,41h
  00687B82: 75 48              jne         00687BCC
  00687B84: D9 46 0C           fld         dword ptr [esi+0Ch]
  00687B87: D9 E1              fabs
  00687B89: D9 C9              fxch        st(1)
  00687B8B: DE D9              fcompp
  00687B8D: DF E0              fnstsw      ax
  00687B8F: F6 C4 41           test        ah,41h
  00687B92: 75 3A              jne         00687BCE
  00687B94: D9 46 10           fld         dword ptr [esi+10h]
  00687B97: 8B CF              mov         ecx,edi
  00687B99: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687B9F: DF E0              fnstsw      ax
  00687BA1: F6 C4 05           test        ah,5
  00687BA4: 7A 13              jp          00687BB9
  00687BA6: 6A 03              push        3
  00687BA8: 6A 00              push        0
  00687BAA: E8 31 4A F9 FF     call        0061C5E0
  00687BAF: 5F                 pop         edi
  00687BB0: 5E                 pop         esi
  00687BB1: 5D                 pop         ebp
  00687BB2: 5B                 pop         ebx
  00687BB3: 83 C4 28           add         esp,28h
  00687BB6: C2 04 00           ret         4
  00687BB9: 6A 05              push        5
  00687BBB: 6A 00              push        0
  00687BBD: E8 1E 4A F9 FF     call        0061C5E0
  00687BC2: 5F                 pop         edi
  00687BC3: 5E                 pop         esi
  00687BC4: 5D                 pop         ebp
  00687BC5: 5B                 pop         ebx
  00687BC6: 83 C4 28           add         esp,28h
  00687BC9: C2 04 00           ret         4
  00687BCC: DD D8              fstp        st(0)
  00687BCE: D9 46 0C           fld         dword ptr [esi+0Ch]
  00687BD1: D9 E1              fabs
  00687BD3: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687BD9: DF E0              fnstsw      ax
  00687BDB: F6 C4 41           test        ah,41h
  00687BDE: 75 2E              jne         00687C0E
  00687BE0: D9 46 0C           fld         dword ptr [esi+0Ch]
  00687BE3: 8B CF              mov         ecx,edi
  00687BE5: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  00687BEB: DF E0              fnstsw      ax
  00687BED: F6 C4 41           test        ah,41h
  00687BF0: 75 13              jne         00687C05
  00687BF2: 6A 06              push        6
  00687BF4: 6A 00              push        0
  00687BF6: E8 E5 49 F9 FF     call        0061C5E0
  00687BFB: 5F                 pop         edi
  00687BFC: 5E                 pop         esi
  00687BFD: 5D                 pop         ebp
  00687BFE: 5B                 pop         ebx
  00687BFF: 83 C4 28           add         esp,28h
  00687C02: C2 04 00           ret         4
  00687C05: 6A 04              push        4
  00687C07: 6A 00              push        0
  00687C09: E8 D2 49 F9 FF     call        0061C5E0
  00687C0E: 5F                 pop         edi
  00687C0F: 5E                 pop         esi
  00687C10: 5D                 pop         ebp
  00687C11: 5B                 pop         ebx
  00687C12: 83 C4 28           add         esp,28h
  00687C15: C2 04 00           ret         4
  00687C18: 90                 nop
  00687C19: 90                 nop
  00687C1A: 90                 nop
  00687C1B: 90                 nop
  00687C1C: 90                 nop
  00687C1D: 90                 nop
  00687C1E: 90                 nop
  00687C1F: 90                 nop
  00687C20: 83

  Summary

      456000 .text
