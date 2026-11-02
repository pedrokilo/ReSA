#include <cstdint>
#include <cstring>
#include <bit>
using int8=int8_t;using int16=int16_t;using uint8=uint8_t;using uint16=uint16_t;using uint32=uint32_t;using int32=int32_t;using int64=int64_t;using uint64=uint64_t;
double __cdecl Veh83DMul(double a,double b);double __cdecl Veh83DAdd(double a,double b);double __cdecl Veh83DSub(double a,double b);double __cdecl Veh83DDiv(double a,double b);
double __cdecl Veh83DSqrt(double a);float __cdecl Veh83DStore(double a);int32_t __cdecl Heli83GFtol(double value);
template<class T> static inline T Ld(uintptr_t a){T v;memcpy(&v,reinterpret_cast<const void*>(a),sizeof(T));return v;}
template<class T> static inline void St(uintptr_t a,T v){memcpy(reinterpret_cast<void*>(a),&v,sizeof(T));}
template<class T> static inline T Ld(const uint8* a){T v;memcpy(&v,a,sizeof(T));return v;}
template<class T> static inline void St(uint8* a,T v){memcpy(a,&v,sizeof(T));}
static inline float F32(uint32 bits){return std::bit_cast<float>(bits);}
static inline bool Parity(uint32 v){v&=0xFF;v^=v>>4;v^=v>>2;v^=v>>1;return !(v&1);}
static inline uint8 X87Compare(double a,double b){return (a!=a||b!=b)?0x45:(a>b?0:(a<b?0x01:0x40));}
void Gen(){
    alignas(16) uint8 F[0x300]={};
    uint32 eax=0,ecx=0,edx=0,ebx=0,esi=0,edi=0,ebp=0;
    double x0=0,x1=0,x2=0,x3=0,x4=0,x5=0,x6=0,x7=0;
    bool zf=0,sf=0,cf=0,of=0,pf=0;uint8 fcc=0;
#include "lifted.inl"
}
