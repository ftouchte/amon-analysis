#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
    json j = json::parse(R"({"happy": true})");
    j["pi"] = 3.141;
    j["list"] = {1, 0, 2};

    std::cout << j.dump(4) << "\n";
}