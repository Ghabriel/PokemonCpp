#include <fstream>
#include <iostream>

#include "parse-json.hpp"
#include "Scanner.hpp"
#include "Token.hpp"

int main(int, char**) {
    std::ifstream file("data.json", std::ios::binary);
    auto json = parseJSON(file);
    std::cout << json["phoneNumbers"][1]["type"].get<std::string>() << std::endl;

    // std::cout << json["firstName"].get<std::string>() << std::endl;
    // std::cout << json["address"]["city"].get<std::string>() << std::endl;
}
