if(MSVC)
    add_definitions(-DFCAppCam -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H)
endif(MSVC)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(Cam_LIBS
    Part
    ${OCC_LIBRARIES}
    FreeCADApp
)

add_subdirectory(Libs)

# generate_from_xml(CamPy)

SET(Features_SRCS
    CamPartsList.cpp
    CamPartsList.h
    Feature.cpp
    Feature.h
    GCodeFeature.cpp
    GCodeFeature.h
    StockGeometry.cpp
    StockGeometry.h
    TPGFeature.cpp
    TPGFeature.h
    TPGList.cpp
    TPGList.h
    CamFeature.cpp
    CamFeature.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

#Classes related to Tool Path Generation
SET(TPG_SRCS
    TPG/TPG.cpp
    TPG/TPG.h
    TPG/TPGCache.cpp
    TPG/TPGCache.h
    TPG/TPGFactory.cpp
    TPG/TPGFactory.h
    TPG/TPGLib.cpp
    TPG/TPGLib.cpp
    TPG/TPGProcess.cpp
    TPG/TPGProcess.h
    TPG/TPGPython.cpp
    TPG/TPGPython.h
    TPG/TPGSettings.cpp
    TPG/TPGSettings.h
    TPG/PyTPGManager.cpp
    TPG/PyTPGManager.h
    
    TPG/CppTPG.h
    TPG/CppTPGDescriptorWrapper.cpp
    TPG/CppTPGDescriptorWrapper.h
    TPG/CppTPGFactory.cpp
    TPG/CppTPGFactory.h
    TPG/CppTPGPlugin.cpp
    TPG/CppTPGPlugin.h
    TPG/CppTPGWrapper.cpp
    TPG/CppTPGWrapper.h
)
SOURCE_GROUP("TPG" FILES ${TPG_SRCS})

# SET(Python_SRCS
# )
# SOURCE_GROUP("Python" FILES ${Python_SRCS})

SET(CamModule_SRCS
    AppCam.cpp
    AppCamPy.cpp
    GCode.cpp
    GCode.h
    MachineSettings.cpp
    MachineSettings.h
    PreCompiled.cpp
    PreCompiled.h
    Support.cpp
    Support.h
)
SOURCE_GROUP("Module" FILES ${CamModule_SRCS})

SET(Cam_SRCS
    ${TPG_SRCS}
    ${CamModule_SRCS}
    ${Features_SRCS}
    ${Python_SRCS}
)

SET(Cam_Scripts
    Init.py
    InitGui.py
)

SET(TPG_Scripts
    __init__.py
    PyCam.py
    PyTPG/__init__.py

    ### Copy Python TPG's to the distribution directory ###
    # Add yours here #
    
    # ExampleTPG module
    PyTPG/ExampleTPG/__init__.py
    PyTPG/ExampleTPG/ExampleTPG.py
)

SOURCE_GROUP("" FILES ${TPG_Scripts})
ADD_CUSTOM_TARGET(TPG_Scripts ALL
    SOURCES ${TPG_Scripts}
)
fc_copy_sources("Cam" "${CMAKE_BINARY_DIR}/Mod/PyCam" ${TPG_Scripts}) #TODO: sort out the C++/Python conflicting package names so this can be moved back inside the Cam directory

install(
    FILES
        ${TPG_Scripts}
    DESTINATION
        Mod/PyCam
)

add_library(Cam SHARED ${Cam_SRCS})
target_link_libraries(Cam ${Cam_LIBS})

fc_target_copy_resource(Cam
    ${CMAKE_SOURCE_DIR}/src/Mod/Cam2
    ${CMAKE_BINARY_DIR}/Mod/Cam
    ${Cam_Scripts})

if(MSVC)
    set_target_properties(Cam PROPERTIES SUFFIX ".pyd")
    set_target_properties(Cam PROPERTIES DEBUG_OUTPUT_NAME "Cam_d")
    set_target_properties(Cam PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam)
    set_target_properties(Cam PROPERTIES PREFIX "../")

elseif(MINGW)
    set_target_properties(Cam PROPERTIES SUFFIX ".pyd")
    set_target_properties(Cam PROPERTIES DEBUG_OUTPUT_NAME "Cam_d")
    set_target_properties(Cam PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam)
    set_target_properties(Cam PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(Cam PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Cam)
    set_target_properties(Cam PROPERTIES PREFIX "")
    set_target_properties(Cam PROPERTIES INSTALL_RPATH ${INSTALL_RPATH})
endif(MSVC)

INSTALL(TARGETS Cam DESTINATION lib)
