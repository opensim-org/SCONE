set(MODELCOMPONENTS_FILES
    Body.cpp
    Body.h
    Dof.cpp
    Dof.h
    Joint.cpp
    Joint.h
    Model.cpp
    Model.h
    Muscle.cpp
    Muscle.h
    )
set(SENSORSANDACTUATORS_FILES
    Actuator.cpp
    Actuator.h
    Sensor.cpp
    Sensor.h
    SensorDelayAdapter.cpp
    SensorDelayAdapter.h
    Sensors.cpp
    Sensors.h
    )
set(SIMULATIONANDCONTROL_FILES
    Controller.cpp
    Controller.h
    Simulation.cpp
    Simulation.h
    State.h
    )
set(SYSTEM_FILES
    Factories.cpp
    Factories.h
    sim.cpp
    sim.h
    Types.h
    )
set(TOPOLOGY_FILES
    Area.cpp
    Area.h
    Leg.cpp
    Leg.h
    Link.cpp
    Link.h
    )
add_library(scone-sim # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${MODELCOMPONENTS_FILES} ${SENSORSANDACTUATORS_FILES}
    ${SIMULATIONANDCONTROL_FILES} ${SYSTEM_FILES} ${TOPOLOGY_FILES} 
    )
target_link_libraries(scone-sim scone-opt)

set_target_properties(scone-sim PROPERTIES
    DEFINE_SYMBOL SCONE_SIM_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL sim # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
# The '\\' creates a nested directory.
source_group("Model Components" FILES ${MODELCOMPONENTS_FILES})
source_group("Sensors and Actuators" FILES ${SENSORSANDACTUATORS_FILES})
source_group("Simulation and Control" FILES ${SIMULATIONANDCONTROL_FILES})
source_group("System" FILES ${SYSTEM_FILES})
source_group("Topology" FILES ${TOPOLOGY_FILES})