include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CORE_BASE_FILES
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
	scone/core/Function.h
	scone/core/PieceWiseConstantFunction.cpp
	scone/core/PieceWiseConstantFunction.h
	scone/core/PieceWiseFunction.h
	scone/core/PieceWiseLinearFunction.cpp
	scone/core/PieceWiseLinearFunction.h
	scone/core/PolynomialFunction.h	
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
	scone/core/core.h
	scone/core/Log.cpp
	scone/core/Log.h
	scone/core/version.h
	scone/core/types.h
	scone/core/system_tools.cpp
	scone/core/system_tools.h
)
set(CORE_STORAGE_FILES
	scone/core/Storage.h
	scone/core/StorageIo.h
	scone/core/StorageIo.cpp
	scone/core/propnode_tools.h
	scone/core/propnode_tools.cpp
	scone/core/PropNode.h
	scone/core/StringMap.h
	scone/core/string_tools.cpp
	scone/core/string_tools.h
)
set(CS_CONTROLLERS_FILES
	scone/controllers/FeedForwardController.cpp
	scone/controllers/FeedForwardController.h
	scone/controllers/PerturbationController.cpp
	scone/controllers/PerturbationController.h
	scone/controllers/ExternalBalanceController.cpp
	scone/controllers/ExternalBalanceController.h
	scone/controllers/MirrorController.h
	)
set(CS_SYSTEM_FILES
	scone/controllers/cs_tools.cpp
	scone/controllers/cs_tools.h
	)
