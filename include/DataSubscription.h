//
// Created by service on 10/28/16.
//

#pragma once

#include <Poco/Types.h>
#include <core/ppcLogger.h>
#include "zmq.hpp"
#include "IDataSubscription.h"
#include "ISubscribe.h"
#include <core/ITick.h>
#include "ProxySubscribe.h"
#include "CmdSubscribe.h"
#include <Globals/CommandDefs.h>

namespace ZMQ {

template<typename DataSetType>
class DataSubscription
        : public IDataSubscription {
public:
    DataSubscription(zmq::context_t &context, const Poco::UInt16 pubSubPort, ppcLogger &log, const std::string strSocket = "tcp://0.0.0.0:", const bool bProxySubscribe = false,
                     const std::string strDeviceID = "NA", const bool bBlock = false)
            : _lastCommTS(-99999999)
              , _ticker(ITick::getTicker())
    {
      if (bProxySubscribe)
      {
        _inVa = std::unique_ptr<ZMQ::ISubscribe<DataSetType>>(
                new ZMQ::ProxySubscribe<DataSetType>(context, PbPoco::Globals::ZMQ_PROXY_BACK, log, strSocket, pubSubPort, strDeviceID));
        if (bBlock)
        {
          _inVa->Init(false, 100, true);
        }
        else
        {
          _inVa->Init();
        }
      }
      else
      {
        _inVa = std::unique_ptr<ZMQ::ISubscribe<DataSetType>>(new ZMQ::CmdSubscribe<DataSetType>(context, pubSubPort, log, strSocket));
        if (bBlock)
        {
          _inVa->Init(true, 100, true);
        }
        else
        {
          _inVa->Init(true);
        }
      }

      memset(&_data, 0, sizeof(_data));
    };

    virtual ~DataSubscription()
    {
      update();
    };

    bool update(const bool bForceBlocking = false) override
    {
      bool newData = false;

      DataSetType localData;
      while (_inVa->readData(localData, bForceBlocking))
      {
        memcpy(&_data, &localData, sizeof(localData));

        _lastCommTS = _ticker.get_num_mSeconds();
        newData = true;
      }

      return newData;
    };

    Poco::Int64 mSecondsSinceLastComm() override
    {
      return _ticker.mSecondsSince(_lastCommTS);
    }

    DataSetType _data;

protected:

private:
    std::unique_ptr<ZMQ::ISubscribe<DataSetType>> _inVa;

    Poco::Int64 _lastCommTS;

    ITick &_ticker;
};

}

