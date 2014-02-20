if(MSVC)
    add_definitions(-DFCAppDrawing -DHAVE_ACOSH -DHAVE_ASINH -DHAVE_ATANH)
else(MSVC)
    add_definitions(-DHAVE_LIMITS_H -DHAVE_CONFIG_H -DHAVE_ACOSH -DHAVE_ATANH -DHAVE_ASINH)
endif(MSVC)

include_directories(
    ${CMAKE_BINARY_DIR}/src
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
    ${OCC_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    ${PYTHON_INCLUDE_PATH}
    ${XERCESC_INCLUDE_DIR}
)
link_directories(${OCC_LIBRARY_DIR})

set(Drawing_LIBS
    Measure
    Part
    FreeCADApp
)

generate_from_xml(FeatureTemplatePy)
generate_from_xml(FeatureParametricTemplatePy)

SET(Features_SRCS
    FeaturePage.cpp
    FeaturePage.h
    FeatureClip.cpp
    FeatureClip.h
    FeatureOrthoView.cpp
    FeatureOrthoView.h
    FeatureParametricTemplate.cpp
    FeatureParametricTemplate.h
    FeatureSVGTemplate.cpp
    FeatureSVGTemplate.h
    FeatureTemplate.cpp
    FeatureTemplate.h
    FeatureView.cpp
    FeatureView.h
    FeatureViewAnnotation.cpp
    FeatureViewAnnotation.h
    FeatureViewCollection.cpp
    FeatureViewCollection.h
    FeatureViewDimension.cpp
    FeatureViewDimension.h
    FeatureViewOrthographic.cpp
    FeatureViewOrthographic.h
    FeatureViewPart.cpp
    FeatureViewPart.h
    FeatureViewSection.cpp
    FeatureViewSection.h
    PageGroup.cpp
    PageGroup.h
)
SOURCE_GROUP("Features" FILES ${Features_SRCS})

SET(DrawingModule_SRCS
    AppDrawing.cpp
    AppDrawingPy.cpp
    PreCompiled.cpp
    PreCompiled.h
)
SOURCE_GROUP("Mod" FILES ${DrawingModule_SRCS})

SET(DrawingGeometry_SRCS
    Geometry.cpp
    Geometry.h
    GeometryObject.cpp
    GeometryObject.h
)
SOURCE_GROUP("Geometry" FILES ${DrawingGeometry_SRCS})

SET(DrawingAlgos_SRCS
    DrawingExport.cpp
    DrawingExport.h
    FeatureProjection.cpp
    FeatureProjection.h
    ProjectionAlgos.cpp
    ProjectionAlgos.h
)
SOURCE_GROUP("Algorithms" FILES ${DrawingAlgos_SRCS})

SET(Python_SRCS
    FeatureTemplatePy.xml
    FeatureTemplatePyImp.cpp
    FeatureParametricTemplatePy.xml
    FeatureParametricTemplatePyImp.cpp
)
SOURCE_GROUP("Python" FILES ${Python_SRCS})


SET(Drawing_SRCS
    ${DrawingModule_SRCS}
    ${Features_SRCS}
    ${DrawingGeometry_SRCS}
    ${Python_SRCS}
    ${DrawingAlgos_SRCS}
)

SET(Drawing_Templates
    Templates/A3_Landscape.svg
    Templates/A4_Landscape.svg
    Templates/A4_Simple.svg
)

add_library(Drawing SHARED ${Drawing_SRCS})
target_link_libraries(Drawing ${Drawing_LIBS})


if(MSVC)
    add_definitions(-D_PreComp_)
    GET_MSVC_PRECOMPILED_SOURCE("PreCompiled.cpp" Drawing_CPP_SRCS ${Drawing_SRCS} ${Features_SRCS} ${DrawingAlgos_SRCS})
    ADD_MSVC_PRECOMPILED_HEADER("PreCompiled.h" "PreCompiled.cpp" Drawing_CPP_SRCS)
endif(MSVC)

# Set special compiler flag to convert a SIGSEV into an exception
# to handle issue #0000478.
IF(MSVC)
SET_SOURCE_FILES_PROPERTIES(ProjectionAlgos.cpp PROPERTIES COMPILE_FLAGS "/EHa")
ENDIF(MSVC)

SET(Drawing_Scripts
    Init.py
    DrawingAlgos.py
)

fc_target_copy_resource(Drawing
    ${CMAKE_SOURCE_DIR}/src/Mod/Drawing
    ${CMAKE_BINARY_DIR}/Mod/Drawing
    ${Drawing_Scripts})

fc_target_copy_resource(Drawing
    ${CMAKE_SOURCE_DIR}/src/Mod/Drawing
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}/Mod/Drawing
    ${Drawing_Templates})

if(MSVC)
    set_target_properties(Drawing PROPERTIES SUFFIX ".pyd")
    set_target_properties(Drawing PROPERTIES DEBUG_OUTPUT_NAME "Drawing_d")
    set_target_properties(Drawing PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Drawing)
    set_target_properties(Drawing PROPERTIES PREFIX "../")
elseif(MINGW)
    set_target_properties(Drawing PROPERTIES SUFFIX ".pyd")
    set_target_properties(Drawing PROPERTIES DEBUG_OUTPUT_NAME "Drawing_d")
    set_target_properties(Drawing PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Drawing)
    set_target_properties(Drawing PROPERTIES PREFIX "")
else(MSVC)
    set_target_properties(Drawing PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Mod/Drawing)
    set_target_properties(Drawing PROPERTIES PREFIX "")
    set_target_properties(Drawing PROPERTIES INSTALL_RPATH ${INSTALL_RPATH})
endif(MSVC)

INSTALL(TARGETS Drawing DESTINATION lib)
