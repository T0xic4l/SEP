#ifndef JUMMYSALESMAN_RESTAURANT_H
#define JUMMYSALESMAN_RESTAURANT_H

#include <utility>

/**
 * @brief Klasse die die vom server vorgegebenen restaurants darstellt
 *
 *
 */
class Restaurant {
private:
    int id{0};
    std::pair<double, double> location;
public:
    Restaurant() = default;
    Restaurant(int id, std::pair<double, double> &location);

    int get_id() const;
    std::pair<double, double> get_location() const;
};


#endif //JUMMYSALESMAN_RESTAURANT_H
