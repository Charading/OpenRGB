#include "LogManager.h"

#include <stdarg.h>
#include <iostream>
#include "ResourceManager.h"

#include "filesystem.h"

#ifdef linux
#include <signal.h>
#include <setjmp.h>
// for pthread_self - it is important to only use safe spots for the threads thwy were created in
#include <pthread.h>
#endif

static const char* log_codes[] = {"CRITICAL", "ERROR", "Message", "Warning", "Notice", "[verbose]", "Debug"};

LogManager::LogManager()
{
}

LogManager* LogManager::get()
{
    static LogManager* _instance = nullptr;
    static std::mutex instance_mutex;
    std::lock_guard<std::mutex> grd(instance_mutex);

    /*-------------------------------------------------*\
    | Create a new instance if one does not exist       |
    \*-------------------------------------------------*/
    if(!_instance)
    {
        _instance = new LogManager();
    }
    
    return _instance;
}

void LogManager::configure(json config, const std::string &defaultDir)
{
    std::lock_guard<std::mutex> grd(entry_mutex);

    /*-------------------------------------------------*\
    | If the log is not open, create a new log file     |
    \*-------------------------------------------------*/
    if(!log_stream.is_open())
    {
        std::string logname = "OpenRGB_#.log";

        /*-------------------------------------------------*\
        | If the logfile is defined in the configuration,   |
        | use the configured name                           |
        \*-------------------------------------------------*/
        if(config.contains("logfile"))
        {
            const json& logfile_obj = config["logfile"];
            if(logfile_obj.is_string())
            {
                std::string tmpname = config["logfile"];
                if(!tmpname.empty())
                {
                    logname = tmpname;
                }
            }
        }

        /*-------------------------------------------------*\
        | If the # symbol is found in the log file name,    |
        | replace it with a timestamp                       |
        \*-------------------------------------------------*/
        time_t t = time(0);
        struct tm* tmp = localtime(&t);
        char time_string[64];
        snprintf(time_string, 64, "%04d%02d%02d_%02d%02d%02d", 1900 + tmp->tm_year, tmp->tm_mon + 1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

        size_t oct = logname.find("#");
        if(oct != logname.npos)
        {
            logname.replace(oct, 1, time_string);
        }

        /*-------------------------------------------------*\
        | If the path is relative, use logs dir             |
        \*-------------------------------------------------*/
        filesystem::path p = logname;
        if(p.is_relative())
        {
            p = defaultDir + "logs/";
            p.append(logname);
        }
        filesystem::create_directories(p.parent_path());

        /*-------------------------------------------------*\
        | Open the logfile                                  |
        \*-------------------------------------------------*/
        log_stream.open(p);

        /*-------------------------------------------------*\
        | Print Git Commit info, version, etc.              |
        \*-------------------------------------------------*/
        log_stream << "    OpenRGB v" << VERSION_STRING << std::endl;
        log_stream << "    Commit: " << GIT_COMMIT_ID << " from " << GIT_COMMIT_DATE << std::endl;
        log_stream << "    Launched: " << time_string << std::endl;
        log_stream << "====================================================================================================" << std::endl;
        log_stream << std::endl;
    }

    /*-------------------------------------------------*\
    | Check loglevel configuration                      |
    \*-------------------------------------------------*/
    if(config.contains("loglevel"))
    {
        const json& loglevel_obj = config["loglevel"];

        /*-------------------------------------------------*\
        | Set the log level if configured                   |
        \*-------------------------------------------------*/
        if(loglevel_obj.is_number_integer())
        {
            loglevel = loglevel_obj;
        }
    }

    /*-------------------------------------------------*\
    | Flush the log                                     |
    \*-------------------------------------------------*/
    _flush();
}

void LogManager::_flush()
{
    /*-------------------------------------------------*\
    | If the log is open, write out buffered messages   |
    \*-------------------------------------------------*/
    if(log_stream.is_open())
    {
        for(size_t msg = 0; msg < temp_messages.size(); ++msg)
        {
            if(temp_messages[msg]->level <= loglevel)
            {
                // Put the timestamp here
                log_stream << log_codes[temp_messages[msg]->level] << ": ";
                log_stream << temp_messages[msg]->buffer;
         
                if(print_source)
                {
                    log_stream << " [" << temp_messages[msg]->filename << ":" << temp_messages[msg]->line << "]";
                }
                
                log_stream << std::endl;
            }
        }

        /*-------------------------------------------------*\
        | Clear temp message buffers after writing them out |
        \*-------------------------------------------------*/
        temp_messages.clear();
    }

    /*-------------------------------------------------*\
    | Flush the stream                                  |
    \*-------------------------------------------------*/
    log_stream.flush();
}

void LogManager::flush()
{
    std::lock_guard<std::mutex> grd(entry_mutex);
    _flush();
}

void LogManager::_append(const char* filename, int line, unsigned int level, const char* fmt, va_list va)
{
    /*-------------------------------------------------*\
    | If a critical message occurs, enable source       |
    | printing and set loglevel and verbosity to highest|
    \*-------------------------------------------------*/
    if(level == LL_CRITICAL)
    {
        print_source = true;
        loglevel = LL_DEBUG;
        verbosity = LL_DEBUG;
    }

    /*-------------------------------------------------*\
    | Create a new message                              |
    \*-------------------------------------------------*/
    PLogMessage mes = std::make_shared<LogMessage>();

    /*-------------------------------------------------*\
    | Resize the buffer, then fill in the message text  |
    \*-------------------------------------------------*/
    va_list va2;
    va_copy(va2, va);
    int len = vsnprintf(nullptr, 0, fmt, va);
    mes->buffer.resize(len + 1);
    vsnprintf(&(mes->buffer[0]), len + 1, fmt, va2);

    /*-------------------------------------------------*\
    | Fill in message information                       |
    \*-------------------------------------------------*/
    mes->level      = level;
    mes->filename   = filename;
    mes->line       = line;

    /*-------------------------------------------------*\
    | If the message is within the current verbosity,   |
    | print it on the screen                            |
    | TODO: Put the timestamp here                      |
    \*-------------------------------------------------*/
    if(level <= verbosity)
    {
        std::cout << mes->buffer;
        if(print_source)
        {
            std::cout << " [" << mes->filename << ":" << mes->line << "]";
        }
        std::cout << std::endl;
    }

    /*-------------------------------------------------*\
    | If the message level is LL_MESSAGE or lower, add  |
    | it to the error queue                             |
    \*-------------------------------------------------*/
    if(level <= LL_MESSAGE)
    {
        for(size_t idx = 0; idx < error_callbacks.size(); ++idx)
        {
            error_callbacks[idx].first(error_callbacks[idx].second, mes);
        }
    }

    /*-------------------------------------------------*\
    | Add the message to the logfile queue              |
    \*-------------------------------------------------*/
    temp_messages.push_back(mes);

    /*-------------------------------------------------*\
    | Flush the queues                                  |
    \*-------------------------------------------------*/
    _flush();
}

void LogManager::append(const char* filename, int line, unsigned int level, const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    std::lock_guard<std::mutex> grd(entry_mutex);
    _append(filename, line, level, fmt, va);

    va_end(va);
}

void LogManager::setLoglevel(unsigned int level)
{
    /*-------------------------------------------------*\
    | Check that the new log level is valid, otherwise  |
    | set it within the valid range                     |
    \*-------------------------------------------------*/
    if(level < LL_CRITICAL)
    {
        level = LL_CRITICAL;
    }

    if(level > LL_DEBUG)
    {
        level = LL_DEBUG;
    }

    LOG_DEBUG("Loglevel set to %d", level);

    /*-------------------------------------------------*\
    | Set the new log level                             |
    \*-------------------------------------------------*/
    loglevel = level;
}

void LogManager::setVerbosity(unsigned int level)
{
    /*-------------------------------------------------*\
    | Check that the new verbosity is valid, otherwise  |
    | set it within the valid range                     |
    \*-------------------------------------------------*/
    if(level < LL_CRITICAL)
    {
        level = LL_CRITICAL;
    }

    if(level > LL_DEBUG)
    {
        level = LL_DEBUG;
    }

    LOG_DEBUG("Verbosity set to %d", level);

    /*-------------------------------------------------*\
    | Set the new verbosity                             |
    \*-------------------------------------------------*/
    verbosity = level;
}

void LogManager::setPrintSource(bool v)
{
    LOG_DEBUG("Source code location printouts were %s", v ? "enabled" : "disabled");
    print_source = v;
}

void LogManager::registerErrorCallback(LogErrorCallback callback, void* receiver)
{
    std::lock_guard<std::mutex> grd(entry_mutex);

    error_callbacks.push_back(LogErrorBlock(callback, receiver));
}

void LogManager::unregisterErrorCallback(LogErrorCallback callback, void* receiver)
{
    std::lock_guard<std::mutex> grd(entry_mutex);

    for(size_t idx = 0; idx < error_callbacks.size(); ++idx)
    {
        if(error_callbacks[idx].first == callback && error_callbacks[idx].second == receiver)
        {
            error_callbacks.erase(error_callbacks.begin() + idx);
        }
    }
}

#ifdef linux

// This class allows to pack jmp_buf into vector (vector requires the class to be either movable or copyable)
class jmp_buf_wrapper
{
public:
    jmp_buf data;
    const char* fname;
    int line;
    jmp_buf_wrapper(){}
    ~jmp_buf_wrapper(){}
    jmp_buf_wrapper(jmp_buf_wrapper& other){memcpy(data, other.data, sizeof(jmp_buf));}
    jmp_buf_wrapper(jmp_buf_wrapper&& other){memmove(data, other.data, sizeof(jmp_buf));}
};

static std::vector<pthread_t> reroute_threads;
static std::vector<std::vector<jmp_buf_wrapper> > reroute_jumps;

static void signalHandler(int s)
{
    pthread_t thid = pthread_self();
    // Find the correct thread
    size_t k = 0;
    for(; k < reroute_threads.size(); ++k)
    {
        if(reroute_threads[k] == thid)
        {
            break;
        }
    }
    if(k == reroute_threads.size() || reroute_jumps[k].empty())
    {
        // Couldn't find the thread, allow to crash
        LOG_CRITICAL("Signal [%d] received, but no trap was set for it, OpenRGB will be terminated", s);
        abort();
    }
    size_t l = reroute_jumps[k].size() - 1;
    LogManager::get()->append(reroute_jumps[k][l].fname, reroute_jumps[k][l].line, LL_WARNING, "Signal [%d] received, attempting to recover", s);
    jmp_buf target;
    memcpy(target, reroute_jumps[k][l].data, sizeof(jmp_buf));
    reroute_jumps[k].resize(l);
    signal(s, signalHandler);
    longjmp(target, s);
}

bool failsafeCheck(const char* fname, int line)
{
    signal(SIGSEGV, signalHandler);
    signal(SIGFPE, signalHandler);
    signal(SIGBUS, signalHandler);
    signal(SIGILL, signalHandler);

    pthread_t thid = pthread_self();
    size_t k = 0;
    for(; k < reroute_threads.size(); ++k)
    {
        if(reroute_threads[k] == thid)
        {
            break;
        }
    }
    if(k == reroute_threads.size())
    {
        // Couldn't find the thread, add it
        reroute_threads.push_back(thid);
        reroute_jumps.emplace_back();
    }
    size_t l = reroute_jumps[k].size();
    reroute_jumps[k].resize(l + 1);
    reroute_jumps[k][l].fname = fname;
    reroute_jumps[k][l].line = line;
    return setjmp((reroute_jumps[k][l].data));
}

#endif

