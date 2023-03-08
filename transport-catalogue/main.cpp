#include <iostream>
#include <fstream>
#include <string_view>
//#include <transport_catalogue.pb.h>

//#include "transport_catalogue.h"
#include "json_reader.h"

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}
/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        MakeBase(std::cin);

    }
    else if (mode == "process_requests"sv) {
        ProcessRequests(std::cin);
    }
    else {
        PrintUsage();
        return 1;
    }
} */

int main()
{
    //setlocale(LC_ALL, "Russian");
    //TransportCatalogue tc;
    {
        //std::ifstream in("C:\\Projects\\again cmake\\tests\\s14_1_opentest_1_make_base.json");
        std::ifstream in("C:\\Projects\\again cmake\\input_make.json");
        //std::ofstream out("C:\\Projects\\again cmake\\3\\output.json");

        //ReadAll(tc, in, out);

        MakeBase(in);
        in.close();
    }
    {
        //std::ifstream in("C:\\Projects\\again cmake\\tests\\s14_1_opentest_1_process_requests.json");
        std::ifstream in("C:\\Projects\\again cmake\\input_process.json");
        ProcessRequests(in);
        in.close();
    }
}

