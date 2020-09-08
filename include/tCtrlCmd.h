/*
 * CmdQueue.h
 *
 *  Created on: Sep 17, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include "zmq.hpp"
#include <Poco/StringTokenizer.h>
#include <Poco/NumberParser.h>
#include "Poco/Types.h"

namespace ZMQ {

static const Poco::UInt16 vionx_MAX_TEXT_VALUE_LENGTH = 64;
static const Poco::UInt16 vionx_MAX_VALUES = 8;

typedef struct tCtrlCmd {

    tCtrlCmd()
    {
      clear_data_object();
    };

    char destination[vionx_MAX_TEXT_VALUE_LENGTH];    // Should contain a mapping string

    char source[vionx_MAX_TEXT_VALUE_LENGTH];

    Poco::Int16 cmd;

    Poco::Int16 devId;           // Indexing in a list of devices within a zmq port.

    Poco::Int16 ioAddr;             // Address index within a device space.

    bool bool_val[vionx_MAX_VALUES];                                  // These are data value.
    int int_val[vionx_MAX_VALUES];
    double float_val[vionx_MAX_VALUES];
    char text_val[vionx_MAX_VALUES][vionx_MAX_TEXT_VALUE_LENGTH];


    // ***********   Helper functions.
    void clear_data_object()
    {
      memset(this, 0, sizeof(tCtrlCmd));
      cmd = -1;
      devId = -1;
    };

    void pack_source_loc(int locationEnum)
    {
      if (snprintf(source, vionx_MAX_TEXT_VALUE_LENGTH, "%d", locationEnum) < 0) {
        throw "snprintf truncation";
      }
    };

    int unpack_source_loc()
    {
      using std::string;

      Poco::StringTokenizer tok(source, "-");

      if(tok.count() != 1) throw Poco::InvalidArgumentException("Destination string not valid [" + string(source) + "]");

      int sourceLocation = Poco::NumberParser::parse(tok[0]);

      if(sourceLocation < 0)    // Zero used as controlling object.
        throw Poco::InvalidArgumentException("Invalid source string[" + string(source) + "]");

      return sourceLocation;
    };

    void set_destination(const char *input_chars)
    {
      Poco::StringTokenizer tok(input_chars, "-");

      if(tok.count() != 2) throw Poco::InvalidArgumentException("Destination string not valid [" + std::string(input_chars) + "]");

      if (snprintf(destination, vionx_MAX_TEXT_VALUE_LENGTH, input_chars) < 0) {
        throw  "snprintf truncation";
      }
    }

    static std::string convert_destination(int primaryLoc, int secondaryLoc)
    {
      std::string dest = std::to_string(primaryLoc) + "-" + std::to_string(secondaryLoc);
      return dest;
    }

    void pack_destination_loc(int primaryLoc, int secondaryLoc)
    {
      if (snprintf(destination, vionx_MAX_TEXT_VALUE_LENGTH, "%d-%d", primaryLoc, secondaryLoc) < 0) {
        throw "snprintf truncation";
      }
    };

    int unpack_destination_primaryLocation()
    {
      using std::string;

      Poco::StringTokenizer tok(destination, "-");

      if(tok.count() != 2) throw Poco::InvalidArgumentException("Destination string not valid [" + string(destination) + "]");

      int primaryLocation = Poco::NumberParser::parse(tok[0]);

      if(primaryLocation < 0)    // Zero used as controlling object.
        throw Poco::InvalidArgumentException("Invalid destination string[" + string(destination) + "]");

      return primaryLocation;
    };

    int unpack_destination_secondaryLocation()
    {
      using std::string;

      Poco::StringTokenizer tok(destination, "-");

      if(tok.count() != 2) throw Poco::InvalidArgumentException("Destination string not valid [" + string(destination) + "]");

      int secondaryLocation = Poco::NumberParser::parse(tok[1]);

      if(secondaryLocation < 0)    // Zero used as controlling object.
        throw Poco::InvalidArgumentException("Invalid destination string[" + string(destination) + "]");

      return secondaryLocation;
    };

} vionxCmdStruct;

}

