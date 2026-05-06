# GBoy: Another Gameboy Emulator

GBoy is a Gameboy emulator being developed as a hobby project. It is primarialy a deep dive into learning emulation as a whole and won't claim to be the best at anything in particular.

## Goals

As previously mentioned, this is a project geared towards learning emulation. The major goal for the project is to get it to pass all of Blarg's test ROMs with a focus on emulation accuracy over speed.

### Potential Future Goals

For now, only the base model of the Gameboy will be emulated; however, more may be added after completion of the base Gameboy.

### Collaboration

Since this is more of a learning project for me. I would like to primarily work on this on my own. Anyone is welcome to pull this code and fork it as they see fit.

## Submodules and Cloning

This project makes use of git submodules to vendor the large SDL libraries. This prevents their large repositories from also being tracked within our repository. This also means that cloning is slightly different than normal:

```sh
git clone --recurse-submodules <https|ssh>
```

If you already cloned the project without specifying the above `--recurse-submodules` option. That's okay, you can still get the submodules after the fact by running the following command instead:

```sh
git submodule update --init --recursive
```

**Note:** Both of these commands will recursively trace each submodule, fetching their required submodules until the entire source tree is created. This may take a awhile on slow networks.

## Building

Currently, a CMake build system is in use. It will automatically configure the project to correctly build and link SDL3 and additional libraries. The CMake still leaves a lot to be desired and I still have a lot to learn in regards to it.

I have written a script to wrap the CMake build system greatly simplifing the build process. It can be located inside of the tools folder. This script is safe to run from anywhere as it operates off its own location instead of the terminals.

### Buildtools Script

The buildtools script is a power bash script that handles everything related to generating, building, testing, and running GBoy.

To build GBoy, simply run the following command:

```bash
buildtools.sh build
```

**Note:** This always defaults to a **DEBUG** configuration. To build a different configuration, add it after the build command option like so:
```bash
buildtools.sh build release
```

#### Build Configurations

Currently, there are 3 build configurations that are supported:
- **DEBUG**
    - Debug configuration. Includes debug symbols and disables optimization in the build.
- **RELEASE**
    - Release configuration. Removes debug symbols and enables optimization in the build.
- **ASAN**
    - ASan Configuration. Same as **DEBUG** configuration with the Address Sanitizer (ASan) also enabled.

**Note** Most commands in the build script will always default to **DEBUG** unless instructed otherwise.

Every command can have the configuration specified. This allows multiple configurations to be built and managed at the same time. Below are some examples of a few commands:

```bash
# Removes releases build artifacts from the build folder
buildtools.sh clean release

# Compiles the project with the RELEASE configuration
buildtools.sh build debug

# Tests the currently build ASAN configuration with CTest
buildtools.sh test asan
```

**Note:** All configurations are mantained completely seperately from each other allowing you to have multiple configurations built at the same time.

#### Clean Building

When a change is made to a ``CMakeList.txt`` file, you should almost always completely regenerate the build files. This is to prevent accidentally building with stale Make files. This is generally referred to as a clean build. There are several methods provided in the build script to handle this:

```bash
# This purges the entire build folder included all currently generated configurations
# Use this to completely purge the entire project
buildtools.sh clean

# This only purges the respective configuration build folder
# In this case, it is purging the DEBUG build folder
buildtools.sh clean debug

# This performs the previous command before building a debug build
# This is the easiest and most robust way to handle clean building
buildtools.sh build --clean
```

#### VS Code Integration

Since we are using a script to control CMake. The VS Code CMake extension should not be used. This means VS Code won't be in charge of the configuration and we need to tell it how to understand the project. To get VS Code to correctly detect the project files. The ``--vsc`` option can be added to the build command. Below is an example of its use:

```sh
# Generates the project with VS Code Integration enabled
# If the project is already built, a clean build may be needed to ensure this takes effect
buildtools.sh build --vsc
```

This causes the script to instruct CMake to create a ``compile_commands.json`` file inside the configuration build folder during generation. When a generation or build stage finishes, it checks for the presence of this file in the target configuration build folder. If found, a symlink to it is generated inside the root build folder (`GBoy/build/compile_commands.json`). This symlink will always be set to the last completed generation/build stage.

Inside of VS Code, the ``compileCommands`` option in your ``c_cpp_properties.json`` file should be set to the path to the symlink (`GBoy/build/compile_commands.json`). This ensures VS code will always be automatically sync'd to the build system. Below is an example of what the ``c_cpp_properties.json`` file should look like:

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c17",
            "cppStandard": "gnu++17",
            "intelliSenseMode": "linux-gcc-x64",
            "compileCommands": "${workspaceFolder}/build/compile_commands.json"
        }
    ],
    "version": 4
}
```

**Note:** If you auto-generated your ``c_cpp_properties.json`` file, there may be an option called ``configurationProvider``. This option tells VS Code what extension should be used to configure the Project and resulting Intellisense. Since CMake is handled externally in the build script, this option should not be set. You may need to also disable the CMake extension for this workspace if it continues to run on its own.

### CMake Extension

If you choose to use the CMake extension in VS Code, you should not invoke the build script from the command line. This can cause instability due to the conflicting ways each one calls CMake. Since I do not use the extension, I cannot provide any advice with setting it up for GBoy.

## Debugging

To be able to debug with something like GDB, you must first build the project in **DEBUG** configuration. This ensure the debugging symbols are included in the build.

**Note:** The build script always defaults to the **DEBUG** configuration if none is specified.

### Debugging with VS Code

To use VS Code to debug GBoy, follow these steps:

1. Ensure you have built the project with the **DEBUG** configuration.
2. Create a ``launch.json`` file inside your local ``.vscode`` directory.
3. Inside the ``launch.json`` add a ``GDB (launch)`` configuration.
4. Set the ``program`` property to where the executable is located. It will most likely be located at ``GBoy/build/debug/bin/GBoy`` if you did step 1 correctly.

Below is an example of how your ``launch.json`` should look:

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/debug/bin/GBoy",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${fileDirname}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                },
                {
                    "description": "Set Disassembly Flavor to Intel",
                    "text": "-gdb-set disassembly-flavor intel",
                    "ignoreFailures": true
                }
            ]
        }

    ]
}
```

If all the above steps were correctly followed. You should be able to run the debugger in VS Code as expected.