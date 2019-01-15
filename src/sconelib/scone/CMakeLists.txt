include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SCONE_CONFIG_FILES
	sconelib_config.h
	)

set(CORE_BASE_FILES
	core/HasData.h
	core/HasName.h
	core/HasExternalResources.h
	core/HasSignature.cpp
	core/HasSignature.h
	)
set(CORE_MATH_FILES
	core/Angle.h
	core/Vec3.h
	core/Quat.h
	core/Delayer.h
	core/math.h
	core/Range.h
	core/Statistic.h
	core/TimedValue.h
	core/Function.h
	core/PieceWiseConstantFunction.cpp
	core/PieceWiseConstantFunction.h
	core/PieceWiseLinearFunction.cpp
	core/PieceWiseLinearFunction.h
	core/Polynomial.h	
	core/Polynomial.cpp
	)
set(CORE_SYSTEM_FILES
	core/Factories.h
    core/Factories.cpp
	core/memory_tools.h
    core/ResourceCache.h
	core/Profiler.cpp
    core/Profiler.h
	core/Exception.h
	core/platform.h
	core/Log.cpp
	core/Log.h
	core/version.cpp
	core/version.h
	core/types.h
	core/system_tools.cpp
	core/system_tools.h
	core/Settings.cpp
	core/Settings.h
	)
set(CORE_STORAGE_FILES
	core/Storage.h
	core/StorageIo.h
	core/StorageIo.cpp
	core/PropNode.h
	core/StringMap.h
	)
set(CORE_TOOL_FILES
	core/Event.cpp
	core/Event.h
	core/string_tools.cpp
	core/string_tools.h
	)
set(CS_CONTROLLERS_FILES
	controllers/Controller.cpp
	controllers/Controller.h
	controllers/FeedForwardController.cpp
	controllers/FeedForwardController.h
	controllers/PerturbationController.cpp
	controllers/PerturbationController.h
	controllers/ExternalBalanceController.cpp
	controllers/ExternalBalanceController.h
	controllers/MirrorController.h
	controllers/NoiseController.h
	controllers/NoiseController.cpp
	)
set(CS_CONTROLLERS_NC_FILES
	controllers/NeuralController.cpp
	controllers/NeuralController.h
	controllers/Neuron.cpp
	controllers/Neuron.h
	controllers/SensorNeuron.cpp
	controllers/SensorNeuron.h
	controllers/InterNeuron.cpp
	controllers/InterNeuron.h
	controllers/MotorNeuron.cpp
	controllers/MotorNeuron.h
	controllers/PatternNeuron.cpp
	controllers/PatternNeuron.h
	controllers/activation_functions.cpp
	controllers/activation_functions.h
	)
set(CS_CONTROLLERS_REFLEX_FILES
	controllers/ConditionalMuscleReflex.cpp
	controllers/ConditionalMuscleReflex.h
	controllers/DofReflex.cpp
	controllers/DofReflex.h
	controllers/BodyPointReflex.cpp
	controllers/BodyPointReflex.h
	controllers/MuscleReflex.cpp
	controllers/MuscleReflex.h
	controllers/Reflex.cpp
	controllers/Reflex.h
	controllers/ReflexController.cpp
	controllers/ReflexController.h
	)
set(CS_CONTROLLERS_STATE_FILES
	controllers/GaitStateController.cpp
	controllers/GaitStateController.h
	controllers/StateController.cpp
	controllers/StateController.h
	controllers/SensorStateController.cpp
	controllers/SensorStateController.h
	controllers/TimeStateController.cpp
	controllers/TimeStateController.h
	)
set(CS_CONTROLLERS_COMPOSITE_FILES
	controllers/CompositeController.cpp
	controllers/CompositeController.h
	controllers/SequentialController.cpp
	controllers/SequentialController.h
	)
set(MEASURE_FILES
	measures/BalanceMeasure.cpp
	measures/BalanceMeasure.h
	measures/BodyMeasure.cpp
	measures/BodyMeasure.h
	measures/CompositeMeasure.cpp
	measures/CompositeMeasure.h
	measures/DofLimitMeasure.cpp
	measures/DofLimitMeasure.h
	measures/DofMeasure.cpp
	measures/DofMeasure.h
	measures/EffortMeasure.cpp
	measures/EffortMeasure.h
	measures/GaitCycleMeasure.cpp
	measures/GaitCycleMeasure.h
	measures/GaitMeasure.cpp
	measures/GaitMeasure.h
	measures/HeightMeasure.cpp
	measures/HeightMeasure.h
	measures/JumpMeasure.cpp
	measures/JumpMeasure.h
	measures/JointLoadMeasure.cpp
	measures/JointLoadMeasure.h
	measures/MimicMeasure.cpp
	measures/MimicMeasure.h
	measures/ReactionForceMeasure.cpp
	measures/ReactionForceMeasure.h
	measures/RangePenalty.h
	measures/Measure.cpp
	measures/Measure.h
	)
