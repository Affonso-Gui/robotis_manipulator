﻿/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Darby Lim, Hye-Jong KIM */

#include "robotis_manipulator/RMTrajectory.h"

using namespace RM_TRAJECTORY;
using namespace Eigen;

MinimumJerk::MinimumJerk()
{
  coefficient_ = VectorXf::Zero(6);
}

MinimumJerk::~MinimumJerk() {}

void MinimumJerk::calcCoefficient(Trajectory start,
                                  Trajectory goal,
                                  double move_time,
                                  double control_time)
{
  uint16_t step_time = uint16_t(floor(move_time / control_time) + 1.0);
  move_time = double(step_time - 1) * control_time;

  Matrix3f A = Matrix3f::Identity(3, 3);
  Vector3f x = Vector3f::Zero();
  Vector3f b = Vector3f::Zero();

  A << pow(move_time, 3), pow(move_time, 4), pow(move_time, 5),
      3 * pow(move_time, 2), 4 * pow(move_time, 3), 5 * pow(move_time, 4),
      6 * pow(move_time, 1), 12 * pow(move_time, 2), 20 * pow(move_time, 3);

  coefficient_(0) = start.position;
  coefficient_(1) = start.velocity;
  coefficient_(2) = 0.5 * start.acceleration;

  b << (goal.position - start.position - (start.velocity * move_time + 0.5 * start.acceleration * pow(move_time, 2))),
      (goal.velocity - start.velocity - (start.acceleration * move_time)),
      (goal.acceleration - start.acceleration);

  ColPivHouseholderQR<Matrix3f> dec(A);
  x = dec.solve(b);

  coefficient_(3) = x(0);
  coefficient_(4) = x(1);
  coefficient_(5) = x(2);
}

VectorXf MinimumJerk::getCoefficient()
{
  return coefficient_;
}

//-------------------- Joint trajectory --------------------//

JointTrajectory::JointTrajectory(uint8_t joint_num)
{
  joint_num_ = joint_num;
  coefficient_ = MatrixXf::Identity(6, joint_num);
  position_.reserve(joint_num);
  velocity_.reserve(joint_num);
  acceleration_.reserve(joint_num);
}

JointTrajectory::~JointTrajectory() {}

void JointTrajectory::init(std::vector<Trajectory> start,
                           std::vector<Trajectory> goal,
                           double move_time,
                           double control_time)
{
  for (uint8_t index = 0; index < start.size(); index++)
  {
    trajectory_generator_.calcCoefficient(start.at(index),
                                    goal.at(index),
                                    move_time,
                                    control_time);

    coefficient_.col(index) = trajectory_generator_.getCoefficient();
  }
}

std::vector<double> JointTrajectory::getPosition(double tick)
{
  position_.clear();
  for (uint8_t index = 0; index < joint_num_; index++)
  {
    double result = 0.0;
    result = coefficient_(0, index) +
             coefficient_(1, index) * pow(tick, 1) +
             coefficient_(2, index) * pow(tick, 2) +
             coefficient_(3, index) * pow(tick, 3) +
             coefficient_(4, index) * pow(tick, 4) +
             coefficient_(5, index) * pow(tick, 5);

    position_.push_back(result);
  }

  return position_;
}

std::vector<double> JointTrajectory::getVelocity(double tick)
{
  velocity_.clear();
  for (uint8_t index = 0; index < joint_num_; index++)
  {
    double result = 0.0;
    result = coefficient_(1, index) +
             2 * coefficient_(2, index) * pow(tick, 1) +
             3 * coefficient_(3, index) * pow(tick, 2) +
             4 * coefficient_(4, index) * pow(tick, 3) +
             5 * coefficient_(5, index) * pow(tick, 4);

    velocity_.push_back(result);
  }

  return velocity_;
}

std::vector<double> JointTrajectory::getAcceleration(double tick)
{
  acceleration_.clear();
  for (uint8_t index = 0; index < joint_num_; index++)
  {
    double result = 0.0;

    result = 2 * coefficient_(2, index) +
             6 * coefficient_(3, index) * pow(tick, 1) +
             12 * coefficient_(4, index) * pow(tick, 2) +
             20 * coefficient_(5, index) * pow(tick, 3);

    acceleration_.push_back(result);
  }

  return acceleration_;
}

MatrixXf JointTrajectory::getCoefficient()
{
  return coefficient_;
}

//-------------------- Task trajectory --------------------//

TaskTrajectory::TaskTrajectory()
{
  num_of_axis_ = 3;
  coefficient_ = MatrixXf::Identity(6, num_of_axis_);
  position_.reserve(num_of_axis_);
  velocity_.reserve(num_of_axis_);
  acceleration_.reserve(num_of_axis_);
}

TaskTrajectory::~TaskTrajectory() {}

void TaskTrajectory::init(std::vector<Trajectory> start,
                           std::vector<Trajectory> goal,
                           double move_time,
                           double control_time)
{
  for (uint8_t index = 0; index < start.size(); index++)
  {
    trajectory_generator_.calcCoefficient(start.at(index),
                                    goal.at(index),
                                    move_time,
                                    control_time);

    coefficient_.col(index) = trajectory_generator_.getCoefficient();
  }
}

std::vector<double> TaskTrajectory::getPosition(double tick)
{
  position_.clear();
  for (uint8_t index = 0; index < num_of_axis_; index++)
  {
    double result = 0.0;
    result = coefficient_(0, index) +
             coefficient_(1, index) * pow(tick, 1) +
             coefficient_(2, index) * pow(tick, 2) +
             coefficient_(3, index) * pow(tick, 3) +
             coefficient_(4, index) * pow(tick, 4) +
             coefficient_(5, index) * pow(tick, 5);

    position_.push_back(result);
  }

  return position_;
}

std::vector<double> TaskTrajectory::getVelocity(double tick)
{
  velocity_.clear();
  for (uint8_t index = 0; index < num_of_axis_; index++)
  {
    double result = 0.0;
    result = coefficient_(1, index) +
             2 * coefficient_(2, index) * pow(tick, 1) +
             3 * coefficient_(3, index) * pow(tick, 2) +
             4 * coefficient_(4, index) * pow(tick, 3) +
             5 * coefficient_(5, index) * pow(tick, 4);

    velocity_.push_back(result);
  }

  return velocity_;
}

std::vector<double> TaskTrajectory::getAcceleration(double tick)
{
  acceleration_.clear();
  for (uint8_t index = 0; index < num_of_axis_; index++)
  {
    double result = 0.0;

    result = 2 * coefficient_(2, index) +
             6 * coefficient_(3, index) * pow(tick, 1) +
             12 * coefficient_(4, index) * pow(tick, 2) +
             20 * coefficient_(5, index) * pow(tick, 3);

    acceleration_.push_back(result);
  }

  return acceleration_;
}

MatrixXf TaskTrajectory::getCoefficient()
{
  return coefficient_;
}
