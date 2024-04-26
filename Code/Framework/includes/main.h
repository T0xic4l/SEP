#ifndef JUMMYSALESMAN_MAIN_H
#define JUMMYSALESMAN_MAIN_H

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <argparse/argparse.hpp>

/*
 * @brief:  Erstellt anhängig von der Anzahl der Lieferfahrer eine JSON-Objekt mit den driver_actions,
 *          wobei die dazugehörigen Listen leer sind.
 * @param:  Anzahl der Lieferfahrer
 * @return: JSON-Objekt, das die driver_actions enthält
 */
nlohmann::json initialize_driver_actions(unsigned long driver_count);


#endif //JUMMYSALESMAN_MAIN_H
