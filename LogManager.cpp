#include "LogManager.h"

#include <stdarg.h>
#include <iostream>
#include "ResourceManager.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

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
        size_t oct = logname.find("#");
        if(oct != logname.npos)
        {
            time_t t = time(0);
            struct tm* tmp = localtime(&t);
            char buf[64];
            snprintf(buf, 64, "%04d%02d%02d_%02d%02d%02d", 1900 + tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
            logname.replace(oct, 1, buf);
        }

        /*-------------------------------------------------*\
        | If the path is relative, use configuration dir    |
        \*-------------------------------------------------*/
        fs::path p = logname;
        if(p.is_relative())
        {
            p = defaultDir;
            p.append(logname);
        }

        /*-------------------------------------------------*\
        | Open the logfile                                  |
        \*-------------------------------------------------*/
        log_stream.open(p);
    }

    /*-------------------------------------------------*\
    | Check loglevel configuration                      |
    \*-------------------------------------------------*/
    if(config.contains("loglevel"))
    {
        const json& loglevel_obj = config["loglevel"];

        /*-------------------------------------------------*\
        | If the log level is configured globally, set same |
        | loglevel for each section                         |
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
                log_stream << " [" << temp_messages[msg]->filename << ":" << temp_messages[msg]->line << "]";
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
    if(level == LL_CRITICAL)
    {
        print_source = true;
        loglevel = LL_DEBUG;
        verbosity = LL_DEBUG;
    }

    PLogMessage mes = std::make_shared<LogMessage>();
    int len = vsnprintf(nullptr, 0, fmt, va);
    mes->buffer.resize(len);
    vsnprintf(&mes->buffer[0], len + 1, fmt, va);
    mes->level = level;
    mes->filename = filename;
    mes->line = line;

    // Print it on the screen
    // Put the timestamp here
    if(level <= verbosity)
    {
        std::cout << mes->buffer;
        if(print_source)
        {
            std::cout << " [" << mes->filename << ":" << mes->line << "]";
        }
        std::cout << std::endl;
    }

    // Add it to the GUI queue
    if(level <= LL_MESSAGE)
    {
        for(size_t idx = 0; idx < error_callbacks.size(); ++idx)
        {
            error_callbacks[idx].first(error_callbacks[idx].second, mes);
        }
    }

    // Add it to the logfile queue
    temp_messages.push_back(mes);

    // Flush the queues
    _flush();
}

void LogManager::append(const char* filename, int line, unsigned int level, const char* fmt, va_list va)
{
    std::lock_guard<std::mutex> grd(entry_mutex);

    _append(filename, line, level, fmt, va);
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
    if(level < LL_CRITICAL) level = LL_CRITICAL;
    if(level > LL_DEBUG) level = LL_DEBUG;
    LOG_DEBUG("Loglevel set to %d", level);
    loglevel = level;
}

void LogManager::setVerbosity(unsigned int level)
{
    if(level < LL_CRITICAL) level = LL_CRITICAL;
    if(level > LL_DEBUG) level = LL_DEBUG;
    LOG_DEBUG("Verbosity set to %d", level);
    verbosity = level;
}

void LogManager::setPrintSource(bool v)
{
    LOG_DEBUG("Source code location pintouts were %s", v ? "enabled" : "disabled");
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
