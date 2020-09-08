//
// Created by service on 4/11/16.
//

#pragma once

#include <Poco/SAX/ContentHandler.h>
#include <Poco/SAX/LexicalHandler.h>
#include <IDataResponder.h>
#include <core/ppcLogger.h>

namespace ZMQ {

class TagParser : public Poco::XML::ContentHandler, public Poco::XML::LexicalHandler {
public:
    TagParser(const char *parser_name);

    virtual ~TagParser();

    // ContentHandler
    virtual void setDocumentLocator(const Poco::XML::Locator *loc);

    virtual void startDocument();

    virtual void endDocument();

    /**
     * Handle requests of the form
     * <?xml version="1.0" encoding="utf-8"?>
        <request
                cmd="{getData|getTemplate|getValue|setValue}"
                valueName="{IO|DC|...}"
                [valueIndex="{number}"]
                [value="number"]
        />
     * @param uri
     * @param localName
     * @param qname
     * @param attributes
     */
    virtual void startElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName, const Poco::XML::XMLString &qname, const Poco::XML::Attributes &attributes) = 0;

    virtual void endElement(const Poco::XML::XMLString &uri, const Poco::XML::XMLString &localName, const Poco::XML::XMLString &qname) = 0;

    virtual void characters(const Poco::XML::XMLChar ch[], int start, int length);

    virtual void ignorableWhitespace(const Poco::XML::XMLChar ch[], int start, int length);

    virtual void processingInstruction(const Poco::XML::XMLString &target, const Poco::XML::XMLString &data);

    virtual void startPrefixMapping(const Poco::XML::XMLString &prefix, const Poco::XML::XMLString &uri);

    virtual void endPrefixMapping(const Poco::XML::XMLString &prefix);

    virtual void skippedEntity(const Poco::XML::XMLString &name);

    // LexicalHandler
    virtual void startDTD(const Poco::XML::XMLString &name, const Poco::XML::XMLString &publicId, const Poco::XML::XMLString &systemId);

    virtual void endDTD();

    virtual void startEntity(const Poco::XML::XMLString &name);

    virtual void endEntity(const Poco::XML::XMLString &name);

    virtual void startCDATA();

    /**
     *
     */
    virtual void endCDATA();

    virtual void comment(const Poco::XML::XMLChar ch[], int start, int length);

    std::string getTagValue()
    { return (_strValue); };

    /**
     * Set the tag name to search for when parsing
     */
//    virtual void setTagName(const std::string strTag)
//    { _strSearchTag = strTag; };

    virtual void clearFaultedElement() = 0;


private:

    ppcLogger *_log;

    const Poco::XML::Locator *m_pLocator;
    std::string _innerText;

    std::string _strValue;
};


}; /*namespace ZMQ*/

