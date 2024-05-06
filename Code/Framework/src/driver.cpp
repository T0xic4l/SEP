#include "../includes/driver.hpp"

Driver::Driver(int id, int capacity, int load, std::pair<double,double> location, int speed)
: id(id), capacity(capacity), load(load), location(location), speed(speed) {

}

int Driver::get_id() const {
    return this->id;
}

int Driver::get_capacity() const {
    return this->capacity;
}

int Driver::get_load() const {
    return this->load;
}

std::pair<double, double> Driver::get_loacation() const{
    return this->location;
}

int Driver::get_speed() const {
    return this->speed;
}

void Driver::update_driver(int new_load, std::pair<double, double> new_location) {
    this->load = new_load;
    this->location = new_location;
}
