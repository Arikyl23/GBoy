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

Simply running the following command in the root directory will generate the build folder and corresponding Make files:

```sh
cmake -B build
```

The next command can then be used to compile GBoy:

```sh
cmake --build build
```

### VS Code Integration

#### Native Integration

To get VS code to integrate with CMake. The CMake tools should be installed and used along side. This allows the build to be completely native in VS Code. 

#### External Terminal

If you want to build from an external terminal and still have VS Code work correctly, the following build options can be used:

```sh
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

This must be ran while initially generated the build folder. If this was not done, remove the folder and regenerate it with the command added.

After building with this command added, a file located at ``build/compile_commands.json`` will appear. Open the C++ Edit Configuration (UI) window and add this to the **Compile Commands**

## Debugging

Debugging this project is a bit annoying at the current moment. A build folder must be generated with the following option enabled:

```sh
-DCMAKE_BUILD_TYPE=Debug
```

If you have already created the build folder, it must be removed and regenerated with this option.

**Note:** This may change where the binary is located inside the build directory.

### Debugging with VS Code

To use VS Code to debug GBoy, follow this steps:

1. Ensure you have built the current build directory with the above Debug option.
2. Create a ``launch.json`` file inside your local ``.vscode`` directory.
3. Inside the ``launch.json`` add a GDB (launch) configuration.
4. Set the ``"program"`` property to where the executable is located. It will most likely be located at ``build/Debug/GBoy`` if you did step 1 correctly.