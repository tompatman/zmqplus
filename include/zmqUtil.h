//
// Created by talexander on 3/18/19.
//

#pragma once

#include <unistd.h>
#include <algorithm>
#include <Poco/Types.h>
#include <Poco/NumberFormatter.h>
#include <string>

namespace ZMQ {

static constexpr Poco::UInt64 INIT_SLEEP_TIME_MS = 15;

class zmqUtil {

public:
///If the socket is using ipc, use the hostname to guarantee a unique ipc path
    static std::string generateSocketFullPath(const std::string &strSocket, const Poco::UInt16 &channel)
    {
      std::string strFullPath;

      if (strSocket.find("ipc://") != std::string::npos)
      {
        if(zmqUtil::_device_id.empty())
        {
          char hostName[128] = {0};
          gethostname(hostName, sizeof(hostName));
          _device_id = hostName;
        }
        strFullPath = strSocket + std::string(_device_id) + Poco::NumberFormatter::format(channel);
      }
      else
      {
        strFullPath = strSocket + Poco::NumberFormatter::format(channel);
      }

      return (strFullPath);
    };

    static std::string generateFullTopic(const std::string &strTopicPrefix, const Poco::UInt16 &channel)
    {
      return (strTopicPrefix + "_" + Poco::NumberFormatter::format(channel));
    }

    static void generateIPCConversion(std::string &strSocket)
    {
      ///If the socket is a tcp type, remove the tcp prefix
      // Search for the substring in string
      size_t namePos = strSocket.find("tcp://");

      if (namePos != std::string::npos)
      {
        // If found then erase it from string
        strSocket.erase(namePos, strlen("tcp://"));
        strSocket.insert(namePos, "ipc:///var/volatile/tmp/ipc");
      }

      namePos = strSocket.find("ipc://");

      ///Replace * with 'localhost'
      ///Replace 127.0.0.1 with localhost
      std::string toReplace("*");
      size_t posLocalHost = strSocket.find(toReplace);
      if (posLocalHost != std::string::npos)
      {
        strSocket.replace(posLocalHost, toReplace.length(), "localhost");
      }
      toReplace = "127.0.0.1";
      posLocalHost = strSocket.find(toReplace);
      if (posLocalHost != std::string::npos)
      {
        strSocket.replace(posLocalHost, toReplace.length(), "localhost");
      }
      ///Replace 0.0.0.0 with localhost
      toReplace = "0.0.0.0";
      posLocalHost = strSocket.find(toReplace);
      if (posLocalHost != std::string::npos)
      {
        strSocket.replace(posLocalHost, toReplace.length(), "localhost");
      }
      ///Replace bms-server with localhost
      toReplace = "bms-server";
      posLocalHost = strSocket.find(toReplace);
      if (posLocalHost != std::string::npos)
      {
        strSocket.replace(posLocalHost, toReplace.length(), "localhost");
      }
      ///Replace . with '_'
      std::replace(strSocket.begin(), strSocket.end(), '.', '_');
      ///Replace : with 'p' after ipc://
      std::replace(strSocket.begin() + (namePos + strlen("ipc://")), strSocket.end(), ':', 'p');


    }

    static void setDeviceId(std::string dev_id)
    {
      _device_id = dev_id;
    }
    static std::string _device_id;

};

}
