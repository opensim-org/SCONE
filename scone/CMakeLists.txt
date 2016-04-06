include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CORE_BASE_FILES
	core/Exception.h
	core/Factory.h
	core/HasData.h
	core/HasName.h
	core/HasSignature.cpp
	core/HasSignature.h
	core/Result.h
)
set(CORE_MATH_FILES
	core/Angle.h
	core/Delayer.h
	core/math.h
	core/Range.h
	core/Statistic.h
	core/TimedValue.h
)
set(CORE_PROPNODE_FILES
	core/InitFromPropNode.h
	core/PropNode.cpp
	core/PropNode.h
)
set(CORE_SYSTEM_FILES
	core/core.h
)
set(CORE_TOOLS_FILES
	core/Log.cpp
	core/Log.h
	core/ResourceCache.h
	core/Storage.h
	core/StorageIo.h
	core/StringMap.h
	core/system.cpp
	core/system.h
	core/tools.cpp
	core/tools.h
)
if(WIN32)
    list(APPEND CORE_TOOLS_FILES core/Profiler.cpp core/Profiler.h)
endif()

set(CS_CONTROLLERS_FEEDFORWARD_FILES
	cs/FeedForwardController.cpp
	cs/FeedForwardController.h
	)
set(CS_CONTROLLERS_MRC_FILES
	cs/MetaReflexParams.cpp
	cs/MetaReflexParams.h
	cs/MetaReflexController.cpp
	cs/MetaReflexController.h
    cs/MetaReflexDof.cpp
    cs/MetaReflexDof.h
    cs/MetaReflexMuscle.cpp
    cs/MetaReflexMuscle.h
    cs/MetaReflexVirtualMuscle.cpp
    cs/MetaReflexVirtualMuscle.h
	)
set(CS_CONTROLLERS_REFLEX_FILES
	cs/ConditionalMuscleReflex.cpp
	cs/ConditionalMuscleReflex.h
	cs/DofReflex.cpp
	cs/DofReflex.h
	cs/MuscleReflex.cpp
	cs/MuscleReflex.h
	cs/Reflex.cpp
	cs/Reflex.h
	cs/ReflexController.cpp
	cs/ReflexController.h
	)
set(CS_CONTROLLERS_STATE_FILES
	cs/GaitStateController.cpp
	cs/GaitStateController.h
	cs/StateController.cpp
	cs/StateController.h
	cs/TimeStateController.cpp
	cs/TimeStateController.h
	)
set(CS_FUNCTIONS_FILES
	cs/Function.h
	cs/PieceWiseConstantFunction.cpp
	cs/PieceWiseConstantFunction.h
	cs/PieceWiseFunction.h
	cs/PieceWiseLinearFunction.cpp
	cs/PieceWiseLinearFunction.h
	cs/Polynomial.cpp
	cs/Polynomial.h
	)
set(CS_MEASURES_FILES
	cs/CompositeMeasure.cpp
	cs/CompositeMeasure.h
	cs/DofLimitMeasure.cpp
	cs/DofLimitMeasure.h
	cs/EffortMeasure.cpp
	cs/EffortMeasure.h
	cs/GaitCycleMeasure.cpp
	cs/GaitCycleMeasure.h
	cs/GaitMeasure.cpp
	cs/GaitMeasure.h
	cs/HeightMeasure.cpp
	cs/HeightMeasure.h
	cs/JumpMeasure.cpp
	cs/JumpMeasure.h
	cs/Measure.cpp
	cs/Measure.h
	)
set(CS_SYSTEM_FILES
	cs/cs.cpp
	cs/cs.h
	cs/Factories.cpp
	cs/Factories.h
	cs/version.h
	)
set(CS_TOOLS_FILES
	cs/SimulationObjective.cpp
	cs/SimulationObjective.h
	cs/tools.h
	)

set(OPT_API_FILES
	opt/CmaOptimizer.cpp
	opt/CmaOptimizer.h
	opt/Objective.cpp
	opt/Objective.h
	opt/Optimizer.cpp
	opt/Optimizer.h
	opt/ParamInfo.cpp
	opt/ParamInfo.h
	opt/ParamSet.cpp
	opt/ParamSet.h
    )
set(OPT_SYSTEM_FILES
    opt/Factories.cpp
    opt/Factories.h
    opt/opt.cpp
    opt/opt.h
	opt/SharkHelperClasses.h
    )
	
set(SIM_MODELCOMPONENTS_FILES
    sim/Body.cpp
    sim/Body.h
    sim/Dof.cpp
    sim/Dof.h
    sim/Joint.cpp
    sim/Joint.h
    sim/Model.cpp
    sim/Model.h
    sim/Muscle.cpp
    sim/Muscle.h
    )
set(SIM_SENSORSANDACTUATORS_FILES
    sim/Actuator.cpp
    sim/Actuator.h
    sim/Sensor.cpp
    sim/Sensor.h
    sim/SensorDelayAdapter.cpp
    sim/SensorDelayAdapter.h
    sim/Sensors.cpp
    sim/Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    sim/Controller.cpp
    sim/Controller.h
    sim/Simulation.cpp
    sim/Simulation.h
    sim/State.h
    )
