#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

using namespace std::chrono;
using namespace std;


ifstream input_file("Orders_Example7.csv");
ofstream output_file("ExecReports7.csv");

string  Check_Validity(vector<string> v);
string formatTime(double elapsed_time);

class Order
{
public:
    string orderID;
    string clientOrderID;
    string instrument;
    int side;
    string status;
    int quantity;
    double price;
    string reason;

    Order(string orderID, string clientOrderID, string instrument, int side, int quantity, double price, string reason)
    {
        this->orderID = orderID;
        this->clientOrderID = clientOrderID;
        this->instrument = instrument;
        this->side = side;
        this->quantity = quantity;
        this->price = price;
        this->reason = reason;
    }
};

vector<Order> orders;

Order *findOrderByOrdID(string order_id)
{
    for (auto &o : orders)
    {
        if (o.orderID == order_id)
        {
            return &o;
        }
    }
    return nullptr;
}


class OrderBookItem
{
public:
    string orderID;
    int quantity;
    double price;

    OrderBookItem(string orderID, int quantity, double price)
    {
        this->orderID = orderID;
        this->quantity = quantity;
        this->price = price;
    }
};

class OrderBook
{
public:
    string instrument; // Instrument or security being traded (e.g., stock symbol)
    vector<OrderBookItem> buy; // Vector to hold buy orders
    vector<OrderBookItem> sell; // Vector to hold sell orders

    // Constructor to initialize the order book with a specific instrument
    OrderBook(string instrument)
    {
        this->instrument = instrument;
    }

    // Adds a new order to the order book
    void addOrder(Order order)
    {
        // Define possible order statuses: New, Rejected, Fill, Partial Fill
        vector<string> status = {"New", "Rejected", "Fill", "PFill"};

        // Check if the order is valid (if not, reject it)
        if (order.reason != "")
        {
            order.status = status[1]; // Set status to "Rejected"
            writeLineOutputFile(order); // Log the order details
            return; // Exit function as the order is invalid
        }

        // Create a new order book item from the order
        OrderBookItem *item = new OrderBookItem(order.orderID, order.quantity, order.price);

        // Process the order based on its side (buy or sell)
        if (order.side == 1)
        {
            // Buy order processing
            // If the sell side is empty or the lowest sell price is higher than the buy price, insert the order
            if (sell.empty() || sell.begin()->price > order.price)
            {
                order.status = status[0]; // Set status to "New"
                writeLineOutputFile(order); // Log the order
                insertInBuyVector(item); // Insert order into the buy vector
            }
            else
            {
                // Match the buy order with sell orders
                matchOrderWithSell(item, order, status);
            }
        }
        else
        {
            // Sell order processing
            // If the buy side is empty or the highest buy price is lower than the sell price, insert the order
            if (buy.empty() || buy.begin()->price < order.price)
            {
                order.status = status[0]; // Set status to "New"
                writeLineOutputFile(order); // Log the order
                insertInSellVector(item); // Insert order into the sell vector
            }
            else
            {
                // Match the sell order with buy orders
                matchOrderWithBuy(item, order, status);
            }
        }
    }

    // Inserts an order into the buy vector in descending order of price
    void insertInBuyVector(OrderBookItem *item)
    {
        if (buy.empty())
        {
            buy.push_back(*item); // If buy vector is empty, just insert the item
        }
        else
        {
            // Find the correct position to insert the new item based on price
            auto it = buy.begin();
            while (it != buy.end() && it->price >= item->price)
            {
                ++it;
            }
            buy.insert(it, *item); // Insert the order at the correct place
        }
    }

