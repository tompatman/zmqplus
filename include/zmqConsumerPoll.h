/*
 * zmqConsumerPoll.h
 *
 *  Created on: May 31, 2013
 *      Author: service
 */

#ifndef ZMQCONSUMERPOLL_H_
#define ZMQCONSUMERPOLL_H_

#include "zmq.hpp"
#include "iConsumer.h"
#include "core/ppcLogger.h"

#include <vector>

using std::vector;

class zmqConsumerPoll
{
public:
    zmqConsumerPoll(ppcLogger *log, uint num_consumers_to_poll, int timeout_ms=1000);
    virtual ~zmqConsumerPoll();

    // Thows XXX Exception if invalid
    void Add(iConsumer *a_consumer);

    // Returns the number of consumers with updated data.
    int Poll();

    // After Polling, call this for each consumer to see what was updated.
    bool WasSignaled(uint consumer_idx);

    // After Polling, call this for each consumer to see what was updated.
    bool WasSignaled(iConsumer *a_consumer);

    uint NumberOfConsumers() { return _num_consumers; };

private:

    ppcLogger *_log;

    const uint _num_consumers;

    const int _timeout_ms;

    zmq::pollitem_t *_items;

    vector<iConsumer *> _consumer_list;
};

#endif /* ZMQCONSUMERPOLL_H_ */
