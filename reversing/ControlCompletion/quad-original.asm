Microsoft (R) COFF/PE Dumper Version 14.51.36257.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file D:\Grand Theft Auto San Andreas\gta-sa.exe

File Type: EXECUTABLE IMAGE

  006CDCC0: 83 EC 38           sub         esp,38h
  006CDCC3: 53                 push        ebx
  006CDCC4: 56                 push        esi
  006CDCC5: 8B F1              mov         esi,ecx
  006CDCC7: F6 46 36 F8        test        byte ptr [esi+36h],0F8h
  006CDCCB: 57                 push        edi
  006CDCCC: 0F 85 31 03 00 00  jne         006CE003
  006CDCD2: A0 50 34 8D 00     mov         al,byte ptr ds:[008D3450h]
  006CDCD7: 84 C0              test        al,al
  006CDCD9: 0F 84 24 03 00 00  je          006CE003
  006CDCDF: A1 C0 1A 87 00     mov         eax,dword ptr ds:[00871AC0h]
  006CDCE4: 8B 15 58 34 8D 00  mov         edx,dword ptr ds:[008D3458h]
  006CDCEA: 8B 0D C4 1A 87 00  mov         ecx,dword ptr ds:[00871AC4h]
  006CDCF0: 89 44 24 0C        mov         dword ptr [esp+0Ch],eax
  006CDCF4: A1 5C 34 8D 00     mov         eax,dword ptr ds:[008D345Ch]
  006CDCF9: 89 4C 24 10        mov         dword ptr [esp+10h],ecx
  006CDCFD: 8B 0D 60 34 8D 00  mov         ecx,dword ptr ds:[008D3460h]
  006CDD03: 89 54 24 14        mov         dword ptr [esp+14h],edx
  006CDD07: 8B 56 14           mov         edx,dword ptr [esi+14h]
  006CDD0A: 89 44 24 18        mov         dword ptr [esp+18h],eax
  006CDD0E: 52                 push        edx
  006CDD0F: 8D 46 50           lea         eax,[esi+50h]
  006CDD12: 89 4C 24 20        mov         dword ptr [esp+20h],ecx
  006CDD16: 50                 push        eax
  006CDD17: 8D 4C 24 28        lea         ecx,[esp+28h]
  006CDD1B: 51                 push        ecx
  006CDD1C: E8 EF EA EC FF     call        0059C810
  006CDD21: 8B 8E D4 07 00 00  mov         ecx,dword ptr [esi+000007D4h]
  006CDD27: B8 00 00 80 3F     mov         eax,3F800000h
  006CDD2C: 83 C4 0C           add         esp,0Ch
  006CDD2F: 3B C8              cmp         ecx,eax
  006CDD31: 0F 85 8D 00 00 00  jne         006CDDC4
  006CDD37: 39 86 DC 07 00 00  cmp         dword ptr [esi+000007DCh],eax
  006CDD3D: 0F 85 81 00 00 00  jne         006CDDC4
  006CDD43: D9 05 BC 1A 87 00  fld         dword ptr ds:[00871ABCh]
  006CDD49: D9 86 D8 07 00 00  fld         dword ptr [esi+000007D8h]
  006CDD4F: D8 1D 24 86 85 00  fcomp       dword ptr ds:[00858624h]
  006CDD55: DF E0              fnstsw      ax
  006CDD57: F6 C4 05           test        ah,5
  006CDD5A: 7B 13              jnp         006CDD6F
  006CDD5C: D9 86 E0 07 00 00  fld         dword ptr [esi+000007E0h]
  006CDD62: D8 1D 24 86 85 00  fcomp       dword ptr ds:[00858624h]
  006CDD68: DF E0              fnstsw      ax
  006CDD6A: F6 C4 05           test        ah,5
  006CDD6D: 7A 44              jp          006CDDB3
  006CDD6F: 8B 4E 14           mov         ecx,dword ptr [esi+14h]
  006CDD72: D9 41 18           fld         dword ptr [ecx+18h]
  006CDD75: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  006CDD7B: DF E0              fnstsw      ax
  006CDD7D: F6 C4 41           test        ah,41h
  006CDD80: 75 31              jne         006CDDB3
  006CDD82: 8B 96 88 09 00 00  mov         edx,dword ptr [esi+00000988h]
  006CDD88: D9 42 2C           fld         dword ptr [edx+2Ch]
  006CDD8B: D8 61 18           fsub        dword ptr [ecx+18h]
  006CDD8E: D9 E1              fabs
  006CDD90: D8 0D 84 8C 85 00  fmul        dword ptr ds:[00858C84h]
  006CDD96: D9 05 A8 8C 85 00  fld         dword ptr ds:[00858CA8h]
  006CDD9C: D8 D9              fcomp       st(1)
  006CDD9E: DF E0              fnstsw      ax
  006CDDA0: F6 C4 05           test        ah,5
  006CDDA3: 7A 08              jp          006CDDAD
  006CDDA5: DD D8              fstp        st(0)
  006CDDA7: D9 05 A8 8C 85 00  fld         dword ptr ds:[00858CA8h]
  006CDDAD: D8 6C 24 14        fsubr       dword ptr [esp+14h]
  006CDDB1: EB 79              jmp         006CDE2C
  006CDDB3: A1 C8 1A 87 00     mov         eax,dword ptr ds:[00871AC8h]
  006CDDB8: D9 05 BC 1A 87 00  fld         dword ptr ds:[00871ABCh]
  006CDDBE: 89 44 24 10        mov         dword ptr [esp+10h],eax
  006CDDC2: EB 68              jmp         006CDE2C
  006CDDC4: 39 86 F8 07 00 00  cmp         dword ptr [esi+000007F8h],eax
  006CDDCA: 75 58              jne         006CDE24
  006CDDCC: 39 86 00 08 00 00  cmp         dword ptr [esi+00000800h],eax
  006CDDD2: 75 50              jne         006CDE24
  006CDDD4: 8B 4E 14           mov         ecx,dword ptr [esi+14h]
  006CDDD7: D9 05 BC 1A 87 00  fld         dword ptr ds:[00871ABCh]
  006CDDDD: D9 41 18           fld         dword ptr [ecx+18h]
  006CDDE0: D8 1D 50 8B 85 00  fcomp       dword ptr ds:[00858B50h]
  006CDDE6: DF E0              fnstsw      ax
  006CDDE8: F6 C4 05           test        ah,5
  006CDDEB: 7A 3B              jp          006CDE28
  006CDDED: 8B 96 88 09 00 00  mov         edx,dword ptr [esi+00000988h]
  006CDDF3: D9 42 30           fld         dword ptr [edx+30h]
  006CDDF6: D8 61 18           fsub        dword ptr [ecx+18h]
  006CDDF9: D9 E1              fabs
  006CDDFB: D8 0D 24 8C 85 00  fmul        dword ptr ds:[00858C24h]
  006CDE01: D9 05 1C 8B 85 00  fld         dword ptr ds:[00858B1Ch]
  006CDE07: D8 D9              fcomp       st(1)
  006CDE09: DF E0              fnstsw      ax
  006CDE0B: F6 C4 05           test        ah,5
  006CDE0E: 7A 08              jp          006CDE18
  006CDE10: DD D8              fstp        st(0)
  006CDE12: D9 05 1C 8B 85 00  fld         dword ptr ds:[00858B1Ch]
  006CDE18: D8 05 20 8C 85 00  fadd        dword ptr ds:[00858C20h]
  006CDE1E: D8 4C 24 14        fmul        dword ptr [esp+14h]
  006CDE22: EB 08              jmp         006CDE2C
  006CDE24: D9 44 24 0C        fld         dword ptr [esp+0Ch]
  006CDE28: D9 44 24 14        fld         dword ptr [esp+14h]
  006CDE2C: D9 44 24 20        fld         dword ptr [esp+20h]
  006CDE30: D8 4C 24 20        fmul        dword ptr [esp+20h]
  006CDE34: D8 CA              fmul        st,st(2)
  006CDE36: D8 05 24 86 85 00  fadd        dword ptr ds:[00858624h]
  006CDE3C: D8 F9              fdivr       st,st(1)
  006CDE3E: D9 5C 24 14        fstp        dword ptr [esp+14h]
  006CDE42: DD D8              fstp        st(0)
  006CDE44: DD D8              fstp        st(0)
  006CDE46: D9 44 24 24        fld         dword ptr [esp+24h]
  006CDE4A: D8 4C 24 24        fmul        dword ptr [esp+24h]
  006CDE4E: D8 4C 24 10        fmul        dword ptr [esp+10h]
  006CDE52: D8 05 24 86 85 00  fadd        dword ptr ds:[00858624h]
  006CDE58: D8 7C 24 18        fdivr       dword ptr [esp+18h]
  006CDE5C: D9 5C 24 18        fstp        dword ptr [esp+18h]
  006CDE60: D9 44 24 14        fld         dword ptr [esp+14h]
  006CDE64: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  006CDE6A: E8 C1 42 15 00     call        00822130
  006CDE6F: D9 5C 24 14        fstp        dword ptr [esp+14h]
  006CDE73: D9 44 24 18        fld         dword ptr [esp+18h]
  006CDE77: D9 05 5C CB B7 00  fld         dword ptr ds:[00B7CB5Ch]
  006CDE7D: E8 AE 42 15 00     call        00822130
  006CDE82: D9 44 24 20        fld         dword ptr [esp+20h]
  006CDE86: D8 4C 24 14        fmul        dword ptr [esp+14h]
  006CDE8A: 8B 7E 14           mov         edi,dword ptr [esi+14h]
  006CDE8D: 8B 86 90 00 00 00  mov         eax,dword ptr [esi+00000090h]
  006CDE93: 89 44 24 0C        mov         dword ptr [esp+0Ch],eax
  006CDE97: D8 64 24 20        fsub        dword ptr [esp+20h]
  006CDE9B: 8D 9E A4 00 00 00  lea         ebx,[esi+000000A4h]
  006CDEA1: 53                 push        ebx
  006CDEA2: 8D 4C 24 3C        lea         ecx,[esp+3Ch]
  006CDEA6: D9 5C 24 14        fstp        dword ptr [esp+14h]
  006CDEAA: 57                 push        edi
  006CDEAB: 51                 push        ecx
  006CDEAC: D8 4C 24 30        fmul        dword ptr [esp+30h]
  006CDEB0: D8 64 24 30        fsub        dword ptr [esp+30h]
  006CDEB4: D9 47 20           fld         dword ptr [edi+20h]
  006CDEB7: D8 0D 1C 8C 85 00  fmul        dword ptr ds:[00858C1Ch]
  006CDEBD: D9 47 24           fld         dword ptr [edi+24h]
  006CDEC0: D8 0D 1C 8C 85 00  fmul        dword ptr ds:[00858C1Ch]
  006CDEC6: D9 47 28           fld         dword ptr [edi+28h]
  006CDEC9: D8 0D 1C 8C 85 00  fmul        dword ptr ds:[00858C1Ch]
  006CDECF: D9 5C 24 40        fstp        dword ptr [esp+40h]
  006CDED3: D9 C9              fxch        st(1)
  006CDED5: D8 CA              fmul        st,st(2)
  006CDED7: D9 5C 24 20        fstp        dword ptr [esp+20h]
  006CDEDB: D8 C9              fmul        st,st(1)
  006CDEDD: D9 44 24 40        fld         dword ptr [esp+40h]
  006CDEE1: D8 CA              fmul        st,st(2)
  006CDEE3: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006CDEE7: D9 44 24 20        fld         dword ptr [esp+20h]
  006CDEEB: D8 4C 24 18        fmul        dword ptr [esp+18h]
  006CDEEF: D9 5C 24 38        fstp        dword ptr [esp+38h]
  006CDEF3: D8 4C 24 18        fmul        dword ptr [esp+18h]
  006CDEF7: D9 5C 24 3C        fstp        dword ptr [esp+3Ch]
  006CDEFB: DD D8              fstp        st(0)
  006CDEFD: D9 44 24 28        fld         dword ptr [esp+28h]
  006CDF01: D8 4C 24 18        fmul        dword ptr [esp+18h]
  006CDF05: D9 5C 24 40        fstp        dword ptr [esp+40h]
  006CDF09: E8 82 E8 EC FF     call        0059C790
  006CDF0E: D9 00              fld         dword ptr [eax]
  006CDF10: D8 07              fadd        dword ptr [edi]
  006CDF12: 8B D4              mov         edx,esp
  006CDF14: D9 5C 24 20        fstp        dword ptr [esp+20h]
  006CDF18: D9 40 04           fld         dword ptr [eax+4]
  006CDF1B: D8 47 04           fadd        dword ptr [edi+4]
  006CDF1E: D9 5C 24 24        fstp        dword ptr [esp+24h]
  006CDF22: D9 40 08           fld         dword ptr [eax+8]
  006CDF25: D8 47 08           fadd        dword ptr [edi+8]
  006CDF28: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006CDF2C: 8B 44 24 20        mov         eax,dword ptr [esp+20h]
  006CDF30: 8B 4C 24 24        mov         ecx,dword ptr [esp+24h]
  006CDF34: 89 02              mov         dword ptr [edx],eax
  006CDF36: 8B 44 24 28        mov         eax,dword ptr [esp+28h]
  006CDF3A: 89 4A 04           mov         dword ptr [edx+4],ecx
  006CDF3D: 89 42 08           mov         dword ptr [edx+8],eax
  006CDF40: 8B 54 24 38        mov         edx,dword ptr [esp+38h]
  006CDF44: 8B 44 24 3C        mov         eax,dword ptr [esp+3Ch]
  006CDF48: 83 EC 0C           sub         esp,0Ch
  006CDF4B: 8B CC              mov         ecx,esp
  006CDF4D: 89 11              mov         dword ptr [ecx],edx
  006CDF4F: 8B 54 24 4C        mov         edx,dword ptr [esp+4Ch]
  006CDF53: 89 41 04           mov         dword ptr [ecx+4],eax
  006CDF56: 89 51 08           mov         dword ptr [ecx+8],edx
  006CDF59: 8B CE              mov         ecx,esi
  006CDF5B: E8 F0 4A E7 FF     call        00542A50
  006CDF60: D9 44 24 10        fld         dword ptr [esp+10h]
  006CDF64: 8B 7E 14           mov         edi,dword ptr [esi+14h]
  006CDF67: D8 4F 20           fmul        dword ptr [edi+20h]
  006CDF6A: 8B 86 90 00 00 00  mov         eax,dword ptr [esi+00000090h]
  006CDF70: D9 44 24 10        fld         dword ptr [esp+10h]
  006CDF74: 53                 push        ebx
  006CDF75: D8 4F 24           fmul        dword ptr [edi+24h]
  006CDF78: 8D 4C 24 3C        lea         ecx,[esp+3Ch]
  006CDF7C: D9 44 24 14        fld         dword ptr [esp+14h]
  006CDF80: 89 44 24 14        mov         dword ptr [esp+14h],eax
  006CDF84: D8 4F 28           fmul        dword ptr [edi+28h]
  006CDF87: 57                 push        edi
  006CDF88: 51                 push        ecx
  006CDF89: D9 5C 24 40        fstp        dword ptr [esp+40h]
  006CDF8D: D9 C9              fxch        st(1)
  006CDF8F: D8 4C 24 1C        fmul        dword ptr [esp+1Ch]
  006CDF93: D9 5C 24 20        fstp        dword ptr [esp+20h]
  006CDF97: D8 4C 24 1C        fmul        dword ptr [esp+1Ch]
  006CDF9B: D9 5C 24 24        fstp        dword ptr [esp+24h]
  006CDF9F: D9 44 24 40        fld         dword ptr [esp+40h]
  006CDFA3: D8 4C 24 1C        fmul        dword ptr [esp+1Ch]
  006CDFA7: D9 5C 24 28        fstp        dword ptr [esp+28h]
  006CDFAB: E8 E0 E7 EC FF     call        0059C790
  006CDFB0: D9 47 10           fld         dword ptr [edi+10h]
  006CDFB3: D8 00              fadd        dword ptr [eax]
  006CDFB5: 8B D4              mov         edx,esp
  006CDFB7: 83 EC 0C           sub         esp,0Ch
  006CDFBA: D9 5C 24 44        fstp        dword ptr [esp+44h]
  006CDFBE: D9 40 04           fld         dword ptr [eax+4]
  006CDFC1: D8 47 14           fadd        dword ptr [edi+14h]
  006CDFC4: D9 5C 24 48        fstp        dword ptr [esp+48h]
  006CDFC8: D9 40 08           fld         dword ptr [eax+8]
  006CDFCB: 8B 44 24 44        mov         eax,dword ptr [esp+44h]
  006CDFCF: D8 47 18           fadd        dword ptr [edi+18h]
  006CDFD2: 8B 4C 24 48        mov         ecx,dword ptr [esp+48h]
  006CDFD6: 89 02              mov         dword ptr [edx],eax
  006CDFD8: 89 4A 04           mov         dword ptr [edx+4],ecx
  006CDFDB: D9 5C 24 4C        fstp        dword ptr [esp+4Ch]
  006CDFDF: 8B 44 24 4C        mov         eax,dword ptr [esp+4Ch]
  006CDFE3: 89 42 08           mov         dword ptr [edx+8],eax
  006CDFE6: 8B 54 24 2C        mov         edx,dword ptr [esp+2Ch]
  006CDFEA: 8B 44 24 30        mov         eax,dword ptr [esp+30h]
  006CDFEE: 8B CC              mov         ecx,esp
  006CDFF0: 89 11              mov         dword ptr [ecx],edx
  006CDFF2: 8B 54 24 34        mov         edx,dword ptr [esp+34h]
  006CDFF6: 89 41 04           mov         dword ptr [ecx+4],eax
  006CDFF9: 89 51 08           mov         dword ptr [ecx+8],edx
  006CDFFC: 8B CE              mov         ecx,esi
  006CDFFE: E8 4D 4A E7 FF     call        00542A50
  006CE003: 8B CE              mov         ecx,esi
  006CE005: E8 76 38 FE FF     call        006B1880
  006CE00A: 5F                 pop         edi
  006CE00B: 5E                 pop         esi
  006CE00C: 5B                 pop         ebx
  006CE00D: 83 C4 38           add         esp,38h
  006CE010: C3                 ret
  006CE011: 90                 nop
  006CE012: 90                 nop
  006CE013: 90                 nop
  006CE014: 90                 nop
  006CE015: 90                 nop
  006CE016: 90                 nop
  006CE017: 90                 nop
  006CE018: 90                 nop
  006CE019: 90                 nop
  006CE01A: 90                 nop
  006CE01B: 90                 nop
  006CE01C: 90                 nop
  006CE01D: 90                 nop
  006CE01E: 90                 nop
  006CE01F: 90                 nop
  006CE020: 51                 push        ecx

  Summary

      456000 .text
