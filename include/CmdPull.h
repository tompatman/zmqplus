/*
 * CmdPull.h
 *
 *  Created on: Sep 18, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include "zmq.hpp"
#include "zCmdStruct.h"
#include "core/ppcLogger.h"
#include "core/systimer.h"
#include <zmqUtil.h>

extern bool g_ForceIPC;
namespace ZMQ {
template<typename msgData>
class CmdPull {
public:
    //The channel is a string identifier used to identify which messages to monitor, like an address

    CmdPull(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log,
            const std::string sock_str = "tcp://0.0.0.0:") //, const Poco::Int16 chanID = -1)
            : _uiPort(uiPort)
              , _log(log)
              , _sock_str(sock_str + Poco::NumberFormatter::format(uiPort))
    {
      //_chanID = chanID;
      _subscriber = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(context, ZMQ_PULL));

    };

    virtual ~CmdPull()
    {
      _subscriber->close();
    };

    void Init(const bool bConflate = false, const Poco::UInt16 queueSize = 100)
    {

      if (g_ForceIPC)
      {
        zmqUtil::generateIPCConversion(_sock_str);
      }
      _log.log(LG_DEBUG, "Creating a PULL socket at %s", _sock_str.c_str());

      _subscriber->bind(_sock_str);

      #ifndef NDEBUG
      int timeout_ms = zmq::READ_WAIT_MS;
      _subscriber->setsockopt(ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms));
      #endif

      Poco::Thread::sleep(INIT_SLEEP_TIME_MS);

      //Poco::Thread::sleep(10);   // -> GM on 3/7/19 removed to speed unit test
    };


    bool readCommand(msgData &recvData)
    {

      //  Read envelope with address
      zmq::message_t message;
      Poco::UInt16 uiChan = 0;

      #ifdef NDEBUG
      _subscriber->recv(&message, ZMQ_NOBLOCK);
      #else
      //_subscriber->recv(&message);
      _subscriber->recv(&message, ZMQ_NOBLOCK);
      #endif

      if (message.size() == 0)
      {
        return (false);
      };

      if (message.size() != sizeof(recvData))
      {
          throw Poco::InvalidAccessException("CmdPull::readCommand() message size is incorrect, received " +
                                             Poco::NumberFormatter::format(message.size()) + " bytes, expected " +
                                             Poco::NumberFormatter::format(sizeof(recvData)) + ".");
      }

      memcpy(&_incomingCmd, message.data(), message.size());
      memcpy(&recvData, message.data(), message.size());

      Poco::Thread::yield(); 

      return (true);

    }

    bool readData(msgData &newData)
    {

      //  Read envelope with address
      zmq::message_t message;
      Poco::UInt16 uiChan = 0;

      _subscriber->recv(&message, ZMQ_NOBLOCK);

      if (message.size() == 0)
      {
        return (false);
      };

      if (message.size() != sizeof(msgData))
      {
        throw Poco::InvalidAccessException("CmdPull::readData() message size is incorrect, received " +
                                           Poco::NumberFormatter::format(message.size()) + " bytes, expected " +
                                           Poco::NumberFormatter::format(sizeof(msgData)) + ".");
      }

      memcpy(&newData, message.data(), message.size());

      Poco::Thread::yield();

      return (true);

    }


private:
    std::unique_ptr<zmq::socket_t> _subscriber;
    msgData _incomingCmd;
    const Poco::UInt16 _uiPort;

    ppcLogger &_log;
    std::string _sock_str;
};

}

