What is CoreFW?
===============

CoreFW is ObjFW's little brother. It aims to provide "object oriented"
programming for plain C and offers basic classes like arrays, strings and
dictionaries.


### based on midars original code - 
* repackaged with CMake for robust build
* rename CFW -> CF
* snake_case -> PascalCase
* rename files to avoid name collision with std, eg, string.h
* added method toString
* added function CFLog
* added class CFBitVector

### suitable for embedded
tested on wasm, arm, amd64

## install
sudo cmake --install build --prefix "/usr/local"

