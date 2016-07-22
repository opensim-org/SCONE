include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CORE_BASE_FILES
	scone/core/Exception.h
	scone/core/Factory.h
	scone/core/HasData.h
	scone/core/HasName.h
	scone/core/HasSignature.cpp
	scone/core/HasSignature.h
	scone/core/Result.h
)
set(CORE_MATH_FILES
	scone/core/Angle.h
	scone/core/Vec3.h
	scone/core/Quat.h
	scone/core/Delayer.h
	scone/core/math.h
	scone/core/Range.h
	scone/core/Statistic.h
	scone/core/TimedValue.h
)
set(CORE_SYSTEM_FILES
	scone/core/core.h
	scone/core/String.h
)
set(CORE_TOOLS_FILES
	scone/core/Log.cpp
	scone/core/Log.h
	scone/core/Profiler.cpp
    scone/core/Profiler.h
    scone/core/ResourceCache.h
	scone/core/Storage.h
	scone/core/StorageIo.h
	scone/core/StorageIo.cpp
	scone/core/StringMap.h
	scone/core/system_tools.cpp
	scone/core/system_tools.h
	scone/core/string_tools.cpp
	scone/core/string_tools.h
	scone/core/memory_tools.h
	scone/core/propnode_tools.h
	scone/core/propnode_tools.cpp
	scone/core/PropNode.cpp
	scone/core/PropNode.h
)
set(CS_CONTROLLERS_FEEDFORWARD_FILES
	scone/cs/FeedForwardController.cpp
	scone/cs/FeedForwardController.h
	)
set(CS_CONTROLLERS_MRC_FILES
	scone/cs/MetaReflexParams.cpp
	scone/cs/MetaReflexParams.h
	scone/cs/MetaReflexController.cpp
	scone/cs/MetaReflexController.h
    scone/cs/MetaReflexDof.cpp
    scone/cs/MetaReflexDof.h
    scone/cs/MetaReflexMuscle.cpp
    scone/cs/MetaReflexMuscle.h
    scone/cs/MetaReflexVirtualMuscle.cpp
    scone/cs/MetaReflexVirtualMuscle.h
	)
set(CS_CONTROLLERS_REFLEX_FILES
	scone/cs/ConditionalMuscleReflex.cpp
	scone/cs/ConditionalMuscleReflex.h
	scone/cs/DofReflex.cpp
	scone/cs/DofReflex.h
	scone/cs/MuscleReflex.cpp
	scone/cs/MuscleReflex.h
	scone/cs/Reflex.cpp
	scone/cs/Reflex.h
	scone/cs/ReflexController.cpp
	scone/cs/ReflexController.h
	)
set(CS_CONTROLLERS_STATE_FILES
	scone/cs/GaitStateController.cpp
	scone/cs/GaitStateController.h
	scone/cs/StateController.cpp
	scone/cs/StateController.h
	scone/cs/TimeStateController.cpp
	scone/cs/TimeStateController.h
	)
set(CS_FUNCTIONS_FILES
	scone/cs/Function.h
	scone/cs/PieceWiseConstantFunction.cpp
	scone/cs/PieceWiseConstantFunction.h
	scone/cs/PieceWiseFunction.h
	scone/cs/PieceWiseLinearFunction.cpp
	scone/cs/PieceWiseLinearFunction.h
	scone/cs/Polynomial.cpp
	scone/cs/Polynomial.h
	)
set(CS_MEASURES_FILES
	scone/cs/CompositeMeasure.cpp
	scone/cs/CompositeMeasure.h
	scone/cs/DofLimitMeasure.cpp
	scone/cs/DofLimitMeasure.h
	scone/cs/EffortMeasure.cpp
	scone/cs/EffortMeasure.h
	scone/cs/GaitCycleMeasure.cpp
	scone/cs/GaitCycleMeasure.h
	scone/cs/GaitMeasure.cpp
	scone/cs/GaitMeasure.h
	scone/cs/HeightMeasure.cpp
	scone/cs/HeightMeasure.h
	scone/cs/JumpMeasure.cpp
	scone/cs/JumpMeasure.h
	scone/cs/JointLoadMeasure.cpp
	scone/cs/JointLoadMeasure.h
	scone/cs/RangePenalty.h
	scone/cs/Measure.cpp
	scone/cs/Measure.h
	)
set(CS_SYSTEM_FILES
    scone/cs/cs_fwd.h
	scone/cs/Factories.cpp
	scone/cs/Factories.h
	scone/cs/version.h
	)
set(CS_TOOLS_FILES
	scone/cs/SimulationObjective.cpp
	scone/cs/SimulationObjective.h
	scone/cs/cs_tools.cpp
	scone/cs/cs_tools.h
	)

