#pragma once
#include <memory>
#include <string>
#include <type_traits>

namespace Gecko::API::Logging
{
    class Logger
    {
    public:
        virtual ~Logger() = default;

        template<typename TLogger, typename ...TArgs> requires
            std::is_base_of_v<Logger, TLogger>
        static void InitInfo(TArgs... args)
        {
            m_infoLogger = std::make_unique<TLogger>(args...);
        }
        
        template<typename TLogger, typename ...TArgs> requires
            std::is_base_of_v<Logger, TLogger>
        static void InitWarn(TArgs... args)
        {
            m_warnLogger = std::make_unique<TLogger>(args...);
        }
        
        template<typename TLogger, typename ...TArgs> requires
            std::is_base_of_v<Logger, TLogger>
        static void InitError(TArgs... args)
        {
            m_errorLogger = std::make_unique<TLogger>(args...);
        }
        
        template<typename TLogger, typename ...TArgs> requires
            std::is_base_of_v<Logger, TLogger>
        static void InitDebug(TArgs... args)
        {
            m_debugLogger = std::make_unique<TLogger>(args...);
        }

        static inline Logger& Info()  { return *m_infoLogger; }
        static inline Logger& Warn()  { return *m_warnLogger; }
        static inline Logger& Error() { return *m_errorLogger; }
        static inline Logger& Debug() { return *m_debugLogger; }

        virtual void operator<<(const std::string& v) = 0;

    private:
        static inline std::unique_ptr<Logger> m_infoLogger;
        static inline std::unique_ptr<Logger> m_warnLogger;
        static inline std::unique_ptr<Logger> m_errorLogger;
        static inline std::unique_ptr<Logger> m_debugLogger;
    };
}