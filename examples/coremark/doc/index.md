# CoreMark

Run [CoreMark](https://www.eembc.org/coremark) benchmark on ING918xx or ING916xx families.

Compiler options affects final score significantly. To achieve higher score,
always use latest IDE/toolchain, and below are recommendations on compiler options:

* ING918xx Family

    * Use compiler version 6 (ARMClang)
    * Enable LTO
    * Optimization: `-Ofast`
    * One ELF per function
    * Preprocessor: `ITERATIONS=3000`

* ING916xx Family

    * Use compiler version 6 (ARMClang)
    * Enable LTO
    * Optimization: `-Ofast`
    * One ELF per function
    * Preprocessor: `ITERATIONS=15000`