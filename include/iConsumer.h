/*
 * iConsumer.h
 *
 *  Created on: May 31, 2013
 *      Author: service
 */

#ifndef ICONSUMER_H_
#define ICONSUMER_H_


#include "zmq/zmq.hpp"


class iConsumer
{
public:
    iConsumer() {};
    virtual ~iConsumer() {};

    virtual void setup_que(int timeout_ms = 500) = 0;

    // Returns true if there was an update.
    virtual bool update_data_share() = 0;

    virtual zmq::socket_t *get_sub_socket_for_poll() = 0;

};

#endif /* ICONSUMER_H_ */
