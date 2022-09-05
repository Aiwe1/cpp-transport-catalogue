#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main()
{
    using namespace std;
    TransportCatalogue tc;

    //ifstream ifs("C:\\Projects VS\\TransportCatalogue1234\\tests\\tsC.txt");
    //std::ofstream out;
    //out.open("C:\\Projects VS\\TransportCatalogue\\tests\\out.txt");

    ReadAll(tc, cin);
    ReadRequests(tc, cin, cout);
} 

/*
Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828 
*/