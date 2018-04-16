# Requires

* CMake build system.
* Compiler support C++14. (MinGW-W64/GCC/Clang recommended)

# Build

```bash
$ cmake . -Bbuild/ 
$ make
```

# Usage

```bash
$ ./mips ./test.asm -o ./test.coe -f coe
$ ./mips ./test.coe -d -o ./dis.asm -f coe
$ ./mips -h
```