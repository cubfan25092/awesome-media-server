#ifdef _MSC_VER
#if (_MSC_VER < 1700) 
#error This requires Visual Studio 2012 or later
#else
//for some idiotic reason, MS hasnt introduced support for unicode string literals
#define u8 
#endif
#endif

#include "../writer.hpp"
#include <pthread.h>
#include <unistd.h>

using namespace Util;
#define LOG_NAME u8"LogTests"

void *thread_write(void *n)
{
	char buf[80];
	snprintf(buf, 80, "thread%lu.txt", pthread_self());
	Log::Writer::get_instance().set_log_file(buf, false);	
	for(int i = 0; i < 20; ++i){
		Log::Writer::get_instance().writef(LOG_NAME, Log::Priority::ERROR, "thread %lu", pthread_self());
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	//console has been tested. now for file
	int ret = Log::Writer::get_instance().set_log_file("test.txt", true);
	if(ret == ELOG_FILE){
		std::cout << "FAILED: couldnt open file" << std::endl;
		return -1;
	}
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::INFO, "test info not written by default");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::DEBUG, "test debug not written by default");
	Log::Writer::get_instance().set_priority(Log::Priority::DEBUG | Log::Priority::INFO);
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::DEBUG, "test debug written on flag");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::ERROR, "test error still written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::WARN, "test warn still written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::INFO, "test info written on flag");
	Log::Writer::get_instance().clear_priority(Log::Priority::DEBUG | Log::Priority::INFO);
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::INFO, "test info not written on flag");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::ERROR, "test error still written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::WARN, "test warn still written");
	Log::Writer::get_instance().clear_priority(Log::Priority::ALL);
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::DEBUG,"test debug not written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::ERROR, "test error not written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::WARN, "test warn not written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::INFO, "test info not written");
	Log::Writer::get_instance().set_priority(Log::Priority::ERROR | Log::Priority::WARN);
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::ERROR, "test error still written");
	Log::Writer::get_instance().write(LOG_NAME, Log::Priority::WARN, "test warn still written");
	//test writef
	Log::Writer::get_instance().writef(LOG_NAME, Log::Priority::ERROR, "derp%d%s%lu", 4, "lol", 56754734);
	logwf(LOG_NAME,"%s%d" ,"lol", 42);
	//test unicode
	loge(LOG_NAME, u8"ハローワールド");
	//thread tests
	pthread_t threads[8];
	for(int i = 0; i < 8; ++i){
		pthread_create(&threads[i], NULL, thread_write, NULL);
	}
	for(int i = 0; i < 8; ++i){
		pthread_join(threads[i], NULL);
	}
	return 0;
}
