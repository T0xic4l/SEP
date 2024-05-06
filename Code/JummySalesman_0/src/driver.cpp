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

void Driver::set_location(std::pair<double, double> new_location) {
    this->location = new_location;
}

void Driver::set_load(int new_load) {
    this->load = new_load;
}