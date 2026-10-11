#pragma once
#include <set>
#include "Core/MatrixLinkList.h"
#include "../ProbeRuntime.hpp"
namespace DependenciesWorld15Matrix {
// Complete pool inventory derived from all three real intrusive lists. No invented capacity.
struct Snapshot {
 std::vector<uint8> header;
 std::vector<std::pair<CMatrixLink*,std::vector<uint8>>> nodes;
 bool legal{};
 Snapshot(){
  header=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList));
  std::set<CMatrixLink*> all;bool valid=true;
  auto list=[&](CMatrixLink* head,CMatrixLink* tail){
   auto* previous=head;auto* link=head->m_pNext;size_t steps=0;
   while(link&&link!=tail){
    if(++steps>8192||link->m_pPrev!=previous||!all.insert(link).second){valid=false;return;}
    previous=link;link=link->m_pNext;
   }
   if(link!=tail||tail->m_pPrev!=previous)valid=false;
  };
  list(&gMatrixList.m_head,&gMatrixList.m_tail);
  list(&gMatrixList.m_allocatedListHead,&gMatrixList.m_allocatedListTail);
  list(&gMatrixList.m_freeListHead,&gMatrixList.m_freeListTail);
  if(!gMatrixList.m_pObjects||all.empty())valid=false;
  size_t index=0;for(auto* m:all){
   if(uintptr_t(m)!=uintptr_t(gMatrixList.m_pObjects)+index*sizeof(CMatrixLink))valid=false;
   nodes.push_back({m,ReversingProbe::Bytes(m,sizeof(CMatrixLink))});++index;
  }
  legal=valid;
 }
 void Restore()const{std::memcpy(&gMatrixList,header.data(),header.size());for(const auto&[at,bytes]:nodes)std::memcpy(at,bytes.data(),bytes.size());}
 bool Equal()const{if(header!=ReversingProbe::Bytes(&gMatrixList,sizeof(gMatrixList)))return false;for(const auto&[at,bytes]:nodes)if(bytes!=ReversingProbe::Bytes(at,bytes.size()))return false;return true;}
 size_t Count()const{return nodes.size();}
};
}
