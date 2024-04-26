#include <iostream>
#include <vector>
#include <map>
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

    nlohmann::json seed = nlohmann::json::parse(R"({"seed":")" + arg_seed + "\"}"); // Seed der an den server gegeben wird

    cpr::Response response = cpr::Post(cpr::Url{arg_host + "/"}, // Adresse des servers
                                cpr::Body{seed.dump()}, // Übergabe des Seeds
                                cpr::Header{{"Content-Type","application/json"}}); //Definiert, dass gesendete daten im JSON format sind

    nlohmann::json parsed_response = nlohmann::json::parse(response.text); // Übersetzen des klartexts in ein JSON Objekt

    unsigned long driver_count = parsed_response["drivers"].size();

    response = cpr::Post(cpr::Url{arg_host + "/step"},  // Initialer Step-Post mit leerer Liste an driver_actions
                  cpr::Body{initialize_driver_actions(driver_count).dump()},
                  cpr::Header{{"Content-Type","application/json"}});
    std::cout << nlohmann::json::parse(response.text) << std::endl;

    return 0;
}

nlohmann::json initialize_driver_actions(const unsigned long driver_count) {
    nlohmann::json actions;
    for(int i = 0; i < driver_count; i++){
        actions["driver_actions"][std::to_string(i)] = std::vector<std::string>();
    }
    return actions;
}
