add_library(toms917 STATIC
    toms917.cpp
)

# Need to build with -fPIC flag on Linux
set_target_properties(toms917 PROPERTIES
    POSITION_INDEPENDENT_CODE TRUE)
