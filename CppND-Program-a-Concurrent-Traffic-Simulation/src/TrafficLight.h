#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"
#include <thread>

enum TrafficLightPhase{red, green};

class Vehicle;

template <class T>
class MessageQueue
{
public:
    void send(T&& phase);
    T receive();
private:
    std::deque<T> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
};


class TrafficLight : public TrafficObject
{
public:
    TrafficLight();
    void waitForGreen();
    void simulate();
    void cycleThroughPhases();
    TrafficLightPhase toggleLight();
    TrafficLightPhase getCurrentPhase();

private:
    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
    std::chrono::_V2::system_clock::time_point start;
    float c_duration;
    MessageQueue<TrafficLightPhase> queue;
};

#endif