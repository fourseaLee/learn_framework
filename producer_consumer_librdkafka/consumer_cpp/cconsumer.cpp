#include "cconsumer.h"


static  bool run_ = true;

void sigterm (int sig) {
  run_ = false;
}

CConsumer::CConsumer()
{

}

CConsumer::CConsumer(const std::__cxx11::string &brokers, const std::__cxx11::string &topic, std::__cxx11::string groupid, int64_t offset)
    :brokers_(brokers),
     topics_(topic),
     groupid_(groupid),
     offset_(offset)
{

}

bool CConsumer::Init()
{

    RdKafka::Conf *conf = nullptr;

    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if ( !conf)
    {
        std::cerr << "RdKafka create global conf failed\n" << std::endl;
        return false;
    }
    std::string errstr;
    if (conf->set("bootstrap.servers",brokers_,errstr) !=
           RdKafka::Conf::CONF_OK)
    {
        std::cerr << "RdKafka conf set brokerlist failed :" << errstr << std::endl;
    }

    if (conf->set("group.id",groupid_,errstr) != RdKafka::Conf::CONF_OK)
    {
        std::cerr << "RdKafka conf set max.partition failed :" << errstr << std::endl;
    }

    kafka_consumer_ = RdKafka::Consumer::create(conf,errstr);

    if (!kafka_consumer_)
    {
        std::cerr << "failed to create consumer" << std::endl;
    }

    delete conf;

    RdKafka::Conf *tconf = nullptr;
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    if (!tconf)
    {
        std::cerr << "RdKafka create topic conf failed" << std::endl;
        return false;
    }if(tconf->set("auto.offset.reset", "smallest", errstr) != RdKafka::Conf::CONF_OK){
        fprintf(stderr, "RdKafka conf set auto.offset.reset failed : %s\n", errstr.c_str());
    }

    topic_ = RdKafka::Topic::create(kafka_consumer_, topics_, tconf, errstr);
    if(!topic_)
    {
        std::cerr << "RdKafka create topic failed : " <<  errstr << std::endl;
    }
    delete tconf;

    RdKafka::ErrorCode resp = kafka_consumer_->start(topic_, partition_, offset_);
    if (resp != RdKafka::ERR_NO_ERROR)
    {
        std::cerr << "failed to start consumer : " << RdKafka::err2str(resp);
    }

    return true;
}

bool CConsumer::Consume(int timeout_ms)
{
    RdKafka::Message *msg = nullptr;

    while(run_){
        msg = kafka_consumer_->consume(topic_, partition_, timeout_ms);
        consumer(msg, nullptr);
        kafka_consumer_->poll(0);
        delete msg;
    }

    kafka_consumer_->stop(topic_, partition_);
    if(topic_){
        delete topic_;
        topic_ = nullptr;
    }
    if(kafka_consumer_){
        delete kafka_consumer_;
        kafka_consumer_ = nullptr;
    }

    /*销毁kafka实例*/
    RdKafka::wait_destroyed(5000);
    return true;
}

void CConsumer::consumer(RdKafka::Message *message, void *opt)
{
    switch(message->err())
    {
        case RdKafka::ERR__TIMED_OUT:
            break;
        case RdKafka::ERR_NO_ERROR:
//            printf("%.*s\n",
//                static_cast<int>(message->len()),
//                static_cast<const>(message->payload()));
            std::cout << "message len : " << static_cast<int>(message->len()) << std::endl;
            std::cout << "message payload : " << static_cast<const char*>(message->payload()) << std::endl;
            last_offset_ = message->offset();
            break;
        case RdKafka::ERR__PARTITION_EOF:
            std::cout << "message len : " << static_cast<int>(message->len()) << std::endl;
            std::cout << "message payload : " << static_cast<const char*>(message->payload()) << std::endl;
            std::cerr << "%% Reached the end of the queue, offset: " << last_offset_ << std::endl;
            break;
        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run_ = false;
            break;
        default:
            std::cerr << "Consume failed: " << message->errstr() << std::endl;
            run_ = false;
            break;
     }
}
