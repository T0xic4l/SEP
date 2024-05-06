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
    std::map<std::string, std::shared_ptr<Driver>> drivers = initialize_drivers(first_response["drivers"]);
    std::vector<std::shared_ptr<Restaurant>> restaurants = initialize_restaurants(first_response["restaurants"]);
    run_simulation(first_response, arg_host, drivers, restaurants);

    return 0;
}

std::map<std::string, std::shared_ptr<Driver>> initialize_drivers(json drivers) {
    std::map<std::string, std::shared_ptr<Driver>> return_drivers;

    for(auto & driver : drivers){
        std::cout << driver.dump() << std::endl;
        int id = driver["id"];
        int capacity = driver["capacity"];
        int load = driver["current_load"];
        std::pair<double, double> location = driver["location"];
        int speed = driver["speed"];
        std::shared_ptr<Driver> new_driver = std::make_shared<Driver>(id, capacity, load, location, speed);
        return_drivers.insert({{std::to_string(id), new_driver}});
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
        std::map<std::string, std::shared_ptr<Driver>> driver_list,
        std::vector<std::shared_ptr<Restaurant>> restaurant_list
        ) {
    std::map<std::string, std::shared_ptr<Driver>> drivers = driver_list;
    std::vector<std::shared_ptr<Restaurant>> restaurants = restaurant_list;
    std::map<std::string, std::shared_ptr<Order>> orders;
    json actions_to_send;
    std::map<std::string, std::vector<std::string>> driver_actions = {{"0",std::vector<std::string>()}};

    while(true) {
        actions_to_send["driver_actions"] = driver_actions;

        cpr::Response response = cpr::Post(cpr::Url{host + "/step"},
                                           cpr::Body{actions_to_send.dump()},
                                           cpr::Header{{"Content-Type","application/json"}});

        json parsed_response = nlohmann::json::parse(response.text);
        handle_event(orders, drivers, driver_actions, parsed_response["event"]);
        update_drivers(drivers, parsed_response["drivers"]);
        choose_actions(parsed_response["mandatory_actions"], orders, drivers, restaurants, driver_actions);
        std::cout << actions_to_send.dump() << std::endl;

        std::cout << "Score: " << parsed_response["score"] << std::endl;
        if(parsed_response["error"] || parsed_response["event"] == "{}") {
            std::cout << parsed_response.dump() << std::endl;
            break;
        }
    }
}

void handle_event(
    std::map<std::string, std::shared_ptr<Order>> &orders,
    std::map<std::string, std::shared_ptr<Driver>> &drivers,
    std::map<std::string, std::vector<std::string>> &driver_actions,
    json event
    ) {
    if(event["event_type"] == "order"){
        std::string order_id = event["data"]["id"];
        int capacity = event["data"]["capacity"];
        int restaurant = event["data"]["restaurant"];
        std::pair<double, double> target_location = event["data"]["target"]["location"];

        std::shared_ptr<Order> new_order = std::make_shared<Order>(capacity, order_id, restaurant, target_location);

        orders.insert({{order_id,  new_order}});
    } else if(event["event_type"] == "pickup" || event["event_type"] == "dropoff") {
        int driver_id = event["data"]["driver"];
        std::string order_id = event["data"]["order_id"];
        std::string action = event["data"]["action"];
        auto result = std::find_if(
                driver_actions[std::to_string(driver_id)].begin(),
                driver_actions[std::to_string(driver_id)].end(),
                [&action](const auto &item) {
                    return item == action;
                });
        if(result != driver_actions[std::to_string(driver_id)].end()) 
            driver_actions[std::to_string(driver_id)].erase(result);

        if(event["event_type"] == "dropoff"){ //falls das event ein dropoff ist verringere den load des drivers
            int current_load = drivers[std::to_string(driver_id)]->get_load();
            int order_cap = orders[order_id]->get_capacity();
            drivers[std::to_string(driver_id)]->set_load(current_load-order_cap);
        }
    } else {
        std::cout << event.dump() << "\n";
    }
}

void update_drivers(std::map<std::string, std::shared_ptr<Driver>> &drivers, json updated_drivers){
    for(auto & updated_driver : updated_drivers){
        std::string update_id = updated_driver["id"].dump();
        drivers[update_id]->set_location(updated_driver["location"]);
    }
}

void choose_actions(
        json mandatory_actions,
        std::map<std::string, std::shared_ptr<Order>> &orders,
        std::map<std::string, std::shared_ptr<Driver>> &drivers,
        std::vector<std::shared_ptr<Restaurant>> &restaurants,
        std::map<std::string, std::vector<std::string>> &driver_actions
        ) {
    for(auto & action : mandatory_actions){
        std::string action_type = action["action_type"];
        std::string order_id = action["data"]["order_id"];

        std::shared_ptr<Order> order = orders[order_id];
        int order_restaurant = order->get_restaurant();
        int order_capacity = order->get_capacity();

        std::string unique_id = action["unique_id"];
        if(action_type == "pickup"){
            auto restaurant_location = std::find_if(
                    restaurants.begin(),
                    restaurants.end(),
                    [order_restaurant](const std::shared_ptr<Restaurant> &restaurant){
                        return restaurant->get_id() == order_restaurant;
                    }).base()->get()->get_location();
            double distance = MAXFLOAT;
            int assign_id = -1;
            for(auto & driver : drivers) {
                std::pair<double, double> driver_location = driver.second->get_loacation();
                double driver_distance = direct_distance(driver_location, restaurant_location);
                if (order_capacity+driver.second->get_load() > driver.second->get_capacity()){
                    continue;
                }
                if (driver_distance < distance) {
                    distance = driver_distance;
                    assign_id = driver.second->get_id();
                }
            }
            if(!order->is_assigned() && assign_id != -1){
                driver_actions[std::to_string(assign_id)].push_back(unique_id);
                drivers[std::to_string(assign_id)]->set_load(drivers[std::to_string(assign_id)]->get_load()+order_capacity);
                order->set_assigned(true);
            }
        }else if(action_type == "dropoff"){
            std::string driver_id = action["data"]["driver"].dump();

            bool already_assigned = std::any_of(
                driver_actions[driver_id].begin(),
                driver_actions[driver_id].end(),
                [&unique_id](const auto &item){
                    return item == unique_id;
                });
            if(!already_assigned){
                driver_actions[driver_id].push_back(unique_id);
            }
        }
    }
}
