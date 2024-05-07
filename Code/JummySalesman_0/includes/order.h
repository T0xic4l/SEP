#ifndef JUMMYSALESMAN_ORDER_H
#define JUMMYSALESMAN_ORDER_H

#include <string>

/**
 * @brief Klasse die die vom server gesendeten Orders darstellt.
 */
class Order {
private:
    int capacity{0};
    std::string id;
    int restaurant{0};
    std::pair<double,double> target_location;
    bool assigned{false};
public:
    Order() = default;
    Order(int capacity, std::string &id, int restaurant, std::pair<double,double> &target_location);

    std::string get_id() const;
    int get_restaurant() const;
    int get_capacity() const;
    std::pair<double,double> get_target_location() const;
    bool is_assigned() const;

    void set_assigned(bool is_assigned);
};


#endif //JUMMYSALESMAN_ORDER_H
