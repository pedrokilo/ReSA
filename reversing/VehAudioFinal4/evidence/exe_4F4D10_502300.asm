004F4D10: push ebx
004F4D11: push ebp
004F4D12: mov ebp,dword ptr [esp+0Ch]
004F4D16: or ebx,0FFFFFFFFh
004F4D19: xor ecx,ecx
004F4D1B: jmp 004F4D20
004F4D1D: lea ecx,[ecx]
004F4D20: movsx eax,cx
004F4D23: shl eax,2
004F4D26: cmp word ptr [eax+00B6B9A0h],bp
 00
004F4D2D: je 004F4D4E
004F4D2F: cmp word ptr [eax+00B6B9A2h],0
004F4D37: jg 004F4D3B
004F4D39: mov ebx,ecx
004F4D3B: inc ecx
004F4D3C: cmp cx,0Ah
004F4D40: jl 004F4D20
004F4D42: cmp bx,0FFFFh
004F4D46: jne 004F4D5F
004F4D48: pop ebp
004F4D49: or ax,bx
004F4D4C: pop ebx
004F4D4D: ret
004F4D4E: movsx eax,cx
004F4D51: inc word ptr [eax*4+00B6B9A2h]
004F4D59: pop ebp
004F4D5A: add eax,7
004F4D5D: pop ebx
004F4D5E: ret
004F4D5F: push esi
004F4D60: push edi
004F4D61: movsx edi,word ptr ds:[00B6B998h]
 00
004F4D68: xor esi,esi
004F4D6A: lea ebx,[ebx+00000000h]
004F4D70: movsx eax,si
004F4D73: add eax,edi
004F4D75: cdq
004F4D76: mov ecx,0Ah
004F4D7B: idiv eax,ecx
004F4D7D: mov ecx,edx
004F4D7F: movsx edx,cx
004F4D82: cmp word ptr [edx*4+00B6B9A2h],0
004F4D8B: jle 004F4D96
004F4D8D: inc esi
004F4D8E: cmp si,0Ah
004F4D92: jl 004F4D70
004F4D94: jmp 004F4DB3
004F4D96: movsx eax,cx
004F4D99: inc word ptr [eax*4+00B6B9A2h]
004F4DA1: inc eax
004F4DA2: cdq
004F4DA3: mov esi,0Ah
004F4DA8: idiv eax,esi
004F4DAA: mov ebx,ecx
004F4DAC: mov word ptr ds:[00B6B998h],dx
 00
004F4DB3: push 1
004F4DB5: lea esi,[ebx+7]
004F4DB8: push esi
004F4DB9: mov ecx,0B62CB0h
004F4DBE: call 004EFC60
004F4DC3: push esi
004F4DC4: push ebp
004F4DC5: mov ecx,0B5F8B8h
004F4DCA: call 004D88A0
004F4DCF: movsx eax,bx
004F4DD2: pop edi
004F4DD3: pop esi
004F4DD4: mov word ptr [eax*4+00B6B9A0h],bp
004F4DDC: pop ebp
004F4DDD: mov word ptr [eax*4+00B6B9A2h],1
004F4DE7: add eax,7
004F4DEA: pop ebx
004F4DEB: ret
004F4DF0: mov eax,dword ptr [esp+4]
004F4DF4: add eax,0FFFFFFF9h
004F4DF7: test ax,ax
004F4DFA: jl 004F4E28
004F4DFC: cmp ax,0Ah
004F4E00: jge 004F4E28
004F4E02: movsx eax,ax
004F4E05: xor ecx,ecx
004F4E07: mov cx,word ptr [eax*4+00B6B9A2h]
004F4E0F: test cx,cx
004F4E12: jle 004F4E1E
004F4E14: dec ecx
004F4E15: mov word ptr [eax*4+00B6B9A2h],cx
004F4E1D: ret
004F4E1E: mov word ptr [eax*4+00B6B9A2h],0
004F4E28: ret
004F4E30: mov eax,dword ptr [esp+4]
004F4E34: add eax,0FFFFFFF9h
004F4E37: test ax,ax
004F4E3A: jl 004F4E56
004F4E3C: cmp ax,0Ah
004F4E40: jge 004F4E56
004F4E42: movsx eax,ax
004F4E45: mov cx,word ptr [eax*4+00B6B9A0h]
004F4E4D: cmp cx,word ptr [esp+8]
004F4E52: sete al
004F4E55: ret
004F4E56: xor al,al
004F4E58: ret
004F4E60: push esi
004F4E61: mov esi,dword ptr [esp+8]
004F4E65: push esi
004F4E66: call 004F4D10
004F4E6B: add esp,4
004F4E6E: cmp ax,0FFFFh
004F4E72: jne 004F4EB2
004F4E74: mov cx,word ptr ds:[00B6B9A2h]
 00
004F4E7B: xor edx,edx
004F4E7D: mov eax,1
004F4E82: push edi
004F4E83: movsx edi,ax
004F4E86: cmp word ptr [edi*4+00B6B9A2h],cx
004F4E8E: jl 004F4E99
004F4E90: inc eax
004F4E91: cmp ax,0Ah
004F4E95: jl 004F4E83
004F4E97: jmp 004F4E9B
004F4E99: mov edx,eax
004F4E9B: movsx eax,dx
004F4E9E: push esi
004F4E9F: mov word ptr [eax*4+00B6B9A2h],0
004F4EA9: call 004F4D10
004F4EAE: add esp,4
004F4EB1: pop edi
004F4EB2: pop esi
004F4EB3: ret
004F4EC0: ret
004F4ED0: mov eax,dword ptr ds:[00B6B99Ch]
004F4ED5: ret
004F4EE0: mov byte ptr ds:[00B6B994h],1
 01
004F4EE7: ret
004F4EF0: mov byte ptr ds:[00B6B994h],0
 00
004F4EF7: ret
004F4F00: mov al,byte ptr [ecx+000000A4h]
004F4F06: test al,al
004F4F08: je 004F4F34
004F4F0A: mov eax,dword ptr [ecx+4]
004F4F0D: test eax,eax
004F4F0F: je 004F4F34
004F4F11: movsx eax,byte ptr [ecx+0000009Ch]
 00
004F4F18: cmp eax,26h
004F4F1B: ja 004F4F34
004F4F1D: movzx eax,byte ptr [eax+004F4F48h]
 00
004F4F24: jmp dword ptr [eax*4+004F4F3Ch]
 00
004F4F2B: xor ax,ax
004F4F2E: ret
004F4F2F: mov ax,1
004F4F33: ret
004F4F34: mov ax,2
004F4F38: ret
004F4F39: lea ecx,[ecx]
004F4F3C: sub ecx,dword ptr [edi+4Fh]
004F4F3F: add byte ptr [edi],ch
004F4F41: dec edi
004F4F42: dec edi
004F4F43: add byte ptr [edi+ecx*2],dh
004F4F46: dec edi
004F4F47: add byte ptr [eax],al
004F4F49: add byte ptr [eax],al
004F4F4B: add al,byte ptr [eax]
004F4F4D: add byte ptr [edx],al
004F4F4F: add al,byte ptr [edx]
004F4F51: add byte ptr [eax],al
004F4F53: add al,byte ptr [edx]
004F4F55: add al,byte ptr [eax]
004F4F57: add al,byte ptr [edx]
004F4F59: add al,byte ptr [edx]
004F4F5B: add al,byte ptr [edx]
004F4F5D: add al,byte ptr [eax]
004F4F5F: add byte ptr [edx],al
004F4F61: add dword ptr [edx],eax
004F4F63: add al,byte ptr [edx]
004F4F65: add dword ptr [ecx],eax
004F4F67: add byte ptr [edx],al
004F4F69: add al,byte ptr [edx]
004F4F6B: add al,byte ptr [edx]
004F4F6D: add al,byte ptr [eax]
004F4F70: xor eax,eax
004F4F72: mov ax,word ptr [ecx+000000DEh]
 00
004F4F79: push 28h
004F4F7B: mov ecx,0B5F8B8h
004F4F80: push eax
004F4F81: call 004D88C0
004F4F86: test al,al
004F4F88: je 004F4FB8
004F4F8A: mov ecx,dword ptr [esp+4]
004F4F8E: cmp word ptr [ecx+22h],0
004F4F93: jg 004F4FB8
004F4F95: mov al,byte ptr [ecx+39h]
004F4F98: test al,al
004F4F9A: jne 004F4FB8
004F4F9C: fld dword ptr [ecx+40h]
004F4F9F: fcomp dword ptr ds:[00858C80h]
004F4FA5: fnstsw ax
004F4FA7: test ah,41h
004F4FAA: je 004F4FB8
004F4FAC: mov al,byte ptr [ecx+46h]
004F4FAF: test al,al
004F4FB1: je 004F4FB8
004F4FB3: xor al,al
004F4FB5: ret 4
004F4FB8: mov al,1
004F4FBA: ret 4
004F4FC0: mov ecx,dword ptr [esp+4]
004F4FC4: fld dword ptr [ecx+40h]
004F4FC7: fcomp dword ptr ds:[00858C80h]
004F4FCD: fnstsw ax
004F4FCF: test ah,41h
004F4FD2: je 004F4FE0
004F4FD4: mov al,byte ptr [ecx+46h]
004F4FD7: test al,al
004F4FD9: je 004F4FE0
004F4FDB: xor al,al
004F4FDD: ret 4
004F4FE0: mov al,1
004F4FE2: ret 4
004F4FF0: mov al,byte ptr [ecx+000000B0h]
004F4FF6: test al,al
004F4FF8: jne 004F5013
004F4FFA: mov eax,dword ptr [esp+4]
004F4FFE: fld dword ptr [eax+1Ch]
004F5001: fcomp dword ptr ds:[008CBCD0h]
004F5007: fnstsw ax
004F5009: test ah,5
004F500C: jp 004F5013
004F500E: mov al,1
004F5010: ret 4
004F5013: xor al,al
004F5015: ret 4
004F5020: mov eax,dword ptr [ecx+000000D0h]
004F5026: cmp eax,dword ptr ds:[00B7CB84h]
004F502C: setb al
004F502F: ret
004F5030: mov edx,dword ptr ds:[00B7CB84h]
004F5036: mov eax,dword ptr [esp+4]
004F503A: add edx,eax
004F503C: mov dword ptr [ecx+000000D0h],edx
004F5042: ret 4
004F5050: mov eax,dword ptr [ecx+000000D4h]
004F5056: cmp eax,dword ptr ds:[00B7CB84h]
004F505C: setb al
004F505F: ret
004F5060: mov edx,dword ptr ds:[00B7CB84h]
004F5066: mov eax,dword ptr [esp+4]
004F506A: add edx,eax
004F506C: mov dword ptr [ecx+000000D4h],edx
004F5072: ret 4
004F5080: cmp byte ptr ds:[00A43088h],1
 01
004F5087: push esi
004F5088: push edi
004F5089: jne 004F5183
004F508F: mov edi,dword ptr [esp+0Ch]
004F5093: fld dword ptr ds:[00858624h]
004F5099: mov esi,dword ptr [edi+10h]
004F509C: fcomp dword ptr [esi+0000049Ch]
004F50A2: fnstsw ax
004F50A4: test ah,5
004F50A7: jnp 004F50C8
004F50A9: fld dword ptr [esi+0000049Ch]
004F50AF: fld dword ptr ds:[00858B50h]
004F50B5: fcomp st(1)
004F50B7: fnstsw ax
004F50B9: fstp st(0)
004F50BB: test ah,41h
004F50BE: jne 004F50C8
004F50C0: fld dword ptr ds:[00858B50h]
004F50C6: jmp 004F50E9
004F50C8: fld dword ptr ds:[00858624h]
004F50CE: fcomp dword ptr [esi+0000049Ch]
004F50D4: fnstsw ax
004F50D6: test ah,5
004F50D9: jp 004F50E3
004F50DB: fld dword ptr ds:[00858624h]
004F50E1: jmp 004F50E9
004F50E3: fld dword ptr [esi+0000049Ch]
004F50E9: fmul dword ptr ds:[00859AACh]
004F50EF: call 00821B40
004F50F4: fld dword ptr ds:[00858624h]
004F50FA: mov word ptr [edi+20h],ax
004F50FE: fcomp dword ptr [esi+000004A0h]
004F5104: fnstsw ax
004F5106: test ah,5
004F5109: jnp 004F513C
004F510B: fld dword ptr [esi+000004A0h]
004F5111: fld dword ptr ds:[00858B50h]
004F5117: fcomp st(1)
004F5119: fnstsw ax
004F511B: fstp st(0)
004F511D: test ah,41h
004F5120: jne 004F513C
004F5122: fld dword ptr ds:[00858B50h]
004F5128: fmul dword ptr ds:[00859AACh]
004F512E: call 00821B40
004F5133: mov word ptr [edi+22h],ax
004F5137: pop edi
004F5138: pop esi
004F5139: ret 4
004F513C: fld dword ptr ds:[00858624h]
004F5142: fcomp dword ptr [esi+000004A0h]
004F5148: fnstsw ax
004F514A: test ah,5
004F514D: jp 004F5169
004F514F: fld dword ptr ds:[00858624h]
004F5155: fmul dword ptr ds:[00859AACh]
004F515B: call 00821B40
004F5160: mov word ptr [edi+22h],ax
004F5164: pop edi
004F5165: pop esi
004F5166: ret 4
004F5169: fld dword ptr [esi+000004A0h]
004F516F: fmul dword ptr ds:[00859AACh]
004F5175: call 00821B40
004F517A: mov word ptr [edi+22h],ax
004F517E: pop edi
004F517F: pop esi
004F5180: ret 4
004F5183: mov eax,dword ptr ds:[00B6B990h]
004F5188: push ebx
004F5189: xor ebx,ebx
004F518B: cmp eax,ebx
004F518D: je 004F51D8
004F518F: cmp byte ptr [ecx+000000A7h],bl
004F5195: jne 004F51D8
004F5197: cmp dword ptr [eax+00000530h],3Fh
 3F
004F519E: je 004F51D8
004F51A0: mov ecx,eax
004F51A2: call 005E0170
004F51A7: test al,al
004F51A9: je 004F51D8
004F51AB: mov ecx,dword ptr ds:[00B6B990h]
004F51B1: call 00609560
004F51B6: mov esi,eax
004F51B8: mov ecx,esi
004F51BA: call 005403F0
004F51BF: mov edi,dword ptr [esp+10h]
004F51C3: mov ecx,esi
004F51C5: mov word ptr [edi+20h],ax
004F51C9: call 00540080
004F51CE: pop ebx
004F51CF: mov word ptr [edi+22h],ax
004F51D3: pop edi
004F51D4: pop esi
004F51D5: ret 4
004F51D8: mov eax,dword ptr [esp+10h]
004F51DC: mov word ptr [eax+20h],bx
004F51E0: mov word ptr [eax+22h],bx
004F51E4: pop ebx
004F51E5: pop edi
004F51E6: pop esi
004F51E7: ret 4
004F51F0: push esi
004F51F1: mov esi,ecx
004F51F3: mov eax,dword ptr [esi+4]
004F51F6: cmp word ptr [eax+22h],1C9h
004F51FC: jne 004F520E
004F51FE: fld dword ptr ds:[008CBC00h]
004F5204: pop esi
004F5205: fsub dword ptr ds:[00858CA4h]
004F520B: ret 8
004F520E: fld dword ptr [esp+8]
004F5212: fdiv dword ptr ds:[008CBBF0h]
004F5218: fcom dword ptr ds:[00858624h]
004F521E: fnstsw ax
004F5220: test ah,5
004F5223: jp 004F5242
004F5225: fld st(0)
004F5227: fld dword ptr ds:[00858B50h]
004F522D: fcomp st(1)
004F522F: fnstsw ax
004F5231: fstp st(0)
004F5233: test ah,41h
004F5236: jne 004F5242
004F5238: fstp st(0)
004F523A: fld dword ptr ds:[00858B50h]
004F5240: jmp 004F5257
004F5242: fcom dword ptr ds:[00858624h]
004F5248: fnstsw ax
004F524A: test ah,5
004F524D: jnp 004F5257
004F524F: fstp st(0)
004F5251: fld dword ptr ds:[00858624h]
004F5257: fld dword ptr ds:[00B6BA24h]
004F525D: mov al,byte ptr [esi+000000A9h]
004F5263: cmp al,2
004F5265: fmul st,st(1)
004F5267: fadd dword ptr ds:[008CBC00h]
004F526D: fstp dword ptr [esp+8]
004F5271: fstp st(0)
004F5273: jne 004F529F
004F5275: mov ecx,dword ptr [esp+0Ch]
004F5279: push 8CC0C4h
004F527E: push 5
004F5280: push ecx
004F5281: call 004D9D90
004F5286: fstp dword ptr [esp+18h]
004F528A: mov edx,dword ptr [esp+18h]
004F528E: push edx
004F528F: call 004D9E50
004F5294: fmul dword ptr ds:[00858BA4h]
004F529A: add esp,10h
004F529D: jmp 004F52CB
004F529F: fld dword ptr [esp+0Ch]
004F52A3: fcomp dword ptr ds:[00862CD0h]
004F52A9: fnstsw ax
004F52AB: test ah,41h
004F52AE: jne 004F52B8
004F52B0: fld dword ptr ds:[00858B50h]
004F52B6: jmp 004F52CB
004F52B8: mov eax,dword ptr [esp+0Ch]
004F52BC: push eax
004F52BD: call 004D9E50
004F52C2: fmul dword ptr ds:[0085862Ch]
004F52C8: add esp,4
004F52CB: mov eax,dword ptr [esi+4]
004F52CE: test byte ptr [eax+0000042Bh],40h
 40
004F52D5: je 004F52E5
004F52D7: fld dword ptr [esp+8]
004F52DB: fsub dword ptr ds:[008CBC44h]
004F52E1: fstp dword ptr [esp+8]
004F52E5: mov ecx,dword ptr [eax+000004C8h]
004F52EB: test ecx,ecx
004F52ED: je 004F52FD
004F52EF: fld dword ptr ds:[008CBC40h]
004F52F5: fadd dword ptr [esp+8]
004F52F9: fstp dword ptr [esp+8]
004F52FD: fadd dword ptr [esi+000000A0h]
004F5303: pop esi
004F5304: fadd dword ptr [esp+4]
004F5308: ret 8
004F5310: push esi
004F5311: mov esi,ecx
004F5313: mov eax,dword ptr [esi+4]
004F5316: cmp word ptr [eax+22h],1C9h
004F531C: jne 004F5328
004F531E: fld dword ptr ds:[00858624h]
004F5324: pop esi
004F5325: ret 8
004F5328: fld dword ptr [esp+8]
004F532C: fdiv dword ptr ds:[008CBBF0h]
004F5332: fcom dword ptr ds:[00858624h]
004F5338: fnstsw ax
004F533A: test ah,5
004F533D: jp 004F535C
004F533F: fld st(0)
004F5341: fld dword ptr ds:[00858B50h]
004F5347: fcomp st(1)
004F5349: fnstsw ax
004F534B: fstp st(0)
004F534D: test ah,41h
004F5350: jne 004F535C
004F5352: fstp st(0)
004F5354: fld dword ptr ds:[00858B50h]
004F535A: jmp 004F5371
004F535C: fcom dword ptr ds:[00858624h]
004F5362: fnstsw ax
004F5364: test ah,5
004F5367: jnp 004F5371
004F5369: fstp st(0)
004F536B: fld dword ptr ds:[00858624h]
004F5371: fld dword ptr ds:[00B6BA20h]
004F5377: mov al,byte ptr [esi+000000A9h]
004F537D: cmp al,2
004F537F: fmul st,st(1)
004F5381: fadd dword ptr ds:[008CBBF8h]
004F5387: fstp dword ptr [esp+8]
004F538B: fstp st(0)
004F538D: jne 004F53A5
004F538F: mov ecx,dword ptr [esp+0Ch]
004F5393: push 8CC0ECh
004F5398: push 5
004F539A: push ecx
004F539B: call 004D9D90
004F53A0: add esp,0Ch
004F53A3: jmp 004F53AB
004F53A5: fld dword ptr ds:[00858624h]
004F53AB: mov edx,dword ptr [esi+4]
004F53AE: test byte ptr [edx+0000042Bh],40h
 40
004F53B5: je 004F53C5
004F53B7: fld dword ptr ds:[008CBC48h]
004F53BD: fmul dword ptr [esp+8]
004F53C1: fstp dword ptr [esp+8]
004F53C5: fmul dword ptr [esp+8]
004F53C9: pop esi
004F53CA: ret 8
004F53D0: fld dword ptr [esp+4]
004F53D4: push esi
004F53D5: fsub dword ptr ds:[008CBBF4h]
004F53DB: mov esi,ecx
004F53DD: fld dword ptr ds:[00858624h]
004F53E3: fsub dword ptr ds:[008CBBF4h]
004F53E9: fdivp st(1),st
004F53EB: fcom dword ptr ds:[00858624h]
004F53F1: fnstsw ax
004F53F3: test ah,5
004F53F6: jp 004F5415
004F53F8: fld st(0)
004F53FA: fld dword ptr ds:[00858B50h]
004F5400: fcomp st(1)
004F5402: fnstsw ax
004F5404: fstp st(0)
004F5406: test ah,41h
004F5409: jne 004F5415
004F540B: fstp st(0)
004F540D: fld dword ptr ds:[00858B50h]
004F5413: jmp 004F542A
004F5415: fcom dword ptr ds:[00858624h]
004F541B: fnstsw ax
004F541D: test ah,5
004F5420: jnp 004F542A
004F5422: fstp st(0)
004F5424: fld dword ptr ds:[00858624h]
004F542A: fld dword ptr ds:[00B6BA2Ch]
004F5430: mov al,byte ptr [esi+000000A9h]
004F5436: cmp al,2
004F5438: fmul st,st(1)
004F543A: fadd dword ptr ds:[008CBC14h]
004F5440: fstp dword ptr [esp+8]
004F5444: fstp st(0)
004F5446: jne 004F548B
004F5448: fld dword ptr [esp+0Ch]
004F544C: fcomp qword ptr ds:[00862CD8h]
004F5452: fnstsw ax
004F5454: test ah,41h
004F5457: jne 004F5461
004F5459: fld dword ptr ds:[00858B50h]
004F545F: jmp 004F54A7
004F5461: mov eax,dword ptr [esp+0Ch]
004F5465: push 8CC114h
004F546A: push 5
004F546C: push eax
004F546D: call 004D9D90
004F5472: fstp dword ptr [esp+18h]
004F5476: mov ecx,dword ptr [esp+18h]
004F547A: push ecx
004F547B: call 004D9E50
004F5480: fmul dword ptr ds:[00858BA4h]
004F5486: add esp,10h
004F5489: jmp 004F54A7
004F548B: fld dword ptr ds:[00858624h]
004F5491: push ecx
004F5492: fsub dword ptr [esp+10h]
004F5496: fstp dword ptr [esp]
004F5499: call 004D9E50
004F549E: fmul dword ptr ds:[0085862Ch]
004F54A4: add esp,4
004F54A7: mov eax,dword ptr [esi+4]
004F54AA: test byte ptr [eax+0000042Bh],40h
 40
004F54B1: je 004F54C1
004F54B3: fld dword ptr [esp+8]
004F54B7: fsub dword ptr ds:[008CBC44h]
004F54BD: fstp dword ptr [esp+8]
004F54C1: mov ecx,dword ptr [eax+000004C8h]
004F54C7: test ecx,ecx
004F54C9: je 004F54D9
004F54CB: fld dword ptr ds:[008CBC40h]
004F54D1: fadd dword ptr [esp+8]
004F54D5: fstp dword ptr [esp+8]
004F54D9: fadd dword ptr [esi+000000A0h]
004F54DF: pop esi
004F54E0: fadd dword ptr [esp+4]
004F54E4: ret 8
004F54F0: fld dword ptr [esp+4]
004F54F4: push esi
004F54F5: fsub dword ptr ds:[008CBBF4h]
004F54FB: mov esi,ecx
004F54FD: fld dword ptr ds:[00858624h]
004F5503: fsub dword ptr ds:[008CBBF4h]
004F5509: fdivp st(1),st
004F550B: fcom dword ptr ds:[00858624h]
004F5511: fnstsw ax
004F5513: test ah,5
004F5516: jp 004F5535
004F5518: fld st(0)
004F551A: fld dword ptr ds:[00858B50h]
004F5520: fcomp st(1)
004F5522: fnstsw ax
004F5524: fstp st(0)
004F5526: test ah,41h
004F5529: jne 004F5535
004F552B: fstp st(0)
004F552D: fld dword ptr ds:[00858B50h]
004F5533: jmp 004F554A
004F5535: fcom dword ptr ds:[00858624h]
004F553B: fnstsw ax
004F553D: test ah,5
004F5540: jnp 004F554A
004F5542: fstp st(0)
004F5544: fld dword ptr ds:[00858624h]
004F554A: fld dword ptr ds:[00B6BA28h]
004F5550: mov al,byte ptr [esi+000000A9h]
004F5556: cmp al,2
004F5558: fmul st,st(1)
004F555A: fadd dword ptr ds:[008CBC0Ch]
004F5560: fstp dword ptr [esp+8]
004F5564: fstp st(0)
004F5566: jne 004F558F
004F5568: fld dword ptr [esp+0Ch]
004F556C: fcomp qword ptr ds:[00862CD8h]
004F5572: fnstsw ax
004F5574: test ah,41h
004F5577: je 004F558F
004F5579: mov eax,dword ptr [esp+0Ch]
004F557D: push 8CC13Ch
004F5582: push 5
004F5584: push eax
004F5585: call 004D9D90
004F558A: add esp,0Ch
004F558D: jmp 004F5595
004F558F: fld dword ptr ds:[00858624h]
004F5595: mov ecx,dword ptr [esi+4]
004F5598: test byte ptr [ecx+0000042Bh],40h
 40
004F559F: pop esi
004F55A0: je 004F55B0
004F55A2: fld dword ptr ds:[008CBC48h]
004F55A8: fmul dword ptr [esp+4]
004F55AC: fstp dword ptr [esp+4]
004F55B0: fmul dword ptr [esp+4]
004F55B4: ret 8
004F55C0: push ebx
004F55C1: push ebp
004F55C2: mov bp,word ptr [esp+0Ch]
004F55C7: push esi
004F55C8: movsx eax,bp
004F55CB: mov esi,ecx
004F55CD: mov ecx,dword ptr [esi+eax*8+000000E8h]
 00
004F55D4: push edi
004F55D5: lea edi,[esi+eax*8+000000E8h]
 00
004F55DC: or ebx,0FFFFFFFFh
004F55DF: test ecx,ecx
004F55E1: je 004F55E7
004F55E3: mov bx,word ptr [ecx+70h]
004F55E7: test bp,bp
004F55EA: jl 004F56C1
004F55F0: cmp bp,0Ch
004F55F4: jge 004F56C1
004F55FA: test ecx,ecx
004F55FC: je 004F5614
004F55FE: push 0
004F5600: push 4
004F5602: call 004EF2B0
004F5607: mov ecx,dword ptr [edi]
004F5609: call 004EF1C0
004F560E: mov dword ptr [edi],0
004F5614: cmp bp,4
004F5618: jne 004F56C1
004F561E: mov cx,word ptr [esi+00000148h]
 00
004F5625: or eax,0FFFFFFFFh
004F5628: test bx,bx
004F562B: mov word ptr [esi+00000154h],cx
 00
004F5632: mov edx,dword ptr ds:[00B7CB84h]
004F5638: mov dword ptr [esi+00000150h],edx
004F563E: mov word ptr [esi+00000148h],ax
 00
004F5645: mov word ptr [esi+0000014Ah],ax
 00
004F564C: jle 004F56B8
004F564E: movsx eax,cx
004F5651: mov dword ptr [esp+14h],eax
004F5655: movsx ecx,bx
004F5658: fild dword ptr [esp+14h]
004F565C: mov dword ptr [esp+14h],ecx
004F5660: fidiv dword ptr [esp+14h]
004F5664: fcom dword ptr ds:[00858624h]
004F566A: fnstsw ax
004F566C: test ah,5
004F566F: jp 004F568A
004F5671: fld dword ptr ds:[00858B50h]
004F5677: fcomp st(1)
004F5679: fnstsw ax
004F567B: test ah,41h
004F567E: jne 004F568A
004F5680: fstp st(0)
004F5682: fld dword ptr ds:[00858B50h]
004F5688: jmp 004F569F
004F568A: fcom dword ptr ds:[00858624h]
004F5690: fnstsw ax
004F5692: test ah,5
004F5695: jnp 004F569F
004F5697: fstp st(0)
004F5699: fld dword ptr ds:[00858624h]
004F569F: fmul dword ptr ds:[00858628h]
004F56A5: call 00821B40
004F56AA: pop edi
004F56AB: mov word ptr [esi+0000014Eh],ax
 00
004F56B2: pop esi
004F56B3: pop ebp
004F56B4: pop ebx
004F56B5: ret 4
004F56B8: mov word ptr [esi+0000014Eh],0
004F56C1: pop edi
004F56C2: pop esi
004F56C3: pop ebp
004F56C4: pop ebx
004F56C5: ret 4
004F56D0: movsx eax,word ptr [esp+4]
004F56D5: fld dword ptr [esp+0Ch]
004F56D9: fadd dword ptr [ecx+000000D8h]
004F56DF: mov ecx,dword ptr [ecx+eax*8+000000E8h]
 00
004F56E6: test ecx,ecx
004F56E8: je 004F56F7
004F56EA: mov edx,dword ptr [esp+8]
004F56EE: fstp dword ptr [ecx+14h]
004F56F1: mov dword ptr [ecx+1Ch],edx
004F56F4: ret 0Ch
004F56F7: fstp st(0)
004F56F9: ret 0Ch
004F5700: push ecx
004F5701: push ebx
004F5702: push esi
004F5703: mov esi,ecx
004F5705: mov eax,dword ptr [esi+4]
004F5708: xor ebx,ebx
004F570A: mov byte ptr [esi+000000A7h],bl
004F5710: mov ecx,dword ptr [eax+00000460h]
004F5716: mov dword ptr ds:[00B6B990h],ecx
004F571C: mov al,byte ptr [esi+00000080h]
004F5722: cmp al,bl
004F5724: push edi
004F5725: je 004F5A05
004F572B: cmp al,1
004F572D: je 004F5A05
004F5733: cmp al,5
004F5735: je 004F5957
004F573B: cmp al,4
004F573D: je 004F5957
004F5743: cmp al,6
004F5745: je 004F5957
004F574B: cmp al,2
004F574D: jne 004F57FB
004F5753: mov edx,dword ptr ds:[00BD00F8h]
004F5759: movsx eax,byte ptr [edx+4Dh]
004F575D: mov dword ptr [esp+0Ch],eax
004F5761: push 28h
004F5763: mov ecx,0B62CB0h
004F5768: fild dword ptr [esp+10h]
004F576C: fstp dword ptr [esi+000000D8h]
004F5772: call 004EF520
004F5777: test ax,ax
004F577A: je 004F5789
004F577C: push ebx
004F577D: push 28h
004F577F: mov ecx,0B62CB0h
004F5784: call 004EFC60
004F5789: xor edi,edi
004F578B: jmp 004F5790
004F578D: lea ecx,[ecx]
004F5790: push edi
004F5791: mov ecx,esi
004F5793: call 004F55C0
004F5798: inc edi
004F5799: cmp di,0Ch
004F579D: jl 004F5790
004F579F: cmp word ptr [esi+000000E0h],0FFFFh
004F57A7: jne 004F57C2
004F57A9: xor ecx,ecx
004F57AB: mov cx,word ptr [esi+000000DCh]
 00
004F57B2: push ecx
004F57B3: call 004F4E60
004F57B8: add esp,4
004F57BB: mov word ptr [esi+000000E0h],ax
 00
004F57C2: xor edx,edx
004F57C4: mov dx,word ptr [esi+000000DEh]
 00
004F57CB: push 28h
004F57CD: mov ecx,0B5F8B8h
004F57D2: push edx
004F57D3: call 004D88C0
004F57D8: test al,al
004F57DA: jne 004F5B0C
004F57E0: xor eax,eax
004F57E2: mov ax,word ptr [esi+000000DEh]
 00
004F57E9: push 28h
004F57EB: mov ecx,0B5F8B8h
004F57F0: push eax
004F57F1: call 004D88A0
004F57F6: pop edi
004F57F7: pop esi
004F57F8: pop ebx
004F57F9: pop ecx
004F57FA: ret
004F57FB: cmp al,3
004F57FD: jne 004F5843
004F57FF: cmp word ptr [esi+000000E0h],0FFFFh
004F5807: mov ecx,dword ptr ds:[00BD00F8h]
004F580D: movsx edx,byte ptr [ecx+4Dh]
004F5811: mov dword ptr [esp+0Ch],edx
004F5815: fild dword ptr [esp+0Ch]
004F5819: fstp dword ptr [esi+000000D8h]
004F581F: jne 004F5B0C
004F5825: xor eax,eax
004F5827: mov ax,word ptr [esi+000000DCh]
 00
004F582E: push eax
004F582F: call 004F4E60
004F5834: add esp,4
004F5837: pop edi
004F5838: mov word ptr [esi+000000E0h],ax
 00
004F583F: pop esi
004F5840: pop ebx
004F5841: pop ecx
004F5842: ret
004F5843: cmp al,8
004F5845: jne 004F58DD
004F584B: mov ecx,dword ptr ds:[00BD00F8h]
004F5851: movsx edx,byte ptr [ecx+4Dh]
004F5855: mov dword ptr [esp+0Ch],edx
004F5859: push 28h
004F585B: mov ecx,0B62CB0h
004F5860: fild dword ptr [esp+10h]
004F5864: fstp dword ptr [esi+000000D8h]
004F586A: call 004EF520
004F586F: test ax,ax
004F5872: je 004F5881
004F5874: push ebx
004F5875: push 28h
004F5877: mov ecx,0B62CB0h
004F587C: call 004EFC60
004F5881: cmp word ptr [esi+000000E0h],0FFFFh
004F5889: jne 004F58A4
004F588B: xor eax,eax
004F588D: mov ax,word ptr [esi+000000DCh]
 00
004F5894: push eax
004F5895: call 004F4E60
004F589A: add esp,4
004F589D: mov word ptr [esi+000000E0h],ax
 00
004F58A4: xor ecx,ecx
004F58A6: mov cx,word ptr [esi+000000DEh]
 00
004F58AD: push 28h
004F58AF: push ecx
004F58B0: mov ecx,0B5F8B8h
004F58B5: call 004D88C0
004F58BA: test al,al
004F58BC: jne 004F5B0C
004F58C2: xor edx,edx
004F58C4: mov dx,word ptr [esi+000000DEh]
 00
004F58CB: push 28h
004F58CD: mov ecx,0B5F8B8h
004F58D2: push edx
004F58D3: call 004D88A0
004F58D8: pop edi
004F58D9: pop esi
004F58DA: pop ebx
004F58DB: pop ecx
004F58DC: ret
004F58DD: cmp al,9
004F58DF: jne 004F5B0C
004F58E5: mov eax,dword ptr ds:[00BD00F8h]
004F58EA: movsx ecx,byte ptr [eax+4Dh]
004F58EE: mov dword ptr [esp+0Ch],ecx
004F58F2: push 28h
004F58F4: mov ecx,0B62CB0h
004F58F9: fild dword ptr [esp+10h]
004F58FD: fstp dword ptr [esi+000000D8h]
004F5903: call 004EF520
004F5908: test ax,ax
004F590B: je 004F591A
004F590D: push ebx
004F590E: push 28h
004F5910: mov ecx,0B62CB0h
004F5915: call 004EFC60
004F591A: xor edi,edi
004F591C: lea esp,[esp]
004F5920: push edi
004F5921: mov ecx,esi
004F5923: call 004F55C0
004F5928: inc edi
004F5929: cmp di,0Ch
004F592D: jl 004F5920
004F592F: or edi,0FFFFFFFFh
004F5932: cmp word ptr [esi+000000E0h],di
 00
004F5939: jne 004F57C2
004F593F: xor eax,eax
004F5941: mov ax,word ptr [esi+000000DCh]
 00
004F5948: cmp ax,di
004F594B: je 004F57C2
004F5951: push eax
004F5952: jmp 004F57B3
004F5957: xor edi,edi
004F5959: lea esp,[esp+00000000h]
 00
004F5960: push edi
004F5961: mov ecx,esi
004F5963: call 004F55C0
004F5968: inc edi
004F5969: cmp di,0Ch
004F596D: jl 004F5960
004F596F: xor eax,eax
004F5971: mov ax,word ptr [esi+000000DEh]
 00
004F5978: or edi,0FFFFFFFFh
004F597B: cmp ax,di
004F597E: je 004F59C5
004F5980: push 28h
004F5982: push eax
004F5983: mov ecx,0B5F8B8h
004F5988: call 004D88C0
004F598D: test al,al
004F598F: jne 004F59C5
004F5991: push 28h
004F5993: mov ecx,0B62CB0h
004F5998: call 004EF520
004F599D: test ax,ax
004F59A0: je 004F59AF
004F59A2: push ebx
004F59A3: push 28h
004F59A5: mov ecx,0B62CB0h
004F59AA: call 004EFC60
004F59AF: xor ecx,ecx
004F59B1: mov cx,word ptr [esi+000000DEh]
 00
004F59B8: push 28h
004F59BA: push ecx
004F59BB: mov ecx,0B5F8B8h
004F59C0: call 004D88A0
004F59C5: xor eax,eax
004F59C7: mov ax,word ptr [esi+000000DCh]
 00
004F59CE: cmp ax,di
004F59D1: je 004F59EC
004F59D3: cmp word ptr [esi+000000E0h],di
 00
004F59DA: jne 004F59EC
004F59DC: push eax
004F59DD: call 004F4E60
004F59E2: add esp,4
004F59E5: mov word ptr [esi+000000E0h],ax
 00
004F59EC: pop edi
004F59ED: mov dword ptr [esi+0000022Ch],0BF800000h
004F59F7: mov dword ptr [esi+00000230h],0C2C80000h
004F5A01: pop esi
004F5A02: pop ebx
004F5A03: pop ecx
004F5A04: ret
004F5A05: or edi,0FFFFFFFFh
004F5A08: mov byte ptr [esi+000000AAh],bl
004F5A0E: mov dword ptr [esi+000000ACh],ebx
004F5A14: mov word ptr [esi+00000148h],di
 00
004F5A1B: mov word ptr [esi+0000014Ah],di
 00
004F5A22: mov edx,dword ptr ds:[00BD00F8h]
004F5A28: movsx eax,byte ptr [edx+4Dh]
004F5A2C: mov dword ptr [esp+0Ch],eax
004F5A30: push 28h
004F5A32: mov ecx,0B62CB0h
004F5A37: fild dword ptr [esp+10h]
004F5A3B: fstp dword ptr [esi+000000D8h]
004F5A41: call 004EF520
004F5A46: test ax,ax
004F5A49: je 004F5A58
004F5A4B: push ebx
004F5A4C: push 28h
004F5A4E: mov ecx,0B62CB0h
004F5A53: call 004EFC60
004F5A58: xor eax,eax
004F5A5A: mov ax,word ptr [esi+000000DCh]
 00
004F5A61: cmp ax,di
004F5A64: je 004F5B0C
004F5A6A: cmp word ptr [esi+000000DEh],di
 00
004F5A71: je 004F5B0C
004F5A77: cmp word ptr [esi+000000E0h],di
 00
004F5A7E: jne 004F5A90
004F5A80: push eax
004F5A81: call 004F4E60
004F5A86: add esp,4
004F5A89: mov word ptr [esi+000000E0h],ax
 00
004F5A90: xor ecx,ecx
004F5A92: mov cx,word ptr [esi+000000DEh]
 00
004F5A99: push 28h
004F5A9B: push ecx
004F5A9C: mov ecx,0B5F8B8h
004F5AA1: call 004D88C0
004F5AA6: test al,al
004F5AA8: jne 004F5AC0
004F5AAA: xor edx,edx
004F5AAC: mov dx,word ptr [esi+000000DEh]
 00
004F5AB3: push 28h
004F5AB5: mov ecx,0B5F8B8h
004F5ABA: push edx
004F5ABB: call 004D88A0
004F5AC0: cmp byte ptr [esi+000000A9h],1
 01
004F5AC7: jne 004F5AF5
004F5AC9: cmp word ptr [esi+000000E0h],di
 00
004F5AD0: je 004F5AF5
004F5AD2: xor edi,edi
004F5AD4: cmp di,2
004F5AD8: je 004F5AE2
004F5ADA: push edi
004F5ADB: mov ecx,esi
004F5ADD: call 004F55C0
004F5AE2: inc edi
004F5AE3: cmp di,0Ch
004F5AE7: jl 004F5AD4
004F5AE9: pop edi
004F5AEA: mov byte ptr [esi+000000A9h],6
 06
004F5AF1: pop esi
004F5AF2: pop ebx
004F5AF3: pop ecx
004F5AF4: ret
004F5AF5: xor edi,edi
004F5AF7: push edi
004F5AF8: mov ecx,esi
004F5AFA: call 004F55C0
004F5AFF: inc edi
004F5B00: cmp di,0Ch
004F5B04: jl 004F5AF7
004F5B06: mov byte ptr [esi+000000A9h],bl
004F5B0C: pop edi
004F5B0D: pop esi
004F5B0E: pop ebx
004F5B0F: pop ecx
004F5B10: ret
004F5B20: mov eax,dword ptr [ecx+4]
004F5B23: mov edx,dword ptr [eax+00000460h]
004F5B29: mov dword ptr ds:[00B6B98Ch],edx
004F5B2F: lea edx,[ecx+00000080h]
004F5B35: mov dword ptr ds:[00B6B99Ch],edx
004F5B3B: movsx eax,byte ptr [ecx+0000009Bh]
 00
004F5B42: sub eax,0
004F5B45: je 004F5B4F
004F5B47: sub eax,2
004F5B4A: je 004F5B4F
004F5B4C: dec eax
004F5B4D: jne 004F5B5A
004F5B4F: mov ecx,0B6BC90h
004F5B54: push edx
004F5B55: call 00507DF0
004F5B5A: ret
004F5B60: movsx eax,byte ptr [ecx+0000009Bh]
 00
004F5B67: push esi
004F5B68: xor esi,esi
004F5B6A: sub eax,esi
004F5B6C: je 004F5B76
004F5B6E: sub eax,2
004F5B71: je 004F5B76
004F5B73: dec eax
004F5B74: jne 004F5B88
004F5B76: add ecx,80h
004F5B7C: push esi
004F5B7D: push ecx
004F5B7E: mov ecx,0B6BC90h
004F5B83: call 00506F70
004F5B88: mov dword ptr ds:[00B6B98Ch],esi
004F5B8E: mov dword ptr ds:[00B6B99Ch],esi
004F5B94: pop esi
004F5B95: ret
004F5BA0: mov al,byte ptr [ecx+000000A5h]
004F5BA6: test al,al
004F5BA8: mov byte ptr [ecx+000000A7h],1
 01
004F5BAF: je 004F5BEC
004F5BB1: movsx eax,byte ptr [ecx+0000009Bh]
 00
004F5BB8: sub eax,0
004F5BBB: je 004F5BC5
004F5BBD: sub eax,2
004F5BC0: je 004F5BC5
004F5BC2: dec eax
004F5BC3: jne 004F5BD8
004F5BC5: add ecx,80h
004F5BCB: push 0
004F5BCD: push ecx
004F5BCE: mov ecx,0B6BC90h
004F5BD3: call 00506F70
004F5BD8: mov dword ptr ds:[00B6B98Ch],0
004F5BE2: mov dword ptr ds:[00B6B99Ch],0
004F5BEC: ret
004F5BF0: mov byte ptr [ecx+000000BDh],1
 01
004F5BF7: ret
004F5C00: mov byte ptr [ecx+000000BDh],0
 00
004F5C07: ret
004F5C10: movsx eax,word ptr [esp+8]
004F5C15: push esi
004F5C16: sub eax,190h
004F5C1B: push edi
004F5C1C: lea esi,[eax+eax*8]
004F5C1F: mov eax,dword ptr [esp+0Ch]
004F5C23: lea esi,[esi*4+00860AF0h]
 00
004F5C2A: mov ecx,9
004F5C2F: mov edi,eax
004F5C31: rep movs dword ptr es:[edi],dword ptr [esi]
004F5C33: pop edi
004F5C34: pop esi
004F5C35: ret 8
004F5C40: mov eax,dword ptr [ecx+4]
004F5C43: mov ax,word ptr [eax+22h]
004F5C47: cmp ax,1E7h
004F5C4B: je 004F5C56
004F5C4D: cmp ax,1E8h
004F5C51: je 004F5C56
004F5C53: xor al,al
004F5C55: ret
004F5C56: mov al,1
004F5C58: ret
004F5C60: xor ecx,ecx
004F5C62: movsx eax,cx
004F5C65: fld dword ptr [eax*8+008CC164h]
 00
004F5C6C: fcomp dword ptr [esp+4]
004F5C70: fnstsw ax
004F5C72: test ah,41h
004F5C75: je 004F5C7E
004F5C77: inc ecx
004F5C78: cmp cx,5
004F5C7C: jl 004F5C62
004F5C7E: fld dword ptr [esp+4]
004F5C82: fcomp qword ptr ds:[0085A310h]
004F5C88: fnstsw ax
004F5C8A: test ah,41h
004F5C8D: je 004F5CE9
004F5C8F: cmp cx,5
004F5C93: je 004F5CE9
004F5C95: fld dword ptr [esp+4]
004F5C99: fcomp dword ptr ds:[00858B50h]
004F5C9F: fnstsw ax
004F5CA1: test ah,5
004F5CA4: jnp 004F5CE0
004F5CA6: test cx,cx
004F5CA9: je 004F5CE0
004F5CAB: fld dword ptr [esp+4]
004F5CAF: movsx eax,cx
004F5CB2: shl eax,3
004F5CB5: fsub dword ptr [eax+008CC15Ch]
004F5CBB: fld dword ptr [eax+008CC164h]
004F5CC1: fsub dword ptr [eax+008CC15Ch]
004F5CC7: fdivp st(1),st
004F5CC9: fld dword ptr [eax+008CC168h]
004F5CCF: fsub dword ptr [eax+008CC160h]
004F5CD5: fmulp st(1),st
004F5CD7: fadd dword ptr [eax+008CC160h]
004F5CDD: ret 4
004F5CE0: fld dword ptr ds:[008CC168h]
004F5CE6: ret 4
004F5CE9: fld dword ptr ds:[008CC188h]
004F5CEF: ret 4
004F5D00: movsx eax,word ptr [esp+8]
004F5D05: dec eax
004F5D06: cmp eax,4
004F5D09: ja 004F5DC0
004F5D0F: jmp dword ptr [eax*4+004F5E30h]
 00
004F5D16: fld dword ptr ds:[00B6BA64h]
004F5D1C: mov eax,dword ptr [esp+4]
004F5D20: fmul dword ptr [eax+3Ch]
004F5D23: fadd dword ptr ds:[008CBCA8h]
004F5D29: jmp 004F5DC4
004F5D2E: mov eax,dword ptr [esp+4]
004F5D32: mov edx,dword ptr [eax+10h]
004F5D35: cmp word ptr [edx+22h],1C9h
004F5D3B: jne 004F5D4B
004F5D3D: fld dword ptr ds:[008CBC78h]
004F5D43: fsub dword ptr ds:[00858CA4h]
004F5D49: jmp 004F5DC4
004F5D4B: fld dword ptr ds:[00B6BA44h]
004F5D51: fmul dword ptr [eax+2Ch]
004F5D54: fadd dword ptr ds:[008CBC78h]
004F5D5A: jmp 004F5DC4
004F5D5C: mov al,byte ptr [ecx+000000B0h]
004F5D62: test al,al
004F5D64: je 004F5D6E
004F5D66: fld dword ptr ds:[00B6BA3Ch]
004F5D6C: jmp 004F5DC4
004F5D6E: fld dword ptr ds:[00B6BA54h]
004F5D74: fmul dword ptr [ecx+000000ACh]
004F5D7A: fadd dword ptr ds:[00B6B9D8h]
004F5D80: jmp 004F5DC4
004F5D82: mov al,byte ptr [ecx+000000B0h]
004F5D88: test al,al
004F5D8A: je 004F5D94
004F5D8C: fld dword ptr ds:[00B6BA3Ch]
004F5D92: jmp 004F5DC4
004F5D94: movzx eax,byte ptr [ecx+000000AAh]
 00
004F5D9B: mov dword ptr [esp+8],eax
004F5D9F: fild dword ptr [esp+8]
004F5DA3: fmul dword ptr ds:[00B6BA38h]
004F5DA9: fadd dword ptr ds:[008CBC68h]
004F5DAF: jmp 004F5DC4
004F5DB1: mov edx,dword ptr [esp+4]
004F5DB5: push 2
004F5DB7: push edx
004F5DB8: call 004F5D00
004F5DBD: ret 8
004F5DC0: fld dword ptr [esp+8]
004F5DC4: mov eax,dword ptr [ecx+4]
004F5DC7: test byte ptr [eax+0000042Bh],40h
 40
004F5DCE: je 004F5DD6
004F5DD0: fsub dword ptr ds:[008CBC44h]
004F5DD6: mov edx,dword ptr [eax+000004C8h]
004F5DDC: fadd dword ptr [ecx+000000A0h]
004F5DE2: test edx,edx
004F5DE4: je 004F5DEC
004F5DE6: fadd dword ptr ds:[008CBC40h]
004F5DEC: mov al,byte ptr [ecx+00000244h]
004F5DF2: test al,al
004F5DF4: je 004F5E2A
004F5DF6: fld dword ptr [ecx+00000248h]
004F5DFC: fcomp dword ptr ds:[00858624h]
004F5E02: fnstsw ax
004F5E04: test ah,41h
004F5E07: jp 004F5E2A
004F5E09: fld dword ptr [ecx+00000248h]
004F5E0F: fcomp dword ptr ds:[00858B50h]
004F5E15: fnstsw ax
004F5E17: test ah,1
004F5E1A: jne 004F5E2A
004F5E1C: fld dword ptr ds:[008CBC4Ch]
004F5E22: fmul dword ptr [ecx+00000248h]
004F5E28: faddp st(1),st
004F5E2A: ret 8
004F5E2D: lea ecx,[ecx]
004F5E30: push ss
004F5E31: pop ebp
004F5E32: dec edi
004F5E33: add byte ptr [esi],ch
004F5E35: pop ebp
004F5E36: dec edi
004F5E37: add byte ptr [ebp+ebx*2+4Fh],bl
004F5E3B: add byte ptr [edx+B1004F5Dh],al
004F5E41: pop ebp
004F5E42: dec edi
004F5E43: add byte ptr [eax+90909090h],dl
004F5E50: xor eax,eax
004F5E52: mov ax,word ptr [ecx+0000014Ch]
 00
004F5E59: cmp ax,word ptr ds:[008CBCC0h]
 00
004F5E60: jge 004F5E6D
004F5E62: inc eax
004F5E63: mov word ptr [ecx+0000014Ch],ax
 00
004F5E6A: xor al,al
004F5E6C: ret
004F5E6D: mov dx,word ptr [ecx+00000148h]
 00
004F5E74: test dx,dx
004F5E77: jl 004F5E6A
004F5E79: cmp word ptr [ecx+0000014Ah],0
004F5E81: jl 004F5E6A
004F5E83: mov eax,dword ptr [ecx+00000108h]
004F5E89: test eax,eax
004F5E8B: je 004F5E6A
004F5E8D: movsx eax,word ptr [eax+70h]
004F5E91: push esi
004F5E92: movsx esi,word ptr ds:[008CBCBCh]
 00
004F5E99: movsx edx,dx
004F5E9C: sub eax,esi
004F5E9E: cmp edx,eax
004F5EA0: pop esi
004F5EA1: jle 004F5E6A
004F5EA3: mov word ptr [ecx+0000014Ch],0
004F5EAC: mov al,1
004F5EAE: ret
004F5EB0: mov eax,dword ptr [esp+4]
004F5EB4: fld dword ptr [eax+0000049Ch]
004F5EBA: mov ecx,dword ptr [esp+8]
004F5EBE: test ecx,ecx
004F5EC0: fabs
004F5EC2: fstp dword ptr [esp+4]
004F5EC6: je 004F5ED5
004F5EC8: cmp ecx,9
004F5ECB: jne 004F5F21
004F5ECD: lea ecx,[eax+00000808h]
004F5ED3: jmp 004F5EDB
004F5ED5: lea ecx,[eax+00000964h]
004F5EDB: fld dword ptr [esp+4]
004F5EDF: fcomp dword ptr [ecx]
004F5EE1: fnstsw ax
004F5EE3: test ah,41h
004F5EE6: jne 004F5F06
004F5EE8: fld dword ptr ds:[008CBC24h]
004F5EEE: fadd dword ptr [ecx]
004F5EF0: fcom dword ptr [esp+4]
004F5EF4: fnstsw ax
004F5EF6: test ah,5
004F5EF9: jnp 004F5F1F
004F5EFB: fstp st(0)
004F5EFD: fld dword ptr [esp+4]
004F5F01: fstp dword ptr [ecx]
004F5F03: ret 8
004F5F06: fld dword ptr [ecx]
004F5F08: fsub dword ptr ds:[008CBC28h]
004F5F0E: fcom dword ptr [esp+4]
004F5F12: fnstsw ax
004F5F14: test ah,41h
004F5F17: je 004F5F1F
004F5F19: fstp st(0)
004F5F1B: fld dword ptr [esp+4]
004F5F1F: fstp dword ptr [ecx]
004F5F21: ret 8
004F5F30: mov al,byte ptr ds:[008CBD80h]
004F5F35: fld dword ptr ds:[00858B50h]
004F5F3B: test al,al
004F5F3D: jne 004F5F4A
004F5F3F: fstp st(0)
004F5F41: fld dword ptr ds:[00858B50h]
004F5F47: ret 14h
004F5F4A: mov eax,dword ptr [esp+8]
004F5F4E: dec eax
004F5F4F: je 004F5FCC
004F5F51: dec eax
004F5F52: je 004F5F9E
004F5F54: dec eax
004F5F55: jne 004F5FFB
004F5F5B: fld dword ptr [esp+14h]
004F5F5F: fabs
004F5F61: fst dword ptr [esp+14h]
004F5F65: fcomp dword ptr ds:[00858CECh]
004F5F6B: fnstsw ax
004F5F6D: test ah,41h
004F5F70: jne 004F5F95
004F5F72: fstp st(0)
004F5F74: fld dword ptr [esp+14h]
004F5F78: fmul dword ptr ds:[00858C80h]
004F5F7E: fld dword ptr ds:[00858624h]
004F5F84: fcomp st(1)
004F5F86: fnstsw ax
004F5F88: test ah,5
004F5F8B: jp 004F5F95
004F5F8D: fstp st(0)
004F5F8F: fld dword ptr ds:[00858624h]
004F5F95: fmul dword ptr ds:[008CBD8Ch]
004F5F9B: ret 14h
004F5F9E: fstp st(0)
004F5FA0: fld dword ptr [esp+14h]
004F5FA4: fabs
004F5FA6: fld dword ptr ds:[00858624h]
004F5FAC: fcomp st(1)
004F5FAE: fnstsw ax
004F5FB0: test ah,5
004F5FB3: jp 004F5FBD
004F5FB5: fstp st(0)
004F5FB7: fld dword ptr ds:[00858624h]
004F5FBD: fmul dword ptr ds:[00858F34h]
004F5FC3: fmul dword ptr ds:[008CBD84h]
004F5FC9: ret 14h
004F5FCC: fld dword ptr [esp+0Ch]
004F5FD0: fcomp dword ptr ds:[00858EE8h]
004F5FD6: fnstsw ax
004F5FD8: test ah,41h
004F5FDB: jne 004F5FF5
004F5FDD: fstp st(0)
004F5FDF: fld dword ptr [esp+0Ch]
004F5FE3: fsub dword ptr ds:[00858EE8h]
004F5FE9: fmul dword ptr ds:[0085F0A8h]
004F5FEF: fmul dword ptr ds:[00858F34h]
004F5FF5: fmul dword ptr ds:[008CBD88h]
004F5FFB: ret 14h
004F6000: mov al,byte ptr ds:[008CBD81h]
004F6005: fld dword ptr ds:[00858B50h]
004F600B: test al,al
004F600D: jne 004F601A
004F600F: fstp st(0)
004F6011: fld dword ptr ds:[00858B50h]
004F6017: ret 10h
004F601A: mov eax,dword ptr [esp+8]
004F601E: sub eax,2
004F6021: je 004F6069
004F6023: dec eax
004F6024: jne 004F60A4
004F6026: fld dword ptr [esp+10h]
004F602A: fabs
004F602C: fst dword ptr [esp+10h]
004F6030: fcomp dword ptr ds:[00858CECh]
004F6036: fnstsw ax
004F6038: test ah,41h
004F603B: jne 004F6060
004F603D: fstp st(0)
004F603F: fld dword ptr [esp+10h]
004F6043: fmul dword ptr ds:[00858C80h]
004F6049: fld dword ptr ds:[00858624h]
004F604F: fcomp st(1)
004F6051: fnstsw ax
004F6053: test ah,5
004F6056: jp 004F6060
004F6058: fstp st(0)
004F605A: fld dword ptr ds:[00858624h]
004F6060: fmul dword ptr ds:[008CBD8Ch]
004F6066: ret 10h
004F6069: fstp st(0)
004F606B: fld dword ptr [esp+10h]
004F606F: fabs
004F6071: fld dword ptr ds:[00858624h]
004F6077: fcomp st(1)
004F6079: fnstsw ax
004F607B: test ah,5
004F607E: jp 004F6088
004F6080: fstp st(0)
004F6082: fld dword ptr ds:[00858624h]
004F6088: fmul dword ptr ds:[00858F34h]
004F608E: mov al,byte ptr [ecx+00000080h]
004F6094: cmp al,2
004F6096: fmul dword ptr ds:[008CBD84h]
004F609C: jne 004F60A4
004F609E: fmul dword ptr ds:[00858CC4h]
004F60A4: ret 10h
004F60B0: xor ecx,ecx
004F60B2: movsx eax,cx
004F60B5: fld dword ptr [eax*8+008CC18Ch]
 00
004F60BC: fcomp dword ptr [esp+4]
004F60C0: fnstsw ax
004F60C2: test ah,41h
004F60C5: je 004F60CE
004F60C7: inc ecx
004F60C8: cmp cx,5
004F60CC: jl 004F60B2
004F60CE: fld dword ptr [esp+4]
004F60D2: fcomp qword ptr ds:[0085A310h]
004F60D8: fnstsw ax
004F60DA: test ah,41h
004F60DD: je 004F6139
004F60DF: cmp cx,5
004F60E3: je 004F6139
004F60E5: fld dword ptr [esp+4]
004F60E9: fcomp dword ptr ds:[00858B50h]
004F60EF: fnstsw ax
004F60F1: test ah,5
004F60F4: jnp 004F6130
004F60F6: test cx,cx
004F60F9: je 004F6130
004F60FB: fld dword ptr [esp+4]
004F60FF: movsx eax,cx
004F6102: shl eax,3
004F6105: fsub dword ptr [eax+008CC184h]
004F610B: fld dword ptr [eax+008CC18Ch]
004F6111: fsub dword ptr [eax+008CC184h]
004F6117: fdivp st(1),st
004F6119: fld dword ptr [eax+008CC190h]
004F611F: fsub dword ptr [eax+008CC188h]
004F6125: fmulp st(1),st
004F6127: fadd dword ptr [eax+008CC188h]
004F612D: ret 4
004F6130: fld dword ptr ds:[008CC190h]
004F6136: ret 4
004F6139: fld dword ptr ds:[008CC1B0h]
004F613F: ret 4
004F6150: mov ecx,dword ptr [esp+4]
004F6154: fld dword ptr [ecx+58h]
004F6157: fld dword ptr [ecx+54h]
004F615A: fld dword ptr [ecx+50h]
004F615D: fld st(0)
004F615F: fmul st,st(1)
004F6161: fld st(2)
004F6163: fmul st,st(3)
004F6165: faddp st(1),st
004F6167: fld st(3)
004F6169: fmul st,st(4)
004F616B: faddp st(1),st
004F616D: fcomp dword ptr ds:[00862CE8h]
004F6173: fstp st(0)
004F6175: fnstsw ax
004F6177: fstp st(0)
004F6179: test ah,5
004F617C: fstp st(0)
004F617E: jp 004F61A1
004F6180: fld dword ptr [ecx+58h]
004F6183: fld dword ptr [ecx+54h]
004F6186: fld dword ptr [ecx+50h]
004F6189: fld st(0)
004F618B: fmul st,st(1)
004F618D: fld st(2)
004F618F: fmul st,st(3)
004F6191: faddp st(1),st
004F6193: fld st(3)
004F6195: fmul st,st(4)
004F6197: faddp st(1),st
004F6199: fstp st(3)
004F619B: fstp st(0)
004F619D: fstp st(0)
004F619F: jmp 004F61A7
004F61A1: fld dword ptr ds:[00862CE8h]
004F61A7: fmul dword ptr ds:[00862CE4h]
004F61AD: fst dword ptr [esp+4]
004F61B1: fcomp dword ptr ds:[00862CE0h]
004F61B7: fnstsw ax
004F61B9: test ah,5
004F61BC: jp 004F61C7
004F61BE: fld dword ptr ds:[00859014h]
004F61C4: ret 4
004F61C7: mov eax,dword ptr [esp+4]
004F61CB: push eax
004F61CC: call 004D9E50
004F61D1: fmul dword ptr ds:[0085862Ch]
004F61D7: add esp,4
004F61DA: ret 4
004F61E0: mov al,byte ptr [ecx+000000A8h]
004F61E6: test al,al
004F61E8: je 004F61F4
004F61EA: mov eax,dword ptr [esp+4]
004F61EE: mov byte ptr [eax],0
004F61F1: ret 8
004F61F4: push esi
004F61F5: mov esi,dword ptr [esp+0Ch]
004F61F9: mov eax,dword ptr [esi+10h]
004F61FC: mov cx,word ptr [eax+0000045Ch]
 00
004F6203: test cx,cx
004F6206: je 004F621A
004F6208: cmp cx,0FFFFh
004F620D: je 004F621A
004F620F: mov cl,byte ptr [eax+36h]
004F6212: and cl,0F8h
004F6215: cmp cl,28h
004F6218: jne 004F628E
004F621A: mov ecx,dword ptr [eax+00000514h]
004F6220: test ecx,ecx
004F6222: jbe 004F628E
004F6224: test byte ptr [eax+36h],0F8h
004F6228: je 004F6283
004F622A: mov edx,2Ch
004F622F: cmp ecx,edx
004F6231: jbe 004F6239
004F6233: mov dword ptr [eax+00000514h],edx
004F6239: mov eax,dword ptr [esi+10h]
004F623C: cmp dword ptr [eax+00000514h],edx
004F6242: jne 004F6259
004F6244: push 7
004F6246: push 0
004F6248: call 004D9C10
004F624D: mov ecx,dword ptr [esi+10h]
004F6250: add esp,8
004F6253: mov byte ptr [ecx+00000518h],al
004F6259: mov eax,dword ptr [esi+10h]
004F625C: movzx edx,byte ptr [eax+00000518h]
 00
004F6263: mov ecx,dword ptr [eax+00000514h]
004F6269: imul edx,edx,2Ch
004F626C: sub edx,ecx
004F626E: mov al,byte ptr [edx+00862B9Ch]
004F6274: test al,al
004F6276: jne 004F6283
004F6278: mov eax,dword ptr [esp+8]
004F627C: mov byte ptr [eax],0
004F627F: pop esi
004F6280: ret 8
004F6283: mov ecx,dword ptr [esp+8]
004F6287: mov byte ptr [ecx],1
004F628A: pop esi
004F628B: ret 8
004F628E: mov edx,dword ptr [esp+8]
004F6292: mov byte ptr [edx],0
004F6295: pop esi
004F6296: ret 8
004F62A0: mov al,byte ptr [ecx+000000A8h]
004F62A6: test al,al
004F62A8: jne 004F630A
004F62AA: mov al,byte ptr [ecx+000000C8h]
004F62B0: test al,al
004F62B2: je 004F630A
004F62B4: mov eax,dword ptr [esp+0Ch]
004F62B8: mov ecx,dword ptr [eax+10h]
004F62BB: mov dl,byte ptr [ecx+0000042Dh]
004F62C1: test dl,dl
004F62C3: jns 004F6300
004F62C5: mov ecx,dword ptr [esp+4]
004F62C9: mov byte ptr [ecx],1
004F62CC: mov eax,dword ptr [eax+10h]
004F62CF: mov dl,byte ptr [eax+36h]
004F62D2: and dl,0F8h
004F62D5: cmp dl,20h
004F62D8: je 004F62FA
004F62DA: mov ecx,dword ptr [eax+00000514h]
004F62E0: test ecx,ecx
004F62E2: je 004F62F6
004F62E4: cmp word ptr [eax+22h],1A7h
004F62EA: je 004F62F6
004F62EC: mov eax,dword ptr [esp+8]
004F62F0: mov byte ptr [eax],1
004F62F3: ret 0Ch
004F62F6: mov ecx,dword ptr [esp+8]
004F62FA: mov byte ptr [ecx],0
004F62FD: ret 0Ch
004F6300: mov edx,dword ptr [esp+4]
004F6304: mov byte ptr [edx],0
004F6307: ret 0Ch
004F630A: mov eax,dword ptr [esp+4]
004F630E: mov byte ptr [eax],0
004F6311: ret 0Ch
004F6320: movsx eax,word ptr [esp+4]
004F6325: push esi
004F6326: lea esi,[ecx+eax*8+000000E8h]
 00
004F632D: mov ecx,dword ptr [esi]
004F632F: test ecx,ecx
004F6331: je 004F6349
004F6333: push 0
004F6335: push 4
004F6337: call 004EF2B0
004F633C: mov ecx,dword ptr [esi]
004F633E: call 004EF1C0
004F6343: mov dword ptr [esi],0
004F6349: pop esi
004F634A: ret 4
004F6350: ret 4
004F6360: ret 4
004F6370: ret 4
004F6380: ret 4
004F6390: ret 4
004F63A0: ret 4
004F63B0: mov eax,ecx
004F63B2: xor ecx,ecx
004F63B4: mov dword ptr [eax+10h],ecx
004F63B7: mov dword ptr [eax],85F438h
004F63BD: mov word ptr [eax+00000088h],cx
 00
004F63C4: mov dword ptr [eax+000000A0h],ecx
004F63CA: mov dword ptr [eax+000000A4h],ecx
004F63D0: ret
004F63E0: mov eax,ecx
004F63E2: xor ecx,ecx
004F63E4: mov dword ptr [eax+10h],ecx
004F63E7: mov dword ptr [eax],862CECh
004F63ED: mov dword ptr [eax+00000194h],ecx
004F63F3: mov dword ptr [eax+00000184h],85F438h
004F63FD: mov word ptr [eax+0000020Ch],cx
 00
004F6404: mov dword ptr [eax+00000224h],ecx
004F640A: mov dword ptr [eax+00000228h],ecx
004F6410: mov byte ptr [eax+000000A4h],cl
004F6416: mov dword ptr [eax+4],ecx
004F6419: ret
004F6420: push 0FFFFFFFFh
004F6422: push 83C288h
004F6427: mov eax,dword ptr fs:[00000000h]
004F642D: push eax
004F642E: mov dword ptr fs:[00000000h],esp
 00
004F6435: sub esp,7Ch
004F6438: push ebp
004F6439: push esi
004F643A: xor ebp,ebp
004F643C: push edi
004F643D: mov esi,ecx
004F643F: mov dword ptr [esp+1Ch],ebp
004F6443: push 13h
004F6445: push 8Ah
004F644A: mov ecx,0B5F8B8h
004F644F: mov dword ptr [esp+00000098h],ebp
 00
004F6456: call 004D88C0
004F645B: test al,al
004F645D: je 004F7494
004F6463: push 2
004F6465: push 27h
004F6467: mov ecx,0B5F8B8h
004F646C: call 004D88C0
004F6471: test al,al
004F6473: je 004F7494
004F6479: push 3
004F647B: push 1Bh
004F647D: mov ecx,0B5F8B8h
004F6482: call 004D88C0
004F6487: test al,al
004F6489: je 004F7494
004F648F: mov eax,dword ptr [esp+00000098h]
 00
004F6496: add eax,0FFFFFFB2h
004F6499: or edi,0FFFFFFFFh
004F649C: cmp eax,2Ch
004F649F: ja 004F7494
004F64A5: movzx eax,byte ptr [eax+004F751Ch]
 00
004F64AC: jmp dword ptr [eax*4+004F74C4h]
 00
004F64B3: movsx eax,byte ptr [esi+00000098h]
 00
004F64BA: cmp eax,4
004F64BD: ja 004F64E2
004F64BF: jmp dword ptr [eax*4+004F754Ch]
 00
004F64C6: mov edx,2Ah
004F64CB: jmp 004F64E7
004F64CD: mov edx,27h
004F64D2: jmp 004F64E7
004F64D4: mov edx,29h
004F64D9: jmp 004F64E7
004F64DB: mov edx,2Bh
004F64E0: jmp 004F64E7
004F64E2: mov edx,28h
004F64E7: mov ecx,dword ptr ds:[00BD00F8h]
004F64ED: movsx eax,byte ptr [ecx+4Eh]
004F64F1: mov dword ptr [esp+10h],eax
004F64F5: fild dword ptr [esp+10h]
004F64F9: jmp 004F6592
004F64FE: add eax,4
004F6501: mov ecx,dword ptr [esp+0Ch]
004F6505: mov edi,dword ptr [eax]
004F6507: push ebp
004F6508: push ebp
004F6509: push ebp
004F650A: push ebp
004F650B: push 3F800000h
004F6510: push 3F800000h
004F6515: push 3F800000h
004F651A: push ecx
004F651B: sub esp,0Ch
004F651E: mov ecx,esp
004F6520: mov dword ptr [ecx],edi
004F6522: mov edi,dword ptr [eax+4]
004F6525: mov eax,dword ptr [eax+8]
004F6528: push esi
004F6529: mov dword ptr [ecx+4],edi
004F652C: push edx
004F652D: mov dword ptr [ecx+8],eax
004F6530: push 13h
004F6532: lea ecx,[esp+4Ch]
004F6536: call 004EFE50
004F653B: lea ecx,[esp+14h]
004F653F: mov dword ptr [esp+34h],3CA3D70Ah
004F6547: push ecx
004F6548: jmp 004F748A
004F654D: movsx eax,byte ptr [esi+00000098h]
 00
004F6554: cmp eax,4
004F6557: ja 004F657C
004F6559: jmp dword ptr [eax*4+004F7560h]
 00
004F6560: mov edx,23h
004F6565: jmp 004F6581
004F6567: mov edx,20h
004F656C: jmp 004F6581
004F656E: mov edx,22h
004F6573: jmp 004F6581
004F6575: mov edx,24h
004F657A: jmp 004F6581
004F657C: mov edx,21h
004F6581: mov eax,dword ptr ds:[00BD00F8h]
004F6586: movsx ecx,byte ptr [eax+54h]
004F658A: mov dword ptr [esp+10h],ecx
004F658E: fild dword ptr [esp+10h]
004F6592: mov eax,dword ptr [esi+4]
004F6595: fstp dword ptr [esp+0Ch]
004F6599: mov ecx,dword ptr [eax+14h]
004F659C: cmp ecx,ebp
004F659E: je 004F64FE
004F65A4: lea eax,[ecx+30h]
004F65A7: jmp 004F6501
004F65AC: mov edx,dword ptr ds:[00BD00F8h]
004F65B2: movsx eax,byte ptr [edx+5Ah]
004F65B6: mov dword ptr [esp+10h],eax
004F65BA: mov eax,dword ptr [esi+4]
004F65BD: mov ecx,dword ptr [eax+14h]
004F65C0: cmp ecx,ebp
004F65C2: fild dword ptr [esp+10h]
004F65C6: fstp dword ptr [esp+0Ch]
004F65CA: je 004F65D1
004F65CC: lea eax,[ecx+30h]
004F65CF: jmp 004F65D4
004F65D1: add eax,4
004F65D4: mov ecx,dword ptr [esp+0Ch]
004F65D8: push ebp
004F65D9: push ebp
004F65DA: push ebp
004F65DB: push ebp
004F65DC: push 3F800000h
004F65E1: push 3F800000h
004F65E6: push 3F800000h
004F65EB: push ecx
004F65EC: sub esp,0Ch
004F65EF: mov edx,esp
004F65F1: push esi
004F65F2: push ebp
004F65F3: push 3
004F65F5: jmp 004F668E
004F65FA: mov edx,dword ptr ds:[00BD00F8h]
004F6600: movsx eax,byte ptr [edx+5Bh]
004F6604: mov dword ptr [esp+10h],eax
004F6608: mov eax,dword ptr [esi+4]
004F660B: mov ecx,dword ptr [eax+14h]
004F660E: cmp ecx,ebp
004F6610: fild dword ptr [esp+10h]
004F6614: fstp dword ptr [esp+0Ch]
004F6618: je 004F661F
004F661A: lea eax,[ecx+30h]
004F661D: jmp 004F6622
004F661F: add eax,4
004F6622: mov ecx,dword ptr [esp+0Ch]
004F6626: push ebp
004F6627: push ebp
004F6628: push ebp
004F6629: push ebp
004F662A: push 3F800000h
004F662F: push 3F800000h
004F6634: push 3F800000h
004F6639: push ecx
004F663A: sub esp,0Ch
004F663D: mov edx,esp
004F663F: push esi
004F6640: push 44h
004F6642: jmp 004F668C
004F6644: mov edx,dword ptr ds:[00BD00F8h]
004F664A: movsx eax,byte ptr [edx+5Ch]
004F664E: mov dword ptr [esp+10h],eax
004F6652: mov eax,dword ptr [esi+4]
004F6655: mov ecx,dword ptr [eax+14h]
004F6658: cmp ecx,ebp
004F665A: fild dword ptr [esp+10h]
004F665E: fstp dword ptr [esp+0Ch]
004F6662: je 004F6669
004F6664: lea eax,[ecx+30h]
004F6667: jmp 004F666C
004F6669: add eax,4
004F666C: mov ecx,dword ptr [esp+0Ch]
004F6670: push ebp
004F6671: push ebp
004F6672: push ebp
004F6673: push ebp
004F6674: push 3F800000h
004F6679: push 3F800000h
004F667E: push 3F800000h
004F6683: push ecx
004F6684: sub esp,0Ch
004F6687: mov edx,esp
004F6689: push esi
004F668A: push 35h
004F668C: push 2
004F668E: mov ecx,dword ptr [eax]
004F6690: mov dword ptr [edx],ecx
004F6692: mov ecx,dword ptr [eax+4]
004F6695: mov eax,dword ptr [eax+8]
004F6698: mov dword ptr [edx+4],ecx
004F669B: lea ecx,[esp+4Ch]
004F669F: mov dword ptr [edx+8],eax
004F66A2: call 004EFE50
004F66A7: lea ecx,[esp+14h]
004F66AB: push ecx
004F66AC: jmp 004F748A
004F66B1: mov edx,dword ptr ds:[00BD00F8h]
004F66B7: movsx eax,byte ptr [edx+63h]
004F66BB: mov ecx,dword ptr [esi+000000CCh]
004F66C1: mov edx,dword ptr ds:[008CBD7Ch]
004F66C7: mov dword ptr [esp+10h],eax
004F66CB: mov eax,dword ptr ds:[00B7CB84h]
004F66D0: add ecx,edx
004F66D2: cmp eax,ecx
004F66D4: fild dword ptr [esp+10h]
004F66D8: fstp dword ptr [esp+0Ch]
004F66DC: jb 004F7494
004F66E2: fld dword ptr [esp+0000009Ch]
 00
004F66E9: fcomp dword ptr ds:[008CBD74h]
004F66EF: fnstsw ax
004F66F1: test ah,5
004F66F4: jnp 004F7494
004F66FA: fld dword ptr [esp+0000009Ch]
 00
004F6701: fdiv dword ptr ds:[008CBD78h]
004F6707: fst dword ptr [esp+10h]
004F670B: fcomp dword ptr ds:[00858624h]
004F6711: fnstsw ax
004F6713: test ah,5
004F6716: jp 004F672D
004F6718: fld dword ptr ds:[00858B50h]
004F671E: fcomp dword ptr [esp+10h]
004F6722: fnstsw ax
004F6724: test ah,41h
004F6727: je 004F7494
004F672D: fld dword ptr [esp+10h]
004F6731: fcomp dword ptr ds:[00858624h]
004F6737: fnstsw ax
004F6739: test ah,5
004F673C: jnp 004F6748
004F673E: mov dword ptr [esp+10h],3F800000h
004F6746: jmp 004F675D
004F6748: fld dword ptr [esp+10h]
004F674C: fcomp dword ptr ds:[00858C14h]
004F6752: fnstsw ax
004F6754: test ah,41h
004F6757: jne 004F7494
004F675D: fld dword ptr ds:[00C8132Ch]
004F6763: fcomp dword ptr ds:[00858B8Ch]
004F6769: fnstsw ax
004F676B: test ah,5
004F676E: jp 004F7494
004F6774: mov edx,dword ptr [esp+10h]
004F6778: push edx
004F6779: call 004D9E50
004F677E: fmul dword ptr ds:[00858BA4h]
004F6784: mov eax,dword ptr [esi+4]
004F6787: mov ecx,dword ptr [eax+14h]
004F678A: fadd dword ptr ds:[00B6B9C8h]
004F6790: add esp,4
004F6793: cmp ecx,ebp
004F6795: fadd dword ptr [esp+0Ch]
004F6799: fstp dword ptr [esp+10h]
004F679D: je 004F67A4
004F679F: lea eax,[ecx+30h]
004F67A2: jmp 004F67A7
004F67A4: add eax,4
004F67A7: mov ecx,dword ptr [esp+10h]
004F67AB: push ebp
004F67AC: push ebp
004F67AD: push ebp
004F67AE: push ebp
004F67AF: push 3F800000h
004F67B4: push 3F800000h
004F67B9: push 3F800000h
004F67BE: push ecx
004F67BF: mov ecx,dword ptr [eax]
004F67C1: sub esp,0Ch
004F67C4: mov edx,esp
004F67C6: mov dword ptr [edx],ecx
004F67C8: mov ecx,dword ptr [eax+4]
004F67CB: mov eax,dword ptr [eax+8]
004F67CE: push esi
004F67CF: mov dword ptr [edx+4],ecx
004F67D2: push 42h
004F67D4: push 2
004F67D6: lea ecx,[esp+4Ch]
004F67DA: mov dword ptr [edx+8],eax
004F67DD: call 004EFE50
004F67E2: lea ecx,[esp+14h]
004F67E6: push ecx
004F67E7: mov ecx,0B62CB0h
004F67EC: call 004EFB10
004F67F1: mov edx,dword ptr ds:[00B7CB84h]
004F67F7: mov dword ptr [esi+000000CCh],edx
004F67FD: jmp 004F7494
004F6802: mov eax,dword ptr ds:[00BD00F8h]
004F6807: movsx ecx,byte ptr [eax+64h]
004F680B: mov ax,word ptr [esi+000000E0h]
 00
004F6812: cmp ax,di
004F6815: mov dword ptr [esp+10h],ecx
004F6819: fild dword ptr [esp+10h]
004F681D: fstp dword ptr [esp+0Ch]
004F6821: je 004F683C
004F6823: xor edx,edx
004F6825: mov dx,word ptr [esi+000000DCh]
 00
004F682C: push eax
004F682D: mov ecx,0B5F8B8h
004F6832: push edx
004F6833: call 004D88C0
004F6838: test al,al
004F683A: jne 004F6848
004F683C: mov dword ptr [esp+00000090h],edi
 00
004F6843: jmp 004F749F
004F6848: push 5
004F684A: push 3
004F684C: call 004D9C10
004F6851: mov ecx,dword ptr [esi+4]
004F6854: mov edx,dword ptr [ecx+14h]
004F6857: add esp,8
004F685A: cmp edx,ebp
004F685C: je 004F6863
004F685E: lea ecx,[edx+30h]
004F6861: jmp 004F6866
004F6863: add ecx,4
004F6866: mov edx,dword ptr [esp+0Ch]
004F686A: mov edi,dword ptr [ecx]
004F686C: push ebp
004F686D: push ebp
004F686E: push ebp
004F686F: push ebp
004F6870: push 3F800000h
004F6875: push 3F800000h
004F687A: push 3F800000h
004F687F: push edx
004F6880: sub esp,0Ch
004F6883: mov edx,esp
004F6885: mov dword ptr [edx],edi
004F6887: mov edi,dword ptr [ecx+4]
004F688A: mov ecx,dword ptr [ecx+8]
004F688D: mov dword ptr [edx+4],edi
004F6890: mov dword ptr [edx+8],ecx
004F6893: xor edx,edx
004F6895: mov dx,word ptr [esi+000000E0h]
 00
004F689C: push esi
004F689D: push eax
004F689E: lea ecx,[esp+48h]
004F68A2: push edx
004F68A3: call 004EFE50
004F68A8: lea eax,[esp+14h]
004F68AC: mov dword ptr [esp+30h],3F800000h
004F68B4: mov dword ptr [esp+34h],3DCCCCCDh
004F68BC: mov dword ptr [esp+2Ch],40000000h
004F68C4: push eax
004F68C5: jmp 004F748A
004F68CA: mov ecx,dword ptr ds:[00BD00F8h]
004F68D0: movsx edx,byte ptr [ecx+77h]
004F68D4: movsx eax,byte ptr [esi+00000080h]
 00
004F68DB: dec eax
004F68DC: mov dword ptr [esp+10h],edx
004F68E0: fild dword ptr [esp+10h]
004F68E4: fstp dword ptr [esp+0Ch]
004F68E8: je 004F68F9
004F68EA: dec eax
004F68EB: jne 004F6907
004F68ED: fld dword ptr [esp+0Ch]
004F68F1: fsub dword ptr ds:[00858B44h]
004F68F7: jmp 004F6903
004F68F9: fld dword ptr [esp+0Ch]
004F68FD: fsub dword ptr ds:[00858B3Ch]
004F6903: fstp dword ptr [esp+0Ch]
004F6907: push 2
004F6909: push 27h
004F690B: mov ecx,0B5F8B8h
004F6910: call 004D88C0
004F6915: test al,al
004F6917: je 004F7494
004F691D: push esi
004F691E: push 77h
004F6920: mov ecx,0B62CB0h
004F6925: call 004EF570
004F692A: test ax,ax
004F692D: je 004F693E
004F692F: mov edi,28h
004F6934: mov dword ptr [esp+10h],3F4CCCCDh
004F693C: jmp 004F6948
004F693E: xor edi,edi
004F6940: mov dword ptr [esp+10h],3F70A3D7h
004F6948: mov eax,dword ptr [esi+4]
004F694B: mov ecx,dword ptr [eax+14h]
004F694E: cmp ecx,ebp
004F6950: je 004F6957
004F6952: lea eax,[ecx+30h]
004F6955: jmp 004F695A
004F6957: add eax,4
004F695A: mov ecx,dword ptr [esp+0Ch]
004F695E: push ebp
004F695F: push ebp
004F6960: push ebp
004F6961: push ebp
004F6962: push 3F800000h
004F6967: push 3F800000h
004F696C: push 3F800000h
004F6971: push ecx
004F6972: mov ecx,dword ptr [eax]
004F6974: sub esp,0Ch
004F6977: mov edx,esp
004F6979: mov dword ptr [edx],ecx
004F697B: mov ecx,dword ptr [eax+4]
004F697E: mov eax,dword ptr [eax+8]
004F6981: push esi
004F6982: mov dword ptr [edx+4],ecx
004F6985: push 1Dh
004F6987: push 2
004F6989: lea ecx,[esp+4Ch]
004F698D: mov dword ptr [edx+8],eax
004F6990: call 004EFE50
004F6995: mov ecx,dword ptr [esp+10h]
004F6999: mov dword ptr [esp+30h],ecx
004F699D: mov dword ptr [esp+34h],3D75C290h
004F69A5: mov dword ptr [esp+2Ch],3FC00000h
004F69AD: mov word ptr [esp+6Ah],20h
 00
004F69B4: mov word ptr [esp+70h],di
004F69B9: mov dword ptr [esp+20h],77h
004F69C1: jmp 004F7485
004F69C6: mov eax,dword ptr ds:[00BD00F8h]
004F69CB: movsx ecx,byte ptr [eax+79h]
004F69CF: movsx eax,byte ptr [esi+00000080h]
 00
004F69D6: dec eax
004F69D7: mov dword ptr [esp+10h],ecx
004F69DB: fild dword ptr [esp+10h]
004F69DF: fstp dword ptr [esp+0Ch]
004F69E3: je 004F69F4
004F69E5: dec eax
004F69E6: jne 004F6A02
004F69E8: fld dword ptr [esp+0Ch]
004F69EC: fsub dword ptr ds:[00858B44h]
004F69F2: jmp 004F69FE
004F69F4: fld dword ptr [esp+0Ch]
004F69F8: fsub dword ptr ds:[00858B3Ch]
004F69FE: fstp dword ptr [esp+0Ch]
004F6A02: push 2
004F6A04: push 27h
004F6A06: mov ecx,0B5F8B8h
004F6A0B: call 004D88C0
004F6A10: test al,al
004F6A12: je 004F7494
004F6A18: mov eax,dword ptr [esi+4]
004F6A1B: mov ecx,dword ptr [eax+14h]
004F6A1E: cmp ecx,ebp
004F6A20: je 004F6A27
004F6A22: lea eax,[ecx+30h]
004F6A25: jmp 004F6A2A
004F6A27: add eax,4
004F6A2A: fld dword ptr [esp+0Ch]
004F6A2E: mov ecx,dword ptr [eax]
004F6A30: fsub dword ptr ds:[00858CA0h]
004F6A36: push ebp
004F6A37: push ebp
004F6A38: push ebp
004F6A39: push ebp
004F6A3A: push 3F800000h
004F6A3F: push 3F800000h
004F6A44: push 3F800000h
004F6A49: sub esp,10h
004F6A4C: mov edx,esp
004F6A4E: fstp dword ptr [esp+0Ch]
004F6A52: mov dword ptr [edx],ecx
004F6A54: mov ecx,dword ptr [eax+4]
004F6A57: mov eax,dword ptr [eax+8]
004F6A5A: push esi
004F6A5B: mov dword ptr [edx+4],ecx
004F6A5E: push 21h
004F6A60: push 2
004F6A62: lea ecx,[esp+4Ch]
004F6A66: mov dword ptr [edx+8],eax
004F6A69: call 004EFE50
004F6A6E: lea ecx,[esp+14h]
004F6A72: push ecx
004F6A73: mov edi,20h
004F6A78: mov ecx,0B62CB0h
004F6A7D: mov dword ptr [esp+34h],3F800000h
004F6A85: mov dword ptr [esp+38h],3D75C290h
004F6A8D: mov dword ptr [esp+30h],3FC00000h
004F6A95: mov word ptr [esp+6Eh],di
004F6A9A: mov word ptr [esp+74h],bp
004F6A9F: mov dword ptr [esp+24h],79h
004F6AA7: call 004EFB10
004F6AAC: push 1Ch
004F6AAE: push 14h
004F6AB0: call 004D9C10
004F6AB5: mov ecx,dword ptr [esi+4]
004F6AB8: mov edx,dword ptr [ecx+14h]
004F6ABB: add esp,8
004F6ABE: cmp edx,ebp
004F6AC0: je 004F6AC7
004F6AC2: lea ecx,[edx+30h]
004F6AC5: jmp 004F6ACA
004F6AC7: add ecx,4
004F6ACA: mov edx,dword ptr [esp+0Ch]
004F6ACE: push ebp
004F6ACF: push ebp
004F6AD0: push ebp
004F6AD1: push ebp
004F6AD2: mov ebp,dword ptr [ecx]
004F6AD4: push 3F800000h
004F6AD9: push 3F800000h
004F6ADE: push 3F800000h
004F6AE3: push edx
004F6AE4: sub esp,0Ch
004F6AE7: mov edx,esp
004F6AE9: mov dword ptr [edx],ebp
004F6AEB: mov ebp,dword ptr [ecx+4]
004F6AEE: mov ecx,dword ptr [ecx+8]
004F6AF1: push esi
004F6AF2: mov dword ptr [edx+4],ebp
004F6AF5: push eax
004F6AF6: mov dword ptr [edx+8],ecx
004F6AF9: push 2
004F6AFB: lea ecx,[esp+4Ch]
004F6AFF: call 004EFE50
004F6B04: mov dword ptr [esp+30h],3F800000h
004F6B0C: mov dword ptr [esp+34h],3D75C290h
004F6B14: mov dword ptr [esp+2Ch],3FC00000h
004F6B1C: mov word ptr [esp+6Ah],di
004F6B21: mov word ptr [esp+70h],37h
 00
004F6B28: mov dword ptr [esp+20h],79h
004F6B30: jmp 004F7485
004F6B35: mov eax,dword ptr ds:[00BD00F8h]
004F6B3A: movsx ecx,byte ptr [eax+78h]
004F6B3E: movsx eax,byte ptr [esi+00000080h]
 00
004F6B45: dec eax
004F6B46: mov dword ptr [esp+10h],ecx
004F6B4A: fild dword ptr [esp+10h]
004F6B4E: fstp dword ptr [esp+0Ch]
004F6B52: je 004F6B63
004F6B54: dec eax
004F6B55: jne 004F6B71
004F6B57: fld dword ptr [esp+0Ch]
004F6B5B: fsub dword ptr ds:[00858B44h]
004F6B61: jmp 004F6B6D
004F6B63: fld dword ptr [esp+0Ch]
004F6B67: fsub dword ptr ds:[00858B3Ch]
004F6B6D: fstp dword ptr [esp+0Ch]
004F6B71: push 2
004F6B73: push 27h
004F6B75: mov ecx,0B5F8B8h
004F6B7A: call 004D88C0
004F6B7F: test al,al
004F6B81: je 004F7494
004F6B87: push 31h
004F6B89: push 2Fh
004F6B8B: call 004D9C10
004F6B90: mov ecx,dword ptr [esi+4]
004F6B93: mov edx,dword ptr [ecx+14h]
004F6B96: add esp,8
004F6B99: cmp edx,ebp
004F6B9B: je 004F6BA2
004F6B9D: lea ecx,[edx+30h]
004F6BA0: jmp 004F6BA5
004F6BA2: add ecx,4
004F6BA5: mov edx,dword ptr [esp+0Ch]
004F6BA9: mov edi,dword ptr [ecx]
004F6BAB: push ebp
004F6BAC: push ebp
004F6BAD: push ebp
004F6BAE: push ebp
004F6BAF: push 3F800000h
004F6BB4: push 3F800000h
004F6BB9: push 3F800000h
004F6BBE: push edx
004F6BBF: sub esp,0Ch
004F6BC2: mov edx,esp
004F6BC4: mov dword ptr [edx],edi
004F6BC6: mov edi,dword ptr [ecx+4]
004F6BC9: mov ecx,dword ptr [ecx+8]
004F6BCC: push esi
004F6BCD: mov dword ptr [edx+4],edi
004F6BD0: push eax
004F6BD1: mov dword ptr [edx+8],ecx
004F6BD4: push 2
004F6BD6: lea ecx,[esp+4Ch]
004F6BDA: call 004EFE50
004F6BDF: mov dword ptr [esp+30h],3F800000h
004F6BE7: mov dword ptr [esp+34h],3D75C290h
004F6BEF: mov dword ptr [esp+2Ch],3FC00000h
004F6BF7: mov word ptr [esp+6Ah],20h
 00
004F6BFE: mov word ptr [esp+70h],bp
004F6C03: mov dword ptr [esp+20h],79h
004F6C0B: jmp 004F7485
004F6C10: mov eax,dword ptr ds:[00BD00F8h]
004F6C15: movsx ecx,byte ptr [eax+7Ah]
004F6C19: movsx eax,byte ptr [esi+00000080h]
 00
004F6C20: dec eax
004F6C21: mov dword ptr [esp+10h],ecx
004F6C25: fild dword ptr [esp+10h]
004F6C29: fstp dword ptr [esp+0Ch]
004F6C2D: je 004F6C3E
004F6C2F: dec eax
004F6C30: jne 004F6C4C
004F6C32: fld dword ptr [esp+0Ch]
004F6C36: fsub dword ptr ds:[00858B44h]
004F6C3C: jmp 004F6C48
004F6C3E: fld dword ptr [esp+0Ch]
004F6C42: fsub dword ptr ds:[00858B3Ch]
004F6C48: fstp dword ptr [esp+0Ch]
004F6C4C: push 2
004F6C4E: push 27h
004F6C50: mov ecx,0B5F8B8h
004F6C55: call 004D88C0
004F6C5A: test al,al
004F6C5C: je 004F7494
004F6C62: mov eax,dword ptr [esi+4]
004F6C65: mov ecx,dword ptr [eax+14h]
004F6C68: cmp ecx,ebp
004F6C6A: je 004F6C71
004F6C6C: lea eax,[ecx+30h]
004F6C6F: jmp 004F6C74
004F6C71: add eax,4
004F6C74: fld dword ptr [esp+0Ch]
004F6C78: mov ecx,dword ptr [eax]
004F6C7A: fsub dword ptr ds:[00858CA0h]
004F6C80: push ebp
004F6C81: push ebp
004F6C82: push ebp
004F6C83: push ebp
004F6C84: push 3F800000h
004F6C89: push 3F800000h
004F6C8E: push 3F800000h
004F6C93: sub esp,10h
004F6C96: mov edx,esp
004F6C98: fstp dword ptr [esp+0Ch]
004F6C9C: mov dword ptr [edx],ecx
004F6C9E: mov ecx,dword ptr [eax+4]
004F6CA1: mov eax,dword ptr [eax+8]
004F6CA4: push esi
004F6CA5: mov dword ptr [edx+4],ecx
004F6CA8: push 1Fh
004F6CAA: push 2
004F6CAC: lea ecx,[esp+4Ch]
004F6CB0: mov dword ptr [edx+8],eax
004F6CB3: call 004EFE50
004F6CB8: lea ecx,[esp+14h]
004F6CBC: push ecx
004F6CBD: mov edi,20h
004F6CC2: mov ecx,0B62CB0h
004F6CC7: mov dword ptr [esp+34h],3F800000h
004F6CCF: mov dword ptr [esp+38h],3D75C290h
004F6CD7: mov dword ptr [esp+30h],3FC00000h
004F6CDF: mov word ptr [esp+6Eh],di
004F6CE4: mov word ptr [esp+74h],bp
004F6CE9: mov dword ptr [esp+24h],7Ah
004F6CF1: call 004EFB10
004F6CF6: push 1Ch
004F6CF8: push 14h
004F6CFA: call 004D9C10
004F6CFF: mov ecx,dword ptr [esi+4]
004F6D02: mov edx,dword ptr [ecx+14h]
004F6D05: add esp,8
004F6D08: cmp edx,ebp
004F6D0A: je 004F6D11
004F6D0C: lea ecx,[edx+30h]
004F6D0F: jmp 004F6D14
004F6D11: add ecx,4
004F6D14: mov edx,dword ptr [esp+0Ch]
004F6D18: push ebp
004F6D19: push ebp
004F6D1A: push ebp
004F6D1B: push ebp
004F6D1C: mov ebp,dword ptr [ecx]
004F6D1E: push 3F800000h
004F6D23: push 3F800000h
004F6D28: push 3F800000h
004F6D2D: push edx
004F6D2E: sub esp,0Ch
004F6D31: mov edx,esp
004F6D33: mov dword ptr [edx],ebp
004F6D35: mov ebp,dword ptr [ecx+4]
004F6D38: mov ecx,dword ptr [ecx+8]
004F6D3B: push esi
004F6D3C: mov dword ptr [edx+4],ebp
004F6D3F: push eax
004F6D40: mov dword ptr [edx+8],ecx
004F6D43: push 2
004F6D45: lea ecx,[esp+4Ch]
004F6D49: call 004EFE50
004F6D4E: mov dword ptr [esp+30h],3F800000h
004F6D56: mov dword ptr [esp+34h],3D75C290h
004F6D5E: mov dword ptr [esp+2Ch],3FC00000h
004F6D66: mov word ptr [esp+6Ah],di
004F6D6B: mov word ptr [esp+70h],19h
 00
004F6D72: mov dword ptr [esp+20h],7Ah
004F6D7A: jmp 004F7485
004F6D7F: push 5
004F6D81: push 8Fh
004F6D86: mov ecx,0B5F8B8h
004F6D8B: call 004D88C0
004F6D90: test al,al
004F6D92: je 004F7494
004F6D98: mov eax,dword ptr ds:[00BD00F8h]
004F6D9D: movsx ecx,byte ptr [eax+66h]
004F6DA1: mov dword ptr [esp+10h],ecx
004F6DA5: push esi
004F6DA6: push 66h
004F6DA8: fild dword ptr [esp+18h]
004F6DAC: mov ecx,0B62CB0h
004F6DB1: fstp dword ptr [esp+14h]
004F6DB5: call 004EF570
004F6DBA: test ax,ax
004F6DBD: jne 004F7494
004F6DC3: mov eax,dword ptr [esi+4]
004F6DC6: mov ecx,dword ptr [eax+14h]
004F6DC9: cmp ecx,ebp
004F6DCB: je 004F6DD2
004F6DCD: lea eax,[ecx+30h]
004F6DD0: jmp 004F6DD5
004F6DD2: add eax,4
004F6DD5: mov edx,dword ptr [esp+0Ch]
004F6DD9: push ebp
004F6DDA: push ebp
004F6DDB: push ebp
004F6DDC: push ebp
004F6DDD: push 3F800000h
004F6DE2: push 3F800000h
004F6DE7: push 3F800000h
004F6DEC: push edx
004F6DED: mov edx,dword ptr [eax]
004F6DEF: sub esp,0Ch
004F6DF2: mov ecx,esp
004F6DF4: mov dword ptr [ecx],edx
004F6DF6: mov edx,dword ptr [eax+4]
004F6DF9: mov eax,dword ptr [eax+8]
004F6DFC: push esi
004F6DFD: mov dword ptr [ecx+4],edx
004F6E00: push 53h
004F6E02: mov dword ptr [ecx+8],eax
004F6E05: push 5
004F6E07: lea ecx,[esp+4Ch]
004F6E0B: call 004EFE50
004F6E10: mov ecx,dword ptr [esi+4]
004F6E13: push ecx
004F6E14: lea ecx,[esp+18h]
004F6E18: mov dword ptr [esp+34h],3F63D70Ah
004F6E20: mov dword ptr [esp+38h],3CF5C28Fh
004F6E28: mov dword ptr [esp+30h],40000000h
004F6E30: mov word ptr [esp+6Eh],80h
 00
004F6E37: call 004EF820
004F6E3C: mov dword ptr [esp+20h],66h
004F6E44: jmp 004F7485
004F6E49: mov eax,dword ptr ds:[00BD00F8h]
004F6E4E: movsx ecx,byte ptr [eax+67h]
004F6E52: mov eax,dword ptr [esi+4]
004F6E55: mov dword ptr [esp+10h],ecx
004F6E59: mov ecx,dword ptr [eax+14h]
004F6E5C: cmp ecx,ebp
004F6E5E: fild dword ptr [esp+10h]
004F6E62: fstp dword ptr [esp+0Ch]
004F6E66: je 004F6E6D
004F6E68: lea eax,[ecx+30h]
004F6E6B: jmp 004F6E70
004F6E6D: add eax,4
004F6E70: mov edx,dword ptr [esp+0Ch]
004F6E74: push ebp
004F6E75: push ebp
004F6E76: push ebp
004F6E77: push ebp
004F6E78: push 3F800000h
004F6E7D: push 3F800000h
004F6E82: push 3F800000h
004F6E87: push edx
004F6E88: mov edx,dword ptr [eax]
004F6E8A: sub esp,0Ch
004F6E8D: mov ecx,esp
004F6E8F: mov dword ptr [ecx],edx
004F6E91: mov edx,dword ptr [eax+4]
004F6E94: mov eax,dword ptr [eax+8]
004F6E97: push esi
004F6E98: mov dword ptr [ecx+4],edx
004F6E9B: push 25h
004F6E9D: mov dword ptr [ecx+8],eax
004F6EA0: push 13h
004F6EA2: lea ecx,[esp+4Ch]
004F6EA6: call 004EFE50
004F6EAB: lea ecx,[esp+14h]
004F6EAF: mov dword ptr [esp+34h],3D4CCCCDh
004F6EB7: mov dword ptr [esp+2Ch],40000000h
004F6EBF: mov dword ptr [esp+20h],67h
004F6EC7: push ecx
004F6EC8: jmp 004F748A
004F6ECD: mov edx,dword ptr ds:[00BD00F8h]
004F6ED3: movsx eax,byte ptr [edx+6Ch]
004F6ED7: mov dword ptr [esp+10h],eax
004F6EDB: push esi
004F6EDC: push 6Ch
004F6EDE: fild dword ptr [esp+18h]
004F6EE2: mov ecx,0B62CB0h
004F6EE7: fadd dword ptr ds:[008CC1BCh]
004F6EED: fstp dword ptr [esp+14h]
004F6EF1: call 004EF570
004F6EF6: test ax,ax
004F6EF9: jne 004F7494
004F6EFF: mov eax,dword ptr [esi+4]
004F6F02: mov ecx,dword ptr [eax+14h]
004F6F05: cmp ecx,ebp
004F6F07: je 004F6F0E
004F6F09: lea eax,[ecx+30h]
004F6F0C: jmp 004F6F11
004F6F0E: add eax,4
004F6F11: mov ecx,dword ptr [esp+0Ch]
004F6F15: push ebp
004F6F16: push ebp
004F6F17: push ebp
004F6F18: push ebp
004F6F19: push 3F800000h
004F6F1E: push 3F800000h
004F6F23: push 3F800000h
004F6F28: push ecx
004F6F29: mov ecx,dword ptr [eax]
004F6F2B: sub esp,0Ch
004F6F2E: mov edx,esp
004F6F30: mov dword ptr [edx],ecx
004F6F32: mov ecx,dword ptr [eax+4]
004F6F35: mov eax,dword ptr [eax+8]
004F6F38: push esi
004F6F39: mov dword ptr [edx+4],ecx
004F6F3C: push 1Fh
004F6F3E: push 13h
004F6F40: lea ecx,[esp+4Ch]
004F6F44: mov dword ptr [edx+8],eax
004F6F47: call 004EFE50
004F6F4C: lea ecx,[esp+14h]
004F6F50: mov dword ptr [esp+34h],3D4CCCCDh
004F6F58: mov dword ptr [esp+2Ch],40000000h
004F6F60: mov dword ptr [esp+20h],6Ch
004F6F68: push ecx
004F6F69: jmp 004F748A
004F6F6E: mov edx,dword ptr ds:[00BD00F8h]
004F6F74: movsx eax,byte ptr [edx+6Dh]
004F6F78: mov dword ptr [esp+10h],eax
004F6F7C: push esi
004F6F7D: push 6Dh
004F6F7F: fild dword ptr [esp+18h]
004F6F83: mov ecx,0B62CB0h
004F6F88: fadd dword ptr ds:[00B6BAC4h]
004F6F8E: fstp dword ptr [esp+14h]
004F6F92: call 004EF570
004F6F97: test ax,ax
004F6F9A: jne 004F7494
004F6FA0: mov eax,dword ptr [esi+4]
004F6FA3: mov ecx,dword ptr [eax+14h]
004F6FA6: cmp ecx,ebp
004F6FA8: je 004F6FAF
004F6FAA: lea eax,[ecx+30h]
004F6FAD: jmp 004F6FB2
004F6FAF: add eax,4
004F6FB2: mov ecx,dword ptr [esp+0Ch]
004F6FB6: push ebp
004F6FB7: push ebp
004F6FB8: push ebp
004F6FB9: push ebp
004F6FBA: push 3F800000h
004F6FBF: push 3F800000h
004F6FC4: push 3F800000h
004F6FC9: push ecx
004F6FCA: mov ecx,dword ptr [eax]
004F6FCC: sub esp,0Ch
004F6FCF: mov edx,esp
004F6FD1: mov dword ptr [edx],ecx
004F6FD3: mov ecx,dword ptr [eax+4]
004F6FD6: mov eax,dword ptr [eax+8]
004F6FD9: push esi
004F6FDA: mov dword ptr [edx+4],ecx
004F6FDD: push 2Ch
004F6FDF: push 13h
004F6FE1: lea ecx,[esp+4Ch]
004F6FE5: mov dword ptr [edx+8],eax
004F6FE8: call 004EFE50
004F6FED: lea ecx,[esp+14h]
004F6FF1: mov word ptr [esp+6Ah],20h
 00
004F6FF8: mov word ptr [esp+70h],bp
004F6FFD: mov dword ptr [esp+34h],3D4CCCCDh
004F7005: mov dword ptr [esp+2Ch],40000000h
004F700D: mov dword ptr [esp+20h],6Dh
004F7015: push ecx
004F7016: jmp 004F748A
004F701B: mov edx,dword ptr ds:[00BD00F8h]
004F7021: movsx eax,byte ptr [edx+6Eh]
004F7025: mov dword ptr [esp+10h],eax
004F7029: push esi
004F702A: push 6Eh
004F702C: fild dword ptr [esp+18h]
004F7030: mov ecx,0B62CB0h
004F7035: fadd dword ptr ds:[00B6BAC0h]
004F703B: fstp dword ptr [esp+14h]
004F703F: call 004EF570
004F7044: test ax,ax
004F7047: jne 004F7494
004F704D: mov eax,dword ptr [esi+4]
004F7050: mov ecx,dword ptr [eax+14h]
004F7053: cmp ecx,ebp
004F7055: je 004F705C
004F7057: lea eax,[ecx+30h]
004F705A: jmp 004F705F
004F705C: add eax,4
004F705F: mov ecx,dword ptr [esp+0Ch]
004F7063: push ebp
004F7064: push ebp
004F7065: push ebp
004F7066: push ebp
004F7067: push 3F800000h
004F706C: push 3F800000h
004F7071: push 3F800000h
004F7076: push ecx
004F7077: mov ecx,dword ptr [eax]
004F7079: sub esp,0Ch
004F707C: mov edx,esp
004F707E: mov dword ptr [edx],ecx
004F7080: mov ecx,dword ptr [eax+4]
004F7083: mov eax,dword ptr [eax+8]
004F7086: push esi
004F7087: mov dword ptr [edx+4],ecx
004F708A: push 2Ch
004F708C: push 13h
004F708E: lea ecx,[esp+4Ch]
004F7092: mov dword ptr [edx+8],eax
004F7095: call 004EFE50
004F709A: lea ecx,[esp+14h]
004F709E: mov word ptr [esp+6Ah],20h
 00
004F70A5: mov word ptr [esp+70h],bp
004F70AA: mov dword ptr [esp+34h],3D4CCCCDh
004F70B2: mov dword ptr [esp+2Ch],40000000h
004F70BA: mov dword ptr [esp+20h],6Eh
004F70C2: push ecx
004F70C3: jmp 004F748A
004F70C8: mov edx,dword ptr ds:[00BD00F8h]
004F70CE: movsx eax,byte ptr [edx+6Bh]
004F70D2: mov dword ptr [esp+10h],eax
004F70D6: push esi
004F70D7: push 6Bh
004F70D9: fild dword ptr [esp+18h]
004F70DD: mov ecx,0B62CB0h
004F70E2: fadd dword ptr ds:[008CC1B8h]
004F70E8: fstp dword ptr [esp+14h]
004F70EC: call 004EF570
004F70F1: test ax,ax
004F70F4: jne 004F7494
004F70FA: mov eax,dword ptr [esi+4]
004F70FD: mov ecx,dword ptr [eax+14h]
004F7100: cmp ecx,ebp
004F7102: je 004F7109
004F7104: lea eax,[ecx+30h]
004F7107: jmp 004F710C
004F7109: add eax,4
004F710C: mov ecx,dword ptr [esp+0Ch]
004F7110: push ebp
004F7111: push ebp
004F7112: push ebp
004F7113: push ebp
004F7114: push 3F800000h
004F7119: push 3F800000h
004F711E: push 3F800000h
004F7123: push ecx
004F7124: mov ecx,dword ptr [eax]
004F7126: sub esp,0Ch
004F7129: mov edx,esp
004F712B: mov dword ptr [edx],ecx
004F712D: mov ecx,dword ptr [eax+4]
004F7130: mov eax,dword ptr [eax+8]
004F7133: push esi
004F7134: mov dword ptr [edx+4],ecx
004F7137: push 26h
004F7139: push 13h
004F713B: lea ecx,[esp+4Ch]
004F713F: mov dword ptr [edx+8],eax
004F7142: call 004EFE50
004F7147: lea ecx,[esp+14h]
004F714B: mov dword ptr [esp+34h],3D4CCCCDh
004F7153: mov dword ptr [esp+2Ch],40000000h
004F715B: mov dword ptr [esp+20h],6Bh
004F7163: push ecx
004F7164: jmp 004F748A
004F7169: mov edx,dword ptr ds:[00BD00F8h]
004F716F: movsx eax,byte ptr [edx+6Fh]
004F7173: mov dword ptr [esp+10h],eax
004F7177: push esi
004F7178: push 6Fh
004F717A: fild dword ptr [esp+18h]
004F717E: mov ecx,0B62CB0h
004F7183: fadd dword ptr ds:[008CC1B4h]
004F7189: fstp dword ptr [esp+14h]
004F718D: call 004EF570
004F7192: test ax,ax
004F7195: jne 004F7494
004F719B: mov eax,dword ptr [esi+4]
004F719E: mov ecx,dword ptr [eax+14h]
004F71A1: cmp ecx,ebp
004F71A3: je 004F71AA
004F71A5: lea eax,[ecx+30h]
004F71A8: jmp 004F71AD
004F71AA: add eax,4
004F71AD: mov ecx,dword ptr [esp+0Ch]
004F71B1: push ebp
004F71B2: push ebp
004F71B3: push ebp
004F71B4: push ebp
004F71B5: push 3F800000h
004F71BA: push 3F800000h
004F71BF: push 3F800000h
004F71C4: push ecx
004F71C5: mov ecx,dword ptr [eax]
004F71C7: sub esp,0Ch
004F71CA: mov edx,esp
004F71CC: mov dword ptr [edx],ecx
004F71CE: mov ecx,dword ptr [eax+4]
004F71D1: mov eax,dword ptr [eax+8]
004F71D4: push esi
004F71D5: mov dword ptr [edx+4],ecx
004F71D8: push 26h
004F71DA: push 13h
004F71DC: lea ecx,[esp+4Ch]
004F71E0: mov dword ptr [edx+8],eax
004F71E3: call 004EFE50
004F71E8: lea ecx,[esp+14h]
004F71EC: mov dword ptr [esp+34h],3D4CCCCDh
004F71F4: mov dword ptr [esp+2Ch],40000000h
004F71FC: mov dword ptr [esp+20h],6Fh
004F7204: push ecx
004F7205: jmp 004F748A
004F720A: mov edx,dword ptr ds:[00BD00F8h]
004F7210: movsx eax,byte ptr [edx+73h]
004F7214: mov dword ptr [esp+10h],eax
004F7218: push esi
004F7219: push 73h
004F721B: fild dword ptr [esp+18h]
004F721F: mov ecx,0B62CB0h
004F7224: fstp dword ptr [esp+14h]
004F7228: call 004EF570
004F722D: test ax,ax
004F7230: jne 004F7494
004F7236: push 28h
004F7238: push 45h
004F723A: mov ecx,0B5F8B8h
004F723F: call 004D88C0
004F7244: test al,al
004F7246: je 004F7494
004F724C: mov eax,dword ptr [esi+4]
004F724F: mov ecx,dword ptr [eax+14h]
004F7252: cmp ecx,ebp
004F7254: je 004F725B
004F7256: lea eax,[ecx+30h]
004F7259: jmp 004F725E
004F725B: add eax,4
004F725E: mov ecx,dword ptr [esp+0Ch]
004F7262: push ebp
004F7263: push ebp
004F7264: push ebp
004F7265: push ebp
004F7266: push 3F800000h
004F726B: push 3F800000h
004F7270: push 3F800000h
004F7275: push ecx
004F7276: mov ecx,dword ptr [eax]
004F7278: sub esp,0Ch
004F727B: mov edx,esp
004F727D: mov dword ptr [edx],ecx
004F727F: mov ecx,dword ptr [eax+4]
004F7282: mov eax,dword ptr [eax+8]
004F7285: push esi
004F7286: mov dword ptr [edx+4],ecx
004F7289: push 4
004F728B: push 28h
004F728D: lea ecx,[esp+4Ch]
004F7291: mov dword ptr [edx+8],eax
004F7294: call 004EFE50
004F7299: mov ecx,dword ptr [esi+4]
004F729C: push ecx
004F729D: lea ecx,[esp+18h]
004F72A1: mov dword ptr [esp+38h],3D4CCCCDh
004F72A9: mov dword ptr [esp+30h],40000000h
004F72B1: mov word ptr [esp+6Eh],80h
 00
004F72B8: call 004EF820
004F72BD: mov dword ptr [esp+20h],73h
004F72C5: jmp 004F7485
004F72CA: mov eax,dword ptr ds:[00BD00F8h]
004F72CF: movsx ecx,byte ptr [eax+71h]
004F72D3: mov dword ptr [esp+10h],ecx
004F72D7: push esi
004F72D8: push 71h
004F72DA: fild dword ptr [esp+18h]
004F72DE: mov ecx,0B62CB0h
004F72E3: fstp dword ptr [esp+14h]
004F72E7: call 004EF570
004F72EC: test ax,ax
004F72EF: jne 004F7494
004F72F5: push esi
004F72F6: push 72h
004F72F8: mov ecx,0B62CB0h
004F72FD: call 004EF570
004F7302: test ax,ax
004F7305: jne 004F7494
004F730B: push 5
004F730D: push 8Fh
004F7312: mov ecx,0B5F8B8h
004F7317: call 004D88C0
004F731C: test al,al
004F731E: je 004F7494
004F7324: mov eax,dword ptr [esi+4]
004F7327: mov ecx,dword ptr [eax+14h]
004F732A: cmp ecx,ebp
004F732C: je 004F7333
004F732E: lea eax,[ecx+30h]
004F7331: jmp 004F7336
004F7333: add eax,4
004F7336: mov edx,dword ptr [esp+0Ch]
004F733A: push ebp
004F733B: push ebp
004F733C: push ebp
004F733D: push ebp
004F733E: push 3F800000h
004F7343: push 3F800000h
004F7348: push 3F800000h
004F734D: push edx
004F734E: mov edx,dword ptr [eax]
004F7350: sub esp,0Ch
004F7353: mov ecx,esp
004F7355: mov dword ptr [ecx],edx
004F7357: mov edx,dword ptr [eax+4]
004F735A: mov eax,dword ptr [eax+8]
004F735D: push esi
004F735E: mov dword ptr [ecx+4],edx
004F7361: push 46h
004F7363: mov dword ptr [ecx+8],eax
004F7366: push 5
004F7368: lea ecx,[esp+4Ch]
004F736C: call 004EFE50
004F7371: mov ecx,dword ptr [esi+4]
004F7374: push ecx
004F7375: lea ecx,[esp+18h]
004F7379: mov dword ptr [esp+34h],3F0F5C29h
004F7381: mov dword ptr [esp+38h],3D4CCCCDh
004F7389: mov dword ptr [esp+30h],40000000h
004F7391: mov word ptr [esp+6Eh],80h
 00
004F7398: call 004EF820
004F739D: mov dword ptr [esp+20h],71h
004F73A5: jmp 004F7485
004F73AA: mov eax,dword ptr ds:[00BD00F8h]
004F73AF: movsx ecx,byte ptr [eax+72h]
004F73B3: mov dword ptr [esp+10h],ecx
004F73B7: push esi
004F73B8: push 71h
004F73BA: fild dword ptr [esp+18h]
004F73BE: mov ecx,0B62CB0h
004F73C3: fstp dword ptr [esp+14h]
004F73C7: call 004EF570
004F73CC: test ax,ax
004F73CF: jne 004F7494
004F73D5: push esi
004F73D6: push 72h
004F73D8: mov ecx,0B62CB0h
004F73DD: call 004EF570
004F73E2: test ax,ax
004F73E5: jne 004F7494
004F73EB: push 5
004F73ED: push 8Fh
004F73F2: mov ecx,0B5F8B8h
004F73F7: call 004D88C0
004F73FC: test al,al
004F73FE: je 004F7494
004F7404: mov eax,dword ptr [esi+4]
004F7407: mov ecx,dword ptr [eax+14h]
004F740A: cmp ecx,ebp
004F740C: je 004F7413
004F740E: lea eax,[ecx+30h]
004F7411: jmp 004F7416
004F7413: add eax,4
004F7416: mov edx,dword ptr [esp+0Ch]
004F741A: push ebp
004F741B: push ebp
004F741C: push ebp
004F741D: push ebp
004F741E: push 3F800000h
004F7423: push 3F800000h
004F7428: push 3F800000h
004F742D: push edx
004F742E: mov edx,dword ptr [eax]
004F7430: sub esp,0Ch
004F7433: mov ecx,esp
004F7435: mov dword ptr [ecx],edx
004F7437: mov edx,dword ptr [eax+4]
004F743A: mov eax,dword ptr [eax+8]
004F743D: push esi
004F743E: mov dword ptr [ecx+4],edx
004F7441: push 46h
004F7443: mov dword ptr [ecx+8],eax
004F7446: push 5
004F7448: lea ecx,[esp+4Ch]
004F744C: call 004EFE50
004F7451: mov ecx,dword ptr [esi+4]
004F7454: push ecx
004F7455: lea ecx,[esp+18h]
004F7459: mov dword ptr [esp+34h],3F000000h
004F7461: mov dword ptr [esp+38h],3D4CCCCDh
004F7469: mov dword ptr [esp+30h],40000000h
004F7471: mov word ptr [esp+6Eh],80h
 00
004F7478: call 004EF820
004F747D: mov dword ptr [esp+20h],72h
004F7485: lea edx,[esp+14h]
004F7489: push edx
004F748A: mov ecx,0B62CB0h
004F748F: call 004EFB10
004F7494: mov dword ptr [esp+00000090h],0FFFFFFFFh
004F749F: lea ecx,[esp+14h]
004F74A3: call 004EF660
004F74A8: mov ecx,dword ptr [esp+00000088h]
 00
004F74AF: pop edi
004F74B0: pop esi
004F74B1: mov dword ptr fs:[00000000h],ecx
 00
004F74B8: pop ebp
004F74B9: add esp,88h
004F74BF: ret 8
004F74C2: mov edi,edi
004F74C4: mov bl,64h
004F74C6: dec edi
004F74C7: add byte ptr [ebp+65h],cl
004F74CA: dec edi
004F74CB: add byte ptr [ebp+65FA004Fh],ch
 65
004F74D2: dec edi
004F74D3: add byte ptr [esi+4Fh],al
004F74D7: add byte ptr [ecx+02004F66h],dh
004F74DD: push 6D7F004Fh
004F74E2: dec edi
004F74E3: add byte ptr [ecx+6Eh],cl
004F74E6: dec edi
004F74E7: add al,cl
004F74E9: jo 004F753A
004F74EB: add ch,cl
004F74ED: outs dx,byte ptr [esi]
004F74EE: dec edi
004F74EF: add byte ptr [esi+6Fh],ch
004F74F2: dec edi
004F74F3: add byte ptr [ebx],bl
004F74F5: jo 004F7546
004F74F7: add byte ptr [ecx+71h],ch
004F74FA: dec edi
004F74FB: add dl,cl
004F74FD: jb 004F754E
004F74FF: add byte ptr [edx+0A004F73h],ch
004F7505: jb 004F7556
004F7507: add dl,cl
004F7509: push 6B35004Fh
004F750E: dec edi
004F750F: add dh,al
004F7511: imul ecx,dword ptr [edi],4F6C10h
 00
004F7518: xchg eax,esp
004F7519: je 004F756A
004F751B: add byte ptr [eax],al
004F751D: add byte ptr [eax],al
004F751F: add byte ptr [eax],al
004F7521: add byte ptr [ecx],al
004F7523: add dword ptr [ecx],eax
004F7525: add dword ptr [ecx],eax
004F7527: add dword ptr [edx],eax
004F7529: add eax,dword ptr [edx+15151515h]
 15
004F7530: adc eax,7150605h
004F7535: or byte ptr ds:[0A091515h],dl
004F753B: or ecx,dword ptr [ecx+100F0E15h]
 10
004F7542: adc eax,12111515h
004F7547: adc edx,dword ptr [ecx*4+64CD0049h]
 64
004F754E: dec edi
004F754F: add ah,dl
004F7551: dec edi
004F7553: add dl,ah
004F7555: dec edi
004F7557: add bl,bl
004F7559: dec edi
004F755B: add dh,al
004F755D: dec edi
004F755F: add byte ptr [edi+65h],ah
004F7562: dec edi
004F7563: add byte ptr [esi+65h],ch
004F7566: dec edi
004F7567: add byte ptr [ebp+4Fh],bh
004F756B: add byte ptr [ebp+65h],dh
004F756E: dec edi
004F756F: add byte ptr [eax+65h],ah
004F7572: dec edi
004F7573: add byte ptr [eax+90909090h],dl
004F7580: push ebx
004F7581: push esi
004F7582: push edi
004F7583: push 13h
004F7585: mov esi,ecx
004F7587: push 8Ah
004F758C: mov ecx,0B5F8B8h
004F7591: call 004D88C0
004F7596: test al,al
004F7598: je 004F7661
004F759E: push 2
004F75A0: push 27h
004F75A2: mov ecx,0B5F8B8h
004F75A7: call 004D88C0
004F75AC: test al,al
004F75AE: je 004F7661
004F75B4: cmp dword ptr [esp+10h],5Dh
004F75B9: jne 004F7661
004F75BF: mov eax,dword ptr ds:[00BD00F8h]
004F75C4: movsx ecx,byte ptr [eax+5Dh]
004F75C8: mov ebx,dword ptr [esp+14h]
004F75CC: test ebx,ebx
004F75CE: mov dword ptr [esp+10h],ecx
004F75D2: fild dword ptr [esp+10h]
004F75D6: fstp dword ptr [esp+10h]
004F75DA: je 004F7661
004F75E0: push ebx
004F75E1: mov ecx,esi
004F75E3: call 004F6150
004F75E8: mov eax,dword ptr [ebx+14h]
004F75EB: test eax,eax
004F75ED: je 004F75F4
004F75EF: add eax,30h
004F75F2: jmp 004F75F7
004F75F4: lea eax,[ebx+4]
004F75F7: fadd dword ptr [esp+10h]
004F75FB: mov ecx,dword ptr [eax]
004F75FD: push 0
004F75FF: push 0
004F7601: push 0
004F7603: push 0
004F7605: push 3F800000h
004F760A: push 3F800000h
004F760F: push 3F800000h
004F7614: sub esp,10h
004F7617: mov edx,esp
004F7619: fstp dword ptr [esp+0Ch]
004F761D: mov dword ptr [edx],ecx
004F761F: mov ecx,dword ptr [eax+4]
004F7622: mov eax,dword ptr [eax+8]
004F7625: push esi
004F7626: mov dword ptr [edx+4],ecx
004F7629: push 0Ch
004F762B: lea edi,[esi+8]
004F762E: push 13h
004F7630: mov ecx,edi
004F7632: mov dword ptr [edx+8],eax
004F7635: call 004EFE50
004F763A: push ebx
004F763B: mov ecx,edi
004F763D: mov dword ptr [esi+20h],3F59999Ah
 3F
004F7644: mov word ptr [esi+5Eh],84h
004F764A: call 004EF820
004F764F: push edi
004F7650: mov ecx,0B62CB0h
004F7655: mov dword ptr [esi+14h],5Dh
 00
004F765C: call 004EFB10
004F7661: pop edi
004F7662: pop esi
004F7663: pop ebx
004F7664: ret 8
004F7670: push ecx
004F7671: mov edx,dword ptr [esp+8]
004F7675: push ebx
004F7676: xor ebx,ebx
004F7678: push ebp
004F7679: mov ebp,ecx
004F767B: push esi
004F767C: or esi,0FFFFFFFFh
004F767F: mov dword ptr [ebp+00000144h],ebx
004F7685: mov dword ptr [ebp+4],edx
004F7688: mov byte ptr [ebp+000000A5h],bl
004F768E: mov byte ptr [ebp+000000A6h],bl
004F7694: mov byte ptr [ebp+000000A7h],bl
004F769A: mov byte ptr [ebp+000000A8h],bl
004F76A0: mov byte ptr [ebp+000000A9h],bl
004F76A6: mov byte ptr [ebp+000000AAh],bl
004F76AC: mov dword ptr [ebp+000000ACh],ebx
004F76B2: mov word ptr [ebp+000000E0h],si
 00
004F76B9: mov word ptr [ebp+000000B2h],bx
 00
004F76C0: mov word ptr [ebp+7Ch],bx
004F76C4: mov word ptr [ebp+000000B4h],bx
 00
004F76CB: mov dword ptr [ebp+000000B8h],ebx
004F76D1: mov byte ptr [ebp+000000BCh],bl
004F76D7: mov dword ptr [ebp+000000CCh],ebx
004F76DD: mov dword ptr [ebp+000000D0h],ebx
004F76E3: mov dword ptr [ebp+000000D4h],ebx
004F76E9: mov byte ptr [ebp+00000244h],bl
004F76EF: mov byte ptr [ebp+000000BDh],bl
004F76F5: mov word ptr [ebp+00000148h],si
 00
004F76FC: mov word ptr [ebp+0000014Ah],si
 00
004F7703: mov word ptr [ebp+00000154h],bx
 00
004F770A: mov word ptr [ebp+0000014Eh],bx
 00
004F7711: mov word ptr [ebp+0000014Ch],bx
 00
004F7718: xor eax,eax
004F771A: lea ecx,[ebp+000000E8h]
004F7720: mov word ptr [ecx-4],ax
004F7724: mov dword ptr [ecx],ebx
004F7726: inc eax
004F7727: add ecx,8
004F772A: cmp ax,0Ch
004F772E: jl 004F7720
004F7730: mov eax,0C2C80000h
004F7735: mov dword ptr [ebp+000000C4h],eax
004F773B: mov dword ptr [ebp+00000230h],eax
004F7741: mov eax,0BF800000h
004F7746: mov word ptr [ebp+00000156h],si
 00
004F774D: mov word ptr [ebp+0000015Ch],si
 00
004F7754: mov word ptr [ebp+00000164h],si
 00
004F775B: mov word ptr [ebp+0000016Ch],si
 00
004F7762: mov dword ptr [ebp+00000234h],eax
004F7768: mov dword ptr [ebp+0000022Ch],eax
004F776E: mov dword ptr [ebp+00000248h],eax
004F7774: mov dword ptr [ebp+00000158h],ebx
004F777A: mov dword ptr [ebp+00000160h],ebx
004F7780: mov dword ptr [ebp+00000168h],ebx
004F7786: mov dword ptr [ebp+00000170h],ebx
004F778C: mov dword ptr [ebp+00000178h],ebx
004F7792: mov dword ptr [ebp+0000017Ch],ebx
004F7798: mov dword ptr [ebp+00000180h],ebx
004F779E: mov dword ptr [ebp+00000238h],ebx
004F77A4: mov dword ptr [ebp+0000023Ch],3F800000h
004F77AE: mov dword ptr [ebp+00000240h],ebx
004F77B4: mov ax,word ptr [edx+22h]
004F77B8: movsx eax,ax
004F77BB: sub eax,190h
004F77C0: push edi
004F77C1: lea esi,[eax+eax*8]
004F77C4: lea edi,[ebp+00000080h]
004F77CA: lea esi,[esi*4+00860AF0h]
 00
004F77D1: mov ecx,9
004F77D6: rep movs dword ptr es:[edi],dword ptr [esi]
004F77D8: mov ecx,edx
004F77DA: call 006D8470
004F77DF: mov byte ptr [ebp+000000C8h],al
004F77E5: movsx eax,byte ptr [ebp+0000009Bh]
 00
004F77EC: sub eax,2
004F77EF: pop edi
004F77F0: jne 004F77F9
004F77F2: mov byte ptr [ebp+0000009Ah],0Dh
 0D
004F77F9: mov eax,dword ptr ds:[00BD00F8h]
004F77FE: movsx ecx,byte ptr [eax+4Dh]
004F7802: mov edx,dword ptr [esp+14h]
004F7806: mov dword ptr [esp+0Ch],ecx
004F780A: fild dword ptr [esp+0Ch]
004F780E: fstp dword ptr [ebp+000000D8h]
004F7814: movsx eax,word ptr [edx+22h]
004F7818: add eax,0FFFFFE40h
004F781D: cmp eax,87h
004F7822: ja 004F783B
004F7824: movzx eax,byte ptr [eax+004F7998h]
 00
004F782B: jmp dword ptr [eax*4+004F7990h]
 00
004F7832: mov byte ptr [ebp+000000B0h],1
 01
004F7839: jmp 004F7841
004F783B: mov byte ptr [ebp+000000B0h],bl
004F7841: movsx eax,byte ptr [ebp+00000080h]
 00
004F7848: cmp eax,9
004F784B: ja 004F7987
004F7851: jmp dword ptr [eax*4+004F7A20h]
 00
004F7858: fld dword ptr [ebp+000000D8h]
004F785E: mov cx,word ptr [ebp+00000082h]
 00
004F7865: fsub dword ptr ds:[008CBD50h]
004F786B: mov word ptr [ebp+000000DEh],cx
 00
004F7872: fstp dword ptr [ebp+000000D8h]
004F7878: jmp 004F7912
004F787D: mov dx,word ptr [ebp+00000082h]
 00
004F7884: mov word ptr [ebp+000000DEh],dx
 00
004F788B: jmp 004F7942
004F7890: mov cx,word ptr [ebp+00000082h]
 00
004F7897: mov word ptr [ebp+000000DEh],cx
 00
004F789E: jmp 004F7954
004F78A3: mov ax,word ptr [ebp+00000084h]
 00
004F78AA: cmp ax,0FFFFh
004F78AE: mov dx,word ptr [ebp+00000082h]
 00
004F78B5: mov word ptr [ebp+000000DCh],ax
 00
004F78BC: mov word ptr [ebp+000000DEh],dx
 00
004F78C3: je 004F7980
004F78C9: cmp byte ptr [ebp+000000A4h],bl
004F78CF: jne 004F7980
004F78D5: jmp 004F7970
004F78DA: mov ax,word ptr [ebp+00000084h]
 00
004F78E1: mov cx,word ptr [ebp+00000082h]
 00
004F78E8: pop esi
004F78E9: mov word ptr [ebp+000000DCh],ax
 00
004F78F0: mov word ptr [ebp+000000DEh],cx
 00
004F78F7: mov byte ptr [ebp+000000A4h],1
 01
004F78FE: pop ebp
004F78FF: pop ebx
004F7900: pop ecx
004F7901: ret 4
004F7904: mov dx,word ptr [ebp+00000082h]
 00
004F790B: mov word ptr [ebp+000000DEh],dx
 00
004F7912: cmp byte ptr [ebp+000000A4h],bl
004F7918: mov ax,word ptr [ebp+00000084h]
 00
004F791F: mov word ptr [ebp+000000DCh],ax
 00
004F7926: jne 004F7980
004F7928: cmp ax,0FFFFh
004F792C: je 004F7980
004F792E: cmp ax,81h
004F7932: jmp 004F796E
004F7934: mov cx,word ptr [ebp+00000082h]
 00
004F793B: mov word ptr [ebp+000000DEh],cx
 00
004F7942: fld dword ptr [ebp+000000D8h]
004F7948: fsub dword ptr ds:[008CBD50h]
004F794E: fstp dword ptr [ebp+000000D8h]
004F7954: cmp byte ptr [ebp+000000A4h],bl
004F795A: mov ax,word ptr [ebp+00000084h]
 00
004F7961: mov word ptr [ebp+000000DCh],ax
 00
004F7968: jne 004F7980
004F796A: cmp ax,0FFFFh
004F796E: je 004F7980
004F7970: push eax
004F7971: call 004F4D10
004F7976: add esp,4
004F7979: mov word ptr [ebp+000000E0h],ax
 00
004F7980: mov byte ptr [ebp+000000A4h],1
 01
004F7987: pop esi
004F7988: pop ebp
004F7989: pop ebx
004F798A: pop ecx
004F798B: ret 4
004F798E: mov edi,edi
004F7990: xor bh,byte ptr [eax+4Fh]
004F7993: add byte ptr [ebx],bh
004F7995: js 004F79E6
004F7997: add byte ptr [eax],al
004F7999: add dword ptr [ecx],eax
004F799B: add dword ptr [ecx],eax
004F799D: add dword ptr [ecx],eax
004F799F: add dword ptr [ecx],eax
004F79A1: add byte ptr [ecx],al
004F79A3: add dword ptr [ecx],eax
004F79A5: add dword ptr [eax],eax
004F79A7: add dword ptr [ecx],eax
004F79A9: add dword ptr [ecx],eax
004F79AB: add dword ptr [ecx],eax
004F79AD: add dword ptr [ecx],eax
004F79AF: add dword ptr [ecx],eax
004F79B1: add dword ptr [ecx],eax
004F79B3: add dword ptr [ecx],eax
004F79B5: add dword ptr [ecx],eax
004F79B7: add dword ptr [ecx],eax
004F79B9: add dword ptr [ecx],eax
004F79BB: add dword ptr [ecx],eax
004F79BD: add byte ptr [ecx],al
004F79BF: add dword ptr [ecx],eax
004F79C1: add dword ptr [ecx],eax
004F79C3: add dword ptr [ecx],eax
004F79C5: add dword ptr [ecx],eax
004F79C7: add dword ptr [ecx],eax
004F79C9: add dword ptr [ecx],eax
004F79CB: add dword ptr [ecx],eax
004F79CD: add dword ptr [ecx],eax
004F79CF: add dword ptr [ecx],eax
004F79D1: add dword ptr [ecx],eax
004F79D3: add dword ptr [ecx],eax
004F79D5: add dword ptr [ecx],eax
004F79D7: add dword ptr [ecx],eax
004F79D9: add dword ptr [ecx],eax
004F79DB: add dword ptr [ecx],eax
004F79DD: add dword ptr [ecx],eax
004F79DF: add dword ptr [ecx],eax
004F79E1: add dword ptr [ecx],eax
004F79E3: add dword ptr [ecx],eax
004F79E5: add dword ptr [ecx],eax
004F79E7: add dword ptr [ecx],eax
004F79E9: add dword ptr [eax],eax
004F79EB: add dword ptr [ecx],eax
004F79ED: add dword ptr [ecx],eax
004F79EF: add dword ptr [ecx],eax
004F79F1: add dword ptr [ecx],eax
004F79F3: add byte ptr [ecx],al
004F79F5: add dword ptr [ecx],eax
004F79F7: add dword ptr [ecx],eax
004F79F9: add dword ptr [ecx],eax
004F79FB: add dword ptr [ecx],eax
004F79FD: add dword ptr [ecx],eax
004F79FF: add dword ptr [ecx],eax
004F7A01: add dword ptr [ecx],eax
004F7A03: add dword ptr [ecx],eax
004F7A05: add dword ptr [ecx],eax
004F7A07: add dword ptr [ecx],eax
004F7A09: add dword ptr [ecx],eax
004F7A0B: add dword ptr [ecx],eax
004F7A0D: add dword ptr [ecx],eax
004F7A0F: add dword ptr [ecx],eax
004F7A11: add dword ptr [ecx],eax
004F7A13: add byte ptr [eax],al
004F7A15: add dword ptr [eax],eax
004F7A17: add dword ptr [ecx],eax
004F7A19: add dword ptr [ecx],eax
004F7A1B: add dword ptr [ecx],eax
004F7A1D: add dword ptr [ecx],eax
004F7A1F: add byte ptr [eax+78h],bl
004F7A22: dec edi
004F7A23: add byte ptr [ebp+78h],bh
004F7A26: dec edi
004F7A27: add byte ptr [eax+04004F78h],dl
004F7A2D: jns 004F7A7E
004F7A2F: add dl,bl
004F7A31: js 004F7A82
004F7A33: add byte ptr [ebx+DA004F78h],ah
004F7A39: js 004F7A8A
004F7A3B: add byte ptr [edi+04004F79h],al
004F7A41: jns 004F7A92
004F7A43: add byte ptr [ecx+edi*2],dh
004F7A46: dec edi
004F7A47: add byte ptr [eax+90909090h],dl
004F7A50: push 0FFFFFFFFh
004F7A52: push 83C2A8h
004F7A57: mov eax,dword ptr fs:[00000000h]
004F7A5D: push eax
004F7A5E: mov dword ptr fs:[00000000h],esp
 00
004F7A65: sub esp,80h
004F7A6B: push ebx
004F7A6C: push esi
004F7A6D: push edi
004F7A6E: mov esi,ecx
004F7A70: mov dword ptr [esp+20h],0
004F7A78: mov ax,word ptr [esi+000000E0h]
 00
004F7A7F: cmp ax,0FFFFh
004F7A83: mov dword ptr [esp+00000094h],0
004F7A8E: je 004F7AD4
004F7A90: mov ebx,dword ptr [esp+0000009Ch]
 00
004F7A97: movsx ecx,bx
004F7A9A: lea edi,[ecx-1]
004F7A9D: cmp edi,4
004F7AA0: mov dword ptr [esp+10h],ecx
004F7AA4: ja 004F7AF5
004F7AA6: jmp dword ptr [edi*4+004F7EF8h]
 00
004F7AAD: push eax
004F7AAE: xor eax,eax
004F7AB0: mov ax,word ptr [esi+000000DCh]
 00
004F7AB7: push eax
004F7AB8: jmp 004F7AC6
004F7ABA: xor ecx,ecx
004F7ABC: mov cx,word ptr [esi+000000DEh]
 00
004F7AC3: push 28h
004F7AC5: push ecx
004F7AC6: mov ecx,0B5F8B8h
004F7ACB: call 004D88C0
004F7AD0: test al,al
004F7AD2: jne 004F7AF1
004F7AD4: lea ecx,[esp+18h]
004F7AD8: mov dword ptr [esp+00000094h],0FFFFFFFFh
004F7AE3: call 004EF660
004F7AE8: or ax,0FFFFh
004F7AEC: jmp 004F7EDB
004F7AF1: mov ecx,dword ptr [esp+10h]
004F7AF5: fld dword ptr [esp+000000A4h]
 00
004F7AFC: mov eax,dword ptr [esi+ecx*8+000000E8h]
 00
004F7B03: test eax,eax
004F7B05: fadd dword ptr [esi+000000D8h]
004F7B0B: fstp dword ptr [esp+0Ch]
004F7B0F: je 004F7B1F
004F7B11: cmp bx,5
004F7B15: je 004F7B1F
004F7B17: push ebx
004F7B18: mov ecx,esi
004F7B1A: call 004F55C0
004F7B1F: cmp edi,4
004F7B22: ja 004F7EAA
004F7B28: jmp dword ptr [edi*4+004F7F0Ch]
 00
004F7B2F: mov eax,dword ptr [esi+4]
004F7B32: mov ecx,dword ptr [eax+14h]
004F7B35: test ecx,ecx
004F7B37: je 004F7B3E
004F7B39: lea eax,[ecx+30h]
004F7B3C: jmp 004F7B41
004F7B3E: add eax,4
004F7B41: mov ecx,dword ptr [eax]
004F7B43: push 0
004F7B45: push 0
004F7B47: push 0
004F7B49: push 0
004F7B4B: push 3F800000h
004F7B50: push 3F800000h
004F7B55: push 3F800000h
004F7B5A: push 0
004F7B5C: sub esp,0Ch
004F7B5F: mov edx,esp
004F7B61: mov dword ptr [edx],ecx
004F7B63: mov ecx,dword ptr [eax+4]
004F7B66: mov eax,dword ptr [eax+8]
004F7B69: mov dword ptr [edx+4],ecx
004F7B6C: xor ecx,ecx
004F7B6E: mov cx,word ptr [esi+000000E0h]
 00
004F7B75: push esi
004F7B76: push 0
004F7B78: mov dword ptr [edx+8],eax
004F7B7B: push ecx
004F7B7C: lea ecx,[esp+50h]
004F7B80: call 004EFE50
004F7B85: mov edx,dword ptr [esp+0Ch]
004F7B89: mov eax,dword ptr [esp+000000A0h]
 00
004F7B90: mov ecx,dword ptr ds:[008CBC30h]
004F7B96: mov dword ptr [esp+2Ch],edx
004F7B9A: mov dword ptr [esp+34h],eax
004F7B9E: mov dword ptr [esp+30h],ecx
004F7BA2: jmp 004F7EA3
004F7BA7: mov eax,dword ptr [esi+4]
004F7BAA: mov ecx,dword ptr [eax+14h]
004F7BAD: test ecx,ecx
004F7BAF: je 004F7BB6
004F7BB1: lea eax,[ecx+30h]
004F7BB4: jmp 004F7BB9
004F7BB6: add eax,4
004F7BB9: mov ecx,dword ptr [eax]
004F7BBB: push 0
004F7BBD: push 0
004F7BBF: push 0
004F7BC1: push 0
004F7BC3: push 3F800000h
004F7BC8: push 3F800000h
004F7BCD: push 3F800000h
004F7BD2: push 0
004F7BD4: sub esp,0Ch
004F7BD7: mov edx,esp
004F7BD9: mov dword ptr [edx],ecx
004F7BDB: mov ecx,dword ptr [eax+4]
004F7BDE: mov eax,dword ptr [eax+8]
004F7BE1: mov dword ptr [edx+4],ecx
004F7BE4: xor ecx,ecx
004F7BE6: mov cx,word ptr [esi+000000E0h]
 00
004F7BED: push esi
004F7BEE: push 1
004F7BF0: mov dword ptr [edx+8],eax
004F7BF3: push ecx
004F7BF4: lea ecx,[esp+50h]
004F7BF8: call 004EFE50
004F7BFD: mov edx,dword ptr [esp+0Ch]
004F7C01: mov eax,dword ptr [esp+000000A0h]
 00
004F7C08: mov ecx,dword ptr ds:[008CBC2Ch]
004F7C0E: mov dword ptr [esp+2Ch],edx
004F7C12: mov dword ptr [esp+34h],eax
004F7C16: mov dword ptr [esp+30h],ecx
004F7C1A: jmp 004F7EA3
004F7C1F: mov eax,dword ptr [esi+4]
004F7C22: mov ecx,dword ptr [eax+14h]
004F7C25: test ecx,ecx
004F7C27: je 004F7C2E
004F7C29: lea eax,[ecx+30h]
004F7C2C: jmp 004F7C31
004F7C2E: add eax,4
004F7C31: mov ecx,dword ptr [eax]
004F7C33: push 0
004F7C35: push 0
004F7C37: push 0
004F7C39: push 0
004F7C3B: push 3F800000h
004F7C40: push 3F800000h
004F7C45: push 3F800000h
004F7C4A: push 0
004F7C4C: sub esp,0Ch
004F7C4F: mov edx,esp
004F7C51: mov dword ptr [edx],ecx
004F7C53: mov ecx,dword ptr [eax+4]
004F7C56: mov eax,dword ptr [eax+8]
004F7C59: push esi
004F7C5A: mov dword ptr [edx+4],ecx
004F7C5D: push 1
004F7C5F: push 28h
004F7C61: lea ecx,[esp+50h]
004F7C65: mov dword ptr [edx+8],eax
004F7C68: call 004EFE50
004F7C6D: mov eax,dword ptr ds:[008CBC38h]
004F7C72: jmp 004F7E8C
004F7C77: mov eax,dword ptr [esi+4]
004F7C7A: mov ecx,dword ptr [eax+14h]
004F7C7D: test ecx,ecx
004F7C7F: je 004F7C86
004F7C81: lea eax,[ecx+30h]
004F7C84: jmp 004F7C89
004F7C86: add eax,4
004F7C89: mov edx,dword ptr [eax]
004F7C8B: push 0
004F7C8D: push 0
004F7C8F: push 0
004F7C91: push 0
004F7C93: push 3F800000h
004F7C98: push 3F800000h
004F7C9D: push 3F800000h
004F7CA2: push 0
004F7CA4: sub esp,0Ch
004F7CA7: mov ecx,esp
004F7CA9: mov dword ptr [ecx],edx
004F7CAB: mov edx,dword ptr [eax+4]
004F7CAE: mov eax,dword ptr [eax+8]
004F7CB1: push esi
004F7CB2: mov dword ptr [ecx+4],edx
004F7CB5: push 0
004F7CB7: mov dword ptr [ecx+8],eax
004F7CBA: push 28h
004F7CBC: lea ecx,[esp+50h]
004F7CC0: call 004EFE50
004F7CC5: mov edx,dword ptr [esp+000000A0h]
 00
004F7CCC: mov eax,dword ptr ds:[008CBC34h]
004F7CD1: mov ecx,dword ptr [esp+0Ch]
004F7CD5: mov dword ptr [esp+34h],edx
004F7CD9: mov edx,dword ptr ds:[008CBCCCh]
004F7CDF: mov dword ptr [esp+30h],eax
004F7CE3: mov eax,dword ptr [esi+00000150h]
004F7CE9: mov dword ptr [esp+2Ch],ecx
004F7CED: mov word ptr [esi+0000014Ch],0
004F7CF6: mov ecx,dword ptr ds:[00B7CB84h]
004F7CFC: lea edi,[edx+eax]
004F7CFF: cmp edi,ecx
004F7D01: mov word ptr [esp+6Eh],24h
 00
004F7D08: jbe 004F7EAA
004F7D0E: sub ecx,eax
004F7D10: test ecx,ecx
004F7D12: mov dword ptr [esp+14h],ecx
004F7D16: fild dword ptr [esp+14h]
004F7D1A: jge 004F7D22
004F7D1C: fadd dword ptr ds:[00858C54h]
004F7D22: test edx,edx
004F7D24: fstp dword ptr [esp+0Ch]
004F7D28: mov dword ptr [esp+14h],edx
004F7D2C: fild dword ptr [esp+14h]
004F7D30: jge 004F7D38
004F7D32: fadd dword ptr ds:[00858C54h]
004F7D38: fld dword ptr [esp+0Ch]
004F7D3C: fcomp st(1)
004F7D3E: fnstsw ax
004F7D40: test ah,5
004F7D43: jp 004F7D4B
004F7D45: fld dword ptr [esp+0Ch]
004F7D49: jmp 004F7D4D
004F7D4B: fld st(0)
004F7D4D: fcomp qword ptr ds:[00859EF8h]
004F7D53: fnstsw ax
004F7D55: test ah,5
004F7D58: jp 004F7D62
004F7D5A: fld qword ptr ds:[00859EF8h]
004F7D60: jmp 004F7D77
004F7D62: fld dword ptr [esp+0Ch]
004F7D66: fcomp st(1)
004F7D68: fnstsw ax
004F7D6A: test ah,5
004F7D6D: jp 004F7D75
004F7D6F: fld dword ptr [esp+0Ch]
004F7D73: jmp 004F7D77
004F7D75: fld st(0)
004F7D77: movsx edx,word ptr ds:[008CBCC8h]
 00
004F7D7E: mov cx,word ptr [esi+0000014Eh]
 00
004F7D85: movsx eax,cx
004F7D88: fdivrp st(1),st
004F7D8A: mov dword ptr [esp+14h],edx
004F7D8E: fld dword ptr ds:[00858624h]
004F7D94: fsub st,st(1)
004F7D96: fstp dword ptr [esp+0Ch]
004F7D9A: fstp st(0)
004F7D9C: fild dword ptr [esp+14h]
004F7DA0: mov dword ptr [esp+14h],eax
004F7DA4: fild dword ptr [esp+14h]
004F7DA8: fmul dword ptr [esp+0Ch]
004F7DAC: fcomp st(1)
004F7DAE: fnstsw ax
004F7DB0: test ah,5
004F7DB3: jp 004F7DC6
004F7DB5: movsx edx,cx
004F7DB8: mov dword ptr [esp+14h],edx
004F7DBC: fild dword ptr [esp+14h]
004F7DC0: fmul dword ptr [esp+0Ch]
004F7DC4: jmp 004F7DC8
004F7DC6: fld st(0)
004F7DC8: fld dword ptr ds:[00858B50h]
004F7DCE: fcomp st(1)
004F7DD0: fnstsw ax
004F7DD2: fstp st(0)
004F7DD4: test ah,41h
004F7DD7: jne 004F7DE3
004F7DD9: fstp st(0)
004F7DDB: fld dword ptr ds:[00858B50h]
004F7DE1: jmp 004F7E0C
004F7DE3: movsx eax,cx
004F7DE6: mov dword ptr [esp+14h],eax
004F7DEA: fild dword ptr [esp+14h]
004F7DEE: fmul dword ptr [esp+0Ch]
004F7DF2: fcomp st(1)
004F7DF4: fnstsw ax
004F7DF6: test ah,5
004F7DF9: jp 004F7E0C
004F7DFB: movsx ecx,cx
004F7DFE: fstp st(0)
004F7E00: mov dword ptr [esp+14h],ecx
004F7E04: fild dword ptr [esp+14h]
004F7E08: fmul dword ptr [esp+0Ch]
004F7E0C: fld st(0)
004F7E0E: call 00821B40
004F7E13: fcomp dword ptr ds:[0085862Ch]
004F7E19: mov word ptr [esp+74h],ax
004F7E1E: fnstsw ax
004F7E20: test ah,41h
004F7E23: jne 004F7EAA
004F7E29: mov edx,dword ptr ds:[008CBCC0h]
004F7E2F: inc edx
004F7E30: mov word ptr [esi+0000014Ch],dx
 00
004F7E37: jmp 004F7EAA
004F7E39: mov eax,dword ptr [esi+4]
004F7E3C: mov ecx,dword ptr [eax+14h]
004F7E3F: test ecx,ecx
004F7E41: je 004F7E48
004F7E43: lea eax,[ecx+30h]
004F7E46: jmp 004F7E4B
004F7E48: add eax,4
004F7E4B: mov edx,dword ptr [eax]
004F7E4D: push 0
004F7E4F: push 0
004F7E51: push 0
004F7E53: push 0
004F7E55: push 3F800000h
004F7E5A: push 3F800000h
004F7E5F: push 3F800000h
004F7E64: push 0
004F7E66: sub esp,0Ch
004F7E69: mov ecx,esp
004F7E6B: mov dword ptr [ecx],edx
004F7E6D: mov edx,dword ptr [eax+4]
004F7E70: mov eax,dword ptr [eax+8]
004F7E73: push esi
004F7E74: mov dword ptr [ecx+4],edx
004F7E77: push 2
004F7E79: mov dword ptr [ecx+8],eax
004F7E7C: push 28h
004F7E7E: lea ecx,[esp+50h]
004F7E82: call 004EFE50
004F7E87: mov eax,dword ptr ds:[008CBC3Ch]
004F7E8C: mov ecx,dword ptr [esp+0Ch]
004F7E90: mov edx,dword ptr [esp+000000A0h]
 00
004F7E97: mov dword ptr [esp+2Ch],ecx
004F7E9B: mov dword ptr [esp+34h],edx
004F7E9F: mov dword ptr [esp+30h],eax
004F7EA3: mov word ptr [esp+6Eh],4
 00
004F7EAA: lea ecx,[esp+18h]
004F7EAE: push ecx
004F7EAF: mov ecx,0B62CB0h
004F7EB4: call 004EFB10
004F7EB9: mov edx,dword ptr [esp+10h]
004F7EBD: lea ecx,[esp+18h]
004F7EC1: mov dword ptr [esi+edx*8+000000E8h],eax
 00
004F7EC8: mov dword ptr [esp+00000094h],0FFFFFFFFh
004F7ED3: call 004EF660
004F7ED8: xor ax,ax
004F7EDB: mov ecx,dword ptr [esp+0000008Ch]
 00
004F7EE2: pop edi
004F7EE3: pop esi
004F7EE4: mov dword ptr fs:[00000000h],ecx
 00
004F7EEB: pop ebx
004F7EEC: add esp,8Ch
004F7EF2: ret 0Ch
004F7EF5: lea ecx,[ecx]
004F7EF8: lods dword ptr [esi]
004F7EF9: jp 004F7F4A
004F7EFB: add byte ptr [ebp+BA004F7Ah],ch
004F7F01: jp 004F7F52
004F7F03: add byte ptr [edx+BA004F7Ah],bh
004F7F09: jp 004F7F5A
004F7F0B: add byte ptr [edi],ch
004F7F0D: jnp 004F7F5E
004F7F0F: add byte ptr [edi+1F004F7Bh],ah
004F7F15: jl 004F7F66
004F7F17: add byte ptr [edi+7Ch],dh
004F7F1A: dec edi
004F7F1B: add byte ptr [ecx],bh
004F7F1D: jle 004F7F6E
004F7F1F: add byte ptr [edx-1],ch
004F7F22: push 83C2C8h
004F7F27: mov eax,dword ptr fs:[00000000h]
004F7F2D: push eax
004F7F2E: mov dword ptr fs:[00000000h],esp
 00
004F7F35: sub esp,78h
004F7F38: push ebx
004F7F39: push esi
004F7F3A: xor ebx,ebx
004F7F3C: push edi
004F7F3D: mov esi,ecx
004F7F3F: mov dword ptr [esp+18h],ebx
004F7F43: fld dword ptr [esp+0000009Ch]
 00
004F7F4A: movsx edi,word ptr [esp+00000094h]
004F7F52: fadd dword ptr [esi+000000D8h]
004F7F58: mov eax,dword ptr [esi+edi*8+000000E8h]
 00
004F7F5F: cmp eax,ebx
004F7F61: mov dword ptr [esp+0000008Ch],ebx
 00
004F7F68: je 004F7F7C
004F7F6A: mov ecx,dword ptr [esp+00000098h]
 00
004F7F71: fstp dword ptr [eax+14h]
004F7F74: mov dword ptr [eax+1Ch],ecx
004F7F77: jmp 004F8036
004F7F7C: mov eax,edi
004F7F7E: fstp dword ptr [esp+0000009Ch]
 00
004F7F85: dec eax
004F7F86: je 004F7F9F
004F7F88: dec eax
004F7F89: jne 004F8036
004F7F8F: mov eax,dword ptr ds:[008CBC2Ch]
004F7F94: mov edx,1
004F7F99: mov dword ptr [esp+0Ch],eax
004F7F9D: jmp 004F7FAB
004F7F9F: mov ecx,dword ptr ds:[008CBC30h]
004F7FA5: xor edx,edx
004F7FA7: mov dword ptr [esp+0Ch],ecx
004F7FAB: mov eax,dword ptr [esi+4]
004F7FAE: mov ecx,dword ptr [eax+14h]
004F7FB1: cmp ecx,ebx
004F7FB3: je 004F7FBA
004F7FB5: lea eax,[ecx+30h]
004F7FB8: jmp 004F7FBD
004F7FBA: add eax,4
004F7FBD: push ebx
004F7FBE: push ebx
004F7FBF: push ebx
004F7FC0: push ebx
004F7FC1: push 3F800000h
004F7FC6: push 3F800000h
004F7FCB: push 3F800000h
004F7FD0: push ebx
004F7FD1: mov ebx,dword ptr [eax]
004F7FD3: sub esp,0Ch
004F7FD6: mov ecx,esp
004F7FD8: mov dword ptr [ecx],ebx
004F7FDA: mov ebx,dword ptr [eax+4]
004F7FDD: mov eax,dword ptr [eax+8]
004F7FE0: mov dword ptr [ecx+4],ebx
004F7FE3: mov dword ptr [ecx+8],eax
004F7FE6: xor ecx,ecx
004F7FE8: mov cx,word ptr [esi+000000E0h]
 00
004F7FEF: push esi
004F7FF0: push edx
004F7FF1: push ecx
004F7FF2: lea ecx,[esp+48h]
004F7FF6: call 004EFE50
004F7FFB: mov edx,dword ptr [esp+0000009Ch]
 00
004F8002: mov ecx,dword ptr [esp+0Ch]
004F8006: mov eax,dword ptr [esp+00000098h]
 00
004F800D: mov dword ptr [esp+24h],edx
004F8011: lea edx,[esp+10h]
004F8015: mov dword ptr [esp+28h],ecx
004F8019: push edx
004F801A: mov ecx,0B62CB0h
004F801F: mov dword ptr [esp+30h],eax
004F8023: mov word ptr [esp+6Ah],4
 00
004F802A: call 004EFB10
004F802F: mov dword ptr [esi+edi*8+000000E8h],eax
 00
004F8036: lea ecx,[esp+10h]
004F803A: mov dword ptr [esp+0000008Ch],0FFFFFFFFh
004F8045: call 004EF660
004F804A: mov ecx,dword ptr [esp+00000084h]
 00
004F8051: pop edi
004F8052: pop esi
004F8053: mov dword ptr fs:[00000000h],ecx
 00
004F805A: pop ebx
004F805B: add esp,84h
004F8061: ret 0Ch
004F8070: sub esp,8
004F8073: mov edx,dword ptr [esp+0Ch]
004F8077: fld dword ptr [edx+1Ch]
004F807A: push esi
004F807B: fcomp dword ptr ds:[00862CF0h]
004F8081: mov esi,ecx
004F8083: mov dword ptr [esp+4],0
004F808B: fnstsw ax
004F808D: test ah,5
004F8090: jp 004F8096
004F8092: mov cl,1
004F8094: jmp 004F8098
004F8096: xor cl,cl
004F8098: fld dword ptr [edx+28h]
004F809B: fcomp dword ptr ds:[00858B50h]
004F80A1: fld dword ptr [edx+28h]
004F80A4: fnstsw ax
004F80A6: test ah,41h
004F80A9: jne 004F80E1
004F80AB: fcomp dword ptr ds:[008CBD40h]
004F80B1: fnstsw ax
004F80B3: test ah,5
004F80B6: jp 004F80CB
004F80B8: fld dword ptr [edx+28h]
004F80BB: fdiv dword ptr ds:[008CBD40h]
004F80C1: fmul dword ptr ds:[008CBD3Ch]
004F80C7: fchs
004F80C9: jmp 004F8105
004F80CB: fld dword ptr ds:[008CBD40h]
004F80D1: fdiv dword ptr ds:[008CBD40h]
004F80D7: fmul dword ptr ds:[008CBD3Ch]
004F80DD: fchs
004F80DF: jmp 004F8105
004F80E1: fcomp dword ptr ds:[008CBD44h]
004F80E7: fnstsw ax
004F80E9: test ah,41h
004F80EC: jne 004F80F3
004F80EE: fld dword ptr [edx+28h]
004F80F1: jmp 004F80F9
004F80F3: fld dword ptr ds:[008CBD44h]
004F80F9: fdiv dword ptr ds:[008CBD44h]
004F80FF: fmul dword ptr ds:[008CBD3Ch]
004F8105: test cl,cl
004F8107: je 004F810B
004F8109: fchs
004F810B: mov eax,dword ptr [edx+14h]
004F810E: fld dword ptr [eax+10h]
004F8111: fdivr dword ptr [edx+24h]
004F8114: fst dword ptr [esp+8]
004F8118: fcomp qword ptr ds:[0085A310h]
004F811E: fnstsw ax
004F8120: test ah,41h
004F8123: jne 004F812D
004F8125: fld qword ptr ds:[0085A310h]
004F812B: jmp 004F8131
004F812D: fld dword ptr [esp+8]
004F8131: mov al,byte ptr [esi+00000080h]
004F8137: fxch st(1)
004F8139: cmp al,1
004F813B: fmul st,st(1)
004F813D: fstp dword ptr [esp+10h]
004F8141: fstp st(0)
004F8143: jne 004F819C
004F8145: mov ecx,dword ptr [edx+10h]
004F8148: fld dword ptr [ecx+00000648h]
004F814E: fsin
004F8150: fcom dword ptr ds:[00858B50h]
004F8156: fnstsw ax
004F8158: test ah,5
004F815B: jp 004F815F
004F815D: fchs
004F815F: mov al,byte ptr [ecx+00000614h]
004F8165: fmul dword ptr ds:[008CBD6Ch]
004F816B: test al,20h
004F816D: je 004F8175
004F816F: fadd dword ptr ds:[008CBD70h]
004F8175: fld dword ptr [esp+8]
004F8179: fcomp qword ptr ds:[0085A310h]
004F817F: fnstsw ax
004F8181: test ah,41h
004F8184: jne 004F818E
004F8186: fld qword ptr ds:[0085A310h]
004F818C: jmp 004F8192
004F818E: fld dword ptr [esp+8]
004F8192: fxch st(1)
004F8194: fmul st,st(1)
004F8196: fstp dword ptr [esp+4]
004F819A: fstp st(0)
004F819C: movsx eax,word ptr [esp+14h]
004F81A1: dec eax
004F81A2: cmp eax,4
004F81A5: ja 004F82E1
004F81AB: jmp dword ptr [eax*4+004F8344h]
 00
004F81B2: fld dword ptr [edx+40h]
004F81B5: fcomp dword ptr ds:[00858C80h]
004F81BB: fld dword ptr [edx+40h]
004F81BE: fnstsw ax
004F81C0: test ah,41h
004F81C3: jne 004F81E8
004F81C5: fsub dword ptr ds:[00858C80h]
004F81CB: fmul dword ptr ds:[00858CC4h]
004F81D1: fmul dword ptr ds:[00B6BA68h]
004F81D7: fadd dword ptr ds:[008CBCB0h]
004F81DD: fadd dword ptr ds:[008CBCA4h]
004F81E3: jmp 004F82C7
004F81E8: fmul dword ptr ds:[00858CC4h]
004F81EE: fmul dword ptr ds:[008CBCACh]
004F81F4: fadd dword ptr ds:[008CBCA4h]
004F81FA: jmp 004F82C7
004F81FF: mov ecx,dword ptr [edx+2Ch]
004F8202: push ecx
004F8203: mov ecx,esi
004F8205: call 004F5C60
004F820A: fmul dword ptr ds:[00B6BA40h]
004F8210: mov edx,dword ptr [edx+10h]
004F8213: cmp word ptr [edx+22h],1C9h
004F8219: fadd dword ptr ds:[008CBC70h]
004F821F: fld dword ptr [esp+10h]
004F8223: fadd dword ptr ds:[00858624h]
004F8229: fmulp st(1),st
004F822B: fld dword ptr [esp+4]
004F822F: fadd dword ptr ds:[00858624h]
004F8235: fmulp st(1),st
004F8237: jne 004F82E5
004F823D: fstp st(0)
004F823F: fld dword ptr ds:[00858624h]
004F8245: jmp 004F82E5
004F824A: fld dword ptr [edx+40h]
004F824D: mov al,byte ptr [esi+000000B0h]
004F8253: test al,al
004F8255: fmul dword ptr ds:[00858CC4h]
004F825B: fmul dword ptr ds:[008CBCB8h]
004F8261: fld dword ptr ds:[00B6BA50h]
004F8267: fmul dword ptr [esi+000000ACh]
004F826D: faddp st(1),st
004F826F: je 004F8279
004F8271: fadd dword ptr ds:[00B6BA58h]
004F8277: jmp 004F82C7
004F8279: fadd dword ptr ds:[008CBC80h]
004F827F: jmp 004F82C7
004F8281: fld dword ptr [edx+40h]
004F8284: mov al,byte ptr [esi+000000B0h]
004F828A: test al,al
004F828C: fmul dword ptr ds:[00858CC4h]
004F8292: fmul dword ptr ds:[008CBCB8h]
004F8298: je 004F82A2
004F829A: fadd dword ptr ds:[008CBC6Ch]
004F82A0: jmp 004F82C7
004F82A2: movzx eax,byte ptr [esi+000000AAh]
 00
004F82A9: fadd dword ptr [eax*4+008CC1C0h]
 00
004F82B0: fadd dword ptr ds:[008CBC60h]
004F82B6: jmp 004F82C7
004F82B8: fld dword ptr ds:[00B6BA5Ch]
004F82BE: fmul dword ptr [edx+2Ch]
004F82C1: fadd dword ptr ds:[008CBC94h]
004F82C7: fld dword ptr [esp+10h]
004F82CB: fadd dword ptr ds:[00858624h]
004F82D1: fmulp st(1),st
004F82D3: fld dword ptr [esp+4]
004F82D7: fadd dword ptr ds:[00858624h]
004F82DD: fmulp st(1),st
004F82DF: jmp 004F82E5
004F82E1: fld dword ptr [esp+14h]
004F82E5: mov ecx,dword ptr [esi+4]
004F82E8: test byte ptr [ecx+0000042Bh],40h
 40
004F82EF: je 004F82F7
004F82F1: fmul dword ptr ds:[008CBC48h]
004F82F7: mov al,byte ptr [esi+00000244h]
004F82FD: test al,al
004F82FF: je 004F833B
004F8301: fld dword ptr [esi+00000248h]
004F8307: fcomp dword ptr ds:[00858624h]
004F830D: fnstsw ax
004F830F: test ah,41h
004F8312: jp 004F833B
004F8314: fld dword ptr [esi+00000248h]
004F831A: fcomp dword ptr ds:[00858B50h]
004F8320: fnstsw ax
004F8322: test ah,1
004F8325: jne 004F833B
004F8327: fld dword ptr ds:[008CBC50h]
004F832D: fmul dword ptr [esi+00000248h]
004F8333: fadd dword ptr ds:[00858624h]
004F8339: fmulp st(1),st
004F833B: pop esi
004F833C: add esp,8
004F833F: ret 8
004F8342: mov edi,edi
004F8344: mov dl,81h
004F8346: dec edi
004F8347: add bh,bh
004F8349: or dword ptr [edi],4F824Ah
 00
004F8350: add dword ptr [edx+82B8004Fh],9090004Fh
004F8360: push 0FFFFFFFFh
004F8362: push 83C2E8h
004F8367: mov eax,dword ptr fs:[00000000h]
004F836D: push eax
004F836E: mov dword ptr fs:[00000000h],esp
 00
004F8375: sub esp,78h
004F8378: push ebx
004F8379: push esi
004F837A: push edi
004F837B: mov esi,ecx
004F837D: mov dword ptr [esp+18h],0
004F8385: fld dword ptr [esp+0000009Ch]
 00
004F838C: fadd dword ptr [esi+000000D8h]
004F8392: mov ax,word ptr [esi+00000156h]
 00
004F8399: mov edi,dword ptr [esp+00000094h]
 00
004F83A0: cmp ax,di
004F83A3: fstp dword ptr [esp+0Ch]
004F83A7: mov dword ptr [esp+0000008Ch],0
004F83B2: jne 004F8407
004F83B4: or edi,0FFFFFFFFh
004F83B7: cmp ax,di
004F83BA: je 004F83FB
004F83BC: mov eax,dword ptr [esi+4]
004F83BF: mov ecx,dword ptr [eax+14h]
004F83C2: test ecx,ecx
004F83C4: je 004F83CB
004F83C6: lea eax,[ecx+30h]
004F83C9: jmp 004F83CE
004F83CB: add eax,4
004F83CE: mov ecx,dword ptr [esp+00000098h]
 00
004F83D5: mov edx,dword ptr [esp+0Ch]
004F83D9: push ecx
004F83DA: push edx
004F83DB: mov edx,dword ptr [eax]
004F83DD: sub esp,0Ch
004F83E0: mov ecx,esp
004F83E2: mov dword ptr [ecx],edx
004F83E4: mov edx,dword ptr [eax+4]
004F83E7: mov eax,dword ptr [eax+8]
004F83EA: mov dword ptr [ecx+4],edx
004F83ED: mov dword ptr [ecx+8],eax
004F83F0: lea ecx,[esi+00000184h]
004F83F6: call 004F29A0
004F83FB: mov dword ptr [esp+0000008Ch],edi
 00
004F8402: jmp 004F84A0
004F8407: cmp word ptr [esi+0000020Ch],0
004F840F: je 004F841C
004F8411: lea ecx,[esi+00000184h]
004F8417: call 004F2960
004F841C: cmp di,0FFFFh
004F8420: mov word ptr [esi+00000156h],di
 00
004F8427: je 004F8495
004F8429: push 0FFFFFFFFh
004F842B: push 0FFFFFFFFh
004F842D: push 3E8h
004F8432: push 0C8h
004F8437: push esi
004F8438: lea ecx,[edi+1]
004F843B: push ecx
004F843C: push edi
004F843D: lea ebx,[esi+00000184h]
004F8443: push 13h
004F8445: mov ecx,ebx
004F8447: call 004F28A0
004F844C: mov esi,dword ptr [esi+4]
004F844F: mov eax,dword ptr [esi+14h]
004F8452: test eax,eax
004F8454: je 004F845B
004F8456: add eax,30h
004F8459: jmp 004F845E
004F845B: lea eax,[esi+4]
004F845E: mov edx,dword ptr ds:[008CBCFCh]
004F8464: mov ecx,dword ptr [esp+00000098h]
 00
004F846B: push 0
004F846D: push 3F800000h
004F8472: push edx
004F8473: mov edx,dword ptr [esp+18h]
004F8477: push ecx
004F8478: push edx
004F8479: mov edx,dword ptr [eax]
004F847B: sub esp,0Ch
004F847E: mov ecx,esp
004F8480: mov dword ptr [ecx],edx
004F8482: mov edx,dword ptr [eax+4]
004F8485: mov eax,dword ptr [eax+8]
004F8488: mov dword ptr [ecx+4],edx
004F848B: mov dword ptr [ecx+8],eax
004F848E: mov ecx,ebx
004F8490: call 004F2CB0
004F8495: mov dword ptr [esp+0000008Ch],0FFFFFFFFh
004F84A0: lea ecx,[esp+10h]
004F84A4: call 004EF660
004F84A9: mov ecx,dword ptr [esp+00000084h]
 00
004F84B0: pop edi
004F84B1: pop esi
004F84B2: mov dword ptr fs:[00000000h],ecx
 00
004F84B9: pop ebx
004F84BA: add esp,84h
004F84C0: ret 0Ch
004F84D0: push 0FFFFFFFFh
004F84D2: push 83C308h
004F84D7: mov eax,dword ptr fs:[00000000h]
004F84DD: push eax
004F84DE: mov dword ptr fs:[00000000h],esp
 00
004F84E5: sub esp,78h
004F84E8: push ebx
004F84E9: push esi
004F84EA: push edi
004F84EB: mov esi,ecx
004F84ED: mov dword ptr [esp+18h],0
004F84F5: fld dword ptr [esp+0000009Ch]
 00
004F84FC: fadd dword ptr [esi+000000D8h]
004F8502: mov ax,word ptr [esi+0000015Ch]
 00
004F8509: mov edi,dword ptr [esp+00000094h]
 00
004F8510: cmp ax,di
004F8513: fstp dword ptr [esp+0Ch]
004F8517: mov dword ptr [esp+0000008Ch],0
004F8522: jne 004F8561
004F8524: or ebx,0FFFFFFFFh
004F8527: cmp ax,bx
004F852A: je 004F8621
004F8530: mov eax,dword ptr [esi+00000160h]
004F8536: test eax,eax
004F8538: mov dword ptr [esp+0000008Ch],ebx
 00
004F853F: je 004F8628
004F8545: mov ecx,dword ptr [esp+00000098h]
 00
004F854C: mov edx,dword ptr [esp+0Ch]
004F8550: mov dword ptr [eax+1Ch],ecx
004F8553: mov esi,dword ptr [esi+00000160h]
004F8559: mov dword ptr [esi+14h],edx
004F855C: jmp 004F8628
004F8561: mov ecx,dword ptr [esi+00000160h]
004F8567: test ecx,ecx
004F8569: je 004F8589
004F856B: push 0
004F856D: push 4
004F856F: call 004EF2B0
004F8574: mov ecx,dword ptr [esi+00000160h]
004F857A: call 004EF1C0
004F857F: mov dword ptr [esi+00000160h],0
004F8589: or ebx,0FFFFFFFFh
004F858C: cmp di,bx
004F858F: mov word ptr [esi+0000015Ch],di
 00
004F8596: je 004F8621
004F859C: mov eax,dword ptr [esi+4]
004F859F: mov ecx,dword ptr [eax+14h]
004F85A2: test ecx,ecx
004F85A4: je 004F85AB
004F85A6: lea eax,[ecx+30h]
004F85A9: jmp 004F85AE
004F85AB: add eax,4
004F85AE: mov edx,dword ptr [eax]
004F85B0: push 0
004F85B2: push 0
004F85B4: push 0
004F85B6: push 0
004F85B8: push 3F800000h
004F85BD: push 3F800000h
004F85C2: push 3F800000h
004F85C7: push 0
004F85C9: sub esp,0Ch
004F85CC: mov ecx,esp
004F85CE: mov dword ptr [ecx],edx
004F85D0: mov edx,dword ptr [eax+4]
004F85D3: mov eax,dword ptr [eax+8]
004F85D6: push esi
004F85D7: mov dword ptr [ecx+4],edx
004F85DA: push edi
004F85DB: mov dword ptr [ecx+8],eax
004F85DE: push 13h
004F85E0: lea ecx,[esp+48h]
004F85E4: call 004EFE50
004F85E9: mov ecx,dword ptr [esp+0Ch]
004F85ED: mov edx,dword ptr [esp+00000098h]
 00
004F85F4: mov eax,dword ptr ds:[008CBD14h]
004F85F9: mov dword ptr [esp+24h],ecx
004F85FD: lea ecx,[esp+10h]
004F8601: push ecx
004F8602: mov ecx,0B62CB0h
004F8607: mov dword ptr [esp+30h],edx
004F860B: mov word ptr [esp+6Ah],4
 00
004F8612: mov dword ptr [esp+2Ch],eax
004F8616: call 004EFB10
004F861B: mov dword ptr [esi+00000160h],eax
004F8621: mov dword ptr [esp+0000008Ch],ebx
 00
004F8628: lea ecx,[esp+10h]
004F862C: call 004EF660
004F8631: mov ecx,dword ptr [esp+00000084h]
 00
004F8638: pop edi
004F8639: pop esi
004F863A: mov dword ptr fs:[00000000h],ecx
 00
004F8641: pop ebx
004F8642: add esp,84h
004F8648: ret 0Ch
004F8650: push 0FFFFFFFFh
004F8652: push 83C328h
004F8657: mov eax,dword ptr fs:[00000000h]
004F865D: push eax
004F865E: mov dword ptr fs:[00000000h],esp
 00
004F8665: sub esp,78h
004F8668: push ebx
004F8669: push esi
004F866A: push edi
004F866B: mov esi,ecx
004F866D: mov dword ptr [esp+18h],0
004F8675: fld dword ptr [esp+0000009Ch]
 00
004F867C: fadd dword ptr [esi+000000D8h]
004F8682: mov ax,word ptr [esi+00000164h]
 00
004F8689: mov edi,dword ptr [esp+00000094h]
 00
004F8690: cmp ax,di
004F8693: fstp dword ptr [esp+0Ch]
004F8697: mov dword ptr [esp+0000008Ch],0
004F86A2: jne 004F86E1
004F86A4: or ebx,0FFFFFFFFh
004F86A7: cmp ax,bx
004F86AA: je 004F87A1
004F86B0: mov eax,dword ptr [esi+00000168h]
004F86B6: test eax,eax
004F86B8: mov dword ptr [esp+0000008Ch],ebx
 00
004F86BF: je 004F87A8
004F86C5: mov ecx,dword ptr [esp+00000098h]
 00
004F86CC: mov edx,dword ptr [esp+0Ch]
004F86D0: mov dword ptr [eax+1Ch],ecx
004F86D3: mov esi,dword ptr [esi+00000168h]
004F86D9: mov dword ptr [esi+14h],edx
004F86DC: jmp 004F87A8
004F86E1: mov ecx,dword ptr [esi+00000168h]
004F86E7: test ecx,ecx
004F86E9: je 004F8709
004F86EB: push 0
004F86ED: push 4
004F86EF: call 004EF2B0
004F86F4: mov ecx,dword ptr [esi+00000168h]
004F86FA: call 004EF1C0
004F86FF: mov dword ptr [esi+00000168h],0
004F8709: or ebx,0FFFFFFFFh
004F870C: cmp di,bx
004F870F: mov word ptr [esi+00000164h],di
 00
004F8716: je 004F87A1
004F871C: mov eax,dword ptr [esi+4]
004F871F: mov ecx,dword ptr [eax+14h]
004F8722: test ecx,ecx
004F8724: je 004F872B
004F8726: lea eax,[ecx+30h]
004F8729: jmp 004F872E
004F872B: add eax,4
004F872E: mov edx,dword ptr [eax]
004F8730: push 0
004F8732: push 0
004F8734: push 0
004F8736: push 0
004F8738: push 3F800000h
004F873D: push 3F800000h
004F8742: push 3F800000h
004F8747: push 0
004F8749: sub esp,0Ch
004F874C: mov ecx,esp
004F874E: mov dword ptr [ecx],edx
004F8750: mov edx,dword ptr [eax+4]
004F8753: mov eax,dword ptr [eax+8]
004F8756: push esi
004F8757: mov dword ptr [ecx+4],edx
004F875A: push edi
004F875B: mov dword ptr [ecx+8],eax
004F875E: push 13h
004F8760: lea ecx,[esp+48h]
004F8764: call 004EFE50
004F8769: mov ecx,dword ptr [esp+0Ch]
004F876D: mov edx,dword ptr [esp+00000098h]
 00
004F8774: mov eax,dword ptr ds:[008CBD20h]
004F8779: mov dword ptr [esp+24h],ecx
004F877D: lea ecx,[esp+10h]
004F8781: push ecx
004F8782: mov ecx,0B62CB0h
004F8787: mov dword ptr [esp+30h],edx
004F878B: mov word ptr [esp+6Ah],4
 00
004F8792: mov dword ptr [esp+2Ch],eax
004F8796: call 004EFB10
004F879B: mov dword ptr [esi+00000168h],eax
004F87A1: mov dword ptr [esp+0000008Ch],ebx
 00
004F87A8: lea ecx,[esp+10h]
004F87AC: call 004EF660
004F87B1: mov ecx,dword ptr [esp+00000084h]
 00
004F87B8: pop edi
004F87B9: pop esi
004F87BA: mov dword ptr fs:[00000000h],ecx
 00
004F87C1: pop ebx
004F87C2: add esp,84h
004F87C8: ret 0Ch
004F87D0: push 0FFFFFFFFh
004F87D2: push 83C348h
004F87D7: mov eax,dword ptr fs:[00000000h]
004F87DD: push eax
004F87DE: mov dword ptr fs:[00000000h],esp
 00
004F87E5: sub esp,78h
004F87E8: push ebx
004F87E9: push esi
004F87EA: push edi
004F87EB: mov esi,ecx
004F87ED: mov dword ptr [esp+18h],0
004F87F5: fld dword ptr [esp+0000009Ch]
 00
004F87FC: fadd dword ptr [esi+000000D8h]
004F8802: mov ax,word ptr [esi+0000016Ch]
 00
004F8809: mov edi,dword ptr [esp+00000094h]
 00
004F8810: cmp ax,di
004F8813: fstp dword ptr [esp+0Ch]
004F8817: mov dword ptr [esp+0000008Ch],0
004F8822: jne 004F8861
004F8824: or ebx,0FFFFFFFFh
004F8827: cmp ax,bx
004F882A: je 004F8914
004F8830: mov eax,dword ptr [esi+00000170h]
004F8836: test eax,eax
004F8838: mov dword ptr [esp+0000008Ch],ebx
 00
004F883F: je 004F891B
004F8845: mov ecx,dword ptr [esp+00000098h]
 00
004F884C: mov edx,dword ptr [esp+0Ch]
004F8850: mov dword ptr [eax+1Ch],ecx
004F8853: mov esi,dword ptr [esi+00000170h]
004F8859: mov dword ptr [esi+14h],edx
004F885C: jmp 004F891B
004F8861: mov ecx,dword ptr [esi+00000170h]
004F8867: test ecx,ecx
004F8869: je 004F8889
004F886B: push 0
004F886D: push 4
004F886F: call 004EF2B0
004F8874: mov ecx,dword ptr [esi+00000170h]
004F887A: call 004EF1C0
004F887F: mov dword ptr [esi+00000170h],0
004F8889: or ebx,0FFFFFFFFh
004F888C: cmp di,bx
004F888F: mov word ptr [esi+0000016Ch],di
 00
004F8896: je 004F8914
004F8898: mov eax,dword ptr [esi+4]
004F889B: mov ecx,dword ptr [eax+14h]
004F889E: test ecx,ecx
004F88A0: je 004F88A7
004F88A2: lea eax,[ecx+30h]
004F88A5: jmp 004F88AA
004F88A7: add eax,4
004F88AA: mov edx,dword ptr [eax]
004F88AC: push 0
004F88AE: push 0
004F88B0: push 0
004F88B2: push 0
004F88B4: push 3F800000h
004F88B9: push 3F800000h
004F88BE: push 3F800000h
004F88C3: push 0
004F88C5: sub esp,0Ch
004F88C8: mov ecx,esp
004F88CA: mov dword ptr [ecx],edx
004F88CC: mov edx,dword ptr [eax+4]
004F88CF: mov eax,dword ptr [eax+8]
004F88D2: push esi
004F88D3: mov dword ptr [ecx+4],edx
004F88D6: push edi
004F88D7: mov dword ptr [ecx+8],eax
004F88DA: push 13h
004F88DC: lea ecx,[esp+48h]
004F88E0: call 004EFE50
004F88E5: mov ecx,dword ptr [esp+0Ch]
004F88E9: mov edx,dword ptr [esp+00000098h]
 00
004F88F0: lea eax,[esp+10h]
004F88F4: mov dword ptr [esp+24h],ecx
004F88F8: push eax
004F88F9: mov ecx,0B62CB0h
004F88FE: mov dword ptr [esp+30h],edx
004F8902: mov word ptr [esp+6Ah],4
 00
004F8909: call 004EFB10
004F890E: mov dword ptr [esi+00000170h],eax
004F8914: mov dword ptr [esp+0000008Ch],ebx
 00
004F891B: lea ecx,[esp+10h]
004F891F: call 004EF660
004F8924: mov ecx,dword ptr [esp+00000084h]
 00
004F892B: pop edi
004F892C: pop esi
004F892D: mov dword ptr fs:[00000000h],ecx
 00
004F8934: pop ebx
004F8935: add esp,84h
004F893B: ret 0Ch
004F8940: sub esp,8
004F8943: push ebx
004F8944: push ebp
004F8945: mov ebp,ecx
004F8947: movsx eax,byte ptr [ebp+00000080h]
 00
004F894E: cmp eax,9
004F8951: push esi
004F8952: push edi
004F8953: mov dword ptr [esp+10h],0C2C80000h
004F895B: ja 004F89B9
004F895D: movzx eax,byte ptr [eax+004F8AE8h]
 00
004F8964: jmp dword ptr [eax*4+004F8AD8h]
 00
004F896B: mov edi,dword ptr [esp+1Ch]
004F896F: mov ecx,dword ptr [edi+10h]
004F8972: cmp word ptr [ecx+22h],1C9h
004F8978: jne 004F89B9
004F897A: xor bl,bl
004F897C: lea esp,[esp]
004F8980: mov ecx,dword ptr [edi+10h]
004F8983: movzx esi,bl
004F8986: push esi
004F8987: add ecx,5A0h
004F898D: call 006C21B0
004F8992: cmp eax,1
004F8995: jne 004F89B2
004F8997: mov edx,dword ptr [edi+10h]
004F899A: fld dword ptr [edx+esi*4+000007F4h]
 00
004F89A1: fcomp dword ptr ds:[00858B50h]
004F89A7: fnstsw ax
004F89A9: test ah,41h
004F89AC: je 004F8A5D
004F89B2: inc bl
004F89B4: cmp bl,4
004F89B7: jb 004F8980
004F89B9: mov dword ptr [esp+10h],0C2C80000h
004F89C1: mov dword ptr [esp+14h],3F800000h
004F89C9: or esi,0FFFFFFFFh
004F89CC: mov eax,dword ptr [esp+10h]
004F89D0: mov ecx,dword ptr [esp+14h]
004F89D4: push eax
004F89D5: push ecx
004F89D6: push esi
004F89D7: mov ecx,ebp
004F89D9: call 004F8650
004F89DE: pop edi
004F89DF: pop esi
004F89E0: pop ebp
004F89E1: pop ebx
004F89E2: add esp,8
004F89E5: ret 4
004F89E8: mov edi,dword ptr [esp+1Ch]
004F89EC: xor bl,bl
004F89EE: mov edi,edi
004F89F0: mov ecx,dword ptr [edi+10h]
004F89F3: movzx esi,bl
004F89F6: push esi
004F89F7: add ecx,5A0h
004F89FD: call 006C21B0
004F8A02: cmp eax,1
004F8A05: jne 004F8A1E
004F8A07: mov eax,dword ptr [edi+10h]
004F8A0A: fld dword ptr [eax+esi*4+000007F4h]
 00
004F8A11: fcomp dword ptr ds:[00858B50h]
004F8A17: fnstsw ax
004F8A19: test ah,41h
004F8A1C: je 004F8A5D
004F8A1E: inc bl
004F8A20: cmp bl,4
004F8A23: jb 004F89F0
004F8A25: jmp 004F89B9
004F8A27: mov edi,dword ptr [esp+1Ch]
004F8A2B: mov edx,dword ptr [edi+10h]
004F8A2E: xor cl,cl
004F8A30: movzx eax,cl
004F8A33: cmp byte ptr [eax+edx+0000065Ch],1
004F8A3B: jne 004F8A51
004F8A3D: fld dword ptr [edx+eax*4+00000730h]
 00
004F8A44: fcomp dword ptr ds:[00858B50h]
004F8A4A: fnstsw ax
004F8A4C: test ah,41h
004F8A4F: je 004F8A5D
004F8A51: inc cl
004F8A53: cmp cl,2
004F8A56: jb 004F8A30
004F8A58: jmp 004F89B9
004F8A5D: fld dword ptr [edi+1Ch]
004F8A60: mov ecx,dword ptr [edi+14h]
004F8A63: fabs
004F8A65: fld dword ptr [ecx+58h]
004F8A68: fmul dword ptr ds:[00858C24h]
004F8A6E: fdivp st(1),st
004F8A70: fld dword ptr ds:[00858624h]
004F8A76: fcomp st(1)
004F8A78: fnstsw ax
004F8A7A: test ah,5
004F8A7D: jp 004F8A87
004F8A7F: fstp st(0)
004F8A81: fld dword ptr ds:[00858624h]
004F8A87: fst dword ptr [esp+1Ch]
004F8A8B: mov esi,4
004F8A90: fld dword ptr ds:[008CBD18h]
004F8A96: fmul st,st(1)
004F8A98: fadd qword ptr ds:[0085A310h]
004F8A9E: fstp dword ptr [esp+14h]
004F8AA2: fcomp qword ptr ds:[00859EF8h]
004F8AA8: fnstsw ax
004F8AAA: test ah,41h
004F8AAD: jne 004F89CC
004F8AB3: mov edx,dword ptr [esp+1Ch]
004F8AB7: push edx
004F8AB8: call 004D9E50
004F8ABD: fmul dword ptr ds:[00858BA4h]
004F8AC3: add esp,4
004F8AC6: fadd dword ptr ds:[008CBD1Ch]
004F8ACC: fstp dword ptr [esp+10h]
004F8AD0: jmp 004F89CC
004F8AD5: lea ecx,[ecx]
004F8AD8: call 274FDA66
004F8ADD: mov cl,byte ptr [edi]
004F8AE0: imul ecx,dword ptr [ecx+89B9004Fh],4Fh
 4F
004F8AE7: add byte ptr [eax],al
004F8AE9: add dword ptr [ecx],eax
004F8AEB: add eax,dword ptr [ebx]
004F8AED: add eax,dword ptr [ebx]
004F8AEF: add eax,dword ptr [ebx]
004F8AF1: add dl,byte ptr [eax+90909090h]
004F8B00: sub esp,0Ch
004F8B03: push ebx
004F8B04: push ebp
004F8B05: push esi
004F8B06: mov esi,dword ptr [esp+1Ch]
004F8B0A: mov eax,dword ptr [esi+4]
004F8B0D: push edi
004F8B0E: or edi,0FFFFFFFFh
004F8B11: test eax,eax
004F8B13: mov ebp,ecx
004F8B15: mov dword ptr [esp+18h],0C2C80000h
004F8B1D: mov dword ptr [esp+10h],3F800000h
004F8B25: je 004F8B3B
004F8B27: cmp eax,9
004F8B2A: je 004F8B30
004F8B2C: mov cl,4
004F8B2E: jmp 004F8B44
004F8B30: mov eax,dword ptr [esi+10h]
004F8B33: mov cl,byte ptr [eax+00000804h]
004F8B39: jmp 004F8B44
004F8B3B: mov ecx,dword ptr [esi+10h]
004F8B3E: mov cl,byte ptr [ecx+00000960h]
004F8B44: mov ebx,dword ptr [esi+10h]
004F8B47: movsx eax,word ptr [ebx+22h]
004F8B4B: add eax,0FFFFFE4Dh
004F8B50: cmp eax,9Ch
004F8B55: ja 004F8B65
004F8B57: movzx edx,byte ptr [eax+004F8D4Ch]
 00
004F8B5E: jmp dword ptr [edx*4+004F8D44h]
 00
004F8B65: mov eax,dword ptr [esi+14h]
004F8B68: test eax,eax
004F8B6A: je 004F8D28
004F8B70: test cl,cl
004F8B72: jbe 004F8D28
004F8B78: fld dword ptr [esi+1Ch]
004F8B7B: fabs
004F8B7D: fcom dword ptr ds:[00858B50h]
004F8B83: fnstsw ax
004F8B85: test ah,41h
004F8B88: jne 004F8D26
004F8B8E: fadd st(0),st
004F8B90: fld dword ptr ds:[00858624h]
004F8B96: fcomp st(1)
004F8B98: fnstsw ax
004F8B9A: test ah,5
004F8B9D: jp 004F8BAB
004F8B9F: fstp st(0)
004F8BA1: mov dword ptr [esp+20h],3F800000h
004F8BA9: jmp 004F8BAF
004F8BAB: fstp dword ptr [esp+20h]
004F8BAF: mov eax,dword ptr ds:[00B6F03Ch]
004F8BB4: test eax,eax
004F8BB6: lea edx,[eax+30h]
004F8BB9: jne 004F8BC0
004F8BBB: mov edx,0B6F02Ch
004F8BC0: mov eax,dword ptr [ebp+4]
004F8BC3: mov ecx,dword ptr [eax+14h]
004F8BC6: test ecx,ecx
004F8BC8: je 004F8BCF
004F8BCA: lea eax,[ecx+30h]
004F8BCD: jmp 004F8BD2
004F8BCF: add eax,4
004F8BD2: fld dword ptr [eax]
004F8BD4: fsub dword ptr [edx]
004F8BD6: fld dword ptr [eax+4]
004F8BD9: fsub dword ptr [edx+4]
004F8BDC: fld dword ptr [eax+8]
004F8BDF: fsub dword ptr [edx+8]
004F8BE2: fld st(0)
004F8BE4: fmul st,st(1)
004F8BE6: fld st(3)
004F8BE8: fmul st,st(4)
004F8BEA: faddp st(1),st
004F8BEC: fld st(2)
004F8BEE: fmul st,st(3)
004F8BF0: faddp st(1),st
004F8BF2: fsqrt
004F8BF4: fstp st(3)
004F8BF6: fstp st(0)
004F8BF8: fstp st(0)
004F8BFA: fdiv dword ptr ds:[008CBD10h]
004F8C00: fld st(0)
004F8C02: fcomp qword ptr ds:[0085A310h]
004F8C08: fnstsw ax
004F8C0A: test ah,5
004F8C0D: jp 004F8C17
004F8C0F: fstp st(0)
004F8C11: fld qword ptr ds:[0085A310h]
004F8C17: fmul qword ptr ds:[00859060h]
004F8C1D: movzx eax,byte ptr [ebx+000000BAh]
 00
004F8C24: push eax
004F8C25: fadd qword ptr ds:[00862CF8h]
004F8C2B: mov ecx,0B79538h
004F8C30: fstp dword ptr [esp+14h]
004F8C34: call 0055EA50
004F8C39: test al,al
004F8C3B: jne 004F8CD5
004F8C41: mov ecx,dword ptr [esi+10h]
004F8C44: movzx edx,byte ptr [ecx+000000BAh]
 00
004F8C4B: push edx
004F8C4C: mov ecx,0B79538h
004F8C51: call 0055EB10
004F8C56: test al,al
004F8C58: jne 004F8CD5
004F8C5A: mov eax,dword ptr [esi+10h]
004F8C5D: movzx ecx,byte ptr [eax+000000BAh]
 00
004F8C64: push ecx
004F8C65: mov ecx,0B79538h
004F8C6A: call 0055EA90
004F8C6F: test al,al
004F8C71: jne 004F8CB0
004F8C73: mov edx,dword ptr [esi+10h]
004F8C76: movzx eax,byte ptr [edx+000000BAh]
 00
004F8C7D: push eax
004F8C7E: mov ecx,0B79538h
004F8C83: call 0055EA70
004F8C88: test al,al
004F8C8A: jne 004F8CB0
004F8C8C: mov ecx,dword ptr [esi+10h]
004F8C8F: movzx edx,byte ptr [ecx+000000BAh]
 00
004F8C96: push edx
004F8C97: mov ecx,0B79538h
004F8C9C: call 0055EAD0
004F8CA1: test al,al
004F8CA3: jne 004F8CB0
004F8CA5: mov eax,dword ptr ds:[008CBD00h]
004F8CAA: mov dword ptr [esp+14h],eax
004F8CAE: jmp 004F8CF3
004F8CB0: fld dword ptr ds:[008CBD0Ch]
004F8CB6: mov edi,16h
004F8CBB: fmul dword ptr [esp+10h]
004F8CBF: fstp dword ptr [esp+10h]
004F8CC3: fld dword ptr ds:[008CBD08h]
004F8CC9: fadd dword ptr ds:[008CBD00h]
004F8CCF: fstp dword ptr [esp+14h]
004F8CD3: jmp 004F8CF8
004F8CD5: fld dword ptr ds:[008CBD04h]
004F8CDB: fmul dword ptr [esp+10h]
004F8CDF: fstp dword ptr [esp+10h]
004F8CE3: fld dword ptr ds:[00B6B9E4h]
004F8CE9: fadd dword ptr ds:[008CBD00h]
004F8CEF: fstp dword ptr [esp+14h]
004F8CF3: mov edi,15h
004F8CF8: fld dword ptr [esp+20h]
004F8CFC: fcomp dword ptr ds:[00862CF4h]
004F8D02: fnstsw ax
004F8D04: test ah,41h
004F8D07: jne 004F8D28
004F8D09: mov ecx,dword ptr [esp+20h]
004F8D0D: push ecx
004F8D0E: call 004D9E50
004F8D13: fmul dword ptr ds:[00858BA4h]
004F8D19: add esp,4
004F8D1C: fadd dword ptr [esp+14h]
004F8D20: fstp dword ptr [esp+18h]
004F8D24: jmp 004F8D28
004F8D26: fstp st(0)
004F8D28: mov edx,dword ptr [esp+18h]
004F8D2C: mov eax,dword ptr [esp+10h]
004F8D30: push edx
004F8D31: push eax
004F8D32: push edi
004F8D33: mov ecx,ebp
004F8D35: call 004F84D0
004F8D3A: pop edi
004F8D3B: pop esi
004F8D3C: pop ebp
004F8D3D: pop ebx
004F8D3E: add esp,0Ch
004F8D41: ret 4
004F8D44: jo 004F8CD1
004F8D46: dec edi
004F8D47: add byte ptr [ebp-75h],ah
004F8D4A: dec edi
004F8D4B: add byte ptr [eax],al
004F8D4D: add dword ptr [ecx],eax
004F8D4F: add dword ptr [ecx],eax
004F8D51: add dword ptr [ecx],eax
004F8D53: add dword ptr [ecx],eax
004F8D55: add dword ptr [ecx],eax
004F8D57: add dword ptr [ecx],eax
004F8D59: add dword ptr [ecx],eax
004F8D5B: add byte ptr [ecx],al
004F8D5D: add dword ptr [ecx],eax
004F8D5F: add dword ptr [ecx],eax
004F8D61: add dword ptr [ecx],eax
004F8D63: add dword ptr [ecx],eax
004F8D65: add dword ptr [ecx],eax
004F8D67: add dword ptr [ecx],eax
004F8D69: add dword ptr [ecx],eax
004F8D6B: add dword ptr [ecx],eax
004F8D6D: add dword ptr [ecx],eax
004F8D6F: add dword ptr [ecx],eax
004F8D71: add dword ptr [ecx],eax
004F8D73: add dword ptr [ecx],eax
004F8D75: add dword ptr [ecx],eax
004F8D77: add dword ptr [ecx],eax
004F8D79: add dword ptr [ecx],eax
004F8D7B: add dword ptr [ecx],eax
004F8D7D: add dword ptr [ecx],eax
004F8D7F: add dword ptr [ecx],eax
004F8D81: add dword ptr [ecx],eax
004F8D83: add dword ptr [ecx],eax
004F8D85: add dword ptr [ecx],eax
004F8D87: add dword ptr [ecx],eax
004F8D89: add dword ptr [ecx],eax
004F8D8B: add dword ptr [ecx],eax
004F8D8D: add dword ptr [ecx],eax
004F8D8F: add dword ptr [ecx],eax
004F8D91: add dword ptr [ecx],eax
004F8D93: add dword ptr [ecx],eax
004F8D95: add dword ptr [ecx],eax
004F8D97: add dword ptr [ecx],eax
004F8D99: add dword ptr [ecx],eax
004F8D9B: add dword ptr [ecx],eax
004F8D9D: add dword ptr [ecx],eax
004F8D9F: add dword ptr [ecx],eax
004F8DA1: add dword ptr [ecx],eax
004F8DA3: add dword ptr [ecx],eax
004F8DA5: add dword ptr [ecx],eax
004F8DA7: add dword ptr [ecx],eax
004F8DA9: add dword ptr [ecx],eax
004F8DAB: add dword ptr [ecx],eax
004F8DAD: add dword ptr [ecx],eax
004F8DAF: add dword ptr [ecx],eax
004F8DB1: add dword ptr [ecx],eax
004F8DB3: add dword ptr [ecx],eax
004F8DB5: add dword ptr [ecx],eax
004F8DB7: add dword ptr [ecx],eax
004F8DB9: add dword ptr [ecx],eax
004F8DBB: add dword ptr [ecx],eax
004F8DBD: add dword ptr [ecx],eax
004F8DBF: add dword ptr [ecx],eax
004F8DC1: add dword ptr [ecx],eax
004F8DC3: add dword ptr [ecx],eax
004F8DC5: add dword ptr [ecx],eax
004F8DC7: add dword ptr [ecx],eax
004F8DC9: add dword ptr [ecx],eax
004F8DCB: add dword ptr [ecx],eax
004F8DCD: add dword ptr [ecx],eax
004F8DCF: add dword ptr [ecx],eax
004F8DD1: add dword ptr [ecx],eax
004F8DD3: add dword ptr [ecx],eax
004F8DD5: add dword ptr [ecx],eax
004F8DD7: add dword ptr [ecx],eax
004F8DD9: add dword ptr [ecx],eax
004F8DDB: add dword ptr [ecx],eax
004F8DDD: add dword ptr [ecx],eax
004F8DDF: add dword ptr [ecx],eax
004F8DE1: add byte ptr [ecx],al
004F8DE3: add dword ptr [ecx],eax
004F8DE5: add dword ptr [ecx],eax
004F8DE7: add dword ptr [eax],eax
004F8DF0: sub esp,8
004F8DF3: mov edx,dword ptr [esp+0Ch]
004F8DF7: push esi
004F8DF8: push edi
004F8DF9: mov edi,ecx
004F8DFB: mov ecx,dword ptr [edx+10h]
004F8DFE: mov al,byte ptr [ecx+00000428h]
004F8E04: or esi,0FFFFFFFFh
004F8E07: test al,10h
004F8E09: mov dword ptr [esp+8],0C2C80000h
004F8E11: mov dword ptr [esp+0Ch],3F800000h
004F8E19: je 004F8EEA
004F8E1F: fld dword ptr [ecx+0000049Ch]
004F8E25: fcomp dword ptr ds:[00858B50h]
004F8E2B: fnstsw ax
004F8E2D: test ah,5
004F8E30: jnp 004F8E40
004F8E32: mov al,byte ptr [ecx+000004B4h]
004F8E38: test al,al
004F8E3A: jne 004F8EEA
004F8E40: mov al,byte ptr [ecx+00000961h]
004F8E46: test al,al
004F8E48: jbe 004F8E55
004F8E4A: mov eax,dword ptr [edx+14h]
004F8E4D: fld dword ptr [edx+1Ch]
004F8E50: fdiv dword ptr [eax+60h]
004F8E53: jmp 004F8E77
004F8E55: mov al,byte ptr [ecx+00000962h]
004F8E5B: test al,al
004F8E5D: jbe 004F8E71
004F8E5F: fld dword ptr [ecx+00000964h]
004F8E65: fmul dword ptr ds:[00858EE8h]
004F8E6B: fstp dword ptr [ecx+00000964h]
004F8E71: fld dword ptr [ecx+00000964h]
004F8E77: fcom dword ptr ds:[00858B50h]
004F8E7D: fst dword ptr [esp+14h]
004F8E81: fnstsw ax
004F8E83: test ah,5
004F8E86: jp 004F8E8E
004F8E88: fchs
004F8E8A: fst dword ptr [esp+14h]
004F8E8E: mov ecx,dword ptr [edx+10h]
004F8E91: fld dword ptr [ecx+0000049Ch]
004F8E97: mov esi,13h
004F8E9C: fcomp dword ptr ds:[00858B50h]
004F8EA2: fnstsw ax
004F8EA4: test ah,5
004F8EA7: jnp 004F8EAE
004F8EA9: mov esi,14h
004F8EAE: fld dword ptr ds:[008CBD28h]
004F8EB4: fmul st,st(1)
004F8EB6: fadd dword ptr ds:[008CBD24h]
004F8EBC: fstp dword ptr [esp+0Ch]
004F8EC0: fcomp qword ptr ds:[00859EF8h]
004F8EC6: fnstsw ax
004F8EC8: test ah,41h
004F8ECB: jne 004F8EEA
004F8ECD: mov edx,dword ptr [esp+14h]
004F8ED1: push edx
004F8ED2: call 004D9E50
004F8ED7: fmul dword ptr ds:[00858BA4h]
004F8EDD: add esp,4
004F8EE0: fadd dword ptr ds:[008CBD2Ch]
004F8EE6: fstp dword ptr [esp+8]
004F8EEA: mov eax,dword ptr [esp+8]
004F8EEE: mov ecx,dword ptr [esp+0Ch]
004F8EF2: push eax
004F8EF3: push ecx
004F8EF4: push esi
004F8EF5: mov ecx,edi
004F8EF7: call 004F87D0
004F8EFC: pop edi
004F8EFD: pop esi
004F8EFE: add esp,8
004F8F01: ret 4
004F8F10: sub esp,20h
004F8F13: fld dword ptr ds:[00858B50h]
004F8F19: push ebx
004F8F1A: push ebp
004F8F1B: push esi
004F8F1C: mov esi,dword ptr [esp+30h]
004F8F20: mov eax,dword ptr [esi+4]
004F8F23: test eax,eax
004F8F25: push edi
004F8F26: mov ebp,ecx
004F8F28: mov dword ptr [esp+14h],0
004F8F30: mov byte ptr [esp+13h],1
004F8F35: mov dword ptr [esp+1Ch],0FFFFFFFFh
004F8F3D: mov dword ptr [esp+18h],3F800000h
004F8F45: mov dword ptr [esp+20h],0
004F8F4D: je 004F8F87
004F8F4F: cmp eax,9
004F8F52: jne 004F92A9
004F8F58: mov edx,dword ptr [esi+10h]
004F8F5B: mov edi,dword ptr [edx+00000808h]
004F8F61: mov ebx,dword ptr [edx+00000810h]
004F8F67: mov al,byte ptr [edx+00000804h]
004F8F6D: lea ecx,[edx+0000080Ch]
004F8F73: mov dword ptr [esp+24h],edi
004F8F77: mov byte ptr [esp+34h],2
004F8F7C: lea edi,[edx+00000730h]
004F8F82: cmp ebx,2
004F8F85: jmp 004F8FB8
004F8F87: mov edx,dword ptr [esi+10h]
004F8F8A: mov edi,dword ptr [edx+00000964h]
004F8F90: mov al,byte ptr [edx+00000960h]
004F8F96: lea ecx,[edx+00000968h]
004F8F9C: mov ebx,2
004F8FA1: cmp dword ptr [ecx+4],ebx
004F8FA4: mov dword ptr [esp+24h],edi
004F8FA8: mov byte ptr [esp+34h],4
004F8FAD: lea edi,[edx+000007F4h]
004F8FB3: je 004F8FBA
004F8FB5: cmp dword ptr [ecx+0Ch],ebx
004F8FB8: jne 004F8FBF
004F8FBA: mov byte ptr [esp+13h],0
004F8FBF: test al,al
004F8FC1: jbe 004F921F
004F8FC7: fld dword ptr ds:[00B7CB64h]
004F8FCD: fcomp dword ptr ds:[00858B50h]
004F8FD3: fnstsw ax
004F8FD5: test ah,44h
004F8FD8: jnp 004F921F
004F8FDE: mov al,byte ptr [esp+34h]
004F8FE2: xor bl,bl
004F8FE4: test al,al
004F8FE6: jbe 004F921F
004F8FEC: mov eax,ecx
004F8FEE: sub edi,ecx
004F8FF0: mov dword ptr [esp+28h],eax
004F8FF4: mov dword ptr [esp+2Ch],edi
004F8FF8: jmp 004F9000
004F8FFA: lea ebx,[ebx+00000000h]
004F9000: mov edi,dword ptr [eax]
004F9002: test edi,edi
004F9004: je 004F90A3
004F900A: mov ecx,dword ptr [esp+2Ch]
004F900E: fld dword ptr [ecx+eax]
004F9011: fcomp dword ptr ds:[00858B50h]
004F9017: fnstsw ax
004F9019: test ah,41h
004F901C: jnp 004F90A3
004F9022: mov al,byte ptr [esp+13h]
004F9026: test al,al
004F9028: je 004F9038
004F902A: test bl,bl
004F902C: je 004F9033
004F902E: cmp bl,2
004F9031: jne 004F9038
004F9033: cmp edi,2
004F9036: je 004F90A3
004F9038: mov eax,dword ptr [esi+14h]
004F903B: mov cl,byte ptr [eax+48h]
004F903E: cmp cl,34h
004F9041: je 004F9084
004F9043: cmp cl,46h
004F9046: je 004F9069
004F9048: cmp cl,52h
004F904B: jne 004F9099
004F904D: cmp bl,1
004F9050: fstp st(0)
004F9052: je 004F9086
004F9054: cmp bl,3
004F9057: je 004F9086
004F9059: mov ecx,dword ptr [esi+1Ch]
004F905C: push ecx
004F905D: push eax
004F905E: push edi
004F905F: push edx
004F9060: mov ecx,ebp
004F9062: call 004F6000
004F9067: jmp 004F9099
004F9069: test bl,bl
004F906B: fstp st(0)
004F906D: je 004F9086
004F906F: cmp bl,2
004F9072: je 004F9086
004F9074: mov ecx,dword ptr [esi+1Ch]
004F9077: push ecx
004F9078: push eax
004F9079: push edi
004F907A: push edx
004F907B: mov ecx,ebp
004F907D: call 004F6000
004F9082: jmp 004F9099
004F9084: fstp st(0)
004F9086: mov ecx,dword ptr [esi+1Ch]
004F9089: push ecx
004F908A: push eax
004F908B: mov eax,dword ptr [esp+2Ch]
004F908F: push eax
004F9090: push edi
004F9091: mov ecx,ebp
004F9093: push edx
004F9094: call 004F5F30
004F9099: fld dword ptr [esp+14h]
004F909D: fadd st,st(1)
004F909F: fstp dword ptr [esp+14h]
004F90A3: mov eax,dword ptr [esp+28h]
004F90A7: mov cl,byte ptr [esp+34h]
004F90AB: inc bl
004F90AD: add eax,4
004F90B0: cmp bl,cl
004F90B2: mov dword ptr [esp+28h],eax
004F90B6: jb 004F9000
004F90BC: fstp st(0)
004F90BE: fld dword ptr [esp+14h]
004F90C2: fcomp dword ptr ds:[00858B50h]
004F90C8: fnstsw ax
004F90CA: test ah,41h
004F90CD: jne 004F9221
004F90D3: cmp byte ptr [ebp+00000080h],2
 02
004F90DA: jne 004F9107
004F90DC: fld dword ptr ds:[008CBCF0h]
004F90E2: mov ecx,dword ptr ds:[008CBCF8h]
004F90E8: fmul dword ptr [esp+14h]
004F90EC: mov dword ptr [esp+1Ch],0
004F90F4: mov dword ptr [esp+20h],ecx
004F90F8: fadd dword ptr ds:[008CBCECh]
004F90FE: fstp dword ptr [esp+18h]
004F9102: jmp 004F9221
004F9107: movzx edx,byte ptr [edx+000000BAh]
 00
004F910E: push edx
004F910F: mov ecx,0B79538h
004F9114: call 0055EA50
004F9119: test al,al
004F911B: jne 004F91DD
004F9121: mov eax,dword ptr [esi+10h]
004F9124: movzx ecx,byte ptr [eax+000000BAh]
 00
004F912B: push ecx
004F912C: mov ecx,0B79538h
004F9131: call 0055EB10
004F9136: test al,al
004F9138: jne 004F91DD
004F913E: mov edx,dword ptr [esi+10h]
004F9141: movzx eax,byte ptr [edx+000000BAh]
 00
004F9148: push eax
004F9149: mov ecx,0B79538h
004F914E: call 0055EA90
004F9153: test al,al
004F9155: jne 004F91BB
004F9157: mov ecx,dword ptr [esi+10h]
004F915A: movzx edx,byte ptr [ecx+000000BAh]
 00
004F9161: push edx
004F9162: mov ecx,0B79538h
004F9167: call 0055EA70
004F916C: test al,al
004F916E: jne 004F91BB
004F9170: mov eax,dword ptr [esi+10h]
004F9173: movzx ecx,byte ptr [eax+000000BAh]
 00
004F917A: push ecx
004F917B: mov ecx,0B79538h
004F9180: call 0055EAD0
004F9185: test al,al
004F9187: jne 004F91BB
004F9189: fld dword ptr ds:[008CBCE8h]
004F918F: mov al,byte ptr [ebp+00000080h]
004F9195: cmp al,1
004F9197: fmul dword ptr [esp+14h]
004F919B: mov dword ptr [esp+1Ch],18h
004F91A3: fadd dword ptr ds:[008CBCE4h]
004F91A9: fstp dword ptr [esp+18h]
004F91AD: jne 004F9201
004F91AF: fld dword ptr [esp+18h]
004F91B3: fmul dword ptr ds:[00858F08h]
004F91B9: jmp 004F91FD
004F91BB: fld dword ptr ds:[008CBCD8h]
004F91C1: mov dword ptr [esp+1Ch],8
004F91C9: fmul dword ptr [esp+14h]
004F91CD: mov dword ptr [esp+20h],0C1100000h
004F91D5: fadd dword ptr ds:[008CBCD4h]
004F91DB: jmp 004F91FD
004F91DD: fld dword ptr ds:[008CBCE0h]
004F91E3: mov dword ptr [esp+1Ch],6
004F91EB: fmul dword ptr [esp+14h]
004F91EF: mov dword ptr [esp+20h],0C1400000h
004F91F7: fadd dword ptr ds:[008CBCDCh]
004F91FD: fstp dword ptr [esp+18h]
004F9201: mov al,byte ptr [ebp+00000080h]
004F9207: cmp al,5
004F9209: je 004F920F
004F920B: cmp al,4
004F920D: jne 004F9221
004F920F: fld dword ptr ds:[008CBCF4h]
004F9215: fadd dword ptr [esp+20h]
004F9219: fstp dword ptr [esp+20h]
004F921D: jmp 004F9221
004F921F: fstp st(0)
004F9221: movzx edx,byte ptr [esp+34h]
004F9226: mov dword ptr [esp+34h],edx
004F922A: fild dword ptr [esp+34h]
004F922E: fdivr dword ptr [esp+14h]
004F9232: fst dword ptr [esp+14h]
004F9236: fcomp dword ptr ds:[00858C14h]
004F923C: fnstsw ax
004F923E: test ah,41h
004F9241: jne 004F9279
004F9243: mov eax,dword ptr [esp+14h]
004F9247: push eax
004F9248: call 004D9E50
004F924D: fmul dword ptr ds:[00858BA4h]
004F9253: fld dword ptr ds:[00B6B9E0h]
004F9259: add esp,4
004F925C: fadd dword ptr [esp+20h]
004F9260: fadd st,st(1)
004F9262: fstp dword ptr [esp+34h]
004F9266: fstp st(0)
004F9268: fld dword ptr [esp+34h]
004F926C: fcomp dword ptr ds:[00859014h]
004F9272: fnstsw ax
004F9274: test ah,5
004F9277: jp 004F9289
004F9279: mov dword ptr [esp+1Ch],0FFFFFFFFh
004F9281: mov dword ptr [esp+34h],0C2C80000h
004F9289: mov ecx,dword ptr [esp+34h]
004F928D: mov edx,dword ptr [esp+18h]
004F9291: mov eax,dword ptr [esp+1Ch]
004F9295: push ecx
004F9296: push edx
004F9297: push eax
004F9298: mov ecx,ebp
004F929A: call 004F8360
004F929F: pop edi
004F92A0: pop esi
004F92A1: pop ebp
004F92A2: pop ebx
004F92A3: add esp,20h
004F92A6: ret 4
004F92A9: pop edi
004F92AA: fstp st(0)
004F92AC: pop esi
004F92AD: pop ebp
004F92AE: pop ebx
004F92AF: add esp,20h
004F92B2: ret 4
004F92C0: push ecx
004F92C1: push ebx
004F92C2: push esi
004F92C3: push edi
004F92C4: push 6
004F92C6: mov esi,ecx
004F92C8: push 69h
004F92CA: mov ecx,0B5F8B8h
004F92CF: call 004D88C0
004F92D4: test al,al
004F92D6: je 004F93AB
004F92DC: fld dword ptr ds:[008CC30Ch]
004F92E2: fcom dword ptr ds:[00859014h]
004F92E8: fnstsw ax
004F92EA: test ah,41h
004F92ED: jne 004F93A9
004F92F3: fld dword ptr ds:[00B6B9ECh]
004F92F9: inc word ptr [esi+000000B2h]
 00
004F9300: mov ax,word ptr [esi+000000B2h]
 00
004F9307: fadd st,st(1)
004F9309: cmp ax,word ptr ds:[008CBD94h]
 00
004F9310: fstp dword ptr [esp+0Ch]
004F9314: fstp st(0)
004F9316: jl 004F93AB
004F931C: push 0Fh
004F931E: push 0Ch
004F9320: mov word ptr [esi+000000B2h],0
004F9329: call 004D9C10
004F932E: mov ecx,dword ptr [esi+4]
004F9331: mov edx,dword ptr [ecx+14h]
004F9334: add esp,8
004F9337: test edx,edx
004F9339: je 004F9340
004F933B: lea ecx,[edx+30h]
004F933E: jmp 004F9343
004F9340: add ecx,4
004F9343: mov edx,dword ptr [esp+0Ch]
004F9347: mov ebx,dword ptr [ecx]
004F9349: push 0
004F934B: push 0
004F934D: push 0
004F934F: push 0
004F9351: push 3F800000h
004F9356: push 3F800000h
004F935B: push 3F800000h
004F9360: push edx
004F9361: sub esp,0Ch
004F9364: mov edx,esp
004F9366: mov dword ptr [edx],ebx
004F9368: mov ebx,dword ptr [ecx+4]
004F936B: mov ecx,dword ptr [ecx+8]
004F936E: push esi
004F936F: mov dword ptr [edx+4],ebx
004F9372: push eax
004F9373: lea edi,[esi+8]
004F9376: mov dword ptr [edx+8],ecx
004F9379: push 6
004F937B: mov ecx,edi
004F937D: call 004EFE50
004F9382: push edi
004F9383: mov ecx,0B62CB0h
004F9388: mov dword ptr [esi+28h],3DCCCCCDh
 3D
004F938F: mov dword ptr [esi+20h],3F800000h
 3F
004F9396: mov dword ptr [esi+14h],8Eh
 00
004F939D: call 004EFB10
004F93A2: pop edi
004F93A3: pop esi
004F93A4: pop ebx
004F93A5: pop ecx
004F93A6: ret 4
004F93A9: fstp st(0)
004F93AB: pop edi
004F93AC: pop esi
004F93AD: pop ebx
004F93AE: pop ecx
004F93AF: ret 4
004F93C0: push 0FFFFFFFFh
004F93C2: push 83C368h
004F93C7: mov eax,dword ptr fs:[00000000h]
004F93CD: push eax
004F93CE: mov dword ptr fs:[00000000h],esp
 00
004F93D5: sub esp,80h
004F93DB: push esi
004F93DC: xor edx,edx
004F93DE: push edi
004F93DF: mov esi,ecx
004F93E1: mov dword ptr [esp+1Ch],edx
004F93E5: movsx edi,word ptr [esp+00000098h]
004F93ED: mov eax,dword ptr [esi+edi*8+000000E8h]
 00
004F93F4: cmp eax,edx
004F93F6: mov dword ptr [esp+00000090h],edx
 00
004F93FD: mov dword ptr [esp+8],edx
004F9401: mov dword ptr [esp+0Ch],3F800000h
004F9409: mov dword ptr [esp+10h],edx
004F940D: jne 004F9637
004F9413: lea eax,[edi-1]
004F9416: cmp eax,6
004F9419: ja 004F9602
004F941F: jmp dword ptr [eax*4+004F9680h]
 00
004F9426: mov eax,dword ptr [esi+4]
004F9429: mov ecx,dword ptr [eax+14h]
004F942C: cmp ecx,edx
004F942E: je 004F9435
004F9430: lea eax,[ecx+30h]
004F9433: jmp 004F9438
004F9435: add eax,4
004F9438: push edx
004F9439: push edx
004F943A: push edx
004F943B: push edx
004F943C: push 3F800000h
004F9441: push 3F800000h
004F9446: push 3F800000h
004F944B: push edx
004F944C: mov edx,dword ptr [eax]
004F944E: sub esp,0Ch
004F9451: mov ecx,esp
004F9453: mov dword ptr [ecx],edx
004F9455: mov edx,dword ptr [eax+4]
004F9458: mov eax,dword ptr [eax+8]
004F945B: mov dword ptr [ecx+4],edx
004F945E: mov edx,dword ptr [esp+000000C8h]
 00
004F9465: mov dword ptr [ecx+8],eax
004F9468: mov ecx,dword ptr [esp+000000CCh]
 00
004F946F: push esi
004F9470: push ecx
004F9471: push edx
004F9472: lea ecx,[esp+4Ch]
004F9476: call 004EFE50
004F947B: mov dword ptr [esp+2Ch],40800000h
004F9483: jmp 004F9602
004F9488: mov eax,dword ptr [esi+4]
004F948B: mov ecx,dword ptr [eax+14h]
004F948E: cmp ecx,edx
004F9490: je 004F9497
004F9492: lea eax,[ecx+30h]
004F9495: jmp 004F949A
004F9497: add eax,4
004F949A: push edx
004F949B: push edx
004F949C: push edx
004F949D: push edx
004F949E: push 3F800000h
004F94A3: push 3F800000h
004F94A8: push 3F800000h
004F94AD: push edx
004F94AE: mov edx,dword ptr [eax]
004F94B0: sub esp,0Ch
004F94B3: mov ecx,esp
004F94B5: mov dword ptr [ecx],edx
004F94B7: mov edx,dword ptr [eax+4]
004F94BA: mov eax,dword ptr [eax+8]
004F94BD: mov dword ptr [ecx+4],edx
004F94C0: mov edx,dword ptr [esp+000000C8h]
 00
004F94C7: mov dword ptr [ecx+8],eax
004F94CA: mov ecx,dword ptr [esp+000000CCh]
 00
004F94D1: push esi
004F94D2: push ecx
004F94D3: push edx
004F94D4: lea ecx,[esp+4Ch]
004F94D8: call 004EFE50
004F94DD: mov dword ptr [esp+2Ch],3F800000h
004F94E5: mov dword ptr [esp+64h],3ECCCCCDh
004F94ED: jmp 004F9602
004F94F2: mov eax,dword ptr [esi+4]
004F94F5: mov ecx,dword ptr [eax+14h]
004F94F8: cmp ecx,edx
004F94FA: je 004F9501
004F94FC: lea eax,[ecx+30h]
004F94FF: jmp 004F9504
004F9501: add eax,4
004F9504: push edx
004F9505: push edx
004F9506: push edx
004F9507: push edx
004F9508: push 3F800000h
004F950D: push 3F800000h
004F9512: push 3F800000h
004F9517: push edx
004F9518: mov edx,dword ptr [eax]
004F951A: sub esp,0Ch
004F951D: mov ecx,esp
004F951F: mov dword ptr [ecx],edx
004F9521: mov edx,dword ptr [eax+4]
004F9524: mov eax,dword ptr [eax+8]
004F9527: mov dword ptr [ecx+4],edx
004F952A: mov edx,dword ptr [esp+000000C8h]
 00
004F9531: mov dword ptr [ecx+8],eax
004F9534: mov ecx,dword ptr [esp+000000CCh]
 00
004F953B: push esi
004F953C: push ecx
004F953D: push edx
004F953E: lea ecx,[esp+4Ch]
004F9542: call 004EFE50
004F9547: mov dword ptr [esp+2Ch],40C00000h
004F954F: jmp 004F9602
004F9554: mov eax,dword ptr [esi+4]
004F9557: mov ecx,dword ptr [eax+14h]
004F955A: cmp ecx,edx
004F955C: je 004F9563
004F955E: lea eax,[ecx+30h]
004F9561: jmp 004F9566
004F9563: add eax,4
004F9566: push edx
004F9567: push edx
004F9568: push edx
004F9569: push edx
004F956A: push 3F800000h
004F956F: push 3F800000h
004F9574: push 3F800000h
004F9579: push edx
004F957A: mov edx,dword ptr [eax]
004F957C: sub esp,0Ch
004F957F: mov ecx,esp
004F9581: mov dword ptr [ecx],edx
004F9583: mov edx,dword ptr [eax+4]
004F9586: mov eax,dword ptr [eax+8]
004F9589: mov dword ptr [ecx+4],edx
004F958C: mov edx,dword ptr [esp+000000C8h]
 00
004F9593: mov dword ptr [ecx+8],eax
004F9596: mov ecx,dword ptr [esp+000000CCh]
 00
004F959D: push esi
004F959E: push ecx
004F959F: push edx
004F95A0: lea ecx,[esp+4Ch]
004F95A4: call 004EFE50
004F95A9: mov dword ptr [esp+2Ch],40900000h
004F95B1: jmp 004F9602
004F95B3: mov ecx,dword ptr [esp+8]
004F95B7: push edx
004F95B8: push edx
004F95B9: push edx
004F95BA: push edx
004F95BB: push 3F800000h
004F95C0: push 3F800000h
004F95C5: push 3F800000h
004F95CA: push edx
004F95CB: mov edx,dword ptr [esp+2Ch]
004F95CF: sub esp,0Ch
004F95D2: mov eax,esp
004F95D4: mov dword ptr [eax],ecx
004F95D6: mov ecx,dword ptr [esp+3Ch]
004F95DA: mov dword ptr [eax+4],edx
004F95DD: mov edx,dword ptr [esp+000000CCh]
 00
004F95E4: push esi
004F95E5: mov dword ptr [eax+8],ecx
004F95E8: mov eax,dword ptr [esp+000000CCh]
 00
004F95EF: push edx
004F95F0: push eax
004F95F1: lea ecx,[esp+4Ch]
004F95F5: call 004EFE50
004F95FA: mov dword ptr [esp+2Ch],42480000h
004F9602: mov ecx,dword ptr [esp+000000A4h]
 00
004F9609: mov edx,dword ptr [esp+000000A8h]
 00
004F9610: lea eax,[esp+14h]
004F9614: mov dword ptr [esp+28h],ecx
004F9618: push eax
004F9619: mov ecx,0B62CB0h
004F961E: mov dword ptr [esp+34h],edx
004F9622: mov word ptr [esp+6Eh],4
 00
004F9629: call 004EFB10
004F962E: mov dword ptr [esi+edi*8+000000E8h],eax
 00
004F9635: jmp 004F9652
004F9637: mov ecx,dword ptr [esp+000000A4h]
 00
004F963E: mov edx,dword ptr [esp+000000A8h]
 00
004F9645: mov dword ptr [eax+14h],ecx
004F9648: mov esi,dword ptr [esi+edi*8+000000E8h]
 00
004F964F: mov dword ptr [esi+1Ch],edx
004F9652: lea ecx,[esp+14h]
004F9656: mov dword ptr [esp+00000090h],0FFFFFFFFh
004F9661: call 004EF660
004F9666: mov ecx,dword ptr [esp+00000088h]
 00
004F966D: pop edi
004F966E: mov dword ptr fs:[00000000h],ecx
 00
004F9675: pop esi
004F9676: add esp,8Ch
004F967C: ret 14h
004F9680: xacquire xchg eax,esp
004F9682: dec edi
004F9683: add byte ptr [esi],ah
004F9685: xchg eax,esp
004F9686: dec edi
004F9687: add byte ptr [eax+26004F94h],cl
004F968D: xchg eax,esp
004F968E: dec edi
004F968F: add byte ptr [ebp+edx*4+4Fh],dl
004F9693: add byte ptr [edx],al
004F9695: xchg eax,esi
004F9696: dec edi
004F9697: add byte ptr [ebx+90004F95h],dh
004F96A0: mov al,byte ptr [ecx+00000080h]
004F96A6: mov ecx,dword ptr [ecx+4]
004F96A9: sub esp,0Ch
004F96AC: cmp al,4
004F96AE: jne 004F96C8
004F96B0: mov eax,dword ptr [ecx]
004F96B2: lea edx,[esp]
004F96B6: push edx
004F96B7: movsx edx,word ptr ds:[008CBD4Ch]
 00
004F96BE: push edx
004F96BF: call dword ptr [eax+64h]
004F96C2: mov ecx,dword ptr [esp+8]
004F96C6: jmp 004F96E7
004F96C8: mov eax,dword ptr [ecx+14h]
004F96CB: test eax,eax
004F96CD: je 004F96D4
004F96CF: add eax,30h
004F96D2: jmp 004F96D7
004F96D4: lea eax,[ecx+4]
004F96D7: mov ecx,dword ptr [eax]
004F96D9: mov dword ptr [esp],ecx
004F96DD: mov edx,dword ptr [eax+4]
004F96E0: mov dword ptr [esp+4],edx
004F96E4: mov ecx,dword ptr [eax+8]
004F96E7: mov eax,dword ptr [esp+10h]
004F96EB: push esi
004F96EC: mov esi,dword ptr [esp+4]
004F96F0: mov edx,eax
004F96F2: mov dword ptr [edx],esi
004F96F4: mov esi,dword ptr [esp+8]
004F96F8: mov dword ptr [edx+4],esi
004F96FB: mov dword ptr [edx+8],ecx
004F96FE: pop esi
004F96FF: add esp,0Ch
004F9702: ret 4
004F9710: push 0FFFFFFFFh
004F9712: push 83C388h
004F9717: mov eax,dword ptr fs:[00000000h]
004F971D: push eax
004F971E: mov dword ptr fs:[00000000h],esp
 00
004F9725: sub esp,74h
004F9728: push ebx
004F9729: push esi
004F972A: push edi
004F972B: mov esi,ecx
004F972D: mov dword ptr [esp+14h],0
004F9735: mov cx,word ptr [esi+000000E0h]
 00
004F973C: cmp cx,0FFFFh
004F9740: mov dword ptr [esp+00000088h],0
004F974B: je 004F99A1
004F9751: mov bx,word ptr [esp+00000090h]
004F9759: movsx edi,bx
004F975C: mov eax,edi
004F975E: dec eax
004F975F: je 004F978C
004F9761: dec eax
004F9762: je 004F976D
004F9764: dec eax
004F9765: jne 004F97A9
004F9767: push 2
004F9769: push 27h
004F976B: jmp 004F9779
004F976D: xor eax,eax
004F976F: mov ax,word ptr [esi+000000DEh]
 00
004F9776: push 28h
004F9778: push eax
004F9779: mov ecx,0B5F8B8h
004F977E: call 004D88C0
004F9783: test al,al
004F9785: jne 004F97A9
004F9787: jmp 004F99A1
004F978C: push ecx
004F978D: xor ecx,ecx
004F978F: mov cx,word ptr [esi+000000DCh]
 00
004F9796: push ecx
004F9797: mov ecx,0B5F8B8h
004F979C: call 004D88C0
004F97A1: test al,al
004F97A3: je 004F99A1
004F97A9: cmp bx,3
004F97AD: jne 004F9870
004F97B3: mov eax,dword ptr [esi+00000100h]
004F97B9: test eax,eax
004F97BB: jne 004F99A1
004F97C1: push 3
004F97C3: push 0
004F97C5: call 004D9C10
004F97CA: movsx eax,ax
004F97CD: shl eax,2
004F97D0: mov dx,word ptr [eax+008CC1D8h]
 00
004F97D7: mov di,word ptr [eax+008CC1DAh]
 00
004F97DE: mov eax,dword ptr [esi+4]
004F97E1: mov ecx,dword ptr [eax+14h]
004F97E4: add esp,8
004F97E7: test ecx,ecx
004F97E9: je 004F97F0
004F97EB: lea eax,[ecx+30h]
004F97EE: jmp 004F97F3
004F97F0: add eax,4
004F97F3: mov ecx,dword ptr [esp+0000009Ch]
 00
004F97FA: mov ebx,dword ptr [eax]
004F97FC: push 0
004F97FE: push 0
004F9800: push 0
004F9802: push 0
004F9804: push 3F800000h
004F9809: push 3F800000h
004F980E: push 3F800000h
004F9813: push ecx
004F9814: sub esp,0Ch
004F9817: mov ecx,esp
004F9819: mov dword ptr [ecx],ebx
004F981B: mov ebx,dword ptr [eax+4]
004F981E: mov eax,dword ptr [eax+8]
004F9821: push esi
004F9822: mov dword ptr [ecx+4],ebx
004F9825: push edx
004F9826: mov dword ptr [ecx+8],eax
004F9829: push 2
004F982B: lea ecx,[esp+44h]
004F982F: call 004EFE50
004F9834: mov ecx,dword ptr [esp+0000009Ch]
 00
004F983B: mov edx,dword ptr [esp+000000A0h]
 00
004F9842: lea eax,[esp+0Ch]
004F9846: mov dword ptr [esp+20h],ecx
004F984A: push eax
004F984B: mov ecx,0B62CB0h
004F9850: mov dword ptr [esp+2Ch],edx
004F9854: mov word ptr [esp+66h],24h
 00
004F985B: mov word ptr [esp+6Ch],di
004F9860: call 004EFB10
004F9865: mov dword ptr [esi+00000100h],eax
004F986B: jmp 004F99A1
004F9870: mov eax,dword ptr [esi+edi*8+000000E8h]
 00
004F9877: test eax,eax
004F9879: jne 004F9986
004F987F: mov eax,edi
004F9881: dec eax
004F9882: je 004F98EF
004F9884: dec eax
004F9885: jne 004F99A1
004F988B: mov eax,dword ptr [esi+4]
004F988E: mov ecx,dword ptr [eax+14h]
004F9891: test ecx,ecx
004F9893: je 004F989A
004F9895: lea eax,[ecx+30h]
004F9898: jmp 004F989D
004F989A: add eax,4
004F989D: mov edx,dword ptr [eax]
004F989F: push 0
004F98A1: push 0
004F98A3: push 0
004F98A5: push 0
004F98A7: push 3F800000h
004F98AC: push 3F800000h
004F98B1: push 3F800000h
004F98B6: push 0
004F98B8: sub esp,0Ch
004F98BB: mov ecx,esp
004F98BD: mov dword ptr [ecx],edx
004F98BF: mov edx,dword ptr [eax+4]
004F98C2: mov eax,dword ptr [eax+8]
004F98C5: mov dword ptr [ecx+4],edx
004F98C8: mov edx,dword ptr [esp+000000C0h]
 00
004F98CF: mov dword ptr [ecx+8],eax
004F98D2: mov ecx,dword ptr [esp+000000C4h]
 00
004F98D9: push esi
004F98DA: push ecx
004F98DB: push edx
004F98DC: lea ecx,[esp+44h]
004F98E0: call 004EFE50
004F98E5: mov dword ptr [esp+24h],3FC00000h
004F98ED: jmp 004F9951
004F98EF: mov eax,dword ptr [esi+4]
004F98F2: mov ecx,dword ptr [eax+14h]
004F98F5: test ecx,ecx
004F98F7: je 004F98FE
004F98F9: lea eax,[ecx+30h]
004F98FC: jmp 004F9901
004F98FE: add eax,4
004F9901: mov edx,dword ptr [eax]
004F9903: push 0
004F9905: push 0
004F9907: push 0
004F9909: push 0
004F990B: push 3F800000h
004F9910: push 3F800000h
004F9915: push 3F800000h
004F991A: push 0
004F991C: sub esp,0Ch
004F991F: mov ecx,esp
004F9921: mov dword ptr [ecx],edx
004F9923: mov edx,dword ptr [eax+4]
004F9926: mov eax,dword ptr [eax+8]
004F9929: mov dword ptr [ecx+4],edx
004F992C: mov edx,dword ptr [esp+000000C0h]
 00
004F9933: mov dword ptr [ecx+8],eax
004F9936: mov ecx,dword ptr [esp+000000C4h]
 00
004F993D: push esi
004F993E: push ecx
004F993F: push edx
004F9940: lea ecx,[esp+44h]
004F9944: call 004EFE50
004F9949: mov dword ptr [esp+24h],40200000h
004F9951: mov ecx,dword ptr [esp+000000A0h]
 00
004F9958: mov eax,dword ptr [esp+0000009Ch]
 00
004F995F: lea edx,[esp+0Ch]
004F9963: mov dword ptr [esp+28h],ecx
004F9967: push edx
004F9968: mov ecx,0B62CB0h
004F996D: mov dword ptr [esp+24h],eax
004F9971: mov word ptr [esp+66h],4
 00
004F9978: call 004EFB10
004F997D: mov dword ptr [esi+edi*8+000000E8h],eax
 00
004F9984: jmp 004F99A1
004F9986: mov ecx,dword ptr [esp+0000009Ch]
 00
004F998D: mov edx,dword ptr [esp+000000A0h]
 00
004F9994: mov dword ptr [eax+14h],ecx
004F9997: mov esi,dword ptr [esi+edi*8+000000E8h]
 00
004F999E: mov dword ptr [esi+1Ch],edx
004F99A1: lea ecx,[esp+0Ch]
004F99A5: mov dword ptr [esp+00000088h],0FFFFFFFFh
004F99B0: call 004EF660
004F99B5: mov ecx,dword ptr [esp+00000080h]
 00
004F99BC: pop edi
004F99BD: pop esi
004F99BE: mov dword ptr fs:[00000000h],ecx
 00
004F99C5: pop ebx
004F99C6: add esp,80h
004F99CC: ret 14h
004F99D0: sub esp,0Ch
004F99D3: mov eax,dword ptr ds:[00BD00F8h]
004F99D8: movsx edx,byte ptr [eax+60h]
004F99DC: push ebx
004F99DD: push ebp
004F99DE: push esi
004F99DF: mov esi,ecx
004F99E1: movsx ecx,byte ptr [eax+5Fh]
004F99E5: movsx eax,byte ptr [eax+61h]
004F99E9: mov dword ptr [esp+0Ch],ecx
004F99ED: xor ebx,ebx
004F99EF: push edi
004F99F0: fild dword ptr [esp+10h]
004F99F4: mov dword ptr [esp+10h],edx
004F99F8: fstp dword ptr [esp+18h]
004F99FC: fild dword ptr [esp+10h]
004F9A00: mov dword ptr [esp+10h],eax
004F9A04: mov al,byte ptr [esi+00000090h]
004F9A0A: cmp al,bl
004F9A0C: fstp dword ptr [esp+14h]
004F9A10: fild dword ptr [esp+10h]
004F9A14: fstp dword ptr [esp+10h]
004F9A18: jne 004F9B64
004F9A1E: cmp byte ptr [esp+20h],bl
004F9A22: je 004F9B08
004F9A28: cmp byte ptr [esi+000000BEh],bl
004F9A2E: jne 004F9B08
004F9A34: mov ecx,dword ptr [esi+00000178h]
004F9A3A: cmp ecx,ebx
004F9A3C: je 004F9A49
004F9A3E: call 004EF850
004F9A43: mov dword ptr [esi+00000178h],ebx
004F9A49: mov ecx,dword ptr [esp+14h]
004F9A4D: push 11h
004F9A4F: mov dword ptr [esi+000000C4h],ecx
004F9A55: push 4Ah
004F9A57: mov ecx,0B5F8B8h
004F9A5C: call 004D88C0
004F9A61: test al,al
004F9A63: jne 004F9A7E
004F9A65: push 13h
004F9A67: push 8Ah
004F9A6C: mov ecx,0B5F8B8h
004F9A71: call 004D88C0
004F9A76: test al,al
004F9A78: je 004F9E84
004F9A7E: mov eax,dword ptr [esi+4]
004F9A81: mov ecx,dword ptr [eax+14h]
004F9A84: cmp ecx,ebx
004F9A86: je 004F9A8D
004F9A88: lea eax,[ecx+30h]
004F9A8B: jmp 004F9A90
004F9A8D: add eax,4
004F9A90: mov edx,dword ptr [esi+000000C4h]
004F9A96: push ebx
004F9A97: push ebx
004F9A98: push ebx
004F9A99: push ebx
004F9A9A: push 3F800000h
004F9A9F: push 3F800000h
004F9AA4: push 3F800000h
004F9AA9: push edx
004F9AAA: mov edx,dword ptr [eax]
004F9AAC: sub esp,0Ch
004F9AAF: mov ecx,esp
004F9AB1: mov dword ptr [ecx],edx
004F9AB3: mov edx,dword ptr [eax+4]
004F9AB6: mov eax,dword ptr [eax+8]
004F9AB9: mov dword ptr [ecx+4],edx
004F9ABC: mov dword ptr [ecx+8],eax
004F9ABF: movsx cx,byte ptr [esi+00000090h]
004F9AC7: push esi
004F9AC8: lea edi,[esi+8]
004F9ACB: push ecx
004F9ACC: push 11h
004F9ACE: mov ecx,edi
004F9AD0: call 004EFE50
004F9AD5: mov dword ptr [esi+24h],3F800000h
 3F
004F9ADC: mov edx,dword ptr ds:[008CBEC4h]
004F9AE2: push edi
004F9AE3: mov ecx,0B62CB0h
004F9AE8: mov word ptr [esi+5Eh],4
004F9AEE: mov dword ptr [esi+20h],40200000h
 40
004F9AF5: mov dword ptr [esi+58h],edx
004F9AF8: call 004EFB10
004F9AFD: mov dword ptr [esi+00000178h],eax
004F9B03: jmp 004F9C57
004F9B08: fld dword ptr [esi+000000C4h]
004F9B0E: fsub dword ptr ds:[008CBECCh]
004F9B14: fcom dword ptr ds:[00859014h]
004F9B1A: fnstsw ax
004F9B1C: test ah,41h
004F9B1F: je 004F9B29
004F9B21: fstp st(0)
004F9B23: fld dword ptr ds:[00859014h]
004F9B29: mov ecx,dword ptr [esi+00000178h]
004F9B2F: fst dword ptr [esi+000000C4h]
004F9B35: cmp ecx,ebx
004F9B37: je 004F9C55
004F9B3D: fcom dword ptr ds:[00859014h]
004F9B43: fnstsw ax
004F9B45: test ah,41h
004F9B48: jne 004F9B52
004F9B4A: fstp dword ptr [ecx+14h]
004F9B4D: jmp 004F9C57
004F9B52: fstp st(0)
004F9B54: call 004EF850
004F9B59: mov dword ptr [esi+00000178h],ebx
004F9B5F: jmp 004F9C57
004F9B64: cmp byte ptr [esp+20h],bl
004F9B68: je 004F9C36
004F9B6E: cmp byte ptr [esi+000000BEh],bl
004F9B74: jne 004F9C57
004F9B7A: cmp al,bl
004F9B7C: jle 004F9E84
004F9B82: cmp al,9
004F9B84: jg 004F9E84
004F9B8A: fld dword ptr [esp+18h]
004F9B8E: push 11h
004F9B90: fadd dword ptr [esi+000000A0h]
004F9B96: push 4Ah
004F9B98: mov ecx,0B5F8B8h
004F9B9D: fstp dword ptr [esi+000000C4h]
004F9BA3: call 004D88C0
004F9BA8: test al,al
004F9BAA: jne 004F9BC5
004F9BAC: push 13h
004F9BAE: push 8Ah
004F9BB3: mov ecx,0B5F8B8h
004F9BB8: call 004D88C0
004F9BBD: test al,al
004F9BBF: je 004F9E84
004F9BC5: cmp dword ptr [esi+00000178h],ebx
004F9BCB: jne 004F9E84
004F9BD1: mov eax,dword ptr [esi+4]
004F9BD4: mov ecx,dword ptr [eax+14h]
004F9BD7: cmp ecx,ebx
004F9BD9: je 004F9BE0
004F9BDB: lea eax,[ecx+30h]
004F9BDE: jmp 004F9BE3
004F9BE0: add eax,4
004F9BE3: mov ecx,dword ptr [esi+000000C4h]
004F9BE9: push ebx
004F9BEA: push ebx
004F9BEB: push ebx
004F9BEC: push ebx
004F9BED: push 3F800000h
004F9BF2: push 3F800000h
004F9BF7: push 3F800000h
004F9BFC: push ecx
004F9BFD: mov ecx,dword ptr [eax]
004F9BFF: sub esp,0Ch
004F9C02: mov edx,esp
004F9C04: mov dword ptr [edx],ecx
004F9C06: mov ecx,dword ptr [eax+4]
004F9C09: mov eax,dword ptr [eax+8]
004F9C0C: mov dword ptr [edx+4],ecx
004F9C0F: movsx cx,byte ptr [esi+00000090h]
004F9C17: push esi
004F9C18: lea edi,[esi+8]
004F9C1B: mov dword ptr [edx+8],eax
004F9C1E: push ecx
004F9C1F: push 11h
004F9C21: mov ecx,edi
004F9C23: call 004EFE50
004F9C28: fld dword ptr [esi+00000094h]
004F9C2E: fstp dword ptr [esi+24h]
004F9C31: jmp 004F9ADC
004F9C36: cmp byte ptr [esi+000000BEh],bl
004F9C3C: je 004F9C57
004F9C3E: mov ecx,dword ptr [esi+00000178h]
004F9C44: cmp ecx,ebx
004F9C46: je 004F9C57
004F9C48: call 004EF850
004F9C4D: mov dword ptr [esi+00000178h],ebx
004F9C53: jmp 004F9C57
004F9C55: fstp st(0)
004F9C57: cmp byte ptr [esp+24h],bl
004F9C5B: je 004F9C67
004F9C5D: cmp byte ptr [esp+28h],bl
004F9C61: jne 004F9C67
004F9C63: mov cl,1
004F9C65: jmp 004F9C69
004F9C67: xor cl,cl
004F9C69: cmp byte ptr [esi+000000BFh],bl
004F9C6F: je 004F9C7D
004F9C71: cmp byte ptr [esi+000000C0h],bl
004F9C77: jne 004F9C7D
004F9C79: mov al,1
004F9C7B: jmp 004F9C7F
004F9C7D: xor al,al
004F9C7F: cmp cl,bl
004F9C81: mov ebp,dword ptr [esp+10h]
004F9C85: je 004F9D96
004F9C8B: cmp al,bl
004F9C8D: jne 004F9DAF
004F9C93: push 11h
004F9C95: push 4Ah
004F9C97: mov ecx,0B5F8B8h
004F9C9C: call 004D88C0
004F9CA1: test al,al
004F9CA3: je 004F9E84
004F9CA9: cmp dword ptr [esi+0000017Ch],ebx
004F9CAF: jne 004F9E84
004F9CB5: mov eax,dword ptr [esp+2Ch]
004F9CB9: mov ecx,dword ptr [eax+10h]
004F9CBC: cmp word ptr [ecx+22h],1A7h
004F9CC2: jne 004F9D1C
004F9CC4: push 28h
004F9CC6: push 4Fh
004F9CC8: mov ecx,0B5F8B8h
004F9CCD: call 004D88C0
004F9CD2: test al,al
004F9CD4: je 004F9E84
004F9CDA: mov eax,dword ptr [esi+4]
004F9CDD: mov ecx,dword ptr [eax+14h]
004F9CE0: cmp ecx,ebx
004F9CE2: je 004F9CE9
004F9CE4: lea eax,[ecx+30h]
004F9CE7: jmp 004F9CEC
004F9CE9: add eax,4
004F9CEC: mov ecx,dword ptr [eax]
004F9CEE: push ebx
004F9CEF: push ebx
004F9CF0: push ebx
004F9CF1: push ebx
004F9CF2: push 3F800000h
004F9CF7: push 3F800000h
004F9CFC: push 3F800000h
004F9D01: push ebp
004F9D02: sub esp,0Ch
004F9D05: mov edx,esp
004F9D07: mov dword ptr [edx],ecx
004F9D09: mov ecx,dword ptr [eax+4]
004F9D0C: mov eax,dword ptr [eax+8]
004F9D0F: push esi
004F9D10: mov dword ptr [edx+4],ecx
004F9D13: push 3
004F9D15: mov dword ptr [edx+8],eax
004F9D18: push 28h
004F9D1A: jmp 004F9D5C
004F9D1C: mov eax,dword ptr [esi+4]
004F9D1F: mov ecx,dword ptr [eax+14h]
004F9D22: cmp ecx,ebx
004F9D24: je 004F9D2B
004F9D26: lea eax,[ecx+30h]
004F9D29: jmp 004F9D2E
004F9D2B: add eax,4
004F9D2E: mov edx,dword ptr [eax]
004F9D30: push ebx
004F9D31: push ebx
004F9D32: push ebx
004F9D33: push ebx
004F9D34: push 3F800000h
004F9D39: push 3F800000h
004F9D3E: push 3F800000h
004F9D43: push ebp
004F9D44: sub esp,0Ch
004F9D47: mov ecx,esp
004F9D49: mov dword ptr [ecx],edx
004F9D4B: mov edx,dword ptr [eax+4]
004F9D4E: mov eax,dword ptr [eax+8]
004F9D51: push esi
004F9D52: mov dword ptr [ecx+4],edx
004F9D55: push 0Ah
004F9D57: mov dword ptr [ecx+8],eax
004F9D5A: push 11h
004F9D5C: lea edi,[esi+8]
004F9D5F: mov ecx,edi
004F9D61: call 004EFE50
004F9D66: mov ecx,dword ptr ds:[008CBEC8h]
004F9D6C: mov dword ptr [esi+58h],ecx
004F9D6F: push edi
004F9D70: mov ecx,0B62CB0h
004F9D75: mov dword ptr [esi+24h],3F800000h
 3F
004F9D7C: mov word ptr [esi+5Eh],4
004F9D82: mov dword ptr [esi+20h],40400000h
 40
004F9D89: call 004EFB10
004F9D8E: mov dword ptr [esi+0000017Ch],eax
004F9D94: jmp 004F9DAF
004F9D96: cmp al,bl
004F9D98: je 004F9DAF
004F9D9A: mov ecx,dword ptr [esi+0000017Ch]
004F9DA0: cmp ecx,ebx
004F9DA2: je 004F9DAF
004F9DA4: call 004EF850
004F9DA9: mov dword ptr [esi+0000017Ch],ebx
004F9DAF: cmp byte ptr [esp+28h],bl
004F9DB3: mov al,byte ptr [esi+000000C0h]
004F9DB9: je 004F9E6B
004F9DBF: cmp al,bl
004F9DC1: jne 004F9E84
004F9DC7: push 11h
004F9DC9: push 4Ah
004F9DCB: mov ecx,0B5F8B8h
004F9DD0: call 004D88C0
004F9DD5: test al,al
004F9DD7: je 004F9E84
004F9DDD: cmp dword ptr [esi+00000180h],ebx
004F9DE3: jne 004F9E84
004F9DE9: mov eax,dword ptr [esi+4]
004F9DEC: mov ecx,dword ptr [eax+14h]
004F9DEF: cmp ecx,ebx
004F9DF1: je 004F9DF8
004F9DF3: lea eax,[ecx+30h]
004F9DF6: jmp 004F9DFB
004F9DF8: add eax,4
004F9DFB: mov ecx,dword ptr [eax]
004F9DFD: push ebx
004F9DFE: push ebx
004F9DFF: push ebx
004F9E00: push ebx
004F9E01: push 3F800000h
004F9E06: push 3F800000h
004F9E0B: push 3F800000h
004F9E10: push ebp
004F9E11: sub esp,0Ch
004F9E14: mov edx,esp
004F9E16: mov dword ptr [edx],ecx
004F9E18: mov ecx,dword ptr [eax+4]
004F9E1B: mov eax,dword ptr [eax+8]
004F9E1E: push esi
004F9E1F: mov dword ptr [edx+4],ecx
004F9E22: push 0Bh
004F9E24: lea edi,[esi+8]
004F9E27: push 11h
004F9E29: mov ecx,edi
004F9E2B: mov dword ptr [edx+8],eax
004F9E2E: call 004EFE50
004F9E33: mov ecx,dword ptr ds:[008CBEC8h]
004F9E39: mov dword ptr [esi+58h],ecx
004F9E3C: push edi
004F9E3D: mov ecx,0B62CB0h
004F9E42: mov dword ptr [esi+24h],3F800000h
 3F
004F9E49: mov word ptr [esi+5Eh],4
004F9E4F: mov dword ptr [esi+20h],40400000h
 40
004F9E56: call 004EFB10
004F9E5B: mov dword ptr [esi+00000180h],eax
004F9E61: pop edi
004F9E62: pop esi
004F9E63: pop ebp
004F9E64: pop ebx
004F9E65: add esp,0Ch
004F9E68: ret 10h
004F9E6B: cmp al,bl
004F9E6D: je 004F9E84
004F9E6F: mov ecx,dword ptr [esi+00000180h]
004F9E75: cmp ecx,ebx
004F9E77: je 004F9E84
004F9E79: call 004EF850
004F9E7E: mov dword ptr [esi+00000180h],ebx
004F9E84: pop edi
004F9E85: pop esi
004F9E86: pop ebp
004F9E87: pop ebx
004F9E88: add esp,0Ch
004F9E8B: ret 10h
004F9E90: push ebx
004F9E91: push esi
004F9E92: mov esi,ecx
004F9E94: cmp byte ptr [esi+00000080h],6
 06
004F9E9B: mov ax,word ptr [esp+0Ch]
004F9EA0: push edi
004F9EA1: je 004F9EB7
004F9EA3: cmp ax,6
004F9EA7: je 004F9EB7
004F9EA9: cmp word ptr [esi+000000E0h],0FFFFh
004F9EB1: je 004FA0AC
004F9EB7: fld dword ptr [esp+20h]
004F9EBB: movsx ebx,ax
004F9EBE: fadd dword ptr [esi+000000D8h]
004F9EC4: mov eax,dword ptr [esi+ebx*8+000000E8h]
 00
004F9ECB: test eax,eax
004F9ECD: fstp dword ptr [esp+20h]
004F9ED1: jne 004FA097
004F9ED7: mov eax,ebx
004F9ED9: sub eax,2
004F9EDC: je 004F9FE5
004F9EE2: dec eax
004F9EE3: je 004F9F6A
004F9EE9: sub eax,3
004F9EEC: jne 004FA0AC
004F9EF2: push 2
004F9EF4: push 27h
004F9EF6: mov ecx,0B5F8B8h
004F9EFB: call 004D88C0
004F9F00: test al,al
004F9F02: je 004FA0AC
004F9F08: mov eax,dword ptr [esi+4]
004F9F0B: mov ecx,dword ptr [eax+14h]
004F9F0E: test ecx,ecx
004F9F10: je 004F9F17
004F9F12: lea eax,[ecx+30h]
004F9F15: jmp 004F9F1A
004F9F17: add eax,4
004F9F1A: mov ecx,dword ptr [esp+20h]
004F9F1E: push 0
004F9F20: push 0
004F9F22: push 0
004F9F24: push 0
004F9F26: push 3F800000h
004F9F2B: push 3F800000h
004F9F30: push 3F800000h
004F9F35: push ecx
004F9F36: mov ecx,dword ptr [eax]
004F9F38: sub esp,0Ch
004F9F3B: mov edx,esp
004F9F3D: mov dword ptr [edx],ecx
004F9F3F: mov ecx,dword ptr [eax+4]
004F9F42: mov eax,dword ptr [eax+8]
004F9F45: mov dword ptr [edx+4],ecx
004F9F48: mov ecx,dword ptr [esp+44h]
004F9F4C: push esi
004F9F4D: mov dword ptr [edx+8],eax
004F9F50: mov edx,dword ptr [esp+44h]
004F9F54: push ecx
004F9F55: lea edi,[esi+8]
004F9F58: push edx
004F9F59: mov ecx,edi
004F9F5B: call 004EFE50
004F9F60: mov eax,dword ptr ds:[008CBEE8h]
004F9F65: jmp 004FA068
004F9F6A: push 13h
004F9F6C: push 8Ah
004F9F71: mov ecx,0B5F8B8h
004F9F76: call 004D88C0
004F9F7B: test al,al
004F9F7D: je 004FA0AC
004F9F83: mov eax,dword ptr [esi+4]
004F9F86: mov ecx,dword ptr [eax+14h]
004F9F89: test ecx,ecx
004F9F8B: je 004F9F92
004F9F8D: lea eax,[ecx+30h]
004F9F90: jmp 004F9F95
004F9F92: add eax,4
004F9F95: mov ecx,dword ptr [esp+20h]
004F9F99: push 0
004F9F9B: push 0
004F9F9D: push 0
004F9F9F: push 0
004F9FA1: push 3F800000h
004F9FA6: push 3F800000h
004F9FAB: push 3F800000h
004F9FB0: push ecx
004F9FB1: mov ecx,dword ptr [eax]
004F9FB3: sub esp,0Ch
004F9FB6: mov edx,esp
004F9FB8: mov dword ptr [edx],ecx
004F9FBA: mov ecx,dword ptr [eax+4]
004F9FBD: mov eax,dword ptr [eax+8]
004F9FC0: mov dword ptr [edx+4],ecx
004F9FC3: mov ecx,dword ptr [esp+44h]
004F9FC7: push esi
004F9FC8: mov dword ptr [edx+8],eax
004F9FCB: mov edx,dword ptr [esp+44h]
004F9FCF: push ecx
004F9FD0: lea edi,[esi+8]
004F9FD3: push edx
004F9FD4: mov ecx,edi
004F9FD6: call 004EFE50
004F9FDB: mov eax,dword ptr ds:[008CBEE4h]
004F9FE0: jmp 004FA068
004F9FE5: xor ecx,ecx
004F9FE7: mov cx,word ptr [esi+000000E0h]
 00
004F9FEE: xor edx,edx
004F9FF0: mov dx,word ptr [esi+000000DCh]
 00
004F9FF7: push ecx
004F9FF8: mov ecx,0B5F8B8h
004F9FFD: push edx
004F9FFE: call 004D88C0
004FA003: test al,al
004FA005: je 004FA0AC
004FA00B: mov eax,dword ptr [esi+4]
004FA00E: mov ecx,dword ptr [eax+14h]
004FA011: test ecx,ecx
004FA013: je 004FA01A
004FA015: lea eax,[ecx+30h]
004FA018: jmp 004FA01D
004FA01A: add eax,4
004FA01D: mov ecx,dword ptr [esp+20h]
004FA021: push 0
004FA023: push 0
004FA025: push 0
004FA027: push 0
004FA029: push 3F800000h
004FA02E: push 3F800000h
004FA033: push 3F800000h
004FA038: push ecx
004FA039: mov ecx,dword ptr [eax]
004FA03B: sub esp,0Ch
004FA03E: mov edx,esp
004FA040: mov dword ptr [edx],ecx
004FA042: mov ecx,dword ptr [eax+4]
004FA045: mov eax,dword ptr [eax+8]
004FA048: mov dword ptr [edx+4],ecx
004FA04B: mov ecx,dword ptr [esp+44h]
004FA04F: push esi
004FA050: mov dword ptr [edx+8],eax
004FA053: mov edx,dword ptr [esp+44h]
004FA057: push ecx
004FA058: lea edi,[esi+8]
004FA05B: push edx
004FA05C: mov ecx,edi
004FA05E: call 004EFE50
004FA063: mov eax,dword ptr ds:[008CBEE0h]
004FA068: mov ecx,dword ptr [esp+20h]
004FA06C: mov edx,dword ptr [esp+1Ch]
004FA070: mov dword ptr [esi+1Ch],ecx
004FA073: push edi
004FA074: mov ecx,0B62CB0h
004FA079: mov dword ptr [esi+20h],eax
004FA07C: mov dword ptr [esi+24h],edx
004FA07F: mov word ptr [esi+5Eh],4
004FA085: call 004EFB10
004FA08A: mov dword ptr [esi+ebx*8+000000E8h],eax
 00
004FA091: pop edi
004FA092: pop esi
004FA093: pop ebx
004FA094: ret 14h
004FA097: mov ecx,dword ptr [esp+20h]
004FA09B: mov dword ptr [eax+14h],ecx
004FA09E: mov edx,dword ptr [esi+ebx*8+000000E8h]
 00
004FA0A5: mov eax,dword ptr [esp+1Ch]
004FA0A9: mov dword ptr [edx+1Ch],eax
004FA0AC: pop edi
004FA0AD: pop esi
004FA0AE: pop ebx
004FA0AF: ret 14h
004FA0C0: push ecx
004FA0C1: mov eax,dword ptr [esp+8]
004FA0C5: fld dword ptr [eax+1Ch]
004FA0C8: push esi
004FA0C9: fabs
004FA0CB: fcom dword ptr ds:[00858F34h]
004FA0D1: mov esi,ecx
004FA0D3: mov ecx,dword ptr [eax+10h]
004FA0D6: fnstsw ax
004FA0D8: test ah,5
004FA0DB: jnp 004FA0E5
004FA0DD: fstp st(0)
004FA0DF: fld dword ptr ds:[00858F34h]
004FA0E5: fmul dword ptr ds:[0085F09Ch]
004FA0EB: mov al,byte ptr [ecx+000005ACh]
004FA0F1: test al,1
004FA0F3: fstp dword ptr [esp+4]
004FA0F7: je 004FA10A
004FA0F9: fld dword ptr [esp+4]
004FA0FD: fcomp dword ptr ds:[00858C14h]
004FA103: fnstsw ax
004FA105: test ah,5
004FA108: jp 004FA114
004FA10A: mov dword ptr [esp+0Ch],0C2C80000h
004FA112: jmp 004FA152
004FA114: cmp byte ptr [esi+00000080h],6
 06
004FA11B: jne 004FA135
004FA11D: mov eax,dword ptr [esp+4]
004FA121: push eax
004FA122: call 004D9E50
004FA127: fmul dword ptr ds:[00858BA4h]
004FA12D: fadd dword ptr ds:[008CBEDCh]
004FA133: jmp 004FA14B
004FA135: mov ecx,dword ptr [esp+4]
004FA139: push ecx
004FA13A: call 004D9E50
004FA13F: fmul dword ptr ds:[00858BA4h]
004FA145: fadd dword ptr ds:[008CBED8h]
004FA14B: fstp dword ptr [esp+10h]
004FA14F: add esp,4
004FA152: fld dword ptr [esp+4]
004FA156: fmul dword ptr ds:[00862D00h]
004FA15C: fadd dword ptr ds:[00858C98h]
004FA162: fstp dword ptr [esp+4]
004FA166: fld dword ptr ds:[00C8132Ch]
004FA16C: fcomp dword ptr ds:[00858B8Ch]
004FA172: fnstsw ax
004FA174: test ah,1
004FA177: jne 004FA195
004FA179: fld dword ptr ds:[008CBEECh]
004FA17F: fmul dword ptr [esp+4]
004FA183: fstp dword ptr [esp+4]
004FA187: fld dword ptr ds:[008CBEF0h]
004FA18D: fadd dword ptr [esp+0Ch]
004FA191: fstp dword ptr [esp+0Ch]
004FA195: mov edx,dword ptr [esp+0Ch]
004FA199: mov eax,dword ptr [esp+4]
004FA19D: push edx
004FA19E: push eax
004FA19F: push 3
004FA1A1: push 2
004FA1A3: push 6
004FA1A5: mov ecx,esi
004FA1A7: call 004F9E90
004FA1AC: pop esi
004FA1AD: pop ecx
004FA1AE: ret 4
004FA1C0: push ebx
004FA1C1: push esi
004FA1C2: mov esi,ecx
004FA1C4: mov cx,word ptr [esi+000000E0h]
 00
004FA1CB: cmp cx,0FFFFh
004FA1CF: push edi
004FA1D0: je 004FA3DC
004FA1D6: fld dword ptr [esp+20h]
004FA1DA: movsx ebx,word ptr [esp+10h]
004FA1DF: fadd dword ptr [esi+000000D8h]
004FA1E5: mov eax,dword ptr [esi+ebx*8+000000E8h]
 00
004FA1EC: test eax,eax
004FA1EE: fstp dword ptr [esp+20h]
004FA1F2: jne 004FA3C7
004FA1F8: mov eax,ebx
004FA1FA: dec eax
004FA1FB: je 004FA308
004FA201: dec eax
004FA202: je 004FA270
004FA204: dec eax
004FA205: jne 004FA3DC
004FA20B: mov eax,dword ptr [esi+4]
004FA20E: mov ecx,dword ptr [eax+14h]
004FA211: test ecx,ecx
004FA213: je 004FA21A
004FA215: lea eax,[ecx+30h]
004FA218: jmp 004FA21D
004FA21A: add eax,4
004FA21D: mov ecx,dword ptr [esp+20h]
004FA221: push 0
004FA223: push 0
004FA225: push 0
004FA227: push 0
004FA229: push 3F800000h
004FA22E: push 3F800000h
004FA233: push 3F800000h
004FA238: push ecx
004FA239: mov ecx,dword ptr [eax]
004FA23B: sub esp,0Ch
004FA23E: mov edx,esp
004FA240: mov dword ptr [edx],ecx
004FA242: mov ecx,dword ptr [eax+4]
004FA245: mov eax,dword ptr [eax+8]
004FA248: mov dword ptr [edx+4],ecx
004FA24B: mov ecx,dword ptr [esp+44h]
004FA24F: push esi
004FA250: mov dword ptr [edx+8],eax
004FA253: mov edx,dword ptr [esp+44h]
004FA257: push ecx
004FA258: lea edi,[esi+8]
004FA25B: push edx
004FA25C: mov ecx,edi
004FA25E: call 004EFE50
004FA263: mov eax,dword ptr ds:[008CBEF4h]
004FA268: mov dword ptr [esi+20h],eax
004FA26B: jmp 004FA39B
004FA270: push ecx
004FA271: xor ecx,ecx
004FA273: mov cx,word ptr [esi+000000DCh]
 00
004FA27A: push ecx
004FA27B: mov ecx,0B5F8B8h
004FA280: call 004D88C0
004FA285: test al,al
004FA287: je 004FA3DC
004FA28D: mov eax,dword ptr [esi+4]
004FA290: mov ecx,dword ptr [eax+14h]
004FA293: test ecx,ecx
004FA295: je 004FA29C
004FA297: lea eax,[ecx+30h]
004FA29A: jmp 004FA29F
004FA29C: add eax,4
004FA29F: mov edx,dword ptr [esp+20h]
004FA2A3: push 0
004FA2A5: push 0
004FA2A7: push 0
004FA2A9: push 0
004FA2AB: push 3F800000h
004FA2B0: push 3F800000h
004FA2B5: push 3F800000h
004FA2BA: push edx
004FA2BB: mov edx,dword ptr [eax]
004FA2BD: sub esp,0Ch
004FA2C0: mov ecx,esp
004FA2C2: mov dword ptr [ecx],edx
004FA2C4: mov edx,dword ptr [eax+4]
004FA2C7: mov eax,dword ptr [eax+8]
004FA2CA: mov dword ptr [ecx+4],edx
004FA2CD: mov edx,dword ptr [esp+40h]
004FA2D1: mov dword ptr [ecx+8],eax
004FA2D4: mov ecx,dword ptr [esp+44h]
004FA2D8: push esi
004FA2D9: push ecx
004FA2DA: lea edi,[esi+8]
004FA2DD: push edx
004FA2DE: mov ecx,edi
004FA2E0: call 004EFE50
004FA2E5: cmp word ptr [esi+000000DCh],85h
004FA2EE: jne 004FA2FD
004FA2F0: mov eax,dword ptr ds:[008CBF1Ch]
004FA2F5: mov dword ptr [esi+20h],eax
004FA2F8: jmp 004FA39B
004FA2FD: mov ecx,dword ptr ds:[008CBF00h]
004FA303: jmp 004FA398
004FA308: xor edx,edx
004FA30A: mov dx,word ptr [esi+000000DCh]
 00
004FA311: push ecx
004FA312: mov ecx,0B5F8B8h
004FA317: push edx
004FA318: call 004D88C0
004FA31D: test al,al
004FA31F: je 004FA3DC
004FA325: mov eax,dword ptr [esi+4]
004FA328: mov ecx,dword ptr [eax+14h]
004FA32B: test ecx,ecx
004FA32D: je 004FA334
004FA32F: lea eax,[ecx+30h]
004FA332: jmp 004FA337
004FA334: add eax,4
004FA337: mov ecx,dword ptr [esp+20h]
004FA33B: push 0
004FA33D: push 0
004FA33F: push 0
004FA341: push 0
004FA343: push 3F800000h
004FA348: push 3F800000h
004FA34D: push 3F800000h
004FA352: push ecx
004FA353: mov ecx,dword ptr [eax]
004FA355: sub esp,0Ch
004FA358: mov edx,esp
004FA35A: mov dword ptr [edx],ecx
004FA35C: mov ecx,dword ptr [eax+4]
004FA35F: mov eax,dword ptr [eax+8]
004FA362: mov dword ptr [edx+4],ecx
004FA365: mov ecx,dword ptr [esp+44h]
004FA369: push esi
004FA36A: mov dword ptr [edx+8],eax
004FA36D: mov edx,dword ptr [esp+44h]
004FA371: push ecx
004FA372: lea edi,[esi+8]
004FA375: push edx
004FA376: mov ecx,edi
004FA378: call 004EFE50
004FA37D: cmp word ptr [esi+000000DCh],85h
004FA386: jne 004FA392
004FA388: mov eax,dword ptr ds:[008CBF18h]
004FA38D: mov dword ptr [esi+20h],eax
004FA390: jmp 004FA39B
004FA392: mov ecx,dword ptr ds:[008CBEFCh]
004FA398: mov dword ptr [esi+20h],ecx
004FA39B: mov edx,dword ptr [esp+20h]
004FA39F: mov eax,dword ptr [esp+1Ch]
004FA3A3: push edi
004FA3A4: mov ecx,0B62CB0h
004FA3A9: mov dword ptr [esi+1Ch],edx
004FA3AC: mov dword ptr [esi+24h],eax
004FA3AF: mov word ptr [esi+5Eh],4
004FA3B5: call 004EFB10
004FA3BA: mov dword ptr [esi+ebx*8+000000E8h],eax
 00
004FA3C1: pop edi
004FA3C2: pop esi
004FA3C3: pop ebx
004FA3C4: ret 14h
004FA3C7: mov ecx,dword ptr [esp+20h]
004FA3CB: mov dword ptr [eax+14h],ecx
004FA3CE: mov edx,dword ptr [esi+ebx*8+000000E8h]
 00
004FA3D5: mov eax,dword ptr [esp+1Ch]
004FA3D9: mov dword ptr [edx+1Ch],eax
004FA3DC: pop edi
004FA3DD: pop esi
004FA3DE: pop ebx
004FA3DF: ret 14h
004FA3F0: sub esp,0Ch
004FA3F3: push edi
004FA3F4: mov edi,ecx
004FA3F6: cmp word ptr [edi+000000E0h],0FFFFh
004FA3FE: jne 004FA423
004FA400: xor eax,eax
004FA402: mov ax,word ptr [edi+000000DCh]
 00
004FA409: push eax
004FA40A: call 004F4D10
004FA40F: add esp,4
004FA412: cmp ax,0FFFFh
004FA416: mov word ptr [edi+000000E0h],ax
 00
004FA41D: je 004FA624
004FA423: xor ecx,ecx
004FA425: mov cx,word ptr [edi+000000E0h]
 00
004FA42C: xor edx,edx
004FA42E: mov dx,word ptr [edi+000000DCh]
 00
004FA435: push ecx
004FA436: mov ecx,0B5F8B8h
004FA43B: push edx
004FA43C: call 004D88C0
004FA441: test al,al
004FA443: jne 004FA498
004FA445: mov ax,word ptr [edi+000000E0h]
 00
004FA44C: sub ax,7
004FA450: test ax,ax
004FA453: jl 004FA478
004FA455: cmp ax,0Ah
004FA459: jge 004FA478
004FA45B: movsx eax,ax
004FA45E: mov cx,word ptr [eax*4+00B6B9A0h]
004FA466: cmp cx,word ptr [edi+000000DCh]
 00
004FA46D: sete al
004FA470: test al,al
004FA472: jne 004FA624
004FA478: xor edx,edx
004FA47A: mov dx,word ptr [edi+000000DCh]
 00
004FA481: push edx
004FA482: call 004F4D10
004FA487: add esp,4
004FA48A: mov word ptr [edi+000000E0h],ax
 00
004FA491: pop edi
004FA492: add esp,0Ch
004FA495: ret 4
004FA498: cmp word ptr [edi+000000DCh],85h
004FA4A1: jne 004FA4B6
004FA4A3: mov eax,dword ptr ds:[008CBF38h]
004FA4A8: mov ecx,dword ptr ds:[008CBF2Ch]
004FA4AE: mov edx,dword ptr ds:[00B6BA84h]
004FA4B4: jmp 004FA4C7
004FA4B6: mov eax,dword ptr ds:[008CBF14h]
004FA4BB: mov ecx,dword ptr ds:[008CBF08h]
004FA4C1: mov edx,dword ptr ds:[00B6BA78h]
004FA4C7: mov dword ptr [esp+4],eax
004FA4CB: mov eax,dword ptr [esp+14h]
004FA4CF: push esi
004FA4D0: mov esi,dword ptr [eax+10h]
004FA4D3: test byte ptr [esi+000005B9h],1
 01
004FA4DA: mov dword ptr [esp+10h],ecx
004FA4DE: mov dword ptr [esp+0Ch],edx
004FA4E2: jne 004FA53C
004FA4E4: fld dword ptr [esi+000005A4h]
004FA4EA: fcomp dword ptr ds:[00858B50h]
004FA4F0: fld dword ptr [esi+000005A4h]
004FA4F6: fnstsw ax
004FA4F8: test ah,5
004FA4FB: jp 004FA4FF
004FA4FD: fchs
004FA4FF: fld dword ptr ds:[00858624h]
004FA505: fcomp st(1)
004FA507: fnstsw ax
004FA509: fstp st(0)
004FA50B: test ah,5
004FA50E: jnp 004FA54F
004FA510: fld dword ptr [esi+000005A4h]
004FA516: fcomp dword ptr ds:[00858B50h]
004FA51C: fld dword ptr [esi+000005A4h]
004FA522: fnstsw ax
004FA524: test ah,5
004FA527: jp 004FA52B
004FA529: fchs
004FA52B: fld dword ptr ds:[00858B50h]
004FA531: fcomp st(1)
004FA533: fnstsw ax
004FA535: fstp st(0)
004FA537: test ah,41h
004FA53A: jne 004FA54F
004FA53C: fld dword ptr ds:[00858B50h]
004FA542: mov dword ptr [esp+8],0C2C80000h
004FA54A: jmp 004FA5DC
004FA54F: fld dword ptr [esi+000005A4h]
004FA555: fcomp dword ptr ds:[00858B50h]
004FA55B: fld dword ptr [esi+000005A4h]
004FA561: fnstsw ax
004FA563: test ah,5
004FA566: jp 004FA56A
004FA568: fchs
004FA56A: fld dword ptr ds:[00858624h]
004FA570: fcomp st(1)
004FA572: fnstsw ax
004FA574: fstp st(0)
004FA576: test ah,5
004FA579: jp 004FA585
004FA57B: mov dword ptr [esp+18h],3F800000h
004FA583: jmp 004FA5BD
004FA585: fld dword ptr [esi+000005A4h]
004FA58B: fcomp dword ptr ds:[00858B50h]
004FA591: fld dword ptr [esi+000005A4h]
004FA597: fnstsw ax
004FA599: test ah,5
004FA59C: jp 004FA5A0
004FA59E: fchs
004FA5A0: fcom dword ptr ds:[00858C14h]
004FA5A6: fst dword ptr [esp+18h]
004FA5AA: fnstsw ax
004FA5AC: test ah,5
004FA5AF: jp 004FA5BB
004FA5B1: mov dword ptr [esp+8],0C2C80000h
004FA5B9: jmp 004FA5DC
004FA5BB: fstp st(0)
004FA5BD: mov ecx,dword ptr [esp+18h]
004FA5C1: push ecx
004FA5C2: call 004D9E50
004FA5C7: fmul dword ptr ds:[00858BA4h]
004FA5CD: add esp,4
004FA5D0: fadd dword ptr [esp+8]
004FA5D4: fstp dword ptr [esp+8]
004FA5D8: fld dword ptr [esp+18h]
004FA5DC: mov eax,esi
004FA5DE: xor ecx,ecx
004FA5E0: pop esi
004FA5E1: mov eax,dword ptr [eax+000005D0h]
004FA5E7: inc ecx
004FA5E8: test eax,eax
004FA5EA: jne 004FA5E1
004FA5EC: mov edx,dword ptr [esp+4]
004FA5F0: fmul dword ptr [esp+8]
004FA5F4: push edx
004FA5F5: movsx eax,cx
004FA5F8: cdq
004FA5F9: mov ecx,5
004FA5FE: idiv eax,ecx
004FA600: push ecx
004FA601: fadd dword ptr [esp+14h]
004FA605: mov ecx,edi
004FA607: fmul dword ptr [edx*4+008CBF54h]
 00
004FA60E: xor edx,edx
004FA610: mov dx,word ptr [edi+000000E0h]
 00
004FA617: fstp dword ptr [esp]
004FA61A: push 2
004FA61C: push edx
004FA61D: push 2
004FA61F: call 004FA1C0
004FA624: pop edi
004FA625: add esp,0Ch
004FA628: ret 4
004FA630: push 0FFFFFFFFh
004FA632: push 83C3A8h
004FA637: mov eax,dword ptr fs:[00000000h]
004FA63D: push eax
004FA63E: mov dword ptr fs:[00000000h],esp
 00
004FA645: sub esp,78h
004FA648: push ebx
004FA649: push esi
004FA64A: push edi
004FA64B: mov esi,ecx
004FA64D: mov dword ptr [esp+18h],0
004FA655: xor eax,eax
004FA657: mov ax,word ptr [esi+000000DEh]
 00
004FA65E: or ebx,0FFFFFFFFh
004FA661: cmp ax,bx
004FA664: mov dword ptr [esp+0000008Ch],0
004FA66F: je 004FA6EA
004FA671: push 28h
004FA673: push eax
004FA674: mov ecx,0B5F8B8h
004FA679: call 004D88C0
004FA67E: test al,al
004FA680: je 004FA6EA
004FA682: fld dword ptr [esp+0000009Ch]
 00
004FA689: mov ax,word ptr [esi+00000156h]
 00
004FA690: fadd dword ptr [esi+000000D8h]
004FA696: mov edi,dword ptr [esp+00000094h]
 00
004FA69D: cmp ax,di
004FA6A0: fstp dword ptr [esp+0Ch]
004FA6A4: jne 004FA714
004FA6A6: cmp ax,bx
004FA6A9: je 004FA6EA
004FA6AB: mov eax,dword ptr [esi+4]
004FA6AE: mov ecx,dword ptr [eax+14h]
004FA6B1: test ecx,ecx
004FA6B3: je 004FA6BA
004FA6B5: lea eax,[ecx+30h]
004FA6B8: jmp 004FA6BD
004FA6BA: add eax,4
004FA6BD: mov ecx,dword ptr [esp+00000098h]
 00
004FA6C4: mov edx,dword ptr [esp+0Ch]
004FA6C8: push ecx
004FA6C9: push edx
004FA6CA: mov edx,dword ptr [eax]
004FA6CC: sub esp,0Ch
004FA6CF: mov ecx,esp
004FA6D1: mov dword ptr [ecx],edx
004FA6D3: mov edx,dword ptr [eax+4]
004FA6D6: mov eax,dword ptr [eax+8]
004FA6D9: mov dword ptr [ecx+4],edx
004FA6DC: mov dword ptr [ecx+8],eax
004FA6DF: lea ecx,[esi+00000184h]
004FA6E5: call 004F29A0
004FA6EA: mov dword ptr [esp+0000008Ch],ebx
 00
004FA6F1: lea ecx,[esp+10h]
004FA6F5: call 004EF660
004FA6FA: mov ecx,dword ptr [esp+00000084h]
 00
004FA701: pop edi
004FA702: pop esi
004FA703: mov dword ptr fs:[00000000h],ecx
 00
004FA70A: pop ebx
004FA70B: add esp,84h
004FA711: ret 0Ch
004FA714: cmp word ptr [esi+0000020Ch],0
004FA71C: je 004FA729
004FA71E: lea ecx,[esi+00000184h]
004FA724: call 004F2960
004FA729: cmp di,bx
004FA72C: mov word ptr [esi+00000156h],di
 00
004FA733: je 004FA6EA
004FA735: push 0FFFFFFFFh
004FA737: push 0FFFFFFFFh
004FA739: push 3E8h
004FA73E: push 0C8h
004FA743: push esi
004FA744: lea ecx,[edi+1]
004FA747: push ecx
004FA748: push edi
004FA749: lea ebx,[esi+00000184h]
004FA74F: push 28h
004FA751: mov ecx,ebx
004FA753: call 004F28A0
004FA758: mov esi,dword ptr [esi+4]
004FA75B: mov eax,dword ptr [esi+14h]
004FA75E: test eax,eax
004FA760: je 004FA767
004FA762: add eax,30h
004FA765: jmp 004FA76A
004FA767: lea eax,[esi+4]
004FA76A: mov edx,dword ptr ds:[008CBEF8h]
004FA770: mov ecx,dword ptr [esp+00000098h]
 00
004FA777: push 0
004FA779: push 3F800000h
004FA77E: push edx
004FA77F: mov edx,dword ptr [esp+18h]
004FA783: push ecx
004FA784: push edx
004FA785: mov edx,dword ptr [eax]
004FA787: sub esp,0Ch
004FA78A: mov ecx,esp
004FA78C: mov dword ptr [ecx],edx
004FA78E: mov edx,dword ptr [eax+4]
004FA791: mov eax,dword ptr [eax+8]
004FA794: mov dword ptr [ecx+4],edx
004FA797: mov dword ptr [ecx+8],eax
004FA79A: mov ecx,ebx
004FA79C: call 004F2CB0
004FA7A1: mov dword ptr [esp+0000008Ch],0FFFFFFFFh
004FA7AC: jmp 004FA6F1
004FA7C0: sub esp,10h
004FA7C3: push esi
004FA7C4: mov esi,ecx
004FA7C6: cmp word ptr [esi+000000E0h],0FFFFh
004FA7CE: mov dword ptr [esp+8],0
004FA7D6: jne 004FA7FB
004FA7D8: xor eax,eax
004FA7DA: mov ax,word ptr [esi+000000DCh]
 00
004FA7E1: push eax
004FA7E2: call 004F4D10
004FA7E7: add esp,4
004FA7EA: cmp ax,0FFFFh
004FA7EE: mov word ptr [esi+000000E0h],ax
 00
004FA7F5: je 004FAA71
004FA7FB: xor ecx,ecx
004FA7FD: mov cx,word ptr [esi+000000E0h]
 00
004FA804: xor edx,edx
004FA806: mov dx,word ptr [esi+000000DCh]
 00
004FA80D: push ecx
004FA80E: mov ecx,0B5F8B8h
004FA813: push edx
004FA814: call 004D88C0
004FA819: test al,al
004FA81B: jne 004FA877
004FA81D: mov ax,word ptr [esi+000000E0h]
 00
004FA824: sub ax,7
004FA828: test ax,ax
004FA82B: jl 004FA850
004FA82D: cmp ax,0Ah
004FA831: jge 004FA850
004FA833: movsx eax,ax
004FA836: mov cx,word ptr [eax*4+00B6B9A0h]
004FA83E: cmp cx,word ptr [esi+000000DCh]
 00
004FA845: sete al
004FA848: test al,al
004FA84A: jne 004FAA71
004FA850: xor edx,edx
004FA852: mov dx,word ptr [esi+000000DCh]
 00
004FA859: push edx
004FA85A: call 004F4D10
004FA85F: add esp,4
004FA862: mov word ptr [esi+000000E0h],ax
 00
004FA869: mov byte ptr [esi+000000A9h],0
 00
004FA870: pop esi
004FA871: add esp,10h
004FA874: ret 4
004FA877: mov al,byte ptr [esi+000000A8h]
004FA87D: test al,al
004FA87F: push edi
004FA880: je 004FA89B
004FA882: xor edi,edi
004FA884: push edi
004FA885: mov ecx,esi
004FA887: call 004F55C0
004FA88C: inc edi
004FA88D: cmp di,0Ch
004FA891: jl 004FA884
004FA893: pop edi
004FA894: pop esi
004FA895: add esp,10h
004FA898: ret 4
004FA89B: mov edi,dword ptr [esp+1Ch]
004FA89F: mov eax,dword ptr [edi+10h]
004FA8A2: fld dword ptr [eax+000009C4h]
004FA8A8: mov eax,dword ptr ds:[00B6B990h]
004FA8AD: test eax,eax
004FA8AF: fmul dword ptr ds:[00862D18h]
004FA8B5: fstp dword ptr [esp+1Ch]
004FA8B9: je 004FA8CF
004FA8BB: mov al,byte ptr [esi+000000A5h]
004FA8C1: test al,al
004FA8C3: je 004FA8CF
004FA8C5: push edi
004FA8C6: mov ecx,esi
004FA8C8: call 004F5080
004FA8CD: jmp 004FA8DB
004FA8CF: mov word ptr [edi+20h],0
004FA8D5: mov word ptr [edi+22h],0
004FA8DB: cmp word ptr [edi+20h],0
004FA8E0: jle 004FA8EA
004FA8E2: mov dword ptr [esp+0Ch],3DCCCCCDh
004FA8EA: cmp word ptr [edi+22h],0
004FA8EF: jle 004FA8F9
004FA8F1: mov dword ptr [esp+0Ch],0BD4CCCCDh
004FA8F9: mov ecx,dword ptr [edi+10h]
004FA8FC: mov eax,dword ptr [ecx+14h]
004FA8FF: fld dword ptr [eax+14h]
004FA902: fadd dword ptr [eax+10h]
004FA905: fmul dword ptr ds:[00858B50h]
004FA90B: fadd dword ptr [eax+18h]
004FA90E: fmul qword ptr ds:[00862D10h]
004FA914: fstp dword ptr [esp+10h]
004FA918: fld dword ptr [eax+4]
004FA91B: fadd dword ptr [eax]
004FA91D: fmul dword ptr ds:[00858B50h]
004FA923: fadd dword ptr [eax+8]
004FA926: fst dword ptr [esp+8]
004FA92A: fcomp dword ptr ds:[00858B50h]
004FA930: fnstsw ax
004FA932: test ah,5
004FA935: jp 004FA945
004FA937: fld dword ptr [esp+8]
004FA93B: fmul qword ptr ds:[00862D10h]
004FA941: fstp dword ptr [esp+8]
004FA945: fld dword ptr [esp+1Ch]
004FA949: fcomp dword ptr ds:[00858624h]
004FA94F: fnstsw ax
004FA951: test ah,41h
004FA954: jne 004FAA14
004FA95A: mov dword ptr [esp+1Ch],3F800000h
004FA962: fld dword ptr [esp+1Ch]
004FA966: fsub dword ptr ds:[00858CC4h]
004FA96C: fmul dword ptr ds:[008595F0h]
004FA972: fcom qword ptr ds:[00862D08h]
004FA978: fnstsw ax
004FA97A: test ah,41h
004FA97D: je 004FA987
004FA97F: fstp st(0)
004FA981: fld qword ptr ds:[00862D08h]
004FA987: push ecx
004FA988: fstp dword ptr [esp]
004FA98B: call 004D9E50
004FA990: fmul dword ptr ds:[00858BA4h]
004FA996: add esp,4
004FA999: fadd dword ptr ds:[00B6BA04h]
004FA99F: fstp dword ptr [esp+14h]
004FA9A3: fld dword ptr [esp+8]
004FA9A7: fmul dword ptr ds:[00858B1Ch]
004FA9AD: fld dword ptr [esp+10h]
004FA9B1: fmul dword ptr ds:[00858FCCh]
004FA9B7: faddp st(1),st
004FA9B9: fadd dword ptr [esp+0Ch]
004FA9BD: fadd dword ptr ds:[00858624h]
004FA9C3: fstp dword ptr [esp+1Ch]
004FA9C7: fld dword ptr [esi+0000022Ch]
004FA9CD: fcomp dword ptr ds:[00858B50h]
004FA9D3: fnstsw ax
004FA9D5: test ah,5
004FA9D8: jp 004FA9E4
004FA9DA: mov edx,dword ptr [esp+1Ch]
004FA9DE: mov dword ptr [esi+0000022Ch],edx
004FA9E4: fld dword ptr [esp+1Ch]
004FA9E8: fcomp dword ptr [esi+0000022Ch]
004FA9EE: fld dword ptr [esi+0000022Ch]
004FA9F4: fnstsw ax
004FA9F6: test ah,1
004FA9F9: jne 004FAA34
004FA9FB: fadd dword ptr ds:[00862D04h]
004FAA01: fld dword ptr [esp+1Ch]
004FAA05: fcomp st(1)
004FAA07: fnstsw ax
004FAA09: test ah,5
004FAA0C: jnp 004FAA47
004FAA0E: fstp dword ptr [esp+1Ch]
004FAA12: jmp 004FAA49
004FAA14: fld dword ptr [esp+1Ch]
004FAA18: fcomp dword ptr ds:[00858CC4h]
004FAA1E: fnstsw ax
004FAA20: test ah,5
004FAA23: jp 004FA962
004FAA29: fld dword ptr ds:[00858B50h]
004FAA2F: jmp 004FA972
004FAA34: fsub dword ptr ds:[00862D04h]
004FAA3A: fld dword ptr [esp+1Ch]
004FAA3E: fcomp st(1)
004FAA40: fnstsw ax
004FAA42: test ah,41h
004FAA45: jne 004FAA0E
004FAA47: fstp st(0)
004FAA49: mov eax,dword ptr [esp+1Ch]
004FAA4D: mov edx,dword ptr [esp+14h]
004FAA51: mov ecx,eax
004FAA53: mov dword ptr [esi+0000022Ch],eax
004FAA59: push ecx
004FAA5A: xor eax,eax
004FAA5C: mov ax,word ptr [esi+000000E0h]
 00
004FAA63: push edx
004FAA64: push 0
004FAA66: mov ecx,esi
004FAA68: push eax
004FAA69: push 2
004FAA6B: call 004F93C0
004FAA70: pop edi
004FAA71: pop esi
004FAA72: add esp,10h
004FAA75: ret 4
004FAA80: sub esp,0Ch
004FAA83: push esi
004FAA84: mov esi,ecx
004FAA86: cmp word ptr [esi+000000E0h],0FFFFh
004FAA8E: mov dword ptr [esp+4],0
004FAA96: jne 004FAABB
004FAA98: xor eax,eax
004FAA9A: mov ax,word ptr [esi+000000DCh]
 00
004FAAA1: push eax
004FAAA2: call 004F4D10
004FAAA7: add esp,4
004FAAAA: cmp ax,0FFFFh
004FAAAE: mov word ptr [esi+000000E0h],ax
 00
004FAAB5: je 004FAD30
004FAABB: xor ecx,ecx
004FAABD: mov cx,word ptr [esi+000000E0h]
 00
004FAAC4: xor edx,edx
004FAAC6: mov dx,word ptr [esi+000000DCh]
 00
004FAACD: push ecx
004FAACE: mov ecx,0B5F8B8h
004FAAD3: push edx
004FAAD4: call 004D88C0
004FAAD9: test al,al
004FAADB: jne 004FAB37
004FAADD: mov ax,word ptr [esi+000000E0h]
 00
004FAAE4: sub ax,7
004FAAE8: test ax,ax
004FAAEB: jl 004FAB10
004FAAED: cmp ax,0Ah
004FAAF1: jge 004FAB10
004FAAF3: movsx eax,ax
004FAAF6: mov cx,word ptr [eax*4+00B6B9A0h]
004FAAFE: cmp cx,word ptr [esi+000000DCh]
 00
004FAB05: sete al
004FAB08: test al,al
004FAB0A: jne 004FAD30
004FAB10: xor edx,edx
004FAB12: mov dx,word ptr [esi+000000DCh]
 00
004FAB19: push edx
004FAB1A: call 004F4D10
004FAB1F: add esp,4
004FAB22: mov word ptr [esi+000000E0h],ax
 00
004FAB29: mov byte ptr [esi+000000A9h],0
 00
004FAB30: pop esi
004FAB31: add esp,0Ch
004FAB34: ret 4
004FAB37: mov al,byte ptr [esi+000000A8h]
004FAB3D: test al,al
004FAB3F: push edi
004FAB40: je 004FAB5B
004FAB42: xor edi,edi
004FAB44: push edi
004FAB45: mov ecx,esi
004FAB47: call 004F55C0
004FAB4C: inc edi
004FAB4D: cmp di,0Ch
004FAB51: jl 004FAB44
004FAB53: pop edi
004FAB54: pop esi
004FAB55: add esp,0Ch
004FAB58: ret 4
004FAB5B: mov edi,dword ptr [esp+18h]
004FAB5F: mov eax,dword ptr [edi+10h]
004FAB62: fld dword ptr [eax+0000084Ch]
004FAB68: mov eax,dword ptr ds:[00B6B990h]
004FAB6D: test eax,eax
004FAB6F: fmul dword ptr ds:[00862D1Ch]
004FAB75: fstp dword ptr [esp+18h]
004FAB79: je 004FAB8F
004FAB7B: mov al,byte ptr [esi+000000A5h]
004FAB81: test al,al
004FAB83: je 004FAB8F
004FAB85: push edi
004FAB86: mov ecx,esi
004FAB88: call 004F5080
004FAB8D: jmp 004FAB9B
004FAB8F: mov word ptr [edi+20h],0
004FAB95: mov word ptr [edi+22h],0
004FAB9B: cmp word ptr [edi+20h],0
004FABA0: jle 004FABAA
004FABA2: mov dword ptr [esp+8],3DCCCCCDh
004FABAA: cmp word ptr [edi+22h],0
004FABAF: jle 004FABB9
004FABB1: mov dword ptr [esp+8],0BD4CCCCDh
004FABB9: mov ecx,dword ptr [edi+10h]
004FABBC: mov eax,dword ptr [ecx+14h]
004FABBF: fld dword ptr [eax+24h]
004FABC2: add eax,20h
004FABC5: fadd dword ptr [eax]
004FABC7: fmul dword ptr ds:[00858B50h]
004FABCD: fadd dword ptr [eax+8]
004FABD0: fsubr dword ptr ds:[00858624h]
004FABD6: fmul dword ptr ds:[00858B8Ch]
004FABDC: fdiv dword ptr ds:[008CBF6Ch]
004FABE2: fcom dword ptr ds:[00858624h]
004FABE8: fnstsw ax
004FABEA: test ah,5
004FABED: jp 004FABF5
004FABEF: fstp dword ptr [esp+0Ch]
004FABF3: jmp 004FABFF
004FABF5: fstp st(0)
004FABF7: mov dword ptr [esp+0Ch],3F800000h
004FABFF: fld dword ptr [esp+18h]
004FAC03: fcomp dword ptr ds:[00858624h]
004FAC09: fnstsw ax
004FAC0B: test ah,41h
004FAC0E: jne 004FACD3
004FAC14: mov dword ptr [esp+18h],3F800000h
004FAC1C: fld dword ptr [esp+18h]
004FAC20: fsub dword ptr ds:[00858CC4h]
004FAC26: fmul dword ptr ds:[008595F0h]
004FAC2C: fcom qword ptr ds:[00862D08h]
004FAC32: fnstsw ax
004FAC34: test ah,41h
004FAC37: je 004FAC41
004FAC39: fstp st(0)
004FAC3B: fld qword ptr ds:[00862D08h]
004FAC41: fstp dword ptr [esp+18h]
004FAC45: mov edx,dword ptr [esp+18h]
004FAC49: push edx
004FAC4A: call 004D9E50
004FAC4F: fmul dword ptr ds:[00858BA4h]
004FAC55: add esp,4
004FAC58: fadd dword ptr ds:[008CBF68h]
004FAC5E: fstp dword ptr [esp+10h]
004FAC62: fld dword ptr [esp+18h]
004FAC66: fmul dword ptr ds:[00862D00h]
004FAC6C: fld dword ptr [esp+0Ch]
004FAC70: fmul dword ptr ds:[00858CC4h]
004FAC76: faddp st(1),st
004FAC78: fadd dword ptr [esp+8]
004FAC7C: fadd dword ptr ds:[00858C98h]
004FAC82: fstp dword ptr [esp+18h]
004FAC86: fld dword ptr [esi+0000022Ch]
004FAC8C: fcomp dword ptr ds:[00858B50h]
004FAC92: fnstsw ax
004FAC94: test ah,5
004FAC97: jp 004FACA3
004FAC99: mov eax,dword ptr [esp+18h]
004FAC9D: mov dword ptr [esi+0000022Ch],eax
004FACA3: fld dword ptr [esp+18h]
004FACA7: fcomp dword ptr [esi+0000022Ch]
004FACAD: fld dword ptr [esi+0000022Ch]
004FACB3: fnstsw ax
004FACB5: test ah,1
004FACB8: jne 004FACF3
004FACBA: fadd dword ptr ds:[00862D04h]
004FACC0: fld dword ptr [esp+18h]
004FACC4: fcomp st(1)
004FACC6: fnstsw ax
004FACC8: test ah,5
004FACCB: jnp 004FAD06
004FACCD: fstp dword ptr [esp+18h]
004FACD1: jmp 004FAD08
004FACD3: fld dword ptr [esp+18h]
004FACD7: fcomp dword ptr ds:[00858CC4h]
004FACDD: fnstsw ax
004FACDF: test ah,5
004FACE2: jp 004FAC1C
004FACE8: fld dword ptr ds:[00858B50h]
004FACEE: jmp 004FAC2C
004FACF3: fsub dword ptr ds:[00862D04h]
004FACF9: fld dword ptr [esp+18h]
004FACFD: fcomp st(1)
004FACFF: fnstsw ax
004FAD01: test ah,41h
004FAD04: jne 004FACCD
004FAD06: fstp st(0)
004FAD08: mov ecx,dword ptr [esp+18h]
004FAD0C: mov eax,dword ptr [esp+10h]
004FAD10: mov edx,ecx
004FAD12: mov dword ptr [esi+0000022Ch],ecx
004FAD18: xor ecx,ecx
004FAD1A: mov cx,word ptr [esi+000000E0h]
 00
004FAD21: push edx
004FAD22: push eax
004FAD23: push 0
004FAD25: push ecx
004FAD26: push 2
004FAD28: mov ecx,esi
004FAD2A: call 004F93C0
004FAD2F: pop edi
004FAD30: pop esi
004FAD31: add esp,0Ch
004FAD34: ret 4
004FAD40: movsx eax,word ptr [esp+4]
004FAD45: fld dword ptr [esp+18h]
004FAD49: push ebx
004FAD4A: push ebp
004FAD4B: push esi
004FAD4C: mov esi,ecx
004FAD4E: lea ebx,[esi+eax*8+000000E8h]
 00
004FAD55: mov eax,dword ptr [ebx]
004FAD57: test eax,eax
004FAD59: fadd dword ptr [esi+000000D8h]
004FAD5F: push edi
004FAD60: fstp dword ptr [esp+28h]
004FAD64: jne 004FAE09
004FAD6A: mov ebp,dword ptr [esp+18h]
004FAD6E: mov ecx,dword ptr [esp+1Ch]
004FAD72: push ebp
004FAD73: push ecx
004FAD74: mov ecx,0B5F8B8h
004FAD79: call 004D88C0
004FAD7E: test al,al
004FAD80: je 004FAE19
004FAD86: mov eax,dword ptr [esi+4]
004FAD89: mov ecx,dword ptr [eax+14h]
004FAD8C: test ecx,ecx
004FAD8E: je 004FAD95
004FAD90: lea eax,[ecx+30h]
004FAD93: jmp 004FAD98
004FAD95: add eax,4
004FAD98: mov edx,dword ptr [esp+28h]
004FAD9C: push 0
004FAD9E: push 0
004FADA0: push 0
004FADA2: push 0
004FADA4: push 3F800000h
004FADA9: push 3F800000h
004FADAE: push 3F800000h
004FADB3: push edx
004FADB4: mov edx,dword ptr [eax]
004FADB6: sub esp,0Ch
004FADB9: mov ecx,esp
004FADBB: mov dword ptr [ecx],edx
004FADBD: mov edx,dword ptr [eax+4]
004FADC0: mov eax,dword ptr [eax+8]
004FADC3: mov dword ptr [ecx+4],edx
004FADC6: mov dword ptr [ecx+8],eax
004FADC9: mov ecx,dword ptr [esp+4Ch]
004FADCD: push esi
004FADCE: push ecx
004FADCF: lea edi,[esi+8]
004FADD2: push ebp
004FADD3: mov ecx,edi
004FADD5: call 004EFE50
004FADDA: mov ecx,dword ptr [esp+24h]
004FADDE: mov edx,dword ptr [esp+2Ch]
004FADE2: mov eax,dword ptr [esp+28h]
004FADE6: mov dword ptr [esi+24h],ecx
004FADE9: push edi
004FADEA: mov ecx,0B62CB0h
004FADEF: mov dword ptr [esi+20h],edx
004FADF2: mov dword ptr [esi+1Ch],eax
004FADF5: mov word ptr [esi+5Eh],4
004FADFB: call 004EFB10
004FAE00: mov dword ptr [ebx],eax
004FAE02: pop edi
004FAE03: pop esi
004FAE04: pop ebp
004FAE05: pop ebx
004FAE06: ret 1Ch
004FAE09: mov edx,dword ptr [esp+28h]
004FAE0D: mov ecx,dword ptr [esp+24h]
004FAE11: mov dword ptr [eax+14h],edx
004FAE14: mov eax,dword ptr [ebx]
004FAE16: mov dword ptr [eax+1Ch],ecx
004FAE19: pop edi
004FAE1A: pop esi
004FAE1B: pop ebp
004FAE1C: pop ebx
004FAE1D: ret 1Ch
004FAE20: push ecx
004FAE21: push ebx
004FAE22: push ebp
004FAE23: mov ebp,dword ptr [esp+10h]
004FAE27: push esi
004FAE28: mov esi,ecx
004FAE2A: mov ecx,dword ptr [ebp+10h]
004FAE2D: fld dword ptr [ecx+000004C0h]
004FAE33: fcomp dword ptr ds:[00859F80h]
004FAE39: fnstsw ax
004FAE3B: test ah,5
004FAE3E: jp 004FAE46
004FAE40: mov cl,1
004FAE42: mov bl,cl
004FAE44: jmp 004FAE91
004FAE46: fld dword ptr [ecx+000004C0h]
004FAE4C: fcomp dword ptr ds:[00862D28h]
004FAE52: fnstsw ax
004FAE54: test ah,5
004FAE57: jp 004FAE5F
004FAE59: mov cl,1
004FAE5B: mov bl,cl
004FAE5D: jmp 004FAE91
004FAE5F: fld dword ptr [ecx+000004C0h]
004FAE65: fcomp dword ptr ds:[00862D24h]
004FAE6B: fnstsw ax
004FAE6D: test ah,5
004FAE70: jp 004FAE76
004FAE72: mov cl,1
004FAE74: jmp 004FAE8F
004FAE76: fld dword ptr [ecx+000004C0h]
004FAE7C: fcomp dword ptr ds:[00862D20h]
004FAE82: fnstsw ax
004FAE84: test ah,5
004FAE87: jp 004FAE8D
004FAE89: mov cl,1
004FAE8B: jmp 004FAE8F
004FAE8D: xor cl,cl
004FAE8F: xor bl,bl
004FAE91: mov eax,dword ptr [esi+4]
004FAE94: test byte ptr [eax+0000042Bh],40h
 40
004FAE9B: je 004FAEA1
004FAE9D: xor cl,cl
004FAE9F: mov bl,1
004FAEA1: mov al,byte ptr [esi+00000080h]
004FAEA7: test al,al
004FAEA9: je 004FAEAD
004FAEAB: xor bl,bl
004FAEAD: mov eax,dword ptr [ebp+14h]
004FAEB0: test eax,eax
004FAEB2: je 004FAEE8
004FAEB4: fld dword ptr [ebp+1Ch]
004FAEB7: fdiv dword ptr [eax+58h]
004FAEBA: fabs
004FAEBC: fld dword ptr ds:[00858624h]
004FAEC2: fcomp st(1)
004FAEC4: fnstsw ax
004FAEC6: test ah,5
004FAEC9: jnp 004FAFBC
004FAECF: fld st(0)
004FAED1: fld dword ptr ds:[00858B50h]
004FAED7: fcomp st(1)
004FAED9: fnstsw ax
004FAEDB: fstp st(0)
004FAEDD: test ah,41h
004FAEE0: jne 004FAFBC
004FAEE6: fstp st(0)
004FAEE8: mov dword ptr [esp+14h],0
004FAEF0: mov al,byte ptr [esi+000000A8h]
004FAEF6: test al,al
004FAEF8: je 004FAFE3
004FAEFE: xor bl,bl
004FAF00: mov ecx,dword ptr [esi+00000130h]
004FAF06: test ecx,ecx
004FAF08: je 004FAF28
004FAF0A: push 0
004FAF0C: push 4
004FAF0E: call 004EF2B0
004FAF13: mov ecx,dword ptr [esi+00000130h]
004FAF19: call 004EF1C0
004FAF1E: mov dword ptr [esi+00000130h],0
004FAF28: test bl,bl
004FAF2A: je 004FB033
004FAF30: mov edx,dword ptr [ebp+10h]
004FAF33: test byte ptr [edx+00000428h],10h
 10
004FAF3A: je 004FB033
004FAF40: fld dword ptr ds:[00B6BAA8h]
004FAF46: mov eax,dword ptr [esi+4]
004FAF49: fmul dword ptr [esp+14h]
004FAF4D: mov cl,byte ptr [eax+0000042Bh]
004FAF53: test cl,40h
004FAF56: fadd dword ptr ds:[008CC00Ch]
004FAF5C: fstp dword ptr [esp+0Ch]
004FAF60: fld dword ptr ds:[00B6BAACh]
004FAF66: fmul dword ptr [esp+14h]
004FAF6A: fadd dword ptr ds:[008CC014h]
004FAF70: fstp dword ptr [esp+14h]
004FAF74: je 004FAF92
004FAF76: fld dword ptr [esp+0Ch]
004FAF7A: fsub dword ptr ds:[008CBC44h]
004FAF80: fstp dword ptr [esp+0Ch]
004FAF84: fld dword ptr ds:[008CBC48h]
004FAF8A: fmul dword ptr [esp+14h]
004FAF8E: fstp dword ptr [esp+14h]
004FAF92: mov ecx,dword ptr ds:[008CC01Ch]
004FAF98: mov edx,dword ptr [esp+0Ch]
004FAF9C: mov eax,dword ptr [esp+14h]
004FAFA0: push ecx
004FAFA1: push edx
004FAFA2: push eax
004FAFA3: push 2
004FAFA5: push 8Ah
004FAFAA: push 13h
004FAFAC: push 0Ah
004FAFAE: mov ecx,esi
004FAFB0: call 004FAD40
004FAFB5: pop esi
004FAFB6: pop ebp
004FAFB7: pop ebx
004FAFB8: pop ecx
004FAFB9: ret 4
004FAFBC: fld dword ptr ds:[00858624h]
004FAFC2: fcomp st(1)
004FAFC4: fnstsw ax
004FAFC6: test ah,5
004FAFC9: jp 004FAFDA
004FAFCB: fstp st(0)
004FAFCD: mov dword ptr [esp+14h],3F800000h
004FAFD5: jmp 004FAEF0
004FAFDA: fstp dword ptr [esp+14h]
004FAFDE: jmp 004FAEF0
004FAFE3: test cl,cl
004FAFE5: je 004FAF00
004FAFEB: fld dword ptr ds:[00B6BAA0h]
004FAFF1: mov ecx,dword ptr ds:[008CC008h]
004FAFF7: fmul dword ptr [esp+14h]
004FAFFB: push ecx
004FAFFC: sub esp,8
004FAFFF: mov ecx,esi
004FB001: fadd dword ptr ds:[008CBFF8h]
004FB007: fstp dword ptr [esp+4]
004FB00B: fld dword ptr ds:[00B6BAA4h]
004FB011: fmul dword ptr [esp+20h]
004FB015: fadd dword ptr ds:[008CC000h]
004FB01B: fstp dword ptr [esp]
004FB01E: push 1Ch
004FB020: push 8Fh
004FB025: push 5
004FB027: push 9
004FB029: call 004FAD40
004FB02E: jmp 004FAF28
004FB033: mov ecx,dword ptr [esi+00000138h]
004FB039: test ecx,ecx
004FB03B: je 004FB05B
004FB03D: push 0
004FB03F: push 4
004FB041: call 004EF2B0
004FB046: mov ecx,dword ptr [esi+00000138h]
004FB04C: call 004EF1C0
004FB051: mov dword ptr [esi+00000138h],0
004FB05B: pop esi
004FB05C: pop ebp
004FB05D: pop ebx
004FB05E: pop ecx
004FB05F: ret 4
004FB070: push ecx
004FB071: push esi
004FB072: push edi
004FB073: mov edi,dword ptr [esp+10h]
004FB077: mov esi,ecx
004FB079: mov ecx,dword ptr [edi+10h]
004FB07C: test dword ptr [ecx+0000038Ch],80000h
004FB086: je 004FB1E5
004FB08C: fld dword ptr [ecx+000008A4h]
004FB092: fcomp dword ptr ds:[00858B50h]
004FB098: fnstsw ax
004FB09A: test ah,5
004FB09D: jp 004FB1E5
004FB0A3: test byte ptr [ecx+00000428h],10h
 10
004FB0AA: je 004FB1E5
004FB0B0: test byte ptr [ecx+0000042Bh],40h
 40
004FB0B7: jne 004FB1E5
004FB0BD: mov al,byte ptr [esi+00000244h]
004FB0C3: test al,al
004FB0C5: jne 004FB0D2
004FB0C7: push 0
004FB0C9: push 66h
004FB0CB: mov ecx,esi
004FB0CD: call 004F6420
004FB0D2: movsx eax,word ptr [edi+20h]
004FB0D6: mov dword ptr [esp+10h],eax
004FB0DA: fild dword ptr [esp+10h]
004FB0DE: fmul dword ptr ds:[00859A3Ch]
004FB0E4: fld dword ptr [esi+00000248h]
004FB0EA: fcomp dword ptr ds:[00858B50h]
004FB0F0: fnstsw ax
004FB0F2: test ah,5
004FB0F5: jp 004FB0FD
004FB0F7: fst dword ptr [esi+00000248h]
004FB0FD: fcom dword ptr [esi+00000248h]
004FB103: fnstsw ax
004FB105: test ah,1
004FB108: jne 004FB12D
004FB10A: fld dword ptr ds:[008CC040h]
004FB110: fadd dword ptr [esi+00000248h]
004FB116: fstp dword ptr [esp+10h]
004FB11A: fcom dword ptr [esp+10h]
004FB11E: fnstsw ax
004FB120: test ah,5
004FB123: jnp 004FB14E
004FB125: fstp st(0)
004FB127: fld dword ptr [esp+10h]
004FB12B: jmp 004FB14E
004FB12D: fld dword ptr [esi+00000248h]
004FB133: fsub dword ptr ds:[008CC044h]
004FB139: fstp dword ptr [esp+10h]
004FB13D: fcom dword ptr [esp+10h]
004FB141: fnstsw ax
004FB143: test ah,41h
004FB146: je 004FB14E
004FB148: fstp st(0)
004FB14A: fld dword ptr [esp+10h]
004FB14E: fst dword ptr [esi+00000248h]
004FB154: push 40400000h
004FB159: fld dword ptr ds:[00B6BABCh]
004FB15F: sub esp,8
004FB162: fmul st,st(1)
004FB164: mov ecx,esi
004FB166: fadd dword ptr ds:[008CC038h]
004FB16C: fstp dword ptr [esp+14h]
004FB170: fld dword ptr ds:[00B6BAB8h]
004FB176: fmul st,st(1)
004FB178: fadd dword ptr ds:[008CC030h]
004FB17E: fstp dword ptr [esp+1Ch]
004FB182: fld dword ptr ds:[00B6BAB0h]
004FB188: fmul st,st(1)
004FB18A: fadd dword ptr ds:[008CC020h]
004FB190: fstp dword ptr [esp+4]
004FB194: fld dword ptr ds:[00B6BAB4h]
004FB19A: fmul st,st(1)
004FB19C: fadd dword ptr ds:[008CC028h]
004FB1A2: fstp dword ptr [esp]
004FB1A5: push 0Ah
004FB1A7: push 8Fh
004FB1AC: fstp st(0)
004FB1AE: push 5
004FB1B0: push 7
004FB1B2: call 004FAD40
004FB1B7: mov ecx,dword ptr [esp+10h]
004FB1BB: mov edx,dword ptr [esp+8]
004FB1BF: push 40400000h
004FB1C4: push ecx
004FB1C5: push edx
004FB1C6: push 1Ah
004FB1C8: push 8Ah
004FB1CD: push 13h
004FB1CF: push 8
004FB1D1: mov ecx,esi
004FB1D3: call 004FAD40
004FB1D8: pop edi
004FB1D9: mov byte ptr [esi+00000244h],1
 01
004FB1E0: pop esi
004FB1E1: pop ecx
004FB1E2: ret 4
004FB1E5: mov al,byte ptr [esi+00000244h]
004FB1EB: test al,al
004FB1ED: je 004FB23F
004FB1EF: mov ecx,dword ptr [esi+00000120h]
004FB1F5: test ecx,ecx
004FB1F7: je 004FB217
004FB1F9: push 0
004FB1FB: push 4
004FB1FD: call 004EF2B0
004FB202: mov ecx,dword ptr [esi+00000120h]
004FB208: call 004EF1C0
004FB20D: mov dword ptr [esi+00000120h],0
004FB217: mov ecx,dword ptr [esi+00000128h]
004FB21D: test ecx,ecx
004FB21F: je 004FB23F
004FB221: push 0
004FB223: push 4
004FB225: call 004EF2B0
004FB22A: mov ecx,dword ptr [esi+00000128h]
004FB230: call 004EF1C0
004FB235: mov dword ptr [esi+00000128h],0
004FB23F: pop edi
004FB240: mov byte ptr [esi+00000244h],0
 00
004FB247: mov dword ptr [esi+00000248h],0BF800000h
004FB251: pop esi
004FB252: pop ecx
004FB253: ret 4
004FB260: push ecx
004FB261: mov edx,dword ptr [esp+8]
004FB265: push esi
004FB266: mov esi,ecx
004FB268: mov ecx,dword ptr [edx+10h]
004FB26B: mov ax,word ptr [ecx+22h]
004FB26F: cmp ax,1BBh
004FB273: je 004FB297
004FB275: cmp ax,1E6h
004FB279: je 004FB297
004FB27B: cmp ax,196h
004FB27F: je 004FB297
004FB281: cmp ax,20Ch
004FB285: je 004FB297
004FB287: cmp ax,250h
004FB28B: je 004FB297
004FB28D: cmp ax,212h
004FB291: jne 004FB5E1
004FB297: movzx eax,word ptr [ecx+0000086Eh]
 00
004FB29E: movzx ecx,word ptr [ecx+0000086Ch]
 00
004FB2A5: sub ecx,eax
004FB2A7: mov dword ptr [esp+0Ch],ecx
004FB2AB: fild dword ptr [esp+0Ch]
004FB2AF: fdiv dword ptr ds:[008CC050h]
004FB2B5: fld st(0)
004FB2B7: fabs
004FB2B9: fld dword ptr ds:[00858624h]
004FB2BF: fcomp st(1)
004FB2C1: fnstsw ax
004FB2C3: test ah,5
004FB2C6: jnp 004FB2E1
004FB2C8: fld dword ptr ds:[00858B50h]
004FB2CE: fcomp st(1)
004FB2D0: fnstsw ax
004FB2D2: test ah,41h
004FB2D5: jne 004FB2E9
004FB2D7: fstp st(0)
004FB2D9: fld dword ptr ds:[00858B50h]
004FB2DF: jmp 004FB2E9
004FB2E1: fstp st(0)
004FB2E3: fld dword ptr ds:[00858624h]
004FB2E9: fcom dword ptr [esi+00000238h]
004FB2EF: fnstsw ax
004FB2F1: test ah,41h
004FB2F4: jne 004FB319
004FB2F6: fld dword ptr ds:[008CC048h]
004FB2FC: fadd dword ptr [esi+00000238h]
004FB302: fstp dword ptr [esp+0Ch]
004FB306: fcom dword ptr [esp+0Ch]
004FB30A: fnstsw ax
004FB30C: test ah,5
004FB30F: jnp 004FB33A
004FB311: fstp st(0)
004FB313: fld dword ptr [esp+0Ch]
004FB317: jmp 004FB33A
004FB319: fld dword ptr [esi+00000238h]
004FB31F: fsub dword ptr ds:[008CC04Ch]
004FB325: fstp dword ptr [esp+0Ch]
004FB329: fcom dword ptr [esp+0Ch]
004FB32D: fnstsw ax
004FB32F: test ah,41h
004FB332: je 004FB33A
004FB334: fstp st(0)
004FB336: fld dword ptr [esp+0Ch]
004FB33A: fstp dword ptr [esi+00000238h]
004FB340: mov edx,dword ptr [edx+10h]
004FB343: movsx eax,word ptr [edx+22h]
004FB347: add eax,0FFFFFE6Ah
004FB34C: cmp eax,0BAh
004FB351: ja 004FB5D5
004FB357: movzx eax,byte ptr [eax+004FB600h]
 00
004FB35E: push ebx
004FB35F: push ebp
004FB360: push edi
004FB361: jmp dword ptr [eax*4+004FB5E8h]
 00
004FB368: fcomp qword ptr ds:[00859EF8h]
004FB36E: mov edi,13h
004FB373: mov ebx,8Ah
004FB378: mov ebp,0Fh
004FB37D: fnstsw ax
004FB37F: test ah,41h
004FB382: jne 004FB3AB
004FB384: mov edx,dword ptr [esi+00000238h]
004FB38A: mov ecx,dword ptr ds:[008CC06Ch]
004FB390: push edx
004FB391: mov dword ptr [esp+14h],ecx
004FB395: call 004D9E50
004FB39A: fmul dword ptr ds:[00858BA4h]
004FB3A0: fadd dword ptr ds:[008CC064h]
004FB3A6: jmp 004FB56E
004FB3AB: mov ecx,dword ptr [esi+00000238h]
004FB3B1: mov eax,dword ptr ds:[008CC070h]
004FB3B6: push ecx
004FB3B7: mov dword ptr [esp+14h],eax
004FB3BB: call 004D9E50
004FB3C0: fmul dword ptr ds:[00858BA4h]
004FB3C6: fadd dword ptr ds:[008CC068h]
004FB3CC: jmp 004FB56E
004FB3D1: fcomp qword ptr ds:[00859EF8h]
004FB3D7: mov edi,13h
004FB3DC: mov ebx,8Ah
004FB3E1: mov ebp,0Fh
004FB3E6: fnstsw ax
004FB3E8: test ah,41h
004FB3EB: jne 004FB414
004FB3ED: mov eax,dword ptr [esi+00000238h]
004FB3F3: mov edx,dword ptr ds:[008CC07Ch]
004FB3F9: push eax
004FB3FA: mov dword ptr [esp+14h],edx
004FB3FE: call 004D9E50
004FB403: fmul dword ptr ds:[00858BA4h]
004FB409: fadd dword ptr ds:[008CC074h]
004FB40F: jmp 004FB56E
004FB414: mov edx,dword ptr [esi+00000238h]
004FB41A: mov ecx,dword ptr ds:[008CC080h]
004FB420: push edx
004FB421: mov dword ptr [esp+14h],ecx
004FB425: call 004D9E50
004FB42A: fmul dword ptr ds:[00858BA4h]
004FB430: fadd dword ptr ds:[008CC078h]
004FB436: jmp 004FB56E
004FB43B: fcomp qword ptr ds:[00859EF8h]
004FB441: mov edi,13h
004FB446: mov ebx,8Ah
004FB44B: mov ebp,0Fh
004FB450: fnstsw ax
004FB452: test ah,41h
004FB455: jne 004FB47D
004FB457: mov ecx,dword ptr [esi+00000238h]
004FB45D: mov eax,dword ptr ds:[008CC08Ch]
004FB462: push ecx
004FB463: mov dword ptr [esp+14h],eax
004FB467: call 004D9E50
004FB46C: fmul dword ptr ds:[00858BA4h]
004FB472: fadd dword ptr ds:[008CC084h]
004FB478: jmp 004FB56E
004FB47D: mov eax,dword ptr [esi+00000238h]
004FB483: mov edx,dword ptr ds:[008CC090h]
004FB489: push eax
004FB48A: mov dword ptr [esp+14h],edx
004FB48E: call 004D9E50
004FB493: fmul dword ptr ds:[00858BA4h]
004FB499: fadd dword ptr ds:[008CC088h]
004FB49F: jmp 004FB56E
004FB4A4: fcomp qword ptr ds:[00859EF8h]
004FB4AA: mov edi,13h
004FB4AF: mov ebx,8Ah
004FB4B4: mov ebp,0Fh
004FB4B9: fnstsw ax
004FB4BB: test ah,41h
004FB4BE: jne 004FB4E7
004FB4C0: mov edx,dword ptr [esi+00000238h]
004FB4C6: mov ecx,dword ptr ds:[008CC09Ch]
004FB4CC: push edx
004FB4CD: mov dword ptr [esp+14h],ecx
004FB4D1: call 004D9E50
004FB4D6: fmul dword ptr ds:[00858BA4h]
004FB4DC: fadd dword ptr ds:[008CC094h]
004FB4E2: jmp 004FB56E
004FB4E7: mov ecx,dword ptr [esi+00000238h]
004FB4ED: mov eax,dword ptr ds:[008CC0A0h]
004FB4F2: push ecx
004FB4F3: mov dword ptr [esp+14h],eax
004FB4F7: call 004D9E50
004FB4FC: fmul dword ptr ds:[00858BA4h]
004FB502: fadd dword ptr ds:[008CC098h]
004FB508: jmp 004FB56E
004FB50A: fcomp qword ptr ds:[00859EF8h]
004FB510: mov di,word ptr [esi+000000E0h]
 00
004FB517: mov ebx,39h
004FB51C: mov ebp,2
004FB521: fnstsw ax
004FB523: test ah,41h
004FB526: jne 004FB54C
004FB528: mov eax,dword ptr [esi+00000238h]
004FB52E: mov edx,dword ptr ds:[008CC05Ch]
004FB534: push eax
004FB535: mov dword ptr [esp+14h],edx
004FB539: call 004D9E50
004FB53E: fmul dword ptr ds:[00858BA4h]
004FB544: fadd dword ptr ds:[008CC054h]
004FB54A: jmp 004FB56E
004FB54C: mov edx,dword ptr [esi+00000238h]
004FB552: mov ecx,dword ptr ds:[008CC060h]
004FB558: push edx
004FB559: mov dword ptr [esp+14h],ecx
004FB55D: call 004D9E50
004FB562: fmul dword ptr ds:[00858BA4h]
004FB568: fadd dword ptr ds:[008CC058h]
004FB56E: fst dword ptr [esp+1Ch]
004FB572: add esp,4
004FB575: fcomp dword ptr ds:[00859014h]
004FB57B: fnstsw ax
004FB57D: test ah,41h
004FB580: jne 004FB5A5
004FB582: mov eax,dword ptr [esp+18h]
004FB586: mov ecx,dword ptr [esp+10h]
004FB58A: push 3FC00000h
004FB58F: push eax
004FB590: push ecx
004FB591: push ebp
004FB592: push ebx
004FB593: push edi
004FB594: push 0Bh
004FB596: mov ecx,esi
004FB598: call 004FAD40
004FB59D: pop edi
004FB59E: pop ebp
004FB59F: pop ebx
004FB5A0: pop esi
004FB5A1: pop ecx
004FB5A2: ret 4
004FB5A5: mov ecx,dword ptr [esi+00000140h]
004FB5AB: test ecx,ecx
004FB5AD: je 004FB5DE
004FB5AF: push 0
004FB5B1: push 4
004FB5B3: call 004EF2B0
004FB5B8: mov ecx,dword ptr [esi+00000140h]
004FB5BE: call 004EF1C0
004FB5C3: pop edi
004FB5C4: pop ebp
004FB5C5: pop ebx
004FB5C6: mov dword ptr [esi+00000140h],0
004FB5D0: pop esi
004FB5D1: pop ecx
004FB5D2: ret 4
004FB5D5: fstp st(0)
004FB5D7: pop esi
004FB5D8: pop ecx
004FB5D9: ret 4
004FB5DC: fstp st(0)
004FB5DE: pop edi
004FB5DF: pop ebp
004FB5E0: pop ebx
004FB5E1: pop esi
004FB5E2: pop ecx
004FB5E3: ret 4
004FB5E6: mov edi,edi
004FB5E8: cmp esi,dword ptr [edi+ecx*2+4FB36800h]
 4F
004FB5EF: add cl,dl
004FB5F1: mov bl,4Fh
004FB5F3: add byte ptr [edx],cl
004FB5F5: mov ch,4Fh
004FB5F7: add byte ptr [esp+esi*4+B5DC004Fh],ah
 B5
004FB5FE: dec edi
004FB5FF: add byte ptr [eax],al
004FB601: add eax,5050505h
004FB606: add eax,5050505h
004FB60B: add eax,5050505h
004FB610: add eax,5050505h
004FB615: add eax,5050505h
004FB61A: add eax,5050505h
004FB61F: add eax,5050505h
004FB624: add eax,5050501h
004FB629: add eax,5050505h
004FB62E: add eax,5050505h
004FB633: add eax,5050505h
004FB638: add eax,5050505h
004FB63D: add eax,5050505h
004FB642: add eax,5050505h
004FB647: add eax,5050505h
004FB64C: add eax,2050505h
004FB651: add eax,5050505h
004FB656: add eax,5050505h
004FB65B: add eax,5050505h
004FB660: add eax,5050505h
004FB665: add eax,5050505h
004FB66A: add eax,5050505h
004FB66F: add eax,5050505h
004FB674: add eax,5050505h
004FB679: add eax,5030505h
004FB67E: add eax,5050505h
004FB683: add eax,5050505h
004FB688: add eax,5050505h
004FB68D: add eax,5050505h
004FB692: add eax,5050505h
004FB697: add eax,5050505h
004FB69C: add eax,5050505h
004FB6A1: add eax,5050505h
004FB6A6: add eax,5050505h
004FB6AB: add eax,5050505h
004FB6B0: add eax,5050505h
004FB6B5: add eax,5050505h
004FB6BA: add al,90h
004FB6C0: sub esp,0Ch
004FB6C3: push ebx
004FB6C4: push ebp
004FB6C5: push esi
004FB6C6: push edi
004FB6C7: mov edi,dword ptr [esp+20h]
004FB6CB: xor ebx,ebx
004FB6CD: cmp edi,ebx
004FB6CF: mov esi,ecx
004FB6D1: je 004FB8A8
004FB6D7: mov eax,dword ptr [edi+0Ch]
004FB6DA: cmp eax,5Dh
004FB6DD: je 004FB87C
004FB6E3: cmp eax,63h
004FB6E6: mov bp,word ptr [esp+24h]
004FB6EB: jne 004FB73F
004FB6ED: cmp bp,0FFFFh
004FB6F1: je 004FB8A8
004FB6F7: mov eax,dword ptr [esi+4]
004FB6FA: mov ecx,dword ptr [eax+14h]
004FB6FD: cmp ecx,ebx
004FB6FF: je 004FB706
004FB701: lea eax,[ecx+30h]
004FB704: jmp 004FB709
004FB706: add eax,4
004FB709: mov edx,dword ptr [eax]
004FB70B: sub esp,0Ch
004FB70E: mov ecx,esp
004FB710: mov dword ptr [ecx],edx
004FB712: mov edx,dword ptr [eax+4]
004FB715: mov eax,dword ptr [eax+8]
004FB718: mov dword ptr [ecx+4],edx
004FB71B: mov dword ptr [ecx+8],eax
004FB71E: mov ecx,edi
004FB720: call 004EF880
004FB725: fld dword ptr ds:[00C8132Ch]
004FB72B: fcomp dword ptr ds:[00858B8Ch]
004FB731: fnstsw ax
004FB733: test ah,1
004FB736: jne 004FB73F
004FB738: mov ecx,edi
004FB73A: call 004EF1C0
004FB73F: cmp bp,0FFFFh
004FB743: jne 004FB7F1
004FB749: cmp edi,dword ptr [esi+00000158h]
004FB74F: jne 004FB75C
004FB751: mov dword ptr [esi+00000158h],ebx
004FB757: jmp 004FB847
004FB75C: cmp edi,dword ptr [esi+00000160h]
004FB762: jne 004FB76F
004FB764: mov dword ptr [esi+00000160h],ebx
004FB76A: jmp 004FB847
004FB76F: cmp edi,dword ptr [esi+00000168h]
004FB775: jne 004FB782
004FB777: mov dword ptr [esi+00000168h],ebx
004FB77D: jmp 004FB847
004FB782: cmp edi,dword ptr [esi+00000170h]
004FB788: jne 004FB795
004FB78A: mov dword ptr [esi+00000170h],ebx
004FB790: jmp 004FB847
004FB795: cmp edi,dword ptr [esi+00000178h]
004FB79B: jne 004FB7A8
004FB79D: mov dword ptr [esi+00000178h],ebx
004FB7A3: jmp 004FB847
004FB7A8: cmp edi,dword ptr [esi+0000017Ch]
004FB7AE: jne 004FB7BB
004FB7B0: mov dword ptr [esi+0000017Ch],ebx
004FB7B6: jmp 004FB847
004FB7BB: cmp edi,dword ptr [esi+00000180h]
004FB7C1: jne 004FB7CB
004FB7C3: mov dword ptr [esi+00000180h],ebx
004FB7C9: jmp 004FB847
004FB7CB: xor eax,eax
004FB7CD: lea ecx,[ecx]
004FB7D0: movsx ecx,ax
004FB7D3: cmp edi,dword ptr [esi+ecx*8+000000E8h]
 00
004FB7DA: je 004FB7E5
004FB7DC: inc eax
004FB7DD: cmp ax,0Ch
004FB7E1: jl 004FB7D0
004FB7E3: jmp 004FB847
004FB7E5: movsx edx,ax
004FB7E8: mov dword ptr [esi+edx*8+000000E8h],ebx
 00
004FB7EF: jmp 004FB847
004FB7F1: mov al,byte ptr [esi+00000080h]
004FB7F7: cmp al,5
004FB7F9: je 004FB803
004FB7FB: cmp al,4
004FB7FD: je 004FB803
004FB7FF: cmp al,6
004FB801: jne 004FB819
004FB803: cmp edi,dword ptr [esi+00000100h]
004FB809: jne 004FB819
004FB80B: lea eax,[esp+10h]
004FB80F: push eax
004FB810: mov ecx,esi
004FB812: call 004F96A0
004FB817: jmp 004FB82B
004FB819: mov eax,dword ptr [esi+4]
004FB81C: mov ecx,dword ptr [eax+14h]
004FB81F: cmp ecx,ebx
004FB821: je 004FB828
004FB823: lea eax,[ecx+30h]
004FB826: jmp 004FB82B
004FB828: add eax,4
004FB82B: mov edx,dword ptr [eax]
004FB82D: sub esp,0Ch
004FB830: mov ecx,esp
004FB832: mov dword ptr [ecx],edx
004FB834: mov edx,dword ptr [eax+4]
004FB837: mov eax,dword ptr [eax+8]
004FB83A: mov dword ptr [ecx+4],edx
004FB83D: mov dword ptr [ecx+8],eax
004FB840: mov ecx,edi
004FB842: call 004EF880
004FB847: mov al,byte ptr [esi+00000080h]
004FB84D: cmp al,bl
004FB84F: je 004FB855
004FB851: cmp al,1
004FB853: jne 004FB8A8
004FB855: cmp edi,dword ptr [esi+00000108h]
004FB85B: jne 004FB8A8
004FB85D: mov cx,word ptr [esi+00000148h]
 00
004FB864: mov word ptr [esi+0000014Ah],cx
 00
004FB86B: mov word ptr [esi+00000148h],bp
 00
004FB872: pop edi
004FB873: pop esi
004FB874: pop ebp
004FB875: pop ebx
004FB876: add esp,0Ch
004FB879: ret 8
004FB87C: mov eax,dword ptr [edi+8]
004FB87F: cmp eax,ebx
004FB881: je 004FB8A8
004FB883: mov edx,dword ptr ds:[00BD00F8h]
004FB889: movsx ecx,byte ptr [edx+5Dh]
004FB88D: mov dword ptr [esp+24h],ecx
004FB891: push eax
004FB892: mov ecx,esi
004FB894: fild dword ptr [esp+28h]
004FB898: fstp dword ptr [esp+28h]
004FB89C: call 004F6150
004FB8A1: fadd dword ptr [esp+24h]
004FB8A5: fstp dword ptr [edi+14h]
004FB8A8: pop edi
004FB8A9: pop esi
004FB8AA: pop ebp
004FB8AB: pop ebx
004FB8AC: add esp,0Ch
004FB8AF: ret 8
004FB8C0: push ebx
004FB8C1: push esi
004FB8C2: mov esi,ecx
004FB8C4: mov al,byte ptr [esi+000000A4h]
004FB8CA: xor ebx,ebx
004FB8CC: cmp al,bl
004FB8CE: je 004FBB04
004FB8D4: push ebp
004FB8D5: push edi
004FB8D6: lea edi,[esi+000000E8h]
004FB8DC: mov ebp,0Ch
004FB8E1: mov ecx,dword ptr [edi]
004FB8E3: cmp ecx,ebx
004FB8E5: je 004FB8F8
004FB8E7: push ebx
004FB8E8: push 4
004FB8EA: call 004EF2B0
004FB8EF: mov ecx,dword ptr [edi]
004FB8F1: call 004EF1C0
004FB8F6: mov dword ptr [edi],ebx
004FB8F8: add edi,8
004FB8FB: dec ebp
004FB8FC: jne 004FB8E1
004FB8FE: push 0C2C80000h
004FB903: push 3F800000h
004FB908: or edi,0FFFFFFFFh
004FB90B: push edi
004FB90C: mov ecx,esi
004FB90E: call 004F8360
004FB913: push 0C2C80000h
004FB918: push 3F800000h
004FB91D: push edi
004FB91E: mov ecx,esi
004FB920: call 004FA630
004FB925: mov ecx,dword ptr [esi+00000158h]
004FB92B: cmp ecx,ebx
004FB92D: je 004FB94F
004FB92F: push ebx
004FB930: push 4
004FB932: call 004EF2B0
004FB937: mov ecx,dword ptr [esi+00000158h]
004FB93D: call 004EF1C0
004FB942: mov dword ptr [esi+00000158h],ebx
004FB948: mov word ptr [esi+00000156h],di
 00
004FB94F: mov ecx,dword ptr [esi+00000160h]
004FB955: cmp ecx,ebx
004FB957: je 004FB979
004FB959: push ebx
004FB95A: push 4
004FB95C: call 004EF2B0
004FB961: mov ecx,dword ptr [esi+00000160h]
004FB967: call 004EF1C0
004FB96C: mov dword ptr [esi+00000160h],ebx
004FB972: mov word ptr [esi+0000015Ch],di
 00
004FB979: mov ecx,dword ptr [esi+00000168h]
004FB97F: cmp ecx,ebx
004FB981: je 004FB9A3
004FB983: push ebx
004FB984: push 4
004FB986: call 004EF2B0
004FB98B: mov ecx,dword ptr [esi+00000168h]
004FB991: call 004EF1C0
004FB996: mov dword ptr [esi+00000168h],ebx
004FB99C: mov word ptr [esi+00000164h],di
 00
004FB9A3: mov ecx,dword ptr [esi+00000170h]
004FB9A9: cmp ecx,ebx
004FB9AB: je 004FB9CD
004FB9AD: push ebx
004FB9AE: push 4
004FB9B0: call 004EF2B0
004FB9B5: mov ecx,dword ptr [esi+00000170h]
004FB9BB: call 004EF1C0
004FB9C0: mov dword ptr [esi+00000170h],ebx
004FB9C6: mov word ptr [esi+0000016Ch],di
 00
004FB9CD: mov ecx,dword ptr [esi+00000178h]
004FB9D3: cmp ecx,ebx
004FB9D5: je 004FB9E2
004FB9D7: call 004EF850
004FB9DC: mov dword ptr [esi+00000178h],ebx
004FB9E2: mov ecx,dword ptr [esi+0000017Ch]
004FB9E8: cmp ecx,ebx
004FB9EA: je 004FB9F7
004FB9EC: call 004EF850
004FB9F1: mov dword ptr [esi+0000017Ch],ebx
004FB9F7: mov ecx,dword ptr [esi+00000180h]
004FB9FD: cmp ecx,ebx
004FB9FF: je 004FBA0C
004FBA01: call 004EF850
004FBA06: mov dword ptr [esi+00000180h],ebx
004FBA0C: cmp byte ptr [esi+000000A5h],bl
004FBA12: je 004FBA43
004FBA14: movsx eax,byte ptr [esi+0000009Bh]
 00
004FBA1B: sub eax,ebx
004FBA1D: je 004FBA31
004FBA1F: sub eax,2
004FBA22: je 004FBA31
004FBA24: dec eax
004FBA25: jne 004FBA43
004FBA27: push ebx
004FBA28: lea eax,[esi+00000080h]
004FBA2E: push eax
004FBA2F: jmp 004FBA39
004FBA31: push ebx
004FBA32: lea ecx,[esi+00000080h]
004FBA38: push ecx
004FBA39: mov ecx,0B6BC90h
004FBA3E: call 00506F70
004FBA43: xor eax,eax
004FBA45: mov ax,word ptr [esi+000000E0h]
 00
004FBA4C: cmp ax,di
004FBA4F: je 004FBAAF
004FBA51: add eax,0FFFFFFF9h
004FBA54: cmp ax,bx
004FBA57: jl 004FBAA8
004FBA59: cmp ax,0Ah
004FBA5D: jge 004FBAA8
004FBA5F: movsx edx,ax
004FBA62: mov cx,word ptr [edx*4+00B6B9A0h]
004FBA6A: cmp cx,word ptr [esi+000000DCh]
 00
004FBA71: sete cl
004FBA74: cmp cl,bl
004FBA76: je 004FBAA8
004FBA78: cmp ax,bx
004FBA7B: jl 004FBAA8
004FBA7D: cmp ax,0Ah
004FBA81: jge 004FBAA8
004FBA83: movsx eax,ax
004FBA86: xor ecx,ecx
004FBA88: mov cx,word ptr [eax*4+00B6B9A2h]
004FBA90: cmp cx,bx
004FBA93: jle 004FBAA0
004FBA95: dec ecx
004FBA96: mov word ptr [eax*4+00B6B9A2h],cx
004FBA9E: jmp 004FBAA8
004FBAA0: mov word ptr [eax*4+00B6B9A2h],bx
004FBAA8: mov word ptr [esi+000000E0h],di
 00
004FBAAF: cmp byte ptr [esi+000000A5h],bl
004FBAB5: mov word ptr [esi+000000DCh],di
 00
004FBABC: mov word ptr [esi+000000DEh],di
 00
004FBAC3: pop edi
004FBAC4: mov dword ptr [esi+4],ebx
004FBAC7: mov byte ptr [esi+000000A9h],bl
004FBACD: pop ebp
004FBACE: je 004FBADE
004FBAD0: mov byte ptr [esi+000000A5h],bl
004FBAD6: mov dword ptr ds:[00B6B990h],ebx
004FBADC: jmp 004FBAEC
004FBADE: cmp byte ptr [esi+000000A6h],bl
004FBAE4: je 004FBAF8
004FBAE6: mov byte ptr [esi+000000A6h],bl
004FBAEC: mov dword ptr ds:[00B6B98Ch],ebx
004FBAF2: mov dword ptr ds:[00B6B99Ch],ebx
004FBAF8: mov byte ptr [esi+000000A7h],bl
004FBAFE: mov byte ptr [esi+000000A4h],bl
004FBB04: pop esi
004FBB05: pop ebx
004FBB06: ret
004FBB10: push ecx
004FBB11: push esi
004FBB12: mov esi,ecx
004FBB14: xor ecx,ecx
004FBB16: mov cx,word ptr [esi+000000E0h]
 00
004FBB1D: xor edx,edx
004FBB1F: mov dx,word ptr [esi+000000DCh]
 00
004FBB26: push edi
004FBB27: mov edi,dword ptr [esp+10h]
004FBB2B: mov eax,dword ptr [edi+10h]
004FBB2E: mov dword ptr [esp+10h],eax
004FBB32: push ecx
004FBB33: mov ecx,0B5F8B8h
004FBB38: push edx
004FBB39: call 004D88C0
004FBB3E: test al,al
004FBB40: je 004FC9E7
004FBB46: push edi
004FBB47: mov ecx,esi
004FBB49: call 004F5080
004FBB4E: fld dword ptr [edi+1Ch]
004FBB51: mov edx,dword ptr [edi+14h]
004FBB54: fabs
004FBB56: fst dword ptr [edi+24h]
004FBB59: fld st(0)
004FBB5B: fdiv dword ptr [edx+58h]
004FBB5E: fld dword ptr ds:[00858624h]
004FBB64: fcomp st(1)
004FBB66: fnstsw ax
004FBB68: test ah,5
004FBB6B: jnp 004FBB8A
004FBB6D: fld st(0)
004FBB6F: fld dword ptr ds:[00858B50h]
004FBB75: fcomp st(1)
004FBB77: fnstsw ax
004FBB79: fstp st(0)
004FBB7B: test ah,41h
004FBB7E: jne 004FBB8A
004FBB80: fstp st(0)
004FBB82: fld dword ptr ds:[00858B50h]
004FBB88: jmp 004FBBA1
004FBB8A: fld dword ptr ds:[00858624h]
004FBB90: fcomp st(1)
004FBB92: fnstsw ax
004FBB94: test ah,5
004FBB97: jp 004FBBA1
004FBB99: fstp st(0)
004FBB9B: fld dword ptr ds:[00858624h]
004FBBA1: mov ecx,dword ptr [edi+10h]
004FBBA4: fstp dword ptr [edi+2Ch]
004FBBA7: mov al,byte ptr [ecx+000004B4h]
004FBBAD: mov byte ptr [edi+38h],al
004FBBB0: mov al,byte ptr [ecx+00000428h]
004FBBB6: shr al,5
004FBBB9: push ebx
004FBBBA: mov bl,byte ptr [edi+38h]
004FBBBD: and al,1
004FBBBF: mov byte ptr [edi+39h],al
004FBBC2: push ebp
004FBBC3: movzx ebp,bl
004FBBC6: lea eax,[ebp+ebp*2]
004FBBCA: lea eax,[edx+eax*4]
004FBBCD: fsub dword ptr [eax+8]
004FBBD0: fld dword ptr [eax]
004FBBD2: fsub dword ptr [eax+8]
004FBBD5: fdivp st(1),st
004FBBD7: fld st(0)
004FBBD9: fcomp qword ptr ds:[0085A310h]
004FBBDF: fnstsw ax
004FBBE1: test ah,41h
004FBBE4: je 004FBC03
004FBBE6: fld st(0)
004FBBE8: fld qword ptr ds:[00859EF8h]
004FBBEE: fcomp st(1)
004FBBF0: fnstsw ax
004FBBF2: fstp st(0)
004FBBF4: test ah,41h
004FBBF7: jne 004FBC03
004FBBF9: fstp st(0)
004FBBFB: fld qword ptr ds:[00859EF8h]
004FBC01: jmp 004FBC1A
004FBC03: fld st(0)
004FBC05: fcomp qword ptr ds:[0085A310h]
004FBC0B: fnstsw ax
004FBC0D: test ah,41h
004FBC10: jne 004FBC1A
004FBC12: fstp st(0)
004FBC14: fld qword ptr ds:[0085A310h]
004FBC1A: test bl,bl
004FBC1C: fstp dword ptr [edi+3Ch]
004FBC1F: jne 004FBC28
004FBC21: mov dword ptr [edi+3Ch],0
 00
004FBC28: fld dword ptr [edi+1Ch]
004FBC2B: mov eax,dword ptr [esp+18h]
004FBC2F: mov eax,dword ptr [eax+000004BCh]
004FBC35: fcomp dword ptr ds:[00862CF0h]
004FBC3B: mov dword ptr [edi+40h],eax
004FBC3E: movzx dx,byte ptr [edx+4Ah]
004FBC43: mov word ptr [edi+44h],dx
004FBC47: fnstsw ax
004FBC49: test ah,5
004FBC4C: jp 004FBC56
004FBC4E: mov dl,1
004FBC50: mov byte ptr [esp+18h],dl
004FBC54: jmp 004FBC5F
004FBC56: mov byte ptr [esp+18h],0
004FBC5B: mov dl,byte ptr [esp+18h]
004FBC5F: fld dword ptr [edi+1Ch]
004FBC62: fcomp dword ptr ds:[00858B50h]
004FBC68: fnstsw ax
004FBC6A: test ah,44h
004FBC6D: jnp 004FBC7A
004FBC6F: fld dword ptr [ecx+4Ch]
004FBC72: fdiv dword ptr [edi+1Ch]
004FBC75: fstp dword ptr [edi+28h]
004FBC78: jmp 004FBC81
004FBC7A: mov dword ptr [edi+28h],0
 00
004FBC81: mov eax,dword ptr [edi+4]
004FBC84: test eax,eax
004FBC86: je 004FBCBD
004FBC88: cmp eax,9
004FBC8B: jne 004FC9E5
004FBC91: mov al,byte ptr [ecx+00000805h]
004FBC97: mov byte ptr [edi+46h],al
004FBC9A: mov al,byte ptr [ecx+00000806h]
004FBCA0: mov byte ptr [edi+47h],al
004FBCA3: lea eax,[ecx+00000808h]
004FBCA9: mov dword ptr [edi+30h],eax
004FBCAC: lea eax,[ecx+0000080Ch]
004FBCB2: mov dword ptr [edi+48h],eax
004FBCB5: mov eax,dword ptr [ecx+000007B8h]
004FBCBB: jmp 004FBCE7
004FBCBD: mov al,byte ptr [ecx+00000961h]
004FBCC3: mov byte ptr [edi+46h],al
004FBCC6: mov al,byte ptr [ecx+00000962h]
004FBCCC: mov byte ptr [edi+47h],al
004FBCCF: lea eax,[ecx+00000964h]
004FBCD5: mov dword ptr [edi+30h],eax
004FBCD8: lea eax,[ecx+00000968h]
004FBCDE: mov dword ptr [edi+48h],eax
004FBCE1: mov eax,dword ptr [ecx+000008B0h]
004FBCE7: mov dword ptr [edi+34h],eax
004FBCEA: mov al,byte ptr [esi+000000B0h]
004FBCF0: test al,al
004FBCF2: je 004FBD09
004FBCF4: cmp bl,1
004FBCF7: mov eax,ebp
004FBCF9: jb 004FBD00
004FBCFB: mov eax,1
004FBD00: mov byte ptr [edi+38h],al
004FBD03: mov word ptr [edi+44h],1
004FBD09: movsx eax,byte ptr [esi+000000A9h]
 00
004FBD10: mov ebx,6
004FBD15: cmp eax,ebx
004FBD17: ja 004FC9E5
004FBD1D: jmp dword ptr [eax*4+004FC9F0h]
 00
004FBD24: test byte ptr [ecx+00000428h],10h
 10
004FBD2B: jne 004FBD54
004FBD2D: xor edi,edi
004FBD30: cmp di,9
004FBD34: je 004FBD3E
004FBD36: push edi
004FBD37: mov ecx,esi
004FBD39: call 004F55C0
004FBD3E: inc edi
004FBD3F: cmp di,0Ch
004FBD43: jl 004FBD30
004FBD45: pop ebp
004FBD46: pop ebx
004FBD47: pop edi
004FBD48: mov byte ptr [esi+000000A9h],0
 00
004FBD4F: pop esi
004FBD50: pop ecx
004FBD51: ret 4
004FBD54: xor ebp,ebp
004FBD56: push ebp
004FBD57: push 67h
004FBD59: mov ecx,esi
004FBD5B: call 004F6420
004FBD60: mov al,byte ptr [esp+18h]
004FBD64: test al,al
004FBD66: je 004FBDD0
004FBD68: mov cx,word ptr [edi+20h]
004FBD6C: cmp cx,word ptr ds:[008CBC54h]
 00
004FBD73: jle 004FC9BC
004FBD79: mov ecx,dword ptr [esi+00000110h]
004FBD7F: cmp ecx,ebp
004FBD81: je 004FBD9C
004FBD83: push ebp
004FBD84: push 4
004FBD86: call 004EF2B0
004FBD8B: mov ecx,dword ptr [esi+00000110h]
004FBD91: call 004EF1C0
004FBD96: mov dword ptr [esi+00000110h],ebp
004FBD9C: mov ecx,esi
004FBD9E: push 1
004FBDA0: push edi
004FBDA1: call 004F5D00
004FBDA6: push ecx
004FBDA7: fstp dword ptr [esp]
004FBDAA: push 1
004FBDAC: push edi
004FBDAD: mov ecx,esi
004FBDAF: call 004F8070
004FBDB4: push ecx
004FBDB5: fstp dword ptr [esp]
004FBDB8: push 1
004FBDBA: mov ecx,esi
004FBDBC: call 004F7A50
004FBDC1: pop ebp
004FBDC2: pop ebx
004FBDC3: pop edi
004FBDC4: mov byte ptr [esi+000000A9h],4
 04
004FBDCB: pop esi
004FBDCC: pop ecx
004FBDCD: ret 4
004FBDD0: cmp word ptr [edi+20h],bp
004FBDD4: mov ecx,esi
004FBDD6: jle 004FBEB9
004FBDDC: push edi
004FBDDD: call 004F4F70
004FBDE2: test al,al
004FBDE4: jne 004FBD68
004FBDE6: push edi
004FBDE7: mov ecx,esi
004FBDE9: call 004F4FF0
004FBDEE: test al,al
004FBDF0: jne 004FBD68
004FBDF6: push edi
004FBDF7: call 004F4FF0
004FBDFC: test al,al
004FBDFE: jne 004FC9BC
004FBE04: mov dx,word ptr [edi+20h]
004FBE08: cmp dx,word ptr ds:[008CBC54h]
 00
004FBE0F: jle 004FBEAA
004FBE15: movzx eax,byte ptr [edi+38h]
004FBE19: cmp ax,word ptr [edi+44h]
004FBE1D: jge 004FBE7E
004FBE1F: fld dword ptr ds:[008CBD34h]
004FBE25: fadd dword ptr [edi+34h]
004FBE28: fcomp dword ptr [edi+1Ch]
004FBE2B: fnstsw ax
004FBE2D: test ah,41h
004FBE30: jne 004FBE7E
004FBE32: fld dword ptr [edi+28h]
004FBE35: fcomp dword ptr ds:[008CBD38h]
004FBE3B: fnstsw ax
004FBE3D: test ah,41h
004FBE40: jne 004FBE7E
004FBE42: mov eax,dword ptr [edi+10h]
004FBE45: or byte ptr [eax+0000042Bh],20h
 20
004FBE4C: push 3
004FBE4E: push edi
004FBE4F: mov ecx,esi
004FBE51: call 004F5D00
004FBE56: push ecx
004FBE57: fstp dword ptr [esp]
004FBE5A: push 3
004FBE5C: push edi
004FBE5D: call 004F8070
004FBE62: push ecx
004FBE63: fstp dword ptr [esp]
004FBE66: push 3
004FBE68: mov ecx,esi
004FBE6A: call 004F7A50
004FBE6F: pop ebp
004FBE70: pop ebx
004FBE71: pop edi
004FBE72: mov byte ptr [esi+000000A9h],5
 05
004FBE79: pop esi
004FBE7A: pop ecx
004FBE7B: ret 4
004FBE7E: mov ecx,dword ptr [esi+00000110h]
004FBE84: cmp ecx,ebp
004FBE86: je 004FC8FE
004FBE8C: push ebp
004FBE8D: push 4
004FBE8F: call 004EF2B0
004FBE94: mov ecx,dword ptr [esi+00000110h]
004FBE9A: call 004EF1C0
004FBE9F: mov dword ptr [esi+00000110h],ebp
004FBEA5: jmp 004FC8FE
004FBEAA: push 0C2C80000h
004FBEAF: push 3F800000h
004FBEB4: jmp 004FC9D6
004FBEB9: push 0C2C80000h
004FBEBE: push 3F800000h
004FBEC3: jmp 004FC9D8
004FBEC8: test byte ptr [ecx+00000428h],10h
 10
004FBECF: jne 004FBEF1
004FBED1: xor edi,edi
004FBED3: push edi
004FBED4: mov ecx,esi
004FBED6: call 004F55C0
004FBEDB: inc edi
004FBEDC: cmp di,0Ch
004FBEE0: jl 004FBED3
004FBEE2: pop ebp
004FBEE3: pop ebx
004FBEE4: pop edi
004FBEE5: mov byte ptr [esi+000000A9h],0
 00
004FBEEC: pop esi
004FBEED: pop ecx
004FBEEE: ret 4
004FBEF1: test dl,dl
004FBEF3: je 004FBF37
004FBEF5: mov ecx,dword ptr [esi+00000100h]
004FBEFB: test ecx,ecx
004FBEFD: je 004FBF1D
004FBEFF: push 0
004FBF01: push 4
004FBF03: call 004EF2B0
004FBF08: mov ecx,dword ptr [esi+00000100h]
004FBF0E: call 004EF1C0
004FBF13: mov dword ptr [esi+00000100h],0
004FBF1D: mov dword ptr [esi+000000ACh],0
004FBF27: mov dx,word ptr [edi+20h]
004FBF2B: cmp dx,word ptr ds:[008CBC54h]
 00
004FBF32: jmp 004FC54A
004FBF37: cmp word ptr [edi+20h],0
004FBF3C: jle 004FC1AB
004FBF42: push edi
004FBF43: mov ecx,esi
004FBF45: call 004F4F70
004FBF4A: test al,al
004FBF4C: jne 004FC177
004FBF52: push edi
004FBF53: mov ecx,esi
004FBF55: call 004F4FF0
004FBF5A: test al,al
004FBF5C: jne 004FC177
004FBF62: push edi
004FBF63: call 004F4FF0
004FBF68: test al,al
004FBF6A: jne 004FC143
004FBF70: mov ax,word ptr [edi+20h]
004FBF74: cmp ax,word ptr ds:[008CBC54h]
 00
004FBF7B: jle 004FC143
004FBF81: fld dword ptr ds:[008CBD30h]
004FBF87: fadd dword ptr [edi+34h]
004FBF8A: fcomp dword ptr [edi+1Ch]
004FBF8D: fnstsw ax
004FBF8F: test ah,5
004FBF92: jp 004FC0AB
004FBF98: movzx ecx,byte ptr [edi+38h]
004FBF9C: cmp cx,word ptr [edi+44h]
004FBFA0: jge 004FC0AB
004FBFA6: mov edx,dword ptr [esi+000000D0h]
004FBFAC: cmp edx,dword ptr ds:[00B7CB84h]
004FBFB2: jb 004FC0AB
004FBFB8: call 0072DDD0
004FBFBD: test al,al
004FBFBF: jne 004FC0AB
004FBFC5: xor ebx,ebx
004FBFC7: mov dword ptr [esi+000000ACh],ebx
004FBFCD: mov ecx,dword ptr [esi+00000100h]
004FBFD3: cmp ecx,ebx
004FBFD5: je 004FBFF0
004FBFD7: push ebx
004FBFD8: push 4
004FBFDA: call 004EF2B0
004FBFDF: mov ecx,dword ptr [esi+00000100h]
004FBFE5: call 004EF1C0
004FBFEA: mov dword ptr [esi+00000100h],ebx
004FBFF0: mov al,byte ptr [esi+000000B0h]
004FBFF6: test al,al
004FBFF8: je 004FC003
004FBFFA: mov byte ptr [esi+000000AAh],1
 01
004FC001: jmp 004FC067
004FC003: mov al,byte ptr [edi+38h]
004FC006: cmp al,1
004FC008: movzx ecx,al
004FC00B: ja 004FC012
004FC00D: mov ecx,1
004FC012: movzx ebx,byte ptr [esi+000000AAh]
 00
004FC019: cmp ecx,ebx
004FC01B: jge 004FC02D
004FC01D: cmp al,1
004FC01F: jbe 004FC026
004FC021: movzx edx,al
004FC024: jmp 004FC02F
004FC026: mov edx,1
004FC02B: jmp 004FC02F
004FC02D: mov edx,ebx
004FC02F: mov ecx,dword ptr [edi+14h]
004FC032: movzx ecx,byte ptr [ecx+4Ah]
004FC036: dec ecx
004FC037: cmp edx,ecx
004FC039: jge 004FC05F
004FC03B: cmp al,1
004FC03D: movzx ecx,al
004FC040: ja 004FC047
004FC042: mov ecx,1
004FC047: cmp ecx,ebx
004FC049: jge 004FC05B
004FC04B: cmp al,1
004FC04D: jbe 004FC054
004FC04F: movzx eax,al
004FC052: jmp 004FC061
004FC054: mov eax,1
004FC059: jmp 004FC061
004FC05B: mov eax,ebx
004FC05D: jmp 004FC061
004FC05F: mov eax,ecx
004FC061: mov byte ptr [esi+000000AAh],al
004FC067: push 4
004FC069: push edi
004FC06A: mov ecx,esi
004FC06C: call 004F5D00
004FC071: push ecx
004FC072: fstp dword ptr [esp]
004FC075: push 4
004FC077: push edi
004FC078: call 004F8070
004FC07D: push ecx
004FC07E: fstp dword ptr [esp]
004FC081: push 4
004FC083: mov ecx,esi
004FC085: call 004F7A50
004FC08A: mov edx,dword ptr ds:[00B7CB84h]
004FC090: pop ebp
004FC091: pop ebx
004FC092: add edx,7D0h
004FC098: pop edi
004FC099: mov dword ptr [esi+000000D4h],edx
004FC09F: mov byte ptr [esi+000000A9h],3
 03
004FC0A6: pop esi
004FC0A7: pop ecx
004FC0A8: ret 4
004FC0AB: fld dword ptr [edi+34h]
004FC0AE: fsub dword ptr ds:[008CBC90h]
004FC0B4: fcomp dword ptr [edi+1Ch]
004FC0B7: fld dword ptr [esi+000000ACh]
004FC0BD: fnstsw ax
004FC0BF: test ah,5
004FC0C2: jp 004FC0F0
004FC0C4: movsx eax,word ptr ds:[008CBC8Ch]
 00
004FC0CB: fadd dword ptr ds:[00858624h]
004FC0D1: mov dword ptr [esp+18h],eax
004FC0D5: fild dword ptr [esp+18h]
004FC0D9: fstp dword ptr [esp+18h]
004FC0DD: fcom dword ptr [esp+18h]
004FC0E1: fnstsw ax
004FC0E3: test ah,5
004FC0E6: jnp 004FC10B
004FC0E8: fstp st(0)
004FC0EA: fld dword ptr [esp+18h]
004FC0EE: jmp 004FC10B
004FC0F0: fsub dword ptr ds:[00858C84h]
004FC0F6: fcom dword ptr ds:[00858B50h]
004FC0FC: fnstsw ax
004FC0FE: test ah,41h
004FC101: je 004FC10B
004FC103: fstp st(0)
004FC105: fld dword ptr ds:[00858B50h]
004FC10B: push 3
004FC10D: fstp dword ptr [esi+000000ACh]
004FC113: push edi
004FC114: mov ecx,esi
004FC116: call 004F5D00
004FC11B: push ecx
004FC11C: fstp dword ptr [esp]
004FC11F: push 3
004FC121: push edi
004FC122: call 004F8070
004FC127: push ecx
004FC128: fstp dword ptr [esp]
004FC12B: push 3
004FC12D: mov ecx,esi
004FC12F: call 004F56D0
004FC134: pop ebp
004FC135: pop ebx
004FC136: pop edi
004FC137: mov byte ptr [esi+000000A9h],5
 05
004FC13E: pop esi
004FC13F: pop ecx
004FC140: ret 4
004FC143: xor ebp,ebp
004FC145: mov dword ptr [esi+000000ACh],ebp
004FC14B: mov ecx,dword ptr [esi+00000100h]
004FC151: cmp ecx,ebp
004FC153: je 004FC997
004FC159: push ebp
004FC15A: push 4
004FC15C: call 004EF2B0
004FC161: mov ecx,dword ptr [esi+00000100h]
004FC167: call 004EF1C0
004FC16C: mov dword ptr [esi+00000100h],ebp
004FC172: jmp 004FC997
004FC177: xor ebp,ebp
004FC179: mov dword ptr [esi+000000ACh],ebp
004FC17F: mov ecx,dword ptr [esi+00000100h]
004FC185: cmp ecx,ebp
004FC187: je 004FC53F
004FC18D: push ebp
004FC18E: push 4
004FC190: call 004EF2B0
004FC195: mov ecx,dword ptr [esi+00000100h]
004FC19B: call 004EF1C0
004FC1A0: mov dword ptr [esi+00000100h],ebp
004FC1A6: jmp 004FC53F
004FC1AB: mov dword ptr [esi+000000ACh],0
004FC1B5: mov ecx,dword ptr [esi+00000100h]
004FC1BB: test ecx,ecx
004FC1BD: je 004FC997
004FC1C3: push 0
004FC1C5: push 4
004FC1C7: call 004EF2B0
004FC1CC: mov ecx,dword ptr [esi+00000100h]
004FC1D2: call 004EF1C0
004FC1D7: mov dword ptr [esi+00000100h],0
004FC1E1: jmp 004FC997
004FC1E6: test byte ptr [ecx+00000428h],10h
 10
004FC1ED: jne 004FC20F
004FC1EF: xor edi,edi
004FC1F1: push edi
004FC1F2: mov ecx,esi
004FC1F4: call 004F55C0
004FC1F9: inc edi
004FC1FA: cmp di,0Ch
004FC1FE: jl 004FC1F1
004FC200: pop ebp
004FC201: pop ebx
004FC202: pop edi
004FC203: mov byte ptr [esi+000000A9h],0
 00
004FC20A: pop esi
004FC20B: pop ecx
004FC20C: ret 4
004FC20F: test dl,dl
004FC211: je 004FC27D
004FC213: mov dx,word ptr [edi+20h]
004FC217: cmp dx,word ptr ds:[008CBC54h]
 00
004FC21E: jle 004FC471
004FC224: mov ecx,dword ptr [esi+000000F8h]
004FC22A: test ecx,ecx
004FC22C: je 004FC24C
004FC22E: push 0
004FC230: push 4
004FC232: call 004EF2B0
004FC237: mov ecx,dword ptr [esi+000000F8h]
004FC23D: call 004EF1C0
004FC242: mov dword ptr [esi+000000F8h],0
004FC24C: mov ecx,dword ptr [esi+00000110h]
004FC252: test ecx,ecx
004FC254: je 004FBD9C
004FC25A: push 0
004FC25C: push 4
004FC25E: call 004EF2B0
004FC263: mov ecx,dword ptr [esi+00000110h]
004FC269: call 004EF1C0
004FC26E: mov dword ptr [esi+00000110h],0
004FC278: jmp 004FBD9C
004FC27D: cmp word ptr [edi+20h],0
004FC282: mov ecx,esi
004FC284: jle 004FC4A2
004FC28A: push edi
004FC28B: call 004F4F70
004FC290: test al,al
004FC292: jne 004FC413
004FC298: push edi
004FC299: mov ecx,esi
004FC29B: call 004F4FF0
004FC2A0: test al,al
004FC2A2: jne 004FC413
004FC2A8: push edi
004FC2A9: call 004F4FF0
004FC2AE: test al,al
004FC2B0: jne 004FC471
004FC2B6: mov ax,word ptr [edi+20h]
004FC2BA: cmp ax,word ptr ds:[008CBC54h]
 00
004FC2C1: jle 004FC471
004FC2C7: movzx ecx,byte ptr [edi+38h]
004FC2CB: cmp cx,word ptr [edi+44h]
004FC2CF: jge 004FC34F
004FC2D1: fld dword ptr ds:[008CBD34h]
004FC2D7: fadd dword ptr [edi+34h]
004FC2DA: fcomp dword ptr [edi+1Ch]
004FC2DD: fnstsw ax
004FC2DF: test ah,41h
004FC2E2: jne 004FC34F
004FC2E4: fld dword ptr [edi+28h]
004FC2E7: fcomp dword ptr ds:[008CBD38h]
004FC2ED: fnstsw ax
004FC2EF: test ah,41h
004FC2F2: jne 004FC34F
004FC2F4: mov eax,dword ptr [edi+10h]
004FC2F7: or byte ptr [eax+0000042Bh],20h
 20
004FC2FE: mov ecx,dword ptr [esi+00000110h]
004FC304: xor ebx,ebx
004FC306: cmp ecx,ebx
004FC308: je 004FC323
004FC30A: push ebx
004FC30B: push 4
004FC30D: call 004EF2B0
004FC312: mov ecx,dword ptr [esi+00000110h]
004FC318: call 004EF1C0
004FC31D: mov dword ptr [esi+00000110h],ebx
004FC323: mov ecx,dword ptr [esi+000000F8h]
004FC329: cmp ecx,ebx
004FC32B: je 004FBE4C
004FC331: push ebx
004FC332: push 4
004FC334: call 004EF2B0
004FC339: mov ecx,dword ptr [esi+000000F8h]
004FC33F: call 004EF1C0
004FC344: mov dword ptr [esi+000000F8h],ebx
004FC34A: jmp 004FBE4C
004FC34F: mov ecx,dword ptr [esi+00000110h]
004FC355: xor ebx,ebx
004FC357: cmp ecx,ebx
004FC359: je 004FC374
004FC35B: push ebx
004FC35C: push 4
004FC35E: call 004EF2B0
004FC363: mov ecx,dword ptr [esi+00000110h]
004FC369: call 004EF1C0
004FC36E: mov dword ptr [esi+00000110h],ebx
004FC374: mov ecx,dword ptr [esi+000000F8h]
004FC37A: cmp ecx,ebx
004FC37C: je 004FC397
004FC37E: push ebx
004FC37F: push 4
004FC381: call 004EF2B0
004FC386: mov ecx,dword ptr [esi+000000F8h]
004FC38C: call 004EF1C0
004FC391: mov dword ptr [esi+000000F8h],ebx
004FC397: mov al,byte ptr [esi+000000B0h]
004FC39D: test al,al
004FC39F: je 004FC3AD
004FC3A1: mov byte ptr [esi+000000AAh],1
 01
004FC3A8: jmp 004FC8FE
004FC3AD: mov al,byte ptr [edi+38h]
004FC3B0: cmp al,1
004FC3B2: movzx ecx,al
004FC3B5: ja 004FC3BC
004FC3B7: mov ecx,1
004FC3BC: movzx ebx,byte ptr [esi+000000AAh]
 00
004FC3C3: cmp ecx,ebx
004FC3C5: jge 004FC3D4
004FC3C7: cmp al,1
004FC3C9: jbe 004FC8EC
004FC3CF: movzx edx,al
004FC3D2: jmp 004FC3D6
004FC3D4: mov edx,ebx
004FC3D6: mov ecx,dword ptr [edi+14h]
004FC3D9: movzx ecx,byte ptr [ecx+4Ah]
004FC3DD: dec ecx
004FC3DE: cmp edx,ecx
004FC3E0: jge 004FC8F6
004FC3E6: cmp al,1
004FC3E8: movzx ecx,al
004FC3EB: ja 004FC3F2
004FC3ED: mov ecx,1
004FC3F2: cmp ecx,ebx
004FC3F4: jge 004FC40C
004FC3F6: cmp al,1
004FC3F8: jbe 004FC402
004FC3FA: movzx eax,al
004FC3FD: jmp 004FC8F8
004FC402: mov eax,1
004FC407: jmp 004FC8F8
004FC40C: mov eax,ebx
004FC40E: jmp 004FC8F8
004FC413: mov dx,word ptr [edi+20h]
004FC417: cmp dx,word ptr ds:[008CBC54h]
 00
004FC41E: jle 004FC471
004FC420: mov ecx,dword ptr [esi+000000F8h]
004FC426: xor ebx,ebx
004FC428: cmp ecx,ebx
004FC42A: je 004FC445
004FC42C: push ebx
004FC42D: push 4
004FC42F: call 004EF2B0
004FC434: mov ecx,dword ptr [esi+000000F8h]
004FC43A: call 004EF1C0
004FC43F: mov dword ptr [esi+000000F8h],ebx
004FC445: mov ecx,dword ptr [esi+00000110h]
004FC44B: cmp ecx,ebx
004FC44D: je 004FBD9C
004FC453: push ebx
004FC454: push 4
004FC456: call 004EF2B0
004FC45B: mov ecx,dword ptr [esi+00000110h]
004FC461: call 004EF1C0
004FC466: mov dword ptr [esi+00000110h],ebx
004FC46C: jmp 004FBD9C
004FC471: push 2
004FC473: push edi
004FC474: mov ecx,esi
004FC476: call 004F5D00
004FC47B: push ecx
004FC47C: fstp dword ptr [esp]
004FC47F: push 2
004FC481: push edi
004FC482: call 004F8070
004FC487: push ecx
004FC488: fstp dword ptr [esp]
004FC48B: push 2
004FC48D: mov ecx,esi
004FC48F: call 004F56D0
004FC494: pop ebp
004FC495: mov byte ptr [esi+000000A9h],bl
004FC49B: pop ebx
004FC49C: pop edi
004FC49D: pop esi
004FC49E: pop ecx
004FC49F: ret 4
004FC4A2: push 2
004FC4A4: push edi
004FC4A5: call 004F5D00
004FC4AA: push ecx
004FC4AB: fstp dword ptr [esp]
004FC4AE: push 2
004FC4B0: push edi
004FC4B1: mov ecx,esi
004FC4B3: call 004F8070
004FC4B8: push ecx
004FC4B9: fstp dword ptr [esp]
004FC4BC: push 2
004FC4BE: mov ecx,esi
004FC4C0: call 004F56D0
004FC4C5: pop ebp
004FC4C6: mov byte ptr [esi+000000A9h],bl
004FC4CC: pop ebx
004FC4CD: pop edi
004FC4CE: pop esi
004FC4CF: pop ecx
004FC4D0: ret 4
004FC4D3: test byte ptr [ecx+00000428h],10h
 10
004FC4DA: jne 004FC4FE
004FC4DC: xor edi,edi
004FC4DE: mov edi,edi
004FC4E0: push edi
004FC4E1: mov ecx,esi
004FC4E3: call 004F55C0
004FC4E8: inc edi
004FC4E9: cmp di,0Ch
004FC4ED: jl 004FC4E0
004FC4EF: pop ebp
004FC4F0: pop ebx
004FC4F1: pop edi
004FC4F2: mov byte ptr [esi+000000A9h],0
 00
004FC4F9: pop esi
004FC4FA: pop ecx
004FC4FB: ret 4
004FC4FE: test dl,dl
004FC500: mov ecx,esi
004FC502: je 004FC521
004FC504: mov ax,word ptr [edi+20h]
004FC508: cmp ax,word ptr ds:[008CBC54h]
 00
004FC50F: push 4
004FC511: jle 004FC760
004FC517: call 004F55C0
004FC51C: jmp 004FBD9C
004FC521: cmp word ptr [edi+20h],0
004FC526: jle 004FC75E
004FC52C: push edi
004FC52D: call 004F4F70
004FC532: test al,al
004FC534: mov ecx,esi
004FC536: je 004FC557
004FC538: push 4
004FC53A: call 004F55C0
004FC53F: mov cx,word ptr [edi+20h]
004FC543: cmp cx,word ptr ds:[008CBC54h]
 00
004FC54A: mov ecx,esi
004FC54C: jle 004FC999
004FC552: jmp 004FBD9E
004FC557: push edi
004FC558: call 004F4FF0
004FC55D: test al,al
004FC55F: jne 004FC750
004FC565: push edi
004FC566: mov ecx,esi
004FC568: call 004F4FF0
004FC56D: test al,al
004FC56F: jne 004FC750
004FC575: mov dx,word ptr [edi+20h]
004FC579: cmp dx,word ptr ds:[008CBC54h]
 00
004FC580: jle 004FC750
004FC586: movzx eax,byte ptr [edi+38h]
004FC58A: cmp ax,word ptr [edi+44h]
004FC58E: jge 004FC631
004FC594: fld dword ptr ds:[008CBD34h]
004FC59A: fadd dword ptr [edi+34h]
004FC59D: fcomp dword ptr [edi+1Ch]
004FC5A0: fnstsw ax
004FC5A2: test ah,41h
004FC5A5: jne 004FC631
004FC5AB: fld dword ptr [edi+28h]
004FC5AE: fcomp dword ptr ds:[008CBD38h]
004FC5B4: fnstsw ax
004FC5B6: test ah,41h
004FC5B9: je 004FC5C4
004FC5BB: call 0072DDD0
004FC5C0: test al,al
004FC5C2: je 004FC631
004FC5C4: mov ecx,dword ptr [esi+000000D4h]
004FC5CA: cmp ecx,dword ptr ds:[00B7CB84h]
004FC5D0: jb 004FC631
004FC5D2: mov eax,dword ptr [edi+10h]
004FC5D5: mov bl,byte ptr [eax+0000042Bh]
004FC5DB: or bl,20h
004FC5DE: push 4
004FC5E0: mov ecx,esi
004FC5E2: mov byte ptr [eax+0000042Bh],bl
004FC5E8: call 004F55C0
004FC5ED: push 3
004FC5EF: push edi
004FC5F0: mov ecx,esi
004FC5F2: call 004F5D00
004FC5F7: push ecx
004FC5F8: fstp dword ptr [esp]
004FC5FB: push 3
004FC5FD: push edi
004FC5FE: call 004F8070
004FC603: push ecx
004FC604: fstp dword ptr [esp]
004FC607: push 3
004FC609: mov ecx,esi
004FC60B: call 004F7A50
004FC610: mov edx,dword ptr ds:[00B7CB84h]
004FC616: pop ebp
004FC617: pop ebx
004FC618: add edx,7D0h
004FC61E: pop edi
004FC61F: mov dword ptr [esi+000000D0h],edx
004FC625: mov byte ptr [esi+000000A9h],5
 05
004FC62C: pop esi
004FC62D: pop ecx
004FC62E: ret 4
004FC631: mov ecx,esi
004FC633: call 004F5E50
004FC638: test al,al
004FC63A: je 004FC6F9
004FC640: movzx ecx,byte ptr [esi+000000AAh]
 00
004FC647: movsx eax,word ptr [edi+44h]
004FC64B: add ecx,2
004FC64E: cmp ecx,eax
004FC650: jle 004FC69D
004FC652: movzx edx,byte ptr [edi+38h]
004FC656: cmp edx,eax
004FC658: jge 004FC663
004FC65A: call 0072DDD0
004FC65F: test al,al
004FC661: je 004FC69D
004FC663: movzx eax,byte ptr [esi+000000AAh]
 00
004FC66A: inc eax
004FC66B: cmp eax,5
004FC66E: jl 004FC675
004FC670: mov eax,5
004FC675: mov byte ptr [esi+000000AAh],al
004FC67B: mov al,byte ptr [esi+000000B0h]
004FC681: test al,al
004FC683: jne 004FC68F
004FC685: mov eax,dword ptr [edi+10h]
004FC688: or byte ptr [eax+0000042Bh],20h
 20
004FC68F: push 4
004FC691: mov ecx,esi
004FC693: call 004F55C0
004FC698: jmp 004FBE4C
004FC69D: movzx eax,byte ptr [esi+000000AAh]
 00
004FC6A4: inc eax
004FC6A5: cmp eax,5
004FC6A8: jl 004FC6AF
004FC6AA: mov eax,5
004FC6AF: mov byte ptr [esi+000000AAh],al
004FC6B5: mov eax,dword ptr [edi+10h]
004FC6B8: mov dl,byte ptr [eax+0000042Bh]
004FC6BE: push 4
004FC6C0: or dl,20h
004FC6C3: push edi
004FC6C4: mov ecx,esi
004FC6C6: mov byte ptr [eax+0000042Bh],dl
004FC6CC: call 004F5D00
004FC6D1: push ecx
004FC6D2: fstp dword ptr [esp]
004FC6D5: push 4
004FC6D7: push edi
004FC6D8: call 004F8070
004FC6DD: push ecx
004FC6DE: fstp dword ptr [esp]
004FC6E1: push 4
004FC6E3: mov ecx,esi
004FC6E5: call 004F56D0
004FC6EA: pop ebp
004FC6EB: pop ebx
004FC6EC: pop edi
004FC6ED: mov byte ptr [esi+000000A9h],3
 03
004FC6F4: pop esi
004FC6F5: pop ecx
004FC6F6: ret 4
004FC6F9: push 4
004FC6FB: push edi
004FC6FC: mov ecx,esi
004FC6FE: call 004F8070
004FC703: fstp dword ptr [esp+10h]
004FC707: push 4
004FC709: push edi
004FC70A: mov ecx,esi
004FC70C: call 004F5D00
004FC711: fstp dword ptr [esp+18h]
004FC715: mov eax,dword ptr [esi+00000108h]
004FC71B: test eax,eax
004FC71D: jne 004FC72E
004FC71F: mov eax,dword ptr [esp+18h]
004FC723: mov ecx,dword ptr [esp+10h]
004FC727: push eax
004FC728: push ecx
004FC729: jmp 004FC918
004FC72E: mov edx,dword ptr [esp+18h]
004FC732: mov eax,dword ptr [esp+10h]
004FC736: push edx
004FC737: push eax
004FC738: push 4
004FC73A: mov ecx,esi
004FC73C: call 004F56D0
004FC741: pop ebp
004FC742: pop ebx
004FC743: pop edi
004FC744: mov byte ptr [esi+000000A9h],3
 03
004FC74B: pop esi
004FC74C: pop ecx
004FC74D: ret 4
004FC750: push 4
004FC752: mov ecx,esi
004FC754: call 004F55C0
004FC759: jmp 004FC997
004FC75E: push 4
004FC760: call 004F55C0
004FC765: jmp 004FC997
004FC76A: test byte ptr [ecx+00000428h],10h
 10
004FC771: jne 004FC793
004FC773: xor edi,edi
004FC775: push edi
004FC776: mov ecx,esi
004FC778: call 004F55C0
004FC77D: inc edi
004FC77E: cmp di,0Ch
004FC782: jl 004FC775
004FC784: pop ebp
004FC785: pop ebx
004FC786: pop edi
004FC787: mov byte ptr [esi+000000A9h],0
 00
004FC78E: pop esi
004FC78F: pop ecx
004FC790: ret 4
004FC793: test dl,dl
004FC795: je 004FC7DA
004FC797: mov cx,word ptr [edi+20h]
004FC79B: cmp cx,word ptr ds:[008CBC54h]
 00
004FC7A2: jle 004FC96F
004FC7A8: push 1
004FC7AA: push edi
004FC7AB: mov ecx,esi
004FC7AD: call 004F5D00
004FC7B2: push ecx
004FC7B3: fstp dword ptr [esp]
004FC7B6: push 1
004FC7B8: push edi
004FC7B9: call 004F8070
004FC7BE: push ecx
004FC7BF: fstp dword ptr [esp]
004FC7C2: push 1
004FC7C4: mov ecx,esi
004FC7C6: call 004F56D0
004FC7CB: pop ebp
004FC7CC: pop ebx
004FC7CD: pop edi
004FC7CE: mov byte ptr [esi+000000A9h],4
 04
004FC7D5: pop esi
004FC7D6: pop ecx
004FC7D7: ret 4
004FC7DA: cmp word ptr [edi+20h],0
004FC7DF: jle 004FC96F
004FC7E5: push edi
004FC7E6: mov ecx,esi
004FC7E8: call 004F4F70
004FC7ED: test al,al
004FC7EF: jne 004FC930
004FC7F5: push edi
004FC7F6: mov ecx,esi
004FC7F8: call 004F4FF0
004FC7FD: test al,al
004FC7FF: jne 004FC930
004FC805: push edi
004FC806: call 004F4FF0
004FC80B: test al,al
004FC80D: jne 004FC96F
004FC813: mov dx,word ptr [edi+20h]
004FC817: cmp dx,word ptr ds:[008CBC54h]
 00
004FC81E: jle 004FC96F
004FC824: movzx eax,byte ptr [edi+38h]
004FC828: cmp ax,word ptr [edi+44h]
004FC82C: jge 004FC88C
004FC82E: fld dword ptr ds:[008CBD34h]
004FC834: fadd dword ptr [edi+34h]
004FC837: fcomp dword ptr [edi+1Ch]
004FC83A: fnstsw ax
004FC83C: test ah,41h
004FC83F: jne 004FC88C
004FC841: fld dword ptr [edi+28h]
004FC844: fcomp dword ptr ds:[008CBD38h]
004FC84A: fnstsw ax
004FC84C: test ah,41h
004FC84F: jne 004FC88C
004FC851: mov eax,dword ptr [edi+10h]
004FC854: or byte ptr [eax+0000042Bh],20h
 20
004FC85B: mov ecx,dword ptr [esi+000000F0h]
004FC861: test ecx,ecx
004FC863: je 004FBE4C
004FC869: push 0
004FC86B: push 4
004FC86D: call 004EF2B0
004FC872: mov ecx,dword ptr [esi+000000F0h]
004FC878: call 004EF1C0
004FC87D: mov dword ptr [esi+000000F0h],0
004FC887: jmp 004FBE4C
004FC88C: mov ecx,dword ptr [esi+000000F0h]
004FC892: test ecx,ecx
004FC894: je 004FC8B4
004FC896: push 0
004FC898: push 4
004FC89A: call 004EF2B0
004FC89F: mov ecx,dword ptr [esi+000000F0h]
004FC8A5: call 004EF1C0
004FC8AA: mov dword ptr [esi+000000F0h],0
004FC8B4: mov al,byte ptr [esi+000000B0h]
004FC8BA: test al,al
004FC8BC: jne 004FC3A1
004FC8C2: mov al,byte ptr [edi+38h]
004FC8C5: cmp al,1
004FC8C7: movzx ecx,al
004FC8CA: ja 004FC8D1
004FC8CC: mov ecx,1
004FC8D1: movzx ebx,byte ptr [esi+000000AAh]
 00
004FC8D8: cmp ecx,ebx
004FC8DA: jge 004FC3D4
004FC8E0: cmp al,1
004FC8E2: jbe 004FC8EC
004FC8E4: movzx edx,al
004FC8E7: jmp 004FC3D6
004FC8EC: mov edx,1
004FC8F1: jmp 004FC3D6
004FC8F6: mov eax,ecx
004FC8F8: mov byte ptr [esi+000000AAh],al
004FC8FE: push 4
004FC900: push edi
004FC901: mov ecx,esi
004FC903: call 004F5D00
004FC908: push ecx
004FC909: fstp dword ptr [esp]
004FC90C: push 4
004FC90E: push edi
004FC90F: call 004F8070
004FC914: push ecx
004FC915: fstp dword ptr [esp]
004FC918: mov ecx,esi
004FC91A: push 4
004FC91C: call 004F7A50
004FC921: pop ebp
004FC922: pop ebx
004FC923: pop edi
004FC924: mov byte ptr [esi+000000A9h],3
 03
004FC92B: pop esi
004FC92C: pop ecx
004FC92D: ret 4
004FC930: mov dx,word ptr [edi+20h]
004FC934: cmp dx,word ptr ds:[008CBC54h]
 00
004FC93B: jle 004FC96F
004FC93D: push 1
004FC93F: push edi
004FC940: mov ecx,esi
004FC942: call 004F5D00
004FC947: push ecx
004FC948: fstp dword ptr [esp]
004FC94B: push 1
004FC94D: push edi
004FC94E: call 004F8070
004FC953: push ecx
004FC954: fstp dword ptr [esp]
004FC957: push 1
004FC959: mov ecx,esi
004FC95B: call 004F56D0
004FC960: pop ebp
004FC961: pop ebx
004FC962: pop edi
004FC963: mov byte ptr [esi+000000A9h],4
 04
004FC96A: pop esi
004FC96B: pop ecx
004FC96C: ret 4
004FC96F: mov ecx,dword ptr [esi+000000F0h]
004FC975: test ecx,ecx
004FC977: je 004FC997
004FC979: push 0
004FC97B: push 4
004FC97D: call 004EF2B0
004FC982: mov ecx,dword ptr [esi+000000F0h]
004FC988: call 004EF1C0
004FC98D: mov dword ptr [esi+000000F0h],0
004FC997: mov ecx,esi
004FC999: push 5
004FC99B: push edi
004FC99C: call 004F5D00
004FC9A1: push ecx
004FC9A2: fstp dword ptr [esp]
004FC9A5: push 5
004FC9A7: mov ecx,esi
004FC9A9: push edi
004FC9AA: call 004F8070
004FC9AF: push ecx
004FC9B0: fstp dword ptr [esp]
004FC9B3: mov ecx,esi
004FC9B5: push 5
004FC9B7: call 004F7A50
004FC9BC: push 2
004FC9BE: mov ecx,esi
004FC9C0: push edi
004FC9C1: call 004F5D00
004FC9C6: push ecx
004FC9C7: fstp dword ptr [esp]
004FC9CA: push 2
004FC9CC: push edi
004FC9CD: call 004F8070
004FC9D2: push ecx
004FC9D3: fstp dword ptr [esp]
004FC9D6: mov ecx,esi
004FC9D8: push 2
004FC9DA: call 004F7A50
004FC9DF: mov byte ptr [esi+000000A9h],bl
004FC9E5: pop ebp
004FC9E6: pop ebx
004FC9E7: pop edi
004FC9E8: pop esi
004FC9E9: pop ecx
004FC9EA: ret 4
004FC9ED: lea ecx,[ecx]
004FC9F0: and al,0BDh
004FC9F2: dec edi
004FC9F3: add ch,ah
004FC9F5: leave
004FC9F6: dec edi
004FC9F7: add ch,ah
004FC9F9: leave
004FC9FA: dec edi
004FC9FB: add bl,dl
004FC9FD: les ecx,fword ptr [edi]
004FCA00: push 0FFFFFFC7h
004FCA02: dec edi
004FCA03: add al,cl
004FCA05: mov esi,0C1E6004Fh
004FCA0A: dec edi
004FCA0B: add byte ptr [eax+A1909090h],dl
004FCA11: test bl,cl
004FCA13: mov bh,0
004FCA15: push ebx
004FCA16: push esi
004FCA17: mov esi,ecx
004FCA19: mov dword ptr [esi+00000144h],eax
004FCA1F: mov al,byte ptr [esi+000000A9h]
004FCA25: cmp al,1
004FCA27: push edi
004FCA28: jne 004FCC35
004FCA2E: mov ax,word ptr [esp+10h]
004FCA33: cmp ax,1
004FCA37: jne 004FCB42
004FCA3D: fld dword ptr ds:[008CBBE4h]
004FCA43: mov byte ptr [esi+000000A9h],al
004FCA49: fadd dword ptr [esi+0000023Ch]
004FCA4F: fcom dword ptr ds:[00858624h]
004FCA55: fnstsw ax
004FCA57: test ah,5
004FCA5A: jnp 004FCA64
004FCA5C: fstp st(0)
004FCA5E: fld dword ptr ds:[00858624h]
004FCA64: mov edi,dword ptr [esp+14h]
004FCA68: fst dword ptr [esp+10h]
004FCA6C: mov ecx,dword ptr [esp+10h]
004FCA70: fstp dword ptr [esi+0000023Ch]
004FCA76: push ecx
004FCA77: push edi
004FCA78: mov ecx,esi
004FCA7A: call 004F51F0
004FCA7F: fstp dword ptr [esp+10h]
004FCA83: mov edx,dword ptr [esi+0000023Ch]
004FCA89: push edx
004FCA8A: push edi
004FCA8B: mov ecx,esi
004FCA8D: call 004F5310
004FCA92: fld dword ptr [esp+10h]
004FCA96: fadd dword ptr [esi+000000D8h]
004FCA9C: mov eax,dword ptr [esi+000000F8h]
004FCAA2: xor ebx,ebx
004FCAA4: cmp eax,ebx
004FCAA6: fstp dword ptr [esp+10h]
004FCAAA: je 004FCAB8
004FCAAC: mov ecx,dword ptr [esp+10h]
004FCAB0: fstp dword ptr [eax+1Ch]
004FCAB3: mov dword ptr [eax+14h],ecx
004FCAB6: jmp 004FCABA
004FCAB8: fstp st(0)
004FCABA: fld dword ptr ds:[008CBBE8h]
004FCAC0: fadd dword ptr [esi+00000240h]
004FCAC6: fcom dword ptr ds:[00858624h]
004FCACC: fnstsw ax
004FCACE: test ah,5
004FCAD1: jnp 004FCADB
004FCAD3: fstp st(0)
004FCAD5: fld dword ptr ds:[00858624h]
004FCADB: fst dword ptr [esp+10h]
004FCADF: fst dword ptr [esi+00000240h]
004FCAE5: fcomp qword ptr ds:[00862CD8h]
004FCAEB: fnstsw ax
004FCAED: test ah,5
004FCAF0: jp 004FCC08
004FCAF6: mov edx,dword ptr [esp+10h]
004FCAFA: push edx
004FCAFB: push edi
004FCAFC: mov ecx,esi
004FCAFE: call 004F53D0
004FCB03: fstp dword ptr [esp+10h]
004FCB07: mov eax,dword ptr [esi+00000240h]
004FCB0D: push eax
004FCB0E: push edi
004FCB0F: mov ecx,esi
004FCB11: call 004F54F0
004FCB16: fld dword ptr [esp+10h]
004FCB1A: fadd dword ptr [esi+000000D8h]
004FCB20: mov esi,dword ptr [esi+000000F0h]
004FCB26: cmp esi,ebx
004FCB28: fstp dword ptr [esp+10h]
004FCB2C: je 004FCF37
004FCB32: mov ecx,dword ptr [esp+10h]
004FCB36: fstp dword ptr [esi+1Ch]
004FCB39: pop edi
004FCB3A: mov dword ptr [esi+14h],ecx
004FCB3D: pop esi
004FCB3E: pop ebx
004FCB3F: ret 0Ch
004FCB42: xor ebx,ebx
004FCB44: cmp ax,2
004FCB48: jne 004FCBDF
004FCB4E: mov edi,dword ptr [esp+14h]
004FCB52: push ebx
004FCB53: push edi
004FCB54: mov ecx,esi
004FCB56: mov byte ptr [esi+000000A9h],al
004FCB5C: mov dword ptr [esi+00000240h],ebx
004FCB62: call 004F51F0
004FCB67: fstp dword ptr [esp+10h]
004FCB6B: mov edx,dword ptr [esi+00000240h]
004FCB71: push edx
004FCB72: push edi
004FCB73: mov ecx,esi
004FCB75: call 004F5310
004FCB7A: fld dword ptr [esp+10h]
004FCB7E: fadd dword ptr [esi+000000D8h]
004FCB84: mov eax,dword ptr [esi+000000F8h]
004FCB8A: cmp eax,ebx
004FCB8C: fstp dword ptr [esp+10h]
004FCB90: je 004FCB9E
004FCB92: mov ecx,dword ptr [esp+10h]
004FCB96: fstp dword ptr [eax+1Ch]
004FCB99: mov dword ptr [eax+14h],ecx
004FCB9C: jmp 004FCBA0
004FCB9E: fstp st(0)
004FCBA0: push ebx
004FCBA1: push edi
004FCBA2: mov ecx,esi
004FCBA4: mov dword ptr [esi+0000023Ch],ebx
004FCBAA: call 004F53D0
004FCBAF: fstp dword ptr [esp+10h]
004FCBB3: mov edx,dword ptr [esi+0000023Ch]
004FCBB9: push edx
004FCBBA: push edi
004FCBBB: mov ecx,esi
004FCBBD: call 004F54F0
004FCBC2: fstp dword ptr [esp+14h]
004FCBC6: mov eax,dword ptr [esp+10h]
004FCBCA: mov ecx,dword ptr [esp+14h]
004FCBCE: push eax
004FCBCF: push ecx
004FCBD0: push 1
004FCBD2: mov ecx,esi
004FCBD4: call 004F7F20
004FCBD9: pop edi
004FCBDA: pop esi
004FCBDB: pop ebx
004FCBDC: ret 0Ch
004FCBDF: mov byte ptr [esi+000000A9h],bl
004FCBE5: mov ecx,dword ptr [esi+000000F8h]
004FCBEB: cmp ecx,ebx
004FCBED: je 004FCC08
004FCBEF: push ebx
004FCBF0: push 4
004FCBF2: call 004EF2B0
004FCBF7: mov ecx,dword ptr [esi+000000F8h]
004FCBFD: call 004EF1C0
004FCC02: mov dword ptr [esi+000000F8h],ebx
004FCC08: mov ecx,dword ptr [esi+000000F0h]
004FCC0E: cmp ecx,ebx
004FCC10: je 004FCF39
004FCC16: push ebx
004FCC17: push 4
004FCC19: call 004EF2B0
004FCC1E: mov ecx,dword ptr [esi+000000F0h]
004FCC24: call 004EF1C0
004FCC29: pop edi
004FCC2A: mov dword ptr [esi+000000F0h],ebx
004FCC30: pop esi
004FCC31: pop ebx
004FCC32: ret 0Ch
004FCC35: cmp al,2
004FCC37: jne 004FCE52
004FCC3D: mov ax,word ptr [esp+10h]
004FCC42: cmp ax,1
004FCC46: jne 004FCCDF
004FCC4C: mov edi,dword ptr [esp+14h]
004FCC50: xor ebx,ebx
004FCC52: push ebx
004FCC53: push edi
004FCC54: mov ecx,esi
004FCC56: mov byte ptr [esi+000000A9h],al
004FCC5C: mov dword ptr [esi+00000240h],ebx
004FCC62: call 004F53D0
004FCC67: fstp dword ptr [esp+10h]
004FCC6B: mov edx,dword ptr [esi+00000240h]
004FCC71: push edx
004FCC72: push edi
004FCC73: mov ecx,esi
004FCC75: call 004F54F0
004FCC7A: fld dword ptr [esp+10h]
004FCC7E: fadd dword ptr [esi+000000D8h]
004FCC84: mov eax,dword ptr [esi+000000F0h]
004FCC8A: cmp eax,ebx
004FCC8C: fstp dword ptr [esp+10h]
004FCC90: je 004FCC9E
004FCC92: mov ecx,dword ptr [esp+10h]
004FCC96: fstp dword ptr [eax+1Ch]
004FCC99: mov dword ptr [eax+14h],ecx
004FCC9C: jmp 004FCCA0
004FCC9E: fstp st(0)
004FCCA0: push ebx
004FCCA1: push edi
004FCCA2: mov ecx,esi
004FCCA4: mov dword ptr [esi+0000023Ch],ebx
004FCCAA: call 004F51F0
004FCCAF: fstp dword ptr [esp+10h]
004FCCB3: mov edx,dword ptr [esi+0000023Ch]
004FCCB9: push edx
004FCCBA: push edi
004FCCBB: mov ecx,esi
004FCCBD: call 004F5310
004FCCC2: fstp dword ptr [esp+14h]
004FCCC6: mov eax,dword ptr [esp+10h]
004FCCCA: mov ecx,dword ptr [esp+14h]
004FCCCE: push eax
004FCCCF: push ecx
004FCCD0: push 2
004FCCD2: mov ecx,esi
004FCCD4: call 004F7F20
004FCCD9: pop edi
004FCCDA: pop esi
004FCCDB: pop ebx
004FCCDC: ret 0Ch
004FCCDF: cmp ax,2
004FCCE3: jne 004FCDFA
004FCCE9: fld dword ptr ds:[008CBBECh]
004FCCEF: mov byte ptr [esi+000000A9h],al
004FCCF5: fadd dword ptr [esi+0000023Ch]
004FCCFB: fcom dword ptr ds:[00858624h]
004FCD01: fnstsw ax
004FCD03: test ah,5
004FCD06: jnp 004FCD10
004FCD08: fstp st(0)
004FCD0A: fld dword ptr ds:[00858624h]
004FCD10: mov edi,dword ptr [esp+14h]
004FCD14: fst dword ptr [esp+10h]
004FCD18: mov edx,dword ptr [esp+10h]
004FCD1C: fstp dword ptr [esi+0000023Ch]
004FCD22: push edx
004FCD23: push edi
004FCD24: mov ecx,esi
004FCD26: call 004F53D0
004FCD2B: fstp dword ptr [esp+10h]
004FCD2F: mov eax,dword ptr [esi+0000023Ch]
004FCD35: push eax
004FCD36: push edi
004FCD37: mov ecx,esi
004FCD39: call 004F54F0
004FCD3E: fstp dword ptr [esp+14h]
004FCD42: mov ecx,dword ptr [esp+10h]
004FCD46: mov edx,dword ptr [esp+14h]
004FCD4A: push ecx
004FCD4B: push edx
004FCD4C: push 1
004FCD4E: mov ecx,esi
004FCD50: call 004F56D0
004FCD55: fld dword ptr ds:[008CBBE0h]
004FCD5B: fadd dword ptr [esi+00000240h]
004FCD61: fcom dword ptr ds:[00858624h]
004FCD67: fnstsw ax
004FCD69: test ah,5
004FCD6C: jnp 004FCD76
004FCD6E: fstp st(0)
004FCD70: fld dword ptr ds:[00858624h]
004FCD76: fcom qword ptr ds:[00862CD8h]
004FCD7C: fst dword ptr [esi+00000240h]
004FCD82: fnstsw ax
004FCD84: test ah,5
004FCD87: jp 004FCDF4
004FCD89: fcom qword ptr ds:[00862D30h]
004FCD8F: fnstsw ax
004FCD91: test ah,41h
004FCD94: jne 004FCDB3
004FCD96: fcomp qword ptr ds:[00862CF8h]
004FCD9C: fnstsw ax
004FCD9E: test ah,5
004FCDA1: jp 004FCDB5
004FCDA3: mov eax,dword ptr [esp+18h]
004FCDA7: mov eax,dword ptr [eax+10h]
004FCDAA: or byte ptr [eax+0000042Bh],20h
 20
004FCDB1: jmp 004FCDB5
004FCDB3: fstp st(0)
004FCDB5: mov ecx,dword ptr [esi+00000240h]
004FCDBB: push ecx
004FCDBC: push edi
004FCDBD: mov ecx,esi
004FCDBF: call 004F51F0
004FCDC4: fstp dword ptr [esp+10h]
004FCDC8: mov edx,dword ptr [esi+00000240h]
004FCDCE: push edx
004FCDCF: push edi
004FCDD0: mov ecx,esi
004FCDD2: call 004F5310
004FCDD7: fstp dword ptr [esp+14h]
004FCDDB: mov eax,dword ptr [esp+10h]
004FCDDF: mov ecx,dword ptr [esp+14h]
004FCDE3: push eax
004FCDE4: push ecx
004FCDE5: push 2
004FCDE7: mov ecx,esi
004FCDE9: call 004F56D0
004FCDEE: pop edi
004FCDEF: pop esi
004FCDF0: pop ebx
004FCDF1: ret 0Ch
004FCDF4: fstp st(0)
004FCDF6: xor ebx,ebx
004FCDF8: jmp 004FCE25
004FCDFA: xor ebx,ebx
004FCDFC: mov byte ptr [esi+000000A9h],bl
004FCE02: mov ecx,dword ptr [esi+000000F0h]
004FCE08: cmp ecx,ebx
004FCE0A: je 004FCE25
004FCE0C: push ebx
004FCE0D: push 4
004FCE0F: call 004EF2B0
004FCE14: mov ecx,dword ptr [esi+000000F0h]
004FCE1A: call 004EF1C0
004FCE1F: mov dword ptr [esi+000000F0h],ebx
004FCE25: mov ecx,dword ptr [esi+000000F8h]
004FCE2B: cmp ecx,ebx
004FCE2D: je 004FCF39
004FCE33: push ebx
004FCE34: push 4
004FCE36: call 004EF2B0
004FCE3B: mov ecx,dword ptr [esi+000000F8h]
004FCE41: call 004EF1C0
004FCE46: pop edi
004FCE47: mov dword ptr [esi+000000F8h],ebx
004FCE4D: pop esi
004FCE4E: pop ebx
004FCE4F: ret 0Ch
004FCE52: xor ebx,ebx
004FCE54: cmp al,bl
004FCE56: je 004FCE60
004FCE58: cmp al,0Ah
004FCE5A: jne 004FCF2B
004FCE60: mov cx,word ptr [esp+10h]
004FCE65: cmp cx,1
004FCE69: jne 004FCEC8
004FCE6B: cmp al,bl
004FCE6D: jne 004FCE79
004FCE6F: push ebx
004FCE70: push 67h
004FCE72: mov ecx,esi
004FCE74: call 004F6420
004FCE79: mov edi,dword ptr [esp+14h]
004FCE7D: mov ebx,3F800000h
004FCE82: push ebx
004FCE83: push edi
004FCE84: mov ecx,esi
004FCE86: mov byte ptr [esi+000000A9h],1
 01
004FCE8D: mov dword ptr [esi+00000240h],ebx
004FCE93: mov dword ptr [esi+0000023Ch],ebx
004FCE99: call 004F51F0
004FCE9E: fstp dword ptr [esp+10h]
004FCEA2: push ebx
004FCEA3: push edi
004FCEA4: mov ecx,esi
004FCEA6: call 004F5310
004FCEAB: fstp dword ptr [esp+14h]
004FCEAF: mov edx,dword ptr [esp+10h]
004FCEB3: mov eax,dword ptr [esp+14h]
004FCEB7: push edx
004FCEB8: push eax
004FCEB9: push 2
004FCEBB: mov ecx,esi
004FCEBD: call 004F7F20
004FCEC2: pop edi
004FCEC3: pop esi
004FCEC4: pop ebx
004FCEC5: ret 0Ch
004FCEC8: cmp cx,2
004FCECC: jne 004FCF2B
004FCECE: cmp al,bl
004FCED0: jne 004FCEDC
004FCED2: push ebx
004FCED3: push 67h
004FCED5: mov ecx,esi
004FCED7: call 004F6420
004FCEDC: mov edi,dword ptr [esp+14h]
004FCEE0: mov ebx,3F800000h
004FCEE5: push ebx
004FCEE6: push edi
004FCEE7: mov ecx,esi
004FCEE9: mov byte ptr [esi+000000A9h],2
 02
004FCEF0: mov dword ptr [esi+00000240h],ebx
004FCEF6: mov dword ptr [esi+0000023Ch],ebx
004FCEFC: call 004F53D0
004FCF01: fstp dword ptr [esp+10h]
004FCF05: push ebx
004FCF06: push edi
004FCF07: mov ecx,esi
004FCF09: call 004F54F0
004FCF0E: fstp dword ptr [esp+14h]
004FCF12: mov ecx,dword ptr [esp+10h]
004FCF16: mov edx,dword ptr [esp+14h]
004FCF1A: push ecx
004FCF1B: push edx
004FCF1C: push 1
004FCF1E: mov ecx,esi
004FCF20: call 004F7F20
004FCF25: pop edi
004FCF26: pop esi
004FCF27: pop ebx
004FCF28: ret 0Ch
004FCF2B: pop edi
004FCF2C: mov byte ptr [esi+000000A9h],bl
004FCF32: pop esi
004FCF33: pop ebx
004FCF34: ret 0Ch
004FCF37: fstp st(0)
004FCF39: pop edi
004FCF3A: pop esi
004FCF3B: pop ebx
004FCF3C: ret 0Ch
004FCF40: push ecx
004FCF41: push ebx
004FCF42: push esi
004FCF43: mov esi,ecx
004FCF45: mov al,byte ptr [esi+00000080h]
004FCF4B: xor ebx,ebx
004FCF4D: cmp al,bl
004FCF4F: push edi
004FCF50: mov byte ptr [esi+000000A7h],bl
004FCF56: je 004FCFFE
004FCF5C: cmp al,1
004FCF5E: je 004FCFFE
004FCF64: cmp al,5
004FCF66: je 004FCFC8
004FCF68: cmp al,4
004FCF6A: je 004FCFC8
004FCF6C: cmp al,6
004FCF6E: je 004FCFC8
004FCF70: cmp al,2
004FCF72: je 004FD097
004FCF78: cmp al,8
004FCF7A: jne 004FCF98
004FCF7C: push 0C2C80000h
004FCF81: push 3F800000h
004FCF86: push 0FFFFFFFFh
004FCF88: call 004FA630
004FCF8D: pop edi
004FCF8E: pop esi
004FCF8F: mov dword ptr ds:[00B6B990h],ebx
004FCF95: pop ebx
004FCF96: pop ecx
004FCF97: ret
004FCF98: cmp al,9
004FCF9A: jne 004FD097
004FCFA0: mov eax,dword ptr ds:[00BD00F8h]
004FCFA5: movsx ecx,byte ptr [eax+4Dh]
004FCFA9: mov dword ptr [esp+0Ch],ecx
004FCFAD: pop edi
004FCFAE: fild dword ptr [esp+8]
004FCFB2: fsub dword ptr ds:[008CBD50h]
004FCFB8: fstp dword ptr [esi+000000D8h]
004FCFBE: pop esi
004FCFBF: mov dword ptr ds:[00B6B990h],ebx
004FCFC5: pop ebx
004FCFC6: pop ecx
004FCFC7: ret
004FCFC8: xor edi,edi
004FCFCA: lea ebx,[ebx+00000000h]
004FCFD0: push edi
004FCFD1: mov ecx,esi
004FCFD3: call 004F55C0
004FCFD8: inc edi
004FCFD9: cmp di,0Ch
004FCFDD: jl 004FCFD0
004FCFDF: pop edi
004FCFE0: mov dword ptr [esi+0000022Ch],0BF800000h
004FCFEA: mov dword ptr [esi+00000230h],0C2C80000h
004FCFF4: pop esi
004FCFF5: mov dword ptr ds:[00B6B990h],ebx
004FCFFB: pop ebx
004FCFFC: pop ecx
004FCFFD: ret
004FCFFE: mov edx,dword ptr ds:[00BD00F8h]
004FD004: movsx eax,byte ptr [edx+4Dh]
004FD008: mov dword ptr [esp+0Ch],eax
004FD00C: mov al,byte ptr [esi+000000A9h]
004FD012: xor edi,edi
004FD014: cmp al,6
004FD016: fild dword ptr [esp+0Ch]
004FD01A: mov dword ptr [esi+0000023Ch],3F800000h
004FD024: mov dword ptr [esi+00000240h],3F800000h
004FD02E: fsub dword ptr ds:[008CBD50h]
004FD034: fstp dword ptr [esi+000000D8h]
004FD03A: jne 004FD060
004FD03C: lea esp,[esp]
004FD040: cmp di,2
004FD044: je 004FD04E
004FD046: push edi
004FD047: mov ecx,esi
004FD049: call 004F55C0
004FD04E: inc edi
004FD04F: cmp di,0Ch
004FD053: jl 004FD040
004FD055: mov byte ptr [esi+000000A9h],1
 01
004FD05C: jmp 004FD07E
004FD05E: mov edi,edi
004FD060: push edi
004FD061: mov ecx,esi
004FD063: call 004F55C0
004FD068: inc edi
004FD069: cmp di,0Ch
004FD06D: jl 004FD060
004FD06F: cmp byte ptr [esi+000000A9h],bl
004FD075: je 004FD07E
004FD077: mov byte ptr [esi+000000A9h],0Ah
 0A
004FD07E: push 0C2C80000h
004FD083: push 3F800000h
004FD088: push 0FFFFFFFFh
004FD08A: mov ecx,esi
004FD08C: call 004F87D0
004FD091: mov byte ptr [esi+000000AAh],bl
004FD097: pop edi
004FD098: pop esi
004FD099: mov dword ptr ds:[00B6B990h],ebx
004FD09F: pop ebx
004FD0A0: pop ecx
004FD0A1: ret
004FD0B0: push ebx
004FD0B1: push esi
004FD0B2: mov esi,ecx
004FD0B4: mov al,byte ptr [esi+000000A5h]
004FD0BA: xor ebx,ebx
004FD0BC: cmp al,bl
004FD0BE: je 004FD0FB
004FD0C0: movsx eax,byte ptr [esi+0000009Bh]
 00
004FD0C7: sub eax,ebx
004FD0C9: je 004FD0DD
004FD0CB: sub eax,2
004FD0CE: je 004FD0DD
004FD0D0: dec eax
004FD0D1: jne 004FD0EF
004FD0D3: push ebx
004FD0D4: lea eax,[esi+00000080h]
004FD0DA: push eax
004FD0DB: jmp 004FD0E5
004FD0DD: push ebx
004FD0DE: lea ecx,[esi+00000080h]
004FD0E4: push ecx
004FD0E5: mov ecx,0B6BC90h
004FD0EA: call 00506F70
004FD0EF: mov dword ptr ds:[00B6B98Ch],ebx
004FD0F5: mov dword ptr ds:[00B6B99Ch],ebx
004FD0FB: movsx eax,byte ptr [esi+00000080h]
 00
004FD102: cmp eax,9
004FD105: push edi
004FD106: ja 004FD13F
004FD108: movzx edx,byte ptr [eax+004FD278h]
 00
004FD10F: jmp dword ptr [edx*4+004FD26Ch]
 00
004FD116: xor edi,edi
004FD118: push edi
004FD119: mov ecx,esi
004FD11B: call 004F55C0
004FD120: inc edi
004FD121: cmp di,0Ch
004FD125: jl 004FD118
004FD127: jmp 004FD13F
004FD129: xor edi,edi
004FD12B: jmp 004FD130
004FD12D: lea ecx,[ecx]
004FD130: push edi
004FD131: mov ecx,esi
004FD133: call 004F55C0
004FD138: inc edi
004FD139: cmp di,0Ch
004FD13D: jl 004FD130
004FD13F: push 0C2C80000h
004FD144: push 3F800000h
004FD149: or edi,0FFFFFFFFh
004FD14C: push edi
004FD14D: mov ecx,esi
004FD14F: call 004F8360
004FD154: mov ecx,dword ptr [esi+00000158h]
004FD15A: cmp ecx,ebx
004FD15C: je 004FD17E
004FD15E: push ebx
004FD15F: push 4
004FD161: call 004EF2B0
004FD166: mov ecx,dword ptr [esi+00000158h]
004FD16C: call 004EF1C0
004FD171: mov dword ptr [esi+00000158h],ebx
004FD177: mov word ptr [esi+00000156h],di
 00
004FD17E: mov ecx,dword ptr [esi+00000160h]
004FD184: cmp ecx,ebx
004FD186: je 004FD1A8
004FD188: push ebx
004FD189: push 4
004FD18B: call 004EF2B0
004FD190: mov ecx,dword ptr [esi+00000160h]
004FD196: call 004EF1C0
004FD19B: mov dword ptr [esi+00000160h],ebx
004FD1A1: mov word ptr [esi+0000015Ch],di
 00
004FD1A8: mov ecx,dword ptr [esi+00000168h]
004FD1AE: cmp ecx,ebx
004FD1B0: je 004FD1D2
004FD1B2: push ebx
004FD1B3: push 4
004FD1B5: call 004EF2B0
004FD1BA: mov ecx,dword ptr [esi+00000168h]
004FD1C0: call 004EF1C0
004FD1C5: mov dword ptr [esi+00000168h],ebx
004FD1CB: mov word ptr [esi+00000164h],di
 00
004FD1D2: mov ecx,dword ptr [esi+00000170h]
004FD1D8: cmp ecx,ebx
004FD1DA: je 004FD1FC
004FD1DC: push ebx
004FD1DD: push 4
004FD1DF: call 004EF2B0
004FD1E4: mov ecx,dword ptr [esi+00000170h]
004FD1EA: call 004EF1C0
004FD1EF: mov dword ptr [esi+00000170h],ebx
004FD1F5: mov word ptr [esi+0000016Ch],di
 00
004FD1FC: mov ecx,dword ptr [esi+00000178h]
004FD202: cmp ecx,ebx
004FD204: pop edi
004FD205: je 004FD220
004FD207: push ebx
004FD208: push 4
004FD20A: call 004EF2B0
004FD20F: mov ecx,dword ptr [esi+00000178h]
004FD215: call 004EF1C0
004FD21A: mov dword ptr [esi+00000178h],ebx
004FD220: mov ecx,dword ptr [esi+0000017Ch]
004FD226: cmp ecx,ebx
004FD228: je 004FD243
004FD22A: push ebx
004FD22B: push 4
004FD22D: call 004EF2B0
004FD232: mov ecx,dword ptr [esi+0000017Ch]
004FD238: call 004EF1C0
004FD23D: mov dword ptr [esi+0000017Ch],ebx
004FD243: mov ecx,dword ptr [esi+00000180h]
004FD249: cmp ecx,ebx
004FD24B: je 004FD266
004FD24D: push ebx
004FD24E: push 4
004FD250: call 004EF2B0
004FD255: mov ecx,dword ptr [esi+00000180h]
004FD25B: call 004EF1C0
004FD260: mov dword ptr [esi+00000180h],ebx
004FD266: pop esi
004FD267: pop ebx
004FD268: ret
004FD269: lea ecx,[ecx]
004FD26C: push ss
004FD26D: ror dword ptr [edi],1
004FD270: sub ecx,edx
004FD272: dec edi
004FD273: add byte ptr [edi],bh
004FD275: ror dword ptr [edi],1
004FD278: add byte ptr [eax],al
004FD27A: add byte ptr [ecx],al
004FD27C: add dword ptr [ecx],eax
004FD27E: add dword ptr [edx],eax
004FD280: add dword ptr [ecx],eax
004FD290: push 0FFFFFFFFh
004FD292: push 83C3C8h
004FD297: mov eax,dword ptr fs:[00000000h]
004FD29D: push eax
004FD29E: mov dword ptr fs:[00000000h],esp
 00
004FD2A5: sub esp,84h
004FD2AB: push ebp
004FD2AC: xor ebp,ebp
004FD2AE: push esi
004FD2AF: mov esi,ecx
004FD2B1: mov dword ptr [esp+00000084h],ebp
 00
004FD2B8: mov dword ptr [esp+00000088h],ebp
 00
004FD2BF: xor eax,eax
004FD2C1: mov ax,word ptr [esi+000000DEh]
 00
004FD2C8: push 28h
004FD2CA: mov ecx,0B5F8B8h
004FD2CF: mov dword ptr [esp+00000098h],ebp
 00
004FD2D6: mov dword ptr [esp+1Ch],ebp
004FD2DA: push eax
004FD2DB: call 004D88C0
004FD2E0: test al,al
004FD2E2: je 004FD2F9
004FD2E4: push 13h
004FD2E6: push 8Ah
004FD2EB: mov ecx,0B5F8B8h
004FD2F0: call 004D88C0
004FD2F5: test al,al
004FD2F7: jne 004FD312
004FD2F9: lea ecx,[esp+44h]
004FD2FD: mov dword ptr [esp+00000094h],0FFFFFFFFh
004FD308: call 0059ACD0
004FD30D: jmp 004FD8CF
004FD312: push ebx
004FD313: push edi
004FD314: mov edi,dword ptr [esp+000000A4h]
 00
004FD31B: mov ebx,dword ptr [edi+10h]
004FD31E: test byte ptr [ebx+00000428h],10h
 10
004FD325: jne 004FD344
004FD327: xor edi,edi
004FD329: lea esp,[esp+00000000h]
 00
004FD330: push edi
004FD331: mov ecx,esi
004FD333: call 004F55C0
004FD338: inc edi
004FD339: cmp di,0Ch
004FD33D: jl 004FD330
004FD33F: jmp 004FD8B9
004FD344: fld dword ptr [ebx+000009C4h]
004FD34A: fmul dword ptr ds:[00862D18h]
004FD350: fstp dword ptr [esp+10h]
004FD354: fld dword ptr ds:[00858624h]
004FD35A: fcomp dword ptr [esp+10h]
004FD35E: fnstsw ax
004FD360: test ah,5
004FD363: jnp 004FD380
004FD365: fld dword ptr ds:[00858B50h]
004FD36B: fcomp dword ptr [esp+10h]
004FD36F: fnstsw ax
004FD371: test ah,41h
004FD374: jne 004FD388
004FD376: mov dword ptr [esp+10h],0
004FD37E: jmp 004FD388
004FD380: mov dword ptr [esp+10h],3F800000h
004FD388: mov ecx,dword ptr [esp+10h]
004FD38C: push 8CBE00h
004FD391: push 3
004FD393: push ecx
004FD394: call 004D9D90
004FD399: fstp dword ptr [esp+1Ch]
004FD39D: add esp,0Ch
004FD3A0: push edi
004FD3A1: mov ecx,esi
004FD3A3: call 004F5080
004FD3A8: push 0B6F99Ch
004FD3AD: lea ecx,[esp+50h]
004FD3B1: call 0059BBC0
004FD3B6: mov eax,dword ptr ds:[00B6F03Ch]
004FD3BB: cmp eax,ebp
004FD3BD: lea ecx,[eax+30h]
004FD3C0: jne 004FD3C7
004FD3C2: mov ecx,0B6F02Ch
004FD3C7: mov eax,dword ptr [esi+4]
004FD3CA: mov edx,dword ptr [eax+14h]
004FD3CD: cmp edx,ebp
004FD3CF: je 004FD3D6
004FD3D1: lea eax,[edx+30h]
004FD3D4: jmp 004FD3D9
004FD3D6: add eax,4
004FD3D9: fld dword ptr [eax+8]
004FD3DC: fsub dword ptr [ecx+8]
004FD3DF: fld dword ptr [eax+4]
004FD3E2: fsub dword ptr [ecx+4]
004FD3E5: fld dword ptr [eax]
004FD3E7: fsub dword ptr [ecx]
004FD3E9: fstp dword ptr [esp+3Ch]
004FD3ED: mov edx,dword ptr [esp+3Ch]
004FD3F1: mov dword ptr [esp+24h],edx
004FD3F5: fstp dword ptr [esp+40h]
004FD3F9: mov eax,dword ptr [esp+40h]
004FD3FD: mov dword ptr [esp+28h],eax
004FD401: fstp dword ptr [esp+44h]
004FD405: mov ecx,dword ptr [esp+44h]
004FD409: mov dword ptr [esp+2Ch],ecx
004FD40D: lea ecx,[esp+24h]
004FD411: call 0059C910
004FD416: fld dword ptr [esp+28h]
004FD41A: mov edx,dword ptr [edi+10h]
004FD41D: mov eax,dword ptr [edx+14h]
004FD420: fmul dword ptr [eax+14h]
004FD423: fld dword ptr [esp+2Ch]
004FD427: fmul dword ptr [eax+18h]
004FD42A: faddp st(1),st
004FD42C: fld dword ptr [esp+24h]
004FD430: fmul dword ptr [eax+10h]
004FD433: faddp st(1),st
004FD435: fld dword ptr [eax+14h]
004FD438: fadd dword ptr [eax+10h]
004FD43B: fmul dword ptr ds:[00858B50h]
004FD441: fadd dword ptr [eax+18h]
004FD444: fmul qword ptr ds:[00862D10h]
004FD44A: fstp dword ptr [esp+30h]
004FD44E: fld dword ptr [eax+4]
004FD451: fadd dword ptr [eax]
004FD453: fmul dword ptr ds:[00858B50h]
004FD459: fadd dword ptr [eax+8]
004FD45C: fst dword ptr [esp+1Ch]
004FD460: fcomp dword ptr ds:[00858B50h]
004FD466: fnstsw ax
004FD468: test ah,5
004FD46B: jp 004FD47B
004FD46D: fld dword ptr [esp+1Ch]
004FD471: fmul qword ptr ds:[00862D10h]
004FD477: fstp dword ptr [esp+1Ch]
004FD47B: cmp word ptr [edi+20h],bp
004FD47F: fadd dword ptr ds:[00858624h]
004FD485: fmul dword ptr ds:[00858B8Ch]
004FD48B: fstp dword ptr [esp+14h]
004FD48F: jle 004FD499
004FD491: mov dword ptr [esp+20h],3DCCCCCDh
004FD499: cmp word ptr [edi+22h],bp
004FD49D: jle 004FD4AD
004FD49F: fld dword ptr [esp+20h]
004FD4A3: fsub dword ptr ds:[00858C28h]
004FD4A9: fstp dword ptr [esp+20h]
004FD4AD: fld dword ptr ds:[008CBD98h]
004FD4B3: fmul dword ptr [esp+14h]
004FD4B7: fsubr dword ptr ds:[00858624h]
004FD4BD: fmul dword ptr [esp+10h]
004FD4C1: fstp dword ptr [esp+18h]
004FD4C5: mov eax,dword ptr [esp+18h]
004FD4C9: push eax
004FD4CA: call 004D9E50
004FD4CF: fmul dword ptr ds:[00858BA4h]
004FD4D5: fadd dword ptr ds:[00859000h]
004FD4DB: fstp dword ptr [esp+38h]
004FD4DF: fld dword ptr [esp+18h]
004FD4E3: fmul dword ptr ds:[00858B8Ch]
004FD4E9: fadd dword ptr ds:[00858B8Ch]
004FD4EF: fmul dword ptr [esp+14h]
004FD4F3: fstp dword ptr [esp+18h]
004FD4F7: mov ecx,dword ptr [esp+18h]
004FD4FB: push ecx
004FD4FC: call 004D9E50
004FD501: fmul dword ptr ds:[00858BA4h]
004FD507: mov ebp,dword ptr [esp+18h]
004FD50B: push ebp
004FD50C: fadd dword ptr ds:[00859000h]
004FD512: fstp dword ptr [esp+44h]
004FD516: call 004D9E50
004FD51B: fmul dword ptr ds:[00858BA4h]
004FD521: mov eax,dword ptr ds:[00B6F03Ch]
004FD526: add esp,0Ch
004FD529: test eax,eax
004FD52B: fadd dword ptr ds:[00859000h]
004FD531: lea edx,[eax+30h]
004FD534: fstp dword ptr [esp+48h]
004FD538: jne 004FD53F
004FD53A: mov edx,0B6F02Ch
004FD53F: mov eax,dword ptr [esi+4]
004FD542: mov ecx,dword ptr [eax+14h]
004FD545: test ecx,ecx
004FD547: je 004FD54E
004FD549: lea eax,[ecx+30h]
004FD54C: jmp 004FD551
004FD54E: add eax,4
004FD551: fld dword ptr [eax+8]
004FD554: fsub dword ptr [edx+8]
004FD557: fld dword ptr [eax+4]
004FD55A: fsub dword ptr [edx+4]
004FD55D: fld dword ptr [eax]
004FD55F: fsub dword ptr [edx]
004FD561: fld st(0)
004FD563: fmul st,st(1)
004FD565: fld st(2)
004FD567: fmul st,st(3)
004FD569: faddp st(1),st
004FD56B: fld st(3)
004FD56D: fmul st,st(4)
004FD56F: faddp st(1),st
004FD571: fstp st(3)
004FD573: fstp st(0)
004FD575: fstp st(0)
004FD577: fsqrt
004FD579: fst dword ptr [esp+10h]
004FD57D: fcomp dword ptr ds:[00858BF4h]
004FD583: fnstsw ax
004FD585: test ah,5
004FD588: jp 004FD5E2
004FD58A: fld dword ptr [esp+10h]
004FD58E: fcomp dword ptr ds:[00862D38h]
004FD594: fnstsw ax
004FD596: test ah,5
004FD599: jp 004FD5A5
004FD59B: mov dword ptr [esp+14h],0C2C80000h
004FD5A3: jmp 004FD5FB
004FD5A5: fld dword ptr [esp+10h]
004FD5A9: fsub dword ptr ds:[00862D38h]
004FD5AF: fmul dword ptr ds:[0085A5C4h]
004FD5B5: fstp dword ptr [esp+14h]
004FD5B9: mov edx,dword ptr [esp+14h]
004FD5BD: push edx
004FD5BE: call 004D9E50
004FD5C3: fmul dword ptr ds:[00858BA4h]
004FD5C9: push ebp
004FD5CA: fstp dword ptr [esp+1Ch]
004FD5CE: call 004D9E50
004FD5D3: fmul dword ptr ds:[00858BA4h]
004FD5D9: add esp,8
004FD5DC: fadd dword ptr [esp+14h]
004FD5E0: jmp 004FD5F1
004FD5E2: push ebp
004FD5E3: call 004D9E50
004FD5E8: fmul dword ptr ds:[00858BA4h]
004FD5EE: add esp,4
004FD5F1: fadd dword ptr ds:[00859000h]
004FD5F7: fstp dword ptr [esp+14h]
004FD5FB: cmp word ptr [esi+7Ch],0
004FD600: je 004FD60F
004FD602: mov ax,word ptr ds:[008CBDC4h]
004FD608: mov word ptr [esi+000000B4h],ax
 00
004FD60F: cmp word ptr [esi+000000B4h],0
004FD617: mov dword ptr [esp+10h],3F800000h
004FD61F: jle 004FD644
004FD621: mov al,byte ptr [ebx+00000960h]
004FD627: test al,al
004FD629: jne 004FD644
004FD62B: mov ecx,dword ptr ds:[008CBDD0h]
004FD631: mov edx,dword ptr ds:[008CBDD4h]
004FD637: mov dword ptr [esp+10h],ecx
004FD63B: mov dword ptr [esp+18h],edx
004FD63F: jmp 004FD6E2
004FD644: mov ecx,dword ptr [edi+10h]
004FD647: fld dword ptr [ecx+000004C0h]
004FD64D: fcomp dword ptr ds:[00859F80h]
004FD653: fnstsw ax
004FD655: test ah,5
004FD658: jp 004FD667
004FD65A: mov eax,dword ptr ds:[008CBDD8h]
004FD65F: mov ecx,dword ptr ds:[008CBDDCh]
004FD665: jmp 004FD6DA
004FD667: fld dword ptr [ecx+000004C0h]
004FD66D: fcomp dword ptr ds:[00862D28h]
004FD673: fnstsw ax
004FD675: test ah,5
004FD678: jp 004FD68F
004FD67A: mov edx,dword ptr ds:[008CBDE0h]
004FD680: mov eax,dword ptr ds:[008CBDE4h]
004FD685: mov dword ptr [esp+10h],edx
004FD689: mov dword ptr [esp+18h],eax
004FD68D: jmp 004FD6E2
004FD68F: fld dword ptr [ecx+000004C0h]
004FD695: fcomp dword ptr ds:[00862D24h]
004FD69B: fnstsw ax
004FD69D: test ah,5
004FD6A0: jp 004FD6B8
004FD6A2: mov ecx,dword ptr ds:[008CBDE8h]
004FD6A8: mov edx,dword ptr ds:[008CBDECh]
004FD6AE: mov dword ptr [esp+10h],ecx
004FD6B2: mov dword ptr [esp+18h],edx
004FD6B6: jmp 004FD6E2
004FD6B8: fld dword ptr [ecx+000004C0h]
004FD6BE: fcomp dword ptr ds:[00862D20h]
004FD6C4: fnstsw ax
004FD6C6: test ah,5
004FD6C9: jp 004FD7AB
004FD6CF: mov eax,dword ptr ds:[008CBDF0h]
004FD6D4: mov ecx,dword ptr ds:[008CBDF4h]
004FD6DA: mov dword ptr [esp+10h],eax
004FD6DE: mov dword ptr [esp+18h],ecx
004FD6E2: mov edx,dword ptr ds:[00B7CB84h]
004FD6E8: cmp edx,dword ptr [esi+000000B8h]
004FD6EE: jbe 004FD788
004FD6F4: mov al,byte ptr [esi+000000BCh]
004FD6FA: test al,al
004FD6FC: je 004FD73C
004FD6FE: mov eax,dword ptr ds:[008CBDC8h]
004FD703: push eax
004FD704: mov byte ptr [esi+000000BCh],0
 00
004FD70B: call 004D9C80
004FD710: mov edi,dword ptr ds:[00B7CB84h]
004FD716: add esp,4
004FD719: test al,al
004FD71B: je 004FD72D
004FD71D: mov ecx,dword ptr ds:[008CBDA8h]
004FD723: mov edx,dword ptr ds:[008CBDA4h]
004FD729: push ecx
004FD72A: push edx
004FD72B: jmp 004FD778
004FD72D: mov eax,dword ptr ds:[008CBDB0h]
004FD732: mov ecx,dword ptr ds:[008CBDACh]
004FD738: push eax
004FD739: push ecx
004FD73A: jmp 004FD778
004FD73C: mov edx,dword ptr ds:[008CBDCCh]
004FD742: push edx
004FD743: mov byte ptr [esi+000000BCh],1
 01
004FD74A: call 004D9C80
004FD74F: mov edi,dword ptr ds:[00B7CB84h]
004FD755: add esp,4
004FD758: test al,al
004FD75A: je 004FD76B
004FD75C: mov eax,dword ptr ds:[008CBDB8h]
004FD761: mov ecx,dword ptr ds:[008CBDB4h]
004FD767: push eax
004FD768: push ecx
004FD769: jmp 004FD778
004FD76B: mov edx,dword ptr ds:[008CBDC0h]
004FD771: mov eax,dword ptr ds:[008CBDBCh]
004FD776: push edx
004FD777: push eax
004FD778: call 004D9C10
004FD77D: add esp,8
004FD780: add eax,edi
004FD782: mov dword ptr [esi+000000B8h],eax
004FD788: movsx eax,word ptr [esi+000000B4h]
 00
004FD78F: dec eax
004FD790: xor ecx,ecx
004FD792: test eax,eax
004FD794: setl cl
004FD797: dec ecx
004FD798: and eax,ecx
004FD79A: mov word ptr [esi+000000B4h],ax
 00
004FD7A1: mov al,byte ptr [esi+000000BCh]
004FD7A7: test al,al
004FD7A9: jne 004FD7B3
004FD7AB: mov dword ptr [esp+18h],0
004FD7B3: fld dword ptr [esp+1Ch]
004FD7B7: fmul dword ptr ds:[00858B1Ch]
004FD7BD: fld dword ptr [esp+30h]
004FD7C1: fmul dword ptr ds:[00858FCCh]
004FD7C7: faddp st(1),st
004FD7C9: fadd dword ptr [esp+20h]
004FD7CD: fadd dword ptr ds:[00858624h]
004FD7D3: fmul dword ptr [esp+10h]
004FD7D7: fstp dword ptr [esp+10h]
004FD7DB: fld dword ptr [esi+0000022Ch]
004FD7E1: fcomp dword ptr ds:[00858B50h]
004FD7E7: fnstsw ax
004FD7E9: test ah,5
004FD7EC: jp 004FD7F8
004FD7EE: mov edx,dword ptr [esp+10h]
004FD7F2: mov dword ptr [esi+0000022Ch],edx
004FD7F8: fld dword ptr [esp+10h]
004FD7FC: fcomp dword ptr [esi+0000022Ch]
004FD802: fld dword ptr [esi+0000022Ch]
004FD808: fnstsw ax
004FD80A: test ah,1
004FD80D: jne 004FD828
004FD80F: fadd dword ptr ds:[00862D04h]
004FD815: fld dword ptr [esp+10h]
004FD819: fcomp st(1)
004FD81B: fnstsw ax
004FD81D: test ah,5
004FD820: jnp 004FD83B
004FD822: fstp dword ptr [esp+10h]
004FD826: jmp 004FD83D
004FD828: fsub dword ptr ds:[00862D04h]
004FD82E: fld dword ptr [esp+10h]
004FD832: fcomp st(1)
004FD834: fnstsw ax
004FD836: test ah,41h
004FD839: jne 004FD822
004FD83B: fstp st(0)
004FD83D: mov eax,dword ptr [esp+10h]
004FD841: fld dword ptr [esp+18h]
004FD845: fadd dword ptr [esp+34h]
004FD849: mov edi,eax
004FD84B: push edi
004FD84C: push ecx
004FD84D: fstp dword ptr [esp]
004FD850: push 0
004FD852: push 28h
004FD854: push 2
004FD856: mov ecx,esi
004FD858: mov dword ptr [esi+0000022Ch],eax
004FD85E: call 004F93C0
004FD863: fld dword ptr [esp+18h]
004FD867: fadd dword ptr [esp+38h]
004FD86B: push edi
004FD86C: push ecx
004FD86D: mov ecx,esi
004FD86F: fstp dword ptr [esp]
004FD872: push 1
004FD874: push 28h
004FD876: push 4
004FD878: call 004F93C0
004FD87D: fld dword ptr [esp+18h]
004FD881: fadd dword ptr [esp+48h]
004FD885: push 3F800000h
004FD88A: push ecx
004FD88B: mov ecx,esi
004FD88D: fstp dword ptr [esp]
004FD890: push 11h
004FD892: push 13h
004FD894: push 3
004FD896: call 004F93C0
004FD89B: fld dword ptr [esp+18h]
004FD89F: fadd dword ptr [esp+14h]
004FD8A3: push 3F800000h
004FD8A8: push ecx
004FD8A9: mov ecx,esi
004FD8AB: fstp dword ptr [esp]
004FD8AE: push 10h
004FD8B0: push 13h
004FD8B2: push 1
004FD8B4: call 004F93C0
004FD8B9: lea ecx,[esp+4Ch]
004FD8BD: mov dword ptr [esp+0000009Ch],0FFFFFFFFh
004FD8C8: call 0059ACD0
004FD8CD: pop edi
004FD8CE: pop ebx
004FD8CF: mov ecx,dword ptr [esp+0000008Ch]
 00
004FD8D6: pop esi
004FD8D7: pop ebp
004FD8D8: mov dword ptr fs:[00000000h],ecx
 00
004FD8DF: add esp,90h
004FD8E5: ret 4
004FD8F0: push 0FFFFFFFFh
004FD8F2: push 83C3E8h
004FD8F7: mov eax,dword ptr fs:[00000000h]
004FD8FD: push eax
004FD8FE: mov dword ptr fs:[00000000h],esp
 00
004FD905: sub esp,84h
004FD90B: push ebp
004FD90C: xor ebp,ebp
004FD90E: push esi
004FD90F: mov esi,ecx
004FD911: mov dword ptr [esp+00000084h],ebp
 00
004FD918: mov dword ptr [esp+00000088h],ebp
 00
004FD91F: push 13h
004FD921: push 8Ah
004FD926: mov ecx,0B5F8B8h
004FD92B: mov dword ptr [esp+0000009Ch],ebp
 00
004FD932: mov dword ptr [esp+20h],ebp
004FD936: call 004D88C0
004FD93B: test al,al
004FD93D: je 004FD959
004FD93F: xor eax,eax
004FD941: mov ax,word ptr [esi+000000DEh]
 00
004FD948: push 28h
004FD94A: mov ecx,0B5F8B8h
004FD94F: push eax
004FD950: call 004D88C0
004FD955: test al,al
004FD957: jne 004FD972
004FD959: lea ecx,[esp+44h]
004FD95D: mov dword ptr [esp+00000094h],0FFFFFFFFh
004FD968: call 0059ACD0
004FD96D: jmp 004FDFAB
004FD972: mov ecx,dword ptr [esp+0000009Ch]
 00
004FD979: push ebx
004FD97A: push edi
004FD97B: mov edi,dword ptr [ecx+10h]
004FD97E: mov cl,byte ptr [edi+00000424h]
004FD984: mov al,byte ptr [edi+00000428h]
004FD98A: test cl,cl
004FD98C: setge bl
004FD98F: test al,10h
004FD991: jne 004FD9B4
004FD993: test bl,bl
004FD995: jne 004FD9B4
004FD997: xor edi,edi
004FD999: lea esp,[esp+00000000h]
 00
004FD9A0: push edi
004FD9A1: mov ecx,esi
004FD9A3: call 004F55C0
004FD9A8: inc edi
004FD9A9: cmp di,0Ch
004FD9AD: jl 004FD9A0
004FD9AF: jmp 004FDF95
004FD9B4: fld dword ptr [edi+0000049Ch]
004FD9BA: fmul dword ptr ds:[00859AACh]
004FD9C0: call 00821B40
004FD9C5: mov edx,dword ptr [esp+000000A4h]
 00
004FD9CC: mov bp,ax
004FD9CF: mov word ptr [edx+20h],bp
004FD9D3: fld dword ptr [edi+000004A0h]
004FD9D9: fmul dword ptr ds:[00859AACh]
004FD9DF: call 00821B40
004FD9E4: cmp bp,ax
004FD9E7: mov ecx,dword ptr [esp+000000A4h]
 00
004FD9EE: mov word ptr [ecx+22h],ax
004FD9F2: jle 004FD9F6
004FD9F4: mov eax,ebp
004FD9F6: movsx edx,ax
004FD9F9: mov dword ptr [esp+18h],edx
004FD9FD: fild dword ptr [esp+18h]
004FDA01: fmul dword ptr ds:[00859A3Ch]
004FDA07: fld dword ptr ds:[00858624h]
004FDA0D: fcomp st(1)
004FDA0F: fnstsw ax
004FDA11: test ah,5
004FDA14: jnp 004FDA2F
004FDA16: fld dword ptr ds:[00858B50h]
004FDA1C: fcomp st(1)
004FDA1E: fnstsw ax
004FDA20: test ah,41h
004FDA23: jne 004FDA37
004FDA25: fstp st(0)
004FDA27: fld dword ptr ds:[00858B50h]
004FDA2D: jmp 004FDA37
004FDA2F: fstp st(0)
004FDA31: fld dword ptr ds:[00858624h]
004FDA37: test bl,bl
004FDA39: jne 004FDA4B
004FDA3B: fstp st(0)
004FDA3D: fld dword ptr [edi+000009C4h]
004FDA43: fmul dword ptr ds:[00862D18h]
004FDA49: jmp 004FDA57
004FDA4B: fmul dword ptr ds:[00858C24h]
004FDA51: fadd dword ptr ds:[00858CB0h]
004FDA57: fstp dword ptr [esp+10h]
004FDA5B: fld dword ptr ds:[00858624h]
004FDA61: fcomp dword ptr [esp+10h]
004FDA65: fnstsw ax
004FDA67: test ah,5
004FDA6A: jnp 004FDA87
004FDA6C: fld dword ptr ds:[00858B50h]
004FDA72: fcomp dword ptr [esp+10h]
004FDA76: fnstsw ax
004FDA78: test ah,41h
004FDA7B: jne 004FDA8F
004FDA7D: mov dword ptr [esp+10h],0
004FDA85: jmp 004FDA8F
004FDA87: mov dword ptr [esp+10h],3F800000h
004FDA8F: mov eax,dword ptr [esp+10h]
004FDA93: push 8CBE00h
004FDA98: push 3
004FDA9A: push eax
004FDA9B: call 004D9D90
004FDAA0: fstp dword ptr [esp+1Ch]
004FDAA4: add esp,0Ch
004FDAA7: push 0B6F99Ch
004FDAAC: lea ecx,[esp+50h]
004FDAB0: call 0059BBC0
004FDAB5: mov eax,dword ptr ds:[00B6F03Ch]
004FDABA: test eax,eax
004FDABC: lea ecx,[eax+30h]
004FDABF: jne 004FDAC6
004FDAC1: mov ecx,0B6F02Ch
004FDAC6: mov eax,dword ptr [esi+4]
004FDAC9: mov edx,dword ptr [eax+14h]
004FDACC: test edx,edx
004FDACE: je 004FDAD5
004FDAD0: lea eax,[edx+30h]
004FDAD3: jmp 004FDAD8
004FDAD5: add eax,4
004FDAD8: fld dword ptr [eax+8]
004FDADB: fsub dword ptr [ecx+8]
004FDADE: fld dword ptr [eax+4]
004FDAE1: fsub dword ptr [ecx+4]
004FDAE4: fld dword ptr [eax]
004FDAE6: fsub dword ptr [ecx]
004FDAE8: fstp dword ptr [esp+3Ch]
004FDAEC: mov ecx,dword ptr [esp+3Ch]
004FDAF0: mov dword ptr [esp+24h],ecx
004FDAF4: fstp dword ptr [esp+40h]
004FDAF8: mov edx,dword ptr [esp+40h]
004FDAFC: lea ecx,[esp+24h]
004FDB00: fstp dword ptr [esp+44h]
004FDB04: mov eax,dword ptr [esp+44h]
004FDB08: mov dword ptr [esp+28h],edx
004FDB0C: mov dword ptr [esp+2Ch],eax
004FDB10: call 0059C910
004FDB15: fld dword ptr [esp+28h]
004FDB19: mov edi,dword ptr [esp+000000A4h]
 00
004FDB20: mov ecx,dword ptr [edi+10h]
004FDB23: mov eax,dword ptr [ecx+14h]
004FDB26: fmul dword ptr [eax+14h]
004FDB29: fld dword ptr [esp+2Ch]
004FDB2D: fmul dword ptr [eax+18h]
004FDB30: faddp st(1),st
004FDB32: fld dword ptr [esp+24h]
004FDB36: fmul dword ptr [eax+10h]
004FDB39: faddp st(1),st
004FDB3B: fld dword ptr [eax+14h]
004FDB3E: fadd dword ptr [eax+10h]
004FDB41: fmul dword ptr ds:[00858B50h]
004FDB47: fadd dword ptr [eax+18h]
004FDB4A: fmul qword ptr ds:[00862D10h]
004FDB50: fstp dword ptr [esp+30h]
004FDB54: fld dword ptr [eax+4]
004FDB57: fadd dword ptr [eax]
004FDB59: fmul dword ptr ds:[00858B50h]
004FDB5F: fadd dword ptr [eax+8]
004FDB62: fst dword ptr [esp+1Ch]
004FDB66: fcomp dword ptr ds:[00858B50h]
004FDB6C: fnstsw ax
004FDB6E: test ah,5
004FDB71: jp 004FDB81
004FDB73: fld dword ptr [esp+1Ch]
004FDB77: fmul qword ptr ds:[00862D10h]
004FDB7D: fstp dword ptr [esp+1Ch]
004FDB81: cmp word ptr [edi+20h],0
004FDB86: fadd dword ptr ds:[00858624h]
004FDB8C: fmul dword ptr ds:[00858B8Ch]
004FDB92: fstp dword ptr [esp+18h]
004FDB96: jle 004FDBA0
004FDB98: mov dword ptr [esp+20h],3DCCCCCDh
004FDBA0: cmp word ptr [edi+22h],0
004FDBA5: jle 004FDBB5
004FDBA7: fld dword ptr [esp+20h]
004FDBAB: fsub dword ptr ds:[00858C28h]
004FDBB1: fstp dword ptr [esp+20h]
004FDBB5: fld dword ptr ds:[008CBD98h]
004FDBBB: fmul dword ptr [esp+18h]
004FDBBF: fsubr dword ptr ds:[00858624h]
004FDBC5: fmul dword ptr [esp+10h]
004FDBC9: fstp dword ptr [esp+14h]
004FDBCD: mov edx,dword ptr [esp+14h]
004FDBD1: push edx
004FDBD2: call 004D9E50
004FDBD7: fmul dword ptr ds:[00858BA4h]
004FDBDD: fadd dword ptr ds:[00859000h]
004FDBE3: fstp dword ptr [esp+38h]
004FDBE7: fld dword ptr [esp+1Ch]
004FDBEB: fmul dword ptr ds:[00858B8Ch]
004FDBF1: fadd dword ptr ds:[00858B8Ch]
004FDBF7: fmul dword ptr [esp+14h]
004FDBFB: fstp dword ptr [esp+1Ch]
004FDBFF: mov eax,dword ptr [esp+1Ch]
004FDC03: push eax
004FDC04: call 004D9E50
004FDC09: fmul dword ptr ds:[00858BA4h]
004FDC0F: mov ebx,dword ptr [esp+18h]
004FDC13: push ebx
004FDC14: fadd dword ptr ds:[00859000h]
004FDC1A: fstp dword ptr [esp+44h]
004FDC1E: call 004D9E50
004FDC23: fmul dword ptr ds:[00858BA4h]
004FDC29: mov eax,dword ptr ds:[00B6F03Ch]
004FDC2E: add esp,0Ch
004FDC31: test eax,eax
004FDC33: fadd dword ptr ds:[00859000h]
004FDC39: lea edx,[eax+30h]
004FDC3C: fstp dword ptr [esp+48h]
004FDC40: jne 004FDC47
004FDC42: mov edx,0B6F02Ch
004FDC47: mov eax,dword ptr [esi+4]
004FDC4A: mov ecx,dword ptr [eax+14h]
004FDC4D: test ecx,ecx
004FDC4F: je 004FDC56
004FDC51: lea eax,[ecx+30h]
004FDC54: jmp 004FDC59
004FDC56: add eax,4
004FDC59: fld dword ptr [eax+8]
004FDC5C: fsub dword ptr [edx+8]
004FDC5F: fld dword ptr [eax+4]
004FDC62: fsub dword ptr [edx+4]
004FDC65: fld dword ptr [eax]
004FDC67: fsub dword ptr [edx]
004FDC69: fld st(0)
004FDC6B: fmul st,st(1)
004FDC6D: fld st(2)
004FDC6F: fmul st,st(3)
004FDC71: faddp st(1),st
004FDC73: fld st(3)
004FDC75: fmul st,st(4)
004FDC77: faddp st(1),st
004FDC79: fstp st(3)
004FDC7B: fstp st(0)
004FDC7D: fstp st(0)
004FDC7F: fsqrt
004FDC81: fst dword ptr [esp+10h]
004FDC85: fcomp dword ptr ds:[00858BF4h]
004FDC8B: fnstsw ax
004FDC8D: test ah,5
004FDC90: jp 004FDCEA
004FDC92: fld dword ptr [esp+10h]
004FDC96: fcomp dword ptr ds:[00862D38h]
004FDC9C: fnstsw ax
004FDC9E: test ah,5
004FDCA1: jp 004FDCAD
004FDCA3: mov dword ptr [esp+18h],0C2C80000h
004FDCAB: jmp 004FDD03
004FDCAD: fld dword ptr [esp+10h]
004FDCB1: fsub dword ptr ds:[00862D38h]
004FDCB7: fmul dword ptr ds:[0085A5C4h]
004FDCBD: fstp dword ptr [esp+18h]
004FDCC1: mov ecx,dword ptr [esp+18h]
004FDCC5: push ecx
004FDCC6: call 004D9E50
004FDCCB: fmul dword ptr ds:[00858BA4h]
004FDCD1: push ebx
004FDCD2: fstp dword ptr [esp+20h]
004FDCD6: call 004D9E50
004FDCDB: fmul dword ptr ds:[00858BA4h]
004FDCE1: add esp,8
004FDCE4: fadd dword ptr [esp+18h]
004FDCE8: jmp 004FDCF9
004FDCEA: push ebx
004FDCEB: call 004D9E50
004FDCF0: fmul dword ptr ds:[00858BA4h]
004FDCF6: add esp,4
004FDCF9: fadd dword ptr ds:[00859000h]
004FDCFF: fstp dword ptr [esp+18h]
004FDD03: cmp word ptr [esi+7Ch],0
004FDD08: je 004FDD18
004FDD0A: mov dx,word ptr ds:[008CBDC4h]
 00
004FDD11: mov word ptr [esi+000000B4h],dx
 00
004FDD18: mov ecx,dword ptr [edi+10h]
004FDD1B: fld dword ptr [ecx+000004C0h]
004FDD21: mov dword ptr [esp+14h],3F800000h
004FDD29: fcomp dword ptr ds:[00859F80h]
004FDD2F: fnstsw ax
004FDD31: test ah,5
004FDD34: jp 004FDD43
004FDD36: mov eax,dword ptr ds:[008CBDD8h]
004FDD3B: mov ecx,dword ptr ds:[008CBDDCh]
004FDD41: jmp 004FDDB6
004FDD43: fld dword ptr [ecx+000004C0h]
004FDD49: fcomp dword ptr ds:[00862D28h]
004FDD4F: fnstsw ax
004FDD51: test ah,5
004FDD54: jp 004FDD6B
004FDD56: mov edx,dword ptr ds:[008CBDE0h]
004FDD5C: mov eax,dword ptr ds:[008CBDE4h]
004FDD61: mov dword ptr [esp+14h],edx
004FDD65: mov dword ptr [esp+10h],eax
004FDD69: jmp 004FDDBE
004FDD6B: fld dword ptr [ecx+000004C0h]
004FDD71: fcomp dword ptr ds:[00862D24h]
004FDD77: fnstsw ax
004FDD79: test ah,5
004FDD7C: jp 004FDD94
004FDD7E: mov ecx,dword ptr ds:[008CBDE8h]
004FDD84: mov edx,dword ptr ds:[008CBDECh]
004FDD8A: mov dword ptr [esp+14h],ecx
004FDD8E: mov dword ptr [esp+10h],edx
004FDD92: jmp 004FDDBE
004FDD94: fld dword ptr [ecx+000004C0h]
004FDD9A: fcomp dword ptr ds:[00862D20h]
004FDDA0: fnstsw ax
004FDDA2: test ah,5
004FDDA5: jp 004FDE87
004FDDAB: mov eax,dword ptr ds:[008CBDF0h]
004FDDB0: mov ecx,dword ptr ds:[008CBDF4h]
004FDDB6: mov dword ptr [esp+14h],eax
004FDDBA: mov dword ptr [esp+10h],ecx
004FDDBE: mov edx,dword ptr ds:[00B7CB84h]
004FDDC4: cmp edx,dword ptr [esi+000000B8h]
004FDDCA: jbe 004FDE64
004FDDD0: mov al,byte ptr [esi+000000BCh]
004FDDD6: test al,al
004FDDD8: je 004FDE18
004FDDDA: mov eax,dword ptr ds:[008CBDC8h]
004FDDDF: push eax
004FDDE0: mov byte ptr [esi+000000BCh],0
 00
004FDDE7: call 004D9C80
004FDDEC: mov edi,dword ptr ds:[00B7CB84h]
004FDDF2: add esp,4
004FDDF5: test al,al
004FDDF7: je 004FDE09
004FDDF9: mov ecx,dword ptr ds:[008CBDA8h]
004FDDFF: mov edx,dword ptr ds:[008CBDA4h]
004FDE05: push ecx
004FDE06: push edx
004FDE07: jmp 004FDE54
004FDE09: mov eax,dword ptr ds:[008CBDB0h]
004FDE0E: mov ecx,dword ptr ds:[008CBDACh]
004FDE14: push eax
004FDE15: push ecx
004FDE16: jmp 004FDE54
004FDE18: mov edx,dword ptr ds:[008CBDCCh]
004FDE1E: push edx
004FDE1F: mov byte ptr [esi+000000BCh],1
 01
004FDE26: call 004D9C80
004FDE2B: mov edi,dword ptr ds:[00B7CB84h]
004FDE31: add esp,4
004FDE34: test al,al
004FDE36: je 004FDE47
004FDE38: mov eax,dword ptr ds:[008CBDB8h]
004FDE3D: mov ecx,dword ptr ds:[008CBDB4h]
004FDE43: push eax
004FDE44: push ecx
004FDE45: jmp 004FDE54
004FDE47: mov edx,dword ptr ds:[008CBDC0h]
004FDE4D: mov eax,dword ptr ds:[008CBDBCh]
004FDE52: push edx
004FDE53: push eax
004FDE54: call 004D9C10
004FDE59: add esp,8
004FDE5C: add eax,edi
004FDE5E: mov dword ptr [esi+000000B8h],eax
004FDE64: movsx eax,word ptr [esi+000000B4h]
 00
004FDE6B: dec eax
004FDE6C: xor ecx,ecx
004FDE6E: test eax,eax
004FDE70: setl cl
004FDE73: dec ecx
004FDE74: and eax,ecx
004FDE76: mov word ptr [esi+000000B4h],ax
 00
004FDE7D: mov al,byte ptr [esi+000000BCh]
004FDE83: test al,al
004FDE85: jne 004FDE8F
004FDE87: mov dword ptr [esp+10h],0
004FDE8F: fld dword ptr [esp+1Ch]
004FDE93: fmul dword ptr ds:[00858B1Ch]
004FDE99: fld dword ptr [esp+30h]
004FDE9D: fmul dword ptr ds:[00858FCCh]
004FDEA3: faddp st(1),st
004FDEA5: fadd dword ptr [esp+20h]
004FDEA9: fadd dword ptr ds:[00858624h]
004FDEAF: fmul dword ptr [esp+14h]
004FDEB3: fstp dword ptr [esp+14h]
004FDEB7: fld dword ptr [esi+0000022Ch]
004FDEBD: fcomp dword ptr ds:[00858B50h]
004FDEC3: fnstsw ax
004FDEC5: test ah,5
004FDEC8: jp 004FDED4
004FDECA: mov edx,dword ptr [esp+14h]
004FDECE: mov dword ptr [esi+0000022Ch],edx
004FDED4: fld dword ptr [esp+14h]
004FDED8: fcomp dword ptr [esi+0000022Ch]
004FDEDE: fld dword ptr [esi+0000022Ch]
004FDEE4: fnstsw ax
004FDEE6: test ah,1
004FDEE9: jne 004FDF04
004FDEEB: fadd dword ptr ds:[00862D04h]
004FDEF1: fld dword ptr [esp+14h]
004FDEF5: fcomp st(1)
004FDEF7: fnstsw ax
004FDEF9: test ah,5
004FDEFC: jnp 004FDF17
004FDEFE: fstp dword ptr [esp+14h]
004FDF02: jmp 004FDF19
004FDF04: fsub dword ptr ds:[00862D04h]
004FDF0A: fld dword ptr [esp+14h]
004FDF0E: fcomp st(1)
004FDF10: fnstsw ax
004FDF12: test ah,41h
004FDF15: jne 004FDEFE
004FDF17: fstp st(0)
004FDF19: mov eax,dword ptr [esp+14h]
004FDF1D: fld dword ptr [esp+10h]
004FDF21: fadd dword ptr [esp+34h]
004FDF25: mov edi,eax
004FDF27: push edi
004FDF28: push ecx
004FDF29: fstp dword ptr [esp]
004FDF2C: push 0
004FDF2E: push 28h
004FDF30: push 2
004FDF32: mov ecx,esi
004FDF34: mov dword ptr [esi+0000022Ch],eax
004FDF3A: call 004F93C0
004FDF3F: fld dword ptr [esp+10h]
004FDF43: fadd dword ptr [esp+38h]
004FDF47: push edi
004FDF48: push ecx
004FDF49: mov ecx,esi
004FDF4B: fstp dword ptr [esp]
004FDF4E: push 1
004FDF50: push 28h
004FDF52: push 4
004FDF54: call 004F93C0
004FDF59: fld dword ptr [esp+10h]
004FDF5D: fadd dword ptr [esp+48h]
004FDF61: push 3F800000h
004FDF66: push ecx
004FDF67: mov ecx,esi
004FDF69: fstp dword ptr [esp]
004FDF6C: push 11h
004FDF6E: push 13h
004FDF70: push 3
004FDF72: call 004F93C0
004FDF77: fld dword ptr [esp+10h]
004FDF7B: fadd dword ptr [esp+18h]
004FDF7F: push 3F800000h
004FDF84: push ecx
004FDF85: mov ecx,esi
004FDF87: fstp dword ptr [esp]
004FDF8A: push 10h
004FDF8C: push 13h
004FDF8E: push 1
004FDF90: call 004F93C0
004FDF95: lea ecx,[esp+4Ch]
004FDF99: mov dword ptr [esp+0000009Ch],0FFFFFFFFh
004FDFA4: call 0059ACD0
004FDFA9: pop edi
004FDFAA: pop ebx
004FDFAB: mov ecx,dword ptr [esp+0000008Ch]
 00
004FDFB2: pop esi
004FDFB3: pop ebp
004FDFB4: mov dword ptr fs:[00000000h],ecx
 00
004FDFBB: add esp,90h
004FDFC1: ret 4
004FDFD0: push 0FFFFFFFFh
004FDFD2: push 83C408h
004FDFD7: mov eax,dword ptr fs:[00000000h]
004FDFDD: push eax
004FDFDE: mov dword ptr fs:[00000000h],esp
 00
004FDFE5: sub esp,54h
004FDFE8: push ebp
004FDFE9: push esi
004FDFEA: xor ebp,ebp
004FDFEC: push edi
004FDFED: mov esi,ecx
004FDFEF: mov dword ptr [esp+58h],ebp
004FDFF3: mov dword ptr [esp+5Ch],ebp
004FDFF7: mov ax,word ptr [esi+000000DCh]
 00
004FDFFE: or edi,0FFFFFFFFh
004FE001: cmp ax,di
004FE004: mov dword ptr [esp+68h],ebp
004FE008: je 004FE09D
004FE00E: cmp word ptr [esi+000000E0h],di
 00
004FE015: jne 004FE02C
004FE017: push eax
004FE018: call 004F4D10
004FE01D: add esp,4
004FE020: cmp ax,di
004FE023: mov word ptr [esi+000000E0h],ax
 00
004FE02A: je 004FE09D
004FE02C: xor eax,eax
004FE02E: mov ax,word ptr [esi+000000E0h]
 00
004FE035: xor ecx,ecx
004FE037: mov cx,word ptr [esi+000000DCh]
 00
004FE03E: push eax
004FE03F: push ecx
004FE040: mov ecx,0B5F8B8h
004FE045: call 004D88C0
004FE04A: test al,al
004FE04C: jne 004FE0BE
004FE04E: mov ax,word ptr [esi+000000E0h]
 00
004FE055: sub ax,7
004FE059: cmp ax,bp
004FE05C: jl 004FE07D
004FE05E: cmp ax,0Ah
004FE062: jge 004FE07D
004FE064: movsx edx,ax
004FE067: mov ax,word ptr [edx*4+00B6B9A0h]
004FE06F: cmp ax,word ptr [esi+000000DCh]
 00
004FE076: sete al
004FE079: test al,al
004FE07B: jne 004FE09D
004FE07D: xor ecx,ecx
004FE07F: mov cx,word ptr [esi+000000DCh]
 00
004FE086: push ecx
004FE087: call 004F4D10
004FE08C: add esp,4
004FE08F: mov word ptr [esi+000000E0h],ax
 00
004FE096: mov byte ptr [esi+000000A9h],0
 00
004FE09D: lea ecx,[esp+18h]
004FE0A1: mov dword ptr [esp+68h],edi
004FE0A5: call 0059ACD0
004FE0AA: pop edi
004FE0AB: pop esi
004FE0AC: pop ebp
004FE0AD: mov ecx,dword ptr [esp+54h]
004FE0B1: mov dword ptr fs:[00000000h],ecx
 00
004FE0B8: add esp,60h
004FE0BB: ret 4
004FE0BE: push 13h
004FE0C0: push 8Ah
004FE0C5: mov ecx,0B5F8B8h
004FE0CA: call 004D88C0
004FE0CF: test al,al
004FE0D1: je 004FE09D
004FE0D3: mov edx,dword ptr [esp+70h]
004FE0D7: mov edi,dword ptr [edx+10h]
004FE0DA: mov cl,byte ptr [edi+00000424h]
004FE0E0: mov al,byte ptr [edi+00000428h]
004FE0E6: test cl,cl
004FE0E8: push ebx
004FE0E9: setge bl
004FE0EC: test al,10h
004FE0EE: jne 004FE10A
004FE0F0: test bl,bl
004FE0F2: jne 004FE10A
004FE0F4: xor edi,edi
004FE0F6: push edi
004FE0F7: mov ecx,esi
004FE0F9: call 004F55C0
004FE0FE: inc edi
004FE0FF: cmp di,0Ch
004FE103: jl 004FE0F6
004FE105: jmp 004FE3F3
004FE10A: fld dword ptr [edi+0000049Ch]
004FE110: fmul dword ptr ds:[00859AACh]
004FE116: call 00821B40
004FE11B: fld dword ptr [edi+000004A0h]
004FE121: fmul dword ptr ds:[00859AACh]
004FE127: mov ebp,eax
004FE129: call 00821B40
004FE12E: cmp bp,ax
004FE131: jle 004FE135
004FE133: mov eax,ebp
004FE135: movsx eax,ax
004FE138: mov dword ptr [esp+10h],eax
004FE13C: fild dword ptr [esp+10h]
004FE140: fmul dword ptr ds:[00859A3Ch]
004FE146: fld dword ptr ds:[00858624h]
004FE14C: fcomp st(1)
004FE14E: fnstsw ax
004FE150: test ah,5
004FE153: jnp 004FE16E
004FE155: fld dword ptr ds:[00858B50h]
004FE15B: fcomp st(1)
004FE15D: fnstsw ax
004FE15F: test ah,41h
004FE162: jne 004FE176
004FE164: fstp st(0)
004FE166: fld dword ptr ds:[00858B50h]
004FE16C: jmp 004FE176
004FE16E: fstp st(0)
004FE170: fld dword ptr ds:[00858624h]
004FE176: test bl,bl
004FE178: jne 004FE18A
004FE17A: fstp st(0)
004FE17C: fld dword ptr [edi+000009C4h]
004FE182: fmul dword ptr ds:[00862D18h]
004FE188: jmp 004FE196
004FE18A: fmul dword ptr ds:[00858C24h]
004FE190: fadd dword ptr ds:[00858CB0h]
004FE196: fstp dword ptr [esp+10h]
004FE19A: fld dword ptr ds:[00858624h]
004FE1A0: fcomp dword ptr [esp+10h]
004FE1A4: fnstsw ax
004FE1A6: test ah,5
004FE1A9: jnp 004FE1C6
004FE1AB: fld dword ptr ds:[00858B50h]
004FE1B1: fcomp dword ptr [esp+10h]
004FE1B5: fnstsw ax
004FE1B7: test ah,41h
004FE1BA: jne 004FE1CE
004FE1BC: mov dword ptr [esp+10h],0
004FE1C4: jmp 004FE1CE
004FE1C6: mov dword ptr [esp+10h],3F800000h
004FE1CE: mov ecx,dword ptr [esp+10h]
004FE1D2: push 8CBE00h
004FE1D7: push 3
004FE1D9: push ecx
004FE1DA: call 004D9D90
004FE1DF: fstp dword ptr [esp+1Ch]
004FE1E3: mov edx,dword ptr [esp+00000080h]
 00
004FE1EA: mov eax,dword ptr [edx+10h]
004FE1ED: mov eax,dword ptr [eax+14h]
004FE1F0: fld dword ptr [eax+14h]
004FE1F3: add esp,0Ch
004FE1F6: fadd dword ptr [eax+10h]
004FE1F9: fmul dword ptr ds:[00858B50h]
004FE1FF: fadd dword ptr [eax+18h]
004FE202: fmul qword ptr ds:[00862D10h]
004FE208: fstp dword ptr [esp+14h]
004FE20C: fld dword ptr [eax+4]
004FE20F: fadd dword ptr [eax]
004FE211: fmul dword ptr ds:[00858B50h]
004FE217: fadd dword ptr [eax+8]
004FE21A: fst dword ptr [esp+74h]
004FE21E: fcomp dword ptr ds:[00858B50h]
004FE224: fnstsw ax
004FE226: test ah,5
004FE229: jp 004FE239
004FE22B: fld dword ptr [esp+74h]
004FE22F: fmul qword ptr ds:[00862D10h]
004FE235: fstp dword ptr [esp+74h]
004FE239: mov edi,dword ptr [esp+10h]
004FE23D: push edi
004FE23E: call 004D9E50
004FE243: fmul dword ptr ds:[00858BA4h]
004FE249: add esp,4
004FE24C: fadd dword ptr ds:[00859000h]
004FE252: fstp dword ptr [esp+18h]
004FE256: fld dword ptr [esp+74h]
004FE25A: fmul dword ptr ds:[00858B1Ch]
004FE260: fld dword ptr [esp+14h]
004FE264: fmul dword ptr ds:[00858FCCh]
004FE26A: faddp st(1),st
004FE26C: fadd dword ptr ds:[00858624h]
004FE272: fstp dword ptr [esp+74h]
004FE276: fld dword ptr [esi+0000022Ch]
004FE27C: fcomp dword ptr ds:[00858B50h]
004FE282: fnstsw ax
004FE284: test ah,5
004FE287: jp 004FE293
004FE289: mov ecx,dword ptr [esp+74h]
004FE28D: mov dword ptr [esi+0000022Ch],ecx
004FE293: fld dword ptr [esp+74h]
004FE297: fcomp dword ptr [esi+0000022Ch]
004FE29D: fld dword ptr [esi+0000022Ch]
004FE2A3: fnstsw ax
004FE2A5: test ah,1
004FE2A8: jne 004FE2C3
004FE2AA: fadd dword ptr ds:[00862D04h]
004FE2B0: fld dword ptr [esp+74h]
004FE2B4: fcomp st(1)
004FE2B6: fnstsw ax
004FE2B8: test ah,5
004FE2BB: jnp 004FE2DC
004FE2BD: fstp dword ptr [esp+74h]
004FE2C1: jmp 004FE2DE
004FE2C3: fsub dword ptr ds:[00862D04h]
004FE2C9: fld dword ptr [esp+74h]
004FE2CD: fcomp st(1)
004FE2CF: fnstsw ax
004FE2D1: test ah,41h
004FE2D4: je 004FE2DC
004FE2D6: fstp dword ptr [esp+74h]
004FE2DA: jmp 004FE2DE
004FE2DC: fstp st(0)
004FE2DE: mov edx,dword ptr [esp+74h]
004FE2E2: mov dword ptr [esi+0000022Ch],edx
004FE2E8: mov eax,dword ptr ds:[00B6F03Ch]
004FE2ED: test eax,eax
004FE2EF: lea edx,[eax+30h]
004FE2F2: jne 004FE2F9
004FE2F4: mov edx,0B6F02Ch
004FE2F9: mov eax,dword ptr [esi+4]
004FE2FC: mov ecx,dword ptr [eax+14h]
004FE2FF: test ecx,ecx
004FE301: je 004FE308
004FE303: lea eax,[ecx+30h]
004FE306: jmp 004FE30B
004FE308: add eax,4
004FE30B: fld dword ptr [eax+8]
004FE30E: fsub dword ptr [edx+8]
004FE311: fld dword ptr [eax+4]
004FE314: fsub dword ptr [edx+4]
004FE317: fld dword ptr [eax]
004FE319: fsub dword ptr [edx]
004FE31B: fld st(0)
004FE31D: fmul st,st(1)
004FE31F: fld st(2)
004FE321: fmul st,st(3)
004FE323: faddp st(1),st
004FE325: fld st(3)
004FE327: fmul st,st(4)
004FE329: faddp st(1),st
004FE32B: fstp st(3)
004FE32D: fstp st(0)
004FE32F: fstp st(0)
004FE331: fsqrt
004FE333: fst dword ptr [esp+10h]
004FE337: fcomp dword ptr ds:[00858BF4h]
004FE33D: fnstsw ax
004FE33F: test ah,5
004FE342: jp 004FE39C
004FE344: fld dword ptr [esp+10h]
004FE348: fcomp dword ptr ds:[00862D38h]
004FE34E: fnstsw ax
004FE350: test ah,5
004FE353: jp 004FE35F
004FE355: mov dword ptr [esp+10h],0C2C80000h
004FE35D: jmp 004FE3B5
004FE35F: fld dword ptr [esp+10h]
004FE363: fsub dword ptr ds:[00862D38h]
004FE369: fmul dword ptr ds:[0085A5C4h]
004FE36F: fstp dword ptr [esp+14h]
004FE373: mov eax,dword ptr [esp+14h]
004FE377: push eax
004FE378: call 004D9E50
004FE37D: fmul dword ptr ds:[00858BA4h]
004FE383: push edi
004FE384: fstp dword ptr [esp+1Ch]
004FE388: call 004D9E50
004FE38D: fmul dword ptr ds:[00858BA4h]
004FE393: add esp,8
004FE396: fadd dword ptr [esp+14h]
004FE39A: jmp 004FE3AB
004FE39C: push edi
004FE39D: call 004D9E50
004FE3A2: fmul dword ptr ds:[00858BA4h]
004FE3A8: add esp,4
004FE3AB: fadd dword ptr ds:[00859000h]
004FE3B1: fstp dword ptr [esp+10h]
004FE3B5: mov ecx,dword ptr [esp+74h]
004FE3B9: mov edx,dword ptr [esp+18h]
004FE3BD: push ecx
004FE3BE: xor eax,eax
004FE3C0: mov ax,word ptr [esi+000000E0h]
 00
004FE3C7: push edx
004FE3C8: push 1
004FE3CA: mov ecx,esi
004FE3CC: push eax
004FE3CD: push 2
004FE3CF: call 004F93C0
004FE3D4: mov ecx,dword ptr [esp+10h]
004FE3D8: xor edx,edx
004FE3DA: mov dx,word ptr [esi+000000E0h]
 00
004FE3E1: push 3F800000h
004FE3E6: push ecx
004FE3E7: push 0
004FE3E9: mov ecx,esi
004FE3EB: push edx
004FE3EC: push 1
004FE3EE: call 004F93C0
004FE3F3: lea ecx,[esp+1Ch]
004FE3F7: mov dword ptr [esp+6Ch],0FFFFFFFFh
004FE3FF: call 0059ACD0
004FE404: mov ecx,dword ptr [esp+64h]
004FE408: pop ebx
004FE409: pop edi
004FE40A: pop esi
004FE40B: pop ebp
004FE40C: mov dword ptr fs:[00000000h],ecx
 00
004FE413: add esp,60h
004FE416: ret 4
004FE420: push 0FFFFFFFFh
004FE422: push 83C428h
004FE427: mov eax,dword ptr fs:[00000000h]
004FE42D: push eax
004FE42E: mov dword ptr fs:[00000000h],esp
 00
004FE435: sub esp,80h
004FE43B: push esi
004FE43C: mov esi,ecx
004FE43E: mov dword ptr [esp+7Ch],0
004FE446: mov dword ptr [esp+00000080h],0
004FE451: xor eax,eax
004FE453: mov ax,word ptr [esi+000000DEh]
 00
004FE45A: push 28h
004FE45C: mov ecx,0B5F8B8h
004FE461: mov dword ptr [esp+00000090h],0
004FE46C: mov dword ptr [esp+8],0
004FE474: push eax
004FE475: call 004D88C0
004FE47A: test al,al
004FE47C: je 004FE493
004FE47E: push 13h
004FE480: push 8Ah
004FE485: mov ecx,0B5F8B8h
004FE48A: call 004D88C0
004FE48F: test al,al
004FE491: jne 004FE4AC
004FE493: lea ecx,[esp+3Ch]
004FE497: mov dword ptr [esp+0000008Ch],0FFFFFFFFh
004FE4A2: call 0059ACD0
004FE4A7: jmp 004FE921
004FE4AC: push ebx
004FE4AD: mov ebx,dword ptr [esp+00000098h]
 00
004FE4B4: mov eax,dword ptr [ebx+10h]
004FE4B7: fld dword ptr [eax+0000084Ch]
004FE4BD: push edi
004FE4BE: fmul dword ptr ds:[00862D1Ch]
004FE4C4: fstp dword ptr [esp+10h]
004FE4C8: fld dword ptr ds:[00858624h]
004FE4CE: fcomp dword ptr [esp+10h]
004FE4D2: fnstsw ax
004FE4D4: test ah,5
004FE4D7: jnp 004FE4F4
004FE4D9: fld dword ptr ds:[00858B50h]
004FE4DF: fcomp dword ptr [esp+10h]
004FE4E3: fnstsw ax
004FE4E5: test ah,41h
004FE4E8: jne 004FE4FC
004FE4EA: mov dword ptr [esp+10h],0
004FE4F2: jmp 004FE4FC
004FE4F4: mov dword ptr [esp+10h],3F800000h
004FE4FC: mov edi,dword ptr [esp+10h]
004FE500: push 8CBE40h
004FE505: push 5
004FE507: push edi
004FE508: call 004D9D90
004FE50D: fstp dword ptr [esp+20h]
004FE511: push 8CBE18h
004FE516: push 5
004FE518: push edi
004FE519: call 004D9D90
004FE51E: fstp dword ptr [esp+3Ch]
004FE522: add esp,18h
004FE525: push ebx
004FE526: mov ecx,esi
004FE528: call 004F5080
004FE52D: push 0B6F99Ch
004FE532: lea ecx,[esp+48h]
004FE536: call 0059BBC0
004FE53B: mov eax,dword ptr ds:[00B6F03Ch]
004FE540: test eax,eax
004FE542: lea ecx,[eax+30h]
004FE545: jne 004FE54C
004FE547: mov ecx,0B6F02Ch
004FE54C: mov eax,dword ptr [esi+4]
004FE54F: mov edx,dword ptr [eax+14h]
004FE552: test edx,edx
004FE554: je 004FE55B
004FE556: lea eax,[edx+30h]
004FE559: jmp 004FE55E
004FE55B: add eax,4
004FE55E: fld dword ptr [eax+8]
004FE561: fsub dword ptr [ecx+8]
004FE564: fld dword ptr [eax+4]
004FE567: fsub dword ptr [ecx+4]
004FE56A: fld dword ptr [eax]
004FE56C: fsub dword ptr [ecx]
004FE56E: fstp dword ptr [esp+38h]
004FE572: mov ecx,dword ptr [esp+38h]
004FE576: mov dword ptr [esp+2Ch],ecx
004FE57A: fstp dword ptr [esp+3Ch]
004FE57E: mov edx,dword ptr [esp+3Ch]
004FE582: lea ecx,[esp+2Ch]
004FE586: fstp dword ptr [esp+40h]
004FE58A: mov eax,dword ptr [esp+40h]
004FE58E: mov dword ptr [esp+30h],edx
004FE592: mov dword ptr [esp+34h],eax
004FE596: call 0059C910
004FE59B: fld dword ptr [esp+30h]
004FE59F: mov ecx,dword ptr [ebx+10h]
004FE5A2: mov eax,dword ptr [ecx+14h]
004FE5A5: fmul dword ptr [eax+14h]
004FE5A8: fld dword ptr [esp+34h]
004FE5AC: fmul dword ptr [eax+18h]
004FE5AF: faddp st(1),st
004FE5B1: fld dword ptr [esp+2Ch]
004FE5B5: fmul dword ptr [eax+10h]
004FE5B8: faddp st(1),st
004FE5BA: fld dword ptr [eax+24h]
004FE5BD: fadd dword ptr [eax+20h]
004FE5C0: fmul dword ptr ds:[00858B50h]
004FE5C6: fadd dword ptr [eax+28h]
004FE5C9: fsubr dword ptr ds:[00858624h]
004FE5CF: fmul dword ptr ds:[00858B8Ch]
004FE5D5: fstp dword ptr [esp+20h]
004FE5D9: fadd dword ptr ds:[00858624h]
004FE5DF: fmul dword ptr ds:[00858B8Ch]
004FE5E5: fstp dword ptr [esp+18h]
004FE5E9: fld dword ptr [esp+10h]
004FE5ED: fcomp dword ptr ds:[00858624h]
004FE5F3: fnstsw ax
004FE5F5: test ah,41h
004FE5F8: jne 004FE602
004FE5FA: mov dword ptr [esp+10h],3F800000h
004FE602: cmp word ptr [ebx+20h],0
004FE607: jle 004FE611
004FE609: mov dword ptr [esp+0Ch],3DCCCCCDh
004FE611: cmp word ptr [ebx+22h],0
004FE616: jle 004FE626
004FE618: fld dword ptr [esp+0Ch]
004FE61C: fsub dword ptr ds:[00858C28h]
004FE622: fstp dword ptr [esp+0Ch]
004FE626: fld dword ptr ds:[008CBD98h]
004FE62C: fmul dword ptr [esp+18h]
004FE630: fsubr dword ptr ds:[00858624h]
004FE636: fmul dword ptr [esp+14h]
004FE63A: fstp dword ptr [esp+1Ch]
004FE63E: mov edx,dword ptr [esp+1Ch]
004FE642: push edx
004FE643: call 004D9E50
004FE648: fmul dword ptr ds:[00858BA4h]
004FE64E: fadd dword ptr ds:[00859000h]
004FE654: fstp dword ptr [esp+2Ch]
004FE658: fld dword ptr [esp+1Ch]
004FE65C: fmul dword ptr ds:[00858B8Ch]
004FE662: fadd dword ptr ds:[00858B8Ch]
004FE668: fmul dword ptr [esp+18h]
004FE66C: fstp dword ptr [esp+20h]
004FE670: mov eax,dword ptr [esp+20h]
004FE674: push eax
004FE675: call 004D9E50
004FE67A: fmul dword ptr ds:[00858BA4h]
004FE680: mov edi,dword ptr [esp+1Ch]
004FE684: push edi
004FE685: fadd dword ptr ds:[00859000h]
004FE68B: fstp dword ptr [esp+28h]
004FE68F: call 004D9E50
004FE694: fmul dword ptr ds:[00858BA4h]
004FE69A: mov ecx,dword ptr [ebx+10h]
004FE69D: add esp,0Ch
004FE6A0: fadd dword ptr ds:[00858CA0h]
004FE6A6: fstp dword ptr [esp+18h]
004FE6AA: fld dword ptr [esp+20h]
004FE6AE: fmul dword ptr ds:[00858CC4h]
004FE6B4: fadd dword ptr ds:[00858624h]
004FE6BA: fld dword ptr [esp+0Ch]
004FE6BE: fadd dword ptr ds:[00858624h]
004FE6C4: fmulp st(1),st
004FE6C6: fmul dword ptr [esp+24h]
004FE6CA: fstp dword ptr [esp+0Ch]
004FE6CE: fld dword ptr [ecx+000004C0h]
004FE6D4: fcomp dword ptr ds:[00862D28h]
004FE6DA: fnstsw ax
004FE6DC: test ah,5
004FE6DF: jp 004FE70E
004FE6E1: mov edx,dword ptr ds:[008CBDFCh]
004FE6E7: fld dword ptr ds:[008CBDFCh]
004FE6ED: fmul dword ptr ds:[00858C1Ch]
004FE6F3: push edx
004FE6F4: push ecx
004FE6F5: fstp dword ptr [esp]
004FE6F8: call 004D9C50
004FE6FD: fadd dword ptr ds:[008CBDF8h]
004FE703: add esp,8
004FE706: fmul dword ptr [esp+0Ch]
004FE70A: fstp dword ptr [esp+0Ch]
004FE70E: fld dword ptr [esi+0000022Ch]
004FE714: fcomp dword ptr ds:[00858B50h]
004FE71A: fnstsw ax
004FE71C: test ah,5
004FE71F: jp 004FE72B
004FE721: mov eax,dword ptr [esp+0Ch]
004FE725: mov dword ptr [esi+0000022Ch],eax
004FE72B: fld dword ptr [esp+0Ch]
004FE72F: fcomp dword ptr [esi+0000022Ch]
004FE735: fld dword ptr [esp+10h]
004FE739: fnstsw ax
004FE73B: fcomp qword ptr ds:[00862D48h]
004FE741: test ah,1
004FE744: fnstsw ax
004FE746: jne 004FE791
004FE748: test ah,5
004FE74B: jp 004FE772
004FE74D: fld dword ptr [esi+0000022Ch]
004FE753: fadd qword ptr ds:[00862D40h]
004FE759: fld dword ptr [esp+0Ch]
004FE75D: fcomp st(1)
004FE75F: fnstsw ax
004FE761: test ah,5
004FE764: jp 004FE76C
004FE766: fstp st(0)
004FE768: fld dword ptr [esp+0Ch]
004FE76C: fstp dword ptr [esp+0Ch]
004FE770: jmp 004FE7DC
004FE772: fld dword ptr [esi+0000022Ch]
004FE778: fadd dword ptr ds:[00862D04h]
004FE77E: fld dword ptr [esp+0Ch]
004FE782: fcomp st(1)
004FE784: fnstsw ax
004FE786: test ah,5
004FE789: jnp 004FE7DA
004FE78B: fstp dword ptr [esp+0Ch]
004FE78F: jmp 004FE7DC
004FE791: test ah,5
004FE794: jp 004FE7BB
004FE796: fld dword ptr [esi+0000022Ch]
004FE79C: fsub qword ptr ds:[00862D40h]
004FE7A2: fld dword ptr [esp+0Ch]
004FE7A6: fcomp st(1)
004FE7A8: fnstsw ax
004FE7AA: test ah,41h
004FE7AD: jne 004FE76C
004FE7AF: fstp st(0)
004FE7B1: fld dword ptr [esp+0Ch]
004FE7B5: fstp dword ptr [esp+0Ch]
004FE7B9: jmp 004FE7DC
004FE7BB: fld dword ptr [esi+0000022Ch]
004FE7C1: fsub dword ptr ds:[00862D04h]
004FE7C7: fld dword ptr [esp+0Ch]
004FE7CB: fcomp st(1)
004FE7CD: fnstsw ax
004FE7CF: test ah,41h
004FE7D2: je 004FE7DA
004FE7D4: fstp dword ptr [esp+0Ch]
004FE7D8: jmp 004FE7DC
004FE7DA: fstp st(0)
004FE7DC: mov ecx,dword ptr [esp+0Ch]
004FE7E0: mov dword ptr [esi+0000022Ch],ecx
004FE7E6: mov eax,dword ptr ds:[00B6F03Ch]
004FE7EB: test eax,eax
004FE7ED: lea edx,[eax+30h]
004FE7F0: jne 004FE7F7
004FE7F2: mov edx,0B6F02Ch
004FE7F7: mov eax,dword ptr [esi+4]
004FE7FA: mov ecx,dword ptr [eax+14h]
004FE7FD: test ecx,ecx
004FE7FF: je 004FE806
004FE801: lea eax,[ecx+30h]
004FE804: jmp 004FE809
004FE806: add eax,4
004FE809: fld dword ptr [eax+8]
004FE80C: fsub dword ptr [edx+8]
004FE80F: fld dword ptr [eax+4]
004FE812: fsub dword ptr [edx+4]
004FE815: fld dword ptr [eax]
004FE817: fsub dword ptr [edx]
004FE819: fld st(0)
004FE81B: fmul st,st(1)
004FE81D: fld st(2)
004FE81F: fmul st,st(3)
004FE821: faddp st(1),st
004FE823: fld st(3)
004FE825: fmul st,st(4)
004FE827: faddp st(1),st
004FE829: fstp st(3)
004FE82B: fstp st(0)
004FE82D: fstp st(0)
004FE82F: fsqrt
004FE831: fst dword ptr [esp+14h]
004FE835: fcomp dword ptr ds:[00858BF4h]
004FE83B: fnstsw ax
004FE83D: test ah,5
004FE840: jp 004FE89A
004FE842: fld dword ptr [esp+14h]
004FE846: fcomp dword ptr ds:[00862D38h]
004FE84C: fnstsw ax
004FE84E: test ah,5
004FE851: jp 004FE85D
004FE853: mov dword ptr [esp+14h],0C2C80000h
004FE85B: jmp 004FE8B3
004FE85D: fld dword ptr [esp+14h]
004FE861: fsub dword ptr ds:[00862D38h]
004FE867: fmul dword ptr ds:[0085A5C4h]
004FE86D: fstp dword ptr [esp+24h]
004FE871: mov edx,dword ptr [esp+24h]
004FE875: push edx
004FE876: call 004D9E50
004FE87B: fmul dword ptr ds:[00858BA4h]
004FE881: push edi
004FE882: fstp dword ptr [esp+2Ch]
004FE886: call 004D9E50
004FE88B: fmul dword ptr ds:[00858BA4h]
004FE891: add esp,8
004FE894: fadd dword ptr [esp+24h]
004FE898: jmp 004FE8A9
004FE89A: push edi
004FE89B: call 004D9E50
004FE8A0: fmul dword ptr ds:[00858BA4h]
004FE8A6: add esp,4
004FE8A9: fadd dword ptr ds:[00859000h]
004FE8AF: fstp dword ptr [esp+14h]
004FE8B3: mov edi,dword ptr [esp+0Ch]
004FE8B7: mov eax,dword ptr [esp+28h]
004FE8BB: push edi
004FE8BC: push eax
004FE8BD: push 0
004FE8BF: push 28h
004FE8C1: push 2
004FE8C3: mov ecx,esi
004FE8C5: call 004F93C0
004FE8CA: mov ecx,dword ptr [esp+1Ch]
004FE8CE: push edi
004FE8CF: push ecx
004FE8D0: push 1
004FE8D2: push 28h
004FE8D4: push 4
004FE8D6: mov ecx,esi
004FE8D8: call 004F93C0
004FE8DD: mov edx,dword ptr [esp+18h]
004FE8E1: push 3F800000h
004FE8E6: push edx
004FE8E7: push 3
004FE8E9: push 28h
004FE8EB: push 3
004FE8ED: mov ecx,esi
004FE8EF: call 004F93C0
004FE8F4: mov eax,dword ptr [esp+14h]
004FE8F8: push 3F800000h
004FE8FD: push eax
004FE8FE: push 3
004FE900: push 13h
004FE902: push 1
004FE904: mov ecx,esi
004FE906: call 004F93C0
004FE90B: lea ecx,[esp+44h]
004FE90F: mov dword ptr [esp+00000094h],0FFFFFFFFh
004FE91A: call 0059ACD0
004FE91F: pop edi
004FE920: pop ebx
004FE921: mov ecx,dword ptr [esp+00000084h]
 00
004FE928: pop esi
004FE929: mov dword ptr fs:[00000000h],ecx
 00
004FE930: add esp,8Ch
004FE936: ret 4
004FE940: push 0FFFFFFFFh
004FE942: push 83C448h
004FE947: mov eax,dword ptr fs:[00000000h]
004FE94D: push eax
004FE94E: mov dword ptr fs:[00000000h],esp
 00
004FE955: sub esp,7Ch
004FE958: push esi
004FE959: mov esi,ecx
004FE95B: mov dword ptr [esp+78h],0
004FE963: mov dword ptr [esp+7Ch],0
004FE96B: xor eax,eax
004FE96D: mov ax,word ptr [esi+000000DEh]
 00
004FE974: push 28h
004FE976: mov ecx,0B5F8B8h
004FE97B: mov dword ptr [esp+0000008Ch],0
004FE986: push eax
004FE987: call 004D88C0
004FE98C: test al,al
004FE98E: je 004FE9A5
004FE990: push 13h
004FE992: push 8Ah
004FE997: mov ecx,0B5F8B8h
004FE99C: call 004D88C0
004FE9A1: test al,al
004FE9A3: jne 004FE9BE
004FE9A5: lea ecx,[esp+38h]
004FE9A9: mov dword ptr [esp+00000088h],0FFFFFFFFh
004FE9B4: call 0059ACD0
004FE9B9: jmp 004FEDFB
004FE9BE: push ebx
004FE9BF: mov ebx,dword ptr [esp+00000094h]
 00
004FE9C6: mov eax,dword ptr [ebx+10h]
004FE9C9: fld dword ptr [eax+0000084Ch]
004FE9CF: push edi
004FE9D0: fmul dword ptr ds:[00862D1Ch]
004FE9D6: fstp dword ptr [esp+10h]
004FE9DA: fld dword ptr ds:[00858624h]
004FE9E0: fcomp dword ptr [esp+10h]
004FE9E4: fnstsw ax
004FE9E6: test ah,5
004FE9E9: jnp 004FEA06
004FE9EB: fld dword ptr ds:[00858B50h]
004FE9F1: fcomp dword ptr [esp+10h]
004FE9F5: fnstsw ax
004FE9F7: test ah,41h
004FE9FA: jne 004FEA0E
004FE9FC: mov dword ptr [esp+10h],0
004FEA04: jmp 004FEA0E
004FEA06: mov dword ptr [esp+10h],3F800000h
004FEA0E: mov edi,dword ptr [esp+10h]
004FEA12: push 8CBE40h
004FEA17: push 5
004FEA19: push edi
004FEA1A: call 004D9D90
004FEA1F: fstp dword ptr [esp+18h]
004FEA23: push 8CBE18h
004FEA28: push 5
004FEA2A: push edi
004FEA2B: call 004D9D90
004FEA30: fstp dword ptr [esp+38h]
004FEA34: add esp,18h
004FEA37: push 0B6F99Ch
004FEA3C: lea ecx,[esp+44h]
004FEA40: call 0059BBC0
004FEA45: mov eax,dword ptr ds:[00B6F03Ch]
004FEA4A: test eax,eax
004FEA4C: lea ecx,[eax+30h]
004FEA4F: jne 004FEA56
004FEA51: mov ecx,0B6F02Ch
004FEA56: mov eax,dword ptr [esi+4]
004FEA59: mov edx,dword ptr [eax+14h]
004FEA5C: test edx,edx
004FEA5E: je 004FEA65
004FEA60: lea eax,[edx+30h]
004FEA63: jmp 004FEA68
004FEA65: add eax,4
004FEA68: fld dword ptr [eax+8]
004FEA6B: fsub dword ptr [ecx+8]
004FEA6E: fld dword ptr [eax+4]
004FEA71: fsub dword ptr [ecx+4]
004FEA74: fld dword ptr [eax]
004FEA76: fsub dword ptr [ecx]
004FEA78: fstp dword ptr [esp+34h]
004FEA7C: mov ecx,dword ptr [esp+34h]
004FEA80: mov dword ptr [esp+28h],ecx
004FEA84: fstp dword ptr [esp+38h]
004FEA88: mov edx,dword ptr [esp+38h]
004FEA8C: lea ecx,[esp+28h]
004FEA90: fstp dword ptr [esp+3Ch]
004FEA94: mov eax,dword ptr [esp+3Ch]
004FEA98: mov dword ptr [esp+2Ch],edx
004FEA9C: mov dword ptr [esp+30h],eax
004FEAA0: call 0059C910
004FEAA5: fld dword ptr [esp+2Ch]
004FEAA9: mov ecx,dword ptr [ebx+10h]
004FEAAC: mov eax,dword ptr [ecx+14h]
004FEAAF: fmul dword ptr [eax+14h]
004FEAB2: fld dword ptr [esp+30h]
004FEAB6: fmul dword ptr [eax+18h]
004FEAB9: faddp st(1),st
004FEABB: fld dword ptr [esp+28h]
004FEABF: fmul dword ptr [eax+10h]
004FEAC2: faddp st(1),st
004FEAC4: fld dword ptr [eax+24h]
004FEAC7: fadd dword ptr [eax+20h]
004FEACA: fmul dword ptr ds:[00858B50h]
004FEAD0: fadd dword ptr [eax+28h]
004FEAD3: fsubr dword ptr ds:[00858624h]
004FEAD9: fmul dword ptr ds:[00858B8Ch]
004FEADF: fstp dword ptr [esp+1Ch]
004FEAE3: fadd dword ptr ds:[00858624h]
004FEAE9: fmul dword ptr ds:[00858B8Ch]
004FEAEF: fstp dword ptr [esp+14h]
004FEAF3: fld dword ptr [esp+10h]
004FEAF7: fcomp dword ptr ds:[00858624h]
004FEAFD: fnstsw ax
004FEAFF: test ah,41h
004FEB02: jne 004FEB0C
004FEB04: mov dword ptr [esp+10h],3F800000h
004FEB0C: fld dword ptr ds:[008CBD98h]
004FEB12: fmul dword ptr [esp+14h]
004FEB16: fsubr dword ptr ds:[00858624h]
004FEB1C: fmul dword ptr [esp+0Ch]
004FEB20: fstp dword ptr [esp+18h]
004FEB24: mov edx,dword ptr [esp+18h]
004FEB28: push edx
004FEB29: call 004D9E50
004FEB2E: fmul dword ptr ds:[00858BA4h]
004FEB34: fadd dword ptr ds:[00859000h]
004FEB3A: fstp dword ptr [esp+28h]
004FEB3E: fld dword ptr [esp+18h]
004FEB42: fmul dword ptr ds:[00858B8Ch]
004FEB48: fadd dword ptr ds:[00858B8Ch]
004FEB4E: fmul dword ptr [esp+10h]
004FEB52: fstp dword ptr [esp+1Ch]
004FEB56: mov eax,dword ptr [esp+1Ch]
004FEB5A: push eax
004FEB5B: call 004D9E50
004FEB60: fmul dword ptr ds:[00858BA4h]
004FEB66: mov edi,dword ptr [esp+14h]
004FEB6A: push edi
004FEB6B: fadd dword ptr ds:[00859000h]
004FEB71: fstp dword ptr [esp+24h]
004FEB75: call 004D9E50
004FEB7A: fmul dword ptr ds:[00858BA4h]
004FEB80: mov ecx,dword ptr [ebx+10h]
004FEB83: add esp,0Ch
004FEB86: fadd dword ptr ds:[00858CA0h]
004FEB8C: fstp dword ptr [esp+14h]
004FEB90: fld dword ptr [esp+1Ch]
004FEB94: fmul dword ptr ds:[00858CC4h]
004FEB9A: fadd dword ptr ds:[00858624h]
004FEBA0: fmul dword ptr [esp+20h]
004FEBA4: fstp dword ptr [esp+0Ch]
004FEBA8: fld dword ptr [ecx+000004C0h]
004FEBAE: fcomp dword ptr ds:[00862D28h]
004FEBB4: fnstsw ax
004FEBB6: test ah,5
004FEBB9: jp 004FEBE8
004FEBBB: mov edx,dword ptr ds:[008CBDFCh]
004FEBC1: fld dword ptr ds:[008CBDFCh]
004FEBC7: fmul dword ptr ds:[00858C1Ch]
004FEBCD: push edx
004FEBCE: push ecx
004FEBCF: fstp dword ptr [esp]
004FEBD2: call 004D9C50
004FEBD7: fadd dword ptr ds:[008CBDF8h]
004FEBDD: add esp,8
004FEBE0: fmul dword ptr [esp+0Ch]
004FEBE4: fstp dword ptr [esp+0Ch]
004FEBE8: fld dword ptr [esi+0000022Ch]
004FEBEE: fcomp dword ptr ds:[00858B50h]
004FEBF4: fnstsw ax
004FEBF6: test ah,5
004FEBF9: jp 004FEC05
004FEBFB: mov eax,dword ptr [esp+0Ch]
004FEBFF: mov dword ptr [esi+0000022Ch],eax
004FEC05: fld dword ptr [esp+0Ch]
004FEC09: fcomp dword ptr [esi+0000022Ch]
004FEC0F: fld dword ptr [esp+10h]
004FEC13: fnstsw ax
004FEC15: fcomp qword ptr ds:[00862D48h]
004FEC1B: test ah,1
004FEC1E: fnstsw ax
004FEC20: jne 004FEC6B
004FEC22: test ah,5
004FEC25: jp 004FEC4C
004FEC27: fld dword ptr [esi+0000022Ch]
004FEC2D: fadd qword ptr ds:[00862D40h]
004FEC33: fld dword ptr [esp+0Ch]
004FEC37: fcomp st(1)
004FEC39: fnstsw ax
004FEC3B: test ah,5
004FEC3E: jp 004FEC46
004FEC40: fstp st(0)
004FEC42: fld dword ptr [esp+0Ch]
004FEC46: fstp dword ptr [esp+0Ch]
004FEC4A: jmp 004FECB6
004FEC4C: fld dword ptr [esi+0000022Ch]
004FEC52: fadd dword ptr ds:[00862D04h]
004FEC58: fld dword ptr [esp+0Ch]
004FEC5C: fcomp st(1)
004FEC5E: fnstsw ax
004FEC60: test ah,5
004FEC63: jnp 004FECB4
004FEC65: fstp dword ptr [esp+0Ch]
004FEC69: jmp 004FECB6
004FEC6B: test ah,5
004FEC6E: jp 004FEC95
004FEC70: fld dword ptr [esi+0000022Ch]
004FEC76: fsub qword ptr ds:[00862D40h]
004FEC7C: fld dword ptr [esp+0Ch]
004FEC80: fcomp st(1)
004FEC82: fnstsw ax
004FEC84: test ah,41h
004FEC87: jne 004FEC46
004FEC89: fstp st(0)
004FEC8B: fld dword ptr [esp+0Ch]
004FEC8F: fstp dword ptr [esp+0Ch]
004FEC93: jmp 004FECB6
004FEC95: fld dword ptr [esi+0000022Ch]
004FEC9B: fsub dword ptr ds:[00862D04h]
004FECA1: fld dword ptr [esp+0Ch]
004FECA5: fcomp st(1)
004FECA7: fnstsw ax
004FECA9: test ah,41h
004FECAC: je 004FECB4
004FECAE: fstp dword ptr [esp+0Ch]
004FECB2: jmp 004FECB6
004FECB4: fstp st(0)
004FECB6: mov ecx,dword ptr [esp+0Ch]
004FECBA: mov dword ptr [esi+0000022Ch],ecx
004FECC0: mov eax,dword ptr ds:[00B6F03Ch]
004FECC5: test eax,eax
004FECC7: lea edx,[eax+30h]
004FECCA: jne 004FECD1
004FECCC: mov edx,0B6F02Ch
004FECD1: mov eax,dword ptr [esi+4]
004FECD4: mov ecx,dword ptr [eax+14h]
004FECD7: test ecx,ecx
004FECD9: je 004FECE0
004FECDB: lea eax,[ecx+30h]
004FECDE: jmp 004FECE3
004FECE0: add eax,4
004FECE3: fld dword ptr [eax+8]
004FECE6: fsub dword ptr [edx+8]
004FECE9: fld dword ptr [eax+4]
004FECEC: fsub dword ptr [edx+4]
004FECEF: fld dword ptr [eax]
004FECF1: fsub dword ptr [edx]
004FECF3: fld st(0)
004FECF5: fmul st,st(1)
004FECF7: fld st(2)
004FECF9: fmul st,st(3)
004FECFB: faddp st(1),st
004FECFD: fld st(3)
004FECFF: fmul st,st(4)
004FED01: faddp st(1),st
004FED03: fstp st(3)
004FED05: fstp st(0)
004FED07: fstp st(0)
004FED09: fsqrt
004FED0B: fst dword ptr [esp+10h]
004FED0F: fcomp dword ptr ds:[00858BF4h]
004FED15: fnstsw ax
004FED17: test ah,5
004FED1A: jp 004FED74
004FED1C: fld dword ptr [esp+10h]
004FED20: fcomp dword ptr ds:[00862D38h]
004FED26: fnstsw ax
004FED28: test ah,5
004FED2B: jp 004FED37
004FED2D: mov dword ptr [esp+10h],0C2C80000h
004FED35: jmp 004FED8D
004FED37: fld dword ptr [esp+10h]
004FED3B: fsub dword ptr ds:[00862D38h]
004FED41: fmul dword ptr ds:[0085A5C4h]
004FED47: fstp dword ptr [esp+20h]
004FED4B: mov edx,dword ptr [esp+20h]
004FED4F: push edx
004FED50: call 004D9E50
004FED55: fmul dword ptr ds:[00858BA4h]
004FED5B: push edi
004FED5C: fstp dword ptr [esp+28h]
004FED60: call 004D9E50
004FED65: fmul dword ptr ds:[00858BA4h]
004FED6B: add esp,8
004FED6E: fadd dword ptr [esp+20h]
004FED72: jmp 004FED83
004FED74: push edi
004FED75: call 004D9E50
004FED7A: fmul dword ptr ds:[00858BA4h]
004FED80: add esp,4
004FED83: fadd dword ptr ds:[00859000h]
004FED89: fstp dword ptr [esp+10h]
004FED8D: mov edi,dword ptr [esp+0Ch]
004FED91: mov eax,dword ptr [esp+24h]
004FED95: push edi
004FED96: push eax
004FED97: push 0
004FED99: push 28h
004FED9B: push 2
004FED9D: mov ecx,esi
004FED9F: call 004F93C0
004FEDA4: mov ecx,dword ptr [esp+18h]
004FEDA8: push edi
004FEDA9: push ecx
004FEDAA: push 1
004FEDAC: push 28h
004FEDAE: push 4
004FEDB0: mov ecx,esi
004FEDB2: call 004F93C0
004FEDB7: mov edx,dword ptr [esp+14h]
004FEDBB: push 3F800000h
004FEDC0: push edx
004FEDC1: push 3
004FEDC3: push 28h
004FEDC5: push 3
004FEDC7: mov ecx,esi
004FEDC9: call 004F93C0
004FEDCE: mov eax,dword ptr [esp+10h]
004FEDD2: push 3F800000h
004FEDD7: push eax
004FEDD8: push 3
004FEDDA: push 13h
004FEDDC: push 1
004FEDDE: mov ecx,esi
004FEDE0: call 004F93C0
004FEDE5: lea ecx,[esp+40h]
004FEDE9: mov dword ptr [esp+00000090h],0FFFFFFFFh
004FEDF4: call 0059ACD0
004FEDF9: pop edi
004FEDFA: pop ebx
004FEDFB: mov ecx,dword ptr [esp+00000080h]
 00
004FEE02: pop esi
004FEE03: mov dword ptr fs:[00000000h],ecx
 00
004FEE0A: add esp,88h
004FEE10: ret 4
004FEE20: push 0FFFFFFFFh
004FEE22: push 83C468h
004FEE27: mov eax,dword ptr fs:[00000000h]
004FEE2D: push eax
004FEE2E: mov dword ptr fs:[00000000h],esp
 00
004FEE35: sub esp,7Ch
004FEE38: push esi
004FEE39: mov esi,ecx
004FEE3B: mov dword ptr [esp+78h],0
004FEE43: mov dword ptr [esp+7Ch],0
004FEE4B: cmp word ptr [esi+000000DEh],0Ch
004FEE53: mov dword ptr [esp+00000088h],0
004FEE5E: jne 004FEE72
004FEE60: push 12h
004FEE62: push 0Dh
004FEE64: mov ecx,0B5F8B8h
004FEE69: call 004D88C0
004FEE6E: test al,al
004FEE70: je 004FEE87
004FEE72: push 13h
004FEE74: push 8Ah
004FEE79: mov ecx,0B5F8B8h
004FEE7E: call 004D88C0
004FEE83: test al,al
004FEE85: jne 004FEEA0
004FEE87: lea ecx,[esp+38h]
004FEE8B: mov dword ptr [esp+00000088h],0FFFFFFFFh
004FEE96: call 0059ACD0
004FEE9B: jmp 004FF303
004FEEA0: push ebx
004FEEA1: push edi
004FEEA2: mov edi,dword ptr [esp+00000098h]
 00
004FEEA9: mov eax,dword ptr [edi+10h]
004FEEAC: fld dword ptr [eax+0000084Ch]
004FEEB2: fmul dword ptr ds:[00862D1Ch]
004FEEB8: fstp dword ptr [esp+10h]
004FEEBC: fld dword ptr ds:[00858624h]
004FEEC2: fcomp dword ptr [esp+10h]
004FEEC6: fnstsw ax
004FEEC8: test ah,5
004FEECB: jnp 004FEEE8
004FEECD: fld dword ptr ds:[00858B50h]
004FEED3: fcomp dword ptr [esp+10h]
004FEED7: fnstsw ax
004FEED9: test ah,41h
004FEEDC: jne 004FEEF0
004FEEDE: mov dword ptr [esp+10h],0
004FEEE6: jmp 004FEEF0
004FEEE8: mov dword ptr [esp+10h],3F800000h
004FEEF0: mov ebx,dword ptr [esp+10h]
004FEEF4: push 8CBE40h
004FEEF9: push 5
004FEEFB: push ebx
004FEEFC: call 004D9D90
004FEF01: fstp dword ptr [esp+1Ch]
004FEF05: push 8CBE18h
004FEF0A: push 5
004FEF0C: push ebx
004FEF0D: call 004D9D90
004FEF12: fstp dword ptr [esp+34h]
004FEF16: add esp,18h
004FEF19: push 0B6F99Ch
004FEF1E: lea ecx,[esp+44h]
004FEF22: call 0059BBC0
004FEF27: mov eax,dword ptr ds:[00B6F03Ch]
004FEF2C: test eax,eax
004FEF2E: lea ecx,[eax+30h]
004FEF31: jne 004FEF38
004FEF33: mov ecx,0B6F02Ch
004FEF38: mov eax,dword ptr [esi+4]
004FEF3B: mov edx,dword ptr [eax+14h]
004FEF3E: test edx,edx
004FEF40: je 004FEF47
004FEF42: lea eax,[edx+30h]
004FEF45: jmp 004FEF4A
004FEF47: add eax,4
004FEF4A: fld dword ptr [eax+8]
004FEF4D: fsub dword ptr [ecx+8]
004FEF50: fld dword ptr [eax+4]
004FEF53: fsub dword ptr [ecx+4]
004FEF56: fld dword ptr [eax]
004FEF58: fsub dword ptr [ecx]
004FEF5A: fstp dword ptr [esp+34h]
004FEF5E: mov eax,dword ptr [esp+34h]
004FEF62: mov dword ptr [esp+28h],eax
004FEF66: fstp dword ptr [esp+38h]
004FEF6A: mov ecx,dword ptr [esp+38h]
004FEF6E: mov dword ptr [esp+2Ch],ecx
004FEF72: fstp dword ptr [esp+3Ch]
004FEF76: mov edx,dword ptr [esp+3Ch]
004FEF7A: lea ecx,[esp+28h]
004FEF7E: mov dword ptr [esp+30h],edx
004FEF82: call 0059C910
004FEF87: fld dword ptr [esp+2Ch]
004FEF8B: mov eax,dword ptr [edi+10h]
004FEF8E: mov eax,dword ptr [eax+14h]
004FEF91: fmul dword ptr [eax+14h]
004FEF94: fld dword ptr [esp+30h]
004FEF98: fmul dword ptr [eax+18h]
004FEF9B: faddp st(1),st
004FEF9D: fld dword ptr [esp+28h]
004FEFA1: fmul dword ptr [eax+10h]
004FEFA4: faddp st(1),st
004FEFA6: fld dword ptr [eax+24h]
004FEFA9: fadd dword ptr [eax+20h]
004FEFAC: fmul dword ptr ds:[00858B50h]
004FEFB2: fadd dword ptr [eax+28h]
004FEFB5: fsubr dword ptr ds:[00858624h]
004FEFBB: fmul dword ptr ds:[00858B8Ch]
004FEFC1: fstp dword ptr [esp+18h]
004FEFC5: fadd dword ptr ds:[00858624h]
004FEFCB: fmul dword ptr ds:[00858B8Ch]
004FEFD1: fstp dword ptr [esp+0Ch]
004FEFD5: fld dword ptr ds:[00858624h]
004FEFDB: fsub dword ptr [esp+0Ch]
004FEFDF: fmul dword ptr [esp+10h]
004FEFE3: fstp dword ptr [esp+14h]
004FEFE7: mov ecx,dword ptr [esp+14h]
004FEFEB: push ecx
004FEFEC: call 004D9E50
004FEFF1: fmul dword ptr ds:[00858BA4h]
004FEFF7: fadd dword ptr ds:[00859000h]
004FEFFD: fstp dword ptr [esp+24h]
004FF001: fld dword ptr [esp+10h]
004FF005: fmul dword ptr ds:[00858B8Ch]
004FF00B: fadd dword ptr ds:[00858B8Ch]
004FF011: fmul dword ptr [esp+14h]
004FF015: fstp dword ptr [esp+18h]
004FF019: mov edx,dword ptr [esp+18h]
004FF01D: push edx
004FF01E: call 004D9E50
004FF023: fmul dword ptr ds:[00858BA4h]
004FF029: mov ebx,dword ptr [esp+18h]
004FF02D: push ebx
004FF02E: fadd dword ptr ds:[00859000h]
004FF034: fstp dword ptr [esp+20h]
004FF038: call 004D9E50
004FF03D: fmul dword ptr ds:[00858BA4h]
004FF043: add esp,0Ch
004FF046: fadd dword ptr ds:[00858CA0h]
004FF04C: fstp dword ptr [esp+24h]
004FF050: fld dword ptr [esp+18h]
004FF054: fmul dword ptr ds:[00858CC4h]
004FF05A: fadd dword ptr ds:[00858624h]
004FF060: mov eax,dword ptr [edi+10h]
004FF063: fmul dword ptr [esp+1Ch]
004FF067: fstp dword ptr [esp+0Ch]
004FF06B: fld dword ptr [eax+000004C0h]
004FF071: fcomp dword ptr ds:[00862D28h]
004FF077: fnstsw ax
004FF079: test ah,5
004FF07C: jp 004FF0AB
004FF07E: mov ecx,dword ptr ds:[008CBDFCh]
004FF084: fld dword ptr ds:[008CBDFCh]
004FF08A: fmul dword ptr ds:[00858C1Ch]
004FF090: push ecx
004FF091: push ecx
004FF092: fstp dword ptr [esp]
004FF095: call 004D9C50
004FF09A: fadd dword ptr ds:[008CBDF8h]
004FF0A0: add esp,8
004FF0A3: fmul dword ptr [esp+0Ch]
004FF0A7: fstp dword ptr [esp+0Ch]
004FF0AB: fld dword ptr [esi+0000022Ch]
004FF0B1: fcomp dword ptr ds:[00858B50h]
004FF0B7: fnstsw ax
004FF0B9: test ah,5
004FF0BC: jp 004FF0C8
004FF0BE: mov edx,dword ptr [esp+0Ch]
004FF0C2: mov dword ptr [esi+0000022Ch],edx
004FF0C8: fld dword ptr [esp+0Ch]
004FF0CC: fcomp dword ptr [esi+0000022Ch]
004FF0D2: fld dword ptr [esi+0000022Ch]
004FF0D8: fnstsw ax
004FF0DA: test ah,1
004FF0DD: jne 004FF0F8
004FF0DF: fadd dword ptr ds:[00862D04h]
004FF0E5: fld dword ptr [esp+0Ch]
004FF0E9: fcomp st(1)
004FF0EB: fnstsw ax
004FF0ED: test ah,5
004FF0F0: jnp 004FF111
004FF0F2: fstp dword ptr [esp+0Ch]
004FF0F6: jmp 004FF113
004FF0F8: fsub dword ptr ds:[00862D04h]
004FF0FE: fld dword ptr [esp+0Ch]
004FF102: fcomp st(1)
004FF104: fnstsw ax
004FF106: test ah,41h
004FF109: je 004FF111
004FF10B: fstp dword ptr [esp+0Ch]
004FF10F: jmp 004FF113
004FF111: fstp st(0)
004FF113: mov eax,dword ptr [esp+0Ch]
004FF117: mov dword ptr [esi+0000022Ch],eax
004FF11D: mov eax,dword ptr ds:[00B6F03Ch]
004FF122: test eax,eax
004FF124: lea edx,[eax+30h]
004FF127: jne 004FF12E
004FF129: mov edx,0B6F02Ch
004FF12E: mov eax,dword ptr [esi+4]
004FF131: mov ecx,dword ptr [eax+14h]
004FF134: test ecx,ecx
004FF136: je 004FF13D
004FF138: lea eax,[ecx+30h]
004FF13B: jmp 004FF140
004FF13D: add eax,4
004FF140: fld dword ptr [eax+8]
004FF143: fsub dword ptr [edx+8]
004FF146: fld dword ptr [eax+4]
004FF149: fsub dword ptr [edx+4]
004FF14C: fld dword ptr [eax]
004FF14E: fsub dword ptr [edx]
004FF150: fld st(0)
004FF152: fmul st,st(1)
004FF154: fld st(2)
004FF156: fmul st,st(3)
004FF158: faddp st(1),st
004FF15A: fld st(3)
004FF15C: fmul st,st(4)
004FF15E: faddp st(1),st
004FF160: fstp st(3)
004FF162: fstp st(0)
004FF164: fstp st(0)
004FF166: fsqrt
004FF168: fst dword ptr [esp+10h]
004FF16C: fcomp dword ptr ds:[00858BF4h]
004FF172: fnstsw ax
004FF174: test ah,5
004FF177: jp 004FF1D1
004FF179: fld dword ptr [esp+10h]
004FF17D: fcomp dword ptr ds:[00862D38h]
004FF183: fnstsw ax
004FF185: test ah,5
004FF188: jp 004FF194
004FF18A: mov dword ptr [esp+10h],0C2C80000h
004FF192: jmp 004FF1EA
004FF194: fld dword ptr [esp+10h]
004FF198: fsub dword ptr ds:[00862D38h]
004FF19E: fmul dword ptr ds:[0085A5C4h]
004FF1A4: fstp dword ptr [esp+1Ch]
004FF1A8: mov ecx,dword ptr [esp+1Ch]
004FF1AC: push ecx
004FF1AD: call 004D9E50
004FF1B2: fmul dword ptr ds:[00858BA4h]
004FF1B8: push ebx
004FF1B9: fstp dword ptr [esp+24h]
004FF1BD: call 004D9E50
004FF1C2: fmul dword ptr ds:[00858BA4h]
004FF1C8: add esp,8
004FF1CB: fadd dword ptr [esp+1Ch]
004FF1CF: jmp 004FF1E0
004FF1D1: push ebx
004FF1D2: call 004D9E50
004FF1D7: fmul dword ptr ds:[00858BA4h]
004FF1DD: add esp,4
004FF1E0: fadd dword ptr ds:[00859000h]
004FF1E6: fstp dword ptr [esp+10h]
004FF1EA: fld dword ptr ds:[00C8132Ch]
004FF1F0: fcomp dword ptr ds:[00858B8Ch]
004FF1F6: fnstsw ax
004FF1F8: test ah,1
004FF1FB: jne 004FF23D
004FF1FD: xor edi,edi
004FF200: cmp di,1
004FF204: je 004FF20E
004FF206: push edi
004FF207: mov ecx,esi
004FF209: call 004F55C0
004FF20E: inc edi
004FF20F: cmp di,0Ch
004FF213: jl 004FF200
004FF215: push ebx
004FF216: call 004D9E50
004FF21B: fmul dword ptr ds:[00858BA4h]
004FF221: add esp,4
004FF224: push 3F800000h
004FF229: fadd dword ptr ds:[00859000h]
004FF22F: fstp dword ptr [esp+14h]
004FF233: mov edx,dword ptr [esp+14h]
004FF237: push edx
004FF238: jmp 004FF2E0
004FF23D: cmp word ptr [esi+000000DEh],0Ch
004FF245: jne 004FF295
004FF247: push ebx
004FF248: call 004D9E50
004FF24D: fmul dword ptr ds:[00858BA4h]
004FF253: mov eax,dword ptr [esp+10h]
004FF257: add esp,4
004FF25A: fadd dword ptr ds:[00859000h]
004FF260: fld dword ptr ds:[008CBDA0h]
004FF266: push eax
004FF267: push ecx
004FF268: fadd st,st(1)
004FF26A: mov ecx,esi
004FF26C: fstp dword ptr [esp]
004FF26F: push 1
004FF271: push 12h
004FF273: push 2
004FF275: fstp st(0)
004FF277: call 004F93C0
004FF27C: fld dword ptr ds:[008CBDA0h]
004FF282: fadd dword ptr [esp+10h]
004FF286: push 3F800000h
004FF28B: push ecx
004FF28C: fstp dword ptr [esp]
004FF28F: push 0
004FF291: push 12h
004FF293: jmp 004FF2E4
004FF295: mov edi,dword ptr [esp+0Ch]
004FF299: mov ecx,dword ptr [esp+20h]
004FF29D: push edi
004FF29E: push ecx
004FF29F: push 5
004FF2A1: push 13h
004FF2A3: push 2
004FF2A5: mov ecx,esi
004FF2A7: call 004F93C0
004FF2AC: mov edx,dword ptr [esp+14h]
004FF2B0: push edi
004FF2B1: push edx
004FF2B2: push 12h
004FF2B4: push 13h
004FF2B6: push 4
004FF2B8: mov ecx,esi
004FF2BA: call 004F93C0
004FF2BF: mov eax,dword ptr [esp+24h]
004FF2C3: push 3F800000h
004FF2C8: push eax
004FF2C9: push 17h
004FF2CB: push 13h
004FF2CD: push 3
004FF2CF: mov ecx,esi
004FF2D1: call 004F93C0
004FF2D6: mov ecx,dword ptr [esp+10h]
004FF2DA: push 3F800000h
004FF2DF: push ecx
004FF2E0: push 3
004FF2E2: push 13h
004FF2E4: mov ecx,esi
004FF2E6: push 1
004FF2E8: call 004F93C0
004FF2ED: lea ecx,[esp+40h]
004FF2F1: mov dword ptr [esp+00000090h],0FFFFFFFFh
004FF2FC: call 0059ACD0
004FF301: pop edi
004FF302: pop ebx
004FF303: mov ecx,dword ptr [esp+00000080h]
 00
004FF30A: pop esi
004FF30B: mov dword ptr fs:[00000000h],ecx
 00
004FF312: add esp,88h
004FF318: ret 4
004FF320: push 0FFFFFFFFh
004FF322: push 83C488h
004FF327: mov eax,dword ptr fs:[00000000h]
004FF32D: push eax
004FF32E: mov dword ptr fs:[00000000h],esp
 00
004FF335: sub esp,80h
004FF33B: push esi
004FF33C: mov esi,ecx
004FF33E: mov dword ptr [esp+7Ch],0
004FF346: mov dword ptr [esp+00000080h],0
004FF351: xor eax,eax
004FF353: mov ax,word ptr [esi+000000DEh]
 00
004FF35A: push 28h
004FF35C: mov ecx,0B5F8B8h
004FF361: mov dword ptr [esp+00000090h],0
004FF36C: mov dword ptr [esp+10h],0
004FF374: push eax
004FF375: call 004D88C0
004FF37A: test al,al
004FF37C: je 004FF393
004FF37E: push 13h
004FF380: push 8Ah
004FF385: mov ecx,0B5F8B8h
004FF38A: call 004D88C0
004FF38F: test al,al
004FF391: jne 004FF3AC
004FF393: lea ecx,[esp+3Ch]
004FF397: mov dword ptr [esp+0000008Ch],0FFFFFFFFh
004FF3A2: call 0059ACD0
004FF3A7: jmp 004FF7A5
004FF3AC: push edi
004FF3AD: mov edi,dword ptr [esp+00000098h]
 00
004FF3B4: mov eax,dword ptr [edi+10h]
004FF3B7: fld dword ptr [eax+000005A4h]
004FF3BD: push edi
004FF3BE: fmul dword ptr ds:[00862D1Ch]
004FF3C4: mov ecx,esi
004FF3C6: fstp dword ptr [esp+0Ch]
004FF3CA: call 004F5080
004FF3CF: push 0B6F99Ch
004FF3D4: lea ecx,[esp+44h]
004FF3D8: call 0059BBC0
004FF3DD: mov eax,dword ptr ds:[00B6F03Ch]
004FF3E2: test eax,eax
004FF3E4: lea ecx,[eax+30h]
004FF3E7: jne 004FF3EE
004FF3E9: mov ecx,0B6F02Ch
004FF3EE: mov eax,dword ptr [esi+4]
004FF3F1: mov edx,dword ptr [eax+14h]
004FF3F4: test edx,edx
004FF3F6: je 004FF3FD
004FF3F8: lea eax,[edx+30h]
004FF3FB: jmp 004FF400
004FF3FD: add eax,4
004FF400: fld dword ptr [eax+8]
004FF403: fsub dword ptr [ecx+8]
004FF406: fld dword ptr [eax+4]
004FF409: fsub dword ptr [ecx+4]
004FF40C: fld dword ptr [eax]
004FF40E: fsub dword ptr [ecx]
004FF410: fstp dword ptr [esp+34h]
004FF414: mov ecx,dword ptr [esp+34h]
004FF418: mov dword ptr [esp+28h],ecx
004FF41C: fstp dword ptr [esp+38h]
004FF420: mov edx,dword ptr [esp+38h]
004FF424: lea ecx,[esp+28h]
004FF428: fstp dword ptr [esp+3Ch]
004FF42C: mov eax,dword ptr [esp+3Ch]
004FF430: mov dword ptr [esp+2Ch],edx
004FF434: mov dword ptr [esp+30h],eax
004FF438: call 0059C910
004FF43D: fld dword ptr [esp+2Ch]
004FF441: mov ecx,dword ptr [edi+10h]
004FF444: mov eax,dword ptr [ecx+14h]
004FF447: fmul dword ptr [eax+14h]
004FF44A: fld dword ptr [esp+30h]
004FF44E: fmul dword ptr [eax+18h]
004FF451: faddp st(1),st
004FF453: fld dword ptr [esp+28h]
004FF457: fmul dword ptr [eax+10h]
004FF45A: faddp st(1),st
004FF45C: fld dword ptr [eax+14h]
004FF45F: fadd dword ptr [eax+10h]
004FF462: fmul dword ptr ds:[00858B50h]
004FF468: fadd dword ptr [eax+18h]
004FF46B: fmul qword ptr ds:[00862D10h]
004FF471: fstp dword ptr [esp+20h]
004FF475: fld dword ptr [eax+4]
004FF478: fadd dword ptr [eax]
004FF47A: fmul dword ptr ds:[00858B50h]
004FF480: fadd dword ptr [eax+8]
004FF483: fst dword ptr [esp+0Ch]
004FF487: fcomp dword ptr ds:[00858B50h]
004FF48D: fnstsw ax
004FF48F: test ah,5
004FF492: jp 004FF4A2
004FF494: fld dword ptr [esp+0Ch]
004FF498: fmul qword ptr ds:[00862D10h]
004FF49E: fstp dword ptr [esp+0Ch]
004FF4A2: fadd dword ptr ds:[00858624h]
004FF4A8: fmul dword ptr ds:[00858B8Ch]
004FF4AE: fstp dword ptr [esp+18h]
004FF4B2: fld dword ptr [esp+8]
004FF4B6: fcomp dword ptr ds:[00858624h]
004FF4BC: fnstsw ax
004FF4BE: test ah,41h
004FF4C1: jne 004FF4F2
004FF4C3: mov dword ptr [esp+8],3F800000h
004FF4CB: fld dword ptr [esp+8]
004FF4CF: fsub dword ptr ds:[00858CC4h]
004FF4D5: fmul dword ptr ds:[008595F0h]
004FF4DB: fld dword ptr [esp+8]
004FF4DF: fcomp qword ptr ds:[00862D08h]
004FF4E5: fnstsw ax
004FF4E7: test ah,41h
004FF4EA: jne 004FF50B
004FF4EC: fld dword ptr [esp+8]
004FF4F0: jmp 004FF511
004FF4F2: fld dword ptr [esp+8]
004FF4F6: fcomp dword ptr ds:[00858CC4h]
004FF4FC: fnstsw ax
004FF4FE: test ah,5
004FF501: jp 004FF4CB
004FF503: fld dword ptr ds:[00858B50h]
004FF509: jmp 004FF4DB
004FF50B: fld qword ptr ds:[00862D08h]
004FF511: fstp dword ptr [esp+8]
004FF515: fcom qword ptr ds:[00862D08h]
004FF51B: fnstsw ax
004FF51D: test ah,41h
004FF520: je 004FF52A
004FF522: fstp st(0)
004FF524: fld qword ptr ds:[00862D08h]
004FF52A: cmp word ptr [edi+20h],0
004FF52F: fstp dword ptr [esp+14h]
004FF533: jle 004FF53D
004FF535: mov dword ptr [esp+10h],3DCCCCCDh
004FF53D: cmp word ptr [edi+22h],0
004FF542: jle 004FF552
004FF544: fld dword ptr [esp+10h]
004FF548: fsub dword ptr ds:[00858C28h]
004FF54E: fstp dword ptr [esp+10h]
004FF552: fld dword ptr ds:[00858624h]
004FF558: fsub dword ptr [esp+18h]
004FF55C: fmul dword ptr [esp+14h]
004FF560: fstp dword ptr [esp+1Ch]
004FF564: mov edx,dword ptr [esp+1Ch]
004FF568: push edx
004FF569: call 004D9E50
004FF56E: fmul dword ptr ds:[00858BA4h]
004FF574: fadd dword ptr ds:[00859000h]
004FF57A: fstp dword ptr [esp+28h]
004FF57E: fld dword ptr [esp+1Ch]
004FF582: fmul dword ptr ds:[00858B8Ch]
004FF588: fadd dword ptr ds:[00858B8Ch]
004FF58E: fmul dword ptr [esp+18h]
004FF592: fstp dword ptr [esp+20h]
004FF596: mov eax,dword ptr [esp+20h]
004FF59A: push eax
004FF59B: call 004D9E50
004FF5A0: fmul dword ptr ds:[00858BA4h]
004FF5A6: mov ecx,dword ptr [esp+1Ch]
004FF5AA: push ecx
004FF5AB: fadd dword ptr ds:[00859000h]
004FF5B1: fstp dword ptr [esp+28h]
004FF5B5: call 004D9E50
004FF5BA: fmul dword ptr ds:[00858BA4h]
004FF5C0: add esp,0Ch
004FF5C3: fadd dword ptr ds:[00859000h]
004FF5C9: fstp dword ptr [esp+18h]
004FF5CD: fld dword ptr [esp+0Ch]
004FF5D1: fmul dword ptr ds:[00858B1Ch]
004FF5D7: fld dword ptr [esp+20h]
004FF5DB: fmul dword ptr ds:[00858FCCh]
004FF5E1: faddp st(1),st
004FF5E3: fadd dword ptr [esp+10h]
004FF5E7: fadd dword ptr ds:[00858624h]
004FF5ED: fstp dword ptr [esp+0Ch]
004FF5F1: fld dword ptr [esi+0000022Ch]
004FF5F7: fcomp dword ptr ds:[00858B50h]
004FF5FD: fnstsw ax
004FF5FF: test ah,5
004FF602: jp 004FF60E
004FF604: mov edx,dword ptr [esp+0Ch]
004FF608: mov dword ptr [esi+0000022Ch],edx
004FF60E: fld dword ptr [esp+0Ch]
004FF612: fcomp dword ptr [esi+0000022Ch]
004FF618: fld dword ptr [esi+0000022Ch]
004FF61E: fnstsw ax
004FF620: test ah,1
004FF623: jne 004FF63E
004FF625: fadd dword ptr ds:[00862D04h]
004FF62B: fld dword ptr [esp+0Ch]
004FF62F: fcomp st(1)
004FF631: fnstsw ax
004FF633: test ah,5
004FF636: jnp 004FF657
004FF638: fstp dword ptr [esp+0Ch]
004FF63C: jmp 004FF659
004FF63E: fsub dword ptr ds:[00862D04h]
004FF644: fld dword ptr [esp+0Ch]
004FF648: fcomp st(1)
004FF64A: fnstsw ax
004FF64C: test ah,41h
004FF64F: je 004FF657
004FF651: fstp dword ptr [esp+0Ch]
004FF655: jmp 004FF659
004FF657: fstp st(0)
004FF659: mov eax,dword ptr [esp+0Ch]
004FF65D: mov dword ptr [esi+0000022Ch],eax
004FF663: mov eax,dword ptr ds:[00B6F03Ch]
004FF668: test eax,eax
004FF66A: lea edx,[eax+30h]
004FF66D: jne 004FF674
004FF66F: mov edx,0B6F02Ch
004FF674: mov eax,dword ptr [esi+4]
004FF677: mov ecx,dword ptr [eax+14h]
004FF67A: test ecx,ecx
004FF67C: je 004FF683
004FF67E: lea eax,[ecx+30h]
004FF681: jmp 004FF686
004FF683: add eax,4
004FF686: fld dword ptr [eax+8]
004FF689: fsub dword ptr [edx+8]
004FF68C: fld dword ptr [eax+4]
004FF68F: fsub dword ptr [edx+4]
004FF692: fld dword ptr [eax]
004FF694: fsub dword ptr [edx]
004FF696: fld st(0)
004FF698: fmul st,st(1)
004FF69A: fld st(2)
004FF69C: fmul st,st(3)
004FF69E: faddp st(1),st
004FF6A0: fld st(3)
004FF6A2: fmul st,st(4)
004FF6A4: faddp st(1),st
004FF6A6: fstp st(3)
004FF6A8: fstp st(0)
004FF6AA: fstp st(0)
004FF6AC: fsqrt
004FF6AE: fst dword ptr [esp+14h]
004FF6B2: fcomp dword ptr ds:[00858BF4h]
004FF6B8: fnstsw ax
004FF6BA: test ah,5
004FF6BD: jp 004FF71B
004FF6BF: fld dword ptr [esp+14h]
004FF6C3: fcomp dword ptr ds:[00862D38h]
004FF6C9: fnstsw ax
004FF6CB: test ah,5
004FF6CE: jp 004FF6DA
004FF6D0: mov dword ptr [esp+14h],0C2C80000h
004FF6D8: jmp 004FF738
004FF6DA: fld dword ptr [esp+14h]
004FF6DE: fsub dword ptr ds:[00862D38h]
004FF6E4: fmul dword ptr ds:[0085A5C4h]
004FF6EA: fstp dword ptr [esp+20h]
004FF6EE: mov ecx,dword ptr [esp+20h]
004FF6F2: push ecx
004FF6F3: call 004D9E50
004FF6F8: fmul dword ptr ds:[00858BA4h]
004FF6FE: mov edx,dword ptr [esp+0Ch]
004FF702: push edx
004FF703: fstp dword ptr [esp+28h]
004FF707: call 004D9E50
004FF70C: fmul dword ptr ds:[00858BA4h]
004FF712: add esp,8
004FF715: fadd dword ptr [esp+20h]
004FF719: jmp 004FF72E
004FF71B: mov eax,dword ptr [esp+8]
004FF71F: push eax
004FF720: call 004D9E50
004FF725: fmul dword ptr ds:[00858BA4h]
004FF72B: add esp,4
004FF72E: fadd dword ptr ds:[00859000h]
004FF734: fstp dword ptr [esp+14h]
004FF738: mov edi,dword ptr [esp+0Ch]
004FF73C: mov ecx,dword ptr [esp+24h]
004FF740: push edi
004FF741: push ecx
004FF742: push 0
004FF744: push 28h
004FF746: push 2
004FF748: mov ecx,esi
004FF74A: call 004F93C0
004FF74F: mov edx,dword ptr [esp+1Ch]
004FF753: push edi
004FF754: push edx
004FF755: push 1
004FF757: push 28h
004FF759: push 4
004FF75B: mov ecx,esi
004FF75D: call 004F93C0
004FF762: mov eax,dword ptr [esp+18h]
004FF766: push 3F800000h
004FF76B: push eax
004FF76C: push 11h
004FF76E: push 13h
004FF770: push 3
004FF772: mov ecx,esi
004FF774: call 004F93C0
004FF779: mov ecx,dword ptr [esp+14h]
004FF77D: push 3F800000h
004FF782: push ecx
004FF783: push 10h
004FF785: push 13h
004FF787: push 1
004FF789: mov ecx,esi
004FF78B: call 004F93C0
004FF790: lea ecx,[esp+40h]
004FF794: mov dword ptr [esp+00000090h],0FFFFFFFFh
004FF79F: call 0059ACD0
004FF7A4: pop edi
004FF7A5: mov ecx,dword ptr [esp+00000084h]
 00
004FF7AC: pop esi
004FF7AD: mov dword ptr fs:[00000000h],ecx
 00
004FF7B4: add esp,8Ch
004FF7BA: ret 4
004FF7C0: push 0FFFFFFFFh
004FF7C2: push 83C4A8h
004FF7C7: mov eax,dword ptr fs:[00000000h]
004FF7CD: push eax
004FF7CE: mov dword ptr fs:[00000000h],esp
 00
004FF7D5: sub esp,48h
004FF7D8: push ebx
004FF7D9: push esi
004FF7DA: xor esi,esi
004FF7DC: push edi
004FF7DD: mov edi,ecx
004FF7DF: mov dword ptr [esp+4Ch],esi
004FF7E3: mov dword ptr [esp+50h],esi
004FF7E7: mov ebx,dword ptr [esp+64h]
004FF7EB: mov eax,dword ptr [ebx+10h]
004FF7EE: test byte ptr [eax+00000428h],10h
 10
004FF7F5: mov dword ptr [esp+5Ch],esi
004FF7F9: jne 004FF814
004FF7FB: jmp 004FF800
004FF7FD: lea ecx,[ecx]
004FF800: push esi
004FF801: mov ecx,edi
004FF803: call 004F55C0
004FF808: inc esi
004FF809: cmp si,0Ch
004FF80D: jl 004FF800
004FF80F: jmp 004FF8AE
004FF814: push 13h
004FF816: push 8Ah
004FF81B: mov ecx,0B5F8B8h
004FF820: call 004D88C0
004FF825: test al,al
004FF827: je 004FF8AE
004FF82D: mov ebx,dword ptr [ebx+10h]
004FF830: fld dword ptr [ebx+000005A4h]
004FF836: fmul dword ptr ds:[00862D1Ch]
004FF83C: fcom dword ptr ds:[00858624h]
004FF842: fnstsw ax
004FF844: test ah,41h
004FF847: jne 004FF8D3
004FF84D: fstp st(0)
004FF84F: fld dword ptr ds:[00858624h]
004FF855: fsub dword ptr ds:[00858CC4h]
004FF85B: fmul dword ptr ds:[008595F0h]
004FF861: fcom qword ptr ds:[00862D08h]
004FF867: fnstsw ax
004FF869: test ah,41h
004FF86C: je 004FF876
004FF86E: fstp st(0)
004FF870: fld qword ptr ds:[00862D08h]
004FF876: fstp dword ptr [esp+64h]
004FF87A: mov ecx,dword ptr [esp+64h]
004FF87E: push ecx
004FF87F: call 004D9E50
004FF884: fmul dword ptr ds:[00858BA4h]
004FF88A: add esp,4
004FF88D: push 3F800000h
004FF892: fadd dword ptr ds:[00859000h]
004FF898: mov ecx,edi
004FF89A: fstp dword ptr [esp+68h]
004FF89E: mov edx,dword ptr [esp+68h]
004FF8A2: push edx
004FF8A3: push 11h
004FF8A5: push 13h
004FF8A7: push 3
004FF8A9: call 004F93C0
004FF8AE: lea ecx,[esp+0Ch]
004FF8B2: mov dword ptr [esp+5Ch],0FFFFFFFFh
004FF8BA: call 0059ACD0
004FF8BF: mov ecx,dword ptr [esp+54h]
004FF8C3: pop edi
004FF8C4: pop esi
004FF8C5: pop ebx
004FF8C6: mov dword ptr fs:[00000000h],ecx
 00
004FF8CD: add esp,54h
004FF8D0: ret 4
004FF8D3: fcom dword ptr ds:[00858CC4h]
004FF8D9: fnstsw ax
004FF8DB: test ah,5
004FF8DE: jp 004FF855
004FF8E4: fstp st(0)
004FF8E6: fld dword ptr ds:[00858B50h]
004FF8EC: jmp 004FF861
004FF900: push 0FFFFFFFFh
004FF902: push 83C4C8h
004FF907: mov eax,dword ptr fs:[00000000h]
004FF90D: push eax
004FF90E: mov dword ptr fs:[00000000h],esp
 00
004FF915: sub esp,88h
004FF91B: push esi
004FF91C: mov esi,ecx
004FF91E: mov dword ptr [esp+00000084h],0
004FF929: mov dword ptr [esp+00000088h],0
004FF934: push 13h
004FF936: push 8Ah
004FF93B: mov ecx,0B5F8B8h
004FF940: mov dword ptr [esp+0000009Ch],0
004FF94B: mov dword ptr [esp+24h],0
004FF953: call 004D88C0
004FF958: test al,al
004FF95A: jne 004FF975
004FF95C: lea ecx,[esp+44h]
004FF960: mov dword ptr [esp+00000094h],0FFFFFFFFh
004FF96B: call 0059ACD0
004FF970: jmp 004FFD3E
004FF975: push edi
004FF976: mov edi,dword ptr [esp+000000A4h]
 00
004FF97D: mov eax,dword ptr [edi+10h]
004FF980: movsx eax,word ptr [eax+22h]
004FF984: add eax,0FFFFFDF9h
004FF989: cmp eax,49h
004FF98C: ja 004FFA40
004FF992: movzx ecx,byte ptr [eax+004FFD6Ch]
 00
004FF999: jmp dword ptr [ecx*4+004FFD58h]
 00
004FF9A0: mov edx,dword ptr ds:[008CBE68h]
004FF9A6: mov eax,dword ptr ds:[008CBE6Ch]
004FF9AB: mov ecx,dword ptr ds:[00B6B9F4h]
004FF9B1: mov dword ptr [esp+8],edx
004FF9B5: mov edx,dword ptr ds:[008CBE78h]
004FF9BB: jmp 004FFA29
004FF9BD: mov eax,dword ptr ds:[008CBE7Ch]
004FF9C2: mov ecx,dword ptr ds:[008CBE80h]
004FF9C8: mov edx,dword ptr ds:[00B6B9F8h]
004FF9CE: mov dword ptr [esp+8],eax
004FF9D2: mov eax,dword ptr ds:[008CBE8Ch]
004FF9D7: mov dword ptr [esp+18h],ecx
004FF9DB: mov dword ptr [esp+14h],edx
004FF9DF: mov dword ptr [esp+1Ch],eax
004FF9E3: jmp 004FFA35
004FF9E5: mov ecx,dword ptr ds:[008CBE90h]
004FF9EB: mov edx,dword ptr ds:[008CBE94h]
004FF9F1: mov eax,dword ptr ds:[008CBE98h]
004FF9F6: mov dword ptr [esp+8],ecx
004FF9FA: mov ecx,dword ptr ds:[008CBEA4h]
004FFA00: mov dword ptr [esp+18h],edx
004FFA04: mov dword ptr [esp+14h],eax
004FFA08: mov dword ptr [esp+1Ch],ecx
004FFA0C: jmp 004FFA35
004FFA0E: mov edx,dword ptr ds:[008CBEA8h]
004FFA14: mov eax,dword ptr ds:[008CBEACh]
004FFA19: mov ecx,dword ptr ds:[00B6B9FCh]
004FFA1F: mov dword ptr [esp+8],edx
004FFA23: mov edx,dword ptr ds:[008CBEB8h]
004FFA29: mov dword ptr [esp+18h],eax
004FFA2D: mov dword ptr [esp+14h],ecx
004FFA31: mov dword ptr [esp+1Ch],edx
004FFA35: mov al,byte ptr [esp+000000A0h]
 00
004FFA3C: test al,al
004FFA3E: jne 004FFA56
004FFA40: xor edi,edi
004FFA42: push edi
004FFA43: mov ecx,esi
004FFA45: call 004F55C0
004FFA4A: inc edi
004FFA4B: cmp di,0Ch
004FFA4F: jl 004FFA42
004FFA51: jmp 004FFD29
004FFA56: push 0B6F99Ch
004FFA5B: lea ecx,[esp+4Ch]
004FFA5F: call 0059BBC0
004FFA64: mov eax,dword ptr ds:[00B6F03Ch]
004FFA69: test eax,eax
004FFA6B: lea ecx,[eax+30h]
004FFA6E: jne 004FFA75
004FFA70: mov ecx,0B6F02Ch
004FFA75: mov eax,dword ptr [esi+4]
004FFA78: mov edx,dword ptr [eax+14h]
004FFA7B: test edx,edx
004FFA7D: je 004FFA84
004FFA7F: lea eax,[edx+30h]
004FFA82: jmp 004FFA87
004FFA84: add eax,4
004FFA87: fld dword ptr [eax+8]
004FFA8A: fsub dword ptr [ecx+8]
004FFA8D: fld dword ptr [eax+4]
004FFA90: fsub dword ptr [ecx+4]
004FFA93: fld dword ptr [eax]
004FFA95: fsub dword ptr [ecx]
004FFA97: fstp dword ptr [esp+38h]
004FFA9B: mov eax,dword ptr [esp+38h]
004FFA9F: mov dword ptr [esp+28h],eax
004FFAA3: fstp dword ptr [esp+3Ch]
004FFAA7: mov ecx,dword ptr [esp+3Ch]
004FFAAB: mov dword ptr [esp+2Ch],ecx
004FFAAF: fstp dword ptr [esp+40h]
004FFAB3: mov edx,dword ptr [esp+40h]
004FFAB7: lea ecx,[esp+28h]
004FFABB: mov dword ptr [esp+30h],edx
004FFABF: call 0059C910
004FFAC4: fld dword ptr [esp+2Ch]
004FFAC8: mov eax,dword ptr [edi+10h]
004FFACB: mov eax,dword ptr [eax+14h]
004FFACE: fmul dword ptr [eax+14h]
004FFAD1: fld dword ptr [esp+30h]
004FFAD5: fmul dword ptr [eax+18h]
004FFAD8: faddp st(1),st
004FFADA: fld dword ptr [esp+28h]
004FFADE: fmul dword ptr [eax+10h]
004FFAE1: faddp st(1),st
004FFAE3: fld dword ptr [eax+14h]
004FFAE6: fadd dword ptr [eax+10h]
004FFAE9: fmul dword ptr ds:[00858B50h]
004FFAEF: fadd dword ptr [eax+18h]
004FFAF2: fmul qword ptr ds:[00862D10h]
004FFAF8: fstp dword ptr [esp+34h]
004FFAFC: fld dword ptr [eax+4]
004FFAFF: fadd dword ptr [eax]
004FFB01: fmul dword ptr ds:[00858B50h]
004FFB07: fadd dword ptr [eax+8]
004FFB0A: fst dword ptr [esp+24h]
004FFB0E: fcomp dword ptr ds:[00858B50h]
004FFB14: fnstsw ax
004FFB16: test ah,5
004FFB19: jp 004FFB29
004FFB1B: fld dword ptr [esp+24h]
004FFB1F: fmul qword ptr ds:[00862D10h]
004FFB25: fstp dword ptr [esp+24h]
004FFB29: fadd dword ptr ds:[00858624h]
004FFB2F: fmul dword ptr ds:[00858B8Ch]
004FFB35: fstp dword ptr [esp+10h]
004FFB39: fld dword ptr [esp+000000ACh]
 00
004FFB40: fcomp dword ptr ds:[00858B50h]
004FFB46: fnstsw ax
004FFB48: test ah,41h
004FFB4B: jne 004FFB55
004FFB4D: mov dword ptr [esp+20h],3DCCCCCDh
004FFB55: fld dword ptr [esp+000000B0h]
 00
004FFB5C: fcomp dword ptr ds:[00858B50h]
004FFB62: fnstsw ax
004FFB64: test ah,41h
004FFB67: jne 004FFB77
004FFB69: fld dword ptr [esp+20h]
004FFB6D: fsub dword ptr ds:[00858C28h]
004FFB73: fstp dword ptr [esp+20h]
004FFB77: fld dword ptr ds:[008CBD98h]
004FFB7D: fmul dword ptr [esp+10h]
004FFB81: fsubr dword ptr ds:[00858624h]
004FFB87: fmul dword ptr [esp+000000A8h]
 00
004FFB8E: fstp dword ptr [esp+0Ch]
004FFB92: mov ecx,dword ptr [esp+0Ch]
004FFB96: push ecx
004FFB97: call 004D9E50
004FFB9C: fmul dword ptr ds:[00858BA4h]
004FFBA2: fadd dword ptr [esp+0Ch]
004FFBA6: fstp dword ptr [esp+48h]
004FFBAA: fld dword ptr [esp+14h]
004FFBAE: fmul dword ptr ds:[00858B8Ch]
004FFBB4: fadd dword ptr ds:[00858B8Ch]
004FFBBA: fmul dword ptr [esp+000000ACh]
 00
004FFBC1: fstp dword ptr [esp+10h]
004FFBC5: mov edx,dword ptr [esp+10h]
004FFBC9: push edx
004FFBCA: call 004D9E50
004FFBCF: fmul dword ptr ds:[00858BA4h]
004FFBD5: add esp,8
004FFBD8: fadd dword ptr [esp+18h]
004FFBDC: fstp dword ptr [esp+0Ch]
004FFBE0: fld dword ptr [esp+000000ACh]
 00
004FFBE7: fcomp dword ptr ds:[00858B50h]
004FFBED: fnstsw ax
004FFBEF: test ah,41h
004FFBF2: jne 004FFC00
004FFBF4: mov eax,dword ptr [esp+14h]
004FFBF8: mov dword ptr [esi+00000230h],eax
004FFBFE: jmp 004FFC12
004FFC00: fld dword ptr [esi+00000230h]
004FFC06: fsub qword ptr ds:[0085A310h]
004FFC0C: fstp dword ptr [esi+00000230h]
004FFC12: mov ecx,dword ptr [esp+000000A8h]
 00
004FFC19: push ecx
004FFC1A: call 004D9E50
004FFC1F: fmul dword ptr ds:[00858BA4h]
004FFC25: add esp,4
004FFC28: fadd dword ptr [esi+00000230h]
004FFC2E: fstp dword ptr [esp+10h]
004FFC32: fld dword ptr [esp+24h]
004FFC36: fmul dword ptr ds:[00858B1Ch]
004FFC3C: fld dword ptr [esp+34h]
004FFC40: fmul dword ptr ds:[00858FCCh]
004FFC46: faddp st(1),st
004FFC48: fadd dword ptr [esp+20h]
004FFC4C: fadd dword ptr ds:[00858624h]
004FFC52: fmul dword ptr [esp+000000B8h]
 00
004FFC59: fstp dword ptr [esp+8]
004FFC5D: fld dword ptr [esi+0000022Ch]
004FFC63: fcomp dword ptr ds:[00858B50h]
004FFC69: fnstsw ax
004FFC6B: test ah,5
004FFC6E: jp 004FFC7A
004FFC70: mov edx,dword ptr [esp+8]
004FFC74: mov dword ptr [esi+0000022Ch],edx
004FFC7A: fld dword ptr [esp+8]
004FFC7E: fcomp dword ptr [esi+0000022Ch]
004FFC84: fld dword ptr [esi+0000022Ch]
004FFC8A: fnstsw ax
004FFC8C: test ah,1
004FFC8F: jne 004FFCAA
004FFC91: fadd dword ptr ds:[00862D04h]
004FFC97: fld dword ptr [esp+8]
004FFC9B: fcomp st(1)
004FFC9D: fnstsw ax
004FFC9F: test ah,5
004FFCA2: jnp 004FFCBD
004FFCA4: fstp dword ptr [esp+8]
004FFCA8: jmp 004FFCBF
004FFCAA: fsub dword ptr ds:[00862D04h]
004FFCB0: fld dword ptr [esp+8]
004FFCB4: fcomp st(1)
004FFCB6: fnstsw ax
004FFCB8: test ah,41h
004FFCBB: jne 004FFCA4
004FFCBD: fstp st(0)
004FFCBF: mov eax,dword ptr [esp+8]
004FFCC3: mov ecx,dword ptr [esp+44h]
004FFCC7: mov edi,eax
004FFCC9: push edi
004FFCCA: push ecx
004FFCCB: push 0Ah
004FFCCD: push 13h
004FFCCF: push 2
004FFCD1: mov ecx,esi
004FFCD3: mov dword ptr [esi+0000022Ch],eax
004FFCD9: call 004F93C0
004FFCDE: mov edx,dword ptr [esp+0Ch]
004FFCE2: push edi
004FFCE3: push edx
004FFCE4: push 0Bh
004FFCE6: push 13h
004FFCE8: push 4
004FFCEA: mov ecx,esi
004FFCEC: call 004F93C0
004FFCF1: fld dword ptr [esp+10h]
004FFCF5: fadd dword ptr [esp+000000B4h]
 00
004FFCFC: push 3F800000h
004FFD01: push ecx
004FFD02: mov ecx,esi
004FFD04: fstp dword ptr [esp]
004FFD07: push 1Ah
004FFD09: push 13h
004FFD0B: push 5
004FFD0D: call 004F93C0
004FFD12: mov eax,dword ptr [esp+1Ch]
004FFD16: push 3F800000h
004FFD1B: push eax
004FFD1C: push 0Eh
004FFD1E: push 13h
004FFD20: push 7
004FFD22: mov ecx,esi
004FFD24: call 004F93C0
004FFD29: lea ecx,[esp+48h]
004FFD2D: mov dword ptr [esp+00000098h],0FFFFFFFFh
004FFD38: call 0059ACD0
004FFD3D: pop edi
004FFD3E: mov ecx,dword ptr [esp+0000008Ch]
 00
004FFD45: pop esi
004FFD46: mov dword ptr fs:[00000000h],ecx
 00
004FFD4D: add esp,94h
004FFD53: ret 1Ch
004FFD56: mov edi,edi
004FFD58: mov al,byte ptr ds:[BD004FF9h]
004FFD5D: stc
004FFD5E: dec edi
004FFD5F: add ch,ah
004FFD61: stc
004FFD62: dec edi
004FFD63: add byte ptr [esi],cl
004FFD65: cli
004FFD66: dec edi
004FFD67: add byte ptr [eax-6],al
004FFD6A: dec edi
004FFD6B: add byte ptr [eax],al
004FFD6D: add dword ptr [esp+eax],eax
004FFD70: add al,4
004FFD72: add al,4
004FFD74: add al,4
004FFD76: add al,4
004FFD78: add al,4
004FFD7A: add al,4
004FFD7C: add al,4
004FFD7E: add al,4
004FFD80: add al,4
004FFD82: add al,4
004FFD84: add al,4
004FFD86: add al,4
004FFD88: add al,4
004FFD8A: add al,4
004FFD8C: add al,4
004FFD8E: add al,4
004FFD90: add al,4
004FFD92: add al,4
004FFD94: add al,4
004FFD96: add al,4
004FFD98: add al,4
004FFD9A: add al,4
004FFD9C: add al,4
004FFD9E: add al,4
004FFDA0: add al,4
004FFDA2: add al,4
004FFDA4: add al,4
004FFDA6: add al,byte ptr [esp+eax]
004FFDA9: add al,4
004FFDAB: add al,4
004FFDAD: add al,4
004FFDAF: add al,4
004FFDB1: add al,4
004FFDB3: add al,4
004FFDB5: add edx,dword ptr [eax+90909090h]
004FFDC0: sub esp,0Ch
004FFDC3: push esi
004FFDC4: mov esi,ecx
004FFDC6: cmp word ptr [esi+000000E0h],0FFFFh
004FFDCE: jne 004FFDF3
004FFDD0: xor eax,eax
004FFDD2: mov ax,word ptr [esi+000000DCh]
 00
004FFDD9: push eax
004FFDDA: call 004F4D10
004FFDDF: add esp,4
004FFDE2: cmp ax,0FFFFh
004FFDE6: mov word ptr [esi+000000E0h],ax
 00
004FFDED: je 00500031
004FFDF3: xor ecx,ecx
004FFDF5: mov cx,word ptr [esi+000000E0h]
 00
004FFDFC: xor edx,edx
004FFDFE: mov dx,word ptr [esi+000000DCh]
 00
004FFE05: push ecx
004FFE06: mov ecx,0B5F8B8h
004FFE0B: push edx
004FFE0C: call 004D88C0
004FFE11: test al,al
004FFE13: jne 004FFE6F
004FFE15: mov ax,word ptr [esi+000000E0h]
 00
004FFE1C: sub ax,7
004FFE20: test ax,ax
004FFE23: jl 004FFE48
004FFE25: cmp ax,0Ah
004FFE29: jge 004FFE48
004FFE2B: movsx eax,ax
004FFE2E: mov cx,word ptr [eax*4+00B6B9A0h]
004FFE36: cmp cx,word ptr [esi+000000DCh]
 00
004FFE3D: sete al
004FFE40: test al,al
004FFE42: jne 00500031
004FFE48: xor edx,edx
004FFE4A: mov dx,word ptr [esi+000000DCh]
 00
004FFE51: push edx
004FFE52: call 004F4D10
004FFE57: add esp,4
004FFE5A: mov word ptr [esi+000000E0h],ax
 00
004FFE61: mov byte ptr [esi+000000A9h],0
 00
004FFE68: pop esi
004FFE69: add esp,0Ch
004FFE6C: ret 4
004FFE6F: push ebx
004FFE70: push edi
004FFE71: mov edi,dword ptr [esp+1Ch]
004FFE75: mov edx,dword ptr [edi+14h]
004FFE78: fld dword ptr [edi+1Ch]
004FFE7B: fdiv dword ptr [edx+58h]
004FFE7E: mov eax,dword ptr [edi+10h]
004FFE81: movzx cx,byte ptr [eax+00000804h]
004FFE89: fcom dword ptr ds:[00858B50h]
004FFE8F: fld st(0)
004FFE91: fnstsw ax
004FFE93: test ah,5
004FFE96: jp 004FFE9A
004FFE98: fchs
004FFE9A: fcomp qword ptr ds:[0085A310h]
004FFEA0: fnstsw ax
004FFEA2: test ah,5
004FFEA5: jp 004FFEB8
004FFEA7: fcom dword ptr ds:[00858B50h]
004FFEAD: fnstsw ax
004FFEAF: test ah,5
004FFEB2: jp 004FFEC0
004FFEB4: fchs
004FFEB6: jmp 004FFEC0
004FFEB8: fstp st(0)
004FFEBA: fld qword ptr ds:[0085A310h]
004FFEC0: fst dword ptr [esp+1Ch]
004FFEC4: fcomp dword ptr ds:[00858B50h]
004FFECA: fnstsw ax
004FFECC: test ah,41h
004FFECF: je 004FFED9
004FFED1: mov dword ptr [esp+1Ch],0
004FFED9: movsx eax,cx
004FFEDC: mov dword ptr [esp+0Ch],eax
004FFEE0: push ecx
004FFEE1: fild dword ptr [esp+10h]
004FFEE5: fst dword ptr [esp+10h]
004FFEE9: fmul dword ptr ds:[00858C84h]
004FFEEF: fmul dword ptr [esp+20h]
004FFEF3: fstp dword ptr [esp]
004FFEF6: call 004D9E50
004FFEFB: fmul dword ptr ds:[00858BA4h]
004FFF01: mov ecx,dword ptr [edi+10h]
004FFF04: add esp,4
004FFF07: fadd dword ptr [esi+000000D8h]
004FFF0D: fadd dword ptr ds:[008CBEBCh]
004FFF13: fstp dword ptr [esp+14h]
004FFF17: fld dword ptr [esp+0Ch]
004FFF1B: fmul dword ptr ds:[00B6BA6Ch]
004FFF21: fmul dword ptr ds:[00858C48h]
004FFF27: fld dword ptr ds:[00B6BA6Ch]
004FFF2D: fmul dword ptr [esp+1Ch]
004FFF31: faddp st(1),st
004FFF33: fadd dword ptr ds:[008CBD5Ch]
004FFF39: fld dword ptr [ecx+00000648h]
004FFF3F: fsin
004FFF41: fcom dword ptr ds:[00858B50h]
004FFF47: fnstsw ax
004FFF49: test ah,5
004FFF4C: jp 004FFF50
004FFF4E: fchs
004FFF50: fmul dword ptr ds:[008CBD60h]
004FFF56: mov al,byte ptr [ecx+00000834h]
004FFF5C: test al,al
004FFF5E: fadd dword ptr ds:[00858624h]
004FFF64: fmul st,st(1)
004FFF66: fstp dword ptr [esp+10h]
004FFF6A: fstp st(0)
004FFF6C: je 004FFF82
004FFF6E: mov ecx,dword ptr [esp+1Ch]
004FFF72: push ecx
004FFF73: mov ecx,esi
004FFF75: call 004F60B0
004FFF7A: fstp dword ptr [esp+0Ch]
004FFF7E: mov bl,1
004FFF80: jmp 004FFF8C
004FFF82: mov dword ptr [esp+0Ch],0
004FFF8A: xor bl,bl
004FFF8C: mov edx,dword ptr [esp+0Ch]
004FFF90: push edx
004FFF91: call 004D9E50
004FFF96: fmul dword ptr ds:[00858BA4h]
004FFF9C: mov eax,dword ptr [esp+14h]
004FFFA0: mov edi,dword ptr [esp+18h]
004FFFA4: fadd dword ptr [esi+000000D8h]
004FFFAA: add esp,4
004FFFAD: xor ecx,ecx
004FFFAF: fadd dword ptr ds:[008CBEC0h]
004FFFB5: mov cx,word ptr [esi+000000E0h]
 00
004FFFBC: push eax
004FFFBD: push edi
004FFFBE: fstp dword ptr [esp+14h]
004FFFC2: push 0
004FFFC4: fld dword ptr ds:[00B6BA70h]
004FFFCA: fmul dword ptr [esp+28h]
004FFFCE: push ecx
004FFFCF: fadd dword ptr ds:[008CBD64h]
004FFFD5: push 1
004FFFD7: mov ecx,esi
004FFFD9: fstp dword ptr [esp+30h]
004FFFDD: call 004F9710
004FFFE2: mov edx,dword ptr [esp+1Ch]
004FFFE6: mov eax,dword ptr [esp+0Ch]
004FFFEA: xor ecx,ecx
004FFFEC: mov cx,word ptr [esi+000000E0h]
 00
004FFFF3: push edx
004FFFF4: push eax
004FFFF5: push 1
004FFFF7: push ecx
004FFFF8: push 2
004FFFFA: mov ecx,esi
004FFFFC: call 004F9710
00500001: test bl,bl
00500003: jne 00500029
00500005: mov al,byte ptr ds:[00B6BAC8h]
0050000A: test al,al
0050000C: je 00500029
0050000E: xor edx,edx
00500010: mov dx,word ptr [esi+000000E0h]
 00
00500017: push 3F800000h
0050001C: push edi
0050001D: push 1
0050001F: mov ecx,esi
00500021: push edx
00500022: push 3
00500024: call 004F9710
00500029: pop edi
0050002A: mov byte ptr ds:[00B6BAC8h],bl
00500030: pop ebx
00500031: pop esi
00500032: add esp,0Ch
00500035: ret 4
00500040: sub esp,0Ch
00500043: push esi
00500044: mov esi,ecx
00500046: xor eax,eax
00500048: mov ax,word ptr [esi+000000E0h]
 00
0050004F: xor ecx,ecx
00500051: mov cx,word ptr [esi+000000DCh]
 00
00500058: push eax
00500059: push ecx
0050005A: mov ecx,0B5F8B8h
0050005F: call 004D88C0
00500064: test al,al
00500066: je 005002B7
0050006C: push ebx
0050006D: push ebp
0050006E: mov ebp,dword ptr [esp+1Ch]
00500072: mov ecx,dword ptr [ebp+10h]
00500075: fld dword ptr [ecx+00000818h]
0050007B: push edi
0050007C: fcomp dword ptr ds:[00858C80h]
00500082: fnstsw ax
00500084: test ah,41h
00500087: jne 005000BF
00500089: fld dword ptr ds:[00B6BAD0h]
0050008F: fmul dword ptr [ecx+00000820h]
00500095: fcomp dword ptr ds:[00858B50h]
0050009B: fnstsw ax
0050009D: test ah,5
005000A0: jp 005000BF
005000A2: fld dword ptr ds:[00B6BACCh]
005000A8: fcomp dword ptr ds:[00858B50h]
005000AE: fnstsw ax
005000B0: test ah,44h
005000B3: jp 005000BF
005000B5: mov dword ptr ds:[00B6BACCh],40C00000h
005000BF: fld dword ptr ds:[00B6BACCh]
005000C5: fsub dword ptr ds:[00858CB0h]
005000CB: fld dword ptr ds:[00858B50h]
005000D1: fcomp st(1)
005000D3: fnstsw ax
005000D5: test ah,41h
005000D8: jne 005000E8
005000DA: fstp st(0)
005000DC: mov dword ptr ds:[00B6BACCh],0
005000E6: jmp 005000EE
005000E8: fstp dword ptr ds:[00B6BACCh]
005000EE: mov edx,dword ptr [ebp+14h]
005000F1: fld dword ptr [ebp+1Ch]
005000F4: fdiv dword ptr [edx+58h]
005000F7: movzx cx,byte ptr [ecx+00000804h]
005000FF: fcom dword ptr ds:[00858B50h]
00500105: fld st(0)
00500107: fnstsw ax
00500109: test ah,5
0050010C: jp 00500110
0050010E: fchs
00500110: fcomp qword ptr ds:[0085A310h]
00500116: fnstsw ax
00500118: test ah,5
0050011B: jp 0050012E
0050011D: fcom dword ptr ds:[00858B50h]
00500123: fnstsw ax
00500125: test ah,5
00500128: jp 00500136
0050012A: fchs
0050012C: jmp 00500136
0050012E: fstp st(0)
00500130: fld qword ptr ds:[0085A310h]
00500136: fst dword ptr [esp+20h]
0050013A: fcomp dword ptr ds:[00858B50h]
00500140: fnstsw ax
00500142: test ah,41h
00500145: je 0050014F
00500147: mov dword ptr [esp+20h],0
0050014F: movsx eax,cx
00500152: mov dword ptr [esp+10h],eax
00500156: push ecx
00500157: fild dword ptr [esp+14h]
0050015B: fst dword ptr [esp+14h]
0050015F: fmul dword ptr ds:[00858C84h]
00500165: fmul dword ptr [esp+24h]
00500169: fstp dword ptr [esp]
0050016C: call 004D9E50
00500171: fmul dword ptr ds:[00858BA4h]
00500177: mov ecx,dword ptr [ebp+10h]
0050017A: add esp,4
0050017D: fadd dword ptr [esi+000000D8h]
00500183: fadd dword ptr ds:[008CBEBCh]
00500189: fstp dword ptr [esp+18h]
0050018D: fld dword ptr [esp+10h]
00500191: fmul dword ptr ds:[00B6BA6Ch]
00500197: fmul dword ptr ds:[00858C48h]
0050019D: fld dword ptr ds:[00B6BA6Ch]
005001A3: fmul dword ptr [esp+20h]
005001A7: faddp st(1),st
005001A9: fadd dword ptr ds:[008CBD5Ch]
005001AF: fld dword ptr [ecx+00000648h]
005001B5: fsin
005001B7: fcom dword ptr ds:[00858B50h]
005001BD: fnstsw ax
005001BF: test ah,5
005001C2: jp 005001C6
005001C4: fchs
005001C6: fmul dword ptr ds:[008CBD60h]
005001CC: mov al,byte ptr [ecx+00000834h]
005001D2: test al,al
005001D4: fadd dword ptr ds:[00858624h]
005001DA: fmul st,st(1)
005001DC: fstp dword ptr [esp+14h]
005001E0: fstp st(0)
005001E2: je 005001F8
005001E4: mov ecx,dword ptr [esp+20h]
005001E8: push ecx
005001E9: mov ecx,esi
005001EB: call 004F60B0
005001F0: fstp dword ptr [esp+10h]
005001F4: mov bl,1
005001F6: jmp 00500202
005001F8: mov dword ptr [esp+10h],0
00500200: xor bl,bl
00500202: mov edx,dword ptr [esp+10h]
00500206: push edx
00500207: call 004D9E50
0050020C: fmul dword ptr ds:[00858BA4h]
00500212: mov eax,dword ptr [esp+18h]
00500216: mov edi,dword ptr [esp+1Ch]
0050021A: fadd dword ptr [esi+000000D8h]
00500220: add esp,4
00500223: xor ecx,ecx
00500225: fadd dword ptr ds:[008CBEC0h]
0050022B: mov cx,word ptr [esi+000000E0h]
 00
00500232: push eax
00500233: push edi
00500234: fstp dword ptr [esp+18h]
00500238: push 0
0050023A: fld dword ptr ds:[00B6BA70h]
00500240: fmul dword ptr [esp+2Ch]
00500244: push ecx
00500245: fadd dword ptr ds:[008CBD64h]
0050024B: push 1
0050024D: mov ecx,esi
0050024F: fstp dword ptr [esp+34h]
00500253: call 004F9710
00500258: mov edx,dword ptr [esp+20h]
0050025C: mov eax,dword ptr [esp+10h]
00500260: xor ecx,ecx
00500262: mov cx,word ptr [esi+000000E0h]
 00
00500269: push edx
0050026A: push eax
0050026B: push 1
0050026D: push ecx
0050026E: push 2
00500270: mov ecx,esi
00500272: call 004F9710
00500277: test bl,bl
00500279: jne 0050029F
0050027B: mov al,byte ptr ds:[00B6BAC9h]
00500280: test al,al
00500282: je 0050029F
00500284: xor edx,edx
00500286: mov dx,word ptr [esi+000000E0h]
 00
0050028D: push 3F800000h
00500292: push edi
00500293: push 1
00500295: mov ecx,esi
00500297: push edx
00500298: push 3
0050029A: call 004F9710
0050029F: mov eax,dword ptr [ebp+10h]
005002A2: mov ecx,dword ptr [eax+00000820h]
005002A8: pop edi
005002A9: pop ebp
005002AA: mov byte ptr ds:[00B6BAC9h],bl
005002B0: mov dword ptr ds:[00B6BAD0h],ecx
005002B6: pop ebx
005002B7: pop esi
005002B8: add esp,0Ch
005002BB: ret 4
005002C0: sub esp,0Ch
005002C3: push esi
005002C4: push edi
005002C5: mov edi,dword ptr [esp+18h]
005002C9: push edi
005002CA: mov esi,ecx
005002CC: lea eax,[esp+14h]
005002D0: push eax
005002D1: lea ecx,[esp+14h]
005002D5: push ecx
005002D6: mov ecx,esi
005002D8: mov byte ptr [esp+14h],0
005002DD: mov byte ptr [esp+18h],0
005002E2: mov byte ptr [esp+1Ch],0
005002E7: call 004F62A0
005002EC: mov al,byte ptr [esp+0Ch]
005002F0: test al,al
005002F2: jne 00500354
005002F4: mov eax,dword ptr [edi+10h]
005002F7: mov cx,word ptr [eax+0000045Ch]
 00
005002FE: test cx,cx
00500301: je 00500347
00500303: cmp cx,0FFFFh
00500308: je 00500347
0050030A: mov dl,byte ptr [eax+36h]
0050030D: and dl,0F8h
00500310: cmp dl,28h
00500313: je 00500347
00500315: mov ecx,dword ptr ds:[00B7CB84h]
0050031B: cmp ecx,dword ptr [eax+00000514h]
00500321: jbe 0050032F
00500323: lea edx,[ecx+000002EEh]
00500329: mov dword ptr [eax+00000514h],edx
0050032F: mov eax,dword ptr [edi+10h]
00500332: mov edx,dword ptr [eax+00000514h]
00500338: add ecx,177h
0050033E: cmp edx,ecx
00500340: setb byte ptr [esp+8]
00500345: jmp 00500354
00500347: push edi
00500348: lea ecx,[esp+0Ch]
0050034C: push ecx
0050034D: mov ecx,esi
0050034F: call 004F61E0
00500354: mov edx,dword ptr [esp+10h]
00500358: mov eax,dword ptr [esp+0Ch]
0050035C: mov ecx,dword ptr [esp+8]
00500360: push edi
00500361: push edx
00500362: push eax
00500363: push ecx
00500364: mov ecx,esi
00500366: call 004F99D0
0050036B: mov dl,byte ptr [esp+8]
0050036F: mov al,byte ptr [esp+0Ch]
00500373: mov cl,byte ptr [esp+10h]
00500377: pop edi
00500378: mov byte ptr [esi+000000BEh],dl
0050037E: mov byte ptr [esi+000000BFh],al
00500384: mov byte ptr [esi+000000C0h],cl
0050038A: pop esi
0050038B: add esp,0Ch
0050038E: ret 4
005003A0: sub esp,8
005003A3: push esi
005003A4: mov esi,ecx
005003A6: cmp word ptr [esi+000000E0h],0FFFFh
005003AE: jne 005003D3
005003B0: xor eax,eax
005003B2: mov ax,word ptr [esi+000000DCh]
 00
005003B9: push eax
005003BA: call 004F4D10
005003BF: add esp,4
005003C2: cmp ax,0FFFFh
005003C6: mov word ptr [esi+000000E0h],ax
 00
005003CD: je 00500705
005003D3: xor ecx,ecx
005003D5: mov cx,word ptr [esi+000000E0h]
 00
005003DC: xor edx,edx
005003DE: mov dx,word ptr [esi+000000DCh]
 00
005003E5: push ecx
005003E6: mov ecx,0B5F8B8h
005003EB: push edx
005003EC: call 004D88C0
005003F1: test al,al
005003F3: jne 00500448
005003F5: mov ax,word ptr [esi+000000E0h]
 00
005003FC: sub ax,7
00500400: test ax,ax
00500403: jl 00500428
00500405: cmp ax,0Ah
00500409: jge 00500428
0050040B: movsx eax,ax
0050040E: mov cx,word ptr [eax*4+00B6B9A0h]
00500416: cmp cx,word ptr [esi+000000DCh]
 00
0050041D: sete al
00500420: test al,al
00500422: jne 00500705
00500428: xor edx,edx
0050042A: mov dx,word ptr [esi+000000DCh]
 00
00500431: push edx
00500432: call 004F4D10
00500437: add esp,4
0050043A: mov word ptr [esi+000000E0h],ax
 00
00500441: pop esi
00500442: add esp,8
00500445: ret 4
00500448: push ebx
00500449: push edi
0050044A: mov edi,dword ptr [esp+18h]
0050044E: mov ebx,dword ptr [edi+10h]
00500451: test byte ptr [ebx+00000428h],10h
 10
00500458: je 005006BB
0050045E: mov al,byte ptr [esi+000000A8h]
00500464: test al,al
00500466: jne 005006BB
0050046C: mov eax,dword ptr ds:[00B6B990h]
00500471: test eax,eax
00500473: je 00500487
00500475: mov al,byte ptr [esi+000000A5h]
0050047B: test al,al
0050047D: je 00500487
0050047F: push edi
00500480: mov ecx,esi
00500482: call 004F5080
00500487: mov eax,dword ptr [edi+10h]
0050048A: mov eax,dword ptr [eax+14h]
0050048D: fld dword ptr [eax+24h]
00500490: add eax,20h
00500493: fadd dword ptr [eax]
00500495: fmul dword ptr ds:[00858B50h]
0050049B: fadd dword ptr [eax+8]
0050049E: fsubr dword ptr ds:[00858624h]
005004A4: fcom dword ptr ds:[00858624h]
005004AA: fnstsw ax
005004AC: test ah,5
005004AF: jp 005004B7
005004B1: fstp dword ptr [esp+18h]
005004B5: jmp 005004C1
005004B7: fstp st(0)
005004B9: mov dword ptr [esp+18h],3F800000h
005004C1: fld dword ptr [ebx+000005A4h]
005004C7: fmul dword ptr ds:[00858B3Ch]
005004CD: fld dword ptr ds:[00858624h]
005004D3: fcomp st(1)
005004D5: fnstsw ax
005004D7: test ah,5
005004DA: jnp 005004F9
005004DC: fld st(0)
005004DE: fld dword ptr ds:[00858B50h]
005004E4: fcomp st(1)
005004E6: fnstsw ax
005004E8: fstp st(0)
005004EA: test ah,41h
005004ED: jne 005004F9
005004EF: fstp st(0)
005004F1: fld dword ptr ds:[00858B50h]
005004F7: jmp 00500510
005004F9: fld dword ptr ds:[00858624h]
005004FF: fcomp st(1)
00500501: fnstsw ax
00500503: test ah,5
00500506: jp 00500510
00500508: fstp st(0)
0050050A: fld dword ptr ds:[00858624h]
00500510: test byte ptr [ebx+000005ACh],2
 02
00500517: je 00500521
00500519: fld dword ptr ds:[00862D50h]
0050051F: jmp 00500527
00500521: fld dword ptr ds:[00B6BA74h]
00500527: fmul st,st(1)
00500529: fadd dword ptr ds:[00858CC8h]
0050052F: fld dword ptr ds:[008CBED4h]
00500535: fmul dword ptr [esp+18h]
00500539: fadd st,st(1)
0050053B: fstp dword ptr [esp+18h]
0050053F: fstp st(0)
00500541: fmul dword ptr ds:[00858B3Ch]
00500547: fsub dword ptr ds:[00858B3Ch]
0050054D: fst dword ptr [esp+0Ch]
00500551: fld dword ptr [esi+00000234h]
00500557: fcomp dword ptr ds:[00858B50h]
0050055D: fnstsw ax
0050055F: test ah,5
00500562: jp 0050056E
00500564: mov ecx,dword ptr [esp+18h]
00500568: mov dword ptr [esi+00000234h],ecx
0050056E: fld dword ptr [esi+00000230h]
00500574: fcomp dword ptr ds:[00858B50h]
0050057A: fnstsw ax
0050057C: test ah,5
0050057F: jp 00500587
00500581: fst dword ptr [esi+00000230h]
00500587: fld dword ptr [esp+18h]
0050058B: fcomp dword ptr [esi+00000234h]
00500591: fld dword ptr [esi+00000234h]
00500597: fnstsw ax
00500599: test ah,41h
0050059C: jne 005005B7
0050059E: fadd dword ptr ds:[00858B38h]
005005A4: fld dword ptr [esp+18h]
005005A8: fcomp st(1)
005005AA: fnstsw ax
005005AC: test ah,5
005005AF: jnp 005005D0
005005B1: fstp dword ptr [esp+18h]
005005B5: jmp 005005D2
005005B7: fsub dword ptr ds:[00858B38h]
005005BD: fld dword ptr [esp+18h]
005005C1: fcomp st(1)
005005C3: fnstsw ax
005005C5: test ah,41h
005005C8: je 005005D0
005005CA: fstp dword ptr [esp+18h]
005005CE: jmp 005005D2
005005D0: fstp st(0)
005005D2: fcom dword ptr [esi+00000230h]
005005D8: fld dword ptr [esi+00000230h]
005005DE: fnstsw ax
005005E0: test ah,41h
005005E3: jne 00500604
005005E5: fadd dword ptr ds:[00858B3Ch]
005005EB: fstp dword ptr [esp+10h]
005005EF: fcomp dword ptr [esp+10h]
005005F3: fnstsw ax
005005F5: test ah,5
005005F8: jnp 00500621
005005FA: mov edx,dword ptr [esp+10h]
005005FE: mov dword ptr [esp+0Ch],edx
00500602: jmp 00500621
00500604: fsub dword ptr ds:[00858B3Ch]
0050060A: fstp dword ptr [esp+10h]
0050060E: fcomp dword ptr [esp+10h]
00500612: fnstsw ax
00500614: test ah,41h
00500617: je 00500621
00500619: mov eax,dword ptr [esp+10h]
0050061D: mov dword ptr [esp+0Ch],eax
00500621: fld dword ptr ds:[00C8132Ch]
00500627: fcomp dword ptr ds:[00858B8Ch]
0050062D: fnstsw ax
0050062F: test ah,1
00500632: jne 00500668
00500634: xor ecx,ecx
00500636: mov cx,word ptr [esi+000000E0h]
 00
0050063D: push 0C2C80000h
00500642: push 3F800000h
00500647: push 0
00500649: push ecx
0050064A: push 2
0050064C: mov ecx,esi
0050064E: call 004F9E90
00500653: fld dword ptr [esp+0Ch]
00500657: fadd dword ptr ds:[00858B44h]
0050065D: mov edx,dword ptr [esp+18h]
00500661: push ecx
00500662: fstp dword ptr [esp]
00500665: push edx
00500666: jmp 00500691
00500668: mov eax,dword ptr [esp+0Ch]
0050066C: mov ecx,dword ptr [esp+18h]
00500670: xor edx,edx
00500672: mov dx,word ptr [esi+000000E0h]
 00
00500679: push eax
0050067A: push ecx
0050067B: push 0
0050067D: mov ecx,esi
0050067F: push edx
00500680: push 2
00500682: call 004F9E90
00500687: push 0C2C80000h
0050068C: push 3F800000h
00500691: push 1Ch
00500693: push 13h
00500695: mov ecx,esi
00500697: push 3
00500699: call 004F9E90
0050069E: mov eax,dword ptr [esp+0Ch]
005006A2: mov ecx,dword ptr [esp+18h]
005006A6: pop edi
005006A7: pop ebx
005006A8: mov dword ptr [esi+00000230h],eax
005006AE: mov dword ptr [esi+00000234h],ecx
005006B4: pop esi
005006B5: add esp,8
005006B8: ret 4
005006BB: mov ecx,dword ptr [esi+000000F8h]
005006C1: xor edi,edi
005006C3: cmp ecx,edi
005006C5: je 005006E0
005006C7: push edi
005006C8: push 4
005006CA: call 004EF2B0
005006CF: mov ecx,dword ptr [esi+000000F8h]
005006D5: call 004EF1C0
005006DA: mov dword ptr [esi+000000F8h],edi
005006E0: mov ecx,dword ptr [esi+00000100h]
005006E6: cmp ecx,edi
005006E8: je 00500703
005006EA: push edi
005006EB: push 4
005006ED: call 004EF2B0
005006F2: mov ecx,dword ptr [esi+00000100h]
005006F8: call 004EF1C0
005006FD: mov dword ptr [esi+00000100h],edi
00500703: pop edi
00500704: pop ebx
00500705: pop esi
00500706: add esp,8
00500709: ret 4
00500710: sub esp,14h
00500713: push ebx
00500714: push esi
00500715: push 0
00500717: mov esi,ecx
00500719: call 0053FB70
0050071E: add esp,4
00500721: cmp word ptr [esi+000000E0h],0FFFFh
00500729: mov ebx,eax
0050072B: jne 00500750
0050072D: xor eax,eax
0050072F: mov ax,word ptr [esi+000000DCh]
 00
00500736: push eax
00500737: call 004F4D10
0050073C: add esp,4
0050073F: cmp ax,0FFFFh
00500743: mov word ptr [esi+000000E0h],ax
 00
0050074A: je 00500A9E
00500750: xor ecx,ecx
00500752: mov cx,word ptr [esi+000000E0h]
 00
00500759: xor edx,edx
0050075B: mov dx,word ptr [esi+000000DCh]
 00
00500762: push ecx
00500763: mov ecx,0B5F8B8h
00500768: push edx
00500769: call 004D88C0
0050076E: test al,al
00500770: jne 005007C6
00500772: mov ax,word ptr [esi+000000E0h]
 00
00500779: sub ax,7
0050077D: test ax,ax
00500780: jl 005007A5
00500782: cmp ax,0Ah
00500786: jge 005007A5
00500788: movsx eax,ax
0050078B: mov cx,word ptr [eax*4+00B6B9A0h]
00500793: cmp cx,word ptr [esi+000000DCh]
 00
0050079A: sete al
0050079D: test al,al
0050079F: jne 00500A9E
005007A5: xor edx,edx
005007A7: mov dx,word ptr [esi+000000DCh]
 00
005007AE: push edx
005007AF: call 004F4D10
005007B4: add esp,4
005007B7: mov word ptr [esi+000000E0h],ax
 00
005007BE: pop esi
005007BF: pop ebx
005007C0: add esp,14h
005007C3: ret 4
005007C6: mov cx,word ptr [esi+000000DCh]
 00
005007CD: cmp cx,85h
005007D2: jne 005007F8
005007D4: mov eax,dword ptr ds:[008CBF30h]
005007D9: fld dword ptr ds:[008CBF50h]
005007DF: mov edx,dword ptr ds:[008CBF24h]
005007E5: mov dword ptr [esp+0Ch],eax
005007E9: mov eax,dword ptr ds:[00B6BA80h]
005007EE: mov dword ptr [esp+14h],edx
005007F2: mov dword ptr [esp+10h],eax
005007F6: jmp 0050081B
005007F8: mov edx,dword ptr ds:[00B6BA00h]
005007FE: fld dword ptr ds:[008CBF4Ch]
00500804: mov eax,dword ptr ds:[008CBF08h]
00500809: mov dword ptr [esp+0Ch],edx
0050080D: mov edx,dword ptr ds:[00B6BA78h]
00500813: mov dword ptr [esp+14h],eax
00500817: mov dword ptr [esp+10h],edx
0050081B: mov eax,dword ptr [esp+20h]
0050081F: push edi
00500820: mov edi,dword ptr [eax+10h]
00500823: mov dl,byte ptr [edi+000005B8h]
00500829: test dl,8
0050082C: jne 00500865
0050082E: mov ecx,dword ptr [esi+000000F0h]
00500834: fstp st(0)
00500836: test ecx,ecx
00500838: je 00500A9D
0050083E: push 0
00500840: push 4
00500842: call 004EF2B0
00500847: mov ecx,dword ptr [esi+000000F0h]
0050084D: call 004EF1C0
00500852: pop edi
00500853: mov dword ptr [esi+000000F0h],0
0050085D: pop esi
0050085E: pop ebx
0050085F: add esp,14h
00500862: ret 4
00500865: mov eax,dword ptr [edi+14h]
00500868: fld dword ptr [eax+14h]
0050086B: add eax,10h
0050086E: fadd dword ptr [eax]
00500870: fmul dword ptr ds:[00858B50h]
00500876: fadd dword ptr [eax+8]
00500879: fcom qword ptr ds:[00862D60h]
0050087F: fnstsw ax
00500881: test ah,41h
00500884: jne 00500893
00500886: fcom qword ptr ds:[00862D58h]
0050088C: fnstsw ax
0050088E: test ah,5
00500891: jp 005008AA
00500893: fcom qword ptr ds:[00862D60h]
00500899: fnstsw ax
0050089B: test ah,41h
0050089E: je 005008B2
005008A0: fstp st(0)
005008A2: fld qword ptr ds:[00862D60h]
005008A8: jmp 005008B2
005008AA: fstp st(0)
005008AC: fld qword ptr ds:[00862D58h]
005008B2: mov al,byte ptr [edi+000005B9h]
005008B8: fxch st(1)
005008BA: and al,1
005008BC: fmul st,st(1)
005008BE: fstp dword ptr [esp+1Ch]
005008C2: fstp st(0)
005008C4: jne 005008D4
005008C6: push ebp
005008C7: mov ebp,dword ptr [edi+000005A4h]
005008CD: mov dword ptr [esp+28h],ebp
005008D1: pop ebp
005008D2: jmp 005008DC
005008D4: mov dword ptr [esp+24h],0
005008DC: test dl,40h
005008DF: jne 005008EB
005008E1: fld dword ptr [esp+24h]
005008E5: fchs
005008E7: fstp dword ptr [esp+24h]
005008EB: test al,al
005008ED: jne 0050099C
005008F3: fld dword ptr [esp+24h]
005008F7: fcomp dword ptr ds:[00858B50h]
005008FD: fld dword ptr [esp+24h]
00500901: fnstsw ax
00500903: test ah,5
00500906: jp 0050090A
00500908: fchs
0050090A: fld dword ptr ds:[00858624h]
00500910: fcomp st(1)
00500912: fnstsw ax
00500914: fstp st(0)
00500916: test ah,5
00500919: jnp 00500943
0050091B: fld dword ptr [esp+24h]
0050091F: fcomp dword ptr ds:[00858B50h]
00500925: fld dword ptr [esp+24h]
00500929: fnstsw ax
0050092B: test ah,5
0050092E: jp 00500932
00500930: fchs
00500932: fld dword ptr ds:[00858B50h]
00500938: fcomp st(1)
0050093A: fnstsw ax
0050093C: fstp st(0)
0050093E: test ah,41h
00500941: je 0050099C
00500943: fld dword ptr [esp+24h]
00500947: fcomp dword ptr ds:[00858B50h]
0050094D: fld dword ptr [esp+24h]
00500951: fnstsw ax
00500953: test ah,5
00500956: jp 0050095A
00500958: fchs
0050095A: fld dword ptr ds:[00858624h]
00500960: fcomp st(1)
00500962: fnstsw ax
00500964: fstp st(0)
00500966: test ah,5
00500969: jp 00500975
0050096B: mov dword ptr [esp+0Ch],3F800000h
00500973: jmp 005009A4
00500975: fld dword ptr [esp+24h]
00500979: fcomp dword ptr ds:[00858B50h]
0050097F: fnstsw ax
00500981: test ah,5
00500984: jp 00500992
00500986: fld dword ptr [esp+24h]
0050098A: fchs
0050098C: fstp dword ptr [esp+0Ch]
00500990: jmp 005009A4
00500992: mov edx,dword ptr [esp+24h]
00500996: mov dword ptr [esp+0Ch],edx
0050099A: jmp 005009A4
0050099C: mov dword ptr [esp+0Ch],0
005009A4: cmp cx,85h
005009A9: jne 005009C8
005009AB: mov eax,dword ptr [esp+0Ch]
005009AF: push eax
005009B0: call 004D9E50
005009B5: fmul dword ptr ds:[00858BA4h]
005009BB: add esp,4
005009BE: fadd dword ptr [esp+10h]
005009C2: fstp dword ptr [esp+10h]
005009C6: jmp 005009D0
005009C8: mov ecx,dword ptr [esp+10h]
005009CC: mov dword ptr [esp+10h],ecx
005009D0: test byte ptr [edi+36h],0F8h
005009D4: jne 00500A67
005009DA: fld dword ptr [esp+24h]
005009DE: fcomp dword ptr ds:[00858B50h]
005009E4: fld dword ptr [esp+24h]
005009E8: fnstsw ax
005009EA: test ah,5
005009ED: jp 005009F1
005009EF: fchs
005009F1: fcomp dword ptr ds:[00858CDCh]
005009F7: mov ecx,ebx
005009F9: fnstsw ax
005009FB: test ah,5
005009FE: jp 00500A1E
00500A00: call 005403F0
00500A05: mov ecx,ebx
00500A07: movsx edi,ax
00500A0A: call 00540080
00500A0F: movsx edx,ax
00500A12: sub edi,edx
00500A14: mov dword ptr [esp+24h],edi
00500A18: fild dword ptr [esp+24h]
00500A1C: jmp 00500A53
00500A1E: fld dword ptr [esp+24h]
00500A22: fcomp dword ptr ds:[00858B50h]
00500A28: fnstsw ax
00500A2A: test ah,41h
00500A2D: jne 00500A41
00500A2F: call 005403F0
00500A34: movsx eax,ax
00500A37: mov dword ptr [esp+24h],eax
00500A3B: fild dword ptr [esp+24h]
00500A3F: jmp 00500A53
00500A41: call 00540080
00500A46: movsx ecx,ax
00500A49: neg ecx
00500A4B: mov dword ptr [esp+24h],ecx
00500A4F: fild dword ptr [esp+24h]
00500A53: fmul dword ptr ds:[00859A3Ch]
00500A59: fmul dword ptr ds:[008CBF10h]
00500A5F: fadd dword ptr [esp+10h]
00500A63: fstp dword ptr [esp+10h]
00500A67: fld dword ptr [esp+0Ch]
00500A6B: mov edx,dword ptr [esp+10h]
00500A6F: fmul dword ptr [esp+14h]
00500A73: push edx
00500A74: xor eax,eax
00500A76: mov ax,word ptr [esi+000000E0h]
 00
00500A7D: fadd dword ptr [esp+1Ch]
00500A81: fld dword ptr [esp+20h]
00500A85: push ecx
00500A86: fadd dword ptr ds:[00858624h]
00500A8C: mov ecx,esi
00500A8E: fmulp st(1),st
00500A90: fstp dword ptr [esp]
00500A93: push 0
00500A95: push eax
00500A96: push 1
00500A98: call 004FA1C0
00500A9D: pop edi
00500A9E: pop esi
00500A9F: pop ebx
00500AA0: add esp,14h
00500AA3: ret 4
00500AB0: sub esp,8
00500AB3: push ebp
00500AB4: push esi
00500AB5: push edi
00500AB6: push 0
00500AB8: mov ebp,ecx
00500ABA: call 0053FB70
00500ABF: mov edi,eax
00500AC1: mov eax,dword ptr [esp+1Ch]
00500AC5: mov esi,dword ptr [eax+10h]
00500AC8: mov al,byte ptr [esi+000005B9h]
00500ACE: add esp,4
00500AD1: test al,1
00500AD3: jne 00500ADD
00500AD5: fld dword ptr [esi+000005A4h]
00500ADB: jmp 00500AE3
00500ADD: fld dword ptr ds:[00858B50h]
00500AE3: test byte ptr [esi+000005B8h],40h
 40
00500AEA: jne 00500AEE
00500AEC: fchs
00500AEE: fcom dword ptr ds:[00858B50h]
00500AF4: fld st(0)
00500AF6: fnstsw ax
00500AF8: test ah,5
00500AFB: jp 00500AFF
00500AFD: fchs
00500AFF: fld dword ptr ds:[00858624h]
00500B05: fcomp st(1)
00500B07: fnstsw ax
00500B09: fstp st(0)
00500B0B: test ah,5
00500B0E: jnp 00500B3C
00500B10: fcom dword ptr ds:[00858B50h]
00500B16: fld st(0)
00500B18: fnstsw ax
00500B1A: test ah,5
00500B1D: jp 00500B21
00500B1F: fchs
00500B21: fld dword ptr ds:[00858B50h]
00500B27: fcomp st(1)
00500B29: fnstsw ax
00500B2B: fstp st(0)
00500B2D: test ah,41h
00500B30: jne 00500B3C
00500B32: mov dword ptr [esp+18h],0
00500B3A: jmp 00500B83
00500B3C: fcom dword ptr ds:[00858B50h]
00500B42: fld st(0)
00500B44: fnstsw ax
00500B46: test ah,5
00500B49: jp 00500B4D
00500B4B: fchs
00500B4D: fld dword ptr ds:[00858624h]
00500B53: fcomp st(1)
00500B55: fnstsw ax
00500B57: fstp st(0)
00500B59: test ah,5
00500B5C: jp 00500B68
00500B5E: mov dword ptr [esp+18h],3F800000h
00500B66: jmp 00500B83
00500B68: fcom dword ptr ds:[00858B50h]
00500B6E: fnstsw ax
00500B70: test ah,5
00500B73: jp 00500B7F
00500B75: fld st(0)
00500B77: fchs
00500B79: fstp dword ptr [esp+18h]
00500B7D: jmp 00500B83
00500B7F: fst dword ptr [esp+18h]
00500B83: test byte ptr [esi+36h],0F8h
00500B87: jne 00500C08
00500B89: fcom dword ptr ds:[00858B50h]
00500B8F: fld st(0)
00500B91: fnstsw ax
00500B93: test ah,5
00500B96: jp 00500B9A
00500B98: fchs
00500B9A: fcomp dword ptr ds:[00858CDCh]
00500BA0: mov ecx,edi
00500BA2: fnstsw ax
00500BA4: test ah,5
00500BA7: jp 00500BC1
00500BA9: fstp st(0)
00500BAB: mov dword ptr [esp+0Ch],0
00500BB3: call 005403F0
00500BB8: mov ecx,edi
00500BBA: call 00540080
00500BBF: jmp 00500C0A
00500BC1: fcomp dword ptr ds:[00858B50h]
00500BC7: fnstsw ax
00500BC9: test ah,41h
00500BCC: jne 00500BEB
00500BCE: call 00540080
00500BD3: movsx ecx,ax
00500BD6: mov dword ptr [esp+10h],ecx
00500BDA: mov ecx,edi
00500BDC: fild dword ptr [esp+10h]
00500BE0: fstp dword ptr [esp+0Ch]
00500BE4: call 005403F0
00500BE9: jmp 00500C0A
00500BEB: call 00540080
00500BF0: mov ecx,edi
00500BF2: call 005403F0
00500BF7: movsx edx,ax
00500BFA: mov dword ptr [esp+10h],edx
00500BFE: fild dword ptr [esp+10h]
00500C02: fstp dword ptr [esp+0Ch]
00500C06: jmp 00500C0A
00500C08: fstp st(0)
00500C0A: test byte ptr [esi+000005B9h],1
 01
00500C11: jne 00500C1C
00500C13: test byte ptr [esi+000005B8h],8
 08
00500C1A: jne 00500C38
00500C1C: push 0C2C80000h
00500C21: push 3F800000h
00500C26: push 0FFFFFFFFh
00500C28: mov ecx,ebp
00500C2A: call 004FA630
00500C2F: pop edi
00500C30: pop esi
00500C31: pop ebp
00500C32: add esp,8
00500C35: ret 4
00500C38: fld dword ptr ds:[00B6BA88h]
00500C3E: mov eax,dword ptr [esp+18h]
00500C42: fmul dword ptr [esp+18h]
00500C46: push 8CC1E8h
00500C4B: push 4
00500C4D: push eax
00500C4E: fadd dword ptr ds:[008CBF44h]
00500C54: fstp dword ptr [esp+1Ch]
00500C58: call 004D9D90
00500C5D: fld dword ptr [esp+18h]
00500C61: fmul dword ptr ds:[00859A3Ch]
00500C67: add esp,0Ch
00500C6A: fmulp st(1),st
00500C6C: fst dword ptr [esp+18h]
00500C70: fcomp dword ptr ds:[00859014h]
00500C76: fnstsw ax
00500C78: test ah,5
00500C7B: jp 00500C99
00500C7D: mov ecx,dword ptr [esp+10h]
00500C81: push 0C2C80000h
00500C86: push ecx
00500C87: push 0FFFFFFFFh
00500C89: mov ecx,ebp
00500C8B: call 004FA630
00500C90: pop edi
00500C91: pop esi
00500C92: pop ebp
00500C93: add esp,8
00500C96: ret 4
00500C99: mov edx,dword ptr [esp+18h]
00500C9D: push edx
00500C9E: call 004D9E50
00500CA3: fmul dword ptr ds:[00858BA4h]
00500CA9: mov ecx,dword ptr [esp+14h]
00500CAD: add esp,4
00500CB0: fadd dword ptr ds:[008CBF3Ch]
00500CB6: fstp dword ptr [esp+18h]
00500CBA: mov eax,dword ptr [esp+18h]
00500CBE: push eax
00500CBF: push ecx
00500CC0: push 0
00500CC2: mov ecx,ebp
00500CC4: call 004FA630
00500CC9: pop edi
00500CCA: pop esi
00500CCB: pop ebp
00500CCC: add esp,8
00500CCF: ret 4
00500CE0: push esi
00500CE1: push 28h
00500CE3: mov esi,ecx
00500CE5: push 45h
00500CE7: mov ecx,0B5F8B8h
00500CEC: call 004D88C0
00500CF1: test al,al
00500CF3: je 00500DB5
00500CF9: mov eax,dword ptr [esp+8]
00500CFD: mov ecx,dword ptr [eax+10h]
00500D00: test byte ptr [ecx+00000428h],10h
 10
00500D07: je 00500D8D
00500D0D: fld dword ptr [eax+1Ch]
00500D10: fdiv dword ptr ds:[008CBF88h]
00500D16: fstp dword ptr [esp+8]
00500D1A: fld dword ptr ds:[00858624h]
00500D20: fcomp dword ptr [esp+8]
00500D24: fnstsw ax
00500D26: test ah,5
00500D29: jnp 00500D46
00500D2B: fld dword ptr ds:[00858B50h]
00500D31: fcomp dword ptr [esp+8]
00500D35: fnstsw ax
00500D37: test ah,41h
00500D3A: jne 00500D4E
00500D3C: mov dword ptr [esp+8],0
00500D44: jmp 00500D4E
00500D46: mov dword ptr [esp+8],3F800000h
00500D4E: mov edx,dword ptr [esp+8]
00500D52: push edx
00500D53: call 004D9E50
00500D58: fmul dword ptr ds:[00858BA4h]
00500D5E: add esp,4
00500D61: push 40000000h
00500D66: fadd dword ptr ds:[008CBF84h]
00500D6C: mov ecx,esi
00500D6E: fstp dword ptr [esp+0Ch]
00500D72: mov eax,dword ptr [esp+0Ch]
00500D76: push eax
00500D77: push 3F800000h
00500D7C: push 3
00500D7E: push 45h
00500D80: push 28h
00500D82: push 0Bh
00500D84: call 004FAD40
00500D89: pop esi
00500D8A: ret 4
00500D8D: mov ecx,dword ptr [esi+00000140h]
00500D93: test ecx,ecx
00500D95: je 00500DB5
00500D97: push 0
00500D99: push 4
00500D9B: call 004EF2B0
00500DA0: mov ecx,dword ptr [esi+00000140h]
00500DA6: call 004EF1C0
00500DAB: mov dword ptr [esi+00000140h],0
00500DB5: pop esi
00500DB6: ret 4
00500DC0: push ecx
00500DC1: push esi
00500DC2: mov esi,ecx
00500DC4: cmp word ptr [esi+000000E0h],0FFFFh
00500DCC: jne 00500DF1
00500DCE: xor eax,eax
00500DD0: mov ax,word ptr [esi+000000DCh]
 00
00500DD7: push eax
00500DD8: call 004F4D10
00500DDD: add esp,4
00500DE0: cmp ax,0FFFFh
00500DE4: mov word ptr [esi+000000E0h],ax
 00
00500DEB: je 00500F41
00500DF1: xor ecx,ecx
00500DF3: mov cx,word ptr [esi+000000E0h]
 00
00500DFA: xor edx,edx
00500DFC: mov dx,word ptr [esi+000000DCh]
 00
00500E03: push ecx
00500E04: mov ecx,0B5F8B8h
00500E09: push edx
00500E0A: call 004D88C0
00500E0F: test al,al
00500E11: jne 00500E6B
00500E13: mov ax,word ptr [esi+000000E0h]
 00
00500E1A: sub ax,7
00500E1E: test ax,ax
00500E21: jl 00500E46
00500E23: cmp ax,0Ah
00500E27: jge 00500E46
00500E29: movsx eax,ax
00500E2C: mov cx,word ptr [eax*4+00B6B9A0h]
00500E34: cmp cx,word ptr [esi+000000DCh]
 00
00500E3B: sete al
00500E3E: test al,al
00500E40: jne 00500F41
00500E46: xor edx,edx
00500E48: mov dx,word ptr [esi+000000DCh]
 00
00500E4F: push edx
00500E50: call 004F4D10
00500E55: add esp,4
00500E58: mov word ptr [esi+000000E0h],ax
 00
00500E5F: mov byte ptr [esi+000000A9h],0
 00
00500E66: pop esi
00500E67: pop ecx
00500E68: ret 4
00500E6B: mov al,byte ptr [esi+000000A8h]
00500E71: test al,al
00500E73: push edi
00500E74: je 00500E8D
00500E76: xor edi,edi
00500E78: push edi
00500E79: mov ecx,esi
00500E7B: call 004F55C0
00500E80: inc edi
00500E81: cmp di,0Ch
00500E85: jl 00500E78
00500E87: pop edi
00500E88: pop esi
00500E89: pop ecx
00500E8A: ret 4
00500E8D: mov eax,dword ptr [esp+10h]
00500E91: mov ecx,dword ptr [eax+14h]
00500E94: fld dword ptr [eax+1Ch]
00500E97: fdiv dword ptr [ecx+58h]
00500E9A: fabs
00500E9C: fld dword ptr ds:[00858624h]
00500EA2: fcomp st(1)
00500EA4: fnstsw ax
00500EA6: test ah,5
00500EA9: jnp 00500ECA
00500EAB: fld st(0)
00500EAD: fld dword ptr ds:[00858B50h]
00500EB3: fcomp st(1)
00500EB5: fnstsw ax
00500EB7: fstp st(0)
00500EB9: test ah,41h
00500EBC: jne 00500ECA
00500EBE: fstp st(0)
00500EC0: mov dword ptr [esp+10h],0
00500EC8: jmp 00500EE9
00500ECA: fld dword ptr ds:[00858624h]
00500ED0: fcomp st(1)
00500ED2: fnstsw ax
00500ED4: test ah,5
00500ED7: jp 00500EE5
00500ED9: fstp st(0)
00500EDB: mov dword ptr [esp+10h],3F800000h
00500EE3: jmp 00500EE9
00500EE5: fstp dword ptr [esp+10h]
00500EE9: mov edi,dword ptr [esp+10h]
00500EED: push 8CC0A4h
00500EF2: push 4
00500EF4: push edi
00500EF5: call 004D9D90
00500EFA: fstp dword ptr [esp+1Ch]
00500EFE: push 8CBF8Ch
00500F03: push 4
00500F05: push edi
00500F06: call 004D9D90
00500F0B: fstp dword ptr [esp+20h]
00500F0F: mov edx,dword ptr [esp+28h]
00500F13: mov eax,dword ptr [esp+20h]
00500F17: add esp,18h
00500F1A: push 3F800000h
00500F1F: push edx
00500F20: xor ecx,ecx
00500F22: mov cx,word ptr [esi+000000DCh]
 00
00500F29: xor edx,edx
00500F2B: mov dx,word ptr [esi+000000E0h]
 00
00500F32: push eax
00500F33: push 0
00500F35: push ecx
00500F36: mov ecx,esi
00500F38: push edx
00500F39: push 2
00500F3B: call 004FAD40
00500F40: pop edi
00500F41: pop esi
00500F42: pop ecx
00500F43: ret 4
00500F50: push ecx
00500F51: push ebx
00500F52: push esi
00500F53: push 0
00500F55: mov esi,ecx
00500F57: call 0053FB70
00500F5C: add esp,4
00500F5F: cmp word ptr [esi+000000E0h],0FFFFh
00500F67: mov ebx,eax
00500F69: jne 00500F8E
00500F6B: xor eax,eax
00500F6D: mov ax,word ptr [esi+000000DCh]
 00
00500F74: push eax
00500F75: call 004F4D10
00500F7A: add esp,4
00500F7D: cmp ax,0FFFFh
00500F81: mov word ptr [esi+000000E0h],ax
 00
00500F88: je 00501260
00500F8E: xor ecx,ecx
00500F90: mov cx,word ptr [esi+000000E0h]
 00
00500F97: xor edx,edx
00500F99: mov dx,word ptr [esi+000000DCh]
 00
00500FA0: push ecx
00500FA1: mov ecx,0B5F8B8h
00500FA6: push edx
00500FA7: call 004D88C0
00500FAC: test al,al
00500FAE: jne 00501009
00500FB0: mov ax,word ptr [esi+000000E0h]
 00
00500FB7: sub ax,7
00500FBB: test ax,ax
00500FBE: jl 00500FE3
00500FC0: cmp ax,0Ah
00500FC4: jge 00500FE3
00500FC6: movsx eax,ax
00500FC9: mov cx,word ptr [eax*4+00B6B9A0h]
00500FD1: cmp cx,word ptr [esi+000000DCh]
 00
00500FD8: sete al
00500FDB: test al,al
00500FDD: jne 00501260
00500FE3: xor edx,edx
00500FE5: mov dx,word ptr [esi+000000DCh]
 00
00500FEC: push edx
00500FED: call 004F4D10
00500FF2: add esp,4
00500FF5: mov word ptr [esi+000000E0h],ax
 00
00500FFC: mov byte ptr [esi+000000A9h],0
 00
00501003: pop esi
00501004: pop ebx
00501005: pop ecx
00501006: ret 4
00501009: mov edx,dword ptr [esp+10h]
0050100D: mov ecx,dword ptr [edx+10h]
00501010: test byte ptr [ecx+00000428h],10h
 10
00501017: push edi
00501018: mov dword ptr [esp+0Ch],ecx
0050101C: je 0050124D
00501022: mov al,byte ptr [esi+000000A8h]
00501028: test al,al
0050102A: jne 0050124D
00501030: fld dword ptr [ecx+00000988h]
00501036: fmul dword ptr ds:[00862D68h]
0050103C: fcom dword ptr ds:[00858B50h]
00501042: fld st(0)
00501044: fnstsw ax
00501046: test ah,5
00501049: jp 0050104D
0050104B: fchs
0050104D: fld dword ptr ds:[00858624h]
00501053: fcomp st(1)
00501055: fnstsw ax
00501057: fstp st(0)
00501059: test ah,5
0050105C: jnp 0050108C
0050105E: fcom dword ptr ds:[00858B50h]
00501064: fld st(0)
00501066: fnstsw ax
00501068: test ah,5
0050106B: jp 0050106F
0050106D: fchs
0050106F: fld dword ptr ds:[00858B50h]
00501075: fcomp st(1)
00501077: fnstsw ax
00501079: fstp st(0)
0050107B: test ah,41h
0050107E: jne 0050108C
00501080: fstp st(0)
00501082: mov dword ptr [esp+14h],0
0050108A: jmp 005010CD
0050108C: fcom dword ptr ds:[00858B50h]
00501092: fld st(0)
00501094: fnstsw ax
00501096: test ah,5
00501099: jp 0050109D
0050109B: fchs
0050109D: fld dword ptr ds:[00858624h]
005010A3: fcomp st(1)
005010A5: fnstsw ax
005010A7: fstp st(0)
005010A9: test ah,5
005010AC: jp 005010BA
005010AE: fstp st(0)
005010B0: mov dword ptr [esp+14h],3F800000h
005010B8: jmp 005010CD
005010BA: fcom dword ptr ds:[00858B50h]
005010C0: fnstsw ax
005010C2: test ah,5
005010C5: jp 005010C9
005010C7: fchs
005010C9: fstp dword ptr [esp+14h]
005010CD: test byte ptr [ecx+36h],0F8h
005010D1: push ebp
005010D2: jne 00501115
005010D4: fld dword ptr [edx+1Ch]
005010D7: mov ecx,ebx
005010D9: fcomp dword ptr ds:[00858CDCh]
005010DF: fnstsw ax
005010E1: test ah,5
005010E4: jp 005010FF
005010E6: xor ebp,ebp
005010E8: call 005403F0
005010ED: mov ecx,ebx
005010EF: mov di,ax
005010F2: call 00540080
005010F7: mov ecx,dword ptr [esp+10h]
005010FB: sub edi,eax
005010FD: jmp 0050111D
005010FF: call 00540080
00501104: mov ecx,ebx
00501106: mov ebp,eax
00501108: call 005403F0
0050110D: mov ecx,dword ptr [esp+10h]
00501111: mov edi,eax
00501113: jmp 0050111D
00501115: mov edi,dword ptr [esp+18h]
00501119: mov ebp,dword ptr [esp+18h]
0050111D: test di,di
00501120: jle 0050112A
00501122: fld dword ptr ds:[00858B1Ch]
00501128: jmp 0050112E
0050112A: fld dword ptr [esp+18h]
0050112E: test bp,bp
00501131: pop ebp
00501132: jle 0050113C
00501134: fstp st(0)
00501136: fld dword ptr ds:[0085901Ch]
0050113C: fld dword ptr [ecx+000009C4h]
00501142: fmul dword ptr ds:[00862D18h]
00501148: fld dword ptr ds:[00858624h]
0050114E: fcomp st(1)
00501150: fnstsw ax
00501152: test ah,5
00501155: jnp 00501170
00501157: fld dword ptr ds:[00858B50h]
0050115D: fcomp st(1)
0050115F: fnstsw ax
00501161: test ah,41h
00501164: jne 00501178
00501166: fstp st(0)
00501168: fld dword ptr ds:[00858B50h]
0050116E: jmp 00501178
00501170: fstp st(0)
00501172: fld dword ptr ds:[00858624h]
00501178: fld dword ptr ds:[00B6BA90h]
0050117E: fmul st,st(1)
00501180: fadd dword ptr ds:[008CBFACh]
00501186: fstp dword ptr [esp+0Ch]
0050118A: fld dword ptr ds:[00B6BA94h]
00501190: fmul st,st(1)
00501192: fld dword ptr ds:[008CBFBCh]
00501198: fmul dword ptr [esp+14h]
0050119C: faddp st(1),st
0050119E: fadd dword ptr ds:[008CBFB4h]
005011A4: fadd st,st(2)
005011A6: fstp dword ptr [esp+14h]
005011AA: fstp st(0)
005011AC: fstp st(0)
005011AE: fld dword ptr [esi+0000022Ch]
005011B4: fcomp dword ptr ds:[00858B50h]
005011BA: fnstsw ax
005011BC: test ah,5
005011BF: jp 005011CB
005011C1: mov eax,dword ptr [esp+14h]
005011C5: mov dword ptr [esi+0000022Ch],eax
005011CB: fld dword ptr [esp+14h]
005011CF: fcomp dword ptr [esi+0000022Ch]
005011D5: fld dword ptr [esi+0000022Ch]
005011DB: fnstsw ax
005011DD: test ah,1
005011E0: jne 005011FB
005011E2: fadd dword ptr ds:[00862D04h]
005011E8: fld dword ptr [esp+14h]
005011EC: fcomp st(1)
005011EE: fnstsw ax
005011F0: test ah,5
005011F3: jnp 0050120E
005011F5: fstp dword ptr [esp+14h]
005011F9: jmp 00501210
005011FB: fsub dword ptr ds:[00862D04h]
00501201: fld dword ptr [esp+14h]
00501205: fcomp st(1)
00501207: fnstsw ax
00501209: test ah,41h
0050120C: jne 005011F5
0050120E: fstp st(0)
00501210: mov ecx,dword ptr [esp+14h]
00501214: mov edx,dword ptr [esp+0Ch]
00501218: push 40600000h
0050121D: push edx
0050121E: mov eax,ecx
00501220: mov dword ptr [esi+0000022Ch],ecx
00501226: xor ecx,ecx
00501228: mov cx,word ptr [esi+000000DCh]
 00
0050122F: xor edx,edx
00501231: mov dx,word ptr [esi+000000E0h]
 00
00501238: push eax
00501239: push 0
0050123B: push ecx
0050123C: mov ecx,esi
0050123E: push edx
0050123F: push 1
00501241: call 004FAD40
00501246: pop edi
00501247: pop esi
00501248: pop ebx
00501249: pop ecx
0050124A: ret 4
0050124D: xor edi,edi
00501250: push edi
00501251: mov ecx,esi
00501253: call 004F55C0
00501258: inc edi
00501259: cmp di,0Ch
0050125D: jl 00501250
0050125F: pop edi
00501260: pop esi
00501261: pop ebx
00501262: pop ecx
00501263: ret 4
00501270: push ecx
00501271: push esi
00501272: mov esi,ecx
00501274: cmp word ptr [esi+000000E0h],0FFFFh
0050127C: jne 005012A1
0050127E: xor eax,eax
00501280: mov ax,word ptr [esi+000000DCh]
 00
00501287: push eax
00501288: call 004F4D10
0050128D: add esp,4
00501290: cmp ax,0FFFFh
00501294: mov word ptr [esi+000000E0h],ax
 00
0050129B: je 00501478
005012A1: xor ecx,ecx
005012A3: mov cx,word ptr [esi+000000E0h]
 00
005012AA: xor edx,edx
005012AC: mov dx,word ptr [esi+000000DCh]
 00
005012B3: push ecx
005012B4: mov ecx,0B5F8B8h
005012B9: push edx
005012BA: call 004D88C0
005012BF: test al,al
005012C1: jne 0050131B
005012C3: mov ax,word ptr [esi+000000E0h]
 00
005012CA: sub ax,7
005012CE: test ax,ax
005012D1: jl 005012F6
005012D3: cmp ax,0Ah
005012D7: jge 005012F6
005012D9: movsx eax,ax
005012DC: mov cx,word ptr [eax*4+00B6B9A0h]
005012E4: cmp cx,word ptr [esi+000000DCh]
 00
005012EB: sete al
005012EE: test al,al
005012F0: jne 00501478
005012F6: xor edx,edx
005012F8: mov dx,word ptr [esi+000000DCh]
 00
005012FF: push edx
00501300: call 004F4D10
00501305: add esp,4
00501308: mov word ptr [esi+000000E0h],ax
 00
0050130F: mov byte ptr [esi+000000A9h],0
 00
00501316: pop esi
00501317: pop ecx
00501318: ret 4
0050131B: mov ecx,dword ptr [esp+0Ch]
0050131F: mov eax,dword ptr [ecx+10h]
00501322: test byte ptr [eax+00000428h],10h
 10
00501329: je 00501465
0050132F: mov al,byte ptr [esi+000000A8h]
00501335: test al,al
00501337: jne 00501465
0050133D: mov eax,dword ptr [ecx+14h]
00501340: test eax,eax
00501342: je 00501370
00501344: fld dword ptr [ecx+1Ch]
00501347: fdiv dword ptr [eax+58h]
0050134A: fabs
0050134C: fld dword ptr ds:[00858624h]
00501352: fcomp st(1)
00501354: fnstsw ax
00501356: test ah,5
00501359: jnp 005013D0
0050135B: fld st(0)
0050135D: fld dword ptr ds:[00858B50h]
00501363: fcomp st(1)
00501365: fnstsw ax
00501367: fstp st(0)
00501369: test ah,41h
0050136C: jne 005013D0
0050136E: fstp st(0)
00501370: mov dword ptr [esp+0Ch],0
00501378: mov ecx,dword ptr [esi+4]
0050137B: mov dl,byte ptr [ecx+36h]
0050137E: and dl,0F8h
00501381: cmp dl,20h
00501384: jne 0050138E
00501386: mov dword ptr [esp+0Ch],0
0050138E: fld dword ptr [esp+0Ch]
00501392: fcomp qword ptr ds:[00859068h]
00501398: fnstsw ax
0050139A: test ah,5
0050139D: jp 005013F1
0050139F: mov ecx,dword ptr [esi+000000F0h]
005013A5: test ecx,ecx
005013A7: je 00501478
005013AD: push 0
005013AF: push 4
005013B1: call 004EF2B0
005013B6: mov ecx,dword ptr [esi+000000F0h]
005013BC: call 004EF1C0
005013C1: mov dword ptr [esi+000000F0h],0
005013CB: pop esi
005013CC: pop ecx
005013CD: ret 4
005013D0: fld dword ptr ds:[00858624h]
005013D6: fcomp st(1)
005013D8: fnstsw ax
005013DA: test ah,5
005013DD: jp 005013EB
005013DF: fstp st(0)
005013E1: mov dword ptr [esp+0Ch],3F800000h
005013E9: jmp 00501378
005013EB: fstp dword ptr [esp+0Ch]
005013EF: jmp 00501378
005013F1: mov eax,dword ptr [esp+0Ch]
005013F5: push 8CBFD8h
005013FA: push 4
005013FC: push eax
005013FD: call 004D9D90
00501402: fstp dword ptr [esp+10h]
00501406: mov ecx,dword ptr [esp+10h]
0050140A: push ecx
0050140B: call 004D9E50
00501410: fmul dword ptr ds:[00858BA4h]
00501416: add esp,10h
00501419: push 40000000h
0050141E: fadd dword ptr ds:[008CBFCCh]
00501424: xor eax,eax
00501426: mov ax,word ptr [esi+000000DCh]
 00
0050142D: fstp dword ptr [esp+8]
00501431: fld dword ptr ds:[00B6BA9Ch]
00501437: mov edx,dword ptr [esp+8]
0050143B: fmul dword ptr [esp+10h]
0050143F: push edx
00501440: push ecx
00501441: xor ecx,ecx
00501443: mov cx,word ptr [esi+000000E0h]
 00
0050144A: fadd dword ptr ds:[008CBFD0h]
00501450: fstp dword ptr [esp]
00501453: push 0
00501455: push eax
00501456: push ecx
00501457: push 1
00501459: mov ecx,esi
0050145B: call 004FAD40
00501460: pop esi
00501461: pop ecx
00501462: ret 4
00501465: push edi
00501466: xor edi,edi
00501468: push edi
00501469: mov ecx,esi
0050146B: call 004F55C0
00501470: inc edi
00501471: cmp di,0Ch
00501475: jl 00501468
00501477: pop edi
00501478: pop esi
00501479: pop ecx
0050147A: ret 4
00501480: push esi
00501481: mov esi,ecx
00501483: cmp word ptr [esi+000000E0h],0FFFFh
0050148B: jne 005014B0
0050148D: xor eax,eax
0050148F: mov ax,word ptr [esi+000000DCh]
 00
00501496: push eax
00501497: call 004F4D10
0050149C: add esp,4
0050149F: cmp ax,0FFFFh
005014A3: mov word ptr [esi+000000E0h],ax
 00
005014AA: je 0050162E
005014B0: xor ecx,ecx
005014B2: mov cx,word ptr [esi+000000E0h]
 00
005014B9: xor edx,edx
005014BB: mov dx,word ptr [esi+000000DCh]
 00
005014C2: push ecx
005014C3: mov ecx,0B5F8B8h
005014C8: push edx
005014C9: call 004D88C0
005014CE: test al,al
005014D0: jne 00501529
005014D2: mov ax,word ptr [esi+000000E0h]
 00
005014D9: sub ax,7
005014DD: test ax,ax
005014E0: jl 00501505
005014E2: cmp ax,0Ah
005014E6: jge 00501505
005014E8: movsx eax,ax
005014EB: mov cx,word ptr [eax*4+00B6B9A0h]
005014F3: cmp cx,word ptr [esi+000000DCh]
 00
005014FA: sete al
005014FD: test al,al
005014FF: jne 0050162E
00501505: xor edx,edx
00501507: mov dx,word ptr [esi+000000DCh]
 00
0050150E: push edx
0050150F: call 004F4D10
00501514: add esp,4
00501517: mov word ptr [esi+000000E0h],ax
 00
0050151E: mov byte ptr [esi+000000A9h],0
 00
00501525: pop esi
00501526: ret 4
00501529: mov ecx,dword ptr [esp+8]
0050152D: mov eax,dword ptr [ecx+10h]
00501530: test byte ptr [eax+00000428h],10h
 10
00501537: je 00501616
0050153D: mov eax,dword ptr [ecx+14h]
00501540: test eax,eax
00501542: je 00501570
00501544: fld dword ptr [ecx+1Ch]
00501547: fdiv dword ptr [eax+58h]
0050154A: fabs
0050154C: fld dword ptr ds:[00858624h]
00501552: fcomp st(1)
00501554: fnstsw ax
00501556: test ah,5
00501559: jnp 005015AA
0050155B: fld st(0)
0050155D: fld dword ptr ds:[00858B50h]
00501563: fcomp st(1)
00501565: fnstsw ax
00501567: fstp st(0)
00501569: test ah,41h
0050156C: jne 005015AA
0050156E: fstp st(0)
00501570: mov dword ptr [esp+8],0
00501578: mov edx,dword ptr [esi+4]
0050157B: mov al,byte ptr [edx+36h]
0050157E: and al,0F8h
00501580: cmp al,20h
00501582: jne 0050158C
00501584: mov dword ptr [esp+8],0
0050158C: movsx eax,byte ptr [esi+000000A9h]
 00
00501593: cmp eax,0Ah
00501596: ja 0050161E
0050159C: movzx edx,byte ptr [eax+00501640h]
 00
005015A3: jmp dword ptr [edx*4+00501634h]
 00
005015AA: fld dword ptr ds:[00858624h]
005015B0: fcomp st(1)
005015B2: fnstsw ax
005015B4: test ah,5
005015B7: jp 005015C5
005015B9: fstp st(0)
005015BB: mov dword ptr [esp+8],3F800000h
005015C3: jmp 00501578
005015C5: fstp dword ptr [esp+8]
005015C9: jmp 00501578
005015CB: fld dword ptr [esp+8]
005015CF: fcomp dword ptr ds:[008CBBF0h]
005015D5: fnstsw ax
005015D7: test ah,5
005015DA: jp 005015F3
005015DC: push ecx
005015DD: mov ecx,dword ptr [esp+0Ch]
005015E1: push ecx
005015E2: mov eax,1
005015E7: push eax
005015E8: mov ecx,esi
005015EA: call 004FCA10
005015EF: pop esi
005015F0: ret 4
005015F3: push ecx
005015F4: mov ecx,dword ptr [esp+0Ch]
005015F8: push ecx
005015F9: mov eax,2
005015FE: push eax
005015FF: mov ecx,esi
00501601: call 004FCA10
00501606: pop esi
00501607: ret 4
0050160A: fld dword ptr [esp+8]
0050160E: fcomp dword ptr ds:[008CBBF4h]
00501614: jmp 005015D5
00501616: mov dword ptr [esp+8],0
0050161E: push ecx
0050161F: mov ecx,dword ptr [esp+0Ch]
00501623: push ecx
00501624: xor eax,eax
00501626: push eax
00501627: mov ecx,esi
00501629: call 004FCA10
0050162E: pop esi
0050162F: ret 4
00501632: mov edi,edi
00501634: retf
00501635: adc eax,160A0050h
0050163A: push eax
0050163B: add byte ptr [esi],bl
0050163D: push ss
0050163E: push eax
0050163F: add byte ptr [eax],al
00501641: add byte ptr [ecx],al
00501643: add al,byte ptr [edx]
00501645: add al,byte ptr [edx]
00501647: add al,byte ptr [edx]
00501649: add al,byte ptr [eax]
00501650: sub esp,8
00501653: push esi
00501654: push 13h
00501656: mov esi,ecx
00501658: push 8Ah
0050165D: mov ecx,0B5F8B8h
00501662: call 004D88C0
00501667: test al,al
00501669: je 0050194D
0050166F: push ebx
00501670: push edi
00501671: mov edi,dword ptr [esp+18h]
00501675: mov eax,dword ptr [edi+10h]
00501678: mov cl,byte ptr [eax+00000424h]
0050167E: test cl,cl
00501680: setge bl
00501683: test bl,bl
00501685: jne 00501691
00501687: push edi
00501688: mov ecx,esi
0050168A: call 004F5080
0050168F: jmp 0050169B
00501691: xor eax,eax
00501693: mov word ptr [edi+20h],ax
00501697: mov word ptr [edi+22h],ax
0050169B: mov cx,word ptr [edi+22h]
0050169F: xor eax,eax
005016A1: mov ax,word ptr [edi+20h]
005016A5: cmp ax,cx
005016A8: jg 005016AC
005016AA: mov eax,ecx
005016AC: movsx ecx,ax
005016AF: mov dword ptr [esp+18h],ecx
005016B3: fild dword ptr [esp+18h]
005016B7: fmul dword ptr ds:[00859A3Ch]
005016BD: fld dword ptr ds:[00858624h]
005016C3: fcomp st(1)
005016C5: fnstsw ax
005016C7: test ah,5
005016CA: jnp 005016E5
005016CC: fld dword ptr ds:[00858B50h]
005016D2: fcomp st(1)
005016D4: fnstsw ax
005016D6: test ah,41h
005016D9: jne 005016ED
005016DB: fstp st(0)
005016DD: fld dword ptr ds:[00858B50h]
005016E3: jmp 005016ED
005016E5: fstp st(0)
005016E7: fld dword ptr ds:[00858624h]
005016ED: test bl,bl
005016EF: jne 00501704
005016F1: mov edx,dword ptr [edi+10h]
005016F4: fstp st(0)
005016F6: fld dword ptr [edx+000009C4h]
005016FC: fmul dword ptr ds:[00862D18h]
00501702: jmp 00501710
00501704: fmul dword ptr ds:[00858C24h]
0050170A: fadd dword ptr ds:[00858CB0h]
00501710: fcom dword ptr ds:[00858624h]
00501716: fnstsw ax
00501718: test ah,41h
0050171B: jne 0050178D
0050171D: fstp st(0)
0050171F: fld dword ptr ds:[00858624h]
00501725: fsub dword ptr ds:[00858CC4h]
0050172B: fmul dword ptr ds:[008595F0h]
00501731: fcom qword ptr ds:[00862D08h]
00501737: fnstsw ax
00501739: test ah,41h
0050173C: je 00501746
0050173E: fstp st(0)
00501740: fld qword ptr ds:[00862D08h]
00501746: cmp word ptr [esi+7Ch],0
0050174B: fstp dword ptr [esp+10h]
0050174F: je 0050175E
00501751: mov ax,word ptr ds:[008CBDC4h]
00501757: mov word ptr [esi+000000B4h],ax
 00
0050175E: mov ecx,dword ptr [edi+10h]
00501761: fld dword ptr [ecx+000004C0h]
00501767: mov dword ptr [esp+18h],3F800000h
0050176F: fcomp dword ptr ds:[00859F80h]
00501775: fnstsw ax
00501777: test ah,5
0050177A: jp 005017A4
0050177C: mov ecx,dword ptr ds:[008CBDD8h]
00501782: mov edx,dword ptr ds:[008CBDDCh]
00501788: jmp 00501817
0050178D: fcom dword ptr ds:[00858CC4h]
00501793: fnstsw ax
00501795: test ah,5
00501798: jp 00501725
0050179A: fstp st(0)
0050179C: fld dword ptr ds:[00858B50h]
005017A2: jmp 00501731
005017A4: fld dword ptr [ecx+000004C0h]
005017AA: fcomp dword ptr ds:[00862D28h]
005017B0: fnstsw ax
005017B2: test ah,5
005017B5: jp 005017CC
005017B7: mov eax,dword ptr ds:[008CBDE0h]
005017BC: mov ecx,dword ptr ds:[008CBDE4h]
005017C2: mov dword ptr [esp+18h],eax
005017C6: mov dword ptr [esp+0Ch],ecx
005017CA: jmp 0050181F
005017CC: fld dword ptr [ecx+000004C0h]
005017D2: fcomp dword ptr ds:[00862D24h]
005017D8: fnstsw ax
005017DA: test ah,5
005017DD: jp 005017F4
005017DF: mov edx,dword ptr ds:[008CBDE8h]
005017E5: mov eax,dword ptr ds:[008CBDECh]
005017EA: mov dword ptr [esp+18h],edx
005017EE: mov dword ptr [esp+0Ch],eax
005017F2: jmp 0050181F
005017F4: fld dword ptr [ecx+000004C0h]
005017FA: fcomp dword ptr ds:[00862D20h]
00501800: fnstsw ax
00501802: test ah,5
00501805: jp 005018EA
0050180B: mov ecx,dword ptr ds:[008CBDF0h]
00501811: mov edx,dword ptr ds:[008CBDF4h]
00501817: mov dword ptr [esp+18h],ecx
0050181B: mov dword ptr [esp+0Ch],edx
0050181F: mov eax,dword ptr ds:[00B7CB84h]
00501824: cmp eax,dword ptr [esi+000000B8h]
0050182A: jbe 005018C7
00501830: mov al,byte ptr [esi+000000BCh]
00501836: test al,al
00501838: push ebp
00501839: je 0050187A
0050183B: mov ecx,dword ptr ds:[008CBDC8h]
00501841: push ecx
00501842: mov byte ptr [esi+000000BCh],0
 00
00501849: call 004D9C80
0050184E: mov ebp,dword ptr ds:[00B7CB84h]
00501854: add esp,4
00501857: test al,al
00501859: je 0050186A
0050185B: mov edx,dword ptr ds:[008CBDA8h]
00501861: mov eax,dword ptr ds:[008CBDA4h]
00501866: push edx
00501867: push eax
00501868: jmp 005018B6
0050186A: mov ecx,dword ptr ds:[008CBDB0h]
00501870: mov edx,dword ptr ds:[008CBDACh]
00501876: push ecx
00501877: push edx
00501878: jmp 005018B6
0050187A: mov eax,dword ptr ds:[008CBDCCh]
0050187F: push eax
00501880: mov byte ptr [esi+000000BCh],1
 01
00501887: call 004D9C80
0050188C: mov ebp,dword ptr ds:[00B7CB84h]
00501892: add esp,4
00501895: test al,al
00501897: je 005018A9
00501899: mov ecx,dword ptr ds:[008CBDB8h]
0050189F: mov edx,dword ptr ds:[008CBDB4h]
005018A5: push ecx
005018A6: push edx
005018A7: jmp 005018B6
005018A9: mov eax,dword ptr ds:[008CBDC0h]
005018AE: mov ecx,dword ptr ds:[008CBDBCh]
005018B4: push eax
005018B5: push ecx
005018B6: call 004D9C10
005018BB: add esp,8
005018BE: add eax,ebp
005018C0: mov dword ptr [esi+000000B8h],eax
005018C6: pop ebp
005018C7: movsx eax,word ptr [esi+000000B4h]
 00
005018CE: dec eax
005018CF: xor edx,edx
005018D1: test eax,eax
005018D3: setl dl
005018D6: dec edx
005018D7: and eax,edx
005018D9: mov word ptr [esi+000000B4h],ax
 00
005018E0: mov al,byte ptr [esi+000000BCh]
005018E6: test al,al
005018E8: jne 005018F2
005018EA: mov dword ptr [esp+0Ch],0
005018F2: movsx edx,word ptr [edi+22h]
005018F6: mov eax,dword ptr [esp+18h]
005018FA: mov ecx,dword ptr [esp+0Ch]
005018FE: push eax
005018FF: movsx eax,word ptr [edi+20h]
00501903: mov dword ptr [esp+1Ch],edx
00501907: mov edx,dword ptr [edi+10h]
0050190A: push ecx
0050190B: fild dword ptr [esp+20h]
0050190F: mov ecx,dword ptr [esp+18h]
00501913: sub esp,8
00501916: mov dword ptr [esp+28h],eax
0050191A: xor eax,eax
0050191C: fmul dword ptr ds:[00859A3Ch]
00501922: mov al,byte ptr [edx+00000428h]
00501928: shr al,4
0050192B: fstp dword ptr [esp+4]
0050192F: fild dword ptr [esp+28h]
00501933: fmul dword ptr ds:[00859A3Ch]
00501939: and eax,0FFFFFF01h
0050193E: fstp dword ptr [esp]
00501941: push ecx
00501942: push edi
00501943: push eax
00501944: mov ecx,esi
00501946: call 004FF900
0050194B: pop edi
0050194C: pop ebx
0050194D: pop esi
0050194E: add esp,8
00501951: ret 4
00501960: mov eax,dword ptr [esp+4]
00501964: sub esp,8
00501967: push ebx
00501968: push ebp
00501969: push esi
0050196A: mov esi,dword ptr [eax+10h]
0050196D: fld dword ptr [esi+0000049Ch]
00501973: mov ebp,ecx
00501975: mov cl,byte ptr [esi+00000424h]
0050197B: fmul dword ptr ds:[00859AACh]
00501981: test cl,cl
00501983: push edi
00501984: setge bl
00501987: call 00821B40
0050198C: fld dword ptr [esi+000004A0h]
00501992: fmul dword ptr ds:[00859AACh]
00501998: mov edi,eax
0050199A: call 00821B40
0050199F: mov ecx,eax
005019A1: cmp di,cx
005019A4: mov eax,edi
005019A6: jg 005019AA
005019A8: mov eax,ecx
005019AA: movsx edx,ax
005019AD: mov dword ptr [esp+10h],edx
005019B1: fild dword ptr [esp+10h]
005019B5: fmul dword ptr ds:[00859A3Ch]
005019BB: fld dword ptr ds:[00858624h]
005019C1: fcomp st(1)
005019C3: fnstsw ax
005019C5: test ah,5
005019C8: jnp 005019E3
005019CA: fld dword ptr ds:[00858B50h]
005019D0: fcomp st(1)
005019D2: fnstsw ax
005019D4: test ah,41h
005019D7: jne 005019EB
005019D9: fstp st(0)
005019DB: fld dword ptr ds:[00858B50h]
005019E1: jmp 005019EB
005019E3: fstp st(0)
005019E5: fld dword ptr ds:[00858624h]
005019EB: test bl,bl
005019ED: jne 005019FF
005019EF: fstp st(0)
005019F1: fld dword ptr [esi+000009C4h]
005019F7: fmul dword ptr ds:[00862D18h]
005019FD: jmp 00501A0B
005019FF: fmul dword ptr ds:[00858C24h]
00501A05: fadd dword ptr ds:[00858CB0h]
00501A0B: fcom dword ptr ds:[00858624h]
00501A11: fnstsw ax
00501A13: test ah,41h
00501A16: jne 00501A41
00501A18: fstp st(0)
00501A1A: fld dword ptr ds:[00858624h]
00501A20: fsub dword ptr ds:[00858CC4h]
00501A26: fmul dword ptr ds:[008595F0h]
00501A2C: fstp dword ptr [esp+10h]
00501A30: test byte ptr [esi+00000428h],10h
 10
00501A37: jne 00501A5A
00501A39: test bl,bl
00501A3B: jne 00501A5A
00501A3D: xor al,al
00501A3F: jmp 00501A5C
00501A41: fcom dword ptr ds:[00858CC4h]
00501A47: fnstsw ax
00501A49: test ah,5
00501A4C: jp 00501A20
00501A4E: fstp st(0)
00501A50: mov dword ptr [esp+10h],0
00501A58: jmp 00501A30
00501A5A: mov al,1
00501A5C: movsx ecx,cx
00501A5F: mov dword ptr [esp+14h],ecx
00501A63: mov ecx,dword ptr [esp+10h]
00501A67: push 3F800000h
00501A6C: fild dword ptr [esp+18h]
00501A70: push 0
00501A72: sub esp,8
00501A75: movsx edx,di
00501A78: mov dword ptr [esp+24h],edx
00501A7C: fmul dword ptr ds:[00859A3Ch]
00501A82: mov edx,dword ptr [esp+2Ch]
00501A86: fstp dword ptr [esp+4]
00501A8A: fild dword ptr [esp+24h]
00501A8E: fmul dword ptr ds:[00859A3Ch]
00501A94: fstp dword ptr [esp]
00501A97: push ecx
00501A98: push edx
00501A99: push eax
00501A9A: mov ecx,ebp
00501A9C: call 004FF900
00501AA1: pop edi
00501AA2: pop esi
00501AA3: pop ebp
00501AA4: pop ebx
00501AA5: add esp,8
00501AA8: ret 4
00501AB0: push esi
00501AB1: push edi
00501AB2: mov edi,dword ptr [esp+0Ch]
00501AB6: mov eax,dword ptr [edi+10h]
00501AB9: movsx eax,word ptr [eax+22h]
00501ABD: add eax,0FFFFFE4Dh
00501AC2: cmp eax,9Ch
00501AC7: mov esi,ecx
00501AC9: ja 00501B86
00501ACF: movzx ecx,byte ptr [eax+00501BA8h]
 00
00501AD6: jmp dword ptr [ecx*4+00501B8Ch]
 00
00501ADD: push edi
00501ADE: mov ecx,esi
00501AE0: call 004FAA80
00501AE5: pop edi
00501AE6: pop esi
00501AE7: ret 4
00501AEA: push edi
00501AEB: mov ecx,esi
00501AED: call 004FA7C0
00501AF2: pop edi
00501AF3: pop esi
00501AF4: ret 4
00501AF7: mov al,byte ptr [esi+000000A5h]
00501AFD: mov ecx,esi
00501AFF: push edi
00501B00: call 00500DC0
00501B05: pop edi
00501B06: pop esi
00501B07: ret 4
00501B0A: push edi
00501B0B: mov ecx,esi
00501B0D: call 00500F50
00501B12: push edi
00501B13: mov ecx,esi
00501B15: call 004F92C0
00501B1A: push edi
00501B1B: mov ecx,esi
00501B1D: call 005002C0
00501B22: pop edi
00501B23: pop esi
00501B24: ret 4
00501B27: push edi
00501B28: mov ecx,esi
00501B2A: call 004F8B00
00501B2F: push edi
00501B30: mov ecx,esi
00501B32: call 004F8F10
00501B37: push edi
00501B38: mov ecx,esi
00501B3A: call 004F8940
00501B3F: push edi
00501B40: mov ecx,esi
00501B42: call 005002C0
00501B47: mov al,byte ptr [esi+000000A5h]
00501B4D: mov ecx,esi
00501B4F: push edi
00501B50: call 00501270
00501B55: push edi
00501B56: mov ecx,esi
00501B58: call 004F92C0
00501B5D: mov edx,dword ptr [edi+10h]
00501B60: mov al,byte ptr [edx+36h]
00501B63: and al,0F8h
00501B65: cmp al,10h
00501B67: je 00501B86
00501B69: push edi
00501B6A: mov ecx,esi
00501B6C: call 004FAE20
00501B71: pop edi
00501B72: pop esi
00501B73: ret 4
00501B76: push edi
00501B77: mov ecx,esi
00501B79: call 004F8B00
00501B7E: push edi
00501B7F: mov ecx,esi
00501B81: call 004F92C0
00501B86: pop edi
00501B87: pop esi
00501B88: ret 4
00501B8C: jbe 00501BA9
00501B8E: push eax
00501B8F: add bh,dh
00501B91: sbb dl,byte ptr [eax]
00501B94: daa
00501B95: sbb edx,dword ptr [eax]
00501B98: jmp 501A:DD00501A
 50
00501B9F: add byte ptr [edx],cl
00501BA1: sbb edx,dword ptr [eax]
00501BA4: xchg bl,byte ptr [ebx]
00501BA6: push eax
00501BA7: add byte ptr [eax],al
00501BA9: push es
00501BAA: push es
00501BAB: push es
00501BAC: push es
00501BAD: push es
00501BAE: add dword ptr [esi],eax
00501BB0: push es
00501BB1: push es
00501BB2: push es
00501BB3: push es
00501BB4: push es
00501BB5: push es
00501BB6: push es
00501BB7: add byte ptr [esi],al
00501BB9: push es
00501BBA: push es
00501BBB: push es
00501BBC: push es
00501BBD: push es
00501BBE: add al,byte ptr [esi]
00501BC0: push es
00501BC1: push es
00501BC2: push es
00501BC3: push es
00501BC4: push es
00501BC5: add eax,dword ptr [esi+eax]
00501BC8: push es
00501BC9: push es
00501BCA: push es
00501BCB: push es
00501BCC: push es
00501BCD: push es
00501BCE: push es
00501BCF: push es
00501BD0: push es
00501BD1: push es
00501BD2: push es
00501BD3: push es
00501BD4: push es
00501BD5: push es
00501BD6: push es
00501BD7: push es
00501BD8: push es
00501BD9: push es
00501BDA: push es
00501BDB: push es
00501BDC: push es
00501BDD: push es
00501BDE: push es
00501BDF: push es
00501BE0: push es
00501BE1: push es
00501BE2: push es
00501BE3: push es
00501BE4: push es
00501BE5: push es
00501BE6: push es
00501BE7: push es
00501BE8: push es
00501BE9: push es
00501BEA: add al,6
00501BEC: push es
00501BED: push es
00501BEE: push es
00501BEF: push es
00501BF0: push es
00501BF1: push es
00501BF2: push es
00501BF3: push es
00501BF4: push es
00501BF5: push es
00501BF6: push es
00501BF7: push es
00501BF8: push es
00501BF9: push es
00501BFA: push es
00501BFB: push es
00501BFC: push es
00501BFD: push es
00501BFE: push es
00501BFF: push es
00501C00: push es
00501C01: push es
00501C02: push es
00501C03: push es
00501C04: push es
00501C05: push es
00501C06: push es
00501C07: push es
00501C08: push es
00501C09: push es
00501C0A: push es
00501C0B: push es
00501C0C: push es
00501C0D: push es
00501C0E: push es
00501C0F: push es
00501C10: add eax,6060606h
00501C15: push es
00501C16: push es
00501C17: push es
00501C18: push es
00501C19: push es
00501C1A: push es
00501C1B: push es
00501C1C: push es
00501C1D: push es
00501C1E: push es
00501C1F: push es
00501C20: push es
00501C21: push es
00501C22: push es
00501C23: push es
00501C24: push es
00501C25: push es
00501C26: push es
00501C27: push es
00501C28: push es
00501C29: add dword ptr [esi],eax
00501C2B: push es
00501C2C: push es
00501C2D: push es
00501C2E: push es
00501C2F: push es
00501C30: push es
00501C31: push es
00501C32: push es
00501C33: push es
00501C34: push es
00501C35: push es
00501C36: push es
00501C37: push es
00501C38: push es
00501C39: push es
00501C3A: push es
00501C3B: push es
00501C3C: push es
00501C3D: add byte ptr [esi],al
00501C3F: push es
00501C40: push es
00501C41: push es
00501C42: push es
00501C43: push es
00501C44: add byte ptr [eax+90909090h],dl
00501C50: push esi
00501C51: push 13h
00501C53: mov esi,ecx
00501C55: push 8Ah
00501C5A: mov ecx,0B5F8B8h
00501C5F: call 004D88C0
00501C64: test al,al
00501C66: je 00501DAB
00501C6C: mov ecx,dword ptr [esp+8]
00501C70: mov edx,dword ptr [ecx+10h]
00501C73: movsx eax,word ptr [edx+22h]
00501C77: add eax,0FFFFFDF9h
00501C7C: cmp eax,49h
00501C7F: push ebx
00501C80: ja 00501C94
00501C82: movzx eax,byte ptr [eax+00501DB8h]
 00
00501C89: jmp dword ptr [eax*4+00501DB0h]
 00
00501C90: mov bl,1
00501C92: jmp 00501C96
00501C94: xor bl,bl
00501C96: movsx eax,byte ptr [esi+00000080h]
 00
00501C9D: sub eax,4
00501CA0: je 00501D50
00501CA6: dec eax
00501CA7: je 00501CDF
00501CA9: dec eax
00501CAA: jne 00501DAA
00501CB0: mov al,byte ptr [esi+000000A8h]
00501CB6: test al,al
00501CB8: jne 00501DAA
00501CBE: mov al,byte ptr [esi+000000A5h]
00501CC4: test al,al
00501CC6: push ecx
00501CC7: mov ecx,esi
00501CC9: je 00501CD5
00501CCB: call 004FF320
00501CD0: pop ebx
00501CD1: pop esi
00501CD2: ret 4
00501CD5: call 004FF7C0
00501CDA: pop ebx
00501CDB: pop esi
00501CDC: ret 4
00501CDF: mov al,byte ptr [esi+000000A8h]
00501CE5: test al,al
00501CE7: jne 00501DAA
00501CED: mov al,byte ptr [esi+000000A5h]
00501CF3: test al,al
00501CF5: je 00501D12
00501CF7: test bl,bl
00501CF9: push ecx
00501CFA: mov ecx,esi
00501CFC: je 00501D08
00501CFE: call 00501650
00501D03: pop ebx
00501D04: pop esi
00501D05: ret 4
00501D08: call 004FD290
00501D0D: pop ebx
00501D0E: pop esi
00501D0F: ret 4
00501D12: test bl,bl
00501D14: je 00501D23
00501D16: push ecx
00501D17: mov ecx,esi
00501D19: call 00501960
00501D1E: pop ebx
00501D1F: pop esi
00501D20: ret 4
00501D23: mov al,byte ptr [edx+36h]
00501D26: and al,0F8h
00501D28: cmp al,18h
00501D2A: je 00501D43
00501D2C: mov al,byte ptr [edx+00000424h]
00501D32: test al,al
00501D34: jge 00501D43
00501D36: push ecx
00501D37: mov ecx,esi
00501D39: call 004FD8F0
00501D3E: pop ebx
00501D3F: pop esi
00501D40: ret 4
00501D43: push ecx
00501D44: mov ecx,esi
00501D46: call 004FDFD0
00501D4B: pop ebx
00501D4C: pop esi
00501D4D: ret 4
00501D50: mov al,byte ptr [esi+000000A8h]
00501D56: test al,al
00501D58: jne 00501DAA
00501D5A: mov al,byte ptr ds:[00B6B994h]
00501D5F: test al,al
00501D61: jne 00501DA3
00501D63: mov al,byte ptr [esi+000000BDh]
00501D69: test al,al
00501D6B: jne 00501DA3
00501D6D: mov al,byte ptr [esi+000000A5h]
00501D73: test al,al
00501D75: push ecx
00501D76: mov ecx,esi
00501D78: je 00501D84
00501D7A: call 004FE420
00501D7F: pop ebx
00501D80: pop esi
00501D81: ret 4
00501D84: mov dl,byte ptr [edx+36h]
00501D87: and dl,0F8h
00501D8A: cmp dl,18h
00501D8D: jne 00501D99
00501D8F: call 004FEE20
00501D94: pop ebx
00501D95: pop esi
00501D96: ret 4
00501D99: call 004FE940
00501D9E: pop ebx
00501D9F: pop esi
00501DA0: ret 4
00501DA3: mov ecx,esi
00501DA5: call 004FD0B0
00501DAA: pop ebx
00501DAB: pop esi
00501DAC: ret 4
00501DB1: sbb al,50h
00501DB3: add byte ptr [esp+ebx+00000050h],dl
 00
00501DBA: add dword ptr [ecx],eax
00501DBC: add dword ptr [ecx],eax
00501DBE: add dword ptr [ecx],eax
00501DC0: add dword ptr [ecx],eax
00501DC2: add dword ptr [ecx],eax
00501DC4: add dword ptr [ecx],eax
00501DC6: add dword ptr [ecx],eax
00501DC8: add dword ptr [ecx],eax
00501DCA: add dword ptr [ecx],eax
00501DCC: add dword ptr [ecx],eax
00501DCE: add dword ptr [ecx],eax
00501DD0: add dword ptr [ecx],eax
00501DD2: add dword ptr [ecx],eax
00501DD4: add dword ptr [ecx],eax
00501DD6: add dword ptr [ecx],eax
00501DD8: add dword ptr [ecx],eax
00501DDA: add dword ptr [ecx],eax
00501DDC: add dword ptr [ecx],eax
00501DDE: add dword ptr [ecx],eax
00501DE0: add dword ptr [ecx],eax
00501DE2: add dword ptr [ecx],eax
00501DE4: add dword ptr [ecx],eax
00501DE6: add dword ptr [ecx],eax
00501DE8: add dword ptr [ecx],eax
00501DEA: add dword ptr [ecx],eax
00501DEC: add dword ptr [ecx],eax
00501DEE: add dword ptr [ecx],eax
00501DF0: add dword ptr [ecx],eax
00501DF2: add byte ptr [ecx],al
00501DF4: add dword ptr [ecx],eax
00501DF6: add dword ptr [ecx],eax
00501DF8: add dword ptr [ecx],eax
00501DFA: add dword ptr [ecx],eax
00501DFC: add dword ptr [ecx],eax
00501DFE: add dword ptr [ecx],eax
00501E00: add dword ptr [eax],eax
00501E10: sub esp,4Ch
00501E13: push ebx
00501E14: xor ebx,ebx
00501E16: push esi
00501E17: or eax,0FFFFFFFFh
00501E1A: push edi
00501E1B: mov edi,dword ptr [esp+5Ch]
00501E1F: mov dword ptr [esp+0Ch],eax
00501E23: mov dword ptr [esp+10h],eax
00501E27: mov byte ptr [esp+14h],bl
00501E2B: mov dword ptr [esp+18h],ebx
00501E2F: mov dword ptr [esp+20h],ebx
00501E33: mov dword ptr [esp+24h],ebx
00501E37: mov dword ptr [esp+28h],ebx
00501E3B: mov word ptr [esp+2Ch],bx
00501E40: mov word ptr [esp+2Eh],bx
00501E45: mov dword ptr [esp+30h],ebx
00501E49: mov dword ptr [esp+38h],ebx
00501E4D: mov dword ptr [esp+3Ch],ebx
00501E51: mov dword ptr [esp+40h],ebx
00501E55: mov byte ptr [esp+44h],bl
00501E59: mov byte ptr [esp+45h],bl
00501E5D: mov dword ptr [esp+48h],ebx
00501E61: mov dword ptr [esp+4Ch],ebx
00501E65: mov dword ptr [esp+1Ch],edi
00501E69: mov eax,dword ptr [edi+00000384h]
00501E6F: cmp eax,ebx
00501E71: mov esi,ecx
00501E73: je 00501E7E
00501E75: add eax,2Ch
00501E78: mov dword ptr [esp+20h],eax
00501E7C: jmp 00501E82
00501E7E: mov dword ptr [esp+20h],ebx
00501E82: movsx eax,word ptr [edi+22h]
00501E86: sub eax,190h
00501E8B: mov dword ptr [esp+24h],eax
00501E8F: mov cl,byte ptr [edi+36h]
00501E92: and cl,0F8h
00501E95: cmp cl,10h
00501E98: jne 00501EA8
00501E9A: fld dword ptr [edi+000003C8h]
00501EA0: fmul dword ptr ds:[00858B38h]
00501EA6: jmp 00501EC3
00501EA8: mov eax,dword ptr [edi+14h]
00501EAB: fld dword ptr [edi+4Ch]
00501EAE: fmul dword ptr [eax+18h]
00501EB1: add eax,10h
00501EB4: fld dword ptr [edi+48h]
00501EB7: fmul dword ptr [eax+4]
00501EBA: faddp st(1),st
00501EBC: fld dword ptr [edi+44h]
00501EBF: fmul dword ptr [eax]
00501EC1: faddp st(1),st
00501EC3: movsx ecx,byte ptr [esi+00000080h]
 00
00501ECA: fstp dword ptr [esp+28h]
00501ECE: cmp ecx,9
00501ED1: mov eax,dword ptr [edi+00000590h]
00501ED7: mov dword ptr [esp+10h],eax
00501EDB: mov edx,dword ptr [edi+00000594h]
00501EE1: mov dword ptr [esp+0Ch],edx
00501EE5: ja 00502240
00501EEB: jmp dword ptr [ecx*4+0050224Ch]
 00
00501EF2: lea eax,[esp+0Ch]
00501EF6: push eax
00501EF7: mov ecx,esi
00501EF9: call 00501AB0
00501EFE: pop edi
00501EFF: pop esi
00501F00: pop ebx
00501F01: add esp,4Ch
00501F04: ret 4
00501F07: lea ecx,[esp+0Ch]
00501F0B: push ecx
00501F0C: mov ecx,esi
00501F0E: call 005003A0
00501F13: lea edx,[esp+0Ch]
00501F17: push edx
00501F18: mov ecx,esi
00501F1A: call 004F92C0
00501F1F: lea eax,[esp+0Ch]
00501F23: push eax
00501F24: mov ecx,esi
00501F26: call 004FA0C0
00501F2B: pop edi
00501F2C: pop esi
00501F2D: pop ebx
00501F2E: add esp,4Ch
00501F31: ret 4
00501F34: lea ecx,[esp+0Ch]
00501F38: push ecx
00501F39: mov ecx,esi
00501F3B: call 004FA3F0
00501F40: cmp byte ptr [esi+000000A5h],bl
00501F46: je 00501F69
00501F48: lea edx,[esp+0Ch]
00501F4C: push edx
00501F4D: mov ecx,esi
00501F4F: call 00500710
00501F54: lea eax,[esp+0Ch]
00501F58: push eax
00501F59: mov ecx,esi
00501F5B: call 00500AB0
00501F60: pop edi
00501F61: pop esi
00501F62: pop ebx
00501F63: add esp,4Ch
00501F66: ret 4
00501F69: lea ecx,[esp+0Ch]
00501F6D: push ecx
00501F6E: mov ecx,esi
00501F70: call 00500710
00501F75: pop edi
00501F76: pop esi
00501F77: pop ebx
00501F78: add esp,4Ch
00501F7B: ret 4
00501F7E: cmp byte ptr [esi+000000A8h],1
 01
00501F85: je 00502240
00501F8B: push eax
00501F8C: push edi
00501F8D: mov ecx,esi
00501F8F: call 004F5EB0
00501F94: lea edx,[esp+0Ch]
00501F98: push edx
00501F99: mov ecx,esi
00501F9B: call 004F8B00
00501FA0: mov eax,dword ptr [esp+1Ch]
00501FA4: mov cl,byte ptr [eax+36h]
00501FA7: and cl,0F8h
00501FAA: cmp cl,10h
00501FAD: je 00501FBB
00501FAF: lea edx,[esp+0Ch]
00501FB3: push edx
00501FB4: mov ecx,esi
00501FB6: call 004F8F10
00501FBB: lea eax,[esp+0Ch]
00501FBF: push eax
00501FC0: mov ecx,esi
00501FC2: call 005002C0
00501FC7: cmp byte ptr [esi+000000A5h],bl
00501FCD: je 00501FDD
00501FCF: lea ecx,[esp+0Ch]
00501FD3: push ecx
00501FD4: mov ecx,esi
00501FD6: call 004FBB10
00501FDB: jmp 00501FE9
00501FDD: lea edx,[esp+0Ch]
00501FE1: push edx
00501FE2: mov ecx,esi
00501FE4: call 00501480
00501FE9: mov eax,dword ptr [esp+1Ch]
00501FED: mov cl,byte ptr [eax+36h]
00501FF0: and cl,0F8h
00501FF3: cmp cl,10h
00501FF6: je 00502004
00501FF8: lea edx,[esp+0Ch]
00501FFC: push edx
00501FFD: mov ecx,esi
00501FFF: call 004FAE20
00502004: mov eax,dword ptr [esp+1Ch]
00502008: mov cl,byte ptr [eax+36h]
0050200B: and cl,0F8h
0050200E: cmp cl,10h
00502011: je 0050201F
00502013: lea edx,[esp+0Ch]
00502017: push edx
00502018: mov ecx,esi
0050201A: call 004F8940
0050201F: lea eax,[esp+0Ch]
00502023: push eax
00502024: mov ecx,esi
00502026: call 004F92C0
0050202B: mov ecx,dword ptr [esp+28h]
0050202F: mov dword ptr [edi+000007B8h],ecx
00502035: pop edi
00502036: pop esi
00502037: pop ebx
00502038: add esp,4Ch
0050203B: ret 4
0050203E: cmp byte ptr [esi+000000A8h],1
 01
00502045: je 00502240
0050204B: push eax
0050204C: push edi
0050204D: mov ecx,esi
0050204F: call 004F5EB0
00502054: lea edx,[esp+0Ch]
00502058: push edx
00502059: mov ecx,esi
0050205B: call 004F8F10
00502060: lea eax,[esp+0Ch]
00502064: push eax
00502065: mov ecx,esi
00502067: call 005002C0
0050206C: cmp byte ptr [esi+000000A5h],bl
00502072: je 00502093
00502074: lea ecx,[esp+0Ch]
00502078: push ecx
00502079: mov ecx,esi
0050207B: call 00500040
00502080: mov eax,dword ptr [esp+28h]
00502084: mov dword ptr [edi+000007B8h],eax
0050208A: pop edi
0050208B: pop esi
0050208C: pop ebx
0050208D: add esp,4Ch
00502090: ret 4
00502093: lea edx,[esp+0Ch]
00502097: push edx
00502098: mov ecx,esi
0050209A: call 004FFDC0
0050209F: mov eax,dword ptr [esp+28h]
005020A3: mov dword ptr [edi+000007B8h],eax
005020A9: pop edi
005020AA: pop esi
005020AB: pop ebx
005020AC: add esp,4Ch
005020AF: ret 4
005020B2: push eax
005020B3: push edi
005020B4: mov ecx,esi
005020B6: call 004F5EB0
005020BB: lea ecx,[esp+0Ch]
005020BF: push ecx
005020C0: mov ecx,esi
005020C2: call 00501C50
005020C7: lea edx,[esp+0Ch]
005020CB: push edx
005020CC: mov ecx,esi
005020CE: call 004F92C0
005020D3: lea eax,[esp+0Ch]
005020D7: push eax
005020D8: mov ecx,esi
005020DA: call 004FB260
005020DF: pop edi
005020E0: pop esi
005020E1: pop ebx
005020E2: add esp,4Ch
005020E5: ret 4
005020E8: push eax
005020E9: push edi
005020EA: mov ecx,esi
005020EC: call 004F5EB0
005020F1: lea ecx,[esp+0Ch]
005020F5: push ecx
005020F6: mov ecx,esi
005020F8: call 00501C50
005020FD: lea edx,[esp+0Ch]
00502101: push edx
00502102: mov ecx,esi
00502104: call 004F92C0
00502109: pop edi
0050210A: pop esi
0050210B: pop ebx
0050210C: add esp,4Ch
0050210F: ret 4
00502112: lea eax,[esp+0Ch]
00502116: push eax
00502117: mov ecx,esi
00502119: call 00501C50
0050211E: lea ecx,[esp+0Ch]
00502122: push ecx
00502123: mov ecx,esi
00502125: call 004F92C0
0050212A: lea edx,[esp+0Ch]
0050212E: push edx
0050212F: mov ecx,esi
00502131: call 004FA0C0
00502136: pop edi
00502137: pop esi
00502138: pop ebx
00502139: add esp,4Ch
0050213C: ret 4
0050213F: push eax
00502140: push edi
00502141: mov ecx,esi
00502143: call 004F5EB0
00502148: cmp byte ptr [esi+000000A8h],1
 01
0050214F: je 00502240
00502155: lea eax,[esp+0Ch]
00502159: push eax
0050215A: mov ecx,esi
0050215C: call 004F8B00
00502161: cmp byte ptr [esi+000000A5h],bl
00502167: je 00502175
00502169: lea ecx,[esp+0Ch]
0050216D: push ecx
0050216E: mov ecx,esi
00502170: call 004F8DF0
00502175: mov edx,dword ptr [esp+1Ch]
00502179: mov al,byte ptr [edx+36h]
0050217C: and al,0F8h
0050217E: cmp al,10h
00502180: je 0050218E
00502182: lea ecx,[esp+0Ch]
00502186: push ecx
00502187: mov ecx,esi
00502189: call 004F8F10
0050218E: mov edx,dword ptr [esp+1Ch]
00502192: mov al,byte ptr [edx+36h]
00502195: and al,0F8h
00502197: cmp al,10h
00502199: je 005021A7
0050219B: lea ecx,[esp+0Ch]
0050219F: push ecx
005021A0: mov ecx,esi
005021A2: call 004F8940
005021A7: lea edx,[esp+0Ch]
005021AB: push edx
005021AC: mov ecx,esi
005021AE: call 005002C0
005021B3: cmp byte ptr [esi+000000A5h],bl
005021B9: je 005021C9
005021BB: lea eax,[esp+0Ch]
005021BF: push eax
005021C0: mov ecx,esi
005021C2: call 004FBB10
005021C7: jmp 005021D5
005021C9: lea ecx,[esp+0Ch]
005021CD: push ecx
005021CE: mov ecx,esi
005021D0: call 00501480
005021D5: mov edx,dword ptr [esp+1Ch]
005021D9: mov al,byte ptr [edx+36h]
005021DC: and al,0F8h
005021DE: cmp al,10h
005021E0: je 005021EE
005021E2: lea ecx,[esp+0Ch]
005021E6: push ecx
005021E7: mov ecx,esi
005021E9: call 004FAE20
005021EE: lea edx,[esp+0Ch]
005021F2: push edx
005021F3: mov ecx,esi
005021F5: call 004F92C0
005021FA: mov eax,dword ptr [esp+1Ch]
005021FE: cmp dword ptr [eax+00000590h],ebx
00502204: jne 00502212
00502206: lea ecx,[esp+0Ch]
0050220A: push ecx
0050220B: mov ecx,esi
0050220D: call 004FB070
00502212: lea edx,[esp+0Ch]
00502216: push edx
00502217: mov ecx,esi
00502219: call 004FB260
0050221E: mov eax,dword ptr [esp+1Ch]
00502222: cmp word ptr [eax+22h],214h
00502228: jne 00502236
0050222A: lea ecx,[esp+0Ch]
0050222E: push ecx
0050222F: mov ecx,esi
00502231: call 00500CE0
00502236: mov edx,dword ptr [esp+28h]
0050223A: mov dword ptr [edi+000008B0h],edx
00502240: pop edi
00502241: pop esi
00502242: pop ebx
00502243: add esp,4Ch
00502246: ret 4
00502249: lea ecx,[ecx]
0050224C: aas
0050224D: and dword ptr [eax],edx
00502250: jle 00502271
00502252: push eax
00502253: add byte ptr [esi],bh
00502255: and byte ptr [eax],dl
00502258: pop es
00502259: pop ds
0050225A: push eax
0050225B: add al,ch
0050225D: and byte ptr [eax],dl
00502260: mov dl,20h
00502262: push eax
00502263: add byte ptr [edx],dl
00502265: and dword ptr [eax],edx
00502268: inc eax
00502269: and dl,byte ptr [eax]
0050226C: xor al,1Fh
0050226E: push eax
0050226F: add dl,dh
00502271: push ds
00502272: push eax
00502273: add byte ptr [eax+90909090h],dl
00502280: push ecx
00502281: push esi
00502282: mov esi,ecx
00502284: mov al,byte ptr [esi+000000A4h]
0050228A: test al,al
0050228C: je 005023C5
00502292: mov al,byte ptr [esi+000000A5h]
00502298: test al,al
0050229A: push ebx
0050229B: push edi
0050229C: mov edi,dword ptr [esi+4]
0050229F: je 005022CE
005022A1: mov ecx,dword ptr ds:[00B6B990h]
005022A7: test ecx,ecx
005022A9: je 005022CE
005022AB: mov al,byte ptr [esi+000000A7h]
005022B1: test al,al
005022B3: jne 005022CE
005022B5: cmp dword ptr [ecx+00000530h],3Fh
 3F
005022BC: je 005022C7
005022BE: call 005E0170
005022C3: test al,al
005022C5: jne 005022CE
005022C7: mov ecx,esi
005022C9: call 004F5BA0
005022CE: mov eax,dword ptr [esi+4]
005022D1: mov cl,byte ptr [eax+36h]
005022D4: and cl,0F8h
005022D7: cmp cl,28h
005022DA: sete bl
005022DD: test bl,bl
005022DF: je 005022F2
005022E1: mov al,byte ptr [esi+000000A8h]
005022E7: test al,al
005022E9: jne 005022F2
005022EB: mov ecx,esi
005022ED: call 004FD0B0
005022F2: mov byte ptr [esi+000000A8h],bl
005022F8: mov ecx,dword ptr [edi+00000460h]
005022FE: test ecx,ecx
 00502300: 74
