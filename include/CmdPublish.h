/*
 * CmdPublish.h
 *
 *  Created on: Sep 18, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Thread.h>
#include <core/ppcLogger.h>
#include "zCmdStruct.h"
#include "IPublish.h"
#include <cstring>
#include <zmqUtil.h>

extern bool g_ForceIPC;

namespace ZMQ {

template<typename msgData>
class CmdPublish
        : public IPublish<msgData> {

public:
    CmdPublish(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strSocket = "tcp://*:")
            : _uiPort(uiPort)
              , _log(log)
              , _strSocket(zmqUtil::generateSocketFullPath(strSocket, uiPort))
    {
      _publisher = new zmq::socket_t(context, ZMQ_PUB);
      //_zMsg = new zmq::message_t(&_dataBuffer, sizeof(_dataBuffer), nullptr, nullptr);
    };

    virtual ~CmdPublish()
    {
      _publisher->close();
      delete _publisher;
    };


    void Init(const int queueSize = 100)
    {

      _log.log(LG_DEBUG, "Creating a PUB socket at %s", _strSocket.c_str());

      //  Prepare our context and publisher
      _publisher->setsockopt(ZMQ_SNDHWM, &queueSize, sizeof(queueSize));
      if (g_ForceIPC)
      {
        zmqUtil::generateIPCConversion(_strSocket);
        _publisher->bind(_strSocket);
      }
      else
      {
        _publisher->bind(_strSocket);
      }

      //Poco::Thread::sleep(200); //  GM on 4/18/18 changed from 300mS.  100mS was not robust in the unit tests.
      // -> GM on 3/7/19 removed to speed unit test, now seems stable.

    };

    /*
     * Send message with raw data only, no channel is associated with it
     */
    bool Send(const msgData &newData)
    {

      _mut.lock();
      bool rc;

      ///Using the zero copy method to prevent mallocs for message publishing
      memcpy(&_dataBuffer[_bufIdx], &newData, sizeof(_dataBuffer[_bufIdx]));
      zmq::message_t message(&_dataBuffer[_bufIdx], sizeof(_dataBuffer[_bufIdx]), nullptr, nullptr);
      rc = _publisher->send(message);

      _bufIdx = _bufIdx < (NUM_BUFFERED_MESSAGES - 1) ? _bufIdx + 1 : 0;
      Poco::Thread::yield();
      _mut.unlock();

      return (rc);
    };

    /*
     * Send data to an associated channel
     */
    bool Send(msgData &newData, Poco::UInt16 uiChan)
    {

      _mut.lock();
      bool rc;

      //  Write two messages, the first is the address indicator

      zmq::message_t chanMessage(sizeof(uiChan));
      memcpy(chanMessage.data(), &uiChan, sizeof(uiChan));
      rc = _publisher->send(chanMessage, ZMQ_SNDMORE);

      zmq::message_t message(sizeof(newData));
      memcpy(message.data(), &newData, sizeof(newData));
      rc &= _publisher->send(message);



      Poco::Thread::yield();
      _mut.unlock();

      return (rc);
    };

private:
    zmq::socket_t *_publisher;
    Poco::Mutex _mut;
    const Poco::UInt16 _uiPort;
    ppcLogger &_log;
    std::string _strSocket;
    zmq::message_t *_zMsg;
    static const Poco::UInt32 NUM_BUFFERED_MESSAGES = 500;
    Poco::UInt32 _bufIdx = 0;
    msgData _dataBuffer[NUM_BUFFERED_MESSAGES];
};

}

