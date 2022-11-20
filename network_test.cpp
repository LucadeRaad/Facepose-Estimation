#include <iostream>

#include "httplib.h"

int main(int agc, char** argv)
{
    //std::cout << GPIO::JETSON_INFO() << std::endl;

    httplib::Client cli("localhost", 5000);

    if (auto res = cli.Get("/aim_camera?pan=100&tilt=25"))
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


    return 0;
}
