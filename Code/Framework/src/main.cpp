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

    json first_response = start_simulation(arg_host, arg_seed);
    std::vector<std::shared_ptr<Driver>> drivers = initialize_drivers(first_response["drivers"]);
    std::vector<std::shared_ptr<Restaurant>> restaurants = initialize_restaurants(first_response["restaurants"]);
    run_simulation(first_response, arg_host, drivers, restaurants);

    return 0;
}

std::vector<std::shared_ptr<Driver>> initialize_drivers(json drivers) {
    std::vector<std::shared_ptr<Driver>> return_drivers;

    for(auto & driver : drivers){
        int id = driver["id"];
        int capacity = driver["capacity"];
        int load = driver["current_load"];
        std::pair<double, double> location = driver["location"];
        int speed = driver["speed"];
        std::shared_ptr<Driver> new_driver = std::make_shared<Driver>(id, capacity, load, location, speed);
        return_drivers.push_back(new_driver);
    }
    return return_drivers;
}

std::vector<std::shared_ptr<Restaurant>> initialize_restaurants(json restaurant_list) {
    std::vector<std::shared_ptr<Restaurant>> restaurants;

    for(auto & restaurant : restaurant_list){
        int id = restaurant["id"];
        std::pair<double, double> location = restaurant["location"];
        std::shared_ptr<Restaurant> new_restaurant = std::make_shared<Restaurant>(id, location);
        restaurants.push_back(new_restaurant);
    }

    return restaurants;
}

nlohmann::json start_simulation(const std::string & host, const std::string & seed) {
    nlohmann::json seed_json = nlohmann::json::parse(R"({"seed":")" + seed + "\"}");

    cpr::Response response = cpr::Post(cpr::Url{host + "/"},
                                       cpr::Body{seed_json.dump()},
                                       cpr::Header{{"Content-Type","application/json"}});
    return nlohmann::json::parse(response.text);
}

void run_simulation(
        json & first_response,
        const std::string & host,
        std::vector<std::shared_ptr<Driver>> driver_list,
        std::vector<std::shared_ptr<Restaurant>> restaurant_list
        ) {
    std::vector<std::shared_ptr<Driver>> drivers = driver_list;
    std::vector<std::shared_ptr<Restaurant>> restaurants = restaurant_list;
    std::vector<std::shared_ptr<Order>> orders;
    json actions_to_send;
    std::map<std::string, std::vector<std::string>> driver_actions = {{"0",std::vector<std::string>()}};

    while(true) {
        actions_to_send["driver_actions"] = driver_actions;

        cpr::Response response = cpr::Post(cpr::Url{host + "/step"},
                                           cpr::Body{actions_to_send.dump()},
                                           cpr::Header{{"Content-Type","application/json"}});

        json parsed_response = nlohmann::json::parse(response.text);
        append_new_orders(orders, parsed_response["event"]);
        std::cout << parsed_response.dump() << std::endl;
        std::cout << actions_to_send.dump() << std::endl;

        if(parsed_response["event"] == "{}") {     // Sendet der Server kein Event, terminiert das Programm – Struktur eines leeren Events unklar!
            break;
        }
    }
}

void append_new_orders(std::vector<std::shared_ptr<Order>> &orders, json event) {
    if(event["event_type"] == "order"){
        int capacity = event["data"]["capacity"];
        std::string id = event["data"]["id"];
        int restaurant = event["data"]["restaurant"];
        std::pair<double, double> target_location = event["data"]["target"]["location"];

        std::shared_ptr<Order> new_order = std::make_shared<Order>(capacity, id, restaurant, target_location);

        orders.push_back(new_order);
    } else {
        std::cout << event.dump() << "\n";
    }
}