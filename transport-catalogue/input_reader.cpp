#include "input_reader.h"

std::string DeleteSpace(std::string s) {
    int i = 0, j = s.size() - 1;
    for (; i < s.size(); ++i)
        if (s[i] != ' ') {
            break;
        }
    for (; j >= 0; --j)
        if (s[j] != ' ') {
            break;
        }
    s = s.substr(i, j - i + 1);
    return s;
}
std::pair<TransportCatalogue::Stop, std::string> SplitStop(const std::string& text) {
    using namespace std;

    TransportCatalogue::Stop stop;
    string s1, s2, dis;
    int i = text.find(':');
    stop.name = text.substr(5, i - 5);
    ++i;
    for (; text[i] != ','; ++i)
        s1.push_back(text[i]);
    ++i;
    for (++i; i < text.size() && text[i] != ','; ++i)
        s2.push_back(text[i]);

    stop.coordinate = { stod(s1), stod(s2) };
    if (i < text.size() - 1)
        dis = text.substr(i);
    return { stop, dis };
}
TransportCatalogue::BusToStops SplitBus(const std::string& text) {
    using namespace std;

    TransportCatalogue::BusToStops BusToStops;
    BusToStops.bus.is_round_ = false;

    int i = text.find(':');
    BusToStops.bus.name = text.substr(4, i - 4);
    ++i;

    for (; i < text.size(); ++i) {
        int x = i;
        while (text[i] != '>' && text[i] != '-' && i < text.size()) {
            ++i;
        }
        if (text[i] == '>')
            BusToStops.bus.is_round_ = true;

        BusToStops.stops.push_back(DeleteSpace(text.substr(x, i - x)));
    }

    return BusToStops;
}

std::vector<std::pair<int, std::string>> SplitDistance(std::string& text) {
    using namespace std;
    vector<pair<int, std::string>> res;
    pair<int, std::string> p;
    int i = 1;
    int size_ = text.size();
    for (; i < size_; ++i) {
        string s1, s2;
        while (text[i] != 'm') {
            s1.push_back(text[i]);
            ++i;
        }
        i += 4;
        while (text[i] != ',' && i < size_) {
            s2.push_back(text[i]);
            ++i;
        }
        ++i;
        s2 = DeleteSpace(s2);
        res.push_back({ stoi(s1), s2 });
    }

    return res;
}

void ReadAll(TransportCatalogue& tc, std::istream& is) {
    using namespace std;
    string line;

    int N = 0;
    getline(is, line);
    N = stoi(line);
    vector<string> commands;
    commands.reserve(N);

    for (int i = 0; i < N; ++i) {
        getline(is, line);
        commands.push_back(line);
    }
    sort(commands.begin(), commands.end(), [](string& s1, string& s2) {return s1[0] > s2[0]; });

    unordered_map<string, vector<pair<int, string>>> distances;
    for (const auto& command : commands) {
        if (command[0] == 'S') {
            auto [stop, dis] = SplitStop(command);
            tc.AddStop(stop);
            if (dis.size() > 0) {
                distances.insert({ stop.name, SplitDistance(dis) });
            }
        }
        else {
            tc.AddBus(SplitBus(command));
        }
    }
    for (auto& [from, dis_to_stops] : distances) {
        for (auto& [dis, to] : dis_to_stops) {
            tc.SetDistance(from, to, dis);
        }

    }
}