set(OPT_API_FILES
	scone/opt/CmaOptimizer.cpp
	scone/opt/CmaOptimizer.h
	scone/opt/Objective.cpp
	scone/opt/Objective.h
	scone/opt/Optimizer.cpp
	scone/opt/Optimizer.h
	scone/opt/ParamInfo.cpp
	scone/opt/ParamInfo.h
	scone/opt/ParamSet.cpp
	scone/opt/ParamSet.h
    scone/opt/opt_tools.cpp
    scone/opt/opt_tools.h
    scone/opt/opt_fwd.h
    )
set(OPT_SYSTEM_FILES
    scone/opt/Factories.cpp
    scone/opt/Factories.h
	scone/opt/SharkHelperClasses.h
    )
	
set(SIM_MODELCOMPONENTS_FILES
    scone/sim/Body.cpp
    scone/sim/Body.h
    scone/sim/Dof.cpp
    scone/sim/Dof.h
    scone/sim/Joint.cpp
    scone/sim/Joint.h
    scone/sim/Model.cpp
    scone/sim/Model.h
    scone/sim/Muscle.cpp
    scone/sim/Muscle.h
    )
set(SIM_SENSORSANDACTUATORS_FILES
    scone/sim/Actuator.cpp
    scone/sim/Actuator.h
    scone/sim/Sensor.cpp
    scone/sim/Sensor.h
    scone/sim/SensorDelayAdapter.cpp
    scone/sim/SensorDelayAdapter.h
    scone/sim/Sensors.cpp
    scone/sim/Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    scone/sim/Controller.cpp
    scone/sim/Controller.h
    scone/sim/Simulation.cpp
    scone/sim/Simulation.h
    )
set(SIM_SYSTEM_FILES
    scone/sim/Factories.cpp
    scone/sim/Factories.h
    scone/sim/sim_tools.cpp
    scone/sim/sim_tools.h
    scone/sim/sim_fwd.h
    scone/sim/Types.h
    )
set(SIM_TOPOLOGY_FILES
    scone/sim/Area.cpp
    scone/sim/Area.h
    scone/sim/Leg.cpp
    scone/sim/Leg.h
    scone/sim/Link.cpp
    scone/sim/Link.h
	scone/sim/Side.h
    )
	
set(SIMBODY_MODELCOMPONENTS_FILES
    scone/sim/simbody/Body_Simbody.cpp
    scone/sim/simbody/Body_Simbody.h
    scone/sim/simbody/Dof_Simbody.cpp
    scone/sim/simbody/Dof_Simbody.h
    scone/sim/simbody/Joint_Simbody.cpp
    scone/sim/simbody/Joint_Simbody.h
    scone/sim/simbody/Model_Simbody.cpp
    scone/sim/simbody/Model_Simbody.h
    scone/sim/simbody/Muscle_Simbody.cpp
    scone/sim/simbody/Muscle_Simbody.h
    scone/sim/simbody/Simulation_Simbody.cpp
    scone/sim/simbody/Simulation_Simbody.h
    )
set(SIMBODY_SYSTEM_FILES
    scone/sim/simbody/sim_simbody.cpp
    scone/sim/simbody/sim_simbody.h
    )
set(SIMBODY_TOOLS_FILES
    scone/sim/simbody/simbody_tools.h
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
	
# specify precompiled headers for MSVC
if(MSVC)
	set(STD_AFX_CPP "stdafx.cpp")
	set(STD_AFX_H "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h")
	set(PCH_FILE "${CMAKE_CURRENT_BINARY_DIR}/sconelib.pch")
	set_source_files_properties( ${STD_AFX_CPP} PROPERTIES COMPILE_FLAGS "/Ycstdafx.h /Fp\"${PCH_FILE}\"" OBJECT_OUTPUTS "${PCH_FILE}" )
	set_source_files_properties( ${SCONELIB_FILES} PROPERTIES COMPILE_FLAGS "/Yu\"${STD_AFX_H}\" /FI\"${STD_AFX_H}\" /Fp\"${PCH_FILE}\"" OBJECT_DEPENDS "${PCH_FILE}" )
	set(STD_AFX_FILES ${STD_AFX_H} ${STD_AFX_CPP} )
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Zm256" )
endif(MSVC)

# add sources to single lib
add_library(sconelib SHARED ${SCONELIB_FILES} ${STD_AFX_FILES} )
	
target_link_libraries(sconelib flut shark ${Boost_LIBRARIES} ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(sconelib PROPERTIES
    PROJECT_LABEL sconelib
    #FOLDER "sconelib"
    )

target_compile_definitions( sconelib PRIVATE SCONE_EXPORTS )

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

source_group("" FILES ${STD_AFX_FILES})
