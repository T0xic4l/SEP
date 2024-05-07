#include "../includes/helperfunction.h"

double direct_distance(std::pair<double, double> &cord1,std::pair<double, double> &cord2) {
    double x = cord1.first - cord2.first;
    double y = cord1.second - cord2.second;
    return std::sqrt(x*x+y*y);
}

bool list_is_empty(std::string id, std::map<std::string, std::vector<std::string>> & list) {
    return list[id].empty();
}