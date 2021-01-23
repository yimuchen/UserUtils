FUNCTION(MAKE_REFLEX PACKAGE)
  set(HEADER      "${CMAKE_HOME_DIRECTORY}/python/${PACKAGE}.hpp"  )
  set(SELECTION   "${CMAKE_HOME_DIRECTORY}/${PACKAGE}/src/${PACKAGE}_Selection.xml")
  set(CPP_OUTPUT  "${PACKAGE}_rflx.cpp"      )
  set(DICT_OUTPUT "${PACKAGE}_rflx_rdict.pcm")
  set(PYLIB "py${PACKAGE}")
  set(TARGET_NAME "${PACKAGE}_REFLEX"        )

  add_custom_target(${TARGET_NAME} ALL
                    DEPENDS ${PYLIB} ${DICT_OUTPUT})

  add_custom_command(
    OUTPUT   "${CPP_OUTPUT}" "${DICT_OUTPUT}"
    COMMAND   genreflex "${HEADER}"
                        "--selection" "${SELECTION}"
                        "-I${CMAKE_HOME_DIRECTORY}/include"
    DEPENDS   "${HEADER}" "${SELECTION}"
    WORKING_DIRECTORY "${CMAKE_HOME_DIRECTORY}/python")

  add_library(${PYLIB} SHARED ${CPP_OUTPUT})
  target_link_libraries(${PYLIB} ${ROOT_LIBRARIES} ${PACAKGE})
  set_target_properties(${PYLIB} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/python )
ENDFUNCTION()

MAKE_REFLEX(PlotUtils)
MAKE_REFLEX(MathUtils)
