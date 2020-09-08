/*
 * CmdSubscribe.h
 *
 *  Created on: Sep 18, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include "zmq.hpp"
#include "core/ppcLogger.h"
#include "core/systimer.h"
#include <zmqUtil.h>
#include <ISubscribe.h>

extern bool g_ForceIPC;

namespace ZMQ {


template<typename msgData>
class CmdSubscribe
        : public ZMQ::ISubscribe<msgData> {
public:
    //The channel is a string identifier used to identify which messages to monitor, like an address

    CmdSubscribe(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strSocket = "tcp://localhost:")
            : _uiPort(uiPort)
              , _strSocket(zmqUtil::generateSocketFullPath(strSocket, uiPort))
              , _tmrRead(READ_TIMEOUT_MS)
              , _context(context)
              , _log(log)
    {
      _subscriber = std::unique_ptr<zmq::socket_t>(new zmq::socket_t(_context, ZMQ_SUB));

    };

    virtual ~CmdSubscribe()
    {
      _log.log(LG_DEBUG, "Deleting CmdSubscribe socket at %s", _strSocket.c_str());

      _subscriber->close();
    };

    void Init(const bool bConflate = false, const int queueSize = 100, const bool bBlock = false) override
    {
      _bConflate = bConflate;
      _bBlock = bBlock;

      _log.log(LG_DEBUG, "Creating a SUB socket at %s", _strSocket.c_str());

      try
      {
        _subscriber->setsockopt(ZMQ_RCVHWM, &queueSize, sizeof(queueSize));
        //  Prepare our context and subscriber
        if (_bConflate)
        {
          //If the subscriber is not channel based it is assumed that it is subscribing to real time data
          //So the conflate option is used to drop all older messages and only the latest data is available
          //ONLY USE WITH NON-MULTIPART MESSAGES.
          int conflate = 1;
          _subscriber->setsockopt(ZMQ_CONFLATE, &conflate, sizeof(conflate));

        }

        _subscriber->setsockopt(ZMQ_SUBSCRIBE, "", 0);


        if (g_ForceIPC)
        {
          zmqUtil::generateIPCConversion(_strSocket);
          _subscriber->connect(_strSocket);
        }
        else
        {
          _subscriber->connect(_strSocket);
        }

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
        _log.log(LG_INFO, "*** Failed to create a SUB socket at %s", _strSocket.c_str());
        throw e;
      }

    };


    // ** Use readCommand for non-repetitive commands.  This does not re-init.

    bool readData(msgData &newData, const bool bForceBlocking = false) override
    {

      //  Read envelope with address
      zmq::message_t message;
      Poco::UInt16 uiChan = 0;
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
        if (!_tmrRead.bIsTimerRunning())
        {
          _tmrRead.ResetTimer();
          _tmrRead.StartTimer();
        }

        if (_tmrRead.CheckTimer() && !_bSocketReopened)
        {
          /**
           * Too much time has passed without receiving a new message. Close and re-open the connection once
           */
          ReInit();
          _tmrRead.StopTimer();
          _tmrRead.ResetTimer();
          _bSocketReopened = true;
        }
        return (false);
      };

      if (message.size() != sizeof(msgData))
      {
        throw Poco::InvalidAccessException("CmdSubscribe::readData() message size is incorrect, received " +
                                           Poco::NumberFormatter::format(message.size()) + " bytes, expected " +
                                           Poco::NumberFormatter::format(sizeof(msgData)) + ".");
      }

      /**
       * Message received. Stop the timeout timer in case it is running
       */
      _tmrRead.StopTimer();

      memcpy(&newData, message.data(), message.size());
      _bSocketReopened = false;

      Poco::Thread::yield();

      return (true);

    }


private:

    void ReInit()
    {
      /**
       * Close and delete the socket, it needs to be recreated
       */
      _subscriber->close();
      _subscriber.reset(new zmq::socket_t(_context, ZMQ_SUB));

      _log.log(LG_INFO, "Recreating a SUB socket at %s", _strSocket.c_str());

      //  Prepare our context and subscriber
      if (_bConflate)
      {
        //If the subscriber is not channel based it is assumed that it is subscribing to real time data
        //So the conflate option is used to drop all older messages and only the latest data is available
        //ONLY USE WITH NON-MULTIPART MESSAGES.
        int conflate = 1;
        _subscriber->setsockopt(ZMQ_CONFLATE, &conflate, sizeof(conflate));

      }

      _subscriber->setsockopt(ZMQ_SUBSCRIBE, "", 0);

      _subscriber->connect(_strSocket);

      if (_bBlock)
      {
        _subscriber->setsockopt(ZMQ_RCVTIMEO, BLOCKING_TIMEOUT_MS);
      }

      if (g_ForceIPC)
      {
        ///It appears to take at least 10 mS for ipc initialization to happen. Tests will fail if attempting to read before that much time has passed
        Poco::Thread::sleep(15);
      }

//      _reciever->setsockopt(ZMQ_RCVHWM, &queueSize, sizeof(int));

    };


    const Poco::UInt32 RECV_MAX_TRIES = 10;
    const Poco::UInt32 READ_TIMEOUT_MS = 20000;
    const int BLOCKING_TIMEOUT_MS = 1000;

    std::unique_ptr<zmq::socket_t> _subscriber;
    msgData _incomingCmd;
    const Poco::UInt16 _uiPort;
    std::string _strSocket;
    PbPoco::Core::SysTimer _tmrRead;
    bool _bSocketReopened = false;
    bool _bConflate = false;
    bool _bBlock = false;
    zmq::context_t &_context;
    ppcLogger &_log;
};

}

