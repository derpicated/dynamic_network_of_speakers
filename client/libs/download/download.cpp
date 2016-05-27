#include "download.hpp"

void download::download (std::string remote_uri, std::string local_name) {
    sf::Http http;
    sf::Http::Request http_req;

    http_req.setMethod (sf::Http::Request::Get);
    http_req.setUri (remote_uri);

    sf::Http::Response http_resp = http.sendRequest (http_req);


    if (http_resp.getStatus () != sf::Http::Response::Status::Ok) {
        LOG (ERROR) << "HTTP:\tcould not download:" << remote_uri
                    << " error: " << http_resp.getStatus ();
    } else {
        std::string http_payload = http_resp.getBody ();

        if (http_payload.size () == 0) {
            LOG (ERROR) << "HTTP:\tempty response from:" << remote_uri;
        } else {
            std::ofstream out_file (local_name, std::ofstream::binary);
            out_file << http_payload;
            out_file.close ();
        }
    }
}
