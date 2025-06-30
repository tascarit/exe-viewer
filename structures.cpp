#include <cstdint>

#define QWORD uint64_t
#define DWORD uint32_t
#define WORD uint16_t
#define BYTE uint8_t
#define ULL unsigned long long

struct DOSHeader{
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo;
	WORD e_res2[10];
	DWORD e_lfanew;
};

struct FileHeader{
	DWORD signature;
	WORD machine;
	WORD numberOfSections;
	DWORD timeDateStamp;
	DWORD pointerToSymbolTable;
	DWORD numberOfSymbols;
	WORD sizeOfOptionalHeader;
	WORD characteristics;
};

struct OptionalHeaderPlus{
	WORD magic;
	BYTE majorLinkerVersion;
	BYTE minorLinkerVersion;
	DWORD sizeOfCode;
	DWORD sizeOfInitializedData;
	DWORD sizeOfUninitializedData;
	DWORD addressOfEntryPoint;
	DWORD baseOfCode;
	ULL imageBase;
	DWORD sectionAlignment;
	DWORD fileAlignment;
	WORD majorOperatingSystemVersion;
	WORD minorOperatingSystemVersion;
	WORD majorImageVersion;
	WORD minorImageVersion;
	WORD majorSubsystemVersion;
	WORD minorSubsystemVersion;
	DWORD win32VersionValue;
	DWORD sizeOfImage;
	DWORD sizeOfHeaders;
	DWORD checksum;
	WORD subsystem;
	WORD dllCharacteristics;
	ULL sizeOfStackReserve;
	ULL sizeOfStackCommit;
	ULL sizeOfHeapReserve;
	ULL sizeOfHeapCommit;
	DWORD loaderFlags;
	DWORD numberOfRVAAndSizes;

};

struct OptionalHeader{
	WORD magic;
	BYTE majorLinkerVersion;
	BYTE minorLinkerVersion;
	DWORD sizeOfCode;
	DWORD sizeOfInitializedData;
	DWORD sizeOfUninitializedData;
	DWORD addressOfEntryPoint;
	DWORD baseOfCode;
	DWORD baseOfData;
	DWORD imageBase;
	DWORD sectionAlignment;
	DWORD fileAlignment;
	WORD majorOperatingSystemVersion;
	WORD minorOperatingSystemVersion;
	WORD majorImageVersion;
	WORD minorImageVersion;
	WORD majorSubsystemVersion;
	WORD minorSubsystemVersion;
	DWORD win32VersionValue;
	DWORD sizeOfImage;
	DWORD sizeOfHeaders;
	DWORD checksum;
	WORD subsystem;
	WORD dllCharacteristics;
	DWORD sizeOfStackReserve;
	DWORD sizeOfStackCommit;
	DWORD sizeOfHeapReserve;
	DWORD sizeOfHeapCommit;
	DWORD loaderFlags;
	DWORD numberOfRVAAndSizes;
};

struct Section{
	char name[8];
	DWORD virtualSize;
	DWORD virtualAddress;
	DWORD sizeOfRawData;
	DWORD pointerToRawData;
	DWORD pointerToReallocations;
	DWORD pointerToLineNumbers;
	WORD numberOfReallocations;
	WORD numberOfLineNumbers;
	DWORD characteristics;
};

struct DataEntry{
	DWORD virtualAddress;
	DWORD size;
};

struct ImportEntry{
	DWORD originalFirstThunk;
	DWORD timeDateStamp;
	DWORD forwarderChain;
	DWORD name;
	DWORD firstThunk;
};
