#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

X64CpuInsnList::X64CpuInsnList()
{
	htCpuInsns["mov"] = std::make_unique<InsnMov>();
}

unique_ptr<X64CpuInsnList> X64CpuInsnList::instance = nullptr;

X64CpuInsnList & X64CpuInsnList::getInstance()
{
	if (instance == nullptr) {
		instance = make_unique<X64CpuInsnList>();
	}
	return (* instance);
}

IInsnProcessor * X64CpuInsnList::getInsnProcessor(const std::string & insn)
{
	std::string str2 = insn;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	auto obj = htCpuInsns.find(str2);
	if (obj == htCpuInsns.end()) {
		return nullptr;
	}
	return obj->second.get();
}

bool X64CpuInsnList::isCpuInstruction(const std::string & str)
{
	std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	return htCpuInsns.find(str2) != htCpuInsns.end();
}


X64PseudoInsnList::X64PseudoInsnList()
{
	htPseudoInsns["proc"] = make_unique<Proc>();
	htPseudoInsns["segment"] = make_unique<Segment>();
}

unique_ptr<X64PseudoInsnList> X64PseudoInsnList::instance = nullptr;

X64PseudoInsnList & X64PseudoInsnList::getInstance()
{
	if (instance == nullptr) {
		instance = make_unique<X64PseudoInsnList>();
	}
	return (* instance);
}

IInsnProcessor * X64PseudoInsnList::getPseudoInsnProcessor(const std::string & insn)
{
	std::string str2 = insn;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	auto obj = htPseudoInsns.find(str2);
	if (obj == htPseudoInsns.end()) {
		return nullptr;
	}
	return obj->second.get();
}

bool X64PseudoInsnList::isPseudoInstruction(const std::string & str)
{
    std::string str2 = str;
    std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c){ return ::tolower(c); });
	return htPseudoInsns.find(str2) != htPseudoInsns.end();
}


X64VirtualInsnList::X64VirtualInsnList()
{
}

unique_ptr<X64VirtualInsnList> X64VirtualInsnList::instance = nullptr;

X64VirtualInsnList & X64VirtualInsnList::getInstance()
{
	if (instance == nullptr) {
		instance = make_unique<X64VirtualInsnList>();
	}
	return (* instance);
}

bool X64VirtualInsnList::isVirtualInstruction(const std::string & str)
{
	return false;
}



