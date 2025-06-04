#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

X64CpuInsnList::X64CpuInsnList()
{
	////
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
	////
}


X64PseudoInsnList::X64PseudoInsnList()
{
	////
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
	////
}