set(SIM_SYSTEM_FILES
    sim/Factories.cpp
    sim/Factories.h
    sim/sim.cpp
    sim/sim.h
    sim/Types.h
    )
set(SIM_TOPOLOGY_FILES
    sim/Area.cpp
    sim/Area.h
    sim/Leg.cpp
    sim/Leg.h
    sim/Link.cpp
    sim/Link.h
    )
	
set(SIMBODY_MODELCOMPONENTS_FILES
    sim/simbody/Body_Simbody.cpp
    sim/simbody/Body_Simbody.h
    sim/simbody/Dof_Simbody.cpp
    sim/simbody/Dof_Simbody.h
    sim/simbody/Joint_Simbody.cpp
    sim/simbody/Joint_Simbody.h
    sim/simbody/Model_Simbody.cpp
    sim/simbody/Model_Simbody.h
    sim/simbody/Muscle_Simbody.cpp
    sim/simbody/Muscle_Simbody.h
    sim/simbody/Simulation_Simbody.cpp
    sim/simbody/Simulation_Simbody.h
    )
set(SIMBODY_SYSTEM_FILES
    sim/simbody/sim_simbody.cpp
    sim/simbody/sim_simbody.h
    )
set(SIMBODY_TOOLS_FILES
    sim/simbody/tools.cpp
    sim/simbody/tools.h
    )
	
set (SCONELIB_FILES
	${CORE_BASE_FILES}
	${CORE_MATH_FILES}
	${CORE_PROPNODE_FILES}
	${CORE_SYSTEM_FILES}
	${CORE_TOOLS_FILES}
	
	${CS_CONTROLLERS_FEEDFORWARD_FILES}
	${CS_CONTROLLERS_MRC_FILES}
	${CS_CONTROLLERS_REFLEX_FILES}
	${CS_CONTROLLERS_STATE_FILES}
    ${CS_FUNCTIONS_FILES}
	${CS_MEASURES_FILES}
	${CS_SYSTEM_FILES} 
    ${CS_TOOLS_FILES}
	
	${OPT_API_FILES}
	${OPT_SYSTEM_FILES}
	
	${SIM_MODELCOMPONENTS_FILES}
	${SIM_SENSORSANDACTUATORS_FILES}
    ${SIM_SIMULATIONANDCONTROL_FILES}
	${SIM_SYSTEM_FILES}
	${SIM_TOPOLOGY_FILES} 
	
	${SIMBODY_MODELCOMPONENTS_FILES}
	${SIMBODY_SYSTEM_FILES}
	${SIMBODY_TOOLS_FILES}
	)

# add sources to single lib
add_library(sconelib SHARED ${SCONELIB_FILES})
	
target_link_libraries(sconelib ${Boost_LIBRARIES} ${Shark_LIBRARIES} ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(sconelib PROPERTIES
    PROJECT_LABEL sconelib
    #FOLDER "sconelib"
    )

target_compile_definitions( sconelib PRIVATE CORE_EXPORTS CS_EXPORTS OPT_EXPORTS SCONE_SIM_EXPORTS SCONE_SIM_SIMBODY_EXPORTS )

# To create folders in Visual Studio.
source_group("core\\Base" FILES ${CORE_BASE_FILES})
source_group("core\\Math" FILES ${CORE_MATH_FILES})
source_group("core\\PropNode" FILES ${CORE_PROPNODE_FILES})
source_group("core\\System" FILES ${CORE_SYSTEM_FILES})
source_group("core\\Tools" FILES ${CORE_TOOLS_FILES})

source_group("cs\\Controllers\\Feed Forward Control" FILES ${CS_CONTROLLERS_FEEDFORWARD_FILES})
source_group("cs\\Controllers\\Meta Reflex Control" FILES ${CS_CONTROLLERS_MRC_FILES})
source_group("cs\\Controllers\\Reflex Control" FILES ${CS_CONTROLLERS_REFLEX_FILES})
source_group("cs\\Controllers\\State Control" FILES ${CS_CONTROLLERS_STATE_FILES})
source_group("cs\\Functions" FILES ${CS_FUNCTIONS_FILES})
source_group("cs\\Measures" FILES ${CS_MEASURES_FILES})
source_group("cs\\System" FILES ${CS_SYSTEM_FILES})
source_group("cs\\Tools" FILES ${CS_TOOLS_FILES})

source_group("opt\\API" FILES ${OPT_API_FILES})
source_group("opt\\System" FILES ${OPT_SYSTEM_FILES})

source_group("sim\\Model Components" FILES ${SIM_MODELCOMPONENTS_FILES})
source_group("sim\\Sensors and Actuators" FILES ${SIM_SENSORSANDACTUATORS_FILES})
source_group("sim\\Simulation and Control" FILES ${SIM_SIMULATIONANDCONTROL_FILES})
source_group("sim\\System" FILES ${SIM_SYSTEM_FILES})
source_group("sim\\Topology" FILES ${SIM_TOPOLOGY_FILES})

source_group("sim/simbody\\Model Components" FILES ${SIMBODY_MODELCOMPONENTS_FILES})
source_group("sim/simbody\\System" FILES ${SIMBODY_SYSTEM_FILES})
source_group("sim/simbody\\Tools" FILES ${SIMBODY_TOOLS_FILES})
