#pragma once
#include <optional>
#include <string>

namespace Gecko::Embedded
{
    class DNS
    {
      public:
        enum class State
        {
            Uninitialized,
            Unresolved,
            Resolving,
            Resolved,
            Error
        };
        
        static bool Init();
        static bool QueryHostname(const std::string& name);
        static std::optional<ip_addr_t> QueryHostnameSync(const std::string& name);

        static inline State ResolveState() { return m_state; }
        static inline const ip_addr_t& ResolvedIP() { return m_resolvedIP; }

      private:
        static void DNSFound_Callback(const char* name, const ip_addr_t *ip, void* context);

        static inline State m_state{ State::Uninitialized };
        static inline ip_addr_t m_resolvedIP{ 0 };
    };
}