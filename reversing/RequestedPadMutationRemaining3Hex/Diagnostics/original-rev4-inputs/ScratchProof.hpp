#pragma once
#include <bcrypt.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#pragma comment(lib,"bcrypt.lib")
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPadRemaining3Phased {
using Simple=ReversibleHooks::ReversibleHook::Simple;
inline Simple* Find(uintptr_t address){Simple* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& item:cat.Items())if(item->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* hook=static_cast<Simple*>(item.get());if(uintptr_t(hook->GetHookGTAAddress())==address)found=hook;}});return found;}
inline std::string HashFile(const std::filesystem::path& path){
 BCRYPT_ALG_HANDLE alg{};BCRYPT_HASH_HANDLE hash{};DWORD objectSize{},received{};std::string result;
 if(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0)return {};
 if(BCryptGetProperty(alg,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&objectSize),sizeof(objectSize),&received,0)>=0){std::vector<uint8_t> object(objectSize);
  if(BCryptCreateHash(alg,&hash,object.data(),objectSize,nullptr,0,0)>=0){std::ifstream file(path,std::ios::binary);std::array<char,65536> block{};bool good=bool(file);
   while(file){file.read(block.data(),block.size());const auto size=file.gcount();if(size&&BCryptHashData(hash,reinterpret_cast<PUCHAR>(block.data()),ULONG(size),0)<0){good=false;break;}}
   std::array<uint8_t,32> digest{};if(good&&file.eof()&&BCryptFinishHash(hash,digest.data(),digest.size(),0)>=0){std::ostringstream text;for(auto byte:digest)text<<std::hex<<std::setw(2)<<std::setfill('0')<<unsigned(byte);result=text.str();}
   BCryptDestroyHash(hash);
  }
 }
 BCryptCloseAlgorithmProvider(alg,0);return result;
}
inline std::vector<uint8_t> Unhex(const std::string& hex){if(hex.size()%2)return {};std::vector<uint8_t> result;for(size_t i=0;i<hex.size();i+=2){char* end{};const auto pair=hex.substr(i,2);const auto value=std::strtoul(pair.c_str(),&end,16);if(end!=pair.c_str()+2||value>255)return {};result.push_back(uint8_t(value));}return result;}
struct CurrentProof {uint8_t* scratch{};uintptr_t moduleBase{};json evidence;};
inline bool VerifyCurrent(const std::filesystem::path& dir,CurrentProof& proof,json& error){
 auto* hook=Find(0x53F530);if(!hook){error="missing Reconcile hook";return false;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();const auto gtaBefore=ReversingProbe::Bytes(hook->GetHookGTAAddress(),52),dllBefore=ReversingProbe::Bytes(hook->GetHookOurAddress(),52);
 hook->LockState(false);hook->State(true);
 bool verified=false;
 try{
  json layout;std::ifstream input(dir/"requested-pad-remaining3-phased-layout.json");if(!input)throw std::runtime_error("Missing exact current compiled layout proof");input>>layout;
  HMODULE module{};if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(hook->GetHookOurAddress()),&module))throw std::runtime_error("Owning module not found");
  const uintptr_t base=reinterpret_cast<uintptr_t>(module);wchar_t path[MAX_PATH]{};const auto pathSize=GetModuleFileNameW(module,path,MAX_PATH);if(!pathSize||pathSize>=MAX_PATH||HashFile(std::filesystem::path(path))!=layout.at("asi_sha256").get<std::string>())throw std::runtime_error("Actual module artifact SHA mismatch");
  const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);if(dos->e_magic!=IMAGE_DOS_SIGNATURE||dos->e_lfanew<=0||dos->e_lfanew>4096)throw std::runtime_error("Invalid mapped DOS header");const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(base+dos->e_lfanew);
  const size_t imageSize=layout.at("image_size");if(nt->Signature!=IMAGE_NT_SIGNATURE||nt->OptionalHeader.Magic!=IMAGE_NT_OPTIONAL_HDR32_MAGIC||nt->OptionalHeader.SizeOfImage!=imageSize)throw std::runtime_error("Mapped image extent mismatch");
  if(layout.at("functions").size()!=1||layout.at("scratch").size()!=1)throw std::runtime_error("Ambiguous current symbols");const auto& function=layout["functions"][0];const auto& scratch=layout["scratch"][0];const size_t rva=function.at("rva"),size=function.at("length"),scratchRva=scratch.at("rva");
  if(scratch.at("name")!="reconciledState"||size<52||scratch.at("length")!=48||rva>imageSize||size>imageSize-rva||scratchRva<16||scratchRva>imageSize||64>imageSize-scratchRva)throw std::runtime_error("Symbol byte bounds invalid");
  auto expected=Unhex(function.at("bytes_hex"));if(expected.size()!=size)throw std::runtime_error("Incomplete compiled body");const uint32_t delta=uint32_t(base)-layout.at("image_base").get<uint32_t>();
  std::set<size_t> relocated;for(const auto& value:function.at("highlow_relocations")){const size_t offset=value;if(offset>expected.size()-4||!relocated.insert(offset).second)throw std::runtime_error("Invalid explicit HIGHLOW relocation");uint32_t bits{};std::memcpy(&bits,expected.data()+offset,4);bits+=delta;std::memcpy(expected.data()+offset,&bits,4);}
  if(expected!=ReversingProbe::Bytes(reinterpret_cast<void*>(base+rva),size)||!SmallPendingSupport::ExactSwitch(*hook,true))throw std::runtime_error("Current mapped candidate body/switch mismatch");
  const auto& operands=function.at("scratch_operands");if(operands.empty())throw std::runtime_error("No current encoded scratch operand evidence");for(const auto& operand:operands){const size_t encoded=operand.at("body_encoded_operand_offset"),offset=operand.at("scratch_offset");if(offset>=48||encoded>size-4||!relocated.contains(encoded))throw std::runtime_error("Scratch operand bounds/relocation mismatch");uint32_t actual{};std::memcpy(&actual,reinterpret_cast<void*>(base+rva+encoded),4);if(actual!=uint32_t(base+scratchRva+offset))throw std::runtime_error("Encoded scratch operand mismatch");}
  proof.scratch=reinterpret_cast<uint8_t*>(base+scratchRva);proof.moduleBase=base;proof.evidence={{"artifact_sha256",layout["asi_sha256"]},{"pdb_sha256",layout["pdb_sha256"]},{"function_rva",rva},{"function_bytes",size},{"scratch_rva",scratchRva},{"scratch_bytes",48},{"encoded_operands",operands.size()},{"actual_module_base",base},{"mapped_body_exact",true},{"artifact_hash_exact",true},{"explicit_highlow_relocations",relocated.size()}};verified=true;
 }catch(const std::exception& exception){error=exception.what();}
 hook->State(enabled);hook->LockState(locked);
 const bool restored=gtaBefore==ReversingProbe::Bytes(hook->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(hook->GetHookOurAddress(),52)&&hook->Hooked()==enabled&&hook->Locked()==locked;
 if(!restored){error="Current-proof entry/flag restoration failed";verified=false;}return verified;
}
}
