#ifndef JUMMYSALESMAN_DRIVER_H
#define JUMMYSALESMAN_DRIVER_H

#include <string>

/**
 * @brief Klasse die für die algorithmen wichtige Datentypen der Fahrer enthält
 *
 */
class Driver {
private:
    int idle_for{-1};
    int id{0};
    int capacity{0};
    int load{0};
    std::pair<double, double> location; //Koordinaten in form (x,y) von: Florian
    int speed{0};
public:
    Driver() = default;
    Driver(int id, int capacity, int load, std::pair<double,double> location, int speed);

    int get_id() const;
    int get_capacity() const;
    int get_load() const;
    std::pair<double, double> get_loacation() const;
    int get_speed() const;

    void set_location(std::pair<double,double> new_location);
    void set_load(int new_load);
    void increase_idle_duration();
    void reset_idle_duration();

    bool is_too_idle() const;
};

#endif