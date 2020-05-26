# License

The R2C code is distributed under the "New/3-clause BSD" license. In short, you are free to use R2C
in your own applications, whether they are free or commercial, open or proprietary, as well as to modify
the R2C code and documentation as you desire, provided that you retain the original copyright notices as
described in the [license](./License.txt).

# Requirements

- [CMake](https://cmake.org/)
- A working compiler
- Clarisse and Clarisse SDK
- Optional: Redshift and Redshift SDK
- Optional: [Doxygen](https://www.doxygen.nl/download.html)

# Content

- `docs`: Documentation folder (will be generated, see Builds instructions)
- `r2c`: Helper library, stands for "your Renderer to Clarisse"
- `module.renderer.base`: Base module needed for integration of renderers into Clarisse.
- `module.redshift`: Example integration of the Redshift renderer into Clarisse, using the 2 previous folders.

More information is available once the documentation has been built. See next section.

# Build

Clone this repository and create a `build` folder next to it:

```sh
$ git clone git@github.com:Isotropix/r2c.git
$ mkdir build
```

Then you can create a `build.sh` file with the following content:

```sh
# go to our build folder
cd build

# configure (using Ninja here, but use whatever generator you prefer)
cmake -G "Ninja"                                    \
    -DCMAKE_INSTALL_PREFIX="<where_to_install>"     \
    -DCLARISSE_INSTALL_DIR="<clarisse_install_dir>" \
    -DCLARISSE_SDK_DIR="<clarisse_sdk_dir>"         \
    -DREDSHIFT_SDK_DIR="<redshift_sdk_dir>"         \
    ../r2c

# build
cmake --build . --config Release --parallel

# optionally install
cmake --build . --config Release --target install
```

Don't forget to replace the paths. By default this will build the `R2C`
library, the Redshift renderer example which uses it, and generate the
documentation using Doxygen (provided everything was correctly installed)

You can disable building the documentation and/or the Redshift example module using
the following variable during configuration:

- `-DBUILD_DOC=OFF`
- `-DBUILD_REDSHIFT=OFF`

You can set the install prefix to the Clarisse install dir, but beware that you must have the
correct rights to write into it (on Windows, the UAC might kick in, on Linux you might need root
access)

Also, the CMake scripts will try to install the needed runtime libraries of Redshift in the
install prefix on Windows. You can disable this using the following variable during configuration:

- `-DINSTALL_REDSHIFT_LIBRARIES=OFF`

# Running

The easiest way is to run the CMake install target directly with the install prefix
being set to your Clarisse installation directory.

If you can't, then still run the CMake install target using some other prefix. Then,
you can run Clarisse using the following script (as usual, replacing the paths where
necessary)

```sh
# so that the libraries will be found. On Windows you'd do something like
# `set PATH=%PATH%;<r2c_install_dir>` And on MacOS it's DYLD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"<r2c_install_dir>"

# run Clarisse
<clarisse_install_dir>/clarisse -module_path "<clarisse_install_dir>/module" "<r2c_install_dir>/module"
```

# Documentation

More information in [docs/index.md](./docs/index.md) and if you built the documentation (which you should have,
if Doxygen was correctly installed on your system) you'll have even more in [docs/doxydocs/html/index.html](./docs/doxydocs/html/index.html)

Copyright (c) 2020 Isotropix SAS. All rights reserved.
