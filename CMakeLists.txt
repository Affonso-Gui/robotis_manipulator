﻿################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 2.8.3)
project(robotis_manipulator)

add_compile_options(-std=c++11)

################################################################################
# Find catkin packages and libraries for catkin and system dependencies
################################################################################
find_package(catkin REQUIRED COMPONENTS
    roscpp
)
find_package(Eigen3 REQUIRED)

################################################################################
# Setup for python modules and scripts
################################################################################

################################################################################
# Declare ROS messages, services and actions
################################################################################

################################################################################
## Declare ROS dynamic reconfigure parameters
################################################################################

################################################################################
# Declare catkin specific configuration to be passed to dependent projects
################################################################################
catkin_package(
  INCLUDE_DIRS include
  DEPENDS EIGEN3
)

################################################################################
# Build
################################################################################
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(robotis_manipulator
  src/robotis_manipulator.cpp
  src/robotis_manipulator_common.cpp
  src/robotis_manipulator_debug.cpp
  src/robotis_manipulator_trajectory_generator.cpp
  src/robotis_manipulator_manager.cpp
  src/robotis_manipulator_math.cpp
)

add_dependencies(robotis_manipulator ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(robotis_manipulator ${catkin_LIBRARIES})
