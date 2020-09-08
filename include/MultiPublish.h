//
// Created by talexander on 3/14/19.
//

#pragma once

#include <zmq.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Thread.h>
#include <core/ppcLogger.h>
#include <zCmdStruct.h>
#include <IPublish.h>
#include <cstring>
#include <CmdPublish.h>
#include <ProxyPublish.h>
#include <Globals/CommandDefs.h>

/**
 * Publish data to both inproc and offboard to a proxy. This allows data to be accessed within the app but also makes it available to a single off board proxy server
 */

namespace ZMQ {

template<typename msgData>
class MultiPublish
        : public ZMQ::IPublish<msgData> {

public:
    MultiPublish(zmq::context_t &context, ppcLogger &log, const std::string strProxyHost, const std::string strInprocPrefix, const Poco::UInt64 uiChan,
                 const std::string strDeviceID = "NA")
            : _log(log)
    {
      _log.log(LG_DEBUG, "Creating a proxy connection to %s and inproc to id %d", strProxyHost.c_str(), uiChan);

      _pubProxy = new ZMQ::ProxyPublish<msgData>(context, PbPoco::Globals::ZMQ_PROXY_FRONT, _log, strProxyHost, uiChan, strDeviceID);
      ///Note: Using ipc because inproc doesn't work on the arm for some reason
      _pubProc = new ZMQ::CmdPublish<msgData>(context, uiChan, strInprocPrefix);


      //_pubProc = new ZMQ::CmdPublish<msgData>(context, uiChan, _log, "inproc://#");
    };

    virtual ~MultiPublish()
    {
      delete _pubProc;
      delete _pubProxy;
    };


    void Init(const int queueSize = 30)
    {

      //  Prepare our context and publisher
      _pubProxy->Init();
      _pubProc->Init();

    };

    /*
     * Send message with raw data only, no channel is associated with it
     */
    bool Send(const msgData &newData)
    {

      bool rc = _pubProxy->Send(newData);
      rc &= _pubProc->Send(newData);

      return (rc);
    };


private:
    ZMQ::IPublish<msgData> *_pubProc = nullptr;
    ZMQ::IPublish<msgData> *_pubProxy = nullptr;

    ppcLogger &_log;

};


}