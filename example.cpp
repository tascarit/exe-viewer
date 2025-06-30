#include <iostream>
#include "exeFile.cpp"

int main(void){
	char filename[256];

	scanf_s("%s", filename);
	
	ExeFile file(filename);
	file.DisplayFullInfo();

	return 0;
}