set(OPT_API_FILES
	optimization/CmaOptimizer.cpp
	optimization/CmaOptimizer.h
	optimization/CmaOptimizerSpot.cpp
	optimization/CmaOptimizerSpot.h
	optimization/CmaPoolOptimizer.cpp
	optimization/CmaPoolOptimizer.h
	optimization/Objective.cpp
	optimization/Objective.h
	optimization/Optimizer.cpp
	optimization/Optimizer.h
	optimization/Params.h
	optimization/ModelObjective.cpp
	optimization/ModelObjective.h
	optimization/SimulationObjective.cpp
	optimization/SimulationObjective.h
	optimization/TestObjective.cpp
	optimization/TestObjective.h
	optimization/ImitationObjective.cpp
	optimization/ImitationObjective.h
	optimization/SimilarityObjective.cpp
	optimization/SimilarityObjective.h
    optimization/opt_tools.cpp
    optimization/opt_tools.h
	)
set(SIM_MODELCOMPONENTS_FILES
    model/Body.cpp
    model/Body.h
    model/Dof.cpp
    model/Dof.h
    model/Joint.cpp
    model/Joint.h
    model/Model.cpp
    model/Model.h
    model/Muscle.cpp
    model/Muscle.h
    model/State.cpp
    model/State.h
	)
set(SIM_SENSORSANDACTUATORS_FILES
    model/Actuator.cpp
    model/Actuator.h
    model/Sensor.h
    model/SensorDelayAdapter.cpp
    model/SensorDelayAdapter.h
    model/Sensors.cpp
    model/Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    model/Simulation.cpp
    model/Simulation.h
    )
set(SIM_SYSTEM_FILES
    model/model_tools.cpp
    model/model_tools.h
    )
set(SIM_TOPOLOGY_FILES
    model/Location.cpp
    model/Location.h
    model/Leg.cpp
    model/Leg.h
    model/Link.cpp
    model/Link.h
	model/Side.h
    )

set (SCONELIB_FILES
	${SCONE_CONFIG_FILES}
	${CORE_BASE_FILES}
	${CORE_MATH_FILES}
	${CORE_PROPNODE_FILES}
	${CORE_SYSTEM_FILES}
	${CORE_STORAGE_FILES}
	${CORE_TOOL_FILES}
	
	${CS_CONTROLLERS_FILES}
	${CS_CONTROLLERS_MRC_FILES}
	${CS_CONTROLLERS_NC_FILES}
	${CS_CONTROLLERS_REFLEX_FILES}
	${CS_CONTROLLERS_STATE_FILES}
	${CS_CONTROLLERS_COMPOSITE_FILES}

    ${CS_FUNCTIONS_FILES}
	${MEASURE_FILES}
    ${CS_TOOLS_FILES}
	
	${OPT_API_FILES}
	
	${SIM_MODELCOMPONENTS_FILES}
	${SIM_SENSORSANDACTUATORS_FILES}
    ${SIM_SIMULATIONANDCONTROL_FILES}
	${SIM_SYSTEM_FILES}
	${SIM_TOPOLOGY_FILES}
	)

# specify precompiled headers for MSVC
include( PrecompiledHeaders )
use_precompiled_headers( SCONELIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h" "stdafx.cpp" 256 )

# add sources to single lib
add_library(sconelib SHARED ${SCONELIB_FILES} )

set_property(TARGET sconelib PROPERTY CXX_STANDARD 17)
set_property(TARGET sconelib PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(sconelib xo spot)

set_target_properties(sconelib PROPERTIES PROJECT_LABEL sconelib )

if (MSVC)
	target_compile_definitions( sconelib PRIVATE SCONE_EXPORTS )
	target_compile_definitions( sconelib PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS )
	add_compile_options("/Wd5040")
	target_compile_definitions( sconelib PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_XO_PROFILING>" )
endif()

# To create folders in Visual Studio.
source_group("core\\base" FILES ${CORE_BASE_FILES})
source_group("core\\math" FILES ${CORE_MATH_FILES})
source_group("core\\system" FILES ${CORE_SYSTEM_FILES})
source_group("core\\tools" FILES ${CORE_TOOL_FILES})
source_group("core\\properties" FILES ${CORE_PROPNODE_FILES})
source_group("core\\storage" FILES ${CORE_STORAGE_FILES})

source_group("controllers" FILES ${CS_CONTROLLERS_FILES} )
source_group("controllers\\State Control" FILES ${CS_CONTROLLERS_STATE_FILES} )
source_group("controllers\\Composite Control" FILES ${CS_CONTROLLERS_COMPOSITE_FILES} )
source_group("controllers\\Reflex Control" FILES ${CS_CONTROLLERS_REFLEX_FILES})
source_group("controllers\\Neural Control" FILES ${CS_CONTROLLERS_NC_FILES})
source_group("controllers" FILES ${CS_SYSTEM_FILES})
source_group("controllers\\Meta Reflex Control" FILES ${CS_CONTROLLERS_MRC_FILES})

source_group("measures" FILES ${MEASURE_FILES})

source_group("optimization" FILES ${OPT_API_FILES})
source_group("optimization" FILES ${OPT_SYSTEM_FILES})

source_group("model" FILES ${SIM_MODELCOMPONENTS_FILES})
source_group("model" FILES ${SIM_SENSORSANDACTUATORS_FILES})
source_group("model" FILES ${SIM_SIMULATIONANDCONTROL_FILES})
source_group("model" FILES ${SIM_SYSTEM_FILES})
source_group("model" FILES ${SIM_TOPOLOGY_FILES})

source_group("" FILES ${PRECOMPILED_HEADER_FILES})
source_group("" FILES ${SCONE_CONFIG_FILES})
