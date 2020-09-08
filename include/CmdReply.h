//
// Created by service on 11/1/16.
//

#pragma once

#include <zmq.h>
#include <Poco/Thread.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Net/IPAddress.h>
#include "zmq.hpp"
#include "zCmdStruct.h"
#include "core/ppcLogger.h"

namespace ZMQ {
template<typename reqMsgData, typename respMsgData>
class CmdReply {
public:
    //The channel is a string identifier used to identify which messages to monitor, like an address

    CmdReply(zmq::context_t &context, const Poco::UInt16 uiPort, const std::string ipAddr, ppcLogger &log) //, const Poco::Int16 chanID = -1)
            : _uiPort(uiPort)
              , _ipAddr(ipAddr)
              , _log(log)
    {
      //_chanID = chanID;
      _reciever = new zmq::socket_t(context, ZMQ_REP);

    };

    virtual ~CmdReply()
    {
      _reciever->close();
      delete _reciever;
    };

    void init()
    {
      std::string sock_str = "tcp://" + _ipAddr + ":" + Poco::NumberFormatter::format(_uiPort);

      _log.log(LG_DEBUG, "Creating a Reply socket at %s", sock_str.c_str());


      _reciever->bind(sock_str);

      //Poco::Thread::sleep(10);   // -> GM on 3/7/19 removed to speed unit test
    };


    bool recieve(reqMsgData &recvData)
    {

      //  Read envelope with address
      zmq::message_t message;
      Poco::UInt16 uiChan = 0;

      bool rc = _reciever->recv(&message, ZMQ_NOBLOCK);

      if (!rc)
      {
        return (false);
      }

      if (message.size() == 0)
      {
        return (false);
      };

      if (message.size() != sizeof(recvData))
      {
        throw Poco::InvalidAccessException("Incoming message size is incorrect.");
      }

      memcpy(&recvData, message.data(), message.size());

      Poco::Thread::yield(); 

      return (true);

    }

    void sendReply(respMsgData &replyMsg)
    {
      zmq::message_t message;
      Poco::UInt16 uiChan = 0;

      _log.log(LG_DEBUG, "Reply size is %d", sizeof(replyMsg));

      zmq::message_t sndMessage(sizeof(replyMsg));
      memcpy(sndMessage.data(), &replyMsg, sizeof(replyMsg));

      bool rc = _reciever->send(sndMessage);

      return;
    }


private:
    zmq::socket_t *_reciever;
    const Poco::UInt16 _uiPort;
    const std::string _ipAddr;
    ppcLogger &_log;

};

}

