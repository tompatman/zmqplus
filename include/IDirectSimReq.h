//
// Created by gmorehead on 7/15/19.
//


#pragma once

namespace ZMQ {

template<typename tDataSetType>
class IDirectSimReq {

public:

    /********************************************************************************************************
     * This object was created to eliminate the Req/Rep to the simulation thread in the EndToEnd unit tests
     ********************************************************************************************************/
    virtual void updateDeviceModelSimulation(tDataSetType &ds) = 0;

};

}

