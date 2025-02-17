// Copyright 2024 HIWIN Technologies Corp.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the {copyright_holder} nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <cmath>

#include <hiwin_robot_client_library/commander.hpp>

namespace hrsdk
{

enum class SpaceOperationTypes
{
  Cartesian = 0,
  Joint,
  Tool,
};

enum class CommandType
{
  Get = 0,
  Set,
  MonitorSet,
};

enum class CommandId : uint16_t
{
  GetPermissions = 0x000A,
  SetPtpSpeed = 0x0096,
  GetPtpSpeed = 0x0098,
  SetOverRideRatio = 0x012C,
  GetOverRideRatio = 0x012D,
  SetServoAmp = 0x0578,
  GetServoAmp = 0x0579,
  GetRobotVersion = 0x057A,
  SetRobotMode = 0x058C,
  GetRobotMode = 0x058D,
  ControllerReset = 0x05AA,
  PtpJoint = 0x07D2,
  PtpJointWithVelocity = 0x07D6,
  LinearSplinePoint = 0x07E8,
  CubicSplinePoint = 0x07E9,
  QuinticSplinePoint = 0x07EA,
  ExtPtpJoint = 0x07EF,
  MotionAbort = 0x07FA,
  GetExtActualRPM = 0x0863,
  GetExtActualPosition = 0x0864,
  GetActualPosition = 0x0866,
  GetActualRPM = 0x0867,
  GetErrorCode = 0x086C,
  GetMotionState = 0x086D,
  SetLogLevel = 0x1003,
  GetActualCurrent = 0x100A,
  GetHRSSVersion = 0x100B,
  GetHrssMode = 0x1036,
};

struct Commandformat
{
  uint16_t cmd_id;
  uint16_t param[249];
} __attribute__((__packed__));

struct Responseformat
{
  uint16_t cmd_id;
  uint16_t result;
  uint16_t data[248];
} __attribute__((__packed__));

Commander::Commander(const std::string& robot_ip, const int port) : robot_ip_(robot_ip), port_(port)
{
}

Commander::~Commander()
{
}

bool Commander::connect()
{
  if (getState() == socket::SocketState::Connected)
  {
    std::cout << "Socket is already connected. Refusing to reconnect." << std::endl;
    return false;
  }

  if (!TCPClient::setup(robot_ip_, port_, 2, std::chrono::seconds(5)))
  {
    return false;
  }

  return true;
}

bool Commander::isRemoteMode()
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetHrssMode);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return false;
  }

  uint16_t data_length = r.data[0];
  if (r.data[1] != 3)
  {
    return false;
  }

  return true;
}

int Commander::getPermissions()
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetPermissions);
  w.param[0] = 0;
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::setLogLevel(LogLevels level)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::SetLogLevel);
  w.param[0] = static_cast<uint16_t>(level);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::setServoAmpState(bool enable)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::SetServoAmp);
  w.param[0] = static_cast<uint16_t>(enable);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::getServoAmpState(bool& enable)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetServoAmp);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  enable = (r.data[1] > 0) ? true : false;
  return r.result;
}

int Commander::getActualRPM(double (&velocities)[6])
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetActualRPM);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  int32_t value;
  for (size_t i = 0; i < 6; i++)
  {
    memcpy(&value, ((data_r + 6) + (i * 4)), sizeof(int32_t));
    velocities[i] = value / 1000.0;
  }
  return r.result;
}

int Commander::getActualCurrent(double (&efforts)[6])
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetActualCurrent);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  int32_t value;
  for (size_t i = 0; i < 6; i++)
  {
    memcpy(&value, ((data_r + 6) + (i * 4)), sizeof(int32_t));
    efforts[i] = value / 1000.0;
  }
  return r.result;
}

int Commander::getExtActualRPM(double (&velocities)[3])
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetExtActualRPM);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  int32_t value;
  for (size_t i = 0; i < 3; i++)
  {
    memcpy(&value, ((data_r + 6) + (i * 4)), sizeof(int32_t));
    velocities[i] = value / 1000.0;
  }
  return r.result;
}

