#include <iostream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main()
{
    using namespace std;
    TransportCatalogue tc;
    ReadAll(tc);
    ReadRequests(tc);
} 
/*
#include <iostream>
#include <fstream>

#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"

int main()
{
    using namespace std;
    TransportCatalogue tc;

    ifstream ifs("C:\\Projects VS\\TransportCatalogue1234\\tests\\tsC.txt");
    cin.rdbuf(ifs.rdbuf());
    //std::ofstream out;
    //out.open("C:\\Projects VS\\TransportCatalogue\\tests\\out.txt");
    //auto ss = cout.rdbuf(out.rdbuf());

    ReadAll(tc);

    ReadRequests(tc);

    //cout.rdbuf(ss);
} */