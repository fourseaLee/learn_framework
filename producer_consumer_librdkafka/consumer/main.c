#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <error.h>
#include <error.h>
#include <getopt.h>

#include <librdkafka/rdkafka.h>

static int run  = 1;

static rd_kafka_t  *rk;
static rd_kafka_topic_partition_list_t *topics;

static void stop(int sig)
{
	if (!run)
		exit(1);
	run = 0;
	fclose(stdin);
}

static void sig_usr1(int sig)
{
	rd_kafka_dump(stdout,rk);
}

static void msg_consume(rd_kafka_message_t*rkmessage,void *opaque)
{
	if (rkmessage->err)
	{
		if (rkmessage->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
		{
			fprintf(stderr,
			"%% Consumer reached end of %s [%"PRId32"] "
			"message queue at offset % "PRId64"\n",
			rd_kafka_topic_name(rkmessage->rkt),
			rkmessage->partition,rkmessage->offset);
			return;
		}

		if (rkmessage->rkt)
		{
			fprintf(stderr, "%% Consume error for"
			"topic \"%s\" [%"PRId32"]"
			"offset %"PRId64":%s\n",
			rd_kafka_topic_name(rkmessage->rkt),
			rkmessage->partition,
			rkmessage->offset,
			rd_kafka_message_errstr(rkmessage));
		}
		else 
		{
			fprintf(stderr,"%% Consumer error:  %s: %s\n",
			rd_kafka_err2str(rkmessage->err),
			rd_kafka_message_errstr(rkmessage));
		}


		if ( rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_PARTITION ||
			 rkmessage->err == RD_KAFKA_RESP_ERR__UNKNOWN_TOPIC)
			 run = 0;

		return ;
	}
	fprintf(stdout ,"%% Message (topic %s [%"PRId32"],offset %"PRId64",%zd bytes):\n",
	rd_kafka_topic_name(rkmessage->rkt),
	rkmessage->partition,
	rkmessage->offset,rkmessage->len);

	if (rkmessage->key_len)
	{
		printf("Key: %.*s\n",(int)rkmessage->key_len,(char*)rkmessage->key);
	}

	printf("%.*s\n",(int)rkmessage->len,(char*)rkmessage->payload);
}

int initKafka(char* brokers,char*group,char* topic)
{
	rd_kafka_conf_t *conf;
	rd_kafka_topic_conf_t *topic_conf;
	rd_kafka_resp_err_t err;
	char tmp[16];
	char errstr[512];
	
	conf = rd_kafka_conf_new();

	snprintf(tmp,sizeof(tmp),"%i",SIGIO);
	rd_kafka_conf_set(conf,"internal.termination.signal",tmp,NULL,0);

	topic_conf = rd_kafka_topic_conf_new();

	if (!group)
		group = "rdkafka_consumer_example";
	if (rd_kafka_conf_set(conf,"group.id",group,errstr,sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		fprintf(stderr,"%% %s\n",errstr);
		return -1;
	}

	if (rd_kafka_topic_conf_set(topic_conf,"offset.store.method","broker",errstr,
	sizeof(errstr)) != RD_KAFKA_CONF_OK)
	{
		fprintf(stderr, "%% %s\n",errstr);
		return -1;
	}

	rd_kafka_conf_set_default_topic_conf(conf,topic_conf);

	rk = rd_kafka_new(RD_KAFKA_CONSUMER,conf,errstr,sizeof(errstr));

	if(!rk)
	{
		fprintf(stderr,"%% Failed to create new consumer:%s\n",errstr);
		return -1;
	}

	rd_kafka_poll_set_consumer(rk);

	topics  = rd_kafka_topic_partition_list_new(1);
	rd_kafka_topic_partition_list_add(topics,topic,-1);

	if ( (err = rd_kafka_subscribe(rk,topics)))
	{
		fprintf(stderr,"%% Failed to start consuming topics :%s\n",rd_kafka_err2str(err));
		return -1;
	}

	return 1;
}


int main (int argc,char*argv[])
{
	char *brokers = "localhost::9092";
	char *group = NULL;
	char *topic = NULL;


	int opt ;
	rd_kafka_resp_err_t err;

	while ((opt = getopt(argc,argv,"g:b:t:qd:eX:As:DO")) != -1)
	{
		switch (opt)
		{
		case 'b':
			brokers = optarg;
			break;
		case 'g':
			group = optarg;
			break;
		case 't':
			topic = optarg;
			break;
		default:
			break;
		}
	}


	signal(SIGINT,stop);

	signal(SIGUSR1,sig_usr1);

	if (!initKafka(brokers,group,topic))
	{
		fprintf(stderr, "kafka server initialize error\n");
	}
	else 
	{
		while(run)
		{
			rd_kafka_message_t*rkmessage;
			rkmessage = rd_kafka_consumer_poll(rk,1000);
			if (rkmessage)
			{
				msg_consume(rkmessage,NULL);
				rd_kafka_message_destroy(rkmessage);
			}
		}
	}

done:
	err = rd_kafka_consumer_close(rk);
	if (err)
	{	
		fprintf(stderr, "%% Failed to close  consumer:  %s \n", rd_kafka_err2str(err));
	}
	else 
	{
		fprintf(stderr, "%% Consumer closed \n");
	}


	rd_kafka_topic_partition_list_destroy(topics);

	rd_kafka_destroy(rk);

	run = 5;

	while (run-- >0 && rd_kafka_wait_destroyed(1000) == -1 )
	{
		printf("Waiting for  librdkafka to decommission\n");
	}

	if (run <= 0)
	{
		rd_kafka_dump(stdout,rk);
	}

	return 0;
}