int Commander::getExtActualPosition(double (&positions)[3])
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetExtActualPosition);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  int32_t value;
  uint16_t data_length = r.data[0];
  for (size_t i = 0; i < 3; i++)
  {
    memcpy(&value, ((data_r + 6) + (i * 4)), sizeof(int32_t));
    positions[i] = (value / 1000.0) * (M_PI / 180);
  }
  return r.result;
}

int Commander::getActualPosition(double (&positions)[6])
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetActualPosition);
  w.param[0] = static_cast<uint16_t>(SpaceOperationTypes::Joint);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  int32_t value;
  uint16_t data_length = r.data[0];
  for (size_t i = 0; i < 6; i++)
  {
    memcpy(&value, ((data_r + 6) + (i * 4)), sizeof(int32_t));
    positions[i] = (value / 1000.0) * (M_PI / 180);
  }
  return r.result;
}

int Commander::getMotionState(MotionStatus& status)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetMotionState);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  status = static_cast<MotionStatus>(r.data[1]);
  return r.result;
}

int Commander::getErrorCode(std::vector<std::string>& error_list)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetErrorCode);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  uint16_t count = data_length >> 2;
  uint16_t first, second, thrid;
  char buffer[12];
  error_list.clear();
  for (size_t i = 0; i < count; i++)
  {
    first = r.data[i * 4 + 4] & 0x00FF;
    second = (r.data[i * 4 + 3] & 0xFF00) >> 8;
    thrid = (r.data[i * 4 + 3] & 0x00FF);

    sprintf(buffer, "Err%02x-%02x-%02x", first, second, thrid);
    error_list.push_back(std::string(buffer));
  }
  return r.result;
}

int Commander::ptpJoint(double* positions)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::PtpJoint);

  // Smooth on between the points
  w.param[0] = 1;

  std::string pos_str = "";
  for (size_t i = 0; i < 6; i++)
  {
    if (i != 0)
    {
      pos_str += ",";
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(7) << (positions[i] * (180 / M_PI));
    pos_str += stream.str();
  }
  w.param[1] = pos_str.length();
  std::copy(pos_str.begin(), pos_str.end(), &w.param[2]);

  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::ptpJoint(double* positions, double acc_time, double ratio)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::PtpJointWithVelocity);

  // Acceleration time
  int32_t acceleration_time = static_cast<int>(std::round(acc_time * 1000));
  memcpy(&w.param[0], &acceleration_time, sizeof(int32_t));

  // PTP motion ratio
  int32_t ratio_integer = static_cast<int>(std::round(ratio * 1000));
  memcpy(&w.param[2], &ratio_integer, sizeof(int32_t));

  // Smooth on between the points
  w.param[4] = 1;

  double pos_deg;
  int32_t deg_integer;
  for (size_t i = 0; i < 6; i++)
  {
    pos_deg = positions[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(pos_deg));
    memcpy(&w.param[(i * 2) + 5], &deg_integer, sizeof(int32_t));
  }
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::extPtpJoint(double* positions)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::ExtPtpJoint);

  // Smooth on between the points
  w.param[0] = 1;

  //
  std::string pos_str = "";
  for (size_t i = 0; i < 9; i++)
  {
    if (i != 0)
    {
      pos_str += ",";
    }
    std::stringstream stream;
    stream << std::fixed << std::setprecision(7) << (positions[i] * (180 / M_PI));
    pos_str += stream.str();
  }
  w.param[1] = pos_str.length();
  std::copy(pos_str.begin(), pos_str.end(), &w.param[2]);

  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::linearSplinePoint(const double* positions, double goal_time_sec)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::LinearSplinePoint);

  double pos_deg;
  int32_t deg_integer;
  for (size_t i = 0; i < 9; i++)
  {
    pos_deg = positions[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(pos_deg));
    memcpy(&w.param[(i * 2) + 0], &deg_integer, sizeof(int32_t));
  }

  int32_t t_integer = static_cast<int>(std::round(goal_time_sec * 1000.0));
  memcpy(&w.param[18], &t_integer, sizeof(int32_t));

  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::CubicSplinePoint(const double* positions, const double* velocities, double goal_time_sec)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::CubicSplinePoint);

  double deg_float;
  int32_t deg_integer;
  for (size_t i = 0; i < 9; i++)
  {
    deg_float = positions[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(deg_float));
    std::cout << deg_integer << " ";
    memcpy(&w.param[(i * 2) + 0], &deg_integer, sizeof(int32_t));
  }

  for (size_t i = 0; i < 9; i++)
  {
    deg_float = velocities[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(deg_float));
    memcpy(&w.param[(i * 2) + 18], &deg_integer, sizeof(int32_t));
  }

  int32_t t_integer = static_cast<int>(std::round(goal_time_sec * 1000.0));
  memcpy(&w.param[36], &t_integer, sizeof(int32_t));

  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::QuintSplinePoint(const double* positions, const double* velocities, const double* acceleration,
                                double goal_time_sec)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::QuinticSplinePoint);

  double deg_float;
  int32_t deg_integer;
  for (size_t i = 0; i < 9; i++)
  {
    deg_float = positions[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(deg_float));
    memcpy(&w.param[(i * 2) + 0], &deg_integer, sizeof(int32_t));
  }

  for (size_t i = 0; i < 9; i++)
  {
    deg_float = velocities[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(deg_float));
    memcpy(&w.param[(i * 2) + 18], &deg_integer, sizeof(int32_t));
  }

  for (size_t i = 0; i < 9; i++)
  {
    deg_float = acceleration[i] * (180 / M_PI) * 1000.0;
    deg_integer = static_cast<int>(std::round(deg_float));
    memcpy(&w.param[(i * 2) + 36], &deg_integer, sizeof(int32_t));
  }

  int32_t t_integer = static_cast<int>(std::round(goal_time_sec * 1000.0));
  memcpy(&w.param[54], &t_integer, sizeof(int32_t));

  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::motionAbort()
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::MotionAbort);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::clearError()
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::ControllerReset);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::setPtpSpeed(int ratio)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::SetPtpSpeed);
  w.param[0] = static_cast<uint16_t>(ratio);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::getPtpSpeed(int& ratio)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetPtpSpeed);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  ratio = static_cast<int>(r.data[1]);
  return r.result;
}

