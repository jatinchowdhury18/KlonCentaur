juce_add_console_app(Centaur_Headless
    PRODUCT_NAME "chow_centaur"
    COMPANY_NAME chowdsp
)

juce_generate_juce_header(Centaur_Headless)
add_custom_command(TARGET Centaur_Headless
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "copying $<TARGET_FILE:Centaur_Headless> to ${PROJECT_BINARY_DIR}/ChowCentaurHeadless"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Centaur_Headless> ${PROJECT_BINARY_DIR}/ChowCentaurHeadless)


target_sources(Centaur_Headless PRIVATE
    main.cpp
)

target_precompile_headers(Centaur_Headless PRIVATE ../pch.h)
target_include_directories(Centaur_Headless PRIVATE ../)
target_include_directories(Centaur_Headless PRIVATE ../../modules/JUCE/modules)
target_include_directories(Centaur_Headless PRIVATE ../../modules)
target_include_directories(Centaur_Headless PRIVATE ../../modules/RTNeural/modules/xsimd/include)

target_link_libraries(Centaur_Headless PUBLIC
    BinaryData
    ChowCentaur
    warning_flags
)

set_target_properties(Centaur_Headless PROPERTIES CXX_VISIBILITY_PRESET hidden)
