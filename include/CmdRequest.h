//
// Created by service on 11/1/16.
//

#pragma once

#include <zmq.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Thread.h>
#include <Poco/Net/IPAddress.h>
#include <zconf.h>
#include <core/ppcLogger.h>
#include "zmq/zmq.hpp"
#include "zCmdStruct.h"
#include "IPublish.h"
#include <string.h>

namespace ZMQ {

template<typename reqMsgData, typename respMsgData>
class CmdRequest { //: public IPublish <reqMsgData>{

public:
    CmdRequest(zmq::context_t &context, const Poco::UInt16 uiPort, const std::string ipAddr,
               const Poco::UInt16 uiTimeoutMs, ppcLogger &log, bool relaxed_operation = false, uint num_response_retries = 2)
            : _context(context)
              , _uiPort(uiPort)
              , _ipAddr(ipAddr)
              , _iTimeoutMs(uiTimeoutMs)
              , _log(log)
              , _relaxed_operation(relaxed_operation)
              , _num_response_retries(num_response_retries)
    {
      _requester = new zmq::socket_t(_context, ZMQ_REQ);

    };

    virtual ~CmdRequest()
    {
      _requester->close();
      delete _requester;
    };

    //Virtual implementations
    void init()
    {
      std::string sock_str = "tcp://" + _ipAddr + ":" + Poco::NumberFormatter::format(_uiPort);

      _log.log(LG_DEBUG, "Creating a ZMQ_REQ socket at %s", sock_str.c_str());

      _requester->setsockopt(ZMQ_RCVTIMEO, _iTimeoutMs);
      _requester->setsockopt(ZMQ_SNDTIMEO, _iTimeoutMs);

      _requester->setsockopt(ZMQ_LINGER, (Poco::Int32) 0);

      if (_relaxed_operation)
      {
        _log.log(LG_DEBUG, "Setting the relaxed options for the ZMQ_REQ port at %s", sock_str.c_str());
        _requester->setsockopt(ZMQ_REQ_CORRELATE, (Poco::Int32) 1);
        _requester->setsockopt(ZMQ_REQ_RELAXED, (Poco::Int32) 1);
      }

      _requester->connect(sock_str);

    };

    void reconnect()
    {
      std::string sock_str = "tcp://" + _ipAddr + ":" + Poco::NumberFormatter::format(_uiPort);

      _log.log(LG_INFO, "Attempting Reconnect of socket at %s:", sock_str.c_str());
      _requester->close();

      delete _requester;

      usleep(1000 * _iTimeoutMs);

      _requester = new zmq::socket_t(_context, ZMQ_REQ);

      init();
    }
    /*
     * Send message with raw data only, no channel is associated with it
     */
    bool send(const reqMsgData &newData)
    {

      _mut.lock();
      bool rc = false;

      zmq::message_t sndMessage(sizeof(newData));
      memcpy(sndMessage.data(), &newData, sizeof(newData));
      rc = _requester->send(sndMessage);


      Poco::Thread::yield();  
      _mut.unlock();

      return rc;
    };

    respMsgData getResponse()
    {

      _mut.lock();

      uint reply_attempts = 0;

      //Wait for reply until timeout
      zmq::message_t rawResponse;
      Poco::UInt16 uiChan = 0;
      Poco::Int16 rc = _requester->recv(&rawResponse);  // Let's try one attempt immediately

      while (rc == false)
      {
        _log.log(LG_ERR, "getResponse() (%s) No reply from server.  Attempting again.", _ipAddr.c_str());

        usleep(500);

        rc = _requester->recv(&rawResponse);

        if (rc == false && ++reply_attempts > _num_response_retries)
        {
          _mut.unlock();
          throw Poco::TimeoutException("No reply from server after " + std::to_string(reply_attempts) + " attempts.");
        }
      }

      if (rawResponse.size() == 0)
      {
        _mut.unlock();
        throw Poco::TimeoutException("Response from server has zero size");
      };

      if (rawResponse.size() != sizeof(respMsgData))
      {
        _log.log(LG_ERR, "Size mismatch %d vs defined %d", rawResponse.size(), sizeof(respMsgData));
        _mut.unlock();
        throw Poco::InvalidAccessException("Incoming message size is incorrect. " + Poco::NumberFormatter::format(rawResponse.size()) + " vs defined "
                                           + Poco::NumberFormatter::format((int) sizeof(respMsgData)));
      }

      respMsgData response;
      memcpy(&response, rawResponse.data(), rawResponse.size());

      _mut.unlock();
      return(response);
    }
private:
    zmq::context_t &_context;
    zmq::socket_t *_requester;
    Poco::Mutex _mut;
    const Poco::UInt16 _uiPort;
    const std::string _ipAddr;
    const Poco::Int32 _iTimeoutMs;

    ppcLogger &_log;

    bool _relaxed_operation;

    const uint _num_response_retries;
};

}