int Commander::setOverrideRatio(int ratio)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::SetOverRideRatio);
  w.param[0] = static_cast<uint16_t>(ratio);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::getOverrideRatio(int& ratio)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetOverRideRatio);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  ratio = static_cast<int>(r.data[1]);
  return r.result;
}

int Commander::setRobotMode(ControlMode mode)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::SetRobotMode);
  w.param[0] = static_cast<uint16_t>(mode);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  return r.result;
}

int Commander::getRobotMode(ControlMode& mode)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetRobotMode);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  mode = static_cast<ControlMode>(r.data[1]);
  return r.result;
}

int Commander::GetRobotVersion(std::string& str)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetRobotVersion);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  uint16_t data_length = r.data[0];
  uint16_t str_length = r.data[1];
  str = "";
  for (size_t i = 0; i < str_length; i++)
  {
    str += r.data[i + 2];
  }
  return r.result;
}

int Commander::GetHRSSVersion(std::string& str)
{
  size_t written;
  Commandformat w = {};
  const uint8_t* data_w = static_cast<const uint8_t*>(static_cast<void*>(&w));

  w.cmd_id = static_cast<uint16_t>(CommandId::GetHRSSVersion);
  TCPClient::write(data_w, sizeof(Commandformat), written);

  size_t read_chars;
  Responseformat r = {};
  uint8_t* data_r = static_cast<uint8_t*>(static_cast<void*>(&r));

  TCPClient::read(data_r, sizeof(Responseformat), read_chars);
  if (r.result != 0)
  {
    return r.result;
  }

  std::stringstream ss;
  ss << r.data[2] << "." << r.data[3] << "." << static_cast<char>(r.data[4]) << "_" << r.data[5];

  str = ss.str();

  return r.result;
}

}  // namespace hrsdk
