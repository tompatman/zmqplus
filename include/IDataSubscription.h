//
// Created by gmorehead on 8/2/17.
//


#pragma once

namespace ZMQ {

class IDataSubscription {
public:
    IDataSubscription()
    {};

    virtual ~IDataSubscription()
    {
    }

    virtual bool update(const bool bForceBlocking = false) = 0;

    virtual Poco::Int64 mSecondsSinceLastComm() = 0;

};

}
