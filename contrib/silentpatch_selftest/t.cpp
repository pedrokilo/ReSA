#include <cstdio>
#include "SilentPatchConfig.h"
int main(){
  const char* names[]={"MeleeCheatAmmo","AIAccuracyFix","SecuricarDamage","Rand16bit","TrueInvincibility","BicycleFire"};
  printf("enabled=%d\n",(int)SilentPatchEnabled());
  for(auto n:names) printf("%s=%d\n",n,(int)SilentPatchOption(n,true));
  printf("default-false opt TrueInvincibility with default false -> %d\n",(int)SilentPatchOption("TrueInvincibility",false));
}
