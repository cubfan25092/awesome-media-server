#pragma once
#ifdef _MSC_VER
#if (_MSC_VER < 1700) 
#error This requires Visual Studio 2012 or later
#else
//for some idiotic reason, MS hasnt introduced support for unicode string literals
#define u8 
#endif
#endif


#include <cstring>
#include <cstdlib>
#include <stdarg.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <iostream>
#include <chrono>
#include <string>
#include <memory>
#include <cstdio>
#include <ctime>
#include <stddef.h>
#include <boost/locale.hpp>
#include "../../external/lib/utf8/utf8.h"

#define ELOG_FILE (-7)

/* macro for writing to error */
#define loge(name, msg) \
	do{ \
		Util::Log::Writer::get_instance().write(name, Util::Log::Priority::ERROR, msg); \
	} while(0) 

/* macro for writing to warn */
#define logw(name, msg) \
	do{ \
		Util::Log::Writer::get_instance().write(name, Util::Log::Priority::WARN, msg); \
	} while(0) 

/* macro for writing to info */
#define logi(name, msg) \
	do{ \
		Util::Log::Writer::get_instance().write(name, Util::Log::Priority::INFO, msg); \
	} while(0) 

/* macro for writing to debug */
#define logd(name, msg) \
	do{ \
		Util::Log::Writer::get_instance().write(name, Util::Log::Priority::DEBUG, msg); \
	} while(0) 
/* macro for writef to error */
#define logef(name, ...) \
	do{ \
		Util::Log::Writer::get_instance().writef(name, Util::Log::Priority::ERROR, __VA_ARGS__); \
	} while(0)


/* macro for writef to warn */
#define logwf(name, ...) \
	do{ \
		Util::Log::Writer::get_instance().writef(name, Util::Log::Priority::WARN, __VA_ARGS__); \
	} while(0)

/* macro for writef to info */
#define logif(name, ...) \
	do{ \
		Util::Log::Writer::get_instance().writef(name, Util::Log::Priority::INFO, __VA_ARGS__); \
	} while(0)	

/* macro for writef to debug */
#define logdf(name, ...) \
	do{ \
		Util::Log::Writer::get_instance().writef(name, Util::Log::Priority::DEBUG, __VA_ARGS__); \
	} while(0)

namespace Util
{
	namespace Log
	{
	/* a priority is a byte. each level is represented by one bit being set in this byte
		thus a set of priorities is a byte */
	typedef uint8_t Priority_set;
	enum Priority
		{
			ERROR = 0x01,
			WARN = 0x02,
			INFO = 0x04,
			DEBUG = 0x08,
			ALL = 0x0f

		};
		class Writer
		{

		public:

		static Writer &get_instance();

				/* sets the log file to the file with name fname
				 * any other log file will be closed
				 * return: 0 on sucess, (file is opened and writable)
				 * ELOG_FILE on failure
				 */
				int set_log_file(const char *fname, bool erase_existing);

				/* adds the priorities to the writer */
				void set_priority(Util::Log::Priority_set p);

				/* clears the given priorities on the writer */
				void clear_priority(Util::Log::Priority_set p);

				/* writes the string message to the log. the module
				 * writing this line is name, and the priority is p
				 * this does not guarantee that this line is written
				 * for example, there may be an error, or the priority
				 * may be silenced
				 */
				void write(const char *name, const Util::Log::Priority p, const std::string &msg);

				/* writes the cstring message to the log. the module
				 * writing this line is name, and the priority is p
				 * this does not guarantee that this line is written
				 * for example, there may be an error, or the priority
				 * may be silenced
				 */
				void write(const char *name, const Util::Log::Priority p, const char *msg);

				/* works like printf just for writing to the log */
				void writef(const char *name, const Util::Log::Priority p,  const char *fmt, ...);
		private:
				double get_time();
				Writer();
				Writer(Writer &copy){};
				void operator=(Writer &rhs);
				Writer(Writer &&other){};
				void operator=(Writer &&rhs);
				void write_internal(const char *name, Util::Log::Priority p, const char *str);
				std::unique_ptr<std::ofstream> log_file;
				Util::Log::Priority_set prio;
				std::chrono::time_point<std::chrono::high_resolution_clock> start;
				std::mutex write_lock;

		};
	}
}