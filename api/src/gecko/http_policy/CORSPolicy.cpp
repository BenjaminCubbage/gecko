#include "gecko/http_policy/CORSPolicy.h"

namespace Gecko::API::HttpPolicy
{
    void CORSPolicy::Attach(httplib::Server& server)
    {
        server.Options(R"(.*)", [this](const httplib::Request&, httplib::Response& res) {
            /*
                ACAO/ACAC headers can be set in post-routing handler.
            */
            res.set_header("Access-Control-Allow-Headers", AllowHeaders);
            res.set_header("Access-Control-Allow-Methods", AllowMethods);
            res.set_header("Access-Control-Max-Age",       MaxAge);
            res.status = httplib::StatusCode::OK_200;
        });

        server.set_post_routing_handler([this](const httplib::Request&, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Credentials", AllowCredentials);
            res.set_header("Access-Control-Allow-Origin",      m_allowList);
        });
    }
}