#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this](){return (_queue.size() > 0);});
    T front = std::move(_queue.front());
    _queue.pop_front();
    return front;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(msg);
    _cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
    c_duration = 4.0 + 2*((float) rand()/(float)RAND_MAX);
    start = std::chrono::system_clock::now();
}

void TrafficLight::waitForGreen()
{
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto light = queue.receive();
        std::cout << light << " in function waitforgreen";
        if(light == green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}


TrafficLightPhase TrafficLight::toggleLight(){
    if(_currentPhase == red) _currentPhase = green;
    else _currentPhase = red;
    return _currentPhase;
}

void TrafficLight::simulate()
{
    std::thread t(&TrafficLight::cycleThroughPhases, this);
    t.detach();
    threads.push_back(std::move(t));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<float> diff = end-start;

        if(diff.count()>c_duration){
            std::unique_lock<std::mutex> lock(_mutex);
            auto phase = toggleLight();
            _currentPhase = phase;
            c_duration = 4.0 + 2*((float) rand()/(float)RAND_MAX);
            start = std::chrono::system_clock::now();
            queue.send(std::move(phase));
        }
    }

}

