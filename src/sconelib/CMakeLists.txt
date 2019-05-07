add_subdirectory(scone)

if (SCONE_OPENSIM_3)
	add_subdirectory(sconeopensim3)
endif()

if (SCONE_OPENSIM_4)
	add_subdirectory(sconeopensim4)
endif()

if (SCONE_LUA)
	add_subdirectory(sconelua)
endif()
