#include <time.h>
#include <iostream>
#include <vector>
#include <cstdint>

#include "errors.h"
#include "defs.h"

class ExeFile{
	public:
		FILE* fp;
		DOSHeader dosHeader;
		FileHeader fileHeader;
		OptionalHeader optionalHeader;
		OptionalHeaderPlus optionalHeaderPlus;
		DataEntry data[16];

		DWORD peHeaderOffset;
		WORD numberOfSections = 0;
		WORD numberOfImports = 0;
		
		ExeFile(char filename[]){
			if ((fp = fopen(filename, "rb")) == NULL){
				Error(BUILD_FAILED);
				return;
			}
			
			Update();
		}

		bool ChangeFilename(char filename[]){
			if (initialized)
				fclose(fp);
			if (sectionsAllocated)
				free(sections);

			if ((fp = fopen(filename, "rb")) != NULL){
				Error(NAME_CHANGE_FAILED);
				return false;
			}
			
			Update();
			return true;	
		}
		
		void DisplaySection(int i){
			
			if (!sectionsAllocated){
				Error(SECTIONS_NOT_ALLOC);
				return;
			}

			if (i >= numberOfSections){
				Error(INDEX_OUT_OF_RANGE);
				return;
			}

			printf("\tSection \033[1;35m%s\033[0;0m: {\n\n", sections[i].name);
			printf("\t\tVirtual size: %i\n", sections[i].virtualSize);
			printf("\t\tVirtual address: \033[1;32m0x%x\033[0;0m\n", sections[i].virtualAddress);
			printf("\t\tSize of raw data: %i\n", sections[i].sizeOfRawData);
			printf("\t\tPointer to raw data: \033[1;32m0x%x\033[0;0m\n", sections[i].pointerToRawData);
			printf("\t\tPointer to reallocations: \033[1;32m0x%x\033[0;0m\n", sections[i].pointerToReallocations);
			printf("\t\tPointer to line numbers: \033[1;32m0x%x\033[0;0m\n", sections[i].pointerToLineNumbers);
			printf("\t\tNumber of reallocations: %i\n", sections[i].numberOfReallocations);
			printf("\t\tNumber of line numbers: %i\n", sections[i].numberOfLineNumbers);
			printf("\t\tCharacteristics: \033[1;32m0x%hx\033[0;0m\n\n\t}\n\n", sections[i].characteristics);
		}

		void DisplayDosHeader(){
			if (!initialized){
				Error(OBJECT_NOT_INIT);
				return;
			}

			puts("DOS HEADER: {\n");

			printf("\te_magic: %hx\n", dosHeader.e_magic);
			printf("\te_cblp: %hx\n", dosHeader.e_cblp);
			printf("\te_cp: %hx\n", dosHeader.e_cp);
			printf("\te_crlc: %hx\n", dosHeader.e_crlc);
			printf("\te_cparhdr: %hx\n", dosHeader.e_cparhdr);
			printf("\te_minalloc: %hx\n", dosHeader.e_minalloc);
			printf("\te_maxalloc: %hx\n", dosHeader.e_maxalloc);
			printf("\te_ss: %hx\n", dosHeader.e_ss);
			printf("\te_sp: %hx\n", dosHeader.e_sp);
			printf("\te_csum: %hx\n", dosHeader.e_csum);
			printf("\te_ip: %hx\n", dosHeader.e_ip);
			printf("\te_cs: %hx\n", dosHeader.e_cs);
			printf("\te_lfarlc: %hx\n", dosHeader.e_lfarlc);
			printf("\te_ovno: %hx\n", dosHeader.e_ovno);
			printf("\te_res: %hx %hx %hx %hx\n", 
					dosHeader.e_res[0], 
					dosHeader.e_res[1], 
					dosHeader.e_res[2], 
					dosHeader.e_res[3]
			);
			printf("\te_oemid: %hx\n", dosHeader.e_oemid);
			printf("\te_oeminfo: %hx\n", dosHeader.e_oeminfo);
			printf("\te_res2: %hx %hx %hx %hx %hx %hx %hx %hx %hx %hx\n",
					dosHeader.e_res2[0],
					dosHeader.e_res2[1],
					dosHeader.e_res2[2],
					dosHeader.e_res2[3],
					dosHeader.e_res2[4],
					dosHeader.e_res2[5],
					dosHeader.e_res2[6],
					dosHeader.e_res2[7],
					dosHeader.e_res2[8],
					dosHeader.e_res2[9]
			);
			printf("\te_lfanew: 0x%x\n\n}\n\n", dosHeader.e_lfanew);
		}

