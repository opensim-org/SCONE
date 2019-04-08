![alt text](https://github.com/opensim-org/SCONE/blob/master/resources/ui/scone_logo_notext.png "SCONE")
# Welcome to SCONE!
SCONE is open source software for predictive simulation of biological motion. See http://scone.software for details.

## Running SCONE on Windows (64 bit)
The fastest way to get started using SCONE is to install the software on Windows (64 bit) and run the tutorials. See http://scone.software for details.

## Building SCONE
SCONE can also be build from source using CMake version 3.12 or higher (see https://cmake.org), using a C++17 conformant compiler. The following compilers / environments have been tested to work with SCONE:
  * Microsoft Visual Studio 2017 on Windows 10 (64 bit)

### Dependencies
SCONE Depends on the following third party libraries:
  * **Qt 5.12** or higher (https://www.qt.io)
  * **OpenSceneGraph 3.4** (https://github.com/openscenegraph/OpenSceneGraph/tree/OpenSceneGraph-3.4)
  * **OpenSim**, the following versions are supported:
    * **OpenSim 3.3** (https://github.com/opensim-org/opensim-core/tree/OpenSim32)
    * **OpenSim 4** (https://github.com/opensim-org/opensim-core/tree/master)

The following libraries are added to the source tree automatically as submodules:
  * **xo**
  * **spot**
  * **osg-vis**
  * **qtfx**

### CMake settings
After all third party libs are installed and compiled, SCONE can be build by running CMake. The following CMake variables need special attention:
  * **OSG_DIR**: path to your OpenSceneGraph installation
  * **Qt5Widgets_DIR**: path to the Qt5WidgetsConfig.cmake in your Qt installation
  * **OPENSIM_INSTALL_DIR**: path to your OpenSim installation

### Testing
After compilation succeeds, be sure to run the included CMake tests to ensure everything is working correctly.

## License
SCONE is licensed under the GNU Public License 3.0
