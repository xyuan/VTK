# javac runs out memory on Mac so increase heapsize
if(APPLE)
  set(JAVAC_OPTIONS -J-Xmx256m)
endif()

foreach(module ${VTK_MODULES_ENABLED})
  if(NOT VTK_MODULE_${module}_EXCLUDE_FROM_WRAPPING)
    include(${${module}_BINARY_DIR}/JavaDependencies.cmake OPTIONAL)
    list(APPEND _MODULES_TO_WRAP ${module})
  endif()
endforeach()

if(NOT VTK_JAR_PATH)
  set(VTK_JAR_PATH "${LIBRARY_OUTPUT_PATH}")
endif()

set(VTK_JAVA_SOURCE_FILES ${VTK_JAVA_DEPENDENCIES})
string(REGEX REPLACE "\\.java" ".class;" VTK_JAVA_CLASS_FILES ${VTK_JAVA_SOURCE_FILES})

# Setup custom targets
add_custom_target(VTKJavaTestClasses ALL DEPENDS ${VTK_BINARY_DIR}/java/javac_tests_stamp.txt)
add_custom_target(VTKJavaJar ALL DEPENDS ${VTK_JAR_PATH}/vtk.jar)
add_custom_target(VTKJavaClasses ALL DEPENDS ${VTK_BINARY_DIR}/java/javac_stamp.txt)

add_dependencies(VTKJavaJar VTKJavaClasses)
add_dependencies(VTKJavaTestClasses VTKJavaJar)

# Some platforms have libjvm in a subdirectory from
# the rest of the java libraries and are missing the symlink.

if(JAVA_AWT_LIBRARY)
  foreach(__java_library ${JAVA_AWT_LIBRARY})
    get_filename_component(JAVA_LIB_DIR ${__java_library} PATH)
    if(EXISTS ${JAVA_LIB_DIR}/xawt)
      link_directories(${JAVA_LIB_DIR}/xawt)
    endif()
    if(EXISTS ${JAVA_LIB_DIR}/client)
       link_directories(${JAVA_LIB_DIR}/client)
    endif()
    if(EXISTS ${JAVA_LIB_DIR}/server)
      link_directories(${JAVA_LIB_DIR}/server)
    endif()
  endforeach()
endif()

set(CMAKE_SKIP_RPATH 0)
include_directories(${vtkCommonCore_SOURCE_DIR})
include_directories(${vtkCommonCore_BINARY_DIR})
add_executable(VTKJavaExecutable VTKJava.cxx)
set(JAVA_LIBRARIES)

foreach(module ${_MODULES_TO_WRAP})

  string(REGEX REPLACE "^vtk" "" _module_no_prefix "${module}")

  if(APPLE)
    set(src ${VTK_JAR_PATH}/libvtk${_module_no_prefix}Java.dylib)
    set(tgt ${VTK_JAR_PATH}/libvtk${_module_no_prefix}Java.jnilib)
    add_custom_command(OUTPUT ${tgt}
                       COMMAND ln
                       ARGS -sf ${src} ${tgt}
                       DEPENDS ${src})
    set(JAVA_LIBRARIES ${JAVA_LIBRARIES} ${tgt})
  endif()

  target_link_libraries(VTKJavaExecutable ${module}Java)

  add_custom_target(VTKJava${_module_no_prefix} ALL)
  add_dependencies(VTKJava${_module_no_prefix} VTKJavaExecutable)
  add_dependencies(VTKJava${_module_no_prefix} ${module}JavaJavaClasses)
  add_dependencies(VTKJavaJar VTKJava${_module_no_prefix})
  # Make sure all .java files are build before any .class files are built.
  foreach(otherModule ${_MODULES_TO_WRAP})
    add_dependencies(VTKJava${_module_no_prefix} ${otherModule}JavaJavaClasses)
  endforeach()

  # Make sure all the VTKJava${kit} projects build *before* VTKJavaClasses.
  add_dependencies(VTKJavaClasses VTKJava${_module_no_prefix})
  add_dependencies(VTKJavaClasses ${module}Java)
endforeach()

