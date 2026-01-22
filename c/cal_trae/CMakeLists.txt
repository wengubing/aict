cmake_minimum_required(VERSION 3.16)

# Project definition
project(CalculatorWinGUI C)

# Enable folders (useful in VS)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sources
set(SRC
    calculator.c
)

# Create a Windows GUI executable (no console)
add_executable(calculator WIN32 ${SRC})

# Ensure C standard
set_target_properties(calculator PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
)

# Include directories (optional if using system headers only)
# target_include_directories(calculator PRIVATE ${CMAKE_SOURCE_DIR})

# Link required Windows libraries
# comctl32: common controls, gdi32: graphics, comdlg32: common dialogs
# user32 is pulled implicitly, but adding it is safe.
target_link_libraries(calculator PRIVATE
    comctl32
    gdi32
    comdlg32
    user32
)

# For MinGW/GCC ensure subsystem is GUI (WIN32 above already sets it),
# but some MinGW setups may still show a console; enforce via linker flags if needed.
# Uncomment if you see a console window:
# if(MINGW)
#   target_link_options(calculator PRIVATE -mwindows)
# endif()

# Runtime output directory
set_target_properties(calculator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Debugging: generate symbols
if(MSVC)
    # MSVC generates PDB automatically in Debug
else()
    target_compile_options(calculator PRIVATE -g)
endif()
