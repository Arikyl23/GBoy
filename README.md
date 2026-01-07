# GBoy: Another Gameboy Emulator

GBoy is a Gameboy emulator being developed as a hobby project. It is primarialy a deep dive into learning emulation as a whole and won't claim to be the best at anything in particular.

## Goals

As previously mentioned, this is a project geared towards learning emulation. The major goal for the project is to get it to pass all of Blarg's test ROMs with a focus on emulation accuracy over speed.

### Potential Future Goals

For now, only the base model of the Gameboy will be emulated; however, more may be added after completion of the base Gameboy.

### Collaboration

Since this is more of a learning project for me. I would like to primarily work on this on my own. Anyone is welcome to pull this code and fork it as they see fit.

## Building

Currently, a CMake build system is in use. It will automatically configure the project to correctly build and link SDL3 and additional libraries. The CMake still leaves a lot to be desired and I still have a lot to learn in regards to it.

I have written a script to wrap the CMake build system greatly simplifing the build process. It can be located inside of the tools folder. This script is safe to run from anywhere as it operates off its own location instead of the terminals.

### Generating The Build Folder

To generate the build folder, the following command can be used:

```sh
buildtools.sh generate
```

**Note:** This always defaults to a **DEBUG** configuration.

**Note:** This must always be done at least once before starting an actual build.

### Compiling the Project

To compile the project, the following command can be used:

```sh
buildtools.sh build
```

**Note:** This always defaults to a **DEBUG** configuration.

### Build Configurations

Currently, only two build configurations are supported: **DEBUG** and **RELEASE**. The build script will always default to **DEBUG** unless instructed otherwise.

Both the ``generate`` and ``build`` modes can have their configuration specified. Below is an example for each:

```sh
# Generates the build folder for a RELEASE build
buildtools.sh generate --configuration RELEASE

# Compiles the project with the RELEASE configuration
buildtools.sh build RELEASE
```

**Note:** Both configurations are mantained completely seperately from each other allowing you to have both a **DEBUG** and **RELEASE** configuration built at the same time.

### Clean Building

When a change is made to a ``CMakeList.txt`` file, you should almost always completely regenerate the build files. This is to prevent accidentally building with stale Make files. There are several methods provided in the build script to handle this. This is generally referred to as a clean build:

```sh
# This purges the entire build folder included all currently generated configurations
# Use this to completely purge the entire project
buildtools.sh clean

# This only purges the respective configuration build folder
# In this case, it is purging the DEBUG build folder
buildtools.sh generate -clean

# Same as the command above except it acts only on the RELEASE configuration
buildtools.sh generate -clean --configuration RELEASE
```

### VS Code Integration

Since we are using a script to control CMake. The VS Code CMake extension should not be used. This means VS Code won't be in charge of the configuration and we need to tell it how to understand the project. To get VS Code to correctly detect the project files. The ``-vsc_integration`` command can be added to the generation mode of the script. Below is an example of its use:

```sh
buildtools.sh generate -vsc_integration
```

This causes the script to instruct CMake to create a ``compile_commands.json`` file inside the build folder. Inside of VS Code, add the path to this file under the ``compileCommands`` option in your ``c_cpp_properties.json`` file. Below is an example ``c_cpp_properties.json`` file:

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
            "compileCommands": [
                "${workspaceFolder}/build/debug/compile_commands.json",
                "${workspaceFolder}/build/release/compile_commands.json"
            ]
        }
    ],
    "version": 4
}
```

**Note:** Due to how CMake works, there is a ``compile_commands.json`` file for each configuration. VS Code won't throw an error if it can't find the file. It will only highlight it with a warning. This makes it safe to blindly include both possible locations so you never have to worry about the files actual existance.

**Note:** If you auto generated your ``c_cpp_properties.json`` file, there may be an option called ``configurationProvider``. This option tells VS Code what extension should be used to configure the Project and resulting Intellisense. Since CMake is handled externally in the build script, this option should not be set. You may need to also disable the CMake extension for this workspace if it continues to run on its own.

#### CMake Extension

If you choose to use the CMake extension in VS Code, you should not invoke the build script from the command line. This can cause instability due to the conflicting ways each one calls CMake.

## Debugging

To be able to debug with something like GDB, you must first build the project in **DEBUG** configuration. This ensure the debugging symbols are included in the build.

**Note:** The build script always defaults to the **DEBUG** configuration if none is specified.

### Debugging with VS Code

To use VS Code to debug GBoy, follow these steps:

1. Ensure you have built the project with the **DEBUG** configuration.
2. Create a ``launch.json`` file inside your local ``.vscode`` directory.
3. Inside the ``launch.json`` add a ``GDB (launch)`` configuration.
4. Set the ``program`` property to where the executable is located. It will most likely be located at ``build/debug/bin/GBoy`` if you did step 1 correctly.

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