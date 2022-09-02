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
std::pair<std::pair<std::string, Coordinates>, std::string> SplitStop(const std::string& text) {
    using namespace std;

    pair<string, Coordinates> Stop;
    string s1, s2, dis;
    int i = text.find(':');
    Stop.first = text.substr(5, i - 5);
    ++i;
    for (; text[i] != ','; ++i)
        s1.push_back(text[i]);
    ++i;
    for (++i; i < text.size() && text[i] != ','; ++i)
        s2.push_back(text[i]);

    Stop.second = { stod(s1), stod(s2) };
    if (i < text.size() - 1)
        dis = text.substr(i);
    return { Stop, dis };
}
std::pair<std::pair<std::string, std::vector<std::string>>, bool> SplitBus(const std::string& text) {
    using namespace std;
    bool is_round_ = false;
    pair<string, vector<string>> Bus;

    int i = text.find(':');
    Bus.first = text.substr(4, i - 4);
    ++i;

    for (; i < text.size(); ++i) {
        int x = i;
        while (text[i] != '>' && text[i] != '-' && i < text.size()) {
            ++i;
        }
        if (text[i] == '>')
            is_round_ = true;

        Bus.second.push_back(DeleteSpace(text.substr(x, i - x)));
    }

    return pair(Bus, is_round_);
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
        //p = { stoi(s1), s2 };
        //res.insert({ stoi(s1), s2 });
    }

    return res;
}

void ReadAll(TransportCatalogue& cat) {
    using namespace std;
    string s;

    int N = 0;
    getline(cin, s);
    N = stoi(s);
    vector<string> All;
    All.reserve(N);

    for (int i = 0; i < N; ++i) {
        getline(cin, s);
        All.push_back(s);
    }
    sort(All.begin(), All.end(), [](string& s1, string& s2) {return s1[0] > s2[0]; });

    unordered_map<string, vector<pair<int, string>>> dist;
    for (const auto& line : All) {
        if (line[0] == 'S') {
            auto [a, b] = SplitStop(line);
            cat.AddStop(a);
            if (b.size() > 0) {
                dist.insert({ a.first, SplitDistance(b) });
            }
        }
        else {
            cat.AddBus(SplitBus(line));
        }
    }

    cat.AddDistance(dist);
}