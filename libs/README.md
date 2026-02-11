# Internal Libraries

This directory is responsible for housing all internal library source. These are generally libraries we created that GBoy depends on. Simply put, if we wrote it, it goes here.

## List of Internal Libraries

Here is a list of the current libraries were create for use within GBoy. You can find more details for each below in its respective section:

- [String](#string)

### String

**Directory:** string <br>
**Author:** [Arikyl23](https://github.com/Arikyl23) <br>
**Version:** Unavaliable (No version history) <br>
**Date Created:** 2026/02/10 <br>
**Date Updated:** 2026/04/25 <br>

String is a universal string library for C. It is designed to dynamically handle strings behind the scenes providing an API similar to that found in higher level languages. It supports both stack and heap strings as well as temporary view references. Note, this is currently implemented using SDL3 as a backend to allow seamless integration into the current project.

As this library becomes more feature complete, it will most likely be converted into a true stand alone library. Recieving its own git repository being transfered to the vendored section.