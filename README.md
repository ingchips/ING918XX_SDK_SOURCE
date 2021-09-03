# ING918XX SDK Source Code

This repository contains ING918XX SDK source code, including platform header files, bundle binaries, and examples.

ING918xx (available in Q2/2020) is a fully integrated Bluetooth Low Energy wireless communication SoC
targeting Bluetooth 5 specification.

## Get Started

### The Easy Way

[Download](/releases) setup file &
install our SDK package, which includes not only source code, but also GUI tools and documentation.

### The Hard Way

1. Install Gnu Arm toolchain;
1. Install CMSIS;
1. Browse to examples in [examples for Gnu Arm toolchain](examples-gcc);
1. Set environment variable `CMSIS_BASE` to the full path of CMSIS;
1. Run `make`.

To build Nim projects, besides of Gnu Arm toolchain & CMSIS:

1. Install Nim compiler;
1. Browse to examples in [examples for Nim](examples-nim);
1. Set environment variable `CMSIS_BASE` to the full path of CMSIS;
1. Set environment variable `NIM_PATH` to the full path of `nim` compiler;
1. Run `make`.

## Branches

1. `master` is used for releases;
1. `develop` is used for developing.

## License

   Copyright 2019-2020 INGCHIPS

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
