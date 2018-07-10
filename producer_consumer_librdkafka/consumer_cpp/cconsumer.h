#ifndef CCONSUMER_H
#define CCONSUMER_H
#include <string>
#include <librdkafka/rdkafkacpp.h>
#include <iostream>
#include <getopt.h>
#include <csignal>
#include <memory>
#include <vector>


void sigterm (int sig);
class CConsumer
{
public:
    CConsumer();
    CConsumer(const std::string&brokers,const std::string&topic,std::string groupid,int64_t offset);
public:
    bool Init();
    bool Consume(int timeout_ms);
    void Finalize();
private:
    void consumer(RdKafka::Message*message, void *opt);
    std::string brokers_;
    std::string topics_;
    std::string groupid_;

    int64_t last_offset_ =0;
    RdKafka::Consumer *kafka_consumer_ = nullptr;
    RdKafka::Topic *topic_ = nullptr;
    int64_t offset_ = RdKafka::Topic::OFFSET_BEGINNING;
    int32_t partition_ = 0;
};

#endif // CCONSUMER_H
