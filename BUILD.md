# Building SCONE

SCONE can be build on Windows and Linux.

## Building SCONE on Windows

  * Microsoft Visual Studio 2017 or 2019 (https://visualstudio.microsoft.com/downloads)
  * CMake 3.12 or higher (https://cmake.org).

### Dependencies

SCONE depends on a number of third party libraries. It is possible to download
binary packages for these libraries (recommended), or you can build the
dependencies from source.

#### Option 1: Download pre-build binary packages for Windows:
  
  * **OpenSim 3.3**
    * Download from https://simtk.org/frs/?group_id=1180 (SimTK user account
      required)
  * **OpenSceneGraph 3.4.1**
    * Download from https://simtk.org/frs/?group_id=1180 (SimTK user account
      required)
  * **Qt 5.12** or higher
    * Download from https://www.qt.io/download, select 'Open Source'

#### Option 2: Build dependencies from source:
  
  * **OpenSim 3.3**
    * Checkout **Simbody 3.5.4**
      (https://github.com/simbody/simbody/tree/Simbody-3.5.4)
    * Configure in CMake and generate project files. Be sure to set
      CMAKE_INSTALL_PREFIX to a folder with write access.
    * Open the Visual Studio project and build the INSTALL project for Debug and
      Release.
    * Download **OpenSim 3.3** (https://github.com/tgeijten/opensim3-scone)
    * Configure in CMake and generate project files. Be sure to set
      CMAKE_INSTALL_PREFIX to a folder with write access, and to set
      SIMBODY_HOME to the Simbody installation.
    * Open the Visual Studio project and build the INSTALL project for Debug and
      Release.
  * **OpenSceneGraph 3.4**
    * Checkout the repository
      (https://github.com/openscenegraph/OpenSceneGraph/tree/OpenSceneGraph-3.4)
    * See
      http://www.openscenegraph.org/index.php/documentation/10-getting-started
      for details
  * **Qt 5.12** or higher
    * Download from https://www.qt.io/download
    * See from https://www.qt.io for details

#### Additional dependencies

The following libraries are included in the SCONE repository and are build
automatically:

  * **QCustomPlot** (https://www.qcustomplot.com)
  * **TCLAP** (http://tclap.sourceforge.net/)
  * **xo** (https://github.com/tgeijten/xo)
  * **spot** (https://github.com/tgeijten/spot)
  * **osg-vis** (https://github.com/tgeijten/vis)
  * **qtfx** (https://github.com/tgeijten/qtfx)

### Building SCONE

Once the required dependencies are build, perform the following steps to build
SCONE:

  * Configure the folder in CMake. Be sure to set CMAKE_INSTALL_PREFIX to a
    folder with write access.
  * Set **OPENSIM_INSTALL_DIR** to the path of your OpenSim installation
  * Set **OSG_DIR** to the path of to your OpenSceneGraph installation
  * Set **Qt5Widgets_DIR** to the path of Qt5WidgetsConfig.cmake in your Qt
    installation
  * Generate and open the Visual Studio project files
  * Build the ALL_BUILD project
  * Build the RUN_TESTS project to validate your build

### Running SCONE

You can now run SCONE by setting the sconestudio project as the startup
project. Running in Release mode is recommended. Once SCONE is running, be sure
to check out the tutorials at
https://scone.software/doku.php?id=tutorials:start.

## Building SCONE on Linux (Ubuntu 18.04)

SCONE can be build on Ubuntu 18.04. In case that you cannot build SCONE, try
following the steps defined in [travis-ci](.travis.yml).

### Dependencies

Install the following dependencies:

```shell
sudo apt install gcc-8 g++-8 freeglut3-dev libxi-dev libxmu-dev cmake
sudo apt install liblapack-dev libqt5widgets5 libqt5opengl5-dev qt5-default
```

SCONE depends on OpenSim v3.3 and OpenSceneGraph v3.2. One can either build them
from source or fetch the binaries for Ubuntu 18.04.

For OpenSceneGraph execute the following commands in the terminal:

```shell
wget https://sourceforge.net/projects/dependencies/files/OpenSceneGraph/OpenSceneGraph-3.4-ubuntu-18.04.tar.xz
tar -xf OpenSceneGraph-3.4-ubuntu-18.04.tar.xz
export OSG_DIR=$(pwd)/OpenSceneGraph-3.4
export LD_LIBRARY_PATH=$OSG_DIR/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$OSG_DIR/lib64/osgPlugins-3.4.2:$LD_LIBRARY_PATH
export PATH=$OSG_DIR/bin:$PATH
```

The export commands can be adapted and included into .bashrc so that they can
have a permanent effect in a terminal.

For OpenSim v3.3 execute the following commands in the terminal:

```shell
wget https://sourceforge.net/projects/dependencies/files/opensim-core/opensim-core-3.3-ubuntu-18.04.tar.xz
tar -xf opensim-core-3.3-ubuntu-18.04.tar.xz
export OPENSIM_HOME=$(pwd)/opensim-core
export PATH=$OPENSIM_HOME/bin:$PATH
export LD_LIBRARY_PATH=$OPENSIM_HOME/lib:$LD_LIBRARY_PATH
```

SCONE uses C++17 standard, therefore it must be compiled with the recent
versions of gcc or clang (gcc/g++ assumed here):

```shell
export CC="gcc-8
export CXX="g++-8"
```

Finally, to build SCONE execute the following commands:

```shell
git clone https://github.com/opensim-org/SCONE.git
git submodule update --init
mkdir build
cd build
cmake ../ \
      -DCMAKE_BUILD_TYPE:STRING=Release \
      -DCMAKE_INSTALL_PREFIX:PATH=../install \
      -DCMAKE_PREFIX_PATH=$OSG_DIR/lib64 \
      -DOPENSIM_INSTALL_DIR:PATH=$OPENSIM_HOME \
      -DOPENSIM_INCLUDE_DIR:PATH=$OPENSIM_HOME/sdk/include \
      -DOSG_DIR:PATH=$OSG_DIR \
      -DOSG_INCLUDE_DIR:PATH=$OSG_DIR/include \
make -j8
```

After building SCONE, a folder is created (`/SCONE/bin/build`) containing the
`sconestudio` application. Before running make sure to execute the
`.updateversion.sh` script located in `/SCONE/tools`. You can also manually copy the
`/SCONE/senarios` folder into `~/SCONE` folder.