    // Inserts an order into the sell vector in ascending order of price
    void insertInSellVector(OrderBookItem *item)
    {
        if (sell.empty())
        {
            sell.push_back(*item); // If sell vector is empty, just insert the item
        }
        else
        {
            // Find the correct position to insert the new item based on price
            auto it = sell.begin();
            while (it != sell.end() && it->price <= item->price)
            {
                ++it;
            }
            sell.insert(it, *item); // Insert the order at the correct place
        }
    }

// Matches a buy order with existing sell orders
    void matchOrderWithSell(OrderBookItem *item, Order &order, vector<string> &status)
    {
        // Continue matching while there are sell orders at a price <= the buy price
        while (!sell.empty() && sell.begin()->price <= order.price && order.quantity > 0)
        {
            if (sell.begin()->quantity < order.quantity)
            {
                // Partial fill: Buy order partially filled
                order.status = status[3]; // Partial Fill
                output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << order.status << "," << sell.begin()->quantity << "," << order.price << "," << order.reason << endl;

                // Decrease buy order quantity by the matched sell order quantity
                order.quantity -= sell.begin()->quantity;
                // Update the matched sell order status to "Fill" and log it
                Order *ord = findOrderByOrdID(sell.begin()->orderID);
                ord->status = status[2]; // Fill
                writeLineOutputFile(*ord);
                
                sell.erase(sell.begin()); // Remove the matched sell order
            }
            else
            {
                // Full fill: Buy order fully filled
                order.status = status[2]; // Fill
                order.price = sell.begin()->price; // Set buy order price to matched sell price
                writeLineOutputFile(order); // Log the filled buy order

                // Decrease sell order quantity by the buy order quantity
                int matched_quantity = order.quantity;
                sell.begin()->quantity -= order.quantity;
                order.quantity = 0; // Buy order fully filled

                // If sell order is fully filled, remove it
                if (sell.begin()->quantity == 0)
                {
                    Order *ord = findOrderByOrdID(sell.begin()->orderID);
                    ord->status = status[2]; // Fill
                    ord->quantity = matched_quantity; // Fill
                    writeLineOutputFile(*ord);
                    sell.erase(sell.begin()); // Remove the matched sell order
                }
            }
        }
        // If there's any remaining buy quantity, insert it into the buy vector
        if (order.quantity > 0)
        {
            item->quantity = order.quantity;
            insertInBuyVector(item); // Insert remaining part of the buy order
        }
    }

    // Matches a sell order with existing buy orders
    void matchOrderWithBuy(OrderBookItem *item, Order &order, vector<string> &status)
    {
        // Continue matching while there are buy orders at a price >= the sell price
        while (!buy.empty() && buy.begin()->price >= order.price && order.quantity > 0)
        {
            if (buy.begin()->quantity < order.quantity)
            {
                // Partial fill: Sell order partially filled
                order.status = status[3]; // Partial Fill
                output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << order.status << "," << buy.begin()->quantity << "," << buy.begin()->price << "," << order.reason << endl;

                // Decrease sell order quantity by the matched buy order quantity
                order.quantity -= buy.begin()->quantity;

                // Update the matched buy order status to "Fill" and log it
                Order *ord = findOrderByOrdID(buy.begin()->orderID);
                ord->status = status[2]; // Fill
                writeLineOutputFile(*ord);
                buy.erase(buy.begin()); // Remove the matched buy order
            }
            else
            {
                // Full fill: Sell order fully filled
                order.status = status[2]; // Fill
                writeLineOutputFile(order); // Log the filled sell order

                // Decrease buy order quantity by the sell order quantity
                int matched_quantity = order.quantity;
                buy.begin()->quantity -= order.quantity;
                order.quantity = 0; // Sell order fully filled

                // If buy order is fully filled, remove it
                if (buy.begin()->quantity == 0)
                {
                    Order *ord = findOrderByOrdID(buy.begin()->orderID);
                    ord->status = status[2]; // Fill
                    ord->quantity = matched_quantity; // Fill
                    writeLineOutputFile(*ord);
                    buy.erase(buy.begin()); // Remove the matched buy order
                }
            }
        }
        // If there's any remaining sell quantity, insert it into the sell vector
        if (order.quantity > 0)
        {
            item->quantity = order.quantity;
            insertInSellVector(item); // Insert remaining part of the sell order
        }
    }

    // Writes order details to the output file
    void writeLineOutputFile(Order order)
    {
        output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << order.status << "," << order.quantity << "," << order.price << "," << order.reason << endl;
    }
};