		void DisplayFileHeader(){
			if (!initialized){
				Error(OBJECT_NOT_INIT);
				return;
			}
			
			char dateTimeBuffer[32];
			time_t timeStamp = static_cast<time_t>(fileHeader.timeDateStamp);
			struct tm* tmp = localtime(&timeStamp);

			strftime(dateTimeBuffer, sizeof(dateTimeBuffer), "%H:%M:%S - %d/%m/%Y", tmp);

			puts("FILE HEADER: {\n");

			printf("\tmachine: \033[1;32m0x%hx\033[0;0m | \033[1;35m%s\033[0;0m\n", fileHeader.machine, GetArchitecture(fileHeader.machine).c_str());
			printf("\tNumber of sections: %i\n", fileHeader.numberOfSections);
			printf("\tTime Date stamp: \033[1;33m%s\033[0;0m\n", dateTimeBuffer);
			printf("\tPointer to symbol table: \033[1;32m0x%x\033[0;0m\n", fileHeader.pointerToSymbolTable);
			printf("\tNumber of symbols: %i\n", fileHeader.numberOfSymbols);
			printf("\tSize of optional header: %i\n", fileHeader.sizeOfOptionalHeader);
			printf("\tCharacteristics: \033[1;32m0x%x\033[0;0m\n\n}\n\n", fileHeader.characteristics);
		}
		
