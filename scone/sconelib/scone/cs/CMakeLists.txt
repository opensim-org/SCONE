set(CS_CONTROLLERS_FEEDFORWARD_FILES
	FeedForwardController.cpp
	FeedForwardController.h
	)
set(CS_CONTROLLERS_MRC_FILES
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
set(CS_CONTROLLERS_REFLEX_FILES
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
set(CS_CONTROLLERS_STATE_FILES
	GaitStateController.cpp
	GaitStateController.h
	StateController.cpp
	StateController.h
	TimeStateController.cpp
	TimeStateController.h
	)
set(CS_FUNCTIONS_FILES
	Function.h
	PieceWiseConstantFunction.cpp
	PieceWiseConstantFunction.h
	PieceWiseFunction.h
	PieceWiseLinearFunction.cpp
	PieceWiseLinearFunction.h
	Polynomial.cpp
	Polynomial.h
	)
set(CS_MEASURES_FILES
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
set(CS_SYSTEM_FILES
	cs.cpp
	cs.h
	Factories.cpp
	Factories.h
	version.h
	)
set(CS_TOOLS_FILES
	SimulationObjective.cpp
	SimulationObjective.h
	tools.h
	)
add_library(scone-cs # Name of the library file.
    SHARED # versus static.
    # Source code files:
    ${CS_CONTROLLERS_FEEDFORWARD_FILES}
	${CS_CONTROLLERS_MRC_FILES}
	${CS_CONTROLLERS_REFLEX_FILES}
	${CS_CONTROLLERS_STATE_FILES}
    ${CS_FUNCTIONS_FILES}
	${CS_MEASURES_FILES}
	${CS_SYSTEM_FILES} 
    ${CS_TOOLS_FILES})
target_link_libraries(scone-cs scone-sim_simbody
    ${Boost_LIBRARIES} ${OPENSIMSIMBODY_LIBRARIES})

set_target_properties(scone-cs PROPERTIES
    DEFINE_SYMBOL CS_EXPORTS # For controlling WIN32 dllimport/export.
    PROJECT_LABEL cs # Name of this target in Visual Studio.
    FOLDER "sconelib" # Place target inside a sconelib folder in Visual Studio.
    )

# To create folders in Visual Studio.
# The '\\' creates a nested directory.
source_group("Controllers\\Feed Forward Control" FILES ${CS_CONTROLLERS_FEEDFORWARD_FILES})
source_group("Controllers\\Meta Reflex Control" FILES ${CS_CONTROLLERS_MRC_FILES})
source_group("Controllers\\Reflex Control" FILES ${CS_CONTROLLERS_REFLEX_FILES})
source_group("Controllers\\State Control" FILES ${CS_CONTROLLERS_STATE_FILES})
source_group("Functions" FILES ${CS_FUNCTIONS_FILES})
source_group("Measures" FILES ${CS_MEASURES_FILES})
source_group("System" FILES ${CS_SYSTEM_FILES})
source_group("Tools" FILES ${CS_TOOLS_FILES})
