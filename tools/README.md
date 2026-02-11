# Tools Directory

This directory is responsible for housing simple tool scripts. These are generally small programs that help automate some tasks such as the build process.

## List Of Tools

Here is a list of the current tools that can be found in this directory. You can find more details about each below in its respective section:

- [Buildtool](#buildtool)

### Buildtool

**File:** buildtool.sh
**Date Created:** 07/01/2026
**Last Modified:** 11/02/2026

This is the primary build script used to generating, cleaning, building, testing, and running the project. It is safe to invoke anywhere in the filesystem as it builds the project relative to itself.

#### Usage

```
GBoy Build & Management Tools
Usage: buildtool.sh <command> [configuration] [options]

Commands:
  clean [configuration]            Removes build artifacts. Wipes all builds if no configuration is given.
  build [configuration] [options]  Builds the project. If configuration hasn't been generated, automatically generates build files.
  test  [configuration]            Runs CTest on the project.
  help  [command]                  Displays this message.

Configurations:
  debug      Debug Configuration. Builds with debug symbols included and optimization off.
  release    Release Configuration. Builds with debug symbols stripped and optimization on.
  asan       ASan Configuration. Debug build with Address Sanitizer enabled.

================================================================================

----- Clean Command -----
Usage: buildtool.sh clean [configuration]

Removes build artifacts. Effectively recursively deletes the selected build
directory. Parent build directory 'GBoy/build' is targeted if no configuration
is given. Below are some examples:

Select all build fragments:
./buildtool.sh clean

Select DEBUG build fragments:
./buildtool.sh clean debug


================================================================================

----- Build Command -----
Usage: buildtool.sh build [configuration] [options]

Builds the project. Will automatically generate build files if out of date or
missing. Defaults to a DEBUG configuration if no configuration is given.

Options:
  -c | --clean     Performs a clean build by removing configuration build artifacts and regenerating before building.
  --parallel       Enables parallel processing during build. Can dramatically speed up build process.
  -g | --generate  Only generates the build files. Does nothing if build files are already up to date.
  --vsc            Enables Visual Studio Code support by generating the compile_commands.json file.
  -t | --test      Runs CTest after build finishes. Note: cannot be used with -g or --generate.


================================================================================

----- Test Command -----
Usage: buildtool.sh test [configuration]

Runs CTest on a given built configuration. If a test fails, related logs during
test are also printed. Defaults to a DEBUG configuration if no configuration is
given.


================================================================================

----- Run Command -----
Usage: buildtool.sh run [configuration] -- [GBoy Args]

Runs a built configuration of GBoy. Arguments can be passed directly to GBoy
following a "--" token. Defaults to a DEBUG configuration if no configuration
is given.
```