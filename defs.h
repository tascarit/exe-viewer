#include <string>
#include "structures.cpp"

#define DUMP(variable) #variable

std::string GetArchitecture(long value);
std::string PEFormat(long value);
std::string ReadString(FILE* fp, DWORD nameOffset);
std::string ReadFuncName(FILE* fp, DWORD nameOffset);
std::string GetSubsystem(long value);
DWORD RvaToOffset(DWORD rva, Section* sections, size_t sectionsCount);

void Error(const char*);
