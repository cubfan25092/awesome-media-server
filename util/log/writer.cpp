#include "writer.hpp"

using namespace Util::Log;

double Writer::get_time()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> end =
		std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>
		(end - start).count() / 1000000.0;
}


Writer::Writer() : log_file(new std::ofstream())
{
	prio = Priority::ERROR | Priority::WARN;
	start = std::chrono::high_resolution_clock::now();
}

Writer &Writer::get_instance()
{
	static Writer w;
	return w;
}


int Writer::set_log_file(const char *fname, bool erase_existing)
{
	std::lock_guard<std::mutex> lock(write_lock);
	if(log_file->is_open()){
		log_file->close();
	}
	log_file->exceptions(std::ios::badbit | std::ios::failbit);
	try{
		if(erase_existing){
			log_file->open(fname, std::ios::trunc);
		} else{
			log_file->open(fname);
		}
	} catch (std::ios::failure e){
		return ELOG_FILE;
	}
	if(log_file->is_open() && log_file->good()){
		boost::locale::generator gen;
		std::locale l = gen("");
		std::locale::global(l);
		log_file->imbue(l);
		return 0;
	} else{
		return ELOG_FILE;
	}
}

void Writer::set_priority(Priority_set p)
{
	if(p > Priority::ALL){
		writef("LogWriter", Priority::WARN, "Invalid priority set: %u", p);
	} else{
		prio |= p;
	}
}

void Writer::clear_priority(Priority_set p)
{
	if(p > Priority::ALL){
		writef("LogWriter", Priority::WARN, "Invalid priority clear: %u", p);
	} else{
		prio &= (~p);
	}
}

void Writer::write(const char *name, const Priority p, const std::string &msg)
{
	if(p & prio){
		write_internal(name, p, msg.c_str());
	}
}

void Writer::write(const char *name, const Priority p, const char *msg)
{
	if(p & prio){
		write_internal(name, p, msg);
	}
}

void Writer::writef(const char *name, const Priority p, const char * fmt, ...)
{
	if(p & prio){
		va_list args, rargs;
		va_start(args, fmt);
		va_copy(rargs, args);
		size_t needed = vsnprintf(nullptr, 0, fmt, args);
		char buffer[++needed];
		if(needed < 2 || vsnprintf(buffer, needed, fmt, rargs) < 0){
			write_internal("LogWriter", Util::Log::Priority::WARN, "Error writing formatted string to log.");
		} else{
			write_internal(name, p, buffer);
		}
	}
}

void Writer::write_internal(const char *name, const Priority p, const char *str)
{
	time_t now = time(0);
	if(log_file && log_file->is_open() && log_file->good()){
		//make sure that the string and name are utf8
		//it is fine to just use strlen as it will return the number of bytes
		if(!utf8::is_valid(name, name+strlen(name))){
			std::string replace; 
			utf8::replace_invalid(name, name+strlen(name), back_inserter(replace));
			name = replace.c_str();
		}
		if(!utf8::is_valid(str, str+strlen(str))){
			std::string replace; 
			utf8::replace_invalid(str, str+strlen(str), back_inserter(replace));
			str = replace.c_str();
		}
		std::lock_guard<std::mutex> lock(write_lock);
    	(*log_file) << "(" << boost::locale::as::date << now << "::" << boost::locale::as::time << now << ")" 
    		<< " [" << std::fixed << std::setprecision(6) << boost::locale::as::number << get_time() << std::resetiosflags(std::ios::fixed) 
			<< std::setprecision(0) << "::" << name << "] ";
		switch(p){
			case Priority::ERROR:
				(*log_file) <<  "(ERROR) " << str << std::endl;
				break;
			case Priority::WARN:
				(*log_file) <<  "(WARN) " << str << std::endl;
				break;
			case Priority::INFO:
				(*log_file) <<  "(INFO) " << str << std::endl;
				break;
			case Priority::DEBUG:
				(*log_file) <<  "(DEBUG) " << str << std::endl;
				break;
			default:
				(*log_file) <<  "(ERROR) " << u8"Unknown priority" << p << std::endl;
				break;
		}  
	}
}