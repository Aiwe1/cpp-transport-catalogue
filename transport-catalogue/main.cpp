#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"

int main()
{
    //setlocale(LC_ALL, "Russian");
    TransportCatalogue tc;

    std::ifstream ifs("D:\\Project VS\\!сдавать\\cpp-transport-catalogue\\transport-catalogue\\input.json");
    //std::ofstream out;
    //out.open("C:\\Projects VS\\TransportCatalogue\\tests\\out.txt");

    ReadAll(tc, ifs, std::cout);
    //ReadRequests(tc, cin, cout);
    //return 0;
} 