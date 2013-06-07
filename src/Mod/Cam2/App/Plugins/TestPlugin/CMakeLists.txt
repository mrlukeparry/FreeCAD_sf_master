
# Set the name of the plugin (excluding the .so)
# While not required this should match the name of the directory we are infor consistency 
SET(TPG_TARGET TestPlugin) 

# Add your .h and .cpp files here
SET(Tpg_SRCS
	CppExampleTPG.cpp
	CppExampleTPG.h
	../../TPG/CppTPGPlugin.cpp
	../../TPG/CppTPGPlugin.h
	../../TPG/CppTPGDescriptor.cpp
	../../TPG/CppTPGDescriptor.h
	../../TPG/CppTPGDescriptorWrapper.cpp
	../../TPG/CppTPGDescriptorWrapper.h
	../../TPG/TPGDescriptor.cpp
	../../TPG/TPGDescriptor.h
	../../TPG/TPGDescriptorCollection.cpp
	../../TPG/TPGDescriptorCollection.h
)

# Add any libs that are needed
SET(Tpg_LIBS
	Cam
)

SOURCE_GROUP("TPG" FILES ${Tpg_SRCS})

ADD_LIBRARY(${TPG_TARGET} SHARED ${Tpg_SRCS})
TARGET_LINK_LIBRARIES(${TPG_TARGET} ${Tpg_LIBS})

set_target_properties(${TPG_TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam/CppTPG)
INSTALL(TARGETS ${TPG_TARGET} DESTINATION lib)
