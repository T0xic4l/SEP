#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../includes/main.h"



int main(int argc, char * argv[]) {
    argparse::ArgumentParser arpa("JummySalesman", "0");
    arpa.add_argument("--seed")
            .help("Generiert ein einzigartiges Szenario für die Simulation")
            .default_value("AAA123");
    arpa.add_argument("--host")
            .help("Url des Server, zu dem eine Verbindung aufgebaut wird")
            .default_value("http://127.0.0.1:1234");
    arpa.parse_args(argc, argv);

    auto arg_seed = arpa.get<>("--seed");
    auto arg_host = arpa.get<>("--host");

    nlohmann::json first_response = start_simulation(arg_host, arg_seed);
    int driver_count = int(first_response["drivers"].size());

    std::map<std::string, std::vector<std::string>> driver_actions;
    std::map<std::string, int> expected_capacities; // Tracke die momentane Rücksack-Füllmenge eines jeden Lieferfahrers !!!!!!!!!!!!!!!!

    while(true) {
        nlohmann::json actions_to_send;
        actions_to_send["driver_actions"] = driver_actions;

        cpr::Response response = cpr::Post(cpr::Url{arg_host + "/step"},
                                           cpr::Body{actions_to_send.dump()},
                                           cpr::Header{{"Content-Type","application/json"}});

        nlohmann::json parsed_response = nlohmann::json::parse(response.text);
        std::cout << parsed_response << std::endl;
        std::cout << parsed_response["event"] << std::endl;
        std::cout << parsed_response["mandatory_actions"] << std::endl;

        if(parsed_response["event"] == "{}") {     // Sendet der Server kein Event, terminiert das Programm – Struktur eines leeren Events unklar!
            break;
        }

        driver_actions = pack_the_bags(driver_actions, parsed_response, driver_count);
    }

    return 0;
}

nlohmann::json start_simulation(const std::string & host, const std::string & seed) {
    nlohmann::json seed_json = nlohmann::json::parse(R"({"seed":")" + seed + "\"}");

    cpr::Response response = cpr::Post(cpr::Url{host + "/"},
                                       cpr::Body{seed_json.dump()},
                                       cpr::Header{{"Content-Type","application/json"}});
    return nlohmann::json::parse(response.text);
}

std::map<std::string, std::vector<std::string>> pack_the_bags(std::map<std::string, std::vector<std::string>> & driver_actions, nlohmann::json & response, int driver_count) {
    nlohmann::json event = response["event"];
    nlohmann::json event_type = event["event_type"];
    nlohmann::json drivers = response["drivers"];

    if(event_type == "order") {
        std::string reg_id = construct_proper_id(event["data"]["id"].dump(), "pickup_");
        for(auto driver : drivers) {
            if(has_sufficient_capacity(int(driver["current_load"]), int(event["data"]["capacity"]), int(driver["capacity"]))) {
                driver_actions[driver["id"].dump()].insert(driver_actions[driver["id"].dump()].end(), reg_id);
                break;
            } else {
                continue;
            }
        }
    } else if(event_type == "pickup") {
        std::string reg_id = construct_proper_id(event["data"]["order_id"].dump(), "dropoff_");
        std::string id_of_driver = event["data"]["driver"].dump();

        driver_actions[id_of_driver].insert(driver_actions[id_of_driver].end(), reg_id);
        driver_actions = delete_corresponding_order(driver_actions, id_of_driver);
    }

    return driver_actions;
}

bool has_sufficient_capacity(const int current_load, const int capacity_of_order, const int driver_capacity) {
    return (current_load + capacity_of_order <= driver_capacity);
}

std::string construct_proper_id(std::string id, const std::string & id_type) {
    id.erase(std::remove(id.begin(), id.end(), '\"'), id.end());
    return id_type + id;
}

std::map<std::string, std::vector<std::string>> delete_corresponding_order(std::map<std::string, std::vector<std::string>> & driver_actions, const std::string & id_of_driver) {
    driver_actions[id_of_driver].erase(driver_actions[id_of_driver].begin());
    return driver_actions;
}