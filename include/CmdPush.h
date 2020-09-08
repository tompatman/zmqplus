/*
 * CmdPush.h
 *
 *  Created on: Sep 18, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Thread.h>
#include <zconf.h>
#include <core/ppcLogger.h>
#include "zmq.hpp"
#include "IPublish.h"
#include "ICmdPush.h"
#include "zmqUtil.h"
#include <cstring>

extern bool g_ForceIPC;
namespace ZMQ {

template<typename msgData>
class CmdPush
        : public ICmdPush { //: public IPublish <reqMsgData>{

public:
    CmdPush(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strSocket = "tcp://0.0.0.0:")
            : _uiPort(uiPort)
              , _strSocket(strSocket)
              , _log(log)
    {
      _publisher = new zmq::socket_t(context, ZMQ_PUSH);

    };

    ~CmdPush() override
    {
      _publisher->close();
      delete _publisher;
    };

    //Virtual implementations
    void Init(const Poco::UInt16 queueSize = 30)
    {
      _strFullSocketPath = _strSocket + Poco::NumberFormatter::format(_uiPort);

      if (g_ForceIPC)
      {
        zmqUtil::generateIPCConversion(_strFullSocketPath);
      }
      _log.log(LG_DEBUG, "Creating a PUSH socket at %s", _strFullSocketPath.c_str());

      try
      {
        //  Prepare our context and publisher
        _publisher->connect(_strFullSocketPath);
        _publisher->setsockopt(ZMQ_LINGER, 0);
      }
      catch(std::exception &e)
      {
        _log.log(LG_INFO, "*** Failed to create a PUSH socket at %s", _strFullSocketPath.c_str());
        throw e;
      }

      // _requester->setsockopt(ZMQ_SNDHWM, &queueSize, sizeof(int));   Note: Would like to use this, but in release it creates an exception.

      // Poco::Thread::sleep(300);    Note: Should not be required.
    };

    /*
     * Send message with raw data only, no channel is associated with it
     */
    bool Send(const msgData &newData)
    {

      _mut.lock();
      bool rc = false;

      zmq::message_t message(sizeof(newData));
      memcpy(message.data(), &newData, sizeof(newData));
      rc = _publisher->send(message);

      Poco::Thread::yield();
      _mut.unlock();

      return (rc);
    };

    /*
     * Send data to an associated channel
     */
    bool Send(msgData &newData, int flags = 0)
    {

      _mut.lock();
      bool rc = false;

      zmq::message_t message(sizeof(newData));
      memcpy(message.data(), &newData, sizeof(newData));
      rc = _publisher->send(message, flags);

      Poco::Thread::yield();
      _mut.unlock();

      return (rc);
    };

private:
    zmq::socket_t *_publisher;
    Poco::Mutex _mut;
    const Poco::UInt16 _uiPort;
    const std::string _strSocket;
    std::string _strFullSocketPath;
    ppcLogger &_log;
};

}
