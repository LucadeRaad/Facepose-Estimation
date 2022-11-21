#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>

#include "httplib.h"

void do_http_get(std::string host, int port, int pan, int tilt)
{
    std::cout << "GET " << host << ":" << port << std::endl;

    httplib::Client cli(host, port);

    std::stringstream uri;
    uri << "/aim_camera?pan=" << pan << "&tilt=" << tilt;

    if (auto res = cli.Get(uri.str()))
    {
        if (res->status == 200)
        {
            std::cout << res->body << std::endl;
        }
    }
    else
    {
        auto err = res.error();
        
        std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
    }
}


int main(int agc, char** argv)
{
    std::thread http_thread(do_http_get, "localhost", 5000, 180, 25);

    http_thread.detach();

    sleep(5);

    return 0;
}

