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
    vector<shared_ptr<Driver>> drivers = initialize_drivers(first_response["drivers"]);
    vector<shared_ptr<Restaurant>> restaurants = initialize_restaurants(first_response["restaurants"]);
    run_simulation(first_response, arg_host, drivers, restaurants);

    return 0;
}

vector<shared_ptr<Driver>> initialize_drivers(json drivers) {
    vector<shared_ptr<Driver>> return_drivers;

    for(auto & driver : drivers){
        int id = driver["id"];
        int capacity = driver["capacity"];
        int load = driver["current_load"];
        pair<double, double> location = driver["location"];
        int speed = driver["speed"];
        shared_ptr<Driver> new_driver = make_shared<Driver>(id, capacity, load, location, speed);
        return_drivers.push_back(new_driver);
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

void run_simulation(
        json & first_response,
        const string & host,
        vector<shared_ptr<Driver>> driver_list,
        vector<shared_ptr<Restaurant>> restaurant_list
        ) {
    vector<shared_ptr<Driver>> drivers = driver_list;
    vector<shared_ptr<Restaurant>> restaurants = restaurant_list;
    vector<shared_ptr<Order>> orders;
    json actions_to_send;
    map<string, vector<string>> driver_actions = {{"0",vector<string>()}};

    while(true) {
        actions_to_send["driver_actions"] = driver_actions;

        Response response = Post(Url{host + "/step"},
                                           Body{actions_to_send.dump()},
                                           Header{{"Content-Type","application/json"}});

        json parsed_response = json::parse(response.text);
        append_new_orders(orders, parsed_response["event"]);
        cout << parsed_response.dump() << endl;
        cout << actions_to_send.dump() << endl;

        if(parsed_response["event"] == "{}") {     // Sendet der Server kein Event, terminiert das Programm – Struktur eines leeren Events unklar!
            break;
        }
    }
}

void append_new_orders(vector<shared_ptr<Order>> &orders, json event) {
    if(event["event_type"] == "order"){
        int capacity = event["data"]["capacity"];
        string id = event["data"]["id"];
        int restaurant = event["data"]["restaurant"];
        pair<double, double> target_location = event["data"]["target"]["location"];

        shared_ptr<Order> new_order = make_shared<Order>(capacity, id, restaurant, target_location);

        orders.push_back(new_order);
    } else {
        cout << event.dump() << endl;
    }
}