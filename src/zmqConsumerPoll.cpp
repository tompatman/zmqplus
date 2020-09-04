/*
 * zmqConsumerPoll.cpp
 *
 *  Created on: May 31, 2013
 *      Author: service
 */

#include "../include/zmqConsumerPoll.h"

using std::string;

zmqConsumerPoll::zmqConsumerPoll(ppcLogger *log, uint num_consumers_to_poll, int timeout_ms)
: _log(log)
, _num_consumers(num_consumers_to_poll)
, _timeout_ms(timeout_ms)
{
    _items = (zmq::pollitem_t *) new zmq::pollitem_t[_num_consumers];

}

zmqConsumerPoll::~zmqConsumerPoll()
{
    delete[] _items;

}


void zmqConsumerPoll::Add(iConsumer *a_consumer)
{
    uint idx = _consumer_list.size();

    poco_assert(idx < _num_consumers);

    _consumer_list.push_back(a_consumer);

    _items[idx].events = ZMQ_POLLIN;
    _items[idx].fd = 0;
    _items[idx].socket =  a_consumer->get_sub_socket_for_poll();
}


int zmqConsumerPoll::Poll()
{
    poco_assert(_consumer_list.size() == _num_consumers);

    int number_signaled = zmq::poll(_items, _num_consumers, _timeout_ms);

    if(number_signaled > 0)        // Any updates
    {
        int count = 0;
        for (uint i = 0; i < _num_consumers; ++i)
        {
            if(_items[i].revents & ZMQ_POLLIN)
            {
                if(!_consumer_list[i]->update_data_share())
                    _items[i].revents = 0;                      // Remove the signal if it does not return true.

                count++;
            }
        }

        poco_assert(count == number_signaled);      // Better be the same.
    }
    if(number_signaled < 0)
    {
        _log->log(LG_ERR, "zmq::poll returned error[%d]:", zmq_errno(), zmq_strerror(zmq_errno()));
        throw Poco::RuntimeException(string("zmq::poll returned error[]:") + zmq_strerror(zmq_errno()));
    }

    return number_signaled;
}

bool zmqConsumerPoll::WasSignaled(uint consumer_idx)
{
    poco_assert(consumer_idx < _num_consumers);

    return (_items[consumer_idx].revents & ZMQ_POLLIN);
}

bool zmqConsumerPoll::WasSignaled(iConsumer *a_consumer)
{
    bool was_signaled = false;

    uint i = 0;
    for (; i < _num_consumers; ++i)
    {
        if(_consumer_list[i] == a_consumer)
        {
            was_signaled = (_items[i].revents & ZMQ_POLLIN);
            break;
        }
    }

    if(i == _num_consumers)
    {
        _log->log(LG_ERR, "The consumer was not found in the list!");
        throw Poco::RuntimeException("WasSignaled() : The consumer was not found in the list!");
    }

    return was_signaled;
}
