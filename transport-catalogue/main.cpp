#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"

int main()
{
    //using namespace std::string_literals;

    //setlocale(LC_ALL, "Russian");
    TransportCatalogue tc;

    //std::ifstream in("C:\\Projects VS\\cpp-transport-catalogue\\transport-catalogue\\input.json");
    //std::ofstream out("C:\\Projects VS\\cpp-transport-catalogue\\transport-catalogue\\output.json");
    //ReadAll(tc, in, out);

    ReadAll(tc, std::cin, std::cout);
}