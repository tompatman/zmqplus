//
// Created by gmorehead on 2/16/17.
//


#pragma once

#include <core/HighResTimer.h>
#include "CmdSubscribe.h"

namespace ZMQ {

template<typename msgData>

class CmdSubTimed {
public:
    CmdSubTimed(zmq::context_t &context, const Poco::UInt16 uiPort, ppcLogger &log, const std::string strSocket = "tcp://localhost:")
    {
      _subscription = new CmdSubscribe<msgData>(context, uiPort, log, strSocket);
      _subscription->Init(true);

      _hrt.reset();
      _hrt.start();

    }

    virtual ~CmdSubTimed()
    {
      delete _subscription;
    }

    bool readData(msgData &newData)
    {
      bool bNewData = _subscription->readData(newData);
      if (bNewData)
      {
        _hrt.start();
      }

      return (bNewData);
    }

    int getElapsedMs()
    {
      return _hrt.getElapsedMs();
    }

private:
    CmdSubscribe <msgData> *_subscription;

    PbPoco::Core::HighResTimer _hrt;
};


}