int main()
{
    auto start = high_resolution_clock::now();

    // If the input file isn't open, return an error
    if (!input_file.is_open())
    {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    // If the output file isn't open, return an error
    if (!output_file.is_open())
    {
        cerr << "Error creating the output file." << endl;
        return 1;
    }

    // Add the header row to the output file
    output_file << "execution_rep.csv" << endl;

    // Add the field names row to the output file
    output_file << "Order ID, Client Order ID, Instrument, Side, Execution Status, Quantity, Price, Reason" << endl;

    
    string line;
    // Read and skip the headers from the input file
    getline(input_file, line); // Skip first line
    getline(input_file, line); // Skip second line

    // initially create 5 order books
    OrderBook *rose = new OrderBook("Rose");
    OrderBook *lavender = new OrderBook("Lavender");
    OrderBook *tulip = new OrderBook("Tulip");
    OrderBook *orchid = new OrderBook("Orchid");
    OrderBook *lotus = new OrderBook("Lotus");

    int ordNumber = 1;
    // read all the rows in the input file
    while (getline(input_file, line))
    {

        stringstream linestream(line);
        vector<string> v;
        string field;

        while (getline(linestream, field, ','))
        {
            v.push_back(field);
            // vector v contains data of 5 fiels of a row
        }

        // check the validity of the order

        string a =Check_Validity(v);
        string ordNumberString = "ord" + to_string(ordNumber);

        Order *order = new Order(ordNumberString, v[0], v[1], stoi(v[2]), stoi(v[3]), stod(v[4]), a);
        orders.push_back(*order);

        if (v[1] == "Rose")
        {
            rose->addOrder(*order);
        }
        else if (v[1] == "Lavender")
        {
            lavender->addOrder(*order);
        }
        else if (v[1] == "Tulip")
        {
            tulip->addOrder(*order);
        }
        else if (v[1] == "Orchid")
        {
            orchid->addOrder(*order);
        }
        else if (v[1] == "Lotus")
        {
            lotus->addOrder(*order);
        }
        else
        {
            output_file << ordNumberString << "," << v[0] << "," << v[1] << "," << v[2] << ","
                  << "Rejected"
                  << "," << v[3] << "," << v[4] << "," << a << endl;
        }

        ordNumber++;
    }



    auto stop = high_resolution_clock::now();

    // Calculate elapsed time in seconds with fractions
    duration<double> elapsed = stop - start;
    double elapsed_time = elapsed.count(); // Time in seconds

    // Format and print the time string
    string formatted_time = formatTime(elapsed_time);
    cout << "Transaction time: " << formatted_time << std::endl;

    output_file << formatted_time << endl;
    input_file.close();
    output_file.close();

    return 0;
}
// Function to convert seconds into formatted time string
string formatTime(double elapsed_time) {
    // Calculate hours, minutes, and seconds from elapsed time
    int total_seconds = static_cast<int>(elapsed_time);
    int hours = total_seconds / 3600;
    int minutes = (total_seconds % 3600) / 60;
    int seconds = total_seconds % 60;
    double fractional_seconds = elapsed_time - total_seconds;

    // Construct the formatted string
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(8) << "00000000" << "-" // Prepend 00000000
               << std::setw(2) << hours 
               << std::setw(2) << minutes 
               << std::setw(2) << seconds
               << std::fixed << std::setprecision(3) 
               << fractional_seconds;

    return timeStream.str();
}


string Check_Validity(vector<string> v) {
    if (v[0].empty()) return "Invalid client order ID";
    if (v[1].empty()) return "Invalid instrument";
    if (v[2].empty()) return "Invalid side";
    if (v[3].empty()) return "Invalid size";
    if (v[4].empty()) return "Invalid price";

    int side = stoi(v[2]);
    int quantity = stoi(v[3]);
    double price = stod(v[4]);

    if (v[1] != "Rose" && v[1] != "Lavender" && v[1] != "Tulip" && v[1] != "Orchid" && v[1] != "Lotus")
        return "Invalid instrument";
    if (side != 1 && side != 2)
        return "Invalid side";
    if (price < 0)
        return "Invalid price";
    if (quantity % 10 != 0 || quantity < 10 || quantity > 1000)
        return "Invalid size";

    return ""; // Return empty string if the order is valid
}