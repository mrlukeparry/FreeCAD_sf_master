
# Set the name of the plugin (excluding the .so)
# While not required this should match the name of the directory we are infor consistency 
SET(TPG_TARGET TestPlugin) 

# Add a CAM TOOL PATH GENERATOR PLUGIN macro so that we can set the dllimport/dllexport linkages correctly.
add_definitions(-DCAM_TPG_PLUGIN)

# Add your .h and .cpp files here
SET(Tpg_SRCS
	CppExampleTPG.cpp
	CppExampleTPG.h
	../../TPG/CppTPG.cpp
	../../TPG/CppTPG.h
	../../TPG/CppTPGDescriptor.cpp
	../../TPG/CppTPGDescriptor.h
	../../TPG/CppTPGDescriptorWrapper.cpp
	../../TPG/CppTPGDescriptorWrapper.h
	../../TPG/TPGDescriptor.cpp
	../../TPG/TPGDescriptor.h
	../../TPG/TPGDescriptorCollection.cpp
	../../TPG/TPGDescriptorCollection.h
	../../TPG/CppTPGPlugin.cpp
	../../TPG/CppTPGPlugin.h
)

# Add any libs that are needed
SET(Tpg_LIBS
	Cam
)

SOURCE_GROUP("TPG" FILES ${Tpg_SRCS})

ADD_LIBRARY(${TPG_TARGET} SHARED ${Tpg_SRCS})
TARGET_LINK_LIBRARIES(${TPG_TARGET} ${Tpg_LIBS})

if (MSVC)
	# Copy 'hack' from src/Main/CMakeLists.txt to send the DLL to the place where
	# the CppTPGPlugin::isOpen() method looks for it.
	set_target_properties(${TPG_TARGET} PROPERTIES PREFIX "../../../../../../../Mod/Cam/CppTPG/")
endif(MSVC)

set_target_properties(${TPG_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam/CppTPG)

if (MSVC)
	INSTALL(TARGETS ${TPG_TARGET} 
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib )
else(MSVC)
	INSTALL(TARGETS ${TPG_TARGET} DESTINATION lib)
endif(MSVC)
