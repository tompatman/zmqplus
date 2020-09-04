//
// Created by gmorehead on 3/28/18.
//


#pragma once

#include "vionxCmdStruct.h"

namespace ZMQ {

class IHandleCmd {
public:
    virtual void handleCommand(ZMQ::vionxCmdStruct cmd) = 0;
};


}
