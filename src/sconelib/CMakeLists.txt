include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CORE_BASE_FILES
	scone/core/HasData.h
	scone/core/HasName.h
	scone/core/HasExternalResources.h
	scone/core/HasSignature.cpp
	scone/core/HasSignature.h
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
	scone/core/Function.h
	scone/core/PieceWiseConstantFunction.cpp
	scone/core/PieceWiseConstantFunction.h
	scone/core/PieceWiseLinearFunction.cpp
	scone/core/PieceWiseLinearFunction.h
	scone/core/Polynomial.h	
	scone/core/Polynomial.cpp
)
set(CORE_SYSTEM_FILES
	scone/core/Factories.h
    scone/core/Factories.cpp
	scone/core/memory_tools.h
    scone/core/ResourceCache.h
	scone/core/Profiler.cpp
    scone/core/Profiler.h
	scone/core/Exception.h
	scone/core/platform.h
	scone/core/Log.cpp
	scone/core/Log.h
	scone/core/version.cpp
	scone/core/version.h
	scone/core/types.h
	scone/core/system_tools.cpp
	scone/core/system_tools.h
	scone/core/Settings.cpp
	scone/core/Settings.h
)
set(CORE_STORAGE_FILES
	scone/core/Storage.h
	scone/core/StorageIo.h
	scone/core/StorageIo.cpp
	scone/core/PropNode.h
	scone/core/StringMap.h
)
set(CORE_TOOL_FILES
	scone/core/Event.cpp
	scone/core/Event.h
	scone/core/string_tools.cpp
	scone/core/string_tools.h
)
set(CS_CONTROLLERS_FILES
	scone/controllers/Controller.cpp
	scone/controllers/Controller.h
	scone/controllers/FeedForwardController.cpp
	scone/controllers/FeedForwardController.h
	scone/controllers/PerturbationController.cpp
	scone/controllers/PerturbationController.h
	scone/controllers/ExternalBalanceController.cpp
	scone/controllers/ExternalBalanceController.h
	scone/controllers/MirrorController.h
	scone/controllers/NoiseController.h
	scone/controllers/NoiseController.cpp
	)
set(CS_CONTROLLERS_NC_FILES
	scone/controllers/NeuralController.cpp
	scone/controllers/NeuralController.h
	scone/controllers/Neuron.cpp
	scone/controllers/Neuron.h
	scone/controllers/SensorNeuron.cpp
	scone/controllers/SensorNeuron.h
	scone/controllers/InterNeuron.cpp
	scone/controllers/InterNeuron.h
	scone/controllers/MotorNeuron.cpp
	scone/controllers/MotorNeuron.h
	scone/controllers/PatternNeuron.cpp
	scone/controllers/PatternNeuron.h
	scone/controllers/activation_functions.cpp
	scone/controllers/activation_functions.h
	)
set(CS_CONTROLLERS_REFLEX_FILES
	scone/controllers/ConditionalMuscleReflex.cpp
	scone/controllers/ConditionalMuscleReflex.h
	scone/controllers/DofReflex.cpp
	scone/controllers/DofReflex.h
	scone/controllers/BodyPointReflex.cpp
	scone/controllers/BodyPointReflex.h
	scone/controllers/MuscleReflex.cpp
	scone/controllers/MuscleReflex.h
	scone/controllers/Reflex.cpp
	scone/controllers/Reflex.h
	scone/controllers/ReflexController.cpp
	scone/controllers/ReflexController.h
	)
set(CS_CONTROLLERS_STATE_FILES
	scone/controllers/GaitStateController.cpp
	scone/controllers/GaitStateController.h
	scone/controllers/StateController.cpp
	scone/controllers/StateController.h
	scone/controllers/SensorStateController.cpp
	scone/controllers/SensorStateController.h
	scone/controllers/TimeStateController.cpp
	scone/controllers/TimeStateController.h
	)
set(CS_CONTROLLERS_COMPOSITE_FILES
	scone/controllers/CompositeController.cpp
	scone/controllers/CompositeController.h
	scone/controllers/SequentialController.cpp
	scone/controllers/SequentialController.h
	)
set(MEASURE_FILES
	scone/measures/BalanceMeasure.cpp
	scone/measures/BalanceMeasure.h
	scone/measures/BodyMeasure.cpp
	scone/measures/BodyMeasure.h
	scone/measures/CompositeMeasure.cpp
	scone/measures/CompositeMeasure.h
	scone/measures/DofLimitMeasure.cpp
	scone/measures/DofLimitMeasure.h
	scone/measures/DofMeasure.cpp
	scone/measures/DofMeasure.h
	scone/measures/EffortMeasure.cpp
	scone/measures/EffortMeasure.h
	scone/measures/GaitCycleMeasure.cpp
	scone/measures/GaitCycleMeasure.h
	scone/measures/GaitMeasure.cpp
	scone/measures/GaitMeasure.h
	scone/measures/HeightMeasure.cpp
	scone/measures/HeightMeasure.h
	scone/measures/JumpMeasure.cpp
	scone/measures/JumpMeasure.h
	scone/measures/JointLoadMeasure.cpp
	scone/measures/JointLoadMeasure.h
	scone/measures/MimicMeasure.cpp
	scone/measures/MimicMeasure.h
	scone/measures/ReactionForceMeasure.cpp
	scone/measures/ReactionForceMeasure.h
	scone/measures/RangePenalty.h
	scone/measures/Measure.cpp
	scone/measures/Measure.h
	)

