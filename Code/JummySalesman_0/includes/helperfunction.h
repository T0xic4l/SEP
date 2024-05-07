#ifndef HELPERFUNCTION_H
#define HELPERFUNCTION_H
#include <complex>
#include <map>
#include <vector>
/**
 * @brief berechnet die distanz zwischen zwei punkten im zweidimensionlaen raum.
 * @param cord1 koordinatem (x und y) des ersten punkts
 * @param cord2 koordinatem (x und y) des zweiten punkts
 * @return distanz der punkte als double
 */
double direct_distance(std::pair<double, double> & cord1,std::pair<double, double> & cord2);

bool list_is_empty(std::string id, std::map<std::string, std::vector<std::string>> & driver_actions);

#endif //HELPERFUNCTION_H
