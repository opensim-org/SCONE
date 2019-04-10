![alt text](https://github.com/opensim-org/SCONE/blob/master/resources/ui/scone_logo_notext.png "SCONE")
# Welcome to SCONE!
SCONE is open source software for predictive simulation of biological motion. More information on SCONE, including tutorials, examples, community guidelines and documentation can be found on https://scone.software.

## Installing SCONE (Windows 64 bit)
The fastest way to get started using SCONE is to install the software on Windows 64 bit and run the tutorials. See https://scone.software/doku.php?id=install for details.

## Building SCONE with Microsoft Visual Studio
SCONE can be build from source using Microsoft Visual Studio 2017 (https://visualstudio.microsoft.com/downloads) and CMake 3.12 or higher (https://cmake.org).

### Dependencies
SCONE depends on a number of third party libraries. It is possible to download binary packages for these libraries (recommended), or you can build the dependencies from source.

#### Option 1: Download pre-build binary packages:
  * **OpenSim 3.3**
    * Download from https://simtk.org/frs/?group_id=1180 (SimTK user account required)
  * **OpenSceneGraph 3.4** (https://scone.software/lib/exe/fetch.php?media=dependencies:OpenSim3.3-scone-VC2017-64.7z)
    * Download from https://simtk.org/frs/?group_id=1180 (SimTK user account required)
  * **Qt 5.12** or higher
    * Download from https://www.qt.io/download, select 'Open Source'

#### Option 2: Build dependencies from source:
  * **OpenSim 3.3**
    * Checkout **Simbody 3.5.4** (https://github.com/simbody/simbody/tree/Simbody-3.5.4)
    * Configure in CMake and generate project files. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access.
    * Open the Visual Studio project and build the INSTALL project for Debug and Release.
    * Download **OpenSim 3.3** (https://github.com/tgeijten/opensim3-scone)
    * Configure in CMake and generate project files. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access, and to set SIMBODY_HOME to the Simbody installation.
    * Open the Visual Studio project and build the INSTALL project for Debug and Release.
  * **OpenSceneGraph 3.4**
    * Checkout the repository (https://github.com/openscenegraph/OpenSceneGraph/tree/OpenSceneGraph-3.4)
    * See http://www.openscenegraph.org/index.php/documentation/10-getting-started for details
  * **Qt 5.12** or higher
    * Download from https://www.qt.io/download
    * See from https://www.qt.io for details

#### Additional dependencies
The following libraries are included in the SCONE repository and are build automatically:
  * **QCustomPlot** (https://www.qcustomplot.com)
  * **TCLAP** (http://tclap.sourceforge.net/)
  * **xo** (https://github.com/tgeijten/xo)
  * **spot** (https://github.com/tgeijten/spot)
  * **osg-vis** (https://github.com/tgeijten/vis)
  * **qtfx** (https://github.com/tgeijten/qtfx)

### Building SCONE
Once the required dependencies are build, perform the following steps to build SCONE:
  * Configure the folder in CMake. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access.
  * Set **OPENSIM_INSTALL_DIR** to the path of your OpenSim installation
  * Set **OSG_DIR** to the path of to your OpenSceneGraph installation
  * Set **Qt5Widgets_DIR** to the path of Qt5WidgetsConfig.cmake in your Qt installation
  * Generate and open the Visual Studio project files
  * Build the ALL_BUILD project
  * Build the RUN_TESTS project to validate your build

### Running SCONE
You can now run SCONE by setting the sconestudio project as the startup project. Running in Release mode is recommended. Once SCONE is running, be sure to check out the tutorials at https://scone.software/doku.php?id=tutorials:start.

## License
SCONE is licensed under the GNU Public License 3.0
