#include "gecko/DNS.h"
#include "gecko/Log.h"
#include "gecko/Wifi.h"

namespace Gecko::Embedded
{
    bool DNS::Init()
    {
        if (m_state != State::Uninitialized)
        {
            Log_Error("DNS: Tried to initialize more than once.\n");
            return false;
        }

        m_state = State::Unresolved;
        return true;
    }

    bool DNS::QueryHostname(const std::string& name)
    {
        if (m_state != State::Unresolved &&
            m_state != State::Error)
        {
            Log_Error("DNS: Uninitialized or hostname already resolved.\n");
            return false;
        }

        m_state = State::Resolving;
        cyw43_arch_lwip_begin();
        const auto r = dns_gethostbyname(
            name.c_str(), 
            &m_resolvedIP, 
            &DNSFound_Callback, 
            nullptr);
        cyw43_arch_lwip_end();

        switch (r)
        {
            case ERR_OK:
                m_state = State::Resolved;
                return true;

            case ERR_INPROGRESS:
                /* I'm being explicit here but this should already be set. */
                m_state = State::Resolving;
                return true;
        }

        Log_Error("DNS: gethostbyname failed immediately: %d\n", r);
        m_state = State::Error;
        return false;
    }

    std::optional<ip_addr_t> DNS::QueryHostnameSync(const std::string& name)
    {
        QueryHostname(name);

        while (m_state == State::Resolving)
        {
            Wifi::Poll();
            sleep_ms(90);
        }

        return m_state == State::Resolved
            ? std::optional{ m_resolvedIP }
            : std::nullopt;
    }

    void DNS::DNSFound_Callback(const char* name, const ip_addr_t *ip, void*)
    {
        if (ip == nullptr)
        {
            Log_Error("DNS: Could not resolve hostname: %s\n", name);
            m_state = State::Error;
            return;
        }

        m_resolvedIP = *ip;
        m_state      = State::Resolved;
    }
}