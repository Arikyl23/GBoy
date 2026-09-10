# Assets Directory

This directory holds asset files that the application needs to access at runtime. Making use of CMake Resource Compiler (CMRC), any files in this directory that are specified to CMRC will automatically be compiled into static binary data and bundled into the application.

The major benefit to using CMRC is that it provides a virtual filesystem local to this directory allowing all assest file references to be relative to it. This is perfect for internal resources used by the application that are not expected to be supplied by the user (fonts, images, etc.).

All files in this directory are not immediately accessable inside the application. To ensure new assets are included in a build, add them to the list at the end of the `cmrc_add_resource_library()` call inside `CMakeLists.txt`.