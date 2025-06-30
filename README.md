# exe-viewer
## Allows you to view EXE file headers and imports table with all imported functions in command line, also you can use this as a library

`exeFile.cpp` contains ExeFile class that allows you to load file just by name and view headers.

`structures.cpp` contains several structures such as FileHeader, OptionalHeader or ImportEntry and etc.

`utility.cpp` contains all utility functions such as ReadString() or RvaToOffset()

`defs.h` contains definitions for all utility functions

`errors.h` contains several macro for errors

in `example.cpp` i wrote a sample code you can view exe file headers and etc. with.

## Example compilation for Windows

`g++ -o example.exe example.cpp exeFile.cpp utility.cpp structures.cpp`

Example usage: `.\example.exe` and after that type path to your target EXE file

### I'm just starting to learn this topic so this code probably containing bugs, feel free to post them in issues

## Screenshots

![1](https://i.ibb.co/CswKt9Fp/Screenshot-2025-06-30-141434.png)
![2](https://i.ibb.co/Z6BsCR2n/Screenshot-2025-06-30-141621.png)
![3](https://i.ibb.co/fYSNDz6X/Screenshot-2025-06-30-141713.png)