# KITS is used in vtkSettings.java.in ( module name no prefix )
string(REGEX REPLACE "vtk" "" KITS "${_MODULES_TO_WRAP}")
configure_file(${VTK_SOURCE_DIR}/Wrapping/Java/vtk/vtkSettings.java.in
               ${VTK_BINARY_DIR}/java/vtk/vtkSettings.java)

# Add to class list so classes get cleaned up
list(APPEND VTK_JAVA_CLASS_FILES ${VTK_BINARY_DIR}/java/vtk/vtkSettings.class)

set(VTK_JAVA_DEPENDENCIES
  CellType
  vtkReferenceInformation
  vtkJavaGarbageCollector
  vtkJavaMemoryManager
  vtkJavaMemoryManagerImpl
  vtkNativeLibrary
  vtkSettings
  )

set(VTK_JAVA_SAMPLE_DEPENDENCIES)

if(vtkRenderingCore_ENABLED)
  set(VTK_JAVA_DEPENDENCIES ${VTK_JAVA_DEPENDENCIES}
    vtkJavaTesting
    AxesActor
    vtkRenderWindowPanel
    )
endif()

if(vtkInteractionWidgets_ENABLED)
  set(VTK_JAVA_DEPENDENCIES ${VTK_JAVA_DEPENDENCIES}
    vtkCanvas
    )
endif()

if(vtkRenderingCore_ENABLED AND vtkIOImage_ENABLED)
  set(VTK_JAVA_DEPENDENCIES ${VTK_JAVA_DEPENDENCIES}
    vtkPanel
    )
  set(VTK_JAVA_SAMPLE_DEPENDENCIES ${VTK_JAVA_SAMPLE_DEPENDENCIES}
    InternalFrames
    Demo
    SimpleVTK
    VTKCanvas
    ReleaseVtkPanel
    )
endif()

if(vtkFiltersModeling_ENABLED AND vtkIOImage_ENABLED)
  set(VTK_JAVA_SAMPLE_DEPENDENCIES ${VTK_JAVA_SAMPLE_DEPENDENCIES}
    ImagePlaneWidget
    )
endif()

# Copy over the sample Java files
foreach(jfile ${VTK_JAVA_SAMPLE_DEPENDENCIES})
  set(src ${VTK_SOURCE_DIR}/Wrapping/Java/vtk/sample/${jfile}.java)
  set(dst ${VTK_JAVA_HOME}/sample/${jfile}.java)
  list(APPEND VTK_JAVA_SOURCE_FILES ${dst})
  set(class ${VTK_JAVA_HOME}/sample/${jfile}.class)
  # Add to class list so classes get cleaned up
  list(APPEND VTK_JAVA_CLASS_FILES ${class})
  configure_file(${src} ${dst} COPYONLY)
endforeach()

# Copy over the handcrafted Java classes
foreach(jfile ${VTK_JAVA_DEPENDENCIES})
  set(src ${VTK_SOURCE_DIR}/Wrapping/Java/vtk/${jfile}.java)
  set(dst ${VTK_JAVA_HOME}/${jfile}.java)
  list(APPEND VTK_JAVA_SOURCE_FILES ${dst})
  set(class ${VTK_JAVA_HOME}/${jfile}.class)
  # Add to class list so classes get cleaned up
  list(APPEND VTK_JAVA_CLASS_FILES ${class})
  if(jfile MATCHES "vtkSettings")
  else()
    configure_file(${src} ${dst} COPYONLY)
  endif()
endforeach()

