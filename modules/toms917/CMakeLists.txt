add_library(toms917 STATIC
    toms917.cpp
)

target_compile_definitions(toms917 PRIVATE
    _USE_MATH_DEFINES
)

# Need to build with -fPIC flag on Linux
set_target_properties(toms917 PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
