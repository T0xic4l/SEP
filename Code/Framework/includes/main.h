#ifndef JUMMYSALESMAN_MAIN_H
#define JUMMYSALESMAN_MAIN_H

#include <iostream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <argparse/argparse.hpp>
#include <string>
#include "driver.hpp"
#include "order.h"
#include "restaurant.h"
#include "helperfunction.h"

using nlohmann::json;

/**
 * @brief Startet die Simulation auf dem Server, indem ihm der seed mitgegeben wird
 * @param host – Der host des Servers
 * @param seed – Der Seed, der die Simulation generiert
 * @return Die Antwort des Servers mit der Ausgangssituation der Simulation
 */
nlohmann::json start_simulation(const std::string & host, const std::string & seed);
/**
 * @brief Erstellt initiale liste an Fahrern aus der response
 * @param drivers JSON aus der die Drivers gelesen werden
 * @return Vector mit zeigern auf die einzelnen Fahrer
 */
std::vector<std::shared_ptr<Driver>> initialize_drivers(nlohmann::json drivers); // ändere json drivers zu cosnt json& drivers wenn möglich.
/**
 * @brief Erstellt initiale list an Restaurants aus der response
 * @param restaurant_list JSON array aus der die Restaurants gelesen werden.
 * @return Vector mit shared_ptr auf die Restaurants.
 */
std::vector<std::shared_ptr<Restaurant>> initialize_restaurants(json restaurant_list);
/**
 * @brief Funktion, welche die simulationsschritte durchführt.
 * @param first_response JSON welche aus der initialen response des Servers besteht
 * @param host Adresse des Servers, mit dem kommuniziert wird.
 * @param driver_list liste der Fahrer in der Simulation
 * @param restaurant_list liste der Restaurants in der Simulation
 */
void run_simulation(
        json & first_response,
        const std::string & host,
        std::vector<std::shared_ptr<Driver>> driver_list,
        std::vector<std::shared_ptr<Restaurant>> restaurant_list
        );
/**
 * @brief Hängt neue order an die liste der orders, falls es ein event des typs order gibt.
 * @param orders die liste der order die schon existieren
 * @param event das event welches vom server gesendet wird.
 */
void append_new_orders(std::vector<std::shared_ptr<Order>> &orders, json event);


#endif //JUMMYSALESMAN_MAIN_H
