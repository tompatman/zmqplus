/*
 * ProxyPublish.h
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

extern bool g_ForceIPC;

namespace ZMQ {

template<typename msgData>
class ProxyPublish
        : public IPublish<msgData> {

public:
    ProxyPublish(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strProxyHost, const Poco::UInt64 uiChan,
                 const std::string strTopicPrefix = "NA", const bool bSendFirstMsgTwice = true)
            : _uiPort(uiPort)
              , _strProxyHost(strProxyHost)
              , _log(log)
              , _uiChan(uiChan)
              , _strTopic(zmqUtil::generateFullTopic(strTopicPrefix, uiChan))
              , _bSendFirstMsgTwice(bSendFirstMsgTwice)
    {

      //xxx one ioservices provider to the proxy. many ioservices consumers from the proxy using these classes. Add interface behind the publisher and subscriber
      _publisher = new zmq::socket_t(context, ZMQ_PUB);

    };

    virtual ~ProxyPublish()
    {
      _publisher->close();
      delete _publisher;
    };


    void Init(const int queueSize = 30)
    {
      _strProxyFullPath = "tcp://" + _strProxyHost + ":" + Poco::NumberFormatter::format(_uiPort);

      if (g_ForceIPC)
      {
        zmqUtil::generateIPCConversion(_strProxyFullPath);
      }
      _log.log(LG_INFO, "Publishing to proxy %s with topic %s", _strProxyFullPath.c_str(), _strTopic.c_str());

      //  Prepare our context and publisher
      _publisher->setsockopt(ZMQ_SNDHWM, &queueSize, sizeof(queueSize));
      _publisher->connect(_strProxyFullPath);

      //Poco::Thread::sleep(500); //  GM on 4/18/18 changed from 300mS.  100mS was not robust in the unit tests.

    };


    /*
     * Send data
     */
    bool Send(const msgData &newData)
    {

      _mut.lock();
      bool rc;

      //  Write two messages, the first is the address indicator

      zmq::message_t chanMessage(_strTopic.length());
      memcpy(chanMessage.data(), _strTopic.c_str(), _strTopic.length());
      rc = _publisher->send(chanMessage, ZMQ_SNDMORE);

      memcpy(&_dataBuffer[_bufIdx], &newData, sizeof(_dataBuffer[_bufIdx]));
      zmq::message_t message(&_dataBuffer[_bufIdx], sizeof(_dataBuffer[_bufIdx]), nullptr, nullptr);

      rc &= _publisher->send(message);

      if (_bSendFirstMsgTwice)
      {
        rc = _publisher->send(chanMessage, ZMQ_SNDMORE);
        rc &= _publisher->send(message);
        _bSendFirstMsgTwice = false;
      }

      _bufIdx = _bufIdx < (NUM_BUFFERED_MESSAGES - 1) ? _bufIdx + 1 : 0;

      Poco::Thread::yield();
      _mut.unlock();

      return (rc);
    };

private:
    zmq::socket_t *_publisher;
    Poco::Mutex _mut;
    const Poco::UInt16 _uiPort;
    const std::string _strProxyHost;
    std::string _strProxyFullPath;
    ppcLogger &_log;
    const Poco::UInt64 _uiChan;
    const std::string _strTopic;
    bool _bSendFirstMsgTwice;
    static const Poco::UInt32 NUM_BUFFERED_MESSAGES = 1000;
    Poco::UInt32 _bufIdx = 0;
    msgData _dataBuffer[NUM_BUFFERED_MESSAGES];
};

}

