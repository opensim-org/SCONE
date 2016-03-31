set(SIM_MODELCOMPONENTS_FILES
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
set(SIM_SENSORSANDACTUATORS_FILES
    Actuator.cpp
    Actuator.h
    Sensor.cpp
    Sensor.h
    SensorDelayAdapter.cpp
    SensorDelayAdapter.h
    Sensors.cpp
    Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    Controller.cpp
    Controller.h
    Simulation.cpp
    Simulation.h
    State.h
    )
set(SIM_SYSTEM_FILES
    Factories.cpp
    Factories.h
    sim.cpp
    sim.h
    Types.h
    )
set(SIM_TOPOLOGY_FILES
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
    ${SIM_MODELCOMPONENTS_FILES} ${SIM_SENSORSANDACTUATORS_FILES}
    ${SIM_SIMULATIONANDCONTROL_FILES} ${SIM_SYSTEM_FILES} ${SIM_TOPOLOGY_FILES} 
    )
target_link_libraries(scone-sim scone-opt)

set_target_properties(scone-sim PROPERTIES
    DEFINE_SYMBOL SCONE_SIM_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL sim # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
# The '\\' creates a nested directory.
source_group("Model Components" FILES ${SIM_MODELCOMPONENTS_FILES})
source_group("Sensors and Actuators" FILES ${SIM_SENSORSANDACTUATORS_FILES})
source_group("Simulation and Control" FILES ${SIM_SIMULATIONANDCONTROL_FILES})
source_group("System" FILES ${SIM_SYSTEM_FILES})
source_group("Topology" FILES ${SIM_TOPOLOGY_FILES})