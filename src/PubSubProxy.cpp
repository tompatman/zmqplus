//
// Created by talexander on 3/8/19.
//

#include "../include/PubSubProxy.h"
#include "../include/zmqUtil.h"
#include <zmq/zmq.hpp>
#include <core/ppcLogger.h>

extern bool g_ForceIPC;

namespace ZMQ {
PubSubProxy::PubSubProxy(const Poco::UInt32 frontEndPort, const Poco::UInt32 backEndPort, const Poco::UInt32 controlPort, const std::string strBindIp, zmq::context_t &context)
        : ppcRunnable("PubSubProxy", 10)
          , _frontEndPort(frontEndPort)
          , _backEndPort(backEndPort)
          , _controlPort(controlPort)
          , _strBindIp(strBindIp)
          , _zmqContext(context)
{
  _log = new ppcLogger("PubSubProxy", LG_DEBUG);

  //  Create frontend and backend sockets
}

void PubSubProxy::close()
{
  zmq::socket_t control(_zmqContext, ZMQ_PUB);
  control.bind(_strControlPath);

  Poco::Thread::sleep(1000);
  // terminate the proxy
  std::string strCmd = "TERMINATE";
  control.send(strCmd.c_str(), strCmd.length(), 0);
  _bClosed = true;
  Poco::Thread::sleep(100);
}

void PubSubProxy::exiting()
{

}

PubSubProxy::~PubSubProxy()
{
  delete _log;
}

void PubSubProxy::process()
{

  if (_bClosed)
  {
    return;
  }

  zmq::socket_t frontend(_zmqContext, ZMQ_XSUB);
  zmq::socket_t backend(_zmqContext, ZMQ_XPUB);
//  Bind both sockets to TCP ports

  _strFrontPath = std::string("tcp://" + _strBindIp + ":" + Poco::NumberFormatter::format(_frontEndPort));

  if (g_ForceIPC)
  {
    zmqUtil::generateIPCConversion(_strFrontPath);
  }

  frontend.bind(_strFrontPath);

  /**
   * Setup a control socket for the steerable proxy. This is made specifically to be able to exit the proxy, because
   * apparantly the zmq developers don't think a regular zmq proxy should bother to receive a terminate request
   */
  zmq::socket_t control(_zmqContext, ZMQ_SUB);


  try
  {
    // _frontend->setsockopt( ZMQ_SUBSCRIBE, "", 0);
  }
  catch (zmq::error_t &ex)
  {
    ppclog(LG_ERR, "zmq error %s", ex.what());
  }

  _strBackPath = std::string("tcp://" + _strBindIp + ":" + Poco::NumberFormatter::format(_backEndPort));
  if (g_ForceIPC)
  {
    zmqUtil::generateIPCConversion(_strBackPath);
  }

  backend.bind(_strBackPath);

  _strControlPath = std::string("tcp://" + _strBindIp + ":" + Poco::NumberFormatter::format(_controlPort));

  if (g_ForceIPC)
  {
    zmqUtil::generateIPCConversion(_strControlPath);
  }

  control.connect(_strControlPath);
  control.setsockopt(ZMQ_SUBSCRIBE, "", 0);

  //_frontend->setsockopt(ZMQ_LINGER, 100);
  //_backend->setsockopt(ZMQ_LINGER, 100);

  //This will run until the context is closed
  zmq::proxy_steerable(frontend, backend, nullptr, control);
  frontend.close();
  backend.close();
  control.close();
}

}
