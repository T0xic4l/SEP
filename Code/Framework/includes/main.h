#ifndef JUMMYSALESMAN_MAIN_H
#define JUMMYSALESMAN_MAIN_H

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <argparse/argparse.hpp>
#include <string>

/*
 * @brief: Startet die Simulation auf dem Server, indem ihm der seed mitgegeben wird
 * @param host – Der host des Servers
 * @param seed – Der Seed, der die Simulation generiert
 * @return: Die Antwort des Servers mit der Ausgangssituation der Simulation
 */
nlohmann::json start_simulation(const std::string & host, const std::string & seed);

/*
 * @brief: Das erste Entscheidungsverfahren, nach dem die Lieferungen den Lieferfahrern zugewiesen werden. Es wird
 * nach folgendem Prinzip entschieden:
 *     Der erste Lieferfahrer, der noch genug Restkapazität für die neuste Bestellung hat, nimmt diese
 *     auch auf!
 * @param driver_actions – Eine Map, wo die Aufgaben für die jeweiligen Lieferfahrer eingetragen werden
 * @param response – Die Antwort des Servers. Dort befindet sich auch das aktuellste Event.
 * @param driver_count – Anzahl der Lieferfahrer in der Simulation
 * @return: Die Map mit den neu eingetragenen Zuweisungen für die Lieferfahrer
 */
std::map<std::string, std::vector<std::string>> pack_the_bags(std::map<std::string, std::vector<std::string>> & driver_actions, nlohmann::json & response, int driver_count);

bool has_sufficient_capacity(int current_load, int capacity_of_order, int driver_capacity);

std::string construct_proper_id(std::string id, const std::string & id_type);

std::map<std::string, std::vector<std::string>> delete_corresponding_order(std::map<std::string, std::vector<std::string>> & driver_actions, const std::string & id_of_driver);


#endif //JUMMYSALESMAN_MAIN_H
