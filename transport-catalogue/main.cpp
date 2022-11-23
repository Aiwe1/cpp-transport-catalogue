#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "json_reader.h"

int main()
{
    //setlocale(LC_ALL, "Russian");
    TransportCatalogue tc;

    std::ifstream ifs("D:\\Project VS\\!сдавать\\cpp-transport-catalogue\\transport-catalogue\\input.json");
    std::ofstream out;
    out.open("D:\\Project VS\\!сдавать\\cpp-transport-catalogue\\transport-catalogue\\SVGout.svg");

    ReadAll(tc, ifs, out);
    //ReadRequests(tc, cin, cout);
    //return 0;
} 