#include "stat_reader.h"
#include "input_reader.h"

void ReadRequests(TransportCatalogue& cat) {
    using namespace std;
    string s;

    int N = 0;
    getline(cin, s);
    N = stoi(s);

    for (int i = 0; i < N; ++i) {
        getline(cin, s);
        s = s.substr(s.find_first_not_of(' '));
        if (s[0] == 'B')
            cout << cat.GetInfoBus(DeleteSpace(s.substr(4))) << endl;
        if (s[0] == 'S')
            cout << cat.GetInfoStop(DeleteSpace(s.substr(5))) << endl;
    }
}