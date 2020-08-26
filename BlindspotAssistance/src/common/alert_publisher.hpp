#include <zmq.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <string>
#define within(num) (int) ((float) num * random () / (RAND_MAX + 1.0))

class AlertPublisher {
    private:

    zmq::context_t context;
    zmq::socket_t publisher;
    char microservice [3];
    int microservice_size = 2;
    int date_size = 10;
    int time_size = 8;
    int spaces = 3;

    public:

    AlertPublisher(const char* ms){
        std::cout << "AlertPublisher is working for " << ms << " microservice." << std::endl;
        strcpy( microservice, ms );
        // Prepare our context and publisher
        publisher = zmq::socket_t(context, ZMQ_PUB);
        publisher.connect("tcp://localhost:4444");
        // Initialize random number generator
        srandom ((unsigned) time (NULL));
    }

    void sendAlert(const char* payload){
        std::cout << "Microservice " << microservice << " sending alert..." << std::endl;

        time_t now = time(0);
        tm *ltm = localtime(&now);

        // Get values that will fool the boss
        char date[11], time[10];
        snprintf ((char *) date, 11, "%.2d/%.2d/%.4d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
        snprintf ((char *) time, 10, "%.2d:%.2d:%.2d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        
        // Send message to all subscribers
        int payload_size = (int) strlen(payload) + 1;
        int message_size = microservice_size + date_size + time_size + payload_size + spaces;
        zmq::message_t message(message_size);
        snprintf ((char *) message.data(), message_size, "%s %s %s %s", microservice, date, time, payload);
        std::cout << "Current Message: " << message << std::endl;
        //publisher.send(message, ZMQ_DONTWAIT);
        publisher.send(message, ZMQ_DONTWAIT);
    }
};