
# Set the name of the plugin (excluding the .so)
# While not required this should match the name of the directory we are infor consistency 
SET(TPG_TARGET TestPlugin) 

# Add your .h and .cpp files here
SET(Tpg_SRCS
	CppExampleTPG.cpp
	CppExampleTPG.h
)

# Add any libs that are needed
SET(Tpg_LIBS
)

SOURCE_GROUP("TPG" FILES ${Tpg_SRCS})

ADD_LIBRARY(TestPlugin SHARED ${Tpg_SRCS})
TARGET_LINK_LIBRARIES(TestPlugin ${Tpg_LIBS})

set_target_properties(TestPlugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam/CppTPG)
INSTALL(TARGETS TestPlugin DESTINATION lib)
