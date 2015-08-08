set(MODELCOMPONENTS_FILES
    Body_Simbody.cpp
    Body_Simbody.h
    Dof_Simbody.cpp
    Dof_Simbody.h
    Joint_Simbody.cpp
    Joint_Simbody.h
    Model_Simbody.cpp
    Model_Simbody.h
    Muscle_Simbody.cpp
    Muscle_Simbody.h
    Simulation_Simbody.cpp
    Simulation_Simbody.h
    )
set(SYSTEM_FILES
    sim_simbody.cpp
    sim_simbody.h
    stdafx.cpp
    stdafx.h
    )
set(TOOLS_FILES
    tools.cpp
    tools.h
    )
add_library(scone-sim_simbody # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${MODELCOMPONENTS_FILES} ${SYSTEM_FILES} ${TOOLS_FILES}
    )
target_link_libraries(scone-sim_simbody scone-sim
    ${Boost_LIBRARIES} ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(scone-sim_simbody PROPERTIES
    DEFINE_SYMBOL SCONE_SIM_SIMBODY_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL sim_simbody # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
source_group("Model Components" FILES ${MODELCOMPONENTS_FILES})
source_group("System" FILES ${SYSTEM_FILES})
source_group("Tools" FILES ${TOOLS_FILES})