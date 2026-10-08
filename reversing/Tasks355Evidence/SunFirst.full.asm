00639CB0: push esi
00639CB1: push edi
00639CB2: mov esi,ecx
00639CB4: call 00821B1E
00639CB9: mov edi,dword ptr [esp+0Ch]
00639CBD: cmp dword ptr [edi+00000598h],5
 05
00639CC4: jne 00639CF5
00639CC6: call 00821B1E
00639CCB: and eax,0FFFFh
00639CD0: mov dword ptr [esp+0Ch],eax
00639CD4: fild dword ptr [esp+0Ch]
00639CD8: fmul dword ptr ds:[00858B14h]
00639CDE: fmul dword ptr ds:[00859024h]
00639CE4: call 00821B40
00639CE9: mov ecx,2
00639CEE: sub ecx,eax
00639CF0: mov dword ptr [esi+20h],ecx
00639CF3: jmp 00639D22
00639CF5: movsx edx,word ptr [edi+22h]
00639CF9: push edx
00639CFA: call 00611760
00639CFF: add esp,4
00639D02: test al,al
00639D04: je 00639D1B
00639D06: call 00821B1E
00639D0B: cmp eax,3FFFh
00639D10: setl al
00639D13: movzx eax,al
00639D16: mov dword ptr [esi+20h],eax
00639D19: jmp 00639D22
00639D1B: mov dword ptr [esi+20h],1
 00
00639D22: mov ecx,dword ptr [esi+24h]
00639D25: mov al,byte ptr [ecx+10h]
00639D28: test al,al
00639D2A: jne 00639D40
00639D2C: mov edx,dword ptr [esi+2Ch]
00639D2F: add edx,63E7h
00639D35: push 8
00639D37: push edx
00639D38: call 004087E0
00639D3D: add esp,8
00639D40: mov eax,dword ptr [esi+28h]
00639D43: mov cl,byte ptr [eax+10h]
00639D46: test cl,cl
00639D48: jne 00639D5E
00639D4A: mov ecx,dword ptr [esi+30h]
00639D4D: add ecx,63E7h
00639D53: push 8
00639D55: push ecx
00639D56: call 004087E0
00639D5B: add esp,8
00639D5E: mov ecx,esi
00639D60: mov byte ptr [esi+0Dh],0
00639D64: call 00632190
00639D69: mov cl,byte ptr [esi+0Ch]
00639D6C: test cl,cl
00639D6E: je 00639D97
00639D70: test al,al
00639D72: je 00639D9B
00639D74: mov eax,dword ptr [esi+34h]
00639D77: test eax,eax
00639D79: jne 00639DBA
00639D7B: mov edx,dword ptr [esi+28h]
00639D7E: mov al,byte ptr [edx+10h]
00639D81: test al,al
00639D83: je 00639DBA
00639D85: push edi
00639D86: push 1ACh
00639D8B: mov ecx,esi
00639D8D: call 00638290
00639D92: pop edi
00639D93: pop esi
00639D94: ret 4
00639D97: test al,al
00639D99: jne 00639DA5
00639D9B: mov ecx,dword ptr [esi+24h]
00639D9E: mov dl,byte ptr [ecx+10h]
00639DA1: test dl,dl
00639DA3: je 00639DB3
00639DA5: mov edx,dword ptr [esi+24h]
00639DA8: mov cl,byte ptr [edx+10h]
00639DAB: test cl,cl
00639DAD: jne 00639DCC
00639DAF: test al,al
00639DB1: jne 00639DBA
00639DB3: pop edi
00639DB4: xor eax,eax
00639DB6: pop esi
00639DB7: ret 4
00639DBA: push edi
00639DBB: push 0CAh
00639DC0: mov ecx,esi
00639DC2: call 00638290
00639DC7: pop edi
00639DC8: pop esi
00639DC9: ret 4
00639DCC: call 00821B1E
00639DD1: and eax,0FFFFh
00639DD6: mov dword ptr [esp+0Ch],eax
00639DDA: fild dword ptr [esp+0Ch]
00639DDE: mov eax,dword ptr ds:[00B7CB84h]
00639DE3: mov dword ptr [esi+14h],eax
00639DE6: fmul dword ptr ds:[00858B14h]
00639DEC: fmul dword ptr ds:[0086E668h]
00639DF2: call 00821B40
00639DF7: mov ecx,4E20h
00639DFC: sub ecx,eax
00639DFE: mov dword ptr [esi+18h],ecx
00639E01: push edi
00639E02: mov byte ptr [esi+1Ch],1
00639E06: push 1A2h
00639E0B: mov ecx,esi
00639E0D: mov byte ptr [esi+0Dh],1
00639E11: call 00638290
00639E16: pop edi
00639E17: pop esi
00639E18: ret 4
00639E20: push esi
