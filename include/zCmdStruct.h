/*
 * CmdQueue.h
 *
 *  Created on: Sep 17, 2015
 *      Author: service
 */

#pragma once

#include <zmq.h>
#include "zmq.hpp"

#include "Poco/Types.h"

namespace ZMQ {

static const Poco::UInt16 MAX_TEXT_VALUE_LENGTH = 64;
static const Poco::UInt16 MAX_VALUES = 8;

typedef struct CommandStruct {

    CommandStruct()
    {
        memset(this, 0, sizeof(CommandStruct));
        cmd = -1;
        deviceId = -1;
        sourceId = -1;
    };

    Poco::Int16 cmd;

    Poco::Int16 deviceId;           // Indexing in a list of devices within a zmq port.

    Poco::Int16 sourceId;           // TODO: Consider changing to a text field.

    Poco::Int16 ioAddr;             // Address index within a device space.

    bool bool_val[MAX_VALUES];                                  // These are data value.
    Poco::Int64 int_val[MAX_VALUES];
    double float_val[MAX_VALUES];
    char text_val[MAX_VALUES][MAX_TEXT_VALUE_LENGTH];

} tCommand;

}