# Compile the Java classes
add_custom_command(
  OUTPUT ${VTK_BINARY_DIR}/java/javac_stamp.txt
  DEPENDS ${VTK_JAVA_SOURCE_FILES}
  COMMAND ${JAVA_COMPILE} ${JAVAC_OPTIONS}
    -source 5 -classpath ${VTK_JAVA_HOME}/.. -sourcepath ${VTK_SOURCE_DIR}/Wrapping/Java/ -d ${VTK_BINARY_DIR}/java
    ${VTK_BINARY_DIR}/java/vtk/*.java
  COMMAND ${CMAKE_COMMAND} -E touch ${VTK_BINARY_DIR}/java/javac_stamp.txt
  COMMENT "Compiling Java Classes"
  )

# Create the JAR file containing all compiled classes and
add_custom_command(
  COMMAND ${JAVA_ARCHIVE} -cvf  "${VTK_JAR_PATH}/vtk.jar"
  -C ${VTK_BINARY_DIR}/java
  vtk
  DEPENDS ${VTK_BINARY_DIR}/java/javac_stamp.txt ${JAVA_LIBRARIES}
  OUTPUT ${VTK_JAR_PATH}/vtk.jar
  COMMENT "Java Archive"
  )

if(BUILD_TESTING)
  if(WIN32)
    set(SEPARATOR "\;")
  else()
    set(SEPARATOR ":")
  endif()

  foreach(jfile
    Regression
    ConcurrencyGC
    JavaDelete
    ManualGC
    JavaGCAndDelete)
    list(APPEND VTK_JAVA_TESTS ${VTK_SOURCE_DIR}/Wrapping/Java/vtk/test/${jfile}.java)
    # Add to class list so classes get cleaned up
    list(APPEND VTK_JAVA_CLASS_FILES ${VTK_BINARY_DIR}/Wrapping/Java/vtk/test/${jfile}.class)
  endforeach()

  # Compile Java Tests
  add_custom_command(
    OUTPUT ${VTK_BINARY_DIR}/java/javac_tests_stamp.txt
    COMMAND ${JAVA_COMPILE} ${JAVAC_OPTIONS} -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
    -sourcepath ${VTK_SOURCE_DIR}/Wrapping/Java
    -d ${VTK_BINARY_DIR}/Wrapping/Java
    ${VTK_JAVA_TESTS}
    COMMAND ${CMAKE_COMMAND} -E touch ${VTK_BINARY_DIR}/java/javac_tests_stamp.txt
    DEPENDS ${VTK_JAR_PATH}/vtk.jar ${VTK_JAVA_TESTS}
    COMMENT "Compiling Java Tests"
    )

  # Since vtkTesting is within vtkTestingRendering we need blockers
  if(vtkTestingRendering_ENABLED)

    # Require Rendering
    if(vtkRenderingOpenGL_ENABLED)
      # If JAVA_DATAMODEL_FLAG is set, pass it as first argument. Otherwise, it's empty.
      # For example, use "-d64" to force 64-bit java jvm.
      #
      add_test(JavaRegression ${JAVA_RUNTIME} ${VTK_TEST_JAVA_DATAMODEL_FLAG}
         -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
         vtk.test.Regression -D ${VTK_DATA_ROOT}
         -V Baseline/Graphics/Cone.png
         -T ${VTK_TEST_OUTPUT_DIR}
         )
    endif()

    add_test(JavaGCRegression ${JAVA_RUNTIME} ${VTK_TEST_JAVA_DATAMODEL_FLAG}
      -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
      vtk.test.ConcurrencyGC
      -T ${VTK_TEST_OUTPUT_DIR}
      )

    add_test(JavaDeleteRegression ${JAVA_RUNTIME} ${VTK_TEST_JAVA_DATAMODEL_FLAG}
      -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
      vtk.test.JavaDelete
     -T ${VTK_TEST_OUTPUT_DIR}
     )

    add_test(JavaManualGC ${JAVA_RUNTIME} ${VTK_TEST_JAVA_DATAMODEL_FLAG}
      -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
      vtk.test.ManualGC
      -T ${VTK_TEST_OUTPUT_DIR}
      )

    add_test(JavaDeleteAndGC ${JAVA_RUNTIME} ${VTK_TEST_JAVA_DATAMODEL_FLAG}
      -classpath ${VTK_JAR_PATH}/vtk.jar${SEPARATOR}${VTK_BINARY_DIR}/Wrapping/Java
      vtk.test.JavaGCAndDelete
     -T ${VTK_TEST_OUTPUT_DIR}
     )
  endif()
endif()

# Make sure .class files are cleaned up
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${VTK_JAVA_CLASS_FILES}")
#  IF(NOT VTK_INSTALL_NO_RUNTIME)
#    INSTALL(FILES
#      ${VTK_JAR_PATH}/vtk.jar
#      DESTINATION ${VTK_INSTALL_JAVA_DIR_CM24}
#      COMPONENT RuntimeLibraries)
#  ENDIF(NOT VTK_INSTALL_NO_RUNTIME)
