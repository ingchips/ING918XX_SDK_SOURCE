# ING918XX/ING916XX SDK Source Code

This repository contains ING918XX/ING916XX SDK source code, including platform header files,
bundle binaries, and examples.

ING918XX/ING916XX are fully integrated Bluetooth Low Energy wireless communication SoC(s)
targeting Bluetooth 5.1 and 5.3 specification.

## Get Started

### The Easy Way

[Download](releases) setup file & install our SDK package, which includes not only source code,
but also GUI tools and documentation.

### The Hard Way

1. Install Gnu Arm toolchain;
1. Install CMSIS;
1. Clone or download this repository;
1. Run [gen_files](tools/README.md) to prepare files;
1. Browse to examples in [examples for Gnu Arm toolchain](examples-gcc);
1. Set environment variable `CMSIS_BASE` to the full path of CMSIS;
1. Run `make`;
1. Use [Flash Downloader](tools/README.md) to download programs.

### The Mixed Way

1. [Download](releases) setup file & install our SDK package;
1. Fork/Clone this repository;
1. Edit _/path/to/sdk/wizard/cfg/settings.json_ while _Wizard_ is closed:
   change _sdk_ to the path of the cloned repository:

    ```json
    {
        "sdk": "path/to/cloned/repository",
    }
    ```

1. Start _Wizard_;
1. Click menu item Tools -> "Re-generate bundle files" in _Wizard_.

## Branches

1. `master` is used for releases;
1. `develop` is used for developing.

## How to Contribute

Contributions - reporting/fixing bugs, adding features, adding documentation - are all welcome. We accept
contributions via Github [Pull Requests](pulls) in the `develop` branch.

## License

   Copyright 2019-2023 INGCHIPS

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
