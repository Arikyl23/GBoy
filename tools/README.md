# Tools Directory

This directory is responsible for housing simple tool scripts. These are generally small programs that help automate some tasks such as the build process.

## List Of Tools

Here is a list of the current tools that can be found in this directory. You can find more details about each below in its respective section:

- [Buildtool](#buildtool)

### Buildtool

**File:** buildtool.sh
**Date Created:** 07/01/2026
**Last Modified:** 07/01/2026

This is the primary build script used to generating, cleaning, and building the project. It is safe to invoke anywhere in the filesystem as it builds the project relative to itself.

#### Usage

```
Usage: buildtool.sh {generate|build|clean} [options]
  clean: Deletes the entire /build folder
  generate [-vsc_integration] [-clean] [--configuration RELEASE|DEBUG]: Generate build files
    -vsc_integration: Generates a compile_commands.json file for use integrating with VS Code
    -clean: Deletes the /build/<configuration> folder before generating a new one
    --configuration [RELEASE|DEBUG]: Specifies the configuration to generate for. Defaults to DEBUG
  build [RELEASE|DEBUG]: Compile the project from the build files
```