set(CS_CONTROLLERS_MRC_FILES
	scone/controllers/MetaReflexParams.cpp
	scone/controllers/MetaReflexParams.h
	scone/controllers/MetaReflexController.cpp
	scone/controllers/MetaReflexController.h
    scone/controllers/MetaReflexDof.cpp
    scone/controllers/MetaReflexDof.h
    scone/controllers/MetaReflexMuscle.cpp
    scone/controllers/MetaReflexMuscle.h
    scone/controllers/MetaReflexVirtualMuscle.cpp
    scone/controllers/MetaReflexVirtualMuscle.h
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
	scone/controllers/MuscleReflex.cpp
	scone/controllers/MuscleReflex.h
	scone/controllers/SimpleMuscleReflex.cpp
	scone/controllers/SimpleMuscleReflex.h
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
set(OBJECTIVES_FILES
	scone/objectives/CompositeMeasure.cpp
	scone/objectives/CompositeMeasure.h
	scone/objectives/DofLimitMeasure.cpp
	scone/objectives/DofLimitMeasure.h
	scone/objectives/EffortMeasure.cpp
	scone/objectives/EffortMeasure.h
	scone/objectives/GaitCycleMeasure.cpp
	scone/objectives/GaitCycleMeasure.h
	scone/objectives/GaitMeasure.cpp
	scone/objectives/GaitMeasure.h
	scone/objectives/HeightMeasure.cpp
	scone/objectives/HeightMeasure.h
	scone/objectives/JumpMeasure.cpp
	scone/objectives/JumpMeasure.h
	scone/objectives/JointLoadMeasure.cpp
	scone/objectives/JointLoadMeasure.h
	scone/objectives/ReactionForceMeasure.cpp
	scone/objectives/ReactionForceMeasure.h
	scone/objectives/PointMeasure.cpp
	scone/objectives/PointMeasure.h
	scone/objectives/RangePenalty.h
	scone/objectives/Measure.cpp
	scone/objectives/Measure.h
	scone/objectives/ModelObjective.cpp
	scone/objectives/ModelObjective.h
	scone/objectives/SimulationObjective.cpp
	scone/objectives/SimulationObjective.h
	scone/objectives/ImitationObjective.cpp
	scone/objectives/ImitationObjective.h
	)

set(OPT_API_FILES
	scone/optimization/CmaOptimizer.cpp
	scone/optimization/CmaOptimizer.h
#	scone/optimization/CmaOptimizerShark2.cpp
#	scone/optimization/CmaOptimizerShark2.h
#	scone/optimization/CmaOptimizerShark3.cpp
#	scone/optimization/CmaOptimizerShark3.h
	scone/optimization/CmaOptimizerCCMAES.cpp
	scone/optimization/CmaOptimizerCCMAES.h
	scone/optimization/Objective.cpp
	scone/optimization/Objective.h
	scone/optimization/Optimizer.cpp
	scone/optimization/Optimizer.h
#	scone/optimization/ParamInfo.cpp
#	scone/optimization/ParamInfo.h
#	scone/optimization/ParamSet.cpp
	scone/optimization/Params.h
    scone/optimization/opt_tools.cpp
    scone/optimization/opt_tools.h
    )
set(OPT_SYSTEM_FILES
#	scone/optimization/SharkHelperClasses.h
	scone/optimization/opt_config.h
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
    scone/model/Sensor.cpp
    scone/model/Sensor.h
    scone/model/SensorDelayAdapter.cpp
    scone/model/SensorDelayAdapter.h
    scone/model/Sensors.cpp
    scone/model/Sensors.h
    )
set(SIM_SIMULATIONANDCONTROL_FILES
    scone/model/Controller.cpp
    scone/model/Controller.h
    scone/model/Simulation.cpp
    scone/model/Simulation.h
    )
set(SIM_SYSTEM_FILES
    scone/model/sim_tools.cpp
    scone/model/sim_tools.h
    )
set(SIM_TOPOLOGY_FILES
    scone/model/Locality.cpp
    scone/model/Locality.h
    scone/model/Leg.cpp
    scone/model/Leg.h
    scone/model/Link.cpp
    scone/model/Link.h
	scone/model/Side.h
    )
	
set(SIMBODY_FILES
    scone/model/simbody/Body_Simbody.cpp
    scone/model/simbody/Body_Simbody.h
    scone/model/simbody/Dof_Simbody.cpp
    scone/model/simbody/Dof_Simbody.h
    scone/model/simbody/Joint_Simbody.cpp
    scone/model/simbody/Joint_Simbody.h
    scone/model/simbody/Model_Simbody.cpp
    scone/model/simbody/Model_Simbody.h
    scone/model/simbody/Muscle_Simbody.cpp
    scone/model/simbody/Muscle_Simbody.h
    scone/model/simbody/Simulation_Simbody.cpp
    scone/model/simbody/Simulation_Simbody.h
    scone/model/simbody/ConstantForce.cpp
    scone/model/simbody/ConstantForce.h
    scone/model/simbody/sim_simbody.cpp
    scone/model/simbody/sim_simbody.h
    scone/model/simbody/simbody_tools.h
    )

set (SCONELIB_FILES
	${CORE_BASE_FILES}
	${CORE_MATH_FILES}
	${CORE_PROPNODE_FILES}
	${CORE_SYSTEM_FILES}
	${CORE_STORAGE_FILES}
	
	${CS_CONTROLLERS_FILES}
	${CS_CONTROLLERS_MRC_FILES}
	${CS_CONTROLLERS_NC_FILES}
	${CS_CONTROLLERS_REFLEX_FILES}
	${CS_CONTROLLERS_STATE_FILES}
    ${CS_FUNCTIONS_FILES}
	${OBJECTIVES_FILES}
	${CS_SYSTEM_FILES} 
    ${CS_TOOLS_FILES}
	
	${OPT_API_FILES}
	${OPT_SYSTEM_FILES}
	
	${SIM_MODELCOMPONENTS_FILES}
	${SIM_SENSORSANDACTUATORS_FILES}
    ${SIM_SIMULATIONANDCONTROL_FILES}
	${SIM_SYSTEM_FILES}
	${SIM_TOPOLOGY_FILES}
	
	${SIMBODY_FILES}
	)

# specify precompiled headers for MSVC
include( PrecompiledHeaders )
use_precompiled_headers( SCONELIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h" "stdafx.cpp" 256 )

# add sources to single lib
add_library(sconelib SHARED ${SCONELIB_FILES} )

target_link_libraries(sconelib flut spot ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(sconelib PROPERTIES PROJECT_LABEL sconelib )

target_compile_definitions( sconelib PRIVATE SCONE_EXPORTS )
target_compile_definitions( sconelib PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SCONE_ENABLE_FLUT_PROFILING>" )

# To create folders in Visual Studio.
source_group("core\\base" FILES ${CORE_BASE_FILES})
source_group("core\\math" FILES ${CORE_MATH_FILES})
source_group("core\\system" FILES ${CORE_SYSTEM_FILES})
source_group("core\\properties" FILES ${CORE_PROPNODE_FILES})
source_group("core\\storage" FILES ${CORE_STORAGE_FILES})

source_group("controllers" FILES ${CS_CONTROLLERS_FILES} )
source_group("controllers\\State Control" FILES ${CS_CONTROLLERS_STATE_FILES} )
source_group("controllers\\Reflex Control" FILES ${CS_CONTROLLERS_REFLEX_FILES})
source_group("controllers\\Neural Control" FILES ${CS_CONTROLLERS_NC_FILES})
source_group("controllers" FILES ${CS_SYSTEM_FILES})
source_group("controllers\\Meta Reflex Control" FILES ${CS_CONTROLLERS_MRC_FILES})

source_group("objectives" FILES ${OBJECTIVES_FILES})

source_group("optimization" FILES ${OPT_API_FILES})
source_group("optimization" FILES ${OPT_SYSTEM_FILES})

source_group("model" FILES ${SIM_MODELCOMPONENTS_FILES})
source_group("model" FILES ${SIM_SENSORSANDACTUATORS_FILES})
source_group("model" FILES ${SIM_SIMULATIONANDCONTROL_FILES})
source_group("model" FILES ${SIM_SYSTEM_FILES})
source_group("model" FILES ${SIM_TOPOLOGY_FILES})

source_group("model\\simbody" FILES ${SIMBODY_FILES})

source_group("" FILES ${PRECOMPILED_HEADER_FILES})
