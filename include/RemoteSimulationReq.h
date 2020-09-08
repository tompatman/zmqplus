//
// Created by gmorehead on 3/31/17.
//


#pragma once

#include <zmq.hpp>
#include <CmdRequest.h>
#include <string>

namespace ZMQ {

template<typename tDataSetType>
class RemoteSimulationReq {

public:
    RemoteSimulationReq(zmq::context_t &context, ppcLogger &log, std::string remote_sim_ip, Poco::UInt16 remote_req_port, uint uiTimeout = 500, uint uiNumRetries = 2)
            : _log(log), _remoteSimReq(NULL)
    {
      _remoteSimReq = new ZMQ::CmdRequest<tDataSetType, tDataSetType>(context, remote_req_port, remote_sim_ip, uiTimeout, _log, true, uiNumRetries);
      _remoteSimReq->init();
    }

    virtual ~RemoteSimulationReq()
    {
      if (_remoteSimReq != NULL) delete _remoteSimReq;
    }


    void updateRemoteSimulation(tDataSetType &data)
    {
      try
      {
        if (!_remoteSimReq->send(data))
        {
          throw Poco::IOException("Request failed.");
        }
      }
      catch (Poco::Exception &e)
      {
        _log.log(LG_ERR, "Request Exception %s", e.message().c_str());

        sleep(2);

        _remoteSimReq->reconnect();
        return;
      }
      catch (...)
      {
        _log.log(LG_ERR, "Request Caught generic exception.");
      }

      try
      {
        tDataSetType sim_resp_data = _remoteSimReq->getResponse();

        memcpy(&data, &sim_resp_data, sizeof(tDataSetType));
      }
      catch (Poco::Exception &e)
      {
        _log.log(LG_ERR, "Response Exception %s", e.message().c_str());

        sleep(2);

        _remoteSimReq->reconnect();
      }
      catch (...)
      {
        _log.log(LG_ERR, "Caught generic exception.");
      }
    }


protected:
    ppcLogger &_log;

    ZMQ::CmdRequest<tDataSetType, tDataSetType> *_remoteSimReq;

};

}
