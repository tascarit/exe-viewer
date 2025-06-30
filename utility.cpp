#include <string>
#include <iostream>

#include "structures.cpp"

#define X64 0x8664
#define ARM 0x1c0
#define ARM64 0xaa64
#define Alpha32 0x184
#define Alpha64 0x284
#define ARMThumb 0x1c4
#define Intel386 0x14c
#define LA32 0x6232
#define LA64 0x6264

#define WINDOWS_NATIVE 1
#define UNKNOWN 0
#define WINDOWS_GUI 2
#define WINDOWS_CONSOLE 3
#define POSIX 7
#define WINDOWS_CE 9
#define EFI_APP 10
#define EFI_BOOT_DRIVER 11
#define EFI_RUNTIME_DRIVER 12
#define EFI_ROM 13
#define XBOX 14
#define WINDOWS_BOOT_APP 16

void Error(const char* err){
	throw std::runtime_error(err);
}

std::string GetArchitecture(long value){
	switch (value){
		case X64: return "x64";
		case ARM: return "ARM";
		case ARM64: return "ARM64";
		case Alpha32: return "Alpha AXP 32-bit";
		case Alpha64: return "Alpha 64-bit";
		case ARMThumb: return "ARM Thumb-2";
		case Intel386: return "Intel 386 32-bit";
		case LA32: return "LoongArch 32-bit";
		case LA64: return "LoongArch 64-bit";
		default: return "Unknown";

	}
}

std::string GetSubsystem(long value){
	switch (value){
		case WINDOWS_NATIVE: return "Windows native";
		case WINDOWS_GUI: return "Windows GUI";
		case WINDOWS_CONSOLE: return "Windows Console";
		case POSIX: return "Posix";
		case WINDOWS_CE: return "Windows CE";
		case EFI_APP: return "EFI Application";
		case EFI_BOOT_DRIVER: return "EFI Boot driver";
		case EFI_RUNTIME_DRIVER: return "EFI Runtime driver";
		case EFI_ROM: return "EFI ROM Image";
		case XBOX: return "XBOX";
		case WINDOWS_BOOT_APP: return "Windows Boot Application";
		default: return "Unknown subsystem";
	}
}

std::string PEFormat(long val){
	switch (val){
		case 0x20b: return "PE32+";
		case 0x10b: return "PE32";
		default: return "Unknown";
	}
}

DWORD RvaToOffset(DWORD rva, Section* sections, size_t numberOfSections){
	for (int i = 0; i < numberOfSections; i++){
		if (rva >= sections[i].virtualAddress && rva < sections[i].virtualSize + sections[i].virtualAddress)
			return rva - sections[i].virtualAddress + sections[i].pointerToRawData;
	}

	return 0;
}

std::string ReadString(FILE* fp, DWORD nameOffset){
	char buffer[32];
	int lastIndex = -1;

	fseek(fp, nameOffset, SEEK_SET);

	do{
		lastIndex++;

		fread((buffer+lastIndex), sizeof(char), 1, fp);
	} while (buffer[lastIndex] != '\0');

	return buffer;
}

std::string ReadFuncName(FILE* fp, DWORD nameOffset){
	WORD hint;
	char buffer[32];
	int lastIndex = -1;

	fseek(fp, nameOffset, SEEK_SET);
	
	fread(&hint, sizeof(WORD), 1, fp);

	do {
		lastIndex++;
		
		fread((buffer+lastIndex), sizeof(char), 1, fp);
	} while (buffer[lastIndex] != '\0');

	return buffer;
}
