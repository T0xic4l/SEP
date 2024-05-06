#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../includes/main.h"

using namespace std;
using namespace cpr;
using namespace nlohmann;
using namespace argparse;

int main(int argc, char * argv[]) {
    ArgumentParser arpa("JummySalesman", "0");
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
    map<string, shared_ptr<Driver>> drivers = initialize_drivers(first_response["drivers"]);
    vector<shared_ptr<Restaurant>> restaurants = initialize_restaurants(first_response["restaurants"]);
    run_simulation(first_response, arg_host, drivers, restaurants);

    return 0;
}

map<string, shared_ptr<Driver>> initialize_drivers(json drivers) {
    map<string, shared_ptr<Driver>> return_drivers;

    for(auto & driver : drivers){
        cout << driver.dump() << endl;
        int id = driver["id"];
        int capacity = driver["capacity"];
        int load = driver["current_load"];
        pair<double, double> location = driver["location"];
        int speed = driver["speed"];
        shared_ptr<Driver> new_driver = make_shared<Driver>(id, capacity, load, location, speed);
        return_drivers.insert({{to_string(id), new_driver}});
    }
    return return_drivers;
}

vector<shared_ptr<Restaurant>> initialize_restaurants(json restaurant_list) {
    vector<shared_ptr<Restaurant>> restaurants;

    for(auto & restaurant : restaurant_list){
        int id = restaurant["id"];
        pair<double, double> location = restaurant["location"];
        shared_ptr<Restaurant> new_restaurant = make_shared<Restaurant>(id, location);
        restaurants.push_back(new_restaurant);
    }

    return restaurants;
}

json start_simulation(const string & host, const string & seed) {
    json seed_json = json::parse(R"({"seed":")" + seed + "\"}");

    Response response = Post(Url{host + "/"},
                                       Body{seed_json.dump()},
                                       Header{{"Content-Type","application/json"}});
    return json::parse(response.text);
}

void run_simulation(json & first_response, const string & host, map<string, shared_ptr<Driver>> driver_list, vector<shared_ptr<Restaurant>> restaurant_list) {
    map<string, shared_ptr<Driver>> drivers = driver_list;
    vector<shared_ptr<Restaurant>> restaurants = restaurant_list;
    map<string, shared_ptr<Order>> orders;
    json actions_to_send;
    map<string, vector<string>> driver_actions = {{"0",vector<string>()}};

    while(true) {
        actions_to_send["driver_actions"] = driver_actions;

        Response response = Post(Url{host + "/step"},
                                           Body{actions_to_send.dump()},
                                           Header{{"Content-Type","application/json"}});

        json parsed_response = json::parse(response.text);
        handle_event(orders, drivers, driver_actions, parsed_response["event"]);
        update_drivers(drivers, parsed_response["drivers"]);
        choose_actions(parsed_response["mandatory_actions"], orders, drivers, restaurants, driver_actions);
        cout << actions_to_send.dump() << endl;

        cout << "Score: " << parsed_response["score"] << endl;
        if(parsed_response["error"] || parsed_response["event"] == "{}") {
            cout << parsed_response.dump() << endl;
            break;
        }
    }
}

void handle_event(
    map<string, shared_ptr<Order>> &orders,
    map<string, shared_ptr<Driver>> &drivers,
    map<string, vector<string>> &driver_actions,
    json event
    ) {
    if(event["event_type"] == "order"){
        string order_id = event["data"]["id"];
        int capacity = event["data"]["capacity"];
        int restaurant = event["data"]["restaurant"];
        pair<double, double> target_location = event["data"]["target"]["location"];

        shared_ptr<Order> new_order = make_shared<Order>(capacity, order_id, restaurant, target_location);

        orders.insert({{order_id,  new_order}});
    } else if(event["event_type"] == "pickup" || event["event_type"] == "dropoff") {
        int driver_id = event["data"]["driver"];
        string order_id = event["data"]["order_id"];
        string action = event["data"]["action"];
        auto result = find_if(
                driver_actions[to_string(driver_id)].begin(),
                driver_actions[to_string(driver_id)].end(),
                [&action](const auto &item) {
                    return item == action;
                });
        if(result != driver_actions[to_string(driver_id)].end())
            driver_actions[to_string(driver_id)].erase(result);

        if(event["event_type"] == "dropoff"){ //falls das event ein dropoff ist verringere den load des drivers
            int current_load = drivers[to_string(driver_id)]->get_load();
            int order_cap = orders[order_id]->get_capacity();
            drivers[to_string(driver_id)]->set_load(current_load-order_cap);
        }
    } else {
        cout << event.dump() << endl;
    }
}

void update_drivers(map<string, shared_ptr<Driver>> &drivers, json updated_drivers){
    for(auto & updated_driver : updated_drivers){
        string update_id = updated_driver["id"].dump();
        drivers[update_id]->set_location(updated_driver["location"]);
    }
}

void choose_actions(
        json mandatory_actions,
        map<string, shared_ptr<Order>> &orders,
        map<string, shared_ptr<Driver>> &drivers,
        vector<shared_ptr<Restaurant>> &restaurants,
        map<string, vector<string>> &driver_actions
        ) {
    for(auto & action : mandatory_actions){
        string action_type = action["action_type"];
        string order_id = action["data"]["order_id"];

        shared_ptr<Order> order = orders[order_id];
        int order_restaurant = order->get_restaurant();
        int order_capacity = order->get_capacity();

        string unique_id = action["unique_id"];
        if(action_type == "pickup"){
            auto restaurant_location = find_if(
                    restaurants.begin(),
                    restaurants.end(),
                    [order_restaurant](const shared_ptr<Restaurant> &restaurant){
                        return restaurant->get_id() == order_restaurant;
                    }).base()->get()->get_location();
            double distance = MAXFLOAT;
            int assign_id = -1;
            for(auto & driver : drivers) {
                pair<double, double> driver_location = driver.second->get_loacation();
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
                driver_actions[to_string(assign_id)].push_back(unique_id);
                drivers[to_string(assign_id)]->set_load(drivers[to_string(assign_id)]->get_load()+order_capacity);
                order->set_assigned(true);
            }
        }else if(action_type == "dropoff"){
            string driver_id = action["data"]["driver"].dump();

            bool already_assigned = any_of(
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
