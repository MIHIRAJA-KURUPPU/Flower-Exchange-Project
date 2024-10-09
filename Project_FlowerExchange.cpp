#include <iostream>           // For input-output functionalities
#include <vector>             // For using vectors (dynamic arrays)
#include <fstream>            // For file handling functionalities
#include <sstream>            // For string stream manipulation
#include <string>             // For string manipulations
#include <unordered_set>      // For unordered set functionality

using namespace std;

// Function declaration to check order validity
string validateOrder(const vector<string>& fields);

// Order class definition
class Order {
public:
    string clientID;
    string item;
    int direction;  // 1 for buy, 2 for sell
    int amount;
    double cost;

    string orderIdentifier;
    int orderStatus;
    string rejectionReason;

    // Constructor
    Order(string clientID, string item, int direction, int amount, double cost) 
        : clientID(clientID), item(item), direction(direction), amount(amount), cost(cost) {}
};

// OrderBookItem class definition for order book entries
class OrderBookEntry {
public:
    string identifier;
    int amount;
    double cost;

    OrderBookEntry(string identifier, int amount, double cost)
        : identifier(identifier), amount(amount), cost(cost) {}
};

// OrderBook class for managing orders
class OrderBook {
private:
    string itemType;
    vector<OrderBookEntry> buyOrders;
    vector<OrderBookEntry> sellOrders;

public:
    OrderBook(string itemType) {
        this->itemType = itemType;
    }

    void addOrder(const Order& order) {
        if (order.direction == 1) {  // Buy order
            buyOrders.emplace_back(order.orderIdentifier, order.amount, order.cost);
        } else {  // Sell order
            sellOrders.emplace_back(order.orderIdentifier, order.amount, order.cost);
        }
    }
};

// Function to check the validity of an order
string validateOrder(const vector<string>& fields) {
    static unordered_set<string> validItems = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};

    // Check for the correct number of fields
    if (fields.size() != 5) {
        return "Invalid input size";
    }

    const string& clientID = fields[0];
    const string& item = fields[1];
    int direction, amount;
    double cost;

    // Check for empty fields
    if (clientID.empty()) return "Invalid client order ID";
    if (item.empty()) return "Invalid item";
    if (fields[2].empty() || fields[3].empty() || fields[4].empty()) return "Missing numerical fields";

    // Convert and validate direction, amount, and cost
    try {
        direction = stoi(fields[2]);
        amount = stoi(fields[3]);
        cost = stod(fields[4]);
    } catch (const invalid_argument&) {
        return "Invalid numerical conversion";
    }

    // Validate item
    if (validItems.find(item) == validItems.end()) {
        return "Invalid item";
    }

    // Validate direction
    if (direction != 1 && direction != 2) {
        return "Invalid direction";
    }

    // Validate cost
    if (cost < 0) {
        return "Invalid cost";
    }

    // Validate amount (should be a multiple of 10, between 10 and 1000)
    if (amount < 10 || amount > 1000 || amount % 10 != 0) {
        return "Invalid amount";
    }

    return "";  // Valid order
}

int main() {
    // Open the Orders.csv file
    ifstream ordersFile("Orders.csv");

    // Check if the file opened successfully
    if (!ordersFile.is_open()) {
        cout << "Error: Unable to open the file." << endl;
        return 1;  // Exit with an error code
    }

    string line;

    // Read the header line and discard it
    getline(ordersFile, line);  
    // Initialize order books for different items
    OrderBook roseBook("Rose");
    OrderBook lavenderBook("Lavender");
    OrderBook tulipBook("Tulip");
    OrderBook orchidBook("Orchid");
    OrderBook lotusBook("Lotus");

    int orderCounter = 1;

    // Debug message indicating the start of processing
    cout << "Starting order processing..." << endl;

    // Read each line in the file
    while (getline(ordersFile, line)) {
        stringstream ss(line);
        vector<string> fields;
        string cell;

        // Split the line into fields
        while (getline(ss, cell, ',')) {
            fields.push_back(cell);
        }

        // Debug: Print the fields being processed
        cout << "Processing row: ";
        for (const string& s : fields) {
            cout << "[" << s << "] ";
        }
        cout << endl;

        // Validate the order
        string validationMessage = validateOrder(fields);
        string orderIdentifier = "ord" + to_string(orderCounter);

        // Debug message before converting to numerical values
        cout << "Attempting to convert direction: " << fields[2] << ", amount: " << fields[3] << ", cost: " << fields[4] << endl;

        try {
            int direction = stoi(fields[2]);
            int amount = stoi(fields[3]);
            double cost = stod(fields[4]);

            Order order(orderIdentifier, fields[1], direction, amount, cost);

            // Debug message after order creation
            cout << "Created order: " << orderIdentifier << " for item: " << fields[1] << endl;

            // If order is valid, add it to the respective order book
            if (validationMessage.empty()) {
                if (fields[1] == "Rose") {
                    roseBook.addOrder(order);
                } else if (fields[1] == "Lavender") {
                    lavenderBook.addOrder(order);
                } else if (fields[1] == "Lotus") {
                    lotusBook.addOrder(order);
                } else if (fields[1] == "Tulip") {
                    tulipBook.addOrder(order);
                } else if (fields[1] == "Orchid") {
                    orchidBook.addOrder(order);
                }

                // Debug message indicating the order was added successfully
                cout << "Order added to book for item: " << fields[1] << endl;
            } else {
                // Debug message for invalid orders
                cout << "Order invalid: " << validationMessage << endl;
            }
        } catch (const invalid_argument& e) {
            cout << "Error parsing order: " << e.what() << " in row: " << line << endl;
            continue;
        } catch (const out_of_range& e) {
            cout << "Out of range error: " << e.what() << " in row: " << line << endl;
            continue;
        }

        orderCounter++;
    }

    // Close the file
    ordersFile.close();
    cout << "Processing complete." << endl;
    return 0;
}
