# Vendored Libraries

This directory is responsible for housing all external library source. If we didn't write it ourselves, it should go here.

## List Of Vendored Libraries

Here is a list of the current libraries we are vendoring. You can find more details for each below in its respective section:

- [log.c](#logc)
- [Simple DirectMedia Layer (SDL)](#simple-directmedia-layer-sdl)

### Log.c

**Directory:** log.c-master <br>
**Author:** [rxi](https://github.com/rxi) <br>
**Version:** Unavaliable (No version history) <br>
**Date Pulled:** 2026/01/07 <br>
**License:** [MIT license](https://github.com/rxi/log.c/blob/master/LICENSE) <br>
**Git Repo:** https://github.com/rxi/log.c <br>


Log.c is a simple and lightweight logging library written in C99.

Note that some changes to the source have been made. In particular:

- Truncated the file trace to be relative to the projects root directory. This is to make reading the logs a bit easier while removing most of the bloat.
- Added modularization to the logs allowing each file to define it's own logging level. Multiple files can claim to be in the same module by using a common CONFIG logging level.

### Simple DirectMedia Layer (SDL)

**Directory:** SDL-release-3.4.0 <br>
**Author:** [libsdl-org](https://github.com/libsdl-org) <br>
**Version:** Release 3.4.0 <br>
**Date Pulled:** 2026/01/06 <br>
**License:** [zlib license](https://github.com/libsdl-org/SDL/blob/main/LICENSE.txt) <br>
**Git Repo:** https://github.com/libsdl-org/SDL <br>

Simple DirectMedia Layer (SDL) is a cross-platform library designed to make it easy to write multi-media software, such as games and emulators.

### SDL TTF

**Directory:** SDL_ttf-release-3.2.0 <br>
**Author:** [libsdl-org](https://github.com/libsdl-org) <br>
**Version:** Release 3.2.0 <br>
**Date Pulled:** 2026/05/04 <br>
**License:** [zlib license](https://github.com/libsdl-org/SDL_ttf/blob/release-3.2.0/LICENSE.txt)
**Git Repo:** https://github.com/libsdl-org/SDL_ttf/tree/release-3.2.0 <br>

Extension library for Simple DirectMedia Layer (SDL) that provides rendering capabilities for True Type Fonts (.ttf).