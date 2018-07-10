#include "cconsumer.h"
#include <iostream>
int main(int argc, char *argv[])
{

    //std::vector<std::string> topics;
    std::string topics = "test_2";
    std::string brokers = "192.168.0.18:9092";
    std::string group = "1";


    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    std::shared_ptr<CConsumer> kafka_consumer_client_ = std::make_shared<CConsumer>(brokers, topics, group, 0);
    //std::shared_ptr<kafka_consumer_client> kafka_consumer_client_ = std::make_shared<kafka_consumer_client>();
    if (!kafka_consumer_client_->Init())
    {
        std::cerr << "init failed " << std::endl;
    }
    else
    {
        std::cout << "start kafka consumer\n";
        kafka_consumer_client_->Consume(1000);
    }

    std::cout <<  "consumer exit successfully!" << std::endl;

    return 0;

}
