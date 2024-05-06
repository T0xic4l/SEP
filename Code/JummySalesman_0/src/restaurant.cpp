#include "../includes/restaurant.h"

Restaurant::Restaurant(int id, std::pair<double, double> &location)
: id(id), location(location){

}

int Restaurant::get_id() const {
    return this->id;
}

std::pair<double, double> Restaurant::get_location() const {
    return this->location;
}
