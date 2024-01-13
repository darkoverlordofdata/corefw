What is CoreFW?
===============

CoreFW is ObjFW's little brother. It aims to provide "object oriented"
programming for plain C and offers basic classes like arrays, strings and
dictionaries.


### based on midars original code - 
* repackaged with CMake for robust build
* added toString to CFObject
* rename CFW -> CF
* snake_case -> PascalCase
* rename files to avoid name collision

## install
sudo cmake --install build --prefix "/usr/local"