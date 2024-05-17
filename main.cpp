#include "Dummy_Request.hpp"
#include "Response.hpp"

int main() 
{
    std::map<std::string, std::string> test; 

    test["method"] = "GET";
    test["uri"] = "index.html";
    test["http_version"] = "HTTP/1.1";

    Dummy_Request testRequest(test);
    Response response(testRequest);

    response.createResponseMessage();

}
