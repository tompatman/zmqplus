#include "IPublish.h"
#include "../include/XmlDataReqHandler.h"


namespace ZMQ {


XmlDataReqHandler::XmlDataReqHandler(ZMQ::ISubscriptionUpdater &updater, ZMQ::TagParser &dataHandler, const char *thread_name,
                                     int comm_timeout_ms, int log_level, int thread_rate)
        : ppcRunnable(thread_name, thread_rate), _comm_timeout_ms(comm_timeout_ms)
          , _dataHandler(dataHandler)
          , _updater(updater)
{
  _logRun->setLevel(log_level);

  init();

}

XmlDataReqHandler::XmlDataReqHandler(ZMQ::ISubscriptionUpdater &updater, ZMQ::TagParser &dataHandler, const Poco::UInt16 watchdogCmd_pushpull_port,
                                     const char *thread_name, zmq::context_t &context, string local_ip_bind, int comm_timeout_ms, int log_level, int thread_rate)
        : ppcRunnable(thread_name, thread_rate), _comm_timeout_ms(comm_timeout_ms)
          , _dataHandler(dataHandler)
          , _updater(updater)
{
  _logRun->setLevel(log_level);

  this->enableWatchdogSignal(context, watchdogCmd_pushpull_port, local_ip_bind);
  init();

}

XmlDataReqHandler::~XmlDataReqHandler()
{
}

void XmlDataReqHandler::process()
{
  std::string strNewData;

  try
  {
    _updater.update();

    if (_updater.recv_string(&strNewData))
    {
      _logRun->log(LG_DEBUG, "Incoming Request: %s", strNewData.c_str());

      _parser.parseString(strNewData);
    }
  }
  catch (Poco::Exception &exc)
  {
    _logRun->log(LG_ERR, "XmlDatqReqHandler Caught Poco::Exception in %s : %s ", _instance_name.c_str(), exc.displayText().c_str());
    _logRun->log(LG_ERR, " - Received data %s", strNewData.c_str());
    _dataHandler.clearFaultedElement();
  }
  catch (std::exception &exc)
  {
    _logRun->log(LG_ERR, "XmlDatqReqHandler Caught a std::exception in %s : %s ", _instance_name.c_str(), exc.what());
    _logRun->log(LG_ERR, " - Received data %s", strNewData.c_str());
    _dataHandler.clearFaultedElement();
  }
  catch (...)
  {
    _logRun->log(LG_ERR, "XmlDatqReqHandler Caught a generic exception in %s ", _instance_name.c_str());
    _logRun->log(LG_ERR, " - Received data %s", strNewData.c_str());
    _dataHandler.clearFaultedElement();
  }

}

void XmlDataReqHandler::init()
{
  //Setup xml parser
  _parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, true);
  _parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACE_PREFIXES, true);
  _parser.setContentHandler(&_dataHandler);
  _parser.setProperty(Poco::XML::XMLReader::PROPERTY_LEXICAL_HANDLER, static_cast<Poco::XML::LexicalHandler *>(&_dataHandler));

}


}
