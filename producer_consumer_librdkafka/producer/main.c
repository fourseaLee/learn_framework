#include <stdio.h>
#include <signal.h>

#include <string.h>
#include <librdkafka/rdkafka.h>

static int  run =1;
static void stop(int sig)
{
	run = 0;
	fclose(stdin);
}

static  void dr_msg_cb(rd_kafka_t *rk,rd_kafka_message_t*rkmessage,void*opaque)
{
	if ( rkmessage->err )
		fprintf(stderr,"%% Message delivery failed: %s \n",
		rd_kafka_err2str(rkmessage->err));
	else
		fprintf(stderr,"%% Message deliverea (%zd bytes,"" partition %"PRId32")\n",
		rkmessage->len,rkmessage->partition);
}


static rd_kafka_t *rk1;
static rd_kafka_topic_t *rkt_test;
void test()
{
	char errstr[1024];
	rd_kafka_conf_t *conf = rd_kafka_conf_new();
	char* brokers = "192.168.0.18:9092";
	if (rd_kafka_conf_set(conf, "bootstrap.servers", brokers, errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		printf(stderr,"Failed to broker: %s error: %s\n",brokers,errstr);
		return;	
	}
	if (rd_kafka_conf_set(conf, "queue.buffering.max.ms", "1", errstr, sizeof(errstr)) != RD_KAFKA_CONF_OK) 
	{
		printf("Set kafka buffering: %s fail: %s", brokers, errstr);
		return ;
	}
//	rd_kafka_conf_set_log_cb(conf, on_logger);
//	rd_kafka_conf_set_dr_msg_cb(conf, on_delivery);

	rk1 = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
	if (rk1 == NULL) {
		printf("Failed to create new producer: %s", errstr);
		return ;
	}

	rkt_test = rd_kafka_topic_new(rk1, "test_one", NULL);
	if (rkt_test == NULL) {
		printf("Failed to create topic object");
		return ;
	}
}

int main(int argc,char**argv)
{
	
//	test();
//	return 0;

	rd_kafka_t *rk ;
	rd_kafka_topic_t  *rkt;
	rd_kafka_conf_t *conf;
	char  errstr[512];
	char  buf[512];
	const char * brokers;
	const char * topic;
	if (argc != 3)
	{
		fprintf(stderr,"%% Usage: %s <broker> <topic> \n",argv[0]);
		return 1;
	}

	brokers = argv[1];
	topic = argv[2];
	conf = rd_kafka_conf_new();
	printf("brokers is : %s \n",brokers);
	printf("topic is: %s \n",topic);

	if (rd_kafka_conf_set(conf,"bootstrap.servers",brokers,errstr,sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		fprintf(stderr,"%s\n",errstr);
		return 1;
	}

	rd_kafka_conf_set_dr_msg_cb(conf,dr_msg_cb);
	
	rk = rd_kafka_new(RD_KAFKA_PRODUCER,conf,errstr,sizeof(errstr));
	if (!rk)
	{
		fprintf(stderr,"%% Failed to create new producer: %s\n",errstr);
		return 1;
	}

	rkt = rd_kafka_topic_new(rk,topic,NULL);
	if (!rkt)
	{
		fprintf(stderr,"%% Failed to create topic object:%s\n",
		rd_kafka_err2str(rd_kafka_last_error()));
		rd_kafka_destroy(rk);
		return 1;
	}


	signal(SIGINT, stop);
	
	while(run && fgets(buf,sizeof(buf),stdin))
	{
		size_t len  = strlen(buf);

		if ( buf[len-1] == '\n')
			buf[--len] = '\0';

		if ( len == 0 )
		{
			rd_kafka_poll(rk,0);
			continue;
		}
	
retry:

		if (rd_kafka_produce(rkt,RD_KAFKA_PARTITION_UA,RD_KAFKA_MSG_F_COPY,buf,len,NULL,0,NULL) == -1)
		{
			fprintf(stderr,"%% Failed to produce to topic  %s: %s\n",
			rd_kafka_topic_name(rkt),rd_kafka_err2str(rd_kafka_last_error()));

			if ( rd_kafka_last_error() == RD_KAFKA_RESP_ERR__QUEUE_FULL )
			{
				rd_kafka_poll(rk,1000);
				goto retry;
			}
		}
		else 
		{
			fprintf(stderr,"%% Enqueued message (%zd bytes) for topic  %s\n",len,
			rd_kafka_topic_name(rkt));
		}
		rd_kafka_poll(rk,0);
	}

	fprintf(stderr, "%% Flushing final message.. \n");

	rd_kafka_flush(rk,10*1000);

	rd_kafka_topic_destroy(rkt);

	rd_kafka_destroy(rk);

	return 0;
}
