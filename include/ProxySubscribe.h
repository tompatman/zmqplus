/*
 * ProxySubscribe.h
 *
 *  Created on: Sep 18, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include "zmq.hpp"
#include <zmqUtil.h>
#include <ISubscribe.h>
#include "core/ppcLogger.h"
#include "core/systimer.h"

extern bool g_ForceIPC;

namespace ZMQ {
template<typename msgData>
class ProxySubscribe
        : public ZMQ::ISubscribe<msgData> {
public:
    //The channel is a string identifier used to identify which messages to monitor, like an address

    ProxySubscribe(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strProxyHost, const Poco::UInt64 uiChan,
                   const std::string strTopicPrefix = "NA")
            : _uiPort(uiPort)
              , _strProxyHost(strProxyHost)
              , _context(context)
              , _log(log)
              , _uiChan(uiChan)
              , _strTopic(zmqUtil::generateFullTopic(strTopicPrefix, uiChan))
    {
      _subscriber = new zmq::socket_t(_context, ZMQ_SUB);
      _tmrRead = new PbPoco::Core::SysTimer(READ_TIMEOUT_MS);
    };

    virtual ~ProxySubscribe()
    {
      _log.log(LG_DEBUG, "Deleting ProxySubscribe socket at %s:%d", _strProxyHost.c_str(), _uiPort);

      delete _tmrRead;
      _subscriber->close();
      delete _subscriber;
    };

    void Init(const bool bConflate = false, const int queueSize = 100, const bool bBlock = false) override
    {
      _bBlock = bBlock;

      if (bConflate) throw Poco::InvalidArgumentException("Conflate is not an allowed option when using a proxy system.");

      _strProxyFullPath = "tcp://" + _strProxyHost + ":" + Poco::NumberFormatter::format(_uiPort);
      if (g_ForceIPC)
      {
        zmqUtil::generateIPCConversion(_strProxyFullPath);
      }

      _log.log(LG_INFO, "Creating a proxy SUB socket at %s with topic %s", _strProxyFullPath.c_str(), _strTopic.c_str());

      try
      {
        _subscriber->setsockopt(ZMQ_RCVHWM, &queueSize, sizeof(queueSize));

        _subscriber->setsockopt(ZMQ_SUBSCRIBE, _strTopic.c_str(), _strTopic.length());

        _subscriber->connect(_strProxyFullPath);

        if (_bBlock)
        {
          _subscriber->setsockopt(ZMQ_RCVTIMEO, BLOCKING_TIMEOUT_MS);
        }

        if (g_ForceIPC)
        {
          ///It appears to take at least 10 mS for ipc initialization to happen. Tests will fail if attempting to read before that much time has passed
          Poco::Thread::sleep(15);
        }

      }
      catch (std::exception &e)
      {
        _log.log(LG_ERR, "*** Failed to create a proxy SUB socket at %s with topic %s", _strProxyFullPath.c_str(), _strTopic.c_str());
        throw e;
      }

    };


    // ** Use readCommand for non-repetitive commands.  This does not re-init.
    bool readData(msgData &recvData, const bool bForceBlocking = false) override
    {

      //  Read envelope with address
      zmq::message_t message;

      Poco::UInt32 count = 0;

      if (_bBlock || bForceBlocking)
      {
        _subscriber->setsockopt(ZMQ_RCVTIMEO, BLOCKING_TIMEOUT_MS);
        _subscriber->recv(&message);
      }
      else
      {
        while (message.size() == 0 && count < RECV_MAX_TRIES)
        {
          _subscriber->recv(&message, ZMQ_NOBLOCK);
          count++;
        }
      }

      if (message.size() == 0)
      {
        if (!_tmrRead->bIsTimerRunning())
        {
          _tmrRead->ResetTimer();
          _tmrRead->StartTimer();
        }

        if (_tmrRead->CheckTimer() && !_bSocketReopened)
        {
          /**
           * Too much time has passed without receiving a new message. Close and re-open the connection once
           */
          ReInit();
          _tmrRead->StopTimer();
          _tmrRead->ResetTimer();
          _bSocketReopened = true;
        }
        return (false);
      };

      if (message.size() != sizeof(recvData) &&
          message.size() != _strTopic.length())
      {
        throw Poco::InvalidAccessException("ProxySubscribe::readCommand() message size is incorrect, received " +
                                           Poco::NumberFormatter::format(message.size()) + " bytes, expected " +
                                           Poco::NumberFormatter::format(sizeof(recvData)) + ".");
      }

      /**
       * Message received. Stop the timeout timer in case it is running
       */
      _tmrRead->StopTimer();
      _bSocketReopened = false;

      if (message.size() == _strTopic.length())
      {

        //Block for the message contents
        _subscriber->setsockopt(ZMQ_RCVTIMEO, &PROXY_CONTENT_BLOCK_TIME_MS, sizeof(PROXY_CONTENT_BLOCK_TIME_MS));
        _subscriber->recv(&message);
        bool bStatus = false;
        if (message.size() != 0)
        {
          _log.log(LG_DEBUG, "Received a proxy message on topic %s of %d bytes", _strTopic.c_str(), message.size());
          memcpy(&recvData, message.data(), message.size());
          bStatus = true;
        }

        ///Go back to previous blocking time, if enabled
        if (_bBlock)
        {
          _subscriber->setsockopt(ZMQ_RCVTIMEO, BLOCKING_TIMEOUT_MS);
        }

        return (bStatus);
      }


      return (true);

    }


    Poco::UInt64 getChannel()
    { return (_uiChan); };

    Poco::UInt32 getReadTimeoutMs()
    { return (READ_TIMEOUT_MS); };

private:

    void ReInit()
    {
      /**
       * Close and delete the socket, it needs to be recreated
       */
      _log.log(LG_INFO, "Recreating a Proxy SUB socket at %s with topic %s", _strProxyFullPath.c_str(), _strTopic.c_str());

      _subscriber->close();
      delete _subscriber;
      _subscriber = new zmq::socket_t(_context, ZMQ_SUB);


      _subscriber->connect(_strProxyFullPath);
      _subscriber->setsockopt(ZMQ_SUBSCRIBE, _strTopic.c_str(), _strTopic.length());

      if (_bBlock)
      {
        _subscriber->setsockopt(ZMQ_RCVTIMEO, BLOCKING_TIMEOUT_MS);
      }

      if (g_ForceIPC)
      {
        ///It appears to take at least 10 mS for ipc initialization to happen. Tests will fail if attempting to read before that much time has passed
        Poco::Thread::sleep(15);
      }

    };

    const int PROXY_CONTENT_BLOCK_TIME_MS = 10;
    const int BLOCKING_TIMEOUT_MS = 3000;

    zmq::socket_t *_subscriber;
    const Poco::UInt16 _uiPort;
    const std::string _strProxyHost;
    std::string _strProxyFullPath;
    PbPoco::Core::SysTimer *_tmrRead;
    bool _bSocketReopened = false;
    static constexpr Poco::UInt32 READ_TIMEOUT_MS = 20000;
    zmq::context_t &_context;
    ppcLogger &_log;
    const Poco::UInt64 _uiChan;
    const std::string _strTopic;
    const Poco::UInt32 RECV_MAX_TRIES = 10;

    bool _bBlock = false;
};

}