		void DisplayOptionalHeader(){
			if (!initialized){
				Error(OBJECT_NOT_INIT);
				return;
			}

			puts("OPTIONAL HEADER: {\n");
			
			if (!pe32){
				printf("\tmagic: \033[1;32m0x%hx\033[0;0m | \033[1;35m%s\033[0;0m\n", optionalHeaderPlus.magic, PEFormat(optionalHeaderPlus.magic).c_str());
				printf("\tMajor linker version: %i\n", optionalHeaderPlus.majorLinkerVersion);
				printf("\tMinor linker version: %i\n", optionalHeaderPlus.minorLinkerVersion);
				printf("\tSize of code: %i\n", optionalHeaderPlus.sizeOfCode);
				printf("\tSize of initialized data: %i\n", optionalHeaderPlus.sizeOfInitializedData);
				printf("\tSize of uninitialized data: %i\n", optionalHeaderPlus.sizeOfUninitializedData);
				printf("\tAddress of entry point: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.addressOfEntryPoint);
				printf("\tBase of code: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.baseOfCode);
				printf("\tImage base: \033[1;32m0x%llx\033[0;0m\n", optionalHeaderPlus.imageBase);
				printf("\tSection alignment: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.sectionAlignment);
				printf("\tFile alignment: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.fileAlignment);
				printf("\tMajor OS version: %i\n", optionalHeaderPlus.majorOperatingSystemVersion);
				printf("\tMinor OS version: %i\n", optionalHeaderPlus.minorOperatingSystemVersion);
				printf("\tMajor image version: %i\n", optionalHeaderPlus.majorImageVersion);
				printf("\tMinor image version: %i\n", optionalHeaderPlus.minorImageVersion);
				printf("\tMajor Subsystem version: %i\n", optionalHeaderPlus.majorSubsystemVersion);
				printf("\tMinor Subsystem version: %i\n", optionalHeaderPlus.minorSubsystemVersion);
				printf("\tWin 32 Version value: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.win32VersionValue);
				printf("\tSize of image: %i\n", optionalHeaderPlus.sizeOfImage);
				printf("\tSize of headers: %i\n", optionalHeaderPlus.sizeOfHeaders);
				printf("\tChecksum: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.checksum);
				printf("\tSubsystem: \033[1;32m0x%hx\033[0;0m | \033[1;35m%s\033[0;0m\n", optionalHeaderPlus.subsystem, GetSubsystem(optionalHeaderPlus.subsystem).c_str());
				printf("\tDll characteristics: \033[1;32m0x%hx\033[0;0m\n", optionalHeaderPlus.dllCharacteristics);
				printf("\tSize of stack reserve: %llu\n", optionalHeaderPlus.sizeOfStackReserve);
				printf("\tSize of stack commit: %llu\n", optionalHeaderPlus.sizeOfStackCommit);
				printf("\tSize of heap reserve: %llu\n", optionalHeaderPlus.sizeOfHeapReserve);
				printf("\tSize of heap commit: %llu\n", optionalHeaderPlus.sizeOfHeapCommit);
				printf("\tLoader flags: \033[1;32m0x%x\033[0;0m\n", optionalHeaderPlus.loaderFlags);
				printf("\tNumber of RVA and sizes: %i\n\n}\n\n", optionalHeaderPlus.numberOfRVAAndSizes);
			} else {
				printf("\tmagic: \033[1;32m0x%hx\033[0;0m | \033[1;35m%s\033[0;0m\n", optionalHeader.magic, PEFormat(optionalHeader.magic).c_str());
				printf("\tMajor linker version: %i\n", optionalHeader.majorLinkerVersion);
				printf("\tMinor linker version: %i\n", optionalHeader.minorLinkerVersion);
				printf("\tSize of code: %i\n", optionalHeader.sizeOfCode);
				printf("\tSize of initialized data: %i\n", optionalHeader.sizeOfInitializedData);
				printf("\tSize of uninitialized data: %i\n", optionalHeader.sizeOfUninitializedData);
				printf("\tAddress of entry point: \033[1;32m0x%x\033[0;0m\n", optionalHeader.addressOfEntryPoint);
				printf("\tBase of code: \033[1;32m0x%x\033[0;0m\n", optionalHeader.baseOfCode);
				printf("\tBase of data: \033[1;32m0x%x\033[0;0m\n", optionalHeader.baseOfData);
				printf("\tImage base: \033[1;32m0x%x\033[0;0m\n", optionalHeader.imageBase);
				printf("\tSection alignment: \033[1;32m0x%x\033[0;0m\n", optionalHeader.sectionAlignment);
				printf("\tFile alignment: \033[1;32m0x%x\033[0;0m\n", optionalHeader.fileAlignment);
				printf("\tMajor OS version: %i\n", optionalHeader.majorOperatingSystemVersion);
				printf("\tMinor OS version: %i\n", optionalHeader.minorOperatingSystemVersion);
				printf("\tMajor image version: %i\n", optionalHeader.majorImageVersion);
				printf("\tMinor image version: %i\n", optionalHeader.minorImageVersion);
				printf("\tMajor Subsystem version: %i\n", optionalHeader.majorSubsystemVersion);
				printf("\tMinor Subsystem version: %i\n", optionalHeader.minorSubsystemVersion);
				printf("\tWin 32 Version value: \033[1;32m0x%x\033[0;0m\n", optionalHeader.win32VersionValue);
				printf("\tSize of image: %i\n", optionalHeader.sizeOfImage);
				printf("\tSize of headers: %i\n", optionalHeader.sizeOfHeaders);
				printf("\tChecksum: \033[1;32m0x%x\033[0;0m\n", optionalHeader.checksum);
				printf("\tSubsystem: \033[1;32m0x%hx\033[0;0m | \033[1;35m%s\033[0;0m\n", optionalHeader.subsystem, GetSubsystem(optionalHeader.subsystem).c_str());
				printf("\tDll characteristics: \033[1;32m0x%hx\033[0;0m\n", optionalHeader.dllCharacteristics);
				printf("\tSize of stack reserve: %xu\n", optionalHeader.sizeOfStackReserve);
				printf("\tSize of stack commit: %xu\n", optionalHeader.sizeOfStackCommit);
				printf("\tSize of heap reserve: %xu\n", optionalHeader.sizeOfHeapReserve);
				printf("\tSize of heap commit: %xu\n", optionalHeader.sizeOfHeapCommit);
				printf("\tLoader flags: \033[1;32m0x%x\033[0;0m\n", optionalHeader.loaderFlags);
				printf("\tNumber of RVA and sizes: %i\n\n}\n\n", optionalHeader.numberOfRVAAndSizes);
			}
		}
		
		void DisplayImport(int i){
			if (!importsAllocated){
				Error(IMPORTS_NOT_ALLOC);
				return;
			}

			if (i >= data[1].size){
				Error(INDEX_OUT_OF_RANGE);
				return;
			}
			
			DWORD nameOffset = RvaToOffset(imports[i].name, sections, numberOfSections);

			printf("\tImport %i: {\n\n", i);
			printf("\t\tOriginal first thunk: \033[1;32m0x%x\033[0;0m\n", imports[i].originalFirstThunk);
			printf("\t\tTime Date stamp: 0x%x\n", imports[i].timeDateStamp);
			printf("\t\tForwarder chain: 0x%x\n", imports[i].forwarderChain);
			printf("\t\tName: \033[1;32m0x%x\033[0;0m | \033[1;35m%s\033[0;0m\n", imports[i].name, ReadString(fp, nameOffset).c_str());
			printf("\t\tFirst thunk: \033[1;32m0x%x\033[0;0m\n\n\t}\n\n", imports[i].firstThunk);
		}

		void DisplayFullInfo(){
			if (initialized){
				DisplayDosHeader();
				DisplayFileHeader();
				DisplayOptionalHeader();
			}

			if (sectionsAllocated){
				puts("SECTION HEADER:");

				for (int i = 0; i < numberOfSections; i++){
					DisplaySection(i);
				}
			}

			if (importsAllocated){
				puts("IMPORTS TABLE:");
				
				for (int i = 0; i < numberOfImports-1; i++){
					DisplayImport(i);
					DisplayCalls(&(imports[i]));
				}
			}
		}
		
		size_t WriteImportedFunctionNames(ImportEntry* import, std::vector<std::string>* buffer){
			DWORD thunkRVA = import->originalFirstThunk ? import->originalFirstThunk : import->firstThunk;
			DWORD thunkOffset = RvaToOffset(thunkRVA, sections, numberOfSections);
			DWORD* nameOffsets = (DWORD*) malloc(sizeof(DWORD));
			size_t size = sizeof(DWORD);
			int i = 0;
			
			fseek(fp, thunkOffset, SEEK_SET);

			while (true){
				if (!pe32){
					QWORD nameAddress;

					fread(&nameAddress, sizeof(QWORD), 1, fp);

					if (nameAddress == 0) break;
					if (!(nameAddress & 0x8000000000000000)){
						nameOffsets = (DWORD*) realloc(nameOffsets, size + sizeof(DWORD));
						size += sizeof(DWORD);

						DWORD functionNameOffset = RvaToOffset(nameAddress, sections, numberOfSections);

						nameOffsets[i] = functionNameOffset;
					}
					i++;

					fseek(fp, thunkOffset + sizeof(QWORD) * i, SEEK_SET);
				} else {
					DWORD nameAddress;

					fread(&nameAddress, sizeof(DWORD), 1, fp);

					if (nameAddress == 0) break;
					if (!(nameAddress & 0x80000000)){
						nameOffsets = (DWORD*) realloc(nameOffsets, size + sizeof(DWORD));
						size += sizeof(DWORD);

						DWORD functionNameOffset = RvaToOffset(nameAddress, sections, numberOfSections);

						nameOffsets[i] = functionNameOffset;
					}
					i++;

					fseek(fp, thunkOffset + sizeof(DWORD) * i, SEEK_SET);
				}
			}

			for (int j = 0; j < i; j++){
				std::string name = ReadFuncName(fp, nameOffsets[j]);
				(*buffer).push_back(name);
			}

			free(nameOffsets);

			return i;
		}

		void DisplayCalls(ImportEntry* import){
			std::vector<std::string> buffer;
			
			printf("\tDISPLAYING FUNCTION CALLS FOR %s:\n", ReadString(fp, RvaToOffset(import->name, sections, numberOfSections)).c_str());
			
			size_t size = WriteImportedFunctionNames(import, &buffer);

			for (int j = 0; j < size; j++){
				printf("\t\tFound: \033[1;36m%s\033[0;0m\n", buffer[j].c_str());
			}
			
			printf("\tEND\n\n");
		}

		void FreeSections(){
			if (!sectionsAllocated){
				Error(SECTIONS_NOT_ALLOC);
				return;
			}

			free(sections);

			sectionsAllocated = false;
		}

		void FreeImports(){
			if (!importsAllocated){
				Error(IMPORTS_NOT_ALLOC);
				return;
			}

			free(imports);
			importsAllocated = false;
		}

		void GetSection(int index, Section* buffer){
			if (!sectionsAllocated){
				Error(SECTIONS_NOT_ALLOC);
				return;
			}

			if (index >= numberOfSections){
				Error(INDEX_OUT_OF_RANGE);
				return;
			}

			buffer = &sections[index];
		}

		void GetImport(int index, ImportEntry* buffer){
			if (!importsAllocated){
				Error(IMPORTS_NOT_ALLOC);
				return;
			}

			if (index >= numberOfImports){
				Error(INDEX_OUT_OF_RANGE);
				return;
			}

			buffer = &imports[index];
		}

		void GetData(int index, DataEntry* buffer){
			if (!initialized){
				Error(OBJECT_NOT_INIT);
				return;
			}

			if (index >= 16){
				Error(INDEX_OUT_OF_RANGE);
				return;
			}

			buffer = &data[index];
		}

		~ExeFile(){
			if (initialized)
				fclose(fp);
			
			FreeSections();
			FreeImports();
		}

	private:
		Section* sections;
		ImportEntry* imports;

		bool initialized = false;
		bool sectionsAllocated = false;
		bool importsAllocated = false;

		bool pe32 = false;
		WORD peFormat = 0;

		void Update(){
			fread(&dosHeader, sizeof(DOSHeader), 1, fp); // Reading DOS Header

			peHeaderOffset = dosHeader.e_lfanew;

			fseek(fp, peHeaderOffset, SEEK_SET); // Moving to NT Header position in file
			fread(&fileHeader, sizeof(FileHeader), 1, fp); // Reading File Header
			fread(&peFormat, sizeof(WORD), 1, fp);

			if (peFormat != 0x20b)
				pe32 = true;
			
			fseek(fp, peHeaderOffset+sizeof(FileHeader), SEEK_SET);

			if (!pe32)
				fread(&optionalHeaderPlus, sizeof(OptionalHeaderPlus), 1, fp); // Reading Optional Header for PE32+ format
			else
				fread(&optionalHeader, sizeof(OptionalHeader), 1, fp); // Reading Optional Header for PE32 format

			for (int i = 0; i < 16; i++){
				fread(&data[i], sizeof(DataEntry), 1, fp);
			}

			numberOfSections = fileHeader.numberOfSections;
			initialized = true;

			ReadSections();
			ReadImports();
		}

		void ReadImports(){
			if (!initialized){
				Error(OBJECT_NOT_INIT);
				return;
			}
			
			ImportEntry import;
			DWORD importTableAddress = RvaToOffset(data[1].virtualAddress, sections, numberOfSections);
			
			fseek(fp, importTableAddress, SEEK_SET);
			
			do{
				fread(&import, sizeof(ImportEntry), 1, fp);

				numberOfImports++;
			} while (import.name != 0);
			
			imports = (ImportEntry*) malloc(sizeof(ImportEntry)*numberOfImports);
			fseek(fp, importTableAddress, SEEK_SET);

			for (int i = 0; i < numberOfImports; i++){
				fread(&(imports[i]), sizeof(ImportEntry), 1, fp);
			}

			importsAllocated = true;
		}

		void ReadSections(){
            if (!initialized){
                Error(OBJECT_NOT_INIT);
                return;
            }

            if (sectionsAllocated){
                return;
            }

            sections = (Section*) malloc(sizeof(Section)*numberOfSections);

            for (int i = 0; i < numberOfSections; i++){
                fread(&(sections[i]), sizeof(Section), 1, fp);
            }

            sectionsAllocated = true;
    	}
};
