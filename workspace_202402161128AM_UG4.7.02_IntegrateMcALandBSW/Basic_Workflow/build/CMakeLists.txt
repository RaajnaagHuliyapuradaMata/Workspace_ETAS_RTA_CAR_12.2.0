######## Initialisation ########
# CMake Target settings
cmake_minimum_required(VERSION 3.9)
# Required to build for Hardware targets
# set(CMAKE_CROSSCOMPILING ON)
# set(CMAKE_SYSTEM_NAME "Generic")
# set(CMAKE_SYSTEM_PROCESSOR "*Target CPU*")

# Load CMakeGeneral.txt
include("CMakeGeneral.txt")
checkLoading()

# Variables used by CMakeGeneral.txt
set(TARGET_NAME "Basic_workflow")
set(LIBRARIES "RTAOS" "winmm" "ws2_32" "stdc++")

set(TARGET_LINK_DIRS "../src/os")

######## Compilation properties ########
set(UNITY_BUILD_FLAG "ON") # Speeds up compilation
# These files will not be added to the unity build (Usually due to dependencies)
set(UNITY_BUILD_BANNED_TAGS ".*/Dem/.*" ".*/Dcm/.*" ".*/rte/.*" ".*/asw/.*" ".*/CanTp_PC/.*" ".*/FiM/.*" ".*/Det/.*" ".*/EcuM/.*" ".*/*.cpp")
# These files should not be compiled and will be filtered out
set(BANNED_FILES "Idle.c" "Dem_KL15Persistence.c" "Dem_PrjSpecificConsistencyCheck.c" "Dem_PrjSpecificFailureMemory.c" "Dem_PrjSpecificIndiHandling.c")
# Recommended to bypass command length limits
set(CMAKE_C_USE_RESPONSE_FILE_FOR_LIBRARIES 1)
set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 1)
set(CMAKE_NINJA_FORCE_RESPONSE_FILE 1)
  
# These flags should be taken from the RTA-OS Port Guide pdf
set(CMAKE_C_FLAGS "-ggdb -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT -DOSENV_RTAOS40")
set(CMAKE_CXX_FLAGS "-ggdb -DNOT_READY_FOR_TESTING_OR_DEPLOYMENT -DOSENV_RTAOS40")

######## Linking/building properties ########
# Recommended linker settings
set(LINKER_SCRIPT "../link.ld") # Replace this with a target specific script
set(CMAKE_EXE_LINKER_FLAGS "-Xlinker -Map=output.map -static")
set(CMAKE_C_LINK_FLAGS "")

######## Continue to CMakeGeneral.txt ########
# Initialise the project
project(Basic_workflow)
# Search for the project source files
runTargetIndependentCmake()
