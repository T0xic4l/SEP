//
// Created by floriangk on 30.04.24.
//

#include "../includes/order.h"

Order::Order(int capacity, std::string &id, int restaurant, std::pair<double, double> &target_location)
: capacity(capacity), id(id), restaurant(restaurant), target_location(target_location) {

}

std::string Order::get_id() const {
    return this->id;
}

int Order::get_restaurant() const {
    return this->restaurant;
}

int Order::get_capacity() const {
    return this->capacity;
}

bool Order::is_assigned() const {
    return this->assigned;
}

void Order::set_assigned(bool is_assigned) {
    this->assigned = is_assigned;
}
