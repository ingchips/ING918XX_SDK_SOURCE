# ING920 ROM Companion

This is a companion for platform binary in ROM of ING920. Purpose of this
companion:

* To initialize SoC, such as system clocks;
* To provide patches.

Base on this example, developers can create their own companion to fulfill the needs.
For example:

* Download/Update app through USB;
* Verify app before running it.

## Step-by-step customization

Here are the steps to develop a customized companion and use it in a new project.

1. Copy this example to somewhere else, and update project options to make it compilable;

1. Develop all functionalities;

1. Run [`make_bundle.py`](../scripts/make_bundle.py);

1. Create a new project in Wizard: select "ROM" and "COPY to my project";

    Assume the project is located in "/path/my_project".

1. Use files in [_generated_](../generated/) to replace corresponding files
   in directory "/path/my_project/sdk/bundles/rom/ING9208xx" of the newly created project;

1. Run [`gen_files`] to generate symbol files:

    ```sh
    gen_files /path/my_project/sdk/bundles
    ```

    [`gen_files`](../../../tools/gen_files.nim) executable can be found in "wizard" directory of SDK installation.

If the companion gets updated, steps 5 & 6 shall be repeated.

Note: Platform version is defined in the companion. It must be defined properly,
otherwise it can bring issues to firmware update, version management, debugging, etc.