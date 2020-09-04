/*
 * TagParser.cpp
 *
 *  Created on: Jan 4, 2016
 *      Author: service
 */

#include "../include/TagParser.h"
#include <Poco/NumberParser.h>

namespace ZMQ {

TagParser::TagParser(const char *parser_name) :
        m_pLocator(0)
{

  _log = new ppcLogger(parser_name);
  _log->setLevel(LG_INFO);


  /*
  Data,
  EnableCluster,
  DisableCluster,
  EnablePump,
  DisablePump,
  SetSpeed,
  StartPump,
  StopPump,
  AutoPumps,
  ManualPumps, */

  //Configure the map for handling xml requests
#if 0
  _parserMap["ENABLE_CLUSTER"] = &TagParser::handleEnableCluster;
                _parserMap["DISABLE_CLUSTER"] = &TagParser::handleDisableCluster;
                _parserMap["PUMP_SPEED"] = &TagParser::handleSetSpeed;
                _parserMap["START_PUMP"] = &TagParser::handleStart;
                _parserMap["STOP_PUMP"] = &TagParser::handleStop;
                _parserMap["ENABLE_PUMP"] = &TagParser::handleEnablePump;
                _parserMap["DISABLE_PUMP"] = &TagParser::handleDisablePump;
                _parserMap["EXPORT_PUMP_CTRL"] = &TagParser::handleDataRequest;
#endif
  //_parserMap["CtrlPumpData"] = &TagParser::handleDataRequest;
  //_test = &TagParser::handleDataRequest;
}

TagParser::~TagParser()
{
  delete _log;
}

void TagParser::setDocumentLocator(const Poco::XML::Locator *loc)
{
  m_pLocator = loc;
}

void TagParser::startDocument()
{

}

void TagParser::endDocument()
{

}


void TagParser::characters(const Poco::XML::XMLChar ch[], int start, int length)
{
  _innerText = std::string(ch + start, length);
}

void TagParser::ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length)
{
}

void TagParser::processingInstruction(const Poco::XML::XMLString &target, const Poco::XML::XMLString &data)
{
  _log->log(LG_DEBUG, "%s", Poco::format("    target=%s, data=%s", target, data).c_str());
}

void TagParser::startPrefixMapping(const Poco::XML::XMLString &prefix, const Poco::XML::XMLString &uri)
{
  _log->log(LG_DEBUG, "%s", Poco::format("    prefix=%s, uri=%s", prefix, uri).c_str());
}

void TagParser::endPrefixMapping(const Poco::XML::XMLString &prefix)
{
  _log->log(LG_DEBUG, "%s", Poco::format("    prefix=%s", prefix).c_str());
}

void TagParser::skippedEntity(const Poco::XML::XMLString &name)
{
  _log->log(LG_DEBUG, Poco::format("    name=%s", name));
}

// LexicalHandler
void TagParser::startDTD(const Poco::XML::XMLString &name, const Poco::XML::XMLString &publicId, const Poco::XML::XMLString &systemId)
{
}

void TagParser::endDTD()
{
}

void TagParser::startEntity(const Poco::XML::XMLString &name)
{
}

void TagParser::endEntity(const Poco::XML::XMLString &name)
{
}

void TagParser::startCDATA()
{
}

void TagParser::endCDATA()
{
}

void TagParser::comment(const Poco::XML::XMLChar ch[], int start, int length)
{
}




}