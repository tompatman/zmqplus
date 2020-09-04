/*
 * ISubscribe.h
 *
 *  Created on: Dec 2, 2015
 *      Author: service
 */

#pragma once


#include "zCmdStruct.h"

namespace ZMQ {

template<typename msgData>
class ISubscribe {

public:
    virtual ~ISubscribe() = default;

    virtual void Init(const bool bConflate = false, const int queueSize = 100, const bool bBlock = false) = 0;

    virtual bool readData(msgData &newData, const bool bForceBlocking = false) = 0;

};

}

