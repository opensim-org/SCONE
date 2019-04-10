![alt text](https://github.com/opensim-org/SCONE/blob/master/resources/ui/scone_logo_notext.png "SCONE")
# Welcome to SCONE!
SCONE is open source software for predictive simulation of biological motion. More information on SCONE, including tutorials, examples, community guidelines and documentation can be found on https://scone.software.

## Installing SCONE (Windows 64 bit)
The fastest way to get started using SCONE is to install the software on Windows 64 bit and run the tutorials. See https://scone.software/doku.php?id=install for details.

## Building SCONE
SCONE can also be build from source, using a C++17 conformant compiler. Currently, the following compilers / environments have been tested to work with SCONE:
  * Microsoft Visual Studio 2017 on Windows 10 64 bit

### Building SCONE with Microsoft Visual Studio (Windows 64 bit)
#### Prerequisites
Make sure you have the following software installed:
  * Microsoft Visual Studio 2017 (latest version)
  * CMake version 3.12 or higher (https://cmake.org)

#### Dependencies
SCONE Depends on a number of third party libraries. 

, which need to be downloaded and build separately:
  * **OpenSim 3.3**
    * Checkout **Simbody 3.5.4** (https://github.com/simbody/simbody/tree/Simbody-3.5.4)
    * Configure the folder in CMake. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access.
    * Generate and open the Visual Studio project files
    * Build the INSTALL project for Debug and Release
    * Download **OpenSim 3.3** (https://github.com/tgeijten/opensim3-scone)
    * Configure the folder in CMake. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access.
    * Set **SIMBODY_HOME** to a the installation folder for Simbody.
    * Generate and open the Visual Studio project files
    * Build the INSTALL project for Debug and Release
  * **Qt 5.12** or higher
    * Download and install **Qt 5.12** or higher (https://www.qt.io)
  * **OpenSceneGraph 3.4**
    * Checkout the repository (https://github.com/openscenegraph/OpenSceneGraph/tree/OpenSceneGraph-3.4)
    * Build and install according to 

The following libraries are included in the repository and are build automatically:
  * **QCustomPlot** (https://www.qcustomplot.com)
  * **TCLAP** (http://tclap.sourceforge.net/)
  * **xo** (https://github.com/tgeijten/xo)
  * **spot** (https://github.com/tgeijten/spot)
  * **osg-vis** (https://github.com/tgeijten/vis)
  * **qtfx** (https://github.com/tgeijten/qtfx)

### Building SCONE
Once the required dependencies are build, perform the following steps to build SCONE:
  * Configure the folder in CMake. Be sure to set CMAKE_INSTALL_PREFIX to a folder with write access.
  * Set **OSG_DIR** to the path of to your OpenSceneGraph installation
  * Set **Qt5Widgets_DIR** to the path of Qt5WidgetsConfig.cmake in your Qt installation
  * Set **OPENSIM_INSTALL_DIR** to the path of your OpenSim installation
  * Generate and open the Visual Studio project files
  * Build the ALL_BUILD project
  * Build the RUN_TESTS project to validate your build

### Running SCONE
You can now run SCONE by setting the sconestudio project as the startup project. Running in Release mode is recommended. Once SCONE is running, be sure to check out the tutorials at https://scone.software/doku.php?id=tutorials:start.

## License
SCONE is licensed under the GNU Public License 3.0
