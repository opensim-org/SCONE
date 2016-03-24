set(CONTROLLERS_FEEDFORWARD_FILES
	FeedForwardController.cpp
	FeedForwardController.h
	)
set(CONTROLLERS_MRC_FILES
	MetaReflexParams.cpp
	MetaReflexParams.h
	MetaReflexController.cpp
	MetaReflexController.h
    MetaReflexDof.cpp
    MetaReflexDof.h
    MetaReflexMuscle.cpp
    MetaReflexMuscle.h
    MetaReflexVirtualMuscle.cpp
    MetaReflexVirtualMuscle.h
	)
set(CONTROLLERS_REFLEX_FILES
	ConditionalMuscleReflex.cpp
	ConditionalMuscleReflex.h
	DofReflex.cpp
	DofReflex.h
	MuscleReflex.cpp
	MuscleReflex.h
	Reflex.cpp
	Reflex.h
	ReflexController.cpp
	ReflexController.h
	)
set(CONTROLLERS_STATE_FILES
	GaitStateController.cpp
	GaitStateController.h
	StateController.cpp
	StateController.h
	TimeStateController.cpp
	TimeStateController.h
	)
set(FUNCTIONS_FILES
	Function.h
	PieceWiseConstantFunction.cpp
	PieceWiseConstantFunction.h
	PieceWiseFunction.h
	PieceWiseLinearFunction.cpp
	PieceWiseLinearFunction.h
	Polynomial.cpp
	Polynomial.h
	)
set(MEASURES_FILES
	CompositeMeasure.cpp
	CompositeMeasure.h
	DofLimitMeasure.cpp
	DofLimitMeasure.h
	EffortMeasure.cpp
	EffortMeasure.h
	GaitCycleMeasure.cpp
	GaitCycleMeasure.h
	GaitMeasure.cpp
	GaitMeasure.h
	HeightMeasure.cpp
	HeightMeasure.h
	JumpMeasure.cpp
	JumpMeasure.h
	Measure.cpp
	Measure.h
	)
set(SYSTEM_FILES
	cs.cpp
	cs.h
	Factories.cpp
	Factories.h
	version.h
	)
set(TOOLS_FILES
	SimulationObjective.cpp
	SimulationObjective.h
	tools.h
	)
add_library(scone-cs # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${CONTROLLERS_FEEDFORWARD_FILES}
	${CONTROLLERS_MRC_FILES}
	${CONTROLLERS_REFLEX_FILES}
	${CONTROLLERS_STATE_FILES}
    ${FUNCTIONS_FILES}
	${MEASURES_FILES}
	${SYSTEM_FILES} 
    ${TOOLS_FILES})
target_link_libraries(scone-cs scone-sim_simbody
    ${Boost_LIBRARIES} ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(scone-cs PROPERTIES
    DEFINE_SYMBOL CS_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL cs # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
# The '\\' creates a nested directory.
source_group("Controllers\\Feed Forward Control" FILES ${CONTROLLERS_FEEDFORWARD_FILES})
source_group("Controllers\\Meta Reflex Control" FILES ${CONTROLLERS_MRC_FILES})
source_group("Controllers\\Reflex Control" FILES ${CONTROLLERS_REFLEX_FILES})
source_group("Controllers\\State Control" FILES ${CONTROLLERS_STATE_FILES})
source_group("Functions" FILES ${FUNCTIONS_FILES})
source_group("Measures" FILES ${MEASURES_FILES})
source_group("System" FILES ${SYSTEM_FILES})
source_group("Tools" FILES ${TOOLS_FILES})