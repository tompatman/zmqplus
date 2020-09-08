//
// Created by gmorehead on 3/28/18.
//


#pragma once

#include "tCtrlCmd.h"

namespace ZMQ {

class IHandleCmd {
public:
    virtual void handleCommand(ZMQ::tCtrlCmd cmd) = 0;
};


}
