# Compiler-Project
Compiler created for Compiler Theory course.

Steps to build compiler and run it on some code:

    1) in compiler directory, run clang++ or g++
        clang++ scanner.cpp parser.cpp main.cpp -o compiler.out

    2) To compile some .src file into LLVM assembly(code.ll)
        ./compiler.out code.src

    3) To create a.out from the compiled LLVM assembly
        clang code.ll runtime.c
