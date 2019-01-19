# First, install cmake and qt5 via Homebrew.
# brew install cmake qt5

# Simbody
mkdir ../simbody
cd ../simbody
git clone https://github.com/simbody/simbody.git
cd simbody
git checkout Simbody-3.6.1
mkdir ../build
cd ../build
cmake ../simbody \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DCMAKE_RELEASE_POSTFIX="OpenSim4.0" \
    -DBUILD_TESTING=off \
    -DBUILD_EXAMPLES=off
make --jobs 4 install
cd ../../scone

# OpenSim
mkdir ../opensim-core
cd ../opensim-core
git clone https://github.com/opensim-org/opensim-core.git
cd opensim-core
git checkout 4.0
mkdir ../build
cd ../build
cmake ../opensim-core \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DCMAKE_RELEASE_POSTFIX="4.0" \
    -DSIMBODY_HOME=../../simbody/install \
    -DBUILD_API_ONLY=on
make --jobs 4 install
cd ../../scone

# OpenSceneGraph
mkdir ../OpenSceneGraph
cd ../OpenSceneGraph
git clone https://github.com/openscenegraph/OpenSceneGraph.git
cd OpenSceneGraph
git checkout OpenSceneGraph-3.5.4
git apply ../../scone/OpenSceneGraph_cpp11.patch
mkdir ../build
cd ../build
cmake ../OpenSceneGraph \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10
make --jobs 4 install
cd ../../scone

# osgQt
mkdir ../osgQt
cd ../osgQt
git clone https://github.com/openscenegraph/osgQt.git
mkdir build
cd build
cmake ../osgQt \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DCMAKE_PREFIX_PATH=/usr/local/opt/qt # From Homebrew.
make --jobs 4 install
cd ../../scone

# # SCONE
git submodule update --init
mkdir ../build
cd ../build
cmake ../scone \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/../install \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=10.10 \
    -DCMAKE_PREFIX_PATH="/usr/local/opt/qt;$(pwd)/../opensim-core/install" \
    -DOSGQT_INCLUDE_DIR=$(pwd)/../osgQt/install/include \
    -DOSGQT_LIBRARY_RELEASE=$(pwd)/../osgQt/install/lib/libosgQt5.dylib \
    -DOSG_DIR="$(pwd)/../OpenSceneGraph/install" \
    -DSCONE_OPENSIM_3=off \
    -DSCONE_OPENSIM_4=on
make --jobs 4













