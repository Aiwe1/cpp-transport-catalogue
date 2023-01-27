#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"

int main()
{
    //using namespace std::string_literals;

    //setlocale(LC_ALL, "Russian");
    TransportCatalogue tc;

    std::ifstream in("C:\\Projects\\TC\\cpp-transport-catalogue\\transport-catalogue\\input-2.json");
    std::ofstream out("C:\\Projects\\TC\\cpp-transport-catalogue\\transport-catalogue\\output.json");


    ReadAll(tc, in, out);
    
    //ReadAll(tc, std::cin, std::cout);
}