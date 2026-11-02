// Cuerpo de CCarCtrl::GenerateOneRandomCar (0x430050..0x431F3A) generado por reversing/Car84L/tools/lift.py a partir del desensamblado del EXE.
// Es una transcripcion instruccion a instruccion, no una reconstruccion legible: registros enteros como variables locales, marco de pila `F` (los punteros
// a locales que reciben los colaboradores apuntan a F), pila x87 como x0..x7 (cada operacion pasa por Veh83D*: mantisa de 24 bits, exponente ampliado) y
// los saltos como goto. Los colaboradores se llaman por su direccion del EXE (los que tienen hook RH ejecutan el C++ de la DLL).
// Para regenerarlo: python reversing/Car84L/tools/lift_emit.py source/game_sa/CarCtrlGenerateOneRandomCar.inl
    // 00430050: sub esp,12Ch
    // 00430056: push ebx
    St<uint32>(F+448,ebx);
    // 00430057: mov eax,0FFFFh
    eax=(uint32)(0xFFFFu);
    // 0043005C: push ebp
    St<uint32>(F+444,ebp);
    // 0043005D: mov word ptr [esp+44h],ax
    St<uint16>((F+512),(uint16)((eax&0xFFFFu)));
    // 00430062: mov word ptr [esp+40h],ax
    St<uint16>((F+508),(uint16)((eax&0xFFFFu)));
    // 00430067: movzx eax,byte ptr ds:[00B7CD74h]
    eax=(uint32)(Ld<uint8>((uintptr_t)(uint32)(0xB7CD74u)));
    // 0043006E: push esi
    St<uint32>(F+440,esi);
    // 0043006F: push edi
    St<uint32>(F+436,edi);
    // 00430070: push eax
    St<uint32>(F+432,eax);
    // 00430071: mov byte ptr [esp+3Bh],0
    St<uint8>((F+491),(uint8)(0x0u));
    // 00430076: mov byte ptr [esp+4Bh],0
    St<uint8>((F+507),(uint8)(0x0u));
    // 0043007B: mov byte ptr [esp+3Ah],0
    St<uint8>((F+490),(uint8)(0x0u));
    // 00430080: call 0056E250
    eax=((uint32(__cdecl*)(uint32))0x56E250)(Ld<uint32>(F+432));
    // 00430085: mov esi,dword ptr [eax]
    esi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00430087: mov ebx,dword ptr [eax+4]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 0043008A: mov ebp,dword ptr [eax+8]
    ebp=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 0043008D: push 0FFFFFFFFh
    St<uint32>(F+428,0xFFFFFFFFu);
    // 0043008F: mov dword ptr [esp+000000CCh],esi
    St<uint32>((F+632),(uint32)(esi));
    // 00430096: mov dword ptr [esp+000000D0h],ebx
    St<uint32>((F+636),(uint32)(ebx));
    // 0043009D: mov dword ptr [esp+000000D4h],ebp
    St<uint32>((F+640),(uint32)(ebp));
    // 004300A4: call 0056E090
    eax=((uint32(__cdecl*)(uint32))0x56E090)(Ld<uint32>(F+428));
    // 004300A9: mov ecx,dword ptr [eax]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 004300AB: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 004300AE: mov eax,dword ptr [eax+8]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 004300B1: mov edi,dword ptr ds:[00969094h]
    edi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x969094u)));
    // 004300B7: mov dword ptr [esp+000000B8h],edx
    St<uint32>((F+612),(uint32)(edx));
    // 004300BE: mov edx,dword ptr ds:[009690ACh]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x9690ACu)));
    // 004300C4: mov dword ptr [esp+000000B4h],ecx
    St<uint32>((F+608),(uint32)(ecx));
    // 004300CB: mov ecx,dword ptr ds:[009690A8h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x9690A8u)));
    // 004300D1: add edx,ecx
    {const uint32 a_=(edx)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004300D3: mov ecx,dword ptr ds:[0096909Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x96909Cu)));
    // 004300D9: mov dword ptr [esp+000000BCh],eax
    St<uint32>((F+616),(uint32)(eax));
    // 004300E0: mov eax,dword ptr ds:[00969098h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x969098u)));
    // 004300E5: add edx,ecx
    {const uint32 a_=(edx)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004300E7: add edx,eax
    {const uint32 a_=(edx)&4294967295,b_=(eax)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004300E9: mov eax,dword ptr ds:[008A5B20h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x8A5B20u)));
    // 004300EE: add edx,edi
    {const uint32 a_=(edx)&4294967295,b_=(edi)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004300F0: add esp,8
    // 004300F3: mov dword ptr [esp+10h],edx
    St<uint32>((F+452),(uint32)(edx));
    // 004300F7: mov dword ptr [esp+14h],eax
    St<uint32>((F+456),(uint32)(eax));
    // 004300FB: call 0072DD80
    eax=((uint8(__cdecl*)(void))0x72DD80)();
    // 00430100: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430102: je 00430112
    if(zf) goto L_00430112;
    // 00430104: fld dword ptr [esp+14h]
    x0=(double)Ld<float>((F+456));
    // 00430108: fmul dword ptr ds:[00858CC8h]
    x0=Veh83DMul(x0,(double)F32(0x3F19999Au));
    // 0043010E: fstp dword ptr [esp+14h]
    St<float>((F+456),Veh83DStore(x0));
L_00430112:;
    // 00430112: fild dword ptr [esp+10h]
    x0=(double)Ld<int32>((F+452));
    // 00430116: fstp dword ptr [esp+54h]
    St<float>((F+520),Veh83DStore(x0));
    // 0043011A: call 00611B60
    x0=(double)((float(__cdecl*)(void))0x611B60)();
    // 0043011F: fimul dword ptr ds:[008A5B24h]
    x0=Veh83DMul(x0,(double)Ld<int32>((uintptr_t)(uint32)(0x8A5B24u)));
    // 00430125: fmul dword ptr [esp+14h]
    x0=Veh83DMul(x0,(double)Ld<float>((F+456)));
    // 00430129: fcomp dword ptr [esp+54h]
    fcc=X87Compare(x0,(double)Ld<float>((F+520)));
    // 0043012D: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 0043012F: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430132: jnp 00431AAD
    if(!pf) goto L_00431AAD;
    // 00430138: call 00611B60
    x0=(double)((float(__cdecl*)(void))0x611B60)();
    // 0043013D: fld dword ptr ds:[00C0BC30h]
    x1=(double)Ld<float>((uintptr_t)(uint32)(0xC0BC30u));
    // 00430143: fadd dword ptr ds:[00C0BC34h]
    x1=Veh83DAdd(x1,(double)Ld<float>((uintptr_t)(uint32)(0xC0BC34u)));
    // 00430149: fadd dword ptr ds:[00C0BC38h]
    x1=Veh83DAdd(x1,(double)Ld<float>((uintptr_t)(uint32)(0xC0BC38u)));
    // 0043014F: fadd dword ptr ds:[00C0BC3Ch]
    x1=Veh83DAdd(x1,(double)Ld<float>((uintptr_t)(uint32)(0xC0BC3Cu)));
    // 00430155: fmulp st(1),st
    x0=Veh83DMul(x0,x1);
    // 00430157: fmul dword ptr [esp+14h]
    x0=Veh83DMul(x0,(double)Ld<float>((F+456)));
    // 0043015B: fcomp dword ptr [esp+54h]
    fcc=X87Compare(x0,(double)Ld<float>((F+520)));
    // 0043015F: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430161: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430164: jnp 00431AAD
    if(!pf) goto L_00431AAD;
    // 0043016A: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 0043016C: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 00430171: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 00430174: add esp,4
    // 00430177: cmp ecx,1
    {const uint32 a_=(ecx)&4294967295,b_=(0x1u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043017A: jle 00430225
    if(zf||sf!=of) goto L_00430225;
    // 00430180: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 00430182: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 00430187: movzx ecx,byte ptr [eax+1Ah]
    ecx=(uint32)(Ld<uint8>((uintptr_t)(uint32)(eax+0x1Au)));
    // 0043018B: mov eax,dword ptr ds:[00969098h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x969098u)));
    // 00430190: add esp,4
    // 00430193: cmp eax,ecx
    {const uint32 a_=(eax)&4294967295,b_=(ecx)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430195: jge 00430225
    if(sf==of) goto L_00430225;
    // 0043019B: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 0043019D: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 004301A2: push 0FFFFFFFFh
    St<uint32>(F+428,0xFFFFFFFFu);
    // 004301A4: mov edi,eax
    edi=(uint32)(eax);
    // 004301A6: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+428));
    // 004301AB: mov dl,byte ptr [eax+18h]
    edx=(edx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(eax+0x18u)))&0xFFu);
    // 004301AE: mov al,byte ptr [edi+19h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(edi+0x19u)))&0xFFu);
    // 004301B1: add esp,8
    // 004301B4: cmp dl,al
    {const uint32 a_=((edx&0xFFu))&255,b_=((eax&0xFFu))&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 004301B6: jae 00430225
    if(!cf) goto L_00430225;
    // 004301B8: mov eax,dword ptr ds:[00B72914h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB72914u)));
    // 004301BD: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 004301BF: jne 00430225
    if(!zf) goto L_00430225;
    // 004301C1: call 00443AC0
    eax=((uint8(__cdecl*)(void))0x443AC0)();
    // 004301C6: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004301C8: jne 00430225
    if(!zf) goto L_00430225;
    // 004301CA: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 004301CC: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 004301D1: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 004301D4: add esp,4
    // 004301D7: cmp ecx,3
    {const uint32 a_=(ecx)&4294967295,b_=(0x3u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004301DA: jg 0043020C
    if(!zf&&sf==of) goto L_0043020C;
    // 004301DC: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 004301DE: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 004301E3: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 004301E6: mov eax,dword ptr ds:[009690B8h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0x9690B8u)));
    // 004301EB: add esp,4
    // 004301EE: cmp ecx,2
    {const uint32 a_=(ecx)&4294967295,b_=(0x2u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004301F1: mov ecx,dword ptr ds:[00B7CB84h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB84u)));
    // 004301F7: jle 00430203
    if(zf||sf!=of) goto L_00430203;
    // 004301F9: lea edx,[eax+00001388h]
    edx=(uint32)((uint32)(uintptr_t)(uint32)(eax+0x1388u));
    // 004301FF: cmp ecx,edx
    {const uint32 a_=(ecx)&4294967295,b_=(edx)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430201: ja 0043020C
    if(!cf&&!zf) goto L_0043020C;
L_00430203:;
    // 00430203: add eax,1F40h
    {const uint32 a_=(eax)&4294967295,b_=(0x1F40u)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430208: cmp ecx,eax
    {const uint32 a_=(ecx)&4294967295,b_=(eax)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043020A: jbe 00430225
    if(cf||zf) goto L_00430225;
L_0043020C:;
    // 0043020C: push 0
    St<uint32>(F+432,0x0u);
    // 0043020E: call 00421980
    eax=((uint32(__cdecl*)(uint32))0x421980)(Ld<uint32>(F+432));
    // 00430213: mov edi,0Dh
    edi=(uint32)(0xDu);
    // 00430218: add esp,4
    // 0043021B: mov dword ptr [esp+28h],eax
    St<uint32>((F+476),(uint32)(eax));
    // 0043021F: mov dword ptr [esp+58h],edi
    St<uint32>((F+524),(uint32)(edi));
    // 00430223: jmp 00430263
    goto L_00430263;
L_00430225:;
    // 00430225: lea eax,[esp+58h]
    eax=(uint32)((uint32)(F+524));
    // 00430229: push eax
    St<uint32>(F+432,eax);
    // 0043022A: call 00424CE0
    eax=((uint32(__cdecl*)(uint32))0x424CE0)(Ld<uint32>(F+432));
    // 0043022F: add esp,4
    // 00430232: cmp eax,0FFFFFFFFh
    {const uint32 a_=(eax)&4294967295,b_=(0xFFFFFFFFu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430235: mov dword ptr [esp+28h],eax
    St<uint32>((F+476),(uint32)(eax));
    // 00430239: je 00431AAD
    if(zf) goto L_00431AAD;
    // 0043023F: mov edi,dword ptr [esp+58h]
    edi=(uint32)(Ld<uint32>((F+524)));
    // 00430243: cmp edi,0Dh
    {const uint32 a_=(edi)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430246: je 0043024D
    if(zf) goto L_0043024D;
    // 00430248: cmp edi,18h
    {const uint32 a_=(edi)&4294967295,b_=(0x18u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043024B: jne 00430263
    if(!zf) goto L_00430263;
L_0043024D:;
    // 0043024D: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 0043024F: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 00430254: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 00430257: add esp,4
    // 0043025A: cmp ecx,1
    {const uint32 a_=(ecx)&4294967295,b_=(0x1u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043025D: jge 00431AAD
    if(sf==of) goto L_00431AAD;
L_00430263:;
    // 00430263: call 00441C10
    eax=((uint8(__cdecl*)(void))0x441C10)();
    // 00430268: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043026A: je 00430298
    if(zf) goto L_00430298;
    // 0043026C: mov al,byte ptr ds:[00B72959h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0xB72959u)))&0xFFu);
    // 00430271: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430273: jne 00430298
    if(!zf) goto L_00430298;
    // 00430275: call 00821B1E
    eax=((uint32(__cdecl*)(void))0x821B1E)();
    // 0043027A: and eax,7Fh
    {const uint32 r_=((eax)&(0x7Fu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043027D: cmp al,37h
    {const uint32 a_=((eax&0xFFu))&255,b_=(0x37u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 0043027F: jge 00430298
    if(sf==of) goto L_00430298;
    // 00430281: push 0
    St<uint32>(F+432,0x0u);
    // 00430283: call 00421980
    eax=((uint32(__cdecl*)(uint32))0x421980)(Ld<uint32>(F+432));
    // 00430288: mov edi,0Dh
    edi=(uint32)(0xDu);
    // 0043028D: add esp,4
    // 00430290: mov dword ptr [esp+28h],eax
    St<uint32>((F+476),(uint32)(eax));
    // 00430294: mov dword ptr [esp+58h],edi
    St<uint32>((F+524),(uint32)(edi));
L_00430298:;
    // 00430298: fld dword ptr ds:[00B6F9B4h]
    x0=(double)Ld<float>((uintptr_t)(uint32)(0xB6F9B4u));
    // 0043029E: fcomp dword ptr ds:[00858CACh]
    fcc=X87Compare(x0,(double)F32(0xBF666666u));
    // 004302A4: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 004302A6: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004302A9: jp 004302CD
    if(pf) goto L_004302CD;
    // 004302AB: mov byte ptr [esp+37h],1
    St<uint8>((F+491),(uint8)(0x1u));
    // 004302B0: mov dword ptr [esp+24h],3F34FDF4h
    St<uint32>((F+472),(uint32)(0x3F34FDF4u));
    // 004302B8: mov dword ptr [esp+14h],3F34FDF4h
    St<uint32>((F+456),(uint32)(0x3F34FDF4u));
    // 004302C0: mov dword ptr [esp+20h],0BF800000h
    St<uint32>((F+468),(uint32)(0xBF800000u));
    // 004302C8: jmp 004303E1
    goto L_004303E1;
L_004302CD:;
    // 004302CD: push 0
    St<uint32>(F+432,0x0u);
    // 004302CF: push 0FFFFFFFFh
    St<uint32>(F+428,0xFFFFFFFFu);
    // 004302D1: call 0056E0D0
    eax=((uint32(__cdecl*)(uint32,uint32))0x56E0D0)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004302D6: add esp,8
    // 004302D9: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 004302DB: je 004303A6
    if(zf) goto L_004303A6;
    // 004302E1: fld dword ptr [eax+48h]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x48u));
    // 004302E4: mov ecx,dword ptr [eax+44h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x44u)));
    // 004302E7: fld st(0)
    x1=x0;
    // 004302E9: mov dword ptr [esp+14h],ecx
    St<uint32>((F+456),(uint32)(ecx));
    // 004302ED: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 004302EF: fld dword ptr [esp+14h]
    x2=(double)Ld<float>((F+456));
    // 004302F3: fmul dword ptr [esp+14h]
    x2=Veh83DMul(x2,(double)Ld<float>((F+456)));
    // 004302F7: faddp st(1),st
    x1=Veh83DAdd(x1,x2);
    // 004302F9: fsqrt
    x1=Veh83DSqrt(x1);
    // 004302FB: fcom dword ptr ds:[00858EE8h]
    fcc=X87Compare(x1,(double)F32(0x3ECCCCCDu));
    // 00430301: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430303: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430306: jne 0043034A
    if(!zf) goto L_0043034A;
    // 00430308: fdivr dword ptr ds:[00858624h]
    x1=Veh83DDiv((double)F32(0x3F800000u),x1);
    // 0043030E: mov eax,dword ptr ds:[00B7CB4Ch]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB4Cu)));
    // 00430313: and eax,3
    {const uint32 r_=((eax)&(0x3u))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430316: cmp eax,3
    {const uint32 a_=(eax)&4294967295,b_=(0x3u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430319: fld dword ptr [esp+14h]
    x2=(double)Ld<float>((F+456));
    // 0043031D: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 0043031F: fstp dword ptr [esp+14h]
    St<float>((F+456),Veh83DStore(x2));
    // 00430323: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 00430325: fstp dword ptr [esp+24h]
    St<float>((F+472),Veh83DStore(x1));
    // 00430329: fstp st(0)
    // 0043032B: ja 004303E6
    if(!cf&&!zf) goto L_004303E6;
    // 00430331: jmp dword ptr [eax*4+00431F3Ch]
    switch(eax){
    case 0: goto L_00430383;
    case 1: goto L_00430383;
    case 2: goto L_004303D9;
    case 3: goto L_00430338;
    default: __assume(0);}
L_00430338:;
    // 00430338: mov dword ptr [esp+20h],3F34FDF4h
    St<uint32>((F+468),(uint32)(0x3F34FDF4u));
    // 00430340: mov byte ptr [esp+2Ch],0
    St<uint8>((F+480),(uint8)(0x0u));
    // 00430345: jmp 004303E6
    goto L_004303E6;
L_0043034A:;
    // 0043034A: fcom dword ptr ds:[00858B1Ch]
    fcc=X87Compare(x1,(double)F32(0x3DCCCCCDu));
    // 00430350: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430352: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430355: jne 0043038D
    if(!zf) goto L_0043038D;
    // 00430357: fdivr dword ptr ds:[00858624h]
    x1=Veh83DDiv((double)F32(0x3F800000u),x1);
    // 0043035D: mov eax,dword ptr ds:[00B7CB4Ch]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB4Cu)));
    // 00430362: and eax,3
    {const uint32 r_=((eax)&(0x3u))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430365: cmp eax,3
    {const uint32 a_=(eax)&4294967295,b_=(0x3u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430368: fld dword ptr [esp+14h]
    x2=(double)Ld<float>((F+456));
    // 0043036C: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 0043036E: fstp dword ptr [esp+14h]
    St<float>((F+456),Veh83DStore(x2));
    // 00430372: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 00430374: fstp dword ptr [esp+24h]
    St<float>((F+472),Veh83DStore(x1));
    // 00430378: fstp st(0)
    // 0043037A: ja 004303E6
    if(!cf&&!zf) goto L_004303E6;
    // 0043037C: jmp dword ptr [eax*4+00431F4Ch]
    switch(eax){
    case 0: goto L_00430383;
    case 1: goto L_004303D9;
    case 2: goto L_00430338;
    case 3: goto L_00430338;
    default: __assume(0);}
L_00430383:;
    // 00430383: mov dword ptr [esp+20h],3F59999Ah
    St<uint32>((F+468),(uint32)(0x3F59999Au));
    // 0043038B: jmp 004303E1
    goto L_004303E1;
L_0043038D:;
    // 0043038D: mov edx,dword ptr ds:[00B6F104h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB6F104u)));
    // 00430393: fstp st(0)
    // 00430395: mov eax,dword ptr ds:[00B6F108h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB6F108u)));
    // 0043039A: fstp st(0)
    // 0043039C: mov dword ptr [esp+14h],edx
    St<uint32>((F+456),(uint32)(edx));
    // 004303A0: mov dword ptr [esp+24h],eax
    St<uint32>((F+472),(uint32)(eax));
    // 004303A4: jmp 004303BA
    goto L_004303BA;
L_004303A6:;
    // 004303A6: mov ecx,dword ptr ds:[00B6F104h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB6F104u)));
    // 004303AC: mov edx,dword ptr ds:[00B6F108h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB6F108u)));
    // 004303B2: mov dword ptr [esp+14h],ecx
    St<uint32>((F+456),(uint32)(ecx));
    // 004303B6: mov dword ptr [esp+24h],edx
    St<uint32>((F+472),(uint32)(edx));
L_004303BA:;
    // 004303BA: mov eax,dword ptr ds:[00B7CB4Ch]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB4Cu)));
    // 004303BF: and eax,1
    {const uint32 r_=((eax)&(0x1u))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004303C2: sub eax,0
    {const uint32 a_=(eax)&4294967295,b_=(0x0u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 004303C5: je 004303D9
    if(zf) goto L_004303D9;
    // 004303C7: dec eax
    {const uint32 a_=(eax)&4294967295,r_=(a_-1)&4294967295;zf=r_==0;sf=(r_>>31)&1;of=a_==2147483648u;pf=Parity(r_);eax=(uint32)(r_);}
    // 004303C8: jne 004303E6
    if(!zf) goto L_004303E6;
    // 004303CA: mov dword ptr [esp+20h],3F34FDF4h
    St<uint32>((F+468),(uint32)(0x3F34FDF4u));
    // 004303D2: mov byte ptr [esp+2Ch],0
    St<uint8>((F+480),(uint8)(0x0u));
    // 004303D7: jmp 004303E6
    goto L_004303E6;
L_004303D9:;
    // 004303D9: mov dword ptr [esp+20h],3F34FDF4h
    St<uint32>((F+468),(uint32)(0x3F34FDF4u));
L_004303E1:;
    // 004303E1: mov byte ptr [esp+2Ch],1
    St<uint8>((F+480),(uint8)(0x1u));
L_004303E6:;
    // 004303E6: cmp edi,0Dh
    {const uint32 a_=(edi)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004303E9: jne 00430401
    if(!zf) goto L_00430401;
    // 004303EB: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 004303ED: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 004303F2: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 004303F5: add esp,4
    // 004303F8: cmp ecx,1
    {const uint32 a_=(ecx)&4294967295,b_=(0x1u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004303FB: jl 00430401
    if(sf!=of) goto L_00430401;
    // 004303FD: xor al,al
    {const uint32 r_=(((eax&0xFFu))^((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 004303FF: jmp 00430405
    goto L_00430405;
L_00430401:;
    // 00430401: xor eax,eax
    {const uint32 r_=((eax)^(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430403: mov al,1
    eax=(eax&0xFFFFFF00u)|((uint32)(0x1u)&0xFFu);
L_00430405:;
    // 00430405: fld dword ptr ds:[00B6F11Ch]
    x0=(double)Ld<float>((uintptr_t)(uint32)(0xB6F11Cu));
    // 0043040B: push 0
    St<uint32>(F+432,0x0u);
    // 0043040D: fmul dword ptr ds:[00858970h]
    x0=Veh83DMul(x0,(double)F32(0x43200000u));
    // 00430413: push eax
    St<uint32>(F+428,eax);
    // 00430414: lea eax,[esp+2Ch]
    eax=(uint32)((uint32)(F+472));
    // 00430418: push eax
    St<uint32>(F+424,eax);
    // 00430419: lea ecx,[esp+54h]
    ecx=(uint32)((uint32)(F+508));
    // 0043041D: push ecx
    St<uint32>(F+420,ecx);
    // 0043041E: lea edx,[esp+5Ch]
    edx=(uint32)((uint32)(F+512));
    // 00430422: push edx
    St<uint32>(F+416,edx);
    // 00430423: mov edx,dword ptr [esp+34h]
    edx=(uint32)(Ld<uint32>((F+468)));
    // 00430427: lea eax,[esp+000000A4h]
    eax=(uint32)((uint32)(F+580));
    // 0043042E: push eax
    St<uint32>(F+412,eax);
    // 0043042F: mov eax,dword ptr [esp+3Ch]
    eax=(uint32)(Ld<uint32>((F+472)));
    // 00430433: push 42180000h
    St<uint32>(F+408,0x42180000u);
    // 00430438: push ecx
    St<uint32>(F+404,ecx);
    // 00430439: mov ecx,dword ptr [esp+4Ch]
    ecx=(uint32)(Ld<uint32>((F+480)));
    // 0043043D: fstp dword ptr [esp]
    St<float>((F+404),Veh83DStore(x0));
    // 00430440: push ecx
    St<uint32>(F+400,ecx);
    // 00430441: mov ecx,dword ptr [esp+38h]
    ecx=(uint32)(Ld<uint32>((F+456)));
    // 00430445: push edx
    St<uint32>(F+396,edx);
    // 00430446: push eax
    St<uint32>(F+392,eax);
    // 00430447: push ecx
    St<uint32>(F+388,ecx);
    // 00430448: sub esp,0Ch
    // 0043044B: mov edx,esp
    edx=(uint32)((uint32)(uintptr_t)(F+376));
    // 0043044D: mov dword ptr [edx],esi
    St<uint32>((uintptr_t)(uint32)(edx),(uint32)(esi));
    // 0043044F: mov dword ptr [edx+4],ebx
    St<uint32>((uintptr_t)(uint32)(edx+0x4u),(uint32)(ebx));
    // 00430452: mov dword ptr [edx+8],ebp
    St<uint32>((uintptr_t)(uint32)(edx+0x8u),(uint32)(ebp));
    // 00430455: call 00424210
    eax=((uint8(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x424210)(Ld<uint32>(F+376),Ld<uint32>(F+380),Ld<uint32>(F+384),Ld<uint32>(F+388),Ld<uint32>(F+392),Ld<uint32>(F+396),Ld<uint32>(F+400),Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 0043045A: add esp,3Ch
    // 0043045D: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043045F: je 00431AAD
    if(zf) goto L_00431AAD;
    // 00430465: movzx eax,word ptr [esp+4Ah]
    eax=(uint32)(Ld<uint16>((F+510)));
    // 0043046A: movzx esi,word ptr [esp+4Eh]
    esi=(uint32)(Ld<uint16>((F+514)));
    // 0043046F: imul eax,eax,1Ch
    eax=(uint32)((uint32)((int32)eax*(int32)0x1Cu));
    // 00430472: movzx ebx,word ptr [esp+4Ch]
    ebx=(uint32)(Ld<uint16>((F+512)));
    // 00430477: imul esi,esi,1Ch
    esi=(uint32)((uint32)((int32)esi*(int32)0x1Cu));
    // 0043047A: movzx ecx,word ptr [esp+48h]
    ecx=(uint32)(Ld<uint16>((F+508)));
    // 0043047F: mov ecx,dword ptr [ecx*4+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F854u)));
    // 00430486: shl ebx,2
    {const uint32 a_=(ebx)&4294967295,r_=((a_<<2)&4294967295);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>30)&1;pf=Parity(r_);ebx=(uint32)(r_);}
    // 00430489: mov edx,dword ptr [ebx+0096F854h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 0043048F: mov dword ptr [esp+54h],eax
    St<uint32>((F+520),(uint32)(eax));
    // 00430493: mov al,byte ptr [ecx+eax+1Ah]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(ecx+eax+0x1Au)))&0xFFu);
    // 00430497: mov cl,byte ptr [esi+edx+1Ah]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+edx+0x1Au)))&0xFFu);
    // 0043049B: and cl,0Fh
    {const uint32 r_=(((ecx&0xFFu))&(0xFu))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 0043049E: and al,0Fh
    {const uint32 r_=(((eax&0xFFu))&(0xFu))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 004304A0: cmp cl,al
    {const uint32 a_=((ecx&0xFFu))&255,b_=((eax&0xFFu))&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 004304A2: mov byte ptr [esp+1Fh],0
    St<uint8>((F+467),(uint8)(0x0u));
    // 004304A7: mov dword ptr [esp+30h],ebx
    St<uint32>((F+484),(uint32)(ebx));
    // 004304AB: mov dword ptr [esp+74h],esi
    St<uint32>((F+552),(uint32)(esi));
    // 004304AF: mov byte ptr [esp+2Ch],cl
    St<uint8>((F+480),(uint8)((ecx&0xFFu)));
    // 004304B3: jb 004304B9
    if(cf) goto L_004304B9;
    // 004304B5: mov byte ptr [esp+2Ch],al
    St<uint8>((F+480),(uint8)((eax&0xFFu)));
L_004304B9:;
    // 004304B9: call 00821B1E
    eax=((uint32(__cdecl*)(void))0x821B1E)();
    // 004304BE: movzx ecx,byte ptr [esp+2Ch]
    ecx=(uint32)(Ld<uint8>((F+480)));
    // 004304C3: and eax,0Fh
    {const uint32 r_=((eax)&(0xFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004304C6: cmp eax,ecx
    {const uint32 a_=(eax)&4294967295,b_=(ecx)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004304C8: ja 00431AAD
    if(!cf&&!zf) goto L_00431AAD;
    // 004304CE: mov edx,dword ptr [ebx+0096F854h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 004304D4: mov al,byte ptr [esi+edx+18h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+edx+0x18u)))&0xFFu);
    // 004304D8: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004304DA: jns 0043054D
    if(!sf) goto L_0043054D;
    // 004304DC: cmp edi,0Dh
    {const uint32 a_=(edi)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004304DF: mov byte ptr [esp+1Fh],1
    St<uint8>((F+467),(uint8)(0x1u));
    // 004304E4: jne 00430519
    if(!zf) goto L_00430519;
    // 004304E6: cmp byte ptr ds:[008E6E68h],1
    {const uint32 a_=(Ld<uint8>((uintptr_t)(uint32)(0x8E6E68u)))&255,b_=(0x1u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 004304ED: mov dword ptr [esp+28h],1AEh
    St<uint32>((F+476),(uint32)(0x1AEu));
    // 004304F5: mov dword ptr [esp+58h],18h
    St<uint32>((F+524),(uint32)(0x18u));
    // 004304FD: je 00430543
    if(zf) goto L_00430543;
    // 004304FF: push 8
    St<uint32>(F+432,0x8u);
    // 00430501: push 1AEh
    St<uint32>(F+428,0x1AEu);
    // 00430506: call 004087E0
    ((void(__cdecl*)(uint32,uint32))0x4087E0)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 0043050B: add esp,8
    // 0043050E: pop edi
    edi=(uint32)(Ld<uint32>(F+436));
    // 0043050F: pop esi
    esi=(uint32)(Ld<uint32>(F+440));
    // 00430510: pop ebp
    ebp=(uint32)(Ld<uint32>(F+444));
    // 00430511: pop ebx
    ebx=(uint32)(Ld<uint32>(F+448));
    // 00430512: add esp,12Ch
    // 00430518: ret
    return;
L_00430519:;
    // 00430519: push 1
    St<uint32>(F+432,0x1u);
    // 0043051B: mov ecx,0C0E998h
    ecx=(uint32)(0xC0E998u);
    // 00430520: call 00611E90
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x611E90)(ecx,edx,Ld<uint32>(F+432));
    // 00430525: cmp eax,0FFFFFFFFh
    {const uint32 a_=(eax)&4294967295,b_=(0xFFFFFFFFu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430528: mov dword ptr [esp+28h],eax
    St<uint32>((F+476),(uint32)(eax));
    // 0043052C: je 00431AAD
    if(zf) goto L_00431AAD;
    // 00430532: lea eax,[eax+eax*4]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(eax+eax*4));
    // 00430535: cmp byte ptr [eax*4+008E4CD0h],1
    {const uint32 a_=(Ld<uint8>((uintptr_t)(uint32)(eax*4+0x8E4CD0u)))&255,b_=(0x1u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 0043053D: jne 00431AAD
    if(!zf) goto L_00431AAD;
L_00430543:;
    // 00430543: mov dword ptr [esp+14h],42200000h
    St<uint32>((F+456),(uint32)(0x42200000u));
    // 0043054B: jmp 00430555
    goto L_00430555;
L_0043054D:;
    // 0043054D: mov dword ptr [esp+14h],41000000h
    St<uint32>((F+456),(uint32)(0x41000000u));
L_00430555:;
    // 00430555: mov edx,dword ptr [esp+14h]
    edx=(uint32)(Ld<uint32>((F+456)));
    // 00430559: push 0
    St<uint32>(F+432,0x0u);
    // 0043055B: push 0
    St<uint32>(F+428,0x0u);
    // 0043055D: push 1
    St<uint32>(F+424,0x1u);
    // 0043055F: push 1
    St<uint32>(F+420,0x1u);
    // 00430561: push 0
    St<uint32>(F+416,0x0u);
    // 00430563: push 0
    St<uint32>(F+412,0x0u);
    // 00430565: push 2
    St<uint32>(F+408,0x2u);
    // 00430567: lea ecx,[esp+48h]
    ecx=(uint32)((uint32)(F+480));
    // 0043056B: push ecx
    St<uint32>(F+404,ecx);
    // 0043056C: push 1
    St<uint32>(F+400,0x1u);
    // 0043056E: push edx
    St<uint32>(F+396,edx);
    // 0043056F: lea eax,[esp+000000B8h]
    eax=(uint32)((uint32)(F+580));
    // 00430576: push eax
    St<uint32>(F+392,eax);
    // 00430577: call 00568B80
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x568B80)(Ld<uint32>(F+392),Ld<uint32>(F+396),Ld<uint32>(F+400),Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 0043057C: add esp,2Ch
    // 0043057F: cmp word ptr [esp+2Ch],0
    {const uint32 a_=(Ld<uint16>((F+480)))&65535,b_=(0x0u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00430585: jne 00431AAD
    if(!zf) goto L_00431AAD;
    // 0043058B: mov ecx,dword ptr [ebx+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430591: add esi,ecx
    {const uint32 a_=(esi)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);esi=(uint32)(r_);}
    // 00430593: xor edx,edx
    {const uint32 r_=((edx)^(edx))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430595: mov dl,byte ptr [esi+18h]
    edx=(edx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x18u)))&0xFFu);
    // 00430598: xor edi,edi
    {const uint32 r_=((edi)^(edi))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);edi=(uint32)(r_);}
    // 0043059A: mov ebp,edx
    ebp=(uint32)(edx);
    // 0043059C: and ebp,0Fh
    {const uint32 r_=((ebp)&(0xFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ebp=(uint32)(r_);}
    // 0043059F: jle 004305C9
    if(zf||sf!=of) goto L_004305C9;
    // 004305A1: xor eax,eax
    {const uint32 r_=((eax)^(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
L_004305A3:;
    // 004305A3: lea ecx,[esp+48h]
    ecx=(uint32)((uint32)(F+508));
    // 004305A7: push ecx
    St<uint32>(F+432,ecx);
    // 004305A8: movsx ecx,word ptr [esi+10h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x10u))));
    // 004305AC: lea edx,[eax+ecx]
    edx=(uint32)((uint32)(uintptr_t)(uint32)(eax+ecx));
    // 004305AF: mov eax,dword ptr [ebx+0096FA94h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96FA94u)));
    // 004305B5: lea ecx,[eax+edx*4]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(eax+edx*4));
    // 004305B8: call 00420950
    eax=((uint8(__fastcall*)(uint32,uint32,uint32))0x420950)(ecx,edx,Ld<uint32>(F+432));
    // 004305BD: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004305BF: je 004305C9
    if(zf) goto L_004305C9;
    // 004305C1: inc edi
    {const uint32 a_=(edi)&4294967295,r_=(a_+1)&4294967295;zf=r_==0;sf=(r_>>31)&1;of=r_==2147483648u;pf=Parity(r_);edi=(uint32)(r_);}
    // 004305C2: movsx eax,di
    eax=(uint32)((uint32)(int32)(int16)((edi&0xFFFFu)));
    // 004305C5: cmp eax,ebp
    {const uint32 a_=(eax)&4294967295,b_=(ebp)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004305C7: jl 004305A3
    if(sf!=of) goto L_004305A3;
L_004305C9:;
    // 004305C9: movsx eax,word ptr [esi+10h]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x10u))));
    // 004305CD: mov edx,dword ptr [ebx+0096FDF4h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96FDF4u)));
    // 004305D3: movsx ecx,di
    ecx=(uint32)((uint32)(int32)(int16)((edi&0xFFFFu)));
    // 004305D6: add eax,ecx
    {const uint32 a_=(eax)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 004305D8: mov ax,word ptr [edx+eax*2]
    eax=(eax&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(edx+eax*2)))&0xFFFFu);
    // 004305DC: mov word ptr [esp+10h],ax
    St<uint16>((F+452),(uint16)((eax&0xFFFFu)));
    // 004305E1: movzx eax,ax
    eax=(uint32)((eax&0xFFFFu));
    // 004305E4: mov ecx,eax
    ecx=(uint32)(eax);
    // 004305E6: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004305EB: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 004305EE: shr ecx,0Ah
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 004305F1: mov edx,dword ptr [ecx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F974u)));
    // 004305F8: lea esi,[edx+eax]
    esi=(uint32)((uint32)(uintptr_t)(uint32)(edx+eax));
    // 004305FB: lea eax,[esp+48h]
    eax=(uint32)((uint32)(F+508));
    // 004305FF: push eax
    St<uint32>(F+432,eax);
    // 00430600: lea ecx,[esi+4]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x4u));
    // 00430603: call 00420920
    eax=((uint8(__fastcall*)(uint32,uint32,uint32))0x420920)(ecx,edx,Ld<uint32>(F+432));
    // 00430608: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043060A: je 00430618
    if(zf) goto L_00430618;
    // 0043060C: mov cl,byte ptr [esi+0Bh]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0xBu)))&0xFFu);
    // 0043060F: and cl,7
    {const uint32 r_=(((ecx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00430612: movzx ax,cl
    eax=(eax&0xFFFF0000u)|((uint32)((ecx&0xFFu))&0xFFFFu);
    // 00430616: jmp 00430625
    goto L_00430625;
L_00430618:;
    // 00430618: mov dl,byte ptr [esi+0Bh]
    edx=(edx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0xBu)))&0xFFu);
    // 0043061B: shr dl,3
    {const uint32 a_=((edx&0xFFu))&255,r_=(a_>>3);zf=r_==0;sf=(r_>>7)&1;cf=(a_>>2)&1;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 0043061E: and dl,7
    {const uint32 r_=(((edx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00430621: movzx ax,dl
    eax=(eax&0xFFFF0000u)|((uint32)((edx&0xFFu))&0xFFFFu);
L_00430625:;
    // 00430625: cmp ax,1
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x1u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00430629: mov edi,dword ptr [esp+28h]
    edi=(uint32)(Ld<uint32>((F+476)));
    // 0043062D: mov dword ptr [esp+14h],eax
    St<uint32>((F+456),(uint32)(eax));
    // 00430631: jle 00430640
    if(zf||sf!=of) goto L_00430640;
    // 00430633: mov ecx,dword ptr [edi*4+00A9B0C8h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edi*4+0xA9B0C8u)));
    // 0043063A: cmp dword ptr [ecx+3Ch],0Ah
    {const uint32 a_=(Ld<uint32>((uintptr_t)(uint32)(ecx+0x3Cu)))&4294967295,b_=(0xAu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043063E: jmp 00430652
    goto L_00430652;
L_00430640:;
    // 00430640: cmp edi,1B5h
    {const uint32 a_=(edi)&4294967295,b_=(0x1B5u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430646: je 00431AAD
    if(zf) goto L_00431AAD;
    // 0043064C: cmp edi,1AFh
    {const uint32 a_=(edi)&4294967295,b_=(0x1AFu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
L_00430652:;
    // 00430652: je 00431AAD
    if(zf) goto L_00431AAD;
    // 00430658: test ax,ax
    {const uint32 r_=(((eax&0xFFFFu))&((eax&0xFFFFu)))&65535;zf=r_==0;sf=(r_>>15)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043065B: je 00431AAD
    if(zf) goto L_00431AAD;
    // 00430661: mov eax,dword ptr ds:[00C0BC64h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xC0BC64u)));
    // 00430666: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430668: je 0043069E
    if(zf) goto L_0043069E;
    // 0043066A: lea edx,[esp+00000090h]
    edx=(uint32)((uint32)(F+580));
    // 00430671: push 0
    St<uint32>(F+432,0x0u);
    // 00430673: push edx
    St<uint32>(F+428,edx);
    // 00430674: call 00572400
    eax=((uint32(__cdecl*)(uint32,uint32))0x572400)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430679: movzx eax,byte ptr [eax+0Fh]
    eax=(uint32)(Ld<uint8>((uintptr_t)(uint32)(eax+0xFu)));
    // 0043067D: and eax,1Fh
    {const uint32 r_=((eax)&(0x1Fu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430680: add esp,8
    // 00430683: cmp eax,11h
    {const uint32 a_=(eax)&4294967295,b_=(0x11u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430686: jl 0043069E
    if(sf!=of) goto L_0043069E;
    // 00430688: cmp eax,13h
    {const uint32 a_=(eax)&4294967295,b_=(0x13u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043068B: jg 0043069E
    if(!zf&&sf==of) goto L_0043069E;
    // 0043068D: cmp eax,dword ptr ds:[00C0BC6Ch]
    {const uint32 a_=(eax)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(0xC0BC6Cu)))&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430693: jne 00431AAD
    if(!zf) goto L_00431AAD;
    // 00430699: mov byte ptr [esp+36h],1
    St<uint8>((F+490),(uint8)(0x1u));
L_0043069E:;
    // 0043069E: push 1
    St<uint32>(F+432,0x1u);
    // 004306A0: push edi
    St<uint32>(F+428,edi);
    // 004306A1: call 00421440
    eax=((uint32(__cdecl*)(uint32,uint32))0x421440)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004306A6: mov ebp,dword ptr [esp+60h]
    ebp=(uint32)(Ld<uint32>((F+524)));
    // 004306AA: mov ecx,dword ptr [esp+50h]
    ecx=(uint32)(Ld<uint32>((F+508)));
    // 004306AE: mov esi,eax
    esi=(uint32)(eax);
    // 004306B0: mov eax,dword ptr [esp+54h]
    eax=(uint32)(Ld<uint32>((F+512)));
    // 004306B4: add esp,8
    // 004306B7: cmp ebp,0Dh
    {const uint32 a_=(ebp)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004306BA: mov word ptr [esi+00000398h],0FFFFh
    St<uint16>((uintptr_t)(uint32)(esi+0x398u),(uint16)(0xFFFFu));
    // 004306C3: mov dword ptr [esi+00000390h],eax
    St<uint32>((uintptr_t)(uint32)(esi+0x390u),(uint32)(eax));
    // 004306C9: mov dword ptr [esi+00000394h],ecx
    St<uint32>((uintptr_t)(uint32)(esi+0x394u),(uint32)(ecx));
    // 004306CF: je 0043080B
    if(zf) goto L_0043080B;
    // 004306D5: cmp ebp,18h
    {const uint32 a_=(ebp)&4294967295,b_=(0x18u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004306D8: je 004307D0
    if(zf) goto L_004307D0;
    // 004306DE: push 41A80000h
    St<uint32>(F+432,0x41A80000u);
    // 004306E3: push 41500000h
    St<uint32>(F+428,0x41500000u);
    // 004306E8: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004306ED: add esp,8
    // 004306F0: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 004306F5: cmp ebp,3
    {const uint32 a_=(ebp)&4294967295,b_=(0x3u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 004306F8: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
    // 004306FE: jne 0043070C
    if(!zf) goto L_0043070C;
    // 00430700: push 41D80000h
    St<uint32>(F+432,0x41D80000u);
    // 00430705: push 41900000h
    St<uint32>(F+428,0x41900000u);
    // 0043070A: jmp 0043071B
    goto L_0043071B;
L_0043070C:;
    // 0043070C: cmp ebp,1
    {const uint32 a_=(ebp)&4294967295,b_=(0x1u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043070F: jne 0043072E
    if(!zf) goto L_0043072E;
    // 00430711: push 41700000h
    St<uint32>(F+432,0x41700000u);
    // 00430716: push 41200000h
    St<uint32>(F+428,0x41200000u);
L_0043071B:;
    // 0043071B: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430720: add esp,8
    // 00430723: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 00430728: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
L_0043072E:;
    // 0043072E: mov cx,word ptr [esi+22h]
    ecx=(ecx&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&0xFFFFu);
    // 00430732: movsx edx,cx
    edx=(uint32)((uint32)(int32)(int16)((ecx&0xFFFFu)));
    // 00430735: mov eax,dword ptr [edx*4+00A9B0C8h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0xA9B0C8u)));
    // 0043073C: mov eax,dword ptr [eax+14h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x14u)));
    // 0043073F: fld dword ptr [eax+10h]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x10u));
    // 00430742: fsub dword ptr [eax+4]
    x0=Veh83DSub(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430745: fcomp dword ptr ds:[0085862Ch]
    fcc=X87Compare(x0,(double)F32(0x41200000u));
    // 0043074B: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 0043074D: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430750: je 00430757
    if(zf) goto L_00430757;
    // 00430752: cmp ebp,5
    {const uint32 a_=(ebp)&4294967295,b_=(0x5u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430755: jne 00430770
    if(!zf) goto L_00430770;
L_00430757:;
    // 00430757: movzx eax,byte ptr [esi+000003D0h]
    eax=(uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3D0u)));
    // 0043075E: lea eax,[eax+eax*2]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(eax+eax*2));
    // 00430761: cdq
    edx=((int32)eax<0)?0xFFFFFFFFu:0u;
    // 00430762: and edx,3
    {const uint32 r_=((edx)&(0x3u))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430765: add eax,edx
    {const uint32 a_=(eax)&4294967295,b_=(edx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430767: sar eax,2
    {const uint32 a_=(eax)&4294967295,r_=(uint32)((int32)a_>>2);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>1)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043076A: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
L_00430770:;
    // 00430770: mov al,byte ptr [esp+1Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+467)))&0xFFu);
    // 00430774: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430776: je 004307B6
    if(zf) goto L_004307B6;
    // 00430778: cmp cx,1BEh
    {const uint32 a_=((ecx&0xFFFFu))&65535,b_=(0x1BEu)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 0043077D: je 00430799
    if(zf) goto L_00430799;
    // 0043077F: cmp cx,1C4h
    {const uint32 a_=((ecx&0xFFFFu))&65535,b_=(0x1C4u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00430784: je 00430799
    if(zf) goto L_00430799;
    // 00430786: cmp cx,1EDh
    {const uint32 a_=((ecx&0xFFFFu))&65535,b_=(0x1EDu)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 0043078B: je 00430799
    if(zf) goto L_00430799;
    // 0043078D: push 41C00000h
    St<uint32>(F+432,0x41C00000u);
    // 00430792: push 41700000h
    St<uint32>(F+428,0x41700000u);
    // 00430797: jmp 004307A3
    goto L_004307A3;
L_00430799:;
    // 00430799: push 420C0000h
    St<uint32>(F+432,0x420C0000u);
    // 0043079E: push 41C80000h
    St<uint32>(F+428,0x41C80000u);
L_004307A3:;
    // 004307A3: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004307A8: add esp,8
    // 004307AB: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 004307B0: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
L_004307B6:;
    // 004307B6: mov byte ptr [esi+000003BAh],1
    St<uint8>((uintptr_t)(uint32)(esi+0x3BAu),(uint8)(0x1u));
    // 004307BD: mov byte ptr [esi+000003BBh],0
    St<uint8>((uintptr_t)(uint32)(esi+0x3BBu),(uint8)(0x0u));
    // 004307C4: mov byte ptr [esi+000003B9h],0
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x0u));
    // 004307CB: jmp 004308A1
    goto L_004308A1;
L_004307D0:;
    // 004307D0: push 41900000h
    St<uint32>(F+432,0x41900000u);
    // 004307D5: push 41600000h
    St<uint32>(F+428,0x41600000u);
    // 004307DA: mov byte ptr [esi+000003BBh],0
    St<uint8>((uintptr_t)(uint32)(esi+0x3BBu),(uint8)(0x0u));
    // 004307E1: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004307E6: add esp,8
    // 004307E9: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 004307EE: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
    // 004307F4: mov byte ptr [esi+000003B9h],2
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x2u));
    // 004307FB: call 0041CA50
    eax=((uint8(__cdecl*)(void))0x41CA50)();
    // 00430800: mov byte ptr [esi+000003BAh],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3BAu),(uint8)((eax&0xFFu)));
    // 00430806: jmp 0043089A
    goto L_0043089A;
L_0043080B:;
    // 0043080B: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 0043080D: mov byte ptr [esi+000003BBh],0
    St<uint8>((uintptr_t)(uint32)(esi+0x3BBu),(uint8)(0x0u));
    // 00430814: call 0056E230
    eax=((uint32(__cdecl*)(uint32))0x56E230)(Ld<uint32>(F+432));
    // 00430819: mov ecx,dword ptr [eax+2Ch]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x2Cu)));
    // 0043081C: add esp,4
    // 0043081F: test ecx,ecx
    {const uint32 r_=((ecx)&(ecx))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430821: jne 00430850
    if(!zf) goto L_00430850;
    // 00430823: push 41C00000h
    St<uint32>(F+432,0x41C00000u);
    // 00430828: push 41900000h
    St<uint32>(F+428,0x41900000u);
    // 0043082D: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430832: add esp,8
    // 00430835: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 0043083A: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
    // 00430840: mov byte ptr [esi+000003B9h],0
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x0u));
    // 00430847: mov byte ptr [esi+000003BAh],1
    St<uint8>((uintptr_t)(uint32)(esi+0x3BAu),(uint8)(0x1u));
    // 0043084E: jmp 00430884
    goto L_00430884;
L_00430850:;
    // 00430850: push esi
    St<uint32>(F+432,esi);
    // 00430851: call 0041CAA0
    eax=((uint8(__cdecl*)(uint32))0x41CAA0)(Ld<uint32>(F+432));
    // 00430856: add esp,4
    // 00430859: mov ecx,esi
    ecx=(uint32)(esi);
    // 0043085B: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
    // 00430861: call 006D1080
    eax=((uint32(__fastcall*)(uint32,uint32))0x6D1080)(ecx,edx);
    // 00430866: cmp eax,2
    {const uint32 a_=(eax)&4294967295,b_=(0x2u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430869: jne 00430872
    if(!zf) goto L_00430872;
    // 0043086B: call 0041CA40
    eax=((uint8(__cdecl*)(void))0x41CA40)();
    // 00430870: jmp 00430877
    goto L_00430877;
L_00430872:;
    // 00430872: call 0041C9D0
    eax=((uint8(__cdecl*)(void))0x41C9D0)();
L_00430877:;
    // 00430877: mov byte ptr [esi+000003BAh],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3BAu),(uint8)((eax&0xFFu)));
    // 0043087D: mov byte ptr [esi+000003B9h],2
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x2u));
L_00430884:;
    // 00430884: cmp edi,1EAh
    {const uint32 a_=(edi)&4294967295,b_=(0x1EAu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 0043088A: jne 0043089A
    if(!zf) goto L_0043089A;
    // 0043088C: mov byte ptr [esi+00000434h],0
    St<uint8>((uintptr_t)(uint32)(esi+0x434u),(uint8)(0x0u));
    // 00430893: mov byte ptr [esi+00000435h],0
    St<uint8>((uintptr_t)(uint32)(esi+0x435u),(uint8)(0x0u));
L_0043089A:;
    // 0043089A: or byte ptr [esi+0000042Ch],1
    {const uint32 r_=((Ld<uint8>((uintptr_t)(uint32)(esi+0x42Cu)))|(0x1u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);St<uint8>((uintptr_t)(uint32)(esi+0x42Cu),(uint8)(r_));}
L_004308A1:;
    // 004308A1: cmp word ptr [esi+22h],1A7h
    {const uint32 a_=(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&65535,b_=(0x1A7u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 004308A7: jne 004308B0
    if(!zf) goto L_004308B0;
    // 004308A9: or byte ptr [esi+0000042Dh],80h
    {const uint32 r_=((Ld<uint8>((uintptr_t)(uint32)(esi+0x42Du)))|(0x80u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);St<uint8>((uintptr_t)(uint32)(esi+0x42Du),(uint8)(r_));}
L_004308B0:;
    // 004308B0: mov cx,word ptr [esp+10h]
    ecx=(ecx&0xFFFF0000u)|((uint32)(Ld<uint16>((F+452)))&0xFFFFu);
    // 004308B5: mov word ptr [esi+000003A6h],cx
    St<uint16>((uintptr_t)(uint32)(esi+0x3A6u),(uint16)((ecx&0xFFFFu)));
    // 004308BC: call 00821B1E
    eax=((uint32(__cdecl*)(void))0x821B1E)();
    // 004308C1: movsx ecx,word ptr [esp+14h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((F+456))));
    // 004308C6: cdq
    edx=((int32)eax<0)?0xFFFFFFFFu:0u;
    // 004308C7: idiv eax,ecx
    {const int64 n_=((int64)(int32)edx<<32)|(int64)eax;const int32 d_=(int32)ecx;eax=(uint32)(int32)(n_/d_);edx=(uint32)(int32)(n_%d_);}
    // 004308C9: mov byte ptr [esi+000003B7h],dl
    St<uint8>((uintptr_t)(uint32)(esi+0x3B7u),(uint8)((edx&0xFFu)));
    // 004308CF: mov byte ptr [esi+000003B8h],dl
    St<uint8>((uintptr_t)(uint32)(esi+0x3B8u),(uint8)((edx&0xFFu)));
    // 004308D5: call 00441C10
    eax=((uint8(__cdecl*)(void))0x441C10)();
    // 004308DA: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004308DC: je 004308E5
    if(zf) goto L_004308E5;
    // 004308DE: mov eax,50h
    eax=(uint32)(0x50u);
    // 004308E3: jmp 00430909
    goto L_00430909;
L_004308E5:;
    // 004308E5: mov ecx,esi
    ecx=(uint32)(esi);
    // 004308E7: call 006D1080
    eax=((uint32(__fastcall*)(uint32,uint32))0x6D1080)(ecx,edx);
    // 004308EC: sub eax,2
    {const uint32 a_=(eax)&4294967295,b_=(0x2u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 004308EF: je 00430904
    if(zf) goto L_00430904;
    // 004308F1: sub eax,2
    {const uint32 a_=(eax)&4294967295,b_=(0x2u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 004308F4: je 004308FD
    if(zf) goto L_004308FD;
    // 004308F6: mov eax,0C8h
    eax=(uint32)(0xC8u);
    // 004308FB: jmp 00430909
    goto L_00430909;
L_004308FD:;
    // 004308FD: mov eax,0Ah
    eax=(uint32)(0xAu);
    // 00430902: jmp 00430909
    goto L_00430909;
L_00430904:;
    // 00430904: mov eax,32h
    eax=(uint32)(0x32u);
L_00430909:;
    // 00430909: mov cl,byte ptr [esp+1Fh]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+467)))&0xFFu);
    // 0043090D: test cl,cl
    {const uint32 r_=(((ecx&0xFFu))&((ecx&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043090F: jne 00430943
    if(!zf) goto L_00430943;
    // 00430911: cmp ebp,0Dh
    {const uint32 a_=(ebp)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00430914: je 00430943
    if(zf) goto L_00430943;
    // 00430916: mov cl,byte ptr [esp+36h]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+490)))&0xFFu);
    // 0043091A: test cl,cl
    {const uint32 r_=(((ecx&0xFFu))&((ecx&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043091C: jne 00430943
    if(!zf) goto L_00430943;
    // 0043091E: push eax
    St<uint32>(F+432,eax);
    // 0043091F: push 0
    St<uint32>(F+428,0x0u);
    // 00430921: call 00407180
    eax=((uint32(__cdecl*)(uint32,uint32))0x407180)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430926: add esp,8
    // 00430929: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043092B: je 00430936
    if(zf) goto L_00430936;
    // 0043092D: mov al,byte ptr ds:[0096914Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0x96914Fu)))&0xFFu);
    // 00430932: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430934: je 00430943
    if(zf) goto L_00430943;
L_00430936:;
    // 00430936: mov byte ptr [esp+47h],1
    St<uint8>((F+507),(uint8)(0x1u));
    // 0043093B: mov dword ptr [esp+24h],3F800000h
    St<uint32>((F+472),(uint32)(0x3F800000u));
L_00430943:;
    // 00430943: movsx edx,word ptr [esi+22h]
    edx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00430947: mov eax,dword ptr [edx*4+00A9B0C8h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0xA9B0C8u)));
    // 0043094E: mov eax,dword ptr [eax+14h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x14u)));
    // 00430951: mov ecx,dword ptr [ebx+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430957: mov edx,dword ptr [esp+74h]
    edx=(uint32)(Ld<uint32>((F+552)));
    // 0043095B: fld dword ptr [eax+10h]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x10u));
    // 0043095E: lea edi,[edx+ecx]
    edi=(uint32)((uint32)(uintptr_t)(uint32)(edx+ecx));
    // 00430961: fsub dword ptr [eax+4]
    x0=Veh83DSub(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430964: lea eax,[esp+000000DCh]
    eax=(uint32)((uint32)(F+656));
    // 0043096B: push eax
    St<uint32>(F+432,eax);
    // 0043096C: fmul dword ptr ds:[00858B8Ch]
    x0=Veh83DMul(x0,(double)F32(0x3F000000u));
    // 00430972: mov ecx,edi
    ecx=(uint32)(edi);
    // 00430974: fadd dword ptr ds:[00858624h]
    x0=Veh83DAdd(x0,(double)F32(0x3F800000u));
    // 0043097A: fstp dword ptr [esp+18h]
    St<float>((F+456),Veh83DStore(x0));
    // 0043097E: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430983: mov ecx,dword ptr [eax+4]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430986: movzx eax,word ptr [esp+48h]
    eax=(uint32)(Ld<uint16>((F+508)));
    // 0043098B: mov edx,dword ptr [eax*4+0096F854h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F854u)));
    // 00430992: mov eax,dword ptr [esp+54h]
    eax=(uint32)(Ld<uint32>((F+520)));
    // 00430996: mov dword ptr [esp+50h],ecx
    St<uint32>((F+516),(uint32)(ecx));
    // 0043099A: lea ecx,[esp+000000F8h]
    ecx=(uint32)((uint32)(F+684));
    // 004309A1: push ecx
    St<uint32>(F+432,ecx);
    // 004309A2: mov ecx,edi
    ecx=(uint32)(edi);
    // 004309A4: lea ebp,[edx+eax]
    ebp=(uint32)((uint32)(uintptr_t)(uint32)(edx+eax));
    // 004309A7: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004309AC: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 004309AF: lea eax,[esp+00000084h]
    eax=(uint32)((uint32)(F+568));
    // 004309B6: push eax
    St<uint32>(F+432,eax);
    // 004309B7: mov ecx,edi
    ecx=(uint32)(edi);
    // 004309B9: mov dword ptr [esp+14h],edx
    St<uint32>((F+452),(uint32)(edx));
    // 004309BD: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004309C2: mov ecx,dword ptr [eax]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 004309C4: lea edx,[esp+38h]
    edx=(uint32)((uint32)(F+492));
    // 004309C8: mov dword ptr [esp+70h],ecx
    St<uint32>((F+548),(uint32)(ecx));
    // 004309CC: push edx
    St<uint32>(F+432,edx);
    // 004309CD: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 004309CF: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004309D4: mov dword ptr [esp+5Ch],eax
    St<uint32>((F+528),(uint32)(eax));
    // 004309D8: lea eax,[esp+000000E8h]
    eax=(uint32)((uint32)(F+668));
    // 004309DF: push eax
    St<uint32>(F+432,eax);
    // 004309E0: mov ecx,edi
    ecx=(uint32)(edi);
    // 004309E2: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004309E7: mov ecx,dword ptr [eax]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 004309E9: lea edx,[esp+000000D0h]
    edx=(uint32)((uint32)(F+644));
    // 004309F0: mov dword ptr [esp+60h],ecx
    St<uint32>((F+532),(uint32)(ecx));
    // 004309F4: push edx
    St<uint32>(F+432,edx);
    // 004309F5: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 004309F7: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004309FC: mov edi,eax
    edi=(uint32)(eax);
    // 004309FE: lea eax,[esp+0000009Ch]
    eax=(uint32)((uint32)(F+592));
    // 00430A05: push eax
    St<uint32>(F+432,eax);
    // 00430A06: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 00430A08: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430A0D: fld dword ptr [esp+10h]
    x0=(double)Ld<float>((F+452));
    // 00430A11: fsub dword ptr [eax+4]
    x0=Veh83DSub(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430A14: lea ecx,[esp+64h]
    ecx=(uint32)((uint32)(F+536));
    // 00430A18: push ecx
    St<uint32>(F+432,ecx);
    // 00430A19: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 00430A1B: fstp dword ptr [esp+14h]
    St<float>((F+452),Veh83DStore(x0));
    // 00430A1F: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430A24: fld dword ptr [esp+50h]
    x0=(double)Ld<float>((F+516));
    // 00430A28: fsub dword ptr [eax+4]
    x0=Veh83DSub(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430A2B: mov edx,dword ptr [esp+5Ch]
    edx=(uint32)(Ld<uint32>((F+528)));
    // 00430A2F: fmul dword ptr [esp+10h]
    x0=Veh83DMul(x0,(double)Ld<float>((F+452)));
    // 00430A33: fld dword ptr [esp+60h]
    x1=(double)Ld<float>((F+532));
    // 00430A37: fsub dword ptr [edi]
    x1=Veh83DSub(x1,(double)Ld<float>((uintptr_t)(uint32)(edi)));
    // 00430A39: fld dword ptr [esp+70h]
    x2=(double)Ld<float>((F+548));
    // 00430A3D: fsub dword ptr [edx]
    x2=Veh83DSub(x2,(double)Ld<float>((uintptr_t)(uint32)(edx)));
    // 00430A3F: fmulp st(1),st
    x1=Veh83DMul(x1,x2);
    // 00430A41: faddp st(1),st
    x0=Veh83DAdd(x0,x1);
    // 00430A43: fsqrt
    x0=Veh83DSqrt(x0);
    // 00430A45: fld dword ptr ds:[00858B8Ch]
    x1=(double)F32(0x3F000000u);
    // 00430A4B: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 00430A4D: fcomp dword ptr [esp+14h]
    fcc=X87Compare(x1,(double)Ld<float>((F+456)));
    // 00430A51: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430A53: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430A56: jp 00430A7D
    if(pf) goto L_00430A7D;
    // 00430A58: mov dword ptr [esp+24h],3F000000h
    St<uint32>((F+472),(uint32)(0x3F000000u));
L_00430A60:;
    // 00430A60: fstp st(0)
L_00430A62:;
    // 00430A62: lea eax,[esp+48h]
    eax=(uint32)((uint32)(F+508));
    // 00430A66: push eax
    St<uint32>(F+432,eax);
    // 00430A67: lea ecx,[esp+50h]
    ecx=(uint32)((uint32)(F+512));
    // 00430A6B: call 00420980
    eax=((uint8(__fastcall*)(uint32,uint32,uint32))0x420980)(ecx,edx,Ld<uint32>(F+432));
    // 00430A70: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430A72: je 00430AAB
    if(zf) goto L_00430AAB;
    // 00430A74: mov byte ptr [esi+000003B6h],0FFh
    St<uint8>((uintptr_t)(uint32)(esi+0x3B6u),(uint8)(0xFFu));
    // 00430A7B: jmp 00430AB2
    goto L_00430AB2;
L_00430A7D:;
    // 00430A7D: fdivr dword ptr [esp+14h]
    x0=Veh83DDiv((double)Ld<float>((F+456)),x0);
    // 00430A81: fld dword ptr [esp+24h]
    x1=(double)Ld<float>((F+472));
    // 00430A85: fcomp st(1)
    fcc=X87Compare(x1,x0);
    // 00430A87: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430A89: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430A8C: je 00430A92
    if(zf) goto L_00430A92;
    // 00430A8E: fst dword ptr [esp+24h]
    St<float>((F+472),Veh83DStore(x0));
L_00430A92:;
    // 00430A92: fsubr dword ptr ds:[00858624h]
    x0=Veh83DSub((double)F32(0x3F800000u),x0);
    // 00430A98: fld dword ptr [esp+24h]
    x1=(double)Ld<float>((F+472));
    // 00430A9C: fcomp st(1)
    fcc=X87Compare(x1,x0);
    // 00430A9E: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430AA0: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430AA3: jnp 00430A60
    if(!pf) goto L_00430A60;
    // 00430AA5: fstp dword ptr [esp+24h]
    St<float>((F+472),Veh83DStore(x0));
    // 00430AA9: jmp 00430A62
    goto L_00430A62;
L_00430AAB:;
    // 00430AAB: mov byte ptr [esi+000003B6h],1
    St<uint8>((uintptr_t)(uint32)(esi+0x3B6u),(uint8)(0x1u));
L_00430AB2:;
    // 00430AB2: mov ecx,dword ptr [ebx+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430AB8: mov edx,dword ptr [esp+74h]
    edx=(uint32)(Ld<uint32>((F+552)));
    // 00430ABC: mov al,byte ptr [edx+ecx+18h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(edx+ecx+0x18u)))&0xFFu);
    // 00430AC0: lea edi,[edx+ecx]
    edi=(uint32)((uint32)(uintptr_t)(uint32)(edx+ecx));
    // 00430AC3: and al,0Fh
    {const uint32 r_=(((eax&0xFFu))&(0xFu))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00430AC5: cmp al,1
    {const uint32 a_=((eax&0xFFu))&255,b_=(0x1u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 00430AC7: je 00431AA5
    if(zf) goto L_00431AA5;
    // 00430ACD: lea ecx,[ecx]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(ecx));
L_00430AD0:;
    // 00430AD0: call 00821B1E
    eax=((uint32(__cdecl*)(void))0x821B1E)();
    // 00430AD5: xor ecx,ecx
    {const uint32 r_=((ecx)^(ecx))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430AD7: mov cl,byte ptr [edi+18h]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(edi+0x18u)))&0xFFu);
    // 00430ADA: cdq
    edx=((int32)eax<0)?0xFFFFFFFFu:0u;
    // 00430ADB: mov ebp,dword ptr [esp+74h]
    ebp=(uint32)(Ld<uint32>((F+552)));
    // 00430ADF: and ecx,0Fh
    {const uint32 r_=((ecx)&(0xFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430AE2: idiv eax,ecx
    {const int64 n_=((int64)(int32)edx<<32)|(int64)eax;const int32 d_=(int32)ecx;eax=(uint32)(int32)(n_/d_);edx=(uint32)(int32)(n_%d_);}
    // 00430AE4: mov eax,dword ptr [ebx+0096F854h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430AEA: movsx ecx,word ptr [eax+ebp+10h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(eax+ebp+0x10u))));
    // 00430AEF: lea edi,[eax+ebp]
    edi=(uint32)((uint32)(uintptr_t)(uint32)(eax+ebp));
    // 00430AF2: movsx eax,dx
    eax=(uint32)((uint32)(int32)(int16)((edx&0xFFFFu)));
    // 00430AF5: add ecx,eax
    {const uint32 a_=(ecx)&4294967295,b_=(eax)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430AF7: mov eax,dword ptr [ebx+0096FDF4h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96FDF4u)));
    // 00430AFD: mov ax,word ptr [eax+ecx*2]
    eax=(eax&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(eax+ecx*2)))&0xFFFFu);
    // 00430B01: cmp ax,word ptr [esi+000003A6h]
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)))&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00430B08: je 00430AD0
    if(zf) goto L_00430AD0;
    // 00430B0A: movzx ecx,ax
    ecx=(uint32)((eax&0xFFFFu));
    // 00430B0D: shr ecx,0Ah
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430B10: mov word ptr [esi+000003A4h],ax
    St<uint16>((uintptr_t)(uint32)(esi+0x3A4u),(uint16)((eax&0xFFFFu)));
    // 00430B17: mov eax,dword ptr [ecx*4+0096F854h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F854u)));
    // 00430B1E: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430B20: je 00431AA5
    if(zf) goto L_00431AA5;
    // 00430B26: mov ecx,dword ptr [ebx+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430B2C: lea eax,[esp+4Ch]
    eax=(uint32)((uint32)(F+512));
    // 00430B30: push eax
    St<uint32>(F+432,eax);
    // 00430B31: movsx eax,word ptr [ecx+ebp+10h]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(ecx+ebp+0x10u))));
    // 00430B36: movsx ecx,dx
    ecx=(uint32)((uint32)(int32)(int16)((edx&0xFFFFu)));
    // 00430B39: mov edx,dword ptr [ebx+0096FA94h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96FA94u)));
    // 00430B3F: add eax,ecx
    {const uint32 a_=(eax)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430B41: lea ecx,[edx+eax*4]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(edx+eax*4));
    // 00430B44: call 00420980
    eax=((uint8(__fastcall*)(uint32,uint32,uint32))0x420980)(ecx,edx,Ld<uint32>(F+432));
    // 00430B49: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430B4B: je 00430B56
    if(zf) goto L_00430B56;
    // 00430B4D: mov byte ptr [esi+000003B5h],0FFh
    St<uint8>((uintptr_t)(uint32)(esi+0x3B5u),(uint8)(0xFFu));
    // 00430B54: jmp 00430B5D
    goto L_00430B5D;
L_00430B56:;
    // 00430B56: mov byte ptr [esi+000003B5h],1
    St<uint8>((uintptr_t)(uint32)(esi+0x3B5u),(uint8)(0x1u));
L_00430B5D:;
    // 00430B5D: movzx eax,word ptr [esp+48h]
    eax=(uint32)(Ld<uint16>((F+508)));
    // 00430B62: mov edi,dword ptr [eax*4+0096F854h]
    edi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F854u)));
    // 00430B69: mov edx,dword ptr [esp+54h]
    edx=(uint32)(Ld<uint32>((F+520)));
    // 00430B6D: mov ecx,dword ptr [ebx+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx+0x96F854u)));
    // 00430B73: lea eax,[esp+64h]
    eax=(uint32)((uint32)(F+536));
    // 00430B77: push eax
    St<uint32>(F+432,eax);
    // 00430B78: add ecx,ebp
    {const uint32 a_=(ecx)&4294967295,b_=(ebp)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430B7A: add edi,edx
    {const uint32 a_=(edi)&4294967295,b_=(edx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edi=(uint32)(r_);}
    // 00430B7C: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430B81: push eax
    St<uint32>(F+432,eax);
    // 00430B82: lea ecx,[esp+000000A0h]
    ecx=(uint32)((uint32)(F+592));
    // 00430B89: push ecx
    St<uint32>(F+428,ecx);
    // 00430B8A: mov ecx,edi
    ecx=(uint32)(edi);
    // 00430B8C: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+428));
    // 00430B91: push eax
    St<uint32>(F+428,eax);
    // 00430B92: lea edx,[esp+000000D8h]
    edx=(uint32)((uint32)(F+644));
    // 00430B99: push edx
    St<uint32>(F+424,edx);
    // 00430B9A: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430B9F: mov ecx,eax
    ecx=(uint32)(eax);
    // 00430BA1: mov edx,dword ptr [ecx]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx)));
    // 00430BA3: mov dword ptr [esp+000000C4h],edx
    St<uint32>((F+620),(uint32)(edx));
    // 00430BAA: mov edx,dword ptr [ecx+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx+0x4u)));
    // 00430BAD: mov dword ptr [esp+000000C8h],edx
    St<uint32>((F+624),(uint32)(edx));
    // 00430BB4: mov ecx,dword ptr [ecx+8]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx+0x8u)));
    // 00430BB7: mov dword ptr [esp+000000CCh],ecx
    St<uint32>((F+628),(uint32)(ecx));
    // 00430BBE: mov ecx,dword ptr [eax+4]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430BC1: mov edx,dword ptr [eax]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00430BC3: mov dword ptr [esp+48h],ecx
    St<uint32>((F+496),(uint32)(ecx));
    // 00430BC7: fld dword ptr [esp+48h]
    x0=(double)Ld<float>((F+496));
    // 00430BCB: fmul dword ptr [esp+48h]
    x0=Veh83DMul(x0,(double)Ld<float>((F+496)));
    // 00430BCF: mov dword ptr [esp+44h],edx
    St<uint32>((F+492),(uint32)(edx));
    // 00430BD3: fld dword ptr [esp+44h]
    x1=(double)Ld<float>((F+492));
    // 00430BD7: mov edx,dword ptr [eax+8]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 00430BDA: fmul dword ptr [esp+44h]
    x1=Veh83DMul(x1,(double)Ld<float>((F+492)));
    // 00430BDE: add esp,0Ch
    // 00430BE1: mov dword ptr [esp+40h],edx
    St<uint32>((F+500),(uint32)(edx));
    // 00430BE5: faddp st(1),st
    x0=Veh83DAdd(x0,x1);
    // 00430BE7: fsqrt
    x0=Veh83DSqrt(x0);
    // 00430BE9: fcom dword ptr ds:[00858B50h]
    fcc=X87Compare(x0,(double)F32(0x0u));
    // 00430BEF: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430BF1: test ah,44h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x44u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430BF4: jp 00430C02
    if(pf) goto L_00430C02;
    // 00430BF6: fstp st(0)
    // 00430BF8: mov dword ptr [esp+38h],3F800000h
    St<uint32>((F+492),(uint32)(0x3F800000u));
    // 00430C00: jmp 00430C1A
    goto L_00430C1A;
L_00430C02:;
    // 00430C02: fdivr dword ptr ds:[00858624h]
    x0=Veh83DDiv((double)F32(0x3F800000u),x0);
    // 00430C08: fld dword ptr [esp+38h]
    x1=(double)Ld<float>((F+492));
    // 00430C0C: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 00430C0E: fstp dword ptr [esp+38h]
    St<float>((F+492),Veh83DStore(x1));
    // 00430C12: fmul dword ptr [esp+3Ch]
    x0=Veh83DMul(x0,(double)Ld<float>((F+496)));
    // 00430C16: fstp dword ptr [esp+3Ch]
    St<float>((F+496),Veh83DStore(x0));
L_00430C1A:;
    // 00430C1A: lea ecx,[esp+000000B8h]
    ecx=(uint32)((uint32)(F+620));
    // 00430C21: call 0059C910
    ((void(__fastcall*)(uint32,uint32))0x59C910)(ecx,edx);
    // 00430C26: fld dword ptr [esp+38h]
    x0=(double)Ld<float>((F+492));
    // 00430C2A: mov eax,dword ptr [esi+14h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C2D: fchs
    x0=-x0;
    // 00430C2F: mov ecx,dword ptr [esp+000000B8h]
    ecx=(uint32)(Ld<uint32>((F+620)));
    // 00430C36: mov dword ptr [eax+10h],ecx
    St<uint32>((uintptr_t)(uint32)(eax+0x10u),(uint32)(ecx));
    // 00430C39: mov edx,dword ptr [esi+14h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C3C: mov eax,dword ptr [esp+000000BCh]
    eax=(uint32)(Ld<uint32>((F+624)));
    // 00430C43: mov dword ptr [edx+14h],eax
    St<uint32>((uintptr_t)(uint32)(edx+0x14u),(uint32)(eax));
    // 00430C46: mov ecx,dword ptr [esi+14h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C49: mov edx,dword ptr [esp+000000C0h]
    edx=(uint32)(Ld<uint32>((F+628)));
    // 00430C50: mov dword ptr [ecx+18h],edx
    St<uint32>((uintptr_t)(uint32)(ecx+0x18u),(uint32)(edx));
    // 00430C53: mov eax,dword ptr [esi+14h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C56: mov ecx,dword ptr [esp+3Ch]
    ecx=(uint32)(Ld<uint32>((F+496)));
    // 00430C5A: mov dword ptr [eax],ecx
    St<uint32>((uintptr_t)(uint32)(eax),(uint32)(ecx));
    // 00430C5C: mov edx,dword ptr [esi+14h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C5F: fstp dword ptr [edx+4]
    St<float>((uintptr_t)(uint32)(edx+0x4u),Veh83DStore(x0));
    // 00430C62: mov ecx,dword ptr [esi+14h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C65: xor eax,eax
    {const uint32 r_=((eax)^(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00430C67: mov dword ptr [ecx+8],eax
    St<uint32>((uintptr_t)(uint32)(ecx+0x8u),(uint32)(eax));
    // 00430C6A: mov edx,dword ptr [esi+14h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C6D: mov dword ptr [edx+20h],eax
    St<uint32>((uintptr_t)(uint32)(edx+0x20u),(uint32)(eax));
    // 00430C70: mov ecx,dword ptr [esi+14h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C73: mov dword ptr [ecx+24h],eax
    St<uint32>((uintptr_t)(uint32)(ecx+0x24u),(uint32)(eax));
    // 00430C76: mov edx,dword ptr [esi+14h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 00430C79: mov dword ptr [edx+28h],3F800000h
    St<uint32>((uintptr_t)(uint32)(edx+0x28u),(uint32)(0x3F800000u));
    // 00430C80: mov ebx,dword ptr [esi+00000390h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x390u)));
    // 00430C86: mov bp,word ptr [esi+000003A6h]
    ebp=(ebp&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)))&0xFFFFu);
    // 00430C8D: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430C8F: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430C92: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430C95: movzx edx,bx
    edx=(uint32)((ebx&0xFFFFu));
    // 00430C98: lea eax,[esp+64h]
    eax=(uint32)((uint32)(F+536));
    // 00430C9C: push eax
    St<uint32>(F+432,eax);
    // 00430C9D: add ecx,dword ptr [edx*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430CA4: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430CA8: movzx edi,bp
    edi=(uint32)((ebp&0xFFFFu));
    // 00430CAB: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430CB0: mov ecx,edi
    ecx=(uint32)(edi);
    // 00430CB2: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430CB8: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00430CBB: mov edx,edi
    edx=(uint32)(edi);
    // 00430CBD: push eax
    St<uint32>(F+432,eax);
    // 00430CBE: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430CC1: mov edi,dword ptr [edx*4+0096F974h]
    edi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00430CC8: lea eax,[esp+000000A0h]
    eax=(uint32)((uint32)(F+592));
    // 00430CCF: push eax
    St<uint32>(F+428,eax);
    // 00430CD0: add ecx,edi
    {const uint32 a_=(ecx)&4294967295,b_=(edi)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430CD2: call 00420A60
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A60)(ecx,edx,Ld<uint32>(F+428));
    // 00430CD7: push eax
    St<uint32>(F+428,eax);
    // 00430CD8: lea eax,[esp+000000D8h]
    eax=(uint32)((uint32)(F+644));
    // 00430CDF: push eax
    St<uint32>(F+424,eax);
    // 00430CE0: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430CE5: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430CE8: add esp,0Ch
    // 00430CEB: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430CED: mov eax,dword ptr [esi+00000394h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x394u)));
    // 00430CF3: fld st(0)
    x2=x1;
    // 00430CF5: lea ecx,[esp+64h]
    ecx=(uint32)((uint32)(F+536));
    // 00430CF9: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430CFB: push ecx
    St<uint32>(F+432,ecx);
    // 00430CFC: fld st(2)
    x3=x0;
    // 00430CFE: mov ecx,eax
    ecx=(uint32)(eax);
    // 00430D00: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430D02: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430D05: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430D08: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430D0A: movzx edx,ax
    edx=(uint32)((eax&0xFFFFu));
    // 00430D0D: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430D0F: mov dword ptr [esp+14h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 00430D13: add ecx,dword ptr [edx*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430D1A: movzx edi,bp
    edi=(uint32)((ebp&0xFFFFu));
    // 00430D1D: fstp st(2)
    x0=x2;
    // 00430D1F: fstp st(0)
    // 00430D21: fstp dword ptr [esp+2Ch]
    St<float>((F+476),Veh83DStore(x0));
    // 00430D25: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430D2A: mov ecx,edi
    ecx=(uint32)(edi);
    // 00430D2C: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430D32: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00430D35: push eax
    St<uint32>(F+432,eax);
    // 00430D36: lea eax,[esp+000000ECh]
    eax=(uint32)((uint32)(F+668));
    // 00430D3D: mov edx,edi
    edx=(uint32)(edi);
    // 00430D3F: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430D42: push eax
    St<uint32>(F+428,eax);
    // 00430D43: add ecx,dword ptr [edx*4+0096F974h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430D4A: call 00420A60
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A60)(ecx,edx,Ld<uint32>(F+428));
    // 00430D4F: push eax
    St<uint32>(F+428,eax);
    // 00430D50: lea eax,[esp+000000E4h]
    eax=(uint32)((uint32)(F+656));
    // 00430D57: push eax
    St<uint32>(F+424,eax);
    // 00430D58: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430D5D: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430D60: add esp,0Ch
    // 00430D63: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430D65: fld st(0)
    x2=x1;
    // 00430D67: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430D69: fld st(2)
    x3=x0;
    // 00430D6B: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430D6D: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430D6F: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430D71: fstp st(2)
    x0=x2;
    // 00430D73: fstp st(0)
    // 00430D75: fadd dword ptr [esp+28h]
    x0=Veh83DAdd(x0,(double)Ld<float>((F+476)));
    // 00430D79: fdivr dword ptr [esp+28h]
    x0=Veh83DDiv((double)Ld<float>((F+476)),x0);
    // 00430D7D: fcomp dword ptr [esp+24h]
    fcc=X87Compare(x0,(double)Ld<float>((F+472)));
    // 00430D81: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430D83: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430D86: jne 00430E62
    if(!zf) goto L_00430E62;
    // 00430D8C: mov di,word ptr [esi+000003A4h]
    edi=(edi&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A4u)))&0xFFFFu);
    // 00430D93: lea ecx,[esp+64h]
    ecx=(uint32)((uint32)(F+536));
    // 00430D97: push ecx
    St<uint32>(F+432,ecx);
    // 00430D98: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430D9A: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430D9D: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430DA0: movzx edx,bx
    edx=(uint32)((ebx&0xFFFFu));
    // 00430DA3: add ecx,dword ptr [edx*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430DAA: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430DAE: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430DB3: movzx ecx,di
    ecx=(uint32)((edi&0xFFFFu));
    // 00430DB6: mov edx,ecx
    edx=(uint32)(ecx);
    // 00430DB8: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430DBE: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00430DC1: push eax
    St<uint32>(F+432,eax);
    // 00430DC2: lea eax,[esp+000000A0h]
    eax=(uint32)((uint32)(F+592));
    // 00430DC9: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430DCC: push eax
    St<uint32>(F+428,eax);
    // 00430DCD: add ecx,dword ptr [edx*4+0096F974h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430DD4: call 00420A60
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A60)(ecx,edx,Ld<uint32>(F+428));
    // 00430DD9: push eax
    St<uint32>(F+428,eax);
    // 00430DDA: lea eax,[esp+000000D8h]
    eax=(uint32)((uint32)(F+644));
    // 00430DE1: push eax
    St<uint32>(F+424,eax);
    // 00430DE2: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430DE7: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430DEA: add esp,0Ch
    // 00430DED: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430DEF: lea ecx,[esp+64h]
    ecx=(uint32)((uint32)(F+536));
    // 00430DF3: fld st(0)
    x2=x1;
    // 00430DF5: push ecx
    St<uint32>(F+432,ecx);
    // 00430DF6: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430DF8: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430DFA: fld st(2)
    x3=x0;
    // 00430DFC: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430DFF: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430E01: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430E04: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430E06: movzx edx,bx
    edx=(uint32)((ebx&0xFFFFu));
    // 00430E09: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430E0B: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430E0F: add ecx,dword ptr [edx*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430E16: fstp st(2)
    x0=x2;
    // 00430E18: fstp st(0)
    // 00430E1A: fstp dword ptr [esp+18h]
    St<float>((F+456),Veh83DStore(x0));
    // 00430E1E: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430E23: push eax
    St<uint32>(F+432,eax);
    // 00430E24: lea eax,[esp+00000094h]
    eax=(uint32)((uint32)(F+580));
    // 00430E2B: push eax
    St<uint32>(F+428,eax);
    // 00430E2C: lea ecx,[esp+000000F0h]
    ecx=(uint32)((uint32)(F+668));
    // 00430E33: push ecx
    St<uint32>(F+424,ecx);
    // 00430E34: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430E39: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430E3C: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430E3E: add esp,0Ch
    // 00430E41: fld st(0)
    x2=x1;
    // 00430E43: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430E45: fld st(2)
    x3=x0;
    // 00430E47: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430E49: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430E4B: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430E4D: fstp st(2)
    x0=x2;
    // 00430E4F: fstp st(0)
    // 00430E51: fadd dword ptr [esp+14h]
    x0=Veh83DAdd(x0,(double)Ld<float>((F+456)));
    // 00430E55: fld dword ptr [esp+14h]
    x1=(double)Ld<float>((F+456));
    // 00430E59: fadd dword ptr [esp+28h]
    x1=Veh83DAdd(x1,(double)Ld<float>((F+476)));
    // 00430E5D: jmp 00430FD3
    goto L_00430FD3;
L_00430E62:;
    // 00430E62: push esi
    St<uint32>(F+432,esi);
    // 00430E63: call 0042DE80
    ((void(__cdecl*)(uint32))0x42DE80)(Ld<uint32>(F+432));
    // 00430E68: mov ebx,dword ptr [esi+00000390h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x390u)));
    // 00430E6E: mov bp,word ptr [esi+000003A6h]
    ebp=(ebp&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)))&0xFFFFu);
    // 00430E75: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430E77: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430E7A: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430E7D: movzx eax,bx
    eax=(uint32)((ebx&0xFFFFu));
    // 00430E80: add esp,4
    // 00430E83: lea edx,[esp+64h]
    edx=(uint32)((uint32)(F+536));
    // 00430E87: push edx
    St<uint32>(F+432,edx);
    // 00430E88: add ecx,dword ptr [eax*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430E8F: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430E93: movzx edi,bp
    edi=(uint32)((ebp&0xFFFFu));
    // 00430E96: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430E9B: push eax
    St<uint32>(F+432,eax);
    // 00430E9C: lea ecx,[esp+000000A0h]
    ecx=(uint32)((uint32)(F+592));
    // 00430EA3: push ecx
    St<uint32>(F+428,ecx);
    // 00430EA4: mov ecx,edi
    ecx=(uint32)(edi);
    // 00430EA6: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430EAC: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00430EAF: mov edx,edi
    edx=(uint32)(edi);
    // 00430EB1: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430EB4: add ecx,dword ptr [edx*4+0096F974h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430EBB: call 00420A60
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A60)(ecx,edx,Ld<uint32>(F+428));
    // 00430EC0: push eax
    St<uint32>(F+428,eax);
    // 00430EC1: lea eax,[esp+000000D8h]
    eax=(uint32)((uint32)(F+644));
    // 00430EC8: push eax
    St<uint32>(F+424,eax);
    // 00430EC9: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430ECE: mov di,word ptr [esi+000003A4h]
    edi=(edi&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A4u)))&0xFFFFu);
    // 00430ED5: add esp,0Ch
    // 00430ED8: lea ecx,[esp+000000E8h]
    ecx=(uint32)((uint32)(F+668));
    // 00430EDF: push ecx
    St<uint32>(F+432,ecx);
    // 00430EE0: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430EE2: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430EE5: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430EE8: movzx edx,bx
    edx=(uint32)((ebx&0xFFFFu));
    // 00430EEB: mov dword ptr [esp+54h],eax
    St<uint32>((F+516),(uint32)(eax));
    // 00430EEF: add ecx,dword ptr [edx*4+0096F854h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430EF6: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430EFA: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430EFF: movzx ecx,di
    ecx=(uint32)((edi&0xFFFFu));
    // 00430F02: mov edx,ecx
    edx=(uint32)(ecx);
    // 00430F04: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430F0A: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00430F0D: push eax
    St<uint32>(F+432,eax);
    // 00430F0E: lea eax,[esp+000000E0h]
    eax=(uint32)((uint32)(F+656));
    // 00430F15: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00430F18: push eax
    St<uint32>(F+428,eax);
    // 00430F19: add ecx,dword ptr [edx*4+0096F974h]
    {const uint32 a_=(ecx)&4294967295,b_=(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)))&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430F20: call 00420A60
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A60)(ecx,edx,Ld<uint32>(F+428));
    // 00430F25: push eax
    St<uint32>(F+428,eax);
    // 00430F26: lea eax,[esp+00000100h]
    eax=(uint32)((uint32)(F+684));
    // 00430F2D: push eax
    St<uint32>(F+424,eax);
    // 00430F2E: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430F33: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430F36: add esp,0Ch
    // 00430F39: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430F3B: lea ecx,[esp+000000E8h]
    ecx=(uint32)((uint32)(F+668));
    // 00430F42: fld st(0)
    x2=x1;
    // 00430F44: push ecx
    St<uint32>(F+432,ecx);
    // 00430F45: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430F47: mov ecx,ebx
    ecx=(uint32)(ebx);
    // 00430F49: fld st(2)
    x3=x0;
    // 00430F4B: movzx edx,bx
    edx=(uint32)((ebx&0xFFFFu));
    // 00430F4E: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430F50: shr ecx,10h
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430F53: mov dword ptr [esp+14h],ebx
    St<uint32>((F+452),(uint32)(ebx));
    // 00430F57: imul ecx,ecx,1Ch
    ecx=(uint32)((uint32)((int32)ecx*(int32)0x1Cu));
    // 00430F5A: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430F5C: mov ebx,dword ptr [edx*4+0096F854h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F854u)));
    // 00430F63: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430F65: fstp st(2)
    x0=x2;
    // 00430F67: fstp st(0)
    // 00430F69: fstp dword ptr [esp+18h]
    St<float>((F+456),Veh83DStore(x0));
    // 00430F6D: add ecx,ebx
    {const uint32 a_=(ecx)&4294967295,b_=(ebx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00430F6F: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00430F74: push eax
    St<uint32>(F+432,eax);
    // 00430F75: lea eax,[esp+00000094h]
    eax=(uint32)((uint32)(F+580));
    // 00430F7C: push eax
    St<uint32>(F+428,eax);
    // 00430F7D: lea ecx,[esp+0000008Ch]
    ecx=(uint32)((uint32)(F+568));
    // 00430F84: push ecx
    St<uint32>(F+424,ecx);
    // 00430F85: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00430F8A: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00430F8D: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00430F8F: mov eax,dword ptr [esp+5Ch]
    eax=(uint32)(Ld<uint32>((F+516)));
    // 00430F93: fld st(0)
    x2=x1;
    // 00430F95: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00430F98: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00430F9A: mov eax,dword ptr [eax]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00430F9C: fld st(2)
    x3=x0;
    // 00430F9E: mov dword ptr [esp+34h],eax
    St<uint32>((F+476),(uint32)(eax));
    // 00430FA2: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00430FA4: mov dword ptr [esp+2Ch],edx
    St<uint32>((F+468),(uint32)(edx));
    // 00430FA8: add esp,0Ch
    // 00430FAB: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00430FAD: fsqrt
    x2=Veh83DSqrt(x2);
    // 00430FAF: fstp st(2)
    x0=x2;
    // 00430FB1: fstp st(0)
    // 00430FB3: fsubr dword ptr [esp+14h]
    x0=Veh83DSub((double)Ld<float>((F+456)),x0);
    // 00430FB7: fld dword ptr [esp+28h]
    x1=(double)Ld<float>((F+476));
    // 00430FBB: fmul dword ptr [esp+28h]
    x1=Veh83DMul(x1,(double)Ld<float>((F+476)));
    // 00430FBF: fld dword ptr [esp+20h]
    x2=(double)Ld<float>((F+468));
    // 00430FC3: fmul dword ptr [esp+20h]
    x2=Veh83DMul(x2,(double)Ld<float>((F+468)));
    // 00430FC7: faddp st(1),st
    x1=Veh83DAdd(x1,x2);
    // 00430FC9: fsqrt
    x1=Veh83DSqrt(x1);
    // 00430FCB: fxch st(1)
    {const double t_=x1;x1=x0;x0=t_;}
    // 00430FCD: fxch st(1)
    {const double t_=x1;x1=x0;x0=t_;}
    // 00430FCF: fadd dword ptr [esp+14h]
    x1=Veh83DAdd(x1,(double)Ld<float>((F+456)));
L_00430FD3:;
    // 00430FD3: fdivp st(1),st
    x0=Veh83DDiv(x0,x1);
    // 00430FD5: fstp dword ptr [esp+20h]
    St<float>((F+468),Veh83DStore(x0));
    // 00430FD9: fld dword ptr ds:[00858B50h]
    x0=(double)F32(0x0u);
    // 00430FDF: fcomp dword ptr [esp+20h]
    fcc=X87Compare(x0,(double)Ld<float>((F+468)));
    // 00430FE3: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430FE5: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430FE8: je 00431005
    if(zf) goto L_00431005;
    // 00430FEA: fld dword ptr ds:[00858624h]
    x0=(double)F32(0x3F800000u);
    // 00430FF0: fcomp dword ptr [esp+20h]
    fcc=X87Compare(x0,(double)Ld<float>((F+468)));
    // 00430FF4: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00430FF6: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00430FF9: jp 0043100D
    if(pf) goto L_0043100D;
    // 00430FFB: mov dword ptr [esp+20h],3F800000h
    St<uint32>((F+468),(uint32)(0x3F800000u));
    // 00431003: jmp 0043100D
    goto L_0043100D;
L_00431005:;
    // 00431005: mov dword ptr [esp+20h],0
    St<uint32>((F+468),(uint32)(0x0u));
L_0043100D:;
    // 0043100D: movsx ecx,byte ptr [esi+000003B5h]
    ecx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B5u))));
    // 00431014: movzx eax,di
    eax=(uint32)((edi&0xFFFFu));
    // 00431017: mov edx,eax
    edx=(uint32)(eax);
    // 00431019: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043101E: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 00431021: mov dword ptr [esp+10h],ecx
    St<uint32>((F+452),(uint32)(ecx));
    // 00431025: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431028: mov ecx,dword ptr [edx*4+0096F974h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 0043102F: fild dword ptr [esp+10h]
    x0=(double)Ld<int32>((F+452));
    // 00431033: movsx edx,byte ptr [ecx+eax+8]
    edx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(ecx+eax+0x8u))));
    // 00431038: movzx eax,di
    eax=(uint32)((edi&0xFFFFu));
    // 0043103B: mov ecx,eax
    ecx=(uint32)(eax);
    // 0043103D: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00431042: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 00431045: shr ecx,0Ah
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00431048: mov dword ptr [esp+10h],edx
    St<uint32>((F+452),(uint32)(edx));
    // 0043104C: mov edx,dword ptr [ecx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F974u)));
    // 00431053: movsx eax,byte ptr [edx+eax+9]
    eax=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(edx+eax+0x9u))));
    // 00431058: fild dword ptr [esp+10h]
    x1=(double)Ld<int32>((F+452));
    // 0043105C: fmul dword ptr ds:[00858C58h]
    x1=Veh83DMul(x1,(double)F32(0x3C23D70Au));
    // 00431062: movsx ecx,byte ptr [esi+000003B6h]
    ecx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B6u))));
    // 00431069: mov dword ptr [esp+10h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 0043106D: movzx eax,bp
    eax=(uint32)((ebp&0xFFFFu));
    // 00431070: mov edx,eax
    edx=(uint32)(eax);
    // 00431072: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00431077: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 0043107A: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 0043107D: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 0043107F: fstp dword ptr [esp+70h]
    St<float>((F+548),Veh83DStore(x1));
    // 00431083: fild dword ptr [esp+10h]
    x1=(double)Ld<int32>((F+452));
    // 00431087: mov dword ptr [esp+10h],ecx
    St<uint32>((F+452),(uint32)(ecx));
    // 0043108B: mov ecx,dword ptr [edx*4+0096F974h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00431092: movsx edx,byte ptr [ecx+eax+8]
    edx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(ecx+eax+0x8u))));
    // 00431097: fmul dword ptr ds:[00858C58h]
    x1=Veh83DMul(x1,(double)F32(0x3C23D70Au));
    // 0043109D: movzx eax,bp
    eax=(uint32)((ebp&0xFFFFu));
    // 004310A0: mov ecx,eax
    ecx=(uint32)(eax);
    // 004310A2: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 004310A4: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004310A9: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 004310AC: fstp dword ptr [esp+60h]
    St<float>((F+532),Veh83DStore(x1));
    // 004310B0: fstp st(0)
    // 004310B2: fild dword ptr [esp+10h]
    x0=(double)Ld<int32>((F+452));
    // 004310B6: mov dword ptr [esp+10h],edx
    St<uint32>((F+452),(uint32)(edx));
    // 004310BA: shr ecx,0Ah
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 004310BD: fild dword ptr [esp+10h]
    x1=(double)Ld<int32>((F+452));
    // 004310C1: mov edx,dword ptr [ecx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F974u)));
    // 004310C8: movsx eax,byte ptr [edx+eax+9]
    eax=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(edx+eax+0x9u))));
    // 004310CD: fmul dword ptr ds:[00858C58h]
    x1=Veh83DMul(x1,(double)F32(0x3C23D70Au));
    // 004310D3: movzx ecx,di
    ecx=(uint32)((edi&0xFFFFu));
    // 004310D6: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 004310D8: mov dword ptr [esp+10h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 004310DC: mov edx,ecx
    edx=(uint32)(ecx);
    // 004310DE: fstp dword ptr [esp+000000A8h]
    St<float>((F+604),Veh83DStore(x1));
    // 004310E5: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 004310EB: fild dword ptr [esp+10h]
    x1=(double)Ld<int32>((F+452));
    // 004310EF: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 004310F2: fmul dword ptr ds:[00858C58h]
    x1=Veh83DMul(x1,(double)F32(0x3C23D70Au));
    // 004310F8: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004310FB: mov eax,dword ptr [edx*4+0096F974h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00431102: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 00431104: add ecx,eax
    {const uint32 a_=(ecx)&4294967295,b_=(eax)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00431106: fstp dword ptr [esp+5Ch]
    St<float>((F+528),Veh83DStore(x1));
    // 0043110A: fstp st(0)
    // 0043110C: call 0044DB00
    x0=(double)((float(__fastcall*)(uint32,uint32))0x44DB00)(ecx,edx);
    // 00431111: movzx ecx,word ptr [esi+000003A6h]
    ecx=(uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)));
    // 00431118: movsx eax,byte ptr [esi+000003B7h]
    eax=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B7u))));
    // 0043111F: mov edx,ecx
    edx=(uint32)(ecx);
    // 00431121: mov dword ptr [esp+10h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 00431125: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 0043112B: fiadd dword ptr [esp+10h]
    x0=Veh83DAdd(x0,(double)Ld<int32>((F+452)));
    // 0043112F: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00431132: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431135: fmul dword ptr ds:[00858C50h]
    x0=Veh83DMul(x0,(double)F32(0x40ACCCCDu));
    // 0043113B: mov edi,dword ptr [edx*4+0096F974h]
    edi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00431142: add ecx,edi
    {const uint32 a_=(ecx)&4294967295,b_=(edi)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00431144: fstp dword ptr [esp+28h]
    St<float>((F+476),Veh83DStore(x0));
    // 00431148: call 0044DB00
    x0=(double)((float(__fastcall*)(uint32,uint32))0x44DB00)(ecx,edx);
    // 0043114D: movsx eax,byte ptr [esi+000003B8h]
    eax=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B8u))));
    // 00431154: mov dword ptr [esp+10h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 00431158: mov eax,dword ptr [esi+00000594h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x594u)));
    // 0043115E: cmp eax,0Ah
    {const uint32 a_=(eax)&4294967295,b_=(0xAu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431161: fiadd dword ptr [esp+10h]
    x0=Veh83DAdd(x0,(double)Ld<int32>((F+452)));
    // 00431165: fmul dword ptr ds:[00858C50h]
    x0=Veh83DMul(x0,(double)F32(0x40ACCCCDu));
    // 0043116B: fstp dword ptr [esp+14h]
    St<float>((F+456),Veh83DStore(x0));
    // 0043116F: jne 0043118D
    if(!zf) goto L_0043118D;
    // 00431171: fld dword ptr [esp+28h]
    x0=(double)Ld<float>((F+476));
    // 00431175: fadd dword ptr ds:[00859010h]
    x0=Veh83DAdd(x0,(double)F32(0x3FBA9FBFu));
    // 0043117B: fstp dword ptr [esp+28h]
    St<float>((F+476),Veh83DStore(x0));
    // 0043117F: fld dword ptr [esp+14h]
    x0=(double)Ld<float>((F+456));
    // 00431183: fadd dword ptr ds:[00859010h]
    x0=Veh83DAdd(x0,(double)F32(0x3FBA9FBFu));
    // 00431189: fstp dword ptr [esp+14h]
    St<float>((F+456),Veh83DStore(x0));
L_0043118D:;
    // 0043118D: mov eax,dword ptr [esi+00000394h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x394u)));
    // 00431193: mov edx,eax
    edx=(uint32)(eax);
    // 00431195: shr edx,10h
    {const uint32 a_=(edx)&4294967295,r_=(a_>>16);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>15)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431198: imul edx,edx,1Ch
    edx=(uint32)((uint32)((int32)edx*(int32)0x1Cu));
    // 0043119B: movzx ecx,ax
    ecx=(uint32)((eax&0xFFFFu));
    // 0043119E: mov dword ptr [esp+10h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 004311A2: mov eax,dword ptr [ecx*4+0096F854h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0x96F854u)));
    // 004311A9: movzx eax,byte ptr [eax+edx+19h]
    eax=(uint32)(Ld<uint8>((uintptr_t)(uint32)(eax+edx+0x19u)));
    // 004311AE: shr al,4
    {const uint32 a_=((eax&0xFFu))&255,r_=(a_>>4);zf=r_==0;sf=(r_>>7)&1;cf=(a_>>3)&1;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 004311B1: and al,3
    {const uint32 r_=(((eax&0xFFu))&(0x3u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 004311B3: mov byte ptr [esi+000003D1h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D1u),(uint8)((eax&0xFFu)));
    // 004311B9: push eax
    St<uint32>(F+432,eax);
    // 004311BA: call 00424130
    x0=(double)((float(__cdecl*)(uint32))0x424130)(Ld<uint32>(F+432));
    // 004311BF: fst dword ptr [esi+000003D4h]
    St<float>((uintptr_t)(uint32)(esi+0x3D4u),Veh83DStore(x0));
    // 004311C5: movzx ecx,byte ptr [esi+000003D0h]
    ecx=(uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3D0u)));
    // 004311CC: mov dword ptr [esp+14h],ecx
    St<uint32>((F+452),(uint32)(ecx));
    // 004311D0: mov cx,word ptr [esi+000003A6h]
    ecx=(ecx&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)))&0xFFFFu);
    // 004311D7: fild dword ptr [esp+14h]
    x1=(double)Ld<int32>((F+452));
    // 004311DB: movzx eax,cx
    eax=(uint32)((ecx&0xFFFFu));
    // 004311DE: fmul st,st(1)
    x1=Veh83DMul(x1,x0);
    // 004311E0: mov edx,eax
    edx=(uint32)(eax);
    // 004311E2: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004311E7: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 004311EA: fstp dword ptr [esi+000003C8h]
    St<float>((uintptr_t)(uint32)(esi+0x3C8u),Veh83DStore(x1));
    // 004311F0: fstp st(0)
    // 004311F2: fld dword ptr [esp+18h]
    x0=(double)Ld<float>((F+456));
    // 004311F6: fmul dword ptr [esp+000000ACh]
    x0=Veh83DMul(x0,(double)Ld<float>((F+604)));
    // 004311FD: fstp dword ptr [esp+54h]
    St<float>((F+516),Veh83DStore(x0));
    // 00431201: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431204: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 0043120B: movsx eax,word ptr [edx+eax+2]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+eax+0x2u))));
    // 00431210: mov dword ptr [esp+14h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 00431214: mov di,word ptr [esi+000003A4h]
    edi=(edi&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A4u)))&0xFFFFu);
    // 0043121B: fild dword ptr [esp+14h]
    x0=(double)Ld<int32>((F+452));
    // 0043121F: movzx eax,cx
    eax=(uint32)((ecx&0xFFFFu));
    // 00431222: fmul dword ptr ds:[00858C48h]
    x0=Veh83DMul(x0,(double)F32(0x3E000000u));
    // 00431228: mov edx,eax
    edx=(uint32)(eax);
    // 0043122A: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043122F: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 00431232: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431235: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 0043123C: movsx eax,word ptr [edx+eax]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+eax))));
    // 00431240: mov dword ptr [esp+14h],eax
    St<uint32>((F+452),(uint32)(eax));
    // 00431244: movzx eax,di
    eax=(uint32)((edi&0xFFFFu));
    // 00431247: fsub dword ptr [esp+54h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+516)));
    // 0043124B: fld dword ptr [esp+18h]
    x1=(double)Ld<float>((F+456));
    // 0043124F: mov edx,eax
    edx=(uint32)(eax);
    // 00431251: fmul dword ptr [esp+60h]
    x1=Veh83DMul(x1,(double)Ld<float>((F+528)));
    // 00431255: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043125A: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 0043125D: fstp dword ptr [esp+18h]
    St<float>((F+456),Veh83DStore(x1));
    // 00431261: fild dword ptr [esp+14h]
    x1=(double)Ld<int32>((F+452));
    // 00431265: fmul dword ptr ds:[00858C48h]
    x1=Veh83DMul(x1,(double)F32(0x3E000000u));
    // 0043126B: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 0043126E: fadd dword ptr [esp+18h]
    x1=Veh83DAdd(x1,(double)Ld<float>((F+456)));
    // 00431272: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00431279: mov dword ptr [esp+44h],0
    St<uint32>((F+500),(uint32)(0x0u));
    // 00431281: fstp dword ptr [esp+3Ch]
    St<float>((F+492),Veh83DStore(x1));
    // 00431285: fstp dword ptr [esp+40h]
    St<float>((F+496),Veh83DStore(x0));
    // 00431289: movsx eax,word ptr [edx+eax+2]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+eax+0x2u))));
    // 0043128E: fld dword ptr [esp+2Ch]
    x0=(double)Ld<float>((F+476));
    // 00431292: mov dword ptr [esp+1Ch],eax
    St<uint32>((F+460),(uint32)(eax));
    // 00431296: fmul dword ptr [esp+74h]
    x0=Veh83DMul(x0,(double)Ld<float>((F+548)));
    // 0043129A: movzx eax,di
    eax=(uint32)((edi&0xFFFFu));
    // 0043129D: fstp dword ptr [esp+14h]
    St<float>((F+452),Veh83DStore(x0));
    // 004312A1: mov edx,eax
    edx=(uint32)(eax);
    // 004312A3: fild dword ptr [esp+1Ch]
    x0=(double)Ld<int32>((F+460));
    // 004312A7: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 004312AC: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004312AF: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 004312B2: fmul dword ptr ds:[00858C48h]
    x0=Veh83DMul(x0,(double)F32(0x3E000000u));
    // 004312B8: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 004312BF: movsx eax,word ptr [edx+eax]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+eax))));
    // 004312C3: mov dword ptr [esp+1Ch],eax
    St<uint32>((F+460),(uint32)(eax));
    // 004312C7: fsub dword ptr [esp+14h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+452)));
    // 004312CB: fld dword ptr [esp+2Ch]
    x1=(double)Ld<float>((F+476));
    // 004312CF: fmul dword ptr [esp+64h]
    x1=Veh83DMul(x1,(double)Ld<float>((F+532)));
    // 004312D3: fstp dword ptr [esp+2Ch]
    St<float>((F+476),Veh83DStore(x1));
    // 004312D7: fild dword ptr [esp+1Ch]
    x1=(double)Ld<int32>((F+460));
    // 004312DB: movsx edx,byte ptr [esi+000003B6h]
    edx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B6u))));
    // 004312E2: movsx ebx,byte ptr [esi+000003B5h]
    ebx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B5u))));
    // 004312E9: fmul dword ptr ds:[00858C48h]
    x1=Veh83DMul(x1,(double)F32(0x3E000000u));
    // 004312EF: movzx eax,cx
    eax=(uint32)((ecx&0xFFFFu));
    // 004312F2: mov dword ptr [esp+1Ch],edx
    St<uint32>((F+460),(uint32)(edx));
    // 004312F6: fadd dword ptr [esp+2Ch]
    x1=Veh83DAdd(x1,(double)Ld<float>((F+476)));
    // 004312FA: movzx ecx,cx
    ecx=(uint32)((ecx&0xFFFFu));
    // 004312FD: mov dword ptr [esp+00000090h],0
    St<uint32>((F+576),(uint32)(0x0u));
    // 00431308: fstp dword ptr [esp+00000088h]
    St<float>((F+568),Veh83DStore(x1));
    // 0043130F: movzx edx,di
    edx=(uint32)((edi&0xFFFFu));
    // 00431312: fstp dword ptr [esp+0000008Ch]
    St<float>((F+572),Veh83DStore(x0));
    // 00431319: fild dword ptr [esp+1Ch]
    x0=(double)Ld<int32>((F+460));
    // 0043131D: mov dword ptr [esp+1Ch],ebx
    St<uint32>((F+460),(uint32)(ebx));
    // 00431321: mov ebx,eax
    ebx=(uint32)(eax);
    // 00431323: and eax,3FFh
    {const uint32 r_=((eax)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);eax=(uint32)(r_);}
    // 00431328: fild dword ptr [esp+1Ch]
    x1=(double)Ld<int32>((F+460));
    // 0043132C: imul eax,eax,0Eh
    eax=(uint32)((uint32)((int32)eax*(int32)0xEu));
    // 0043132F: shr ebx,0Ah
    {const uint32 a_=(ebx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ebx=(uint32)(r_);}
    // 00431332: mov ebx,dword ptr [ebx*4+0096F974h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx*4+0x96F974u)));
    // 00431339: movsx eax,byte ptr [ebx+eax+9]
    eax=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(ebx+eax+0x9u))));
    // 0043133E: mov dword ptr [esp+1Ch],eax
    St<uint32>((F+460),(uint32)(eax));
    // 00431342: mov eax,ecx
    eax=(uint32)(ecx);
    // 00431344: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 0043134A: fild dword ptr [esp+1Ch]
    x2=(double)Ld<int32>((F+460));
    // 0043134E: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 00431351: fmul dword ptr ds:[00858C58h]
    x2=Veh83DMul(x2,(double)F32(0x3C23D70Au));
    // 00431357: shr eax,0Ah
    {const uint32 a_=(eax)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043135A: mov eax,dword ptr [eax*4+0096F974h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F974u)));
    // 00431361: movsx ecx,byte ptr [eax+ecx+8]
    ecx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(eax+ecx+0x8u))));
    // 00431366: mov eax,edx
    eax=(uint32)(edx);
    // 00431368: and edx,3FFh
    {const uint32 r_=((edx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);edx=(uint32)(r_);}
    // 0043136E: imul edx,edx,0Eh
    edx=(uint32)((uint32)((int32)edx*(int32)0xEu));
    // 00431371: fmul st,st(2)
    x2=Veh83DMul(x2,x0);
    // 00431373: fstp dword ptr [esp]
    St<float>((F+432),Veh83DStore(x2));
    // 00431376: mov dword ptr [esp+1Ch],ecx
    St<uint32>((F+460),(uint32)(ecx));
    // 0043137A: shr eax,0Ah
    {const uint32 a_=(eax)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 0043137D: mov ecx,dword ptr [eax*4+0096F974h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F974u)));
    // 00431384: fild dword ptr [esp+1Ch]
    x2=(double)Ld<int32>((F+460));
    // 00431388: movsx edx,byte ptr [ecx+edx+9]
    edx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(ecx+edx+0x9u))));
    // 0043138D: fmul dword ptr ds:[00858C58h]
    x2=Veh83DMul(x2,(double)F32(0x3C23D70Au));
    // 00431393: movzx edi,di
    edi=(uint32)((edi&0xFFFFu));
    // 00431396: mov eax,edi
    eax=(uint32)(edi);
    // 00431398: sub esp,0Ch
    // 0043139B: fmul st,st(2)
    x2=Veh83DMul(x2,x0);
    // 0043139D: mov dword ptr [esp+28h],edx
    St<uint32>((F+460),(uint32)(edx));
    // 004313A1: and edi,3FFh
    {const uint32 r_=((edi)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);edi=(uint32)(r_);}
    // 004313A7: imul edi,edi,0Eh
    edi=(uint32)((uint32)((int32)edi*(int32)0xEu));
    // 004313AA: fstp dword ptr [esp+8]
    St<float>((F+428),Veh83DStore(x2));
    // 004313AE: fild dword ptr [esp+28h]
    x2=(double)Ld<int32>((F+460));
    // 004313B2: fmul dword ptr ds:[00858C58h]
    x2=Veh83DMul(x2,(double)F32(0x3C23D70Au));
    // 004313B8: shr eax,0Ah
    {const uint32 a_=(eax)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 004313BB: mov ecx,dword ptr [eax*4+0096F974h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F974u)));
    // 004313C2: movsx edx,byte ptr [ecx+edi+8]
    edx=(uint32)((uint32)(int32)(int8)(Ld<uint8>((uintptr_t)(uint32)(ecx+edi+0x8u))));
    // 004313C7: mov dword ptr [esp+28h],edx
    St<uint32>((F+460),(uint32)(edx));
    // 004313CB: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 004313CD: lea eax,[esp+48h]
    eax=(uint32)((uint32)(F+492));
    // 004313D1: lea ecx,[esp+00000094h]
    ecx=(uint32)((uint32)(F+568));
    // 004313D8: fstp dword ptr [esp+4]
    St<float>((F+424),Veh83DStore(x2));
    // 004313DC: fild dword ptr [esp+28h]
    x2=(double)Ld<int32>((F+460));
    // 004313E0: fmul dword ptr ds:[00858C58h]
    x2=Veh83DMul(x2,(double)F32(0x3C23D70Au));
    // 004313E6: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 004313E8: fstp dword ptr [esp]
    St<float>((F+420),Veh83DStore(x2));
    // 004313EB: push eax
    St<uint32>(F+416,eax);
    // 004313EC: push ecx
    St<uint32>(F+412,ecx);
    // 004313ED: fstp st(0)
    // 004313EF: fstp st(0)
    // 004313F1: call 0043C710
    x0=(double)((float(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32))0x43C710)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004313F6: fld dword ptr ds:[00858C4Ch]
    x1=(double)F32(0x447A0000u);
    // 004313FC: add esp,18h
    // 004313FF: fdiv dword ptr [esi+000003C8h]
    x1=Veh83DDiv(x1,(double)Ld<float>((uintptr_t)(uint32)(esi+0x3C8u)));
    // 00431405: fmulp st(1),st
    x0=Veh83DMul(x0,x1);
    // 00431407: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 0043140C: mov edi,eax
    edi=(uint32)(eax);
    // 0043140E: mov dword ptr [esp+18h],edi
    St<uint32>((F+460),(uint32)(edi));
    // 00431412: fild dword ptr [esp+18h]
    x0=(double)Ld<int32>((F+460));
    // 00431416: mov dword ptr [esi+000003A0h],edi
    St<uint32>((uintptr_t)(uint32)(esi+0x3A0u),(uint32)(edi));
    // 0043141C: mov edx,dword ptr ds:[00B7CB84h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB84u)));
    // 00431422: fild dword ptr ds:[00B7CB84h]
    x1=(double)Ld<int32>((uintptr_t)(uint32)(0xB7CB84u));
    // 00431428: test edx,edx
    {const uint32 r_=((edx)&(edx))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043142A: jge 00431432
    if(sf==of) goto L_00431432;
    // 0043142C: fadd dword ptr ds:[00858C54h]
    x1=Veh83DAdd(x1,(double)F32(0x4F800000u));
L_00431432:;
    // 00431432: fld dword ptr [esp+20h]
    x2=(double)Ld<float>((F+468));
    // 00431436: fmul st,st(2)
    x2=Veh83DMul(x2,x0);
    // 00431438: fsubp st(1),st
    x1=Veh83DSub(x1,x2);
    // 0043143A: call 00821B40
    eax=(uint32)Heli83GFtol(x1);
    // 0043143F: mov edx,dword ptr [esp+5Ch]
    edx=(uint32)(Ld<uint32>((F+528)));
    // 00431443: mov ecx,dword ptr [esp+000000A8h]
    ecx=(uint32)(Ld<uint32>((F+604)));
    // 0043144A: mov dword ptr [esp+00000084h],ecx
    St<uint32>((F+568),(uint32)(ecx));
    // 00431451: mov ecx,dword ptr [esp+70h]
    ecx=(uint32)(Ld<uint32>((F+548)));
    // 00431455: mov dword ptr [esp+00000088h],edx
    St<uint32>((F+572),(uint32)(edx));
    // 0043145C: mov edx,dword ptr [esp+60h]
    edx=(uint32)(Ld<uint32>((F+532)));
    // 00431460: mov dword ptr [esp+38h],ecx
    St<uint32>((F+492),(uint32)(ecx));
    // 00431464: mov dword ptr [esp+3Ch],edx
    St<uint32>((F+496),(uint32)(edx));
    // 00431468: mov dx,word ptr [esi+000003A6h]
    edx=(edx&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A6u)))&0xFFFFu);
    // 0043146F: movzx ecx,dx
    ecx=(uint32)((edx&0xFFFFu));
    // 00431472: mov ebx,ecx
    ebx=(uint32)(ecx);
    // 00431474: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 0043147A: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 0043147D: mov dword ptr [esi+0000039Ch],eax
    St<uint32>((uintptr_t)(uint32)(esi+0x39Cu),(uint32)(eax));
    // 00431483: shr ebx,0Ah
    {const uint32 a_=(ebx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ebx=(uint32)(r_);}
    // 00431486: mov ebx,dword ptr [ebx*4+0096F974h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx*4+0x96F974u)));
    // 0043148D: mov dword ptr [esp+0000008Ch],0
    St<uint32>((F+576),(uint32)(0x0u));
    // 00431498: mov dword ptr [esp+40h],0
    St<uint32>((F+500),(uint32)(0x0u));
    // 004314A0: movsx ecx,word ptr [ebx+ecx+2]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(ebx+ecx+0x2u))));
    // 004314A5: mov dword ptr [esp+18h],ecx
    St<uint32>((F+460),(uint32)(ecx));
    // 004314A9: movzx ecx,dx
    ecx=(uint32)((edx&0xFFFFu));
    // 004314AC: fild dword ptr [esp+18h]
    x1=(double)Ld<int32>((F+460));
    // 004314B0: mov edx,ecx
    edx=(uint32)(ecx);
    // 004314B2: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 004314B8: fmul dword ptr ds:[00858C48h]
    x1=Veh83DMul(x1,(double)F32(0x3E000000u));
    // 004314BE: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 004314C1: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 004314C4: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 004314CB: movsx ecx,word ptr [edx+ecx]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+ecx))));
    // 004314CF: mov dx,word ptr [esi+000003A4h]
    edx=(edx&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x3A4u)))&0xFFFFu);
    // 004314D6: mov dword ptr [esp+18h],ecx
    St<uint32>((F+460),(uint32)(ecx));
    // 004314DA: movzx ecx,dx
    ecx=(uint32)((edx&0xFFFFu));
    // 004314DD: mov ebx,ecx
    ebx=(uint32)(ecx);
    // 004314DF: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 004314E5: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 004314E8: fsub dword ptr [esp+50h]
    x1=Veh83DSub(x1,(double)Ld<float>((F+516)));
    // 004314EC: fild dword ptr [esp+18h]
    x2=(double)Ld<int32>((F+460));
    // 004314F0: fmul dword ptr ds:[00858C48h]
    x2=Veh83DMul(x2,(double)F32(0x3E000000u));
    // 004314F6: shr ebx,0Ah
    {const uint32 a_=(ebx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);ebx=(uint32)(r_);}
    // 004314F9: fadd dword ptr [esp+14h]
    x2=Veh83DAdd(x2,(double)Ld<float>((F+456)));
    // 004314FD: mov ebx,dword ptr [ebx*4+0096F974h]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ebx*4+0x96F974u)));
    // 00431504: mov dword ptr [esp+00000100h],0
    St<uint32>((F+692),(uint32)(0x0u));
    // 0043150F: fstp dword ptr [esp+000000F8h]
    St<float>((F+684),Veh83DStore(x2));
    // 00431516: fstp dword ptr [esp+000000FCh]
    St<float>((F+688),Veh83DStore(x1));
    // 0043151D: movsx ecx,word ptr [ebx+ecx+2]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(ebx+ecx+0x2u))));
    // 00431522: mov dword ptr [esp+18h],ecx
    St<uint32>((F+460),(uint32)(ecx));
    // 00431526: movzx ecx,dx
    ecx=(uint32)((edx&0xFFFFu));
    // 00431529: fild dword ptr [esp+18h]
    x1=(double)Ld<int32>((F+460));
    // 0043152D: mov edx,ecx
    edx=(uint32)(ecx);
    // 0043152F: and ecx,3FFh
    {const uint32 r_=((ecx)&(0x3FFu))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00431535: fmul dword ptr ds:[00858C48h]
    x1=Veh83DMul(x1,(double)F32(0x3E000000u));
    // 0043153B: imul ecx,ecx,0Eh
    ecx=(uint32)((uint32)((int32)ecx*(int32)0xEu));
    // 0043153E: shr edx,0Ah
    {const uint32 a_=(edx)&4294967295,r_=(a_>>10);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>9)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431541: mov edx,dword ptr [edx*4+0096F974h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0x96F974u)));
    // 00431548: movsx ecx,word ptr [edx+ecx]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(edx+ecx))));
    // 0043154C: mov dword ptr [esp+18h],ecx
    St<uint32>((F+460),(uint32)(ecx));
    // 00431550: lea edx,[esp+00000104h]
    edx=(uint32)((uint32)(F+696));
    // 00431557: fsub dword ptr [esp+10h]
    x1=Veh83DSub(x1,(double)Ld<float>((F+452)));
    // 0043155B: push edx
    St<uint32>(F+432,edx);
    // 0043155C: fild dword ptr [esp+1Ch]
    x2=(double)Ld<int32>((F+460));
    // 00431560: mov edx,dword ptr ds:[00B7CB84h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB84u)));
    // 00431566: sub edx,eax
    {const uint32 a_=(edx)&4294967295,b_=(eax)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431568: lea ecx,[esp+00000094h]
    ecx=(uint32)((uint32)(F+580));
    // 0043156F: fmul dword ptr ds:[00858C48h]
    x2=Veh83DMul(x2,(double)F32(0x3E000000u));
    // 00431575: push ecx
    St<uint32>(F+428,ecx);
    // 00431576: mov dword ptr [esp+20h],edx
    St<uint32>((F+460),(uint32)(edx));
    // 0043157A: fadd dword ptr [esp+30h]
    x2=Veh83DAdd(x2,(double)Ld<float>((F+476)));
    // 0043157E: mov dword ptr [esp+000000ECh],0
    St<uint32>((F+664),(uint32)(0x0u));
    // 00431589: push edi
    St<uint32>(F+424,edi);
    // 0043158A: test edx,edx
    {const uint32 r_=((edx)&(edx))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043158C: fstp dword ptr [esp+000000E8h]
    St<float>((F+656),Veh83DStore(x2));
    // 00431593: fstp dword ptr [esp+000000ECh]
    St<float>((F+660),Veh83DStore(x1));
    // 0043159A: fild dword ptr [esp+24h]
    x1=(double)Ld<int32>((F+460));
    // 0043159E: jge 004315A6
    if(sf==of) goto L_004315A6;
    // 004315A0: fadd dword ptr ds:[00858C54h]
    x1=Veh83DAdd(x1,(double)F32(0x4F800000u));
L_004315A6:;
    // 004315A6: fdiv st,st(1)
    x1=Veh83DDiv(x1,x0);
    // 004315A8: push ecx
    St<uint32>(F+420,ecx);
    // 004315A9: lea eax,[esp+00000094h]
    eax=(uint32)((uint32)(F+568));
    // 004315B0: lea ecx,[esp+48h]
    ecx=(uint32)((uint32)(F+492));
    // 004315B4: lea edx,[esp+00000108h]
    edx=(uint32)((uint32)(F+684));
    // 004315BB: fstp dword ptr [esp]
    St<float>((F+420),Veh83DStore(x1));
    // 004315BE: push eax
    St<uint32>(F+416,eax);
    // 004315BF: push ecx
    St<uint32>(F+412,ecx);
    // 004315C0: fstp st(0)
    // 004315C2: push edx
    St<uint32>(F+408,edx);
    // 004315C3: lea eax,[esp+000000F8h]
    eax=(uint32)((uint32)(F+656));
    // 004315CA: push eax
    St<uint32>(F+404,eax);
    // 004315CB: call 0043C900
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x43C900)(Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004315D0: mov ecx,dword ptr [esp+50h]
    ecx=(uint32)(Ld<uint32>((F+484)));
    // 004315D4: mov edx,dword ptr [ecx+0096F854h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx+0x96F854u)));
    // 004315DA: mov eax,dword ptr [esp+00000094h]
    eax=(uint32)(Ld<uint32>((F+552)));
    // 004315E1: lea edi,[eax+edx]
    edi=(uint32)((uint32)(uintptr_t)(uint32)(eax+edx));
    // 004315E4: movzx eax,word ptr [esp+68h]
    eax=(uint32)(Ld<uint16>((F+508)));
    // 004315E9: mov ecx,dword ptr [eax*4+0096F854h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0x96F854u)));
    // 004315F0: mov edx,dword ptr [esp+74h]
    edx=(uint32)(Ld<uint32>((F+520)));
    // 004315F4: add esp,20h
    // 004315F7: lea ebp,[ecx+edx]
    ebp=(uint32)((uint32)(uintptr_t)(uint32)(ecx+edx));
    // 004315FA: lea eax,[esp+64h]
    eax=(uint32)((uint32)(F+536));
    // 004315FE: push eax
    St<uint32>(F+432,eax);
    // 004315FF: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 00431601: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 00431606: push eax
    St<uint32>(F+432,eax);
    // 00431607: lea ecx,[esp+000000A0h]
    ecx=(uint32)((uint32)(F+592));
    // 0043160E: push ecx
    St<uint32>(F+428,ecx);
    // 0043160F: mov ecx,edi
    ecx=(uint32)(edi);
    // 00431611: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+428));
    // 00431616: push eax
    St<uint32>(F+428,eax);
    // 00431617: lea edx,[esp+000000D8h]
    edx=(uint32)((uint32)(F+644));
    // 0043161E: push edx
    St<uint32>(F+424,edx);
    // 0043161F: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431624: mov ecx,dword ptr [eax]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00431626: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00431629: mov eax,dword ptr [eax+8]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 0043162C: mov dword ptr [esp+44h],ecx
    St<uint32>((F+492),(uint32)(ecx));
    // 00431630: add esp,0Ch
    // 00431633: lea ecx,[esp+38h]
    ecx=(uint32)((uint32)(F+492));
    // 00431637: mov dword ptr [esp+3Ch],edx
    St<uint32>((F+496),(uint32)(edx));
    // 0043163B: mov dword ptr [esp+40h],eax
    St<uint32>((F+500),(uint32)(eax));
    // 0043163F: call 004082C0
    x0=(double)((float(__fastcall*)(uint32,uint32))0x4082C0)(ecx,edx);
    // 00431644: fld dword ptr ds:[00858CA0h]
    x1=(double)F32(0x40000000u);
    // 0043164A: fdiv st,st(1)
    x1=Veh83DDiv(x1,x0);
    // 0043164C: push ecx
    St<uint32>(F+432,ecx);
    // 0043164D: lea ecx,[esp+3Ch]
    ecx=(uint32)((uint32)(F+492));
    // 00431651: lea edx,[esp+68h]
    edx=(uint32)((uint32)(F+536));
    // 00431655: fstp dword ptr [esp]
    St<float>((F+432),Veh83DStore(x1));
    // 00431658: push ecx
    St<uint32>(F+428,ecx);
    // 00431659: push edx
    St<uint32>(F+424,edx);
    // 0043165A: fstp st(0)
    // 0043165C: call 0040FEC0
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FEC0)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431661: push eax
    St<uint32>(F+420,eax);
    // 00431662: lea eax,[esp+000000A0h]
    eax=(uint32)((uint32)(F+580));
    // 00431669: push eax
    St<uint32>(F+416,eax);
    // 0043166A: lea ecx,[esp+000000B0h]
    ecx=(uint32)((uint32)(F+592));
    // 00431671: push ecx
    St<uint32>(F+412,ecx);
    // 00431672: call 0040FE30
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE30)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420));
    // 00431677: mov ebx,dword ptr [eax]
    ebx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00431679: mov dword ptr [esp+00000090h],ebx
    St<uint32>((F+556),(uint32)(ebx));
    // 00431680: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00431683: add esp,18h
    // 00431686: lea ecx,[esp+64h]
    ecx=(uint32)((uint32)(F+536));
    // 0043168A: mov dword ptr [esp+7Ch],edx
    St<uint32>((F+560),(uint32)(edx));
    // 0043168E: mov eax,dword ptr [eax+8]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 00431691: push ecx
    St<uint32>(F+432,ecx);
    // 00431692: mov ecx,edi
    ecx=(uint32)(edi);
    // 00431694: mov dword ptr [esp+00000084h],eax
    St<uint32>((F+564),(uint32)(eax));
    // 0043169B: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004316A0: fld dword ptr ds:[00858624h]
    x0=(double)F32(0x3F800000u);
    // 004316A6: fsub dword ptr [esp+24h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+472)));
    // 004316AA: lea edx,[esp+000000E8h]
    edx=(uint32)((uint32)(F+668));
    // 004316B1: push edx
    St<uint32>(F+432,edx);
    // 004316B2: fmul dword ptr [eax+8]
    x0=Veh83DMul(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x8u)));
    // 004316B5: fstp dword ptr [esp+34h]
    St<float>((F+484),Veh83DStore(x0));
    // 004316B9: mov ecx,ebp
    ecx=(uint32)(ebp);
    // 004316BB: call 00420A10
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x420A10)(ecx,edx,Ld<uint32>(F+432));
    // 004316C0: fld dword ptr [esp+24h]
    x0=(double)Ld<float>((F+472));
    // 004316C4: fmul dword ptr [eax+8]
    x0=Veh83DMul(x0,(double)Ld<float>((uintptr_t)(uint32)(eax+0x8u)));
    // 004316C7: mov al,byte ptr [esp+1Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+467)))&0xFFu);
    // 004316CB: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004316CD: mov dword ptr [esp+24h],4E6E6B28h
    St<uint32>((F+472),(uint32)(0x4E6E6B28u));
    // 004316D5: fadd dword ptr [esp+30h]
    x0=Veh83DAdd(x0,(double)Ld<float>((F+484)));
    // 004316D9: push 0
    St<uint32>(F+432,0x0u);
    // 004316DB: fstp dword ptr [esp+00000084h]
    St<float>((F+564),Veh83DStore(x0));
    // 004316E2: je 00431719
    if(zf) goto L_00431719;
    // 004316E4: mov ecx,dword ptr [esp+00000084h]
    ecx=(uint32)(Ld<uint32>((F+564)));
    // 004316EB: mov edx,dword ptr [esp+00000080h]
    edx=(uint32)(Ld<uint32>((F+560)));
    // 004316F2: push 1
    St<uint32>(F+428,0x1u);
    // 004316F4: lea eax,[esp+38h]
    eax=(uint32)((uint32)(F+484));
    // 004316F8: push eax
    St<uint32>(F+424,eax);
    // 004316F9: push ecx
    St<uint32>(F+420,ecx);
    // 004316FA: push edx
    St<uint32>(F+416,edx);
    // 004316FB: push ebx
    St<uint32>(F+412,ebx);
    // 004316FC: call 006EB690
    eax=((uint8(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32))0x6EB690)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431701: add esp,18h
    // 00431704: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431706: je 00431B2D
    if(zf) goto L_00431B2D;
    // 0043170C: mov ecx,dword ptr [esp+30h]
    ecx=(uint32)(Ld<uint32>((F+484)));
    // 00431710: mov dword ptr [esp+24h],ecx
    St<uint32>((F+472),(uint32)(ecx));
    // 00431714: jmp 004317DF
    goto L_004317DF;
L_00431719:;
    // 00431719: push 0
    St<uint32>(F+428,0x0u);
    // 0043171B: push 1
    St<uint32>(F+424,0x1u);
    // 0043171D: push 0
    St<uint32>(F+420,0x0u);
    // 0043171F: push 0
    St<uint32>(F+416,0x0u);
    // 00431721: push 0
    St<uint32>(F+412,0x0u);
    // 00431723: push 1
    St<uint32>(F+408,0x1u);
    // 00431725: lea edx,[esp+00000110h]
    edx=(uint32)((uint32)(F+680));
    // 0043172C: push edx
    St<uint32>(F+404,edx);
    // 0043172D: lea eax,[esp+00000130h]
    eax=(uint32)((uint32)(F+708));
    // 00431734: push eax
    St<uint32>(F+400,eax);
    // 00431735: lea ecx,[esp+0000009Ch]
    ecx=(uint32)((uint32)(F+556));
    // 0043173C: push 447A0000h
    St<uint32>(F+396,0x447A0000u);
    // 00431741: push ecx
    St<uint32>(F+392,ecx);
    // 00431742: call 005674E0
    eax=((uint8(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x5674E0)(Ld<uint32>(F+392),Ld<uint32>(F+396),Ld<uint32>(F+400),Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431747: add esp,2Ch
    // 0043174A: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043174C: je 00431759
    if(zf) goto L_00431759;
    // 0043174E: mov edx,dword ptr [esp+00000118h]
    edx=(uint32)(Ld<uint32>((F+716)));
    // 00431755: mov dword ptr [esp+24h],edx
    St<uint32>((F+472),(uint32)(edx));
L_00431759:;
    // 00431759: push 0
    St<uint32>(F+432,0x0u);
    // 0043175B: push 0
    St<uint32>(F+428,0x0u);
    // 0043175D: push 1
    St<uint32>(F+424,0x1u);
    // 0043175F: push 0
    St<uint32>(F+420,0x0u);
    // 00431761: push 0
    St<uint32>(F+416,0x0u);
    // 00431763: push 0
    St<uint32>(F+412,0x0u);
    // 00431765: push 1
    St<uint32>(F+408,0x1u);
    // 00431767: lea eax,[esp+00000110h]
    eax=(uint32)((uint32)(F+680));
    // 0043176E: push eax
    St<uint32>(F+404,eax);
    // 0043176F: lea ecx,[esp+00000130h]
    ecx=(uint32)((uint32)(F+708));
    // 00431776: push ecx
    St<uint32>(F+400,ecx);
    // 00431777: lea edx,[esp+0000009Ch]
    edx=(uint32)((uint32)(F+556));
    // 0043177E: push 0C47A0000h
    St<uint32>(F+396,0xC47A0000u);
    // 00431783: push edx
    St<uint32>(F+392,edx);
    // 00431784: call 005674E0
    eax=((uint8(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x5674E0)(Ld<uint32>(F+392),Ld<uint32>(F+396),Ld<uint32>(F+400),Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431789: add esp,2Ch
    // 0043178C: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043178E: je 004317DF
    if(zf) goto L_004317DF;
    // 00431790: fld dword ptr [esp+00000118h]
    x0=(double)Ld<float>((F+716));
    // 00431797: fsub dword ptr [esp+00000080h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+564)));
    // 0043179E: fcom dword ptr ds:[00858B50h]
    fcc=X87Compare(x0,(double)F32(0x0u));
    // 004317A4: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 004317A6: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004317A9: jp 004317AD
    if(pf) goto L_004317AD;
    // 004317AB: fchs
    x0=-x0;
L_004317AD:;
    // 004317AD: fld dword ptr [esp+24h]
    x1=(double)Ld<float>((F+472));
    // 004317B1: fsub dword ptr [esp+00000080h]
    x1=Veh83DSub(x1,(double)Ld<float>((F+564)));
    // 004317B8: fcom dword ptr ds:[00858B50h]
    fcc=X87Compare(x1,(double)F32(0x0u));
    // 004317BE: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 004317C0: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004317C3: jp 004317C7
    if(pf) goto L_004317C7;
    // 004317C5: fchs
    x1=-x1;
L_004317C7:;
    // 004317C7: fxch st(1)
    {const double t_=x1;x1=x0;x0=t_;}
    // 004317C9: fcomp st(1)
    fcc=X87Compare(x1,x0);
    // 004317CB: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 004317CD: fstp st(0)
    // 004317CF: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004317D2: jp 004317DF
    if(pf) goto L_004317DF;
    // 004317D4: mov eax,dword ptr [esp+00000118h]
    eax=(uint32)(Ld<uint32>((F+716)));
    // 004317DB: mov dword ptr [esp+24h],eax
    St<uint32>((F+472),(uint32)(eax));
L_004317DF:;
    // 004317DF: fld dword ptr [esp+24h]
    x0=(double)Ld<float>((F+472));
    // 004317E3: fcomp dword ptr ds:[00858FECh]
    fcc=X87Compare(x0,(double)F32(0x4E6E6B28u));
    // 004317E9: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 004317EB: test ah,44h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x44u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004317EE: jnp 00431AA5
    if(!pf) goto L_00431AA5;
    // 004317F4: fld dword ptr [esp+24h]
    x0=(double)Ld<float>((F+472));
    // 004317F8: fsub dword ptr [esp+00000080h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+564)));
    // 004317FF: fcom dword ptr ds:[00858B50h]
    fcc=X87Compare(x0,(double)F32(0x0u));
    // 00431805: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431807: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043180A: jp 0043180E
    if(pf) goto L_0043180E;
    // 0043180C: fchs
    x0=-x0;
L_0043180E:;
    // 0043180E: fcomp dword ptr ds:[00858F48h]
    fcc=X87Compare(x0,(double)F32(0x40E00000u));
    // 00431814: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431816: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431819: je 00431AA5
    if(zf) goto L_00431AA5;
    // 0043181F: movsx ecx,word ptr [esi+22h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00431823: push ecx
    St<uint32>(F+432,ecx);
    // 00431824: call 004C5A70
    eax=((uint8(__cdecl*)(uint32))0x4C5A70)(Ld<uint32>(F+432));
    // 00431829: add esp,4
    // 0043182C: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043182E: je 00431846
    if(zf) goto L_00431846;
    // 00431830: mov edx,dword ptr [esp+24h]
    edx=(uint32)(Ld<uint32>((F+472)));
    // 00431834: mov dword ptr [esp+00000080h],edx
    St<uint32>((F+564),(uint32)(edx));
    // 0043183B: mov word ptr [esi+000004A6h],0FFh
    St<uint16>((uintptr_t)(uint32)(esi+0x4A6u),(uint16)(0xFFu));
    // 00431844: jmp 0043185B
    goto L_0043185B;
L_00431846:;
    // 00431846: mov eax,dword ptr [esi]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi)));
    // 00431848: mov ecx,esi
    ecx=(uint32)(esi);
    // 0043184A: call dword ptr [eax+000000D4h]
    {typedef float(__fastcall*Fn_)(uint32,uint32);x0=(double)((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(eax+0xD4u)))(ecx,edx);}
    // 00431850: fadd dword ptr [esp+24h]
    x0=Veh83DAdd(x0,(double)Ld<float>((F+472)));
    // 00431854: fstp dword ptr [esp+00000080h]
    St<float>((F+564),Veh83DStore(x0));
L_0043185B:;
    // 0043185B: lea ecx,[esp+78h]
    ecx=(uint32)((uint32)(F+556));
    // 0043185F: push ecx
    St<uint32>(F+432,ecx);
    // 00431860: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431862: call 004241C0
    ((void(__fastcall*)(uint32,uint32,uint32))0x4241C0)(ecx,edx,Ld<uint32>(F+432));
    // 00431867: lea ebp,[esi+44h]
    ebp=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x44u));
    // 0043186A: mov dword ptr [esp+38h],0
    St<uint32>((F+492),(uint32)(0x0u));
    // 00431872: mov eax,dword ptr [esp+38h]
    eax=(uint32)(Ld<uint32>((F+492)));
    // 00431876: mov edx,ebp
    edx=(uint32)(ebp);
    // 00431878: mov dword ptr [edx],eax
    St<uint32>((uintptr_t)(uint32)(edx),(uint32)(eax));
    // 0043187A: mov dword ptr [esp+3Ch],0
    St<uint32>((F+496),(uint32)(0x0u));
    // 00431882: mov ecx,dword ptr [esp+3Ch]
    ecx=(uint32)(Ld<uint32>((F+496)));
    // 00431886: mov dword ptr [edx+4],ecx
    St<uint32>((uintptr_t)(uint32)(edx+0x4u),(uint32)(ecx));
    // 00431889: lea ecx,[esp+000000ACh]
    ecx=(uint32)((uint32)(F+608));
    // 00431890: push ecx
    St<uint32>(F+432,ecx);
    // 00431891: mov dword ptr [esp+44h],0
    St<uint32>((F+500),(uint32)(0x0u));
    // 00431899: mov eax,dword ptr [esp+44h]
    eax=(uint32)(Ld<uint32>((F+500)));
    // 0043189D: mov dword ptr [edx+8],eax
    St<uint32>((uintptr_t)(uint32)(edx+0x8u),(uint32)(eax));
    // 004318A0: push 42700000h
    St<uint32>(F+428,0x42700000u);
    // 004318A5: lea edx,[esp+0000010Ch]
    edx=(uint32)((uint32)(F+696));
    // 004318AC: push edx
    St<uint32>(F+424,edx);
    // 004318AD: lea eax,[esp+70h]
    eax=(uint32)((uint32)(F+536));
    // 004318B1: push eax
    St<uint32>(F+420,eax);
    // 004318B2: call 004119D0
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x4119D0)(Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428));
    // 004318B7: add esp,0Ch
    // 004318BA: push eax
    St<uint32>(F+428,eax);
    // 004318BB: lea ecx,[esp+000000A4h]
    ecx=(uint32)((uint32)(F+592));
    // 004318C2: push ecx
    St<uint32>(F+424,ecx);
    // 004318C3: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004318C8: fld dword ptr [esp+0000009Ch]
    x0=(double)Ld<float>((F+580));
    // 004318CF: fsub dword ptr [esp+000000D0h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+632)));
    // 004318D6: mov edx,dword ptr [eax]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 004318D8: mov ebx,dword ptr [esp+64h]
    ebx=(uint32)(Ld<uint32>((F+524)));
    // 004318DC: mov ecx,dword ptr [eax+4]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 004318DF: fstp dword ptr [esp+000000B8h]
    St<float>((F+608),Veh83DStore(x0));
    // 004318E6: mov dword ptr [esp+44h],edx
    St<uint32>((F+492),(uint32)(edx));
    // 004318EA: fld dword ptr [esp+000000A0h]
    x0=(double)Ld<float>((F+584));
    // 004318F1: mov edx,dword ptr [eax+8]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 004318F4: fsub dword ptr [esp+000000D4h]
    x0=Veh83DSub(x0,(double)Ld<float>((F+636)));
    // 004318FB: add esp,0Ch
    // 004318FE: cmp ebx,0Dh
    {const uint32 a_=(ebx)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431901: mov dword ptr [esp+3Ch],ecx
    St<uint32>((F+496),(uint32)(ecx));
    // 00431905: fstp dword ptr [esp+000000B0h]
    St<float>((F+612),Veh83DStore(x0));
    // 0043190C: mov dword ptr [esp+40h],edx
    St<uint32>((F+500),(uint32)(edx));
    // 00431910: je 0043194C
    if(zf) goto L_0043194C;
    // 00431912: cmp ebx,18h
    {const uint32 a_=(ebx)&4294967295,b_=(0x18u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431915: je 0043193E
    if(zf) goto L_0043193E;
    // 00431917: mov al,byte ptr [esp+1Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+467)))&0xFFu);
    // 0043191B: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043191D: mov al,byte ptr [esi+36h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431920: je 0043192B
    if(zf) goto L_0043192B;
    // 00431922: and al,7
    {const uint32 r_=(((eax&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431924: or al,18h
    {const uint32 r_=(((eax&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431926: mov byte ptr [esi+36h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((eax&0xFFu)));
    // 00431929: jmp 0043196D
    goto L_0043196D;
L_0043192B:;
    // 0043192B: mov cl,al
    ecx=(ecx&0xFFFFFF00u)|((uint32)((eax&0xFFu))&0xFFu);
    // 0043192D: and cl,0F8h
    {const uint32 r_=(((ecx&0xFFu))&(0xF8u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431930: cmp cl,18h
    {const uint32 a_=((ecx&0xFFu))&255,b_=(0x18u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 00431933: je 0043196D
    if(zf) goto L_0043196D;
    // 00431935: and al,7
    {const uint32 r_=(((eax&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431937: or al,10h
    {const uint32 r_=(((eax&0xFFu))|(0x10u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431939: mov byte ptr [esi+36h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((eax&0xFFu)));
    // 0043193C: jmp 0043196D
    goto L_0043196D;
L_0043193E:;
    // 0043193E: mov dl,byte ptr [esi+36h]
    edx=(edx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431941: and dl,7
    {const uint32 r_=(((edx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431944: or dl,18h
    {const uint32 r_=(((edx&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431947: mov byte ptr [esi+36h],dl
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((edx&0xFFu)));
    // 0043194A: jmp 0043196D
    goto L_0043196D;
L_0043194C:;
    // 0043194C: cmp byte ptr [esi+000003BAh],1
    {const uint32 a_=(Ld<uint8>((uintptr_t)(uint32)(esi+0x3BAu)))&255,b_=(0x1u)&255,r_=(a_-b_)&255;zf=r_==0;sf=(r_>>7)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>7)&1;pf=Parity(r_);}
    // 00431953: jne 00431961
    if(!zf) goto L_00431961;
    // 00431955: mov al,byte ptr [esi+36h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431958: and al,7
    {const uint32 r_=(((eax&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 0043195A: or al,10h
    {const uint32 r_=(((eax&0xFFu))|(0x10u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 0043195C: mov byte ptr [esi+36h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((eax&0xFFu)));
    // 0043195F: jmp 0043196D
    goto L_0043196D;
L_00431961:;
    // 00431961: mov cl,byte ptr [esi+36h]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431964: and cl,7
    {const uint32 r_=(((ecx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431967: or cl,18h
    {const uint32 r_=(((ecx&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 0043196A: mov byte ptr [esi+36h],cl
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((ecx&0xFFu)));
L_0043196D:;
    // 0043196D: mov edx,dword ptr [esi+18h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x18u)));
    // 00431970: push 0
    St<uint32>(F+432,0x0u);
    // 00431972: push edx
    St<uint32>(F+428,edx);
    // 00431973: call 00732B00
    ((void(__cdecl*)(uint32,uint32))0x732B00)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431978: mov al,byte ptr ds:[00969176h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0x969176u)))&0xFFu);
    // 0043197D: add esp,8
    // 00431980: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431982: je 0043199D
    if(zf) goto L_0043199D;
    // 00431984: mov eax,dword ptr [esi+00000590h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x590u)));
    // 0043198A: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 0043198C: jne 0043199D
    if(!zf) goto L_0043199D;
    // 0043198E: movzx eax,word ptr ds:[008CD76Ch]
    eax=(uint32)(Ld<uint16>((uintptr_t)(uint32)(0x8CD76Cu)));
    // 00431995: push eax
    St<uint32>(F+432,eax);
    // 00431996: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431998: call 006E3290
    ((void(__fastcall*)(uint32,uint32,uint32))0x6E3290)(ecx,edx,Ld<uint32>(F+432));
L_0043199D:;
    // 0043199D: mov ecx,esi
    ecx=(uint32)(esi);
    // 0043199F: call 00534540
    eax=((uint8(__fastcall*)(uint32,uint32))0x534540)(ecx,edx);
    // 004319A4: mov edi,dword ptr [esi+14h]
    edi=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x14u)));
    // 004319A7: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 004319A9: lea eax,[edi+30h]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(edi+0x30u));
    // 004319AC: je 00431AB8
    if(zf) goto L_00431AB8;
    // 004319B2: test edi,edi
    {const uint32 r_=((edi)&(edi))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 004319B4: jne 004319B9
    if(!zf) goto L_004319B9;
    // 004319B6: lea eax,[esi+4]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x4u));
L_004319B9:;
    // 004319B9: push eax
    St<uint32>(F+432,eax);
    // 004319BA: lea ecx,[esp+000000C8h]
    ecx=(uint32)((uint32)(F+632));
    // 004319C1: push ecx
    St<uint32>(F+428,ecx);
    // 004319C2: lea edx,[esp+6Ch]
    edx=(uint32)((uint32)(F+536));
    // 004319C6: push edx
    St<uint32>(F+424,edx);
    // 004319C7: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 004319CC: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 004319CF: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 004319D1: movsx eax,word ptr [esi+000004A6h]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x4A6u))));
    // 004319D8: fld st(0)
    x2=x1;
    // 004319DA: mov dword ptr [esp+3Ch],eax
    St<uint32>((F+484),(uint32)(eax));
    // 004319DE: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 004319E0: add esp,8
    // 004319E3: fld st(2)
    x3=x0;
    // 004319E5: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 004319E7: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 004319E9: fsqrt
    x2=Veh83DSqrt(x2);
    // 004319EB: fstp st(2)
    x0=x2;
    // 004319ED: fstp st(0)
    // 004319EF: fstp dword ptr [esp+14h]
    St<float>((F+452),Veh83DStore(x0));
    // 004319F3: fild dword ptr [esp+34h]
    x0=(double)Ld<int32>((F+484));
    // 004319F7: fstp dword ptr [esp]
    St<float>((F+432),Veh83DStore(x0));
    // 004319FA: push 432A0000h
    St<uint32>(F+428,0x432A0000u);
    // 004319FF: call 00420800
    x0=(double)((float(__cdecl*)(uint32,uint32))0x420800)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431A04: fmul dword ptr ds:[00B6F11Ch]
    x0=Veh83DMul(x0,(double)Ld<float>((uintptr_t)(uint32)(0xB6F11Cu)));
    // 00431A0A: fcomp dword ptr [esp+18h]
    fcc=X87Compare(x0,(double)Ld<float>((F+452)));
    // 00431A0E: add esp,8
    // 00431A11: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431A13: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A16: jnp 00431AA5
    if(!pf) goto L_00431AA5;
    // 00431A1C: fld dword ptr ds:[00B6F11Ch]
    x0=(double)Ld<float>((uintptr_t)(uint32)(0xB6F11Cu));
    // 00431A22: fmul dword ptr ds:[00858A28h]
    x0=Veh83DMul(x0,(double)F32(0x43160000u));
    // 00431A28: fcomp dword ptr [esp+10h]
    fcc=X87Compare(x0,(double)Ld<float>((F+452)));
    // 00431A2C: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431A2E: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A31: je 00431AA5
    if(zf) goto L_00431AA5;
    // 00431A33: test edi,edi
    {const uint32 r_=((edi)&(edi))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A35: lea ecx,[edi+30h]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(edi+0x30u));
    // 00431A38: jne 00431A3D
    if(!zf) goto L_00431A3D;
    // 00431A3A: lea ecx,[esi+4]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x4u));
L_00431A3D:;
    // 00431A3D: mov eax,dword ptr ds:[00B6F03Ch]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB6F03Cu)));
    // 00431A42: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A44: je 00431A4B
    if(zf) goto L_00431A4B;
    // 00431A46: add eax,30h
    {const uint32 a_=(eax)&4294967295,b_=(0x30u)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);eax=(uint32)(r_);}
    // 00431A49: jmp 00431A50
    goto L_00431A50;
L_00431A4B:;
    // 00431A4B: mov eax,0B6F02Ch
    eax=(uint32)(0xB6F02Cu);
L_00431A50:;
    // 00431A50: push ecx
    St<uint32>(F+432,ecx);
    // 00431A51: push eax
    St<uint32>(F+428,eax);
    // 00431A52: lea ecx,[esp+6Ch]
    ecx=(uint32)((uint32)(F+536));
    // 00431A56: push ecx
    St<uint32>(F+424,ecx);
    // 00431A57: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431A5C: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00431A5F: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00431A61: add esp,0Ch
    // 00431A64: fld st(0)
    x2=x1;
    // 00431A66: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00431A68: fld st(2)
    x3=x0;
    // 00431A6A: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00431A6C: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00431A6E: fsqrt
    x2=Veh83DSqrt(x2);
    // 00431A70: fstp st(2)
    x0=x2;
    // 00431A72: fstp st(0)
    // 00431A74: fld dword ptr ds:[00B6F11Ch]
    x1=(double)Ld<float>((uintptr_t)(uint32)(0xB6F11Cu));
    // 00431A7A: fmul dword ptr ds:[00858BB0h]
    x1=Veh83DMul(x1,(double)F32(0x42F00000u));
    // 00431A80: fcompp
    fcc=X87Compare(x1,x0);
    // 00431A82: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431A84: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A87: je 00431F28
    if(zf) goto L_00431F28;
    // 00431A8D: mov al,byte ptr [esp+37h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+491)))&0xFFu);
    // 00431A91: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431A93: jne 00431F28
    if(!zf) goto L_00431F28;
    // 00431A99: cmp word ptr [esi+22h],1E4h
    {const uint32 a_=(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&65535,b_=(0x1E4u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431A9F: jne 00431B40
    if(!zf) goto L_00431B40;
L_00431AA5:;
    // 00431AA5: mov edx,dword ptr [esi]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi)));
    // 00431AA7: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431AA9: push 1
    St<uint32>(F+432,0x1u);
    // 00431AAB: call dword ptr [edx]
    {typedef void(__fastcall*Fn_)(uint32,uint32,uint32);((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(edx+0x0u)))(ecx,edx,Ld<uint32>(F+432));}
L_00431AAD:;
    // 00431AAD: pop edi
    edi=(uint32)(Ld<uint32>(F+436));
    // 00431AAE: pop esi
    esi=(uint32)(Ld<uint32>(F+440));
    // 00431AAF: pop ebp
    ebp=(uint32)(Ld<uint32>(F+444));
    // 00431AB0: pop ebx
    ebx=(uint32)(Ld<uint32>(F+448));
    // 00431AB1: add esp,12Ch
    // 00431AB7: ret
    return;
L_00431AB8:;
    // 00431AB8: test edi,edi
    {const uint32 r_=((edi)&(edi))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431ABA: jne 00431ABF
    if(!zf) goto L_00431ABF;
    // 00431ABC: lea eax,[esi+4]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x4u));
L_00431ABF:;
    // 00431ABF: push eax
    St<uint32>(F+432,eax);
    // 00431AC0: lea eax,[esp+000000C8h]
    eax=(uint32)((uint32)(F+632));
    // 00431AC7: push eax
    St<uint32>(F+428,eax);
    // 00431AC8: lea ecx,[esp+6Ch]
    ecx=(uint32)((uint32)(F+536));
    // 00431ACC: push ecx
    St<uint32>(F+424,ecx);
    // 00431ACD: call 0040FE60
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FE60)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431AD2: fld dword ptr [eax+4]
    x0=(double)Ld<float>((uintptr_t)(uint32)(eax+0x4u));
    // 00431AD5: fld dword ptr [eax]
    x1=(double)Ld<float>((uintptr_t)(uint32)(eax));
    // 00431AD7: movsx edx,word ptr [esi+000004A6h]
    edx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x4A6u))));
    // 00431ADE: fld st(0)
    x2=x1;
    // 00431AE0: add esp,0Ch
    // 00431AE3: fmul st,st(1)
    x2=Veh83DMul(x2,x1);
    // 00431AE5: mov dword ptr [esp+30h],edx
    St<uint32>((F+484),(uint32)(edx));
    // 00431AE9: fld st(2)
    x3=x0;
    // 00431AEB: push 432A0000h
    St<uint32>(F+432,0x432A0000u);
    // 00431AF0: fmul st,st(3)
    x3=Veh83DMul(x3,x0);
    // 00431AF2: push ecx
    St<uint32>(F+428,ecx);
    // 00431AF3: faddp st(1),st
    x2=Veh83DAdd(x2,x3);
    // 00431AF5: fsqrt
    x2=Veh83DSqrt(x2);
    // 00431AF7: fstp st(2)
    x0=x2;
    // 00431AF9: fstp st(0)
    // 00431AFB: fstp dword ptr [esp+20h]
    St<float>((F+460),Veh83DStore(x0));
    // 00431AFF: fild dword ptr [esp+38h]
    x0=(double)Ld<int32>((F+484));
    // 00431B03: fstp dword ptr [esp]
    St<float>((F+428),Veh83DStore(x0));
    // 00431B06: call 00420800
    x0=(double)((float(__cdecl*)(uint32,uint32))0x420800)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431B0B: fmul dword ptr ds:[00858F94h]
    x0=Veh83DMul(x0,(double)F32(0x3BC0C0C1u));
    // 00431B11: add esp,8
    // 00431B14: fmul dword ptr ds:[00858CB4h]
    x0=Veh83DMul(x0,(double)F32(0x42340000u));
    // 00431B1A: fcomp dword ptr [esp+18h]
    fcc=X87Compare(x0,(double)Ld<float>((F+460)));
    // 00431B1E: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431B20: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431B23: jp 00431B40
    if(pf) goto L_00431B40;
    // 00431B25: mov al,byte ptr [esp+37h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+491)))&0xFFu);
    // 00431B29: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431B2B: jne 00431B40
    if(!zf) goto L_00431B40;
L_00431B2D:;
    // 00431B2D: mov eax,dword ptr [esi]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi)));
    // 00431B2F: push 1
    St<uint32>(F+432,0x1u);
    // 00431B31: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431B33: call dword ptr [eax]
    {typedef void(__fastcall*Fn_)(uint32,uint32,uint32);((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(eax+0x0u)))(ecx,edx,Ld<uint32>(F+432));}
    // 00431B35: pop edi
    edi=(uint32)(Ld<uint32>(F+436));
    // 00431B36: pop esi
    esi=(uint32)(Ld<uint32>(F+440));
    // 00431B37: pop ebp
    ebp=(uint32)(Ld<uint32>(F+444));
    // 00431B38: pop ebx
    ebx=(uint32)(Ld<uint32>(F+448));
    // 00431B39: add esp,12Ch
    // 00431B3F: ret
    return;
L_00431B40:;
    // 00431B40: test edi,edi
    {const uint32 r_=((edi)&(edi))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431B42: movsx ecx,word ptr [esi+22h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00431B46: mov edx,dword ptr [ecx*4+00A9B0C8h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(ecx*4+0xA9B0C8u)));
    // 00431B4D: mov eax,dword ptr [edx+14h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx+0x14u)));
    // 00431B50: mov ecx,dword ptr [eax+24h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x24u)));
    // 00431B53: mov dword ptr [esp+30h],ecx
    St<uint32>((F+484),(uint32)(ecx));
    // 00431B57: lea eax,[edi+30h]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(edi+0x30u));
    // 00431B5A: jne 00431B5F
    if(!zf) goto L_00431B5F;
    // 00431B5C: lea eax,[esi+4]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x4u));
L_00431B5F:;
    // 00431B5F: mov ecx,dword ptr [esp+30h]
    ecx=(uint32)(Ld<uint32>((F+484)));
    // 00431B63: push 0
    St<uint32>(F+432,0x0u);
    // 00431B65: push 0
    St<uint32>(F+428,0x0u);
    // 00431B67: push 1
    St<uint32>(F+424,0x1u);
    // 00431B69: push 1
    St<uint32>(F+420,0x1u);
    // 00431B6B: push 0
    St<uint32>(F+416,0x0u);
    // 00431B6D: push 0
    St<uint32>(F+412,0x0u);
    // 00431B6F: push 2
    St<uint32>(F+408,0x2u);
    // 00431B71: lea edx,[esp+48h]
    edx=(uint32)((uint32)(F+480));
    // 00431B75: push edx
    St<uint32>(F+404,edx);
    // 00431B76: push 1
    St<uint32>(F+400,0x1u);
    // 00431B78: push ecx
    St<uint32>(F+396,ecx);
    // 00431B79: push eax
    St<uint32>(F+392,eax);
    // 00431B7A: call 00568B80
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x568B80)(Ld<uint32>(F+392),Ld<uint32>(F+396),Ld<uint32>(F+400),Ld<uint32>(F+404),Ld<uint32>(F+408),Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431B7F: add esp,2Ch
    // 00431B82: cmp word ptr [esp+2Ch],0
    {const uint32 a_=(Ld<uint16>((F+480)))&65535,b_=(0x0u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431B88: jne 00431F28
    if(!zf) goto L_00431F28;
    // 00431B8E: fld dword ptr [esp+38h]
    x0=(double)Ld<float>((F+492));
    // 00431B92: fmul dword ptr [esp+000000ACh]
    x0=Veh83DMul(x0,(double)Ld<float>((F+608)));
    // 00431B99: fld dword ptr [esp+3Ch]
    x1=(double)Ld<float>((F+496));
    // 00431B9D: fmul dword ptr [esp+000000B0h]
    x1=Veh83DMul(x1,(double)Ld<float>((F+612)));
    // 00431BA4: faddp st(1),st
    x0=Veh83DAdd(x0,x1);
    // 00431BA6: fcomp dword ptr ds:[00858B50h]
    fcc=X87Compare(x0,(double)F32(0x0u));
    // 00431BAC: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431BAE: test ah,5
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x5u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431BB1: jp 00431F28
    if(pf) goto L_00431F28;
    // 00431BB7: push 1
    St<uint32>(F+432,0x1u);
    // 00431BB9: lea edx,[esi+00000437h]
    edx=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x437u));
    // 00431BBF: push edx
    St<uint32>(F+428,edx);
    // 00431BC0: lea eax,[esi+00000436h]
    eax=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x436u));
    // 00431BC6: push eax
    St<uint32>(F+424,eax);
    // 00431BC7: movsx eax,word ptr [esi+22h]
    eax=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00431BCB: lea ecx,[esi+00000435h]
    ecx=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x435u));
    // 00431BD1: push ecx
    St<uint32>(F+420,ecx);
    // 00431BD2: mov ecx,dword ptr [eax*4+00A9B0C8h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax*4+0xA9B0C8u)));
    // 00431BD9: lea edx,[esi+00000434h]
    edx=(uint32)((uint32)(uintptr_t)(uint32)(esi+0x434u));
    // 00431BDF: push edx
    St<uint32>(F+416,edx);
    // 00431BE0: call 004C8500
    ((void(__fastcall*)(uint32,uint32,uint32,uint32,uint32,uint32,uint32))0x4C8500)(ecx,edx,Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431BE5: push esi
    St<uint32>(F+432,esi);
    // 00431BE6: call 00563220
    ((void(__cdecl*)(uint32))0x563220)(Ld<uint32>(F+432));
    // 00431BEB: mov ax,word ptr [esi+22h]
    eax=(eax&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&0xFFFFu);
    // 00431BEF: add esp,4
    // 00431BF2: cmp ax,213h
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x213u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431BF6: je 00431C07
    if(zf) goto L_00431C07;
    // 00431BF8: cmp ax,214h
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x214u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431BFC: je 00431C07
    if(zf) goto L_00431C07;
    // 00431BFE: cmp dword ptr [esi+00000594h],0Ah
    {const uint32 a_=(Ld<uint32>((uintptr_t)(uint32)(esi+0x594u)))&4294967295,b_=(0xAu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431C05: jne 00431C22
    if(!zf) goto L_00431C22;
L_00431C07:;
    // 00431C07: movzx ecx,byte ptr [esi+000003D0h]
    ecx=(uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3D0u)));
    // 00431C0E: mov eax,55555556h
    eax=(uint32)(0x55555556u);
    // 00431C13: imul ecx
    {const int64 p_=(int64)(int32)eax*(int64)(int32)ecx;eax=(uint32)p_;edx=(uint32)((uint64)p_>>32);}
    // 00431C15: mov ecx,edx
    ecx=(uint32)(edx);
    // 00431C17: shr ecx,1Fh
    {const uint32 a_=(ecx)&4294967295,r_=(a_>>31);zf=r_==0;sf=(r_>>31)&1;cf=(a_>>30)&1;pf=Parity(r_);ecx=(uint32)(r_);}
    // 00431C1A: add edx,ecx
    {const uint32 a_=(edx)&4294967295,b_=(ecx)&4294967295,r_=(a_+b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=r_<a_;of=((~(a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);edx=(uint32)(r_);}
    // 00431C1C: mov byte ptr [esi+000003D0h],dl
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((edx&0xFFu)));
L_00431C22:;
    // 00431C22: call 00441C10
    eax=((uint8(__cdecl*)(void))0x441C10)();
    // 00431C27: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431C29: je 00431C46
    if(zf) goto L_00431C46;
    // 00431C2B: call 00821B1E
    eax=((uint32(__cdecl*)(void))0x821B1E)();
    // 00431C30: cdq
    edx=((int32)eax<0)?0xFFFFFFFFu:0u;
    // 00431C31: mov ecx,3E8h
    ecx=(uint32)(0x3E8u);
    // 00431C36: idiv eax,ecx
    {const int64 n_=((int64)(int32)edx<<32)|(int64)eax;const int32 d_=(int32)ecx;eax=(uint32)(int32)(n_/d_);edx=(uint32)(int32)(n_%d_);}
    // 00431C38: mov dword ptr [esp+30h],edx
    St<uint32>((F+484),(uint32)(edx));
    // 00431C3C: fild dword ptr [esp+30h]
    x0=(double)Ld<int32>((F+484));
    // 00431C40: fstp dword ptr [esi+000004C0h]
    St<float>((uintptr_t)(uint32)(esi+0x4C0u),Veh83DStore(x0));
L_00431C46:;
    // 00431C46: cmp ebx,0Dh
    {const uint32 a_=(ebx)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431C49: jne 00431C57
    if(!zf) goto L_00431C57;
    // 00431C4B: mov edx,dword ptr ds:[00B7CB84h]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(0xB7CB84u)));
    // 00431C51: mov dword ptr ds:[009690B8h],edx
    St<uint32>((uintptr_t)(uint32)(0x9690B8u),(uint32)(edx));
L_00431C57:;
    // 00431C57: cmp word ptr [esi+22h],1C9h
    {const uint32 a_=(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&65535,b_=(0x1C9u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431C5D: jne 00431C70
    if(!zf) goto L_00431C70;
    // 00431C5F: mov al,byte ptr [esi+36h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431C62: and al,7
    {const uint32 r_=(((eax&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431C64: or al,18h
    {const uint32 r_=(((eax&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);eax=(eax&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431C66: mov byte ptr [esi+36h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((eax&0xFFu)));
    // 00431C69: mov byte ptr [esi+000003B9h],2
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x2u));
L_00431C70:;
    // 00431C70: mov eax,dword ptr [esi+00000590h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x590u)));
    // 00431C76: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431C78: jne 00431CB9
    if(!zf) goto L_00431CB9;
    // 00431C7A: cmp ebx,17h
    {const uint32 a_=(ebx)&4294967295,b_=(0x17u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431C7D: ja 00431CB9
    if(!cf&&!zf) goto L_00431CB9;
    // 00431C7F: movzx ecx,byte ptr [ebx+00431F68h]
    ecx=(uint32)(Ld<uint8>((uintptr_t)(uint32)(ebx+0x431F68u)));
    // 00431C86: jmp dword ptr [ecx*4+00431F5Ch]
    switch(ecx){
    case 0: goto L_00431C8D;
    case 1: goto L_00431CA0;
    case 2: goto L_00431CB9;
    default: __assume(0);}
L_00431C8D:;
    // 00431C8D: push 14h
    St<uint32>(F+432,0x14u);
    // 00431C8F: push 0
    St<uint32>(F+428,0x0u);
    // 00431C91: call 00407180
    eax=((uint32(__cdecl*)(uint32,uint32))0x407180)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431C96: add esp,8
    // 00431C99: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431C9B: jne 00431CB9
    if(!zf) goto L_00431CB9;
    // 00431C9D: push eax
    St<uint32>(F+432,eax);
    // 00431C9E: jmp 00431CB2
    goto L_00431CB2;
L_00431CA0:;
    // 00431CA0: push 8
    St<uint32>(F+432,0x8u);
    // 00431CA2: push 0
    St<uint32>(F+428,0x0u);
    // 00431CA4: call 00407180
    eax=((uint32(__cdecl*)(uint32,uint32))0x407180)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431CA9: add esp,8
    // 00431CAC: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431CAE: jne 00431CB9
    if(!zf) goto L_00431CB9;
    // 00431CB0: push 1
    St<uint32>(F+432,0x1u);
L_00431CB2:;
    // 00431CB2: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431CB4: call 006A2530
    ((void(__fastcall*)(uint32,uint32,uint32))0x6A2530)(ecx,edx,Ld<uint32>(F+432));
L_00431CB9:;
    // 00431CB9: cmp dword ptr [esi+00000594h],9
    {const uint32 a_=(Ld<uint32>((uintptr_t)(uint32)(esi+0x594u)))&4294967295,b_=(0x9u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431CC0: jne 00431CDF
    if(!zf) goto L_00431CDF;
    // 00431CC2: mov al,byte ptr [esi+000003B9h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3B9u)))&0xFFu);
    // 00431CC8: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431CCA: jne 00431CDF
    if(!zf) goto L_00431CDF;
    // 00431CCC: mov dl,byte ptr [esi+36h]
    edx=(edx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431CCF: and dl,7
    {const uint32 r_=(((edx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431CD2: or dl,18h
    {const uint32 r_=(((edx&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);edx=(edx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431CD5: mov byte ptr [esi+36h],dl
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((edx&0xFFu)));
    // 00431CD8: mov byte ptr [esi+000003B9h],6
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x6u));
L_00431CDF:;
    // 00431CDF: mov al,byte ptr [esp+1Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+467)))&0xFFu);
    // 00431CE3: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431CE5: jne 00431D7D
    if(!zf) goto L_00431D7D;
    // 00431CEB: cmp ebx,0Dh
    {const uint32 a_=(ebx)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431CEE: je 00431D7D
    if(zf) goto L_00431D7D;
    // 00431CF4: push 0FFFFFFFFh
    St<uint32>(F+432,0xFFFFFFFFu);
    // 00431CF6: call 0056E210
    eax=((uint32(__cdecl*)(uint32))0x56E210)(Ld<uint32>(F+432));
    // 00431CFB: add esp,4
    // 00431CFE: mov ecx,eax
    ecx=(uint32)(eax);
    // 00431D00: call 0041BE60
    eax=((uint32(__fastcall*)(uint32,uint32))0x41BE60)(ecx,edx);
    // 00431D05: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D07: jne 00431D7D
    if(!zf) goto L_00431D7D;
    // 00431D09: mov al,byte ptr ds:[0096914Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0x96914Fu)))&0xFFu);
    // 00431D0E: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D10: jne 00431D25
    if(!zf) goto L_00431D25;
    // 00431D12: fld dword ptr ds:[009690BCh]
    x0=(double)Ld<float>((uintptr_t)(uint32)(0x9690BCu));
    // 00431D18: fcomp dword ptr ds:[00858B50h]
    fcc=X87Compare(x0,(double)F32(0x0u));
    // 00431D1E: fnstsw ax
    eax=(eax&0xFFFF0000u)|((uint32)fcc<<8);
    // 00431D20: test ah,41h
    {const uint32 r_=((((eax>>8)&0xFFu))&(0x41u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D23: jp 00431D7D
    if(pf) goto L_00431D7D;
L_00431D25:;
    // 00431D25: mov al,byte ptr [esp+36h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+490)))&0xFFu);
    // 00431D29: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D2B: jne 00431D7D
    if(!zf) goto L_00431D7D;
    // 00431D2D: mov eax,dword ptr [esp+4Ch]
    eax=(uint32)(Ld<uint32>((F+512)));
    // 00431D31: push eax
    St<uint32>(F+432,eax);
    // 00431D32: push ebx
    St<uint32>(F+428,ebx);
    // 00431D33: push esi
    St<uint32>(F+424,esi);
    // 00431D34: call 0042C2B0
    eax=((uint8(__cdecl*)(uint32,uint32,uint32))0x42C2B0)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431D39: add esp,0Ch
    // 00431D3C: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D3E: je 00431D7D
    if(zf) goto L_00431D7D;
    // 00431D40: call 00441C10
    eax=((uint8(__cdecl*)(void))0x441C10)();
    // 00431D45: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D47: je 00431D63
    if(zf) goto L_00431D63;
    // 00431D49: push 43F00000h
    St<uint32>(F+432,0x43F00000u);
    // 00431D4E: push 43700000h
    St<uint32>(F+428,0x43700000u);
    // 00431D53: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431D58: fstp dword ptr ds:[009690BCh]
    St<float>((uintptr_t)(uint32)(0x9690BCu),Veh83DStore(x0));
    // 00431D5E: jmp 00431EEA
    goto L_00431EEA;
L_00431D63:;
    // 00431D63: push 44960000h
    St<uint32>(F+432,0x44960000u);
    // 00431D68: push 44160000h
    St<uint32>(F+428,0x44160000u);
    // 00431D6D: call 0041BD90
    x0=(double)((float(__cdecl*)(uint32,uint32))0x41BD90)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431D72: fstp dword ptr ds:[009690BCh]
    St<float>((uintptr_t)(uint32)(0x9690BCu),Veh83DStore(x0));
    // 00431D78: jmp 00431EEA
    goto L_00431EEA;
L_00431D7D:;
    // 00431D7D: mov al,byte ptr [esp+47h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((F+507)))&0xFFu);
    // 00431D81: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431D83: je 00431EB6
    if(zf) goto L_00431EB6;
    // 00431D89: mov ax,word ptr [esi+22h]
    eax=(eax&0xFFFF0000u)|((uint32)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u)))&0xFFFFu);
    // 00431D8D: cmp ax,1CFh
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x1CFu)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431D91: je 00431DB1
    if(zf) goto L_00431DB1;
    // 00431D93: cmp ax,1CDh
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x1CDu)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431D97: je 00431DB1
    if(zf) goto L_00431DB1;
    // 00431D99: cmp ax,209h
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x209u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431D9D: je 00431DB1
    if(zf) goto L_00431DB1;
    // 00431D9F: cmp ax,20Ah
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x20Au)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431DA3: je 00431DB1
    if(zf) goto L_00431DB1;
    // 00431DA5: cmp ax,245h
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x245u)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431DA9: je 00431DB1
    if(zf) goto L_00431DB1;
    // 00431DAB: cmp ax,24Ah
    {const uint32 a_=((eax&0xFFFFu))&65535,b_=(0x24Au)&65535,r_=(a_-b_)&65535;zf=r_==0;sf=(r_>>15)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>15)&1;pf=Parity(r_);}
    // 00431DAF: jne 00431DEF
    if(!zf) goto L_00431DEF;
L_00431DB1:;
    // 00431DB1: mov al,byte ptr ds:[00B72958h]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0xB72958u)))&0xFFu);
    // 00431DB6: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431DB8: jne 00431DEF
    if(!zf) goto L_00431DEF;
    // 00431DBA: push 7
    St<uint32>(F+432,0x7u);
    // 00431DBC: push 0
    St<uint32>(F+428,0x0u);
    // 00431DBE: call 00407180
    eax=((uint32(__cdecl*)(uint32,uint32))0x407180)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431DC3: add esp,8
    // 00431DC6: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431DC8: jne 00431DEF
    if(!zf) goto L_00431DEF;
    // 00431DCA: push ebx
    St<uint32>(F+432,ebx);
    // 00431DCB: push esi
    St<uint32>(F+428,esi);
    // 00431DCC: call 0042C740
    eax=((uint8(__cdecl*)(uint32,uint32))0x42C740)(Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431DD1: add esp,8
    // 00431DD4: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431DD6: je 00431DEF
    if(zf) goto L_00431DEF;
    // 00431DD8: push 63h
    St<uint32>(F+432,0x63u);
    // 00431DDA: push 0
    St<uint32>(F+428,0x0u);
    // 00431DDC: push 1
    St<uint32>(F+424,0x1u);
    // 00431DDE: push 1
    St<uint32>(F+420,0x1u);
    // 00431DE0: push ebx
    St<uint32>(F+416,ebx);
    // 00431DE1: push esi
    St<uint32>(F+412,esi);
    // 00431DE2: call 004217C0
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32))0x4217C0)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431DE7: add esp,18h
    // 00431DEA: jmp 00431EED
    goto L_00431EED;
L_00431DEF:;
    // 00431DEF: push 63h
    St<uint32>(F+432,0x63u);
    // 00431DF1: push 0
    St<uint32>(F+428,0x0u);
    // 00431DF3: push 1
    St<uint32>(F+424,0x1u);
    // 00431DF5: push 1
    St<uint32>(F+420,0x1u);
    // 00431DF7: push ebx
    St<uint32>(F+416,ebx);
    // 00431DF8: push esi
    St<uint32>(F+412,esi);
    // 00431DF9: call 004217C0
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32))0x4217C0)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431DFE: movzx edx,byte ptr [esi+000003D0h]
    edx=(uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x3D0u)));
    // 00431E05: mov cl,byte ptr [esi+36h]
    ecx=(ecx&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(esi+0x36u)))&0xFFu);
    // 00431E08: mov dword ptr [esp+48h],edx
    St<uint32>((F+484),(uint32)(edx));
    // 00431E0C: and cl,7
    {const uint32 r_=(((ecx&0xFFu))&(0x7u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431E0F: fild dword ptr [esp+48h]
    x0=(double)Ld<int32>((F+484));
    // 00431E13: or cl,18h
    {const uint32 r_=(((ecx&0xFFu))|(0x18u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);ecx=(ecx&0xFFFFFF00u)|((uint32)(r_)&0xFFu);}
    // 00431E16: add esp,18h
    // 00431E19: mov byte ptr [esi+36h],cl
    St<uint8>((uintptr_t)(uint32)(esi+0x36u),(uint8)((ecx&0xFFu)));
    // 00431E1C: fadd dword ptr ds:[0085862Ch]
    x0=Veh83DAdd(x0,(double)F32(0x41200000u));
    // 00431E22: mov byte ptr [esi+000003B9h],2
    St<uint8>((uintptr_t)(uint32)(esi+0x3B9u),(uint8)(0x2u));
    // 00431E29: call 00821B40
    eax=(uint32)Heli83GFtol(x0);
    // 00431E2E: mov byte ptr [esi+000003D0h],al
    St<uint8>((uintptr_t)(uint32)(esi+0x3D0u),(uint8)((eax&0xFFu)));
    // 00431E34: movzx eax,al
    eax=(uint32)((eax&0xFFu));
    // 00431E37: push 3CA3D70Ah
    St<uint32>(F+432,0x3CA3D70Au);
    // 00431E3C: mov dword ptr [esp+34h],eax
    St<uint32>((F+484),(uint32)(eax));
    // 00431E40: push ecx
    St<uint32>(F+428,ecx);
    // 00431E41: fild dword ptr [esp+38h]
    x0=(double)Ld<int32>((F+484));
    // 00431E45: lea ecx,[esp+6Ch]
    ecx=(uint32)((uint32)(F+536));
    // 00431E49: fstp dword ptr [esp]
    St<float>((F+428),Veh83DStore(x0));
    // 00431E4C: push ecx
    St<uint32>(F+424,ecx);
    // 00431E4D: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431E4F: call 0041CCB0
    eax=((uint32(__fastcall*)(uint32,uint32,uint32))0x41CCB0)(ecx,edx,Ld<uint32>(F+424));
    // 00431E54: push eax
    St<uint32>(F+424,eax);
    // 00431E55: lea edx,[esp+000000A8h]
    edx=(uint32)((uint32)(F+592));
    // 00431E5C: push edx
    St<uint32>(F+420,edx);
    // 00431E5D: call 0040FEC0
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FEC0)(Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428));
    // 00431E62: add esp,0Ch
    // 00431E65: push eax
    St<uint32>(F+428,eax);
    // 00431E66: lea eax,[esp+000000D8h]
    eax=(uint32)((uint32)(F+644));
    // 00431E6D: push eax
    St<uint32>(F+424,eax);
    // 00431E6E: call 0040FEC0
    eax=((uint32(__cdecl*)(uint32,uint32,uint32))0x40FEC0)(Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431E73: mov ecx,dword ptr [eax]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax)));
    // 00431E75: mov dword ptr [ebp],ecx
    St<uint32>((uintptr_t)(uint32)(ebp),(uint32)(ecx));
    // 00431E78: mov edx,dword ptr [eax+4]
    edx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x4u)));
    // 00431E7B: mov dword ptr [ebp+4],edx
    St<uint32>((uintptr_t)(uint32)(ebp+0x4u),(uint32)(edx));
    // 00431E7E: mov eax,dword ptr [eax+8]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(eax+0x8u)));
    // 00431E81: add esp,0Ch
    // 00431E84: mov dword ptr [ebp+8],eax
    St<uint32>((uintptr_t)(uint32)(ebp+0x8u),(uint32)(eax));
    // 00431E87: call 00441C10
    eax=((uint8(__cdecl*)(void))0x441C10)();
    // 00431E8C: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431E8E: jne 00431E99
    if(!zf) goto L_00431E99;
    // 00431E90: mov al,byte ptr ds:[0096914Fh]
    eax=(eax&0xFFFFFF00u)|((uint32)(Ld<uint8>((uintptr_t)(uint32)(0x96914Fu)))&0xFFu);
    // 00431E95: test al,al
    {const uint32 r_=(((eax&0xFFu))&((eax&0xFFu)))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431E97: je 00431EAD
    if(zf) goto L_00431EAD;
L_00431E99:;
    // 00431E99: mov eax,dword ptr [esi+00000460h]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi+0x460u)));
    // 00431E9F: test eax,eax
    {const uint32 r_=((eax)&(eax))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);}
    // 00431EA1: je 00431EAD
    if(zf) goto L_00431EAD;
    // 00431EA3: or dword ptr [eax+00000478h],800h
    {const uint32 r_=((Ld<uint32>((uintptr_t)(uint32)(eax+0x478u)))|(0x800u))&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=0;of=0;pf=Parity(r_);St<uint32>((uintptr_t)(uint32)(eax+0x478u),(uint32)(r_));}
L_00431EAD:;
    // 00431EAD: or byte ptr [esi+0000042Eh],8
    {const uint32 r_=((Ld<uint8>((uintptr_t)(uint32)(esi+0x42Eu)))|(0x8u))&255;zf=r_==0;sf=(r_>>7)&1;cf=0;of=0;pf=Parity(r_);St<uint8>((uintptr_t)(uint32)(esi+0x42Eu),(uint8)(r_));}
    // 00431EB4: jmp 00431EED
    goto L_00431EED;
L_00431EB6:;
    // 00431EB6: cmp ebx,0Dh
    {const uint32 a_=(ebx)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431EB9: je 00431EE2
    if(zf) goto L_00431EE2;
    // 00431EBB: cmp ebx,18h
    {const uint32 a_=(ebx)&4294967295,b_=(0x18u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431EBE: je 00431EE2
    if(zf) goto L_00431EE2;
    // 00431EC0: push 63h
    St<uint32>(F+432,0x63u);
    // 00431EC2: push 0
    St<uint32>(F+428,0x0u);
    // 00431EC4: push 0
    St<uint32>(F+424,0x0u);
    // 00431EC6: push 0
    St<uint32>(F+420,0x0u);
    // 00431EC8: push ebx
    St<uint32>(F+416,ebx);
    // 00431EC9: push esi
    St<uint32>(F+412,esi);
    // 00431ECA: mov byte ptr ds:[009690CCh],1
    St<uint8>((uintptr_t)(uint32)(0x9690CCu),(uint8)(0x1u));
    // 00431ED1: call 004217C0
    ((void(__cdecl*)(uint32,uint32,uint32,uint32,uint32,uint32))0x4217C0)(Ld<uint32>(F+412),Ld<uint32>(F+416),Ld<uint32>(F+420),Ld<uint32>(F+424),Ld<uint32>(F+428),Ld<uint32>(F+432));
    // 00431ED6: add esp,18h
    // 00431ED9: mov byte ptr ds:[009690CCh],0
    St<uint8>((uintptr_t)(uint32)(0x9690CCu),(uint8)(0x0u));
    // 00431EE0: jmp 00431EED
    goto L_00431EED;
L_00431EE2:;
    // 00431EE2: push 0
    St<uint32>(F+432,0x0u);
    // 00431EE4: push esi
    St<uint32>(F+428,esi);
    // 00431EE5: call 0041C070
    ((void(__cdecl*)(uint32,uint32))0x41C070)(Ld<uint32>(F+428),Ld<uint32>(F+432));
L_00431EEA:;
    // 00431EEA: add esp,8
L_00431EED:;
    // 00431EED: cmp ebx,0Dh
    {const uint32 a_=(ebx)&4294967295,b_=(0xDu)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431EF0: je 00431EF7
    if(zf) goto L_00431EF7;
    // 00431EF2: cmp ebx,18h
    {const uint32 a_=(ebx)&4294967295,b_=(0x18u)&4294967295,r_=(a_-b_)&4294967295;zf=r_==0;sf=(r_>>31)&1;cf=a_<b_;of=(((a_^b_)&(a_^r_))>>31)&1;pf=Parity(r_);}
    // 00431EF5: jne 00431F00
    if(!zf) goto L_00431F00;
L_00431EF7:;
    // 00431EF7: push 1
    St<uint32>(F+432,0x1u);
    // 00431EF9: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431EFB: call 006D2330
    ((void(__fastcall*)(uint32,uint32,uint32))0x6D2330)(ecx,edx,Ld<uint32>(F+432));
L_00431F00:;
    // 00431F00: movsx ecx,word ptr [esi+22h]
    ecx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00431F04: push ecx
    St<uint32>(F+432,ecx);
    // 00431F05: call 0040BA70
    ((void(__cdecl*)(uint32))0x40BA70)(Ld<uint32>(F+432));
    // 00431F0A: movsx edx,word ptr [esi+22h]
    edx=(uint32)((uint32)(int32)(int16)(Ld<uint16>((uintptr_t)(uint32)(esi+0x22u))));
    // 00431F0E: mov ecx,dword ptr [edx*4+00A9B0C8h]
    ecx=(uint32)(Ld<uint32>((uintptr_t)(uint32)(edx*4+0xA9B0C8u)));
    // 00431F15: add esp,4
    // 00431F18: call 00421120
    ((void(__fastcall*)(uint32,uint32))0x421120)(ecx,edx);
    // 00431F1D: pop edi
    edi=(uint32)(Ld<uint32>(F+436));
    // 00431F1E: pop esi
    esi=(uint32)(Ld<uint32>(F+440));
    // 00431F1F: pop ebp
    ebp=(uint32)(Ld<uint32>(F+444));
    // 00431F20: pop ebx
    ebx=(uint32)(Ld<uint32>(F+448));
    // 00431F21: add esp,12Ch
    // 00431F27: ret
    return;
L_00431F28:;
    // 00431F28: mov eax,dword ptr [esi]
    eax=(uint32)(Ld<uint32>((uintptr_t)(uint32)(esi)));
    // 00431F2A: mov ecx,esi
    ecx=(uint32)(esi);
    // 00431F2C: push 1
    St<uint32>(F+432,0x1u);
    // 00431F2E: call dword ptr [eax]
    {typedef void(__fastcall*Fn_)(uint32,uint32,uint32);((Fn_)(uintptr_t)Ld<uint32>((uintptr_t)(uint32)(eax+0x0u)))(ecx,edx,Ld<uint32>(F+432));}
    // 00431F30: pop edi
    edi=(uint32)(Ld<uint32>(F+436));
    // 00431F31: pop esi
    esi=(uint32)(Ld<uint32>(F+440));
    // 00431F32: pop ebp
    ebp=(uint32)(Ld<uint32>(F+444));
    // 00431F33: pop ebx
    ebx=(uint32)(Ld<uint32>(F+448));
    // 00431F34: add esp,12Ch
    // 00431F3A: ret
    return;
