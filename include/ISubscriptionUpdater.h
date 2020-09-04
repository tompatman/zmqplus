//
// Created by gmorehead on 8/29/16.
//

#ifndef VIONX_CODE_ISUBSCRIPTIONUPDATER_H
#define VIONX_CODE_ISUBSCRIPTIONUPDATER_H

#include "string"

namespace ZMQ {

class ISubscriptionUpdater {
public:
    virtual ~ISubscriptionUpdater()
    {};

    virtual void update() = 0;

    virtual bool recv_string(std::string *result) = 0;
};

}

#endif //VIONX_CODE_ISUBSCRIPTIONUPDATER_H
