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

#ifndef HIWIN_ROBOT_CLIENT_LIBRARY_EVENT_CB_HPP_
#define HIWIN_ROBOT_CLIENT_LIBRARY_EVENT_CB_HPP_

#include <hiwin_robot_client_library/socket/tcp_client.hpp>

namespace hrsdk
{
class EventCb : public socket::TCPClient
{
private:
  std::string robot_ip_;
  int port_;

public:
  EventCb(const std::string& robot_ip, const int port)
  {
    robot_ip_ = robot_ip;
    port_ = port;
  }

  ~EventCb()
  {
  }

  bool connect()
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
};
}  // namespace hrsdk

#endif  // HIWIN_ROBOT_CLIENT_LIBRARY_EVENT_CB_HPP_
