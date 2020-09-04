//
// Created by service on 10/21/16.
//

#pragma once

#include "zmq/CmdPublish.h"
#include "zmq/MultiPublish.h"


namespace ZMQ {

template<typename DataSetType>
class DataPublication {
public:
    DataPublication(zmq::context_t &context, DataSetType &localData, const Poco::UInt16 pubSubPort,
                    ppcLogger &log, const bool bMultiPublish = false, const std::string strSocket = "tcp://*:", const std::string strDeviceID = "NA",
                    const std::string strInprocPrefix = "ipc://tmp")
            : _localData(localData)
    {
      if (bMultiPublish)
      {
        ///If using multiPublish the socket must be only an IP address
        _dataPub = std::unique_ptr<ZMQ::IPublish<DataSetType>>(new ZMQ::MultiPublish<DataSetType>(context, log, strSocket, strInprocPrefix, pubSubPort, strDeviceID));
      }
      else
      {
        _dataPub = std::unique_ptr<ZMQ::IPublish<DataSetType>>(new ZMQ::CmdPublish<DataSetType>(context, pubSubPort, log, strSocket));
      }
      _dataPub->Init(2);
    }


    virtual ~DataPublication()
    {

    }

    void publish()
    {
      _dataPub->Send(_localData);
    }

    std::unique_ptr<ZMQ::IPublish<DataSetType>> _dataPub;

    DataSetType &_localData;
};

}