set(OPT_API_FILES
	scone/optimization/CmaOptimizer.cpp
	scone/optimization/CmaOptimizer.h
	scone/optimization/CmaOptimizerSpot.cpp
	scone/optimization/CmaOptimizerSpot.h
	scone/optimization/CmaPoolOptimizer.cpp
	scone/optimization/CmaPoolOptimizer.h
	scone/optimization/Objective.cpp
	scone/optimization/Objective.h
	scone/optimization/Optimizer.cpp
	scone/optimization/Optimizer.h
	scone/optimization/Params.h
	scone/optimization/ModelObjective.cpp
	scone/optimization/ModelObjective.h
	scone/optimization/SimulationObjective.cpp
	scone/optimization/SimulationObjective.h
	scone/optimization/TestObjective.cpp
	scone/optimization/TestObjective.h
	scone/optimization/ImitationObjective.cpp
	scone/optimization/ImitationObjective.h
	scone/optimization/SimilarityObjective.cpp
	scone/optimization/SimilarityObjective.h
    scone/optimization/opt_tools.cpp
    scone/optimization/opt_tools.h
    )
	
set(SIM_MODELCOMPONENTS_FILES
    scone/model/Body.cpp
    scone/model/Body.h
    scone/model/Dof.cpp
    scone/model/Dof.h
    scone/model/Joint.cpp
    scone/model/Joint.h
    scone/model/Model.cpp
    scone/model/Model.h
    scone/model/Muscle.cpp
    scone/model/Muscle.h
    scone/model/State.cpp
    scone/model/State.h
    )
set(SIM_SENSORSANDACTUATORS_FILES
    scone/model/Actuator.cpp
    scone/model/Actuator.h
    scone/model/Sensor.h
    scone/model/SensorDelayAdapter.cpp
    scone/model/SensorDelayAdapter.h
    scone/model/Sensors.cpp
    scone/model/Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    scone/model/Simulation.cpp
    scone/model/Simulation.h
    )
set(SIM_SYSTEM_FILES
    scone/model/model_tools.cpp
    scone/model/model_tools.h
    )
set(SIM_TOPOLOGY_FILES
    scone/model/Location.cpp
    scone/model/Location.h
    scone/model/Leg.cpp
    scone/model/Leg.h
    scone/model/Link.cpp
    scone/model/Link.h
	scone/model/Side.h
    )

if(SCONE_OPENSIM_3)
	set(SIMBODY_FILES
		scone/model/simbody/BodySimbody.cpp
		scone/model/simbody/BodySimbody.h
		scone/model/simbody/DofSimbody.cpp
		scone/model/simbody/DofSimbody.h
		scone/model/simbody/JointSimbody.cpp
		scone/model/simbody/JointSimbody.h
		scone/model/simbody/ModelSimbody.cpp
		scone/model/simbody/ModelSimbody.h
		scone/model/simbody/MuscleSimbody.cpp
		scone/model/simbody/MuscleSimbody.h
		scone/model/simbody/SimulationSimbody.cpp
		scone/model/simbody/SimulationSimbody.h
		scone/model/simbody/ConstantForce.cpp
		scone/model/simbody/ConstantForce.h
		scone/model/simbody/sim_simbody.cpp
		scone/model/simbody/sim_simbody.h
		scone/model/simbody/simbody_tools.h
		)
else()
    set(SIMBODY_FILES)
endif()

if(SCONE_OPENSIM_4)
	set(SIMBODY4_FILES
			scone/model/simbody4/BodySimbody.cpp
			scone/model/simbody4/BodySimbody.h
			scone/model/simbody4/DofSimbody.cpp
			scone/model/simbody4/DofSimbody.h
			scone/model/simbody4/JointSimbody.cpp
			scone/model/simbody4/JointSimbody.h
			scone/model/simbody4/ModelSimbody.cpp
			scone/model/simbody4/ModelSimbody.h
			scone/model/simbody4/MuscleSimbody.cpp
			scone/model/simbody4/MuscleSimbody.h
			scone/model/simbody4/SimulationSimbody.cpp
			scone/model/simbody4/SimulationSimbody.h
			scone/model/simbody4/ConstantForce.cpp
			scone/model/simbody4/ConstantForce.h
			scone/model/simbody4/sim_simbody.cpp
			scone/model/simbody4/sim_simbody.h
			scone/model/simbody4/simbody_tools.h
			)
else()
	set(SIMBODY_FILES)
endif()

set (SCONELIB_FILES
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
	
	${SIMBODY_FILES}
	${SIMBODY4_FILES}
	)

# specify precompiled headers for MSVC
include( PrecompiledHeaders )
use_precompiled_headers( SCONELIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h" "stdafx.cpp" 256 )

# add sources to single lib
add_library(sconelib SHARED ${SCONELIB_FILES} )

set_property(TARGET sconelib PROPERTY CXX_STANDARD 17)
set_property(TARGET sconelib PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(sconelib xo spot)
if(SCONE_OPENSIM_3)
	target_link_libraries(sconelib ${OPENSIMSIMBODY_LIBRARIES})
	target_compile_definitions(sconelib PUBLIC SCONE_OPENSIM_3)
endif()
if(SCONE_OPENSIM_4)
	target_link_libraries(sconelib osimTools)
	target_compile_definitions(sconelib PUBLIC SCONE_OPENSIM_4)
endif()


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

source_group("model\\simbody" FILES ${SIMBODY_FILES})

source_group("" FILES ${PRECOMPILED_HEADER_FILES})
