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


ifstream input_file("Orders.csv");
ofstream output_file("Execution_Rep.csv");

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
    string instrument;
    vector<OrderBookItem> buy;
    vector<OrderBookItem> sell;

    OrderBook(string instrument)
    {
        this->instrument = instrument;
    }

    void addOrder(Order order)
    {
        vector<string> status = {"New", "Rejected", "Fill", "PFill"};

        // first check the order is valid, if not write to the output file - (status = Rejected)
        if (order.reason != "")
        {
            order.status = status[1];
            writeLineOutputFile(order);
            return;
        }

        // create new item object
        OrderBookItem *item = new OrderBookItem(order.orderID, order.quantity, order.price);

        if (order.side == 1)
        {
            // if the sell vecotr is empty or when the order is not matching with any sell orders,
            if (sell.empty() || sell.begin()->price > order.price)
            {
                // write to the output file - (status = New)
                order.status = status[0];
                writeLineOutputFile(order);

                // place corrcetly in buy vector, as buy vector is in the descending order of price
                if (buy.empty())
                {
                    buy.push_back(*item);
                    return;
                }
                else
                {
                    for (int i = 0; i < buy.size(); i++)
                    {
                        if (buy[i].price < order.price)
                        {
                            buy.insert(buy.begin() + i, *item);
                            return;
                        }
                        buy.insert(buy.begin(), *item);
                    }
                    return;
                }
            }

            // when the order is matching with a sell orders,
            while (!sell.empty() && sell.begin()->price <= order.price && order.quantity > 0)
            {
                if (sell.begin()->quantity < order.quantity)
                {
                    // status of the order changed to PFill
                    order.status = status[3];
                    output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << order.status << "," << sell.begin()->quantity << "," << order.price << "," << order.reason << endl;

                    order.quantity -= sell.begin()->quantity;

                    Order *ord1 = findOrderByOrdID(order.orderID); // Get the order details by order ID
                    ord1->quantity = order.quantity;               // update the current quantity of the order in orders vector

                    // status of the sell.begin() change to Fill
                    string b = sell.begin()->orderID;
                    Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                    ord->status = status[2];          // Change the status of the sell.begin()
                    writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                    ord->quantity = 0; // update the current quantity of the order in orders vector

                    sell.erase(sell.begin());
                }
                else
                {
                    // status of the order changed to Fill
                    order.status = status[2];
                    order.price = sell.begin()->price;
                    writeLineOutputFile(order);

                    sell.begin()->quantity -= order.quantity;
                    order.quantity = 0;

                    if (sell.begin()->quantity == 0)
                    {
                        // status of the sell.begin() change to Fill
                        string b = sell.begin()->orderID;
                        Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                        ord->status = status[2];          // Change the status of the sell.begin()
                        writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                        ord->quantity = 0; // update the current quantity of the sell order in orders vector

                        sell.erase(sell.begin());
                        return;
                    }
                    else
                    {
                        // status of the sell.begin() change to PFill
                        string b = sell.begin()->orderID;
                        Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                        ord->status = status[3];          // Change the status of the sell.begin()
                        writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                        ord->quantity = sell.begin()->quantity; // update the current quantity of the sell order in orders vector
                        return;
                    }
                }
            }

            // place the order in the buy vector
            item->quantity = order.quantity;
            buy.insert(buy.begin(), *item);
        }
        else
        {
            // if the buy vecotr is empty or when the order is not matching with any buy orders,
            if (buy.empty() || buy.begin()->price < order.price)
            {
                // write to the output file - (status = New)
                order.status = status[0];
                writeLineOutputFile(order);

                // place corrcetly in sell vector, as sell vector is in the ascending order of price
                if (sell.empty())
                {
                    sell.push_back(*item);
                    return;
                }
                else
                {
                    for (int i = 0; i < sell.size(); i++)
                    {
                        if (sell[i].price > order.price)
                        {
                            sell.insert(sell.begin() + i, *item);
                            return;
                        }
                        sell.push_back(*item);
                    }
                }
            }

            // when the order is matching with a buy order,
            while (!buy.empty() && buy.begin()->price >= order.price && order.quantity > 0)
            {
                if (buy.begin()->quantity < order.quantity)
                {
                    // status of the order changed to PFill
                    output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << status[3] << "," << buy.begin()->quantity << "," << buy.begin()->price << "," << order.reason << endl;

                    order.quantity -= buy.begin()->quantity;

                    Order *ord1 = findOrderByOrdID(order.orderID); // Get the order details by order ID
                    ord1->quantity = order.quantity;               // update the current quantity of the order in orders vector

                    // status of the buy.begin() change to Fill
                    string b = buy.begin()->orderID;
                    Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                    ord->status = status[2];          // Change the status of the sell.begin()
                    writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                    ord->quantity = 0; // update the current quantity of the buy order in orders vector

                    buy.erase(buy.begin());
                }
                else
                {
                    // status of the order changed to Fill
                    output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << status[2] << "," << order.quantity << "," << buy.begin()->price << "," << order.reason << endl;

                    buy.begin()->quantity -= order.quantity;
                    order.quantity = 0;

                    Order *ord1 = findOrderByOrdID(order.orderID); // Get the order details by order ID
                    ord1->quantity = order.quantity;               // update the current quantity of the order in orders vector

                    if (buy.begin()->quantity == 0)
                    {
                        // status of the buy.begin() change to Fill
                        string b = buy.begin()->orderID;
                        Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                        ord->status = status[2];          // Change the status of the sell.begin()
                        writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                        ord->quantity = 0; // update the current quantity of the buy order in orders vector

                        buy.erase(buy.begin());
                        return;
                    }
                    else
                    {
                        // status of the buy.begin() change to PFill
                        string b = buy.begin()->orderID;
                        Order *ord = findOrderByOrdID(b); // Get the order details by order ID
                        ord->status = status[3];          // Change the status of the sell.begin()
                        writeLineOutputFile(*ord);        // Fix: Dereference the pointer to ord

                        ord->quantity = buy.begin()->quantity; // update the current quantity of the buy order in orders vector
                        return;
                    }
                }
            }

            // place the order in the sell vector
            item->quantity = order.quantity;
            sell.insert(sell.begin(), *item);
        }
    }

    void writeLineOutputFile(Order order)
    {
        output_file << order.orderID << "," << order.clientOrderID << "," << order.instrument << "," << order.side << "," << order.status << "," << order.quantity << "," << order.price << "," << order.reason << endl;
        return;
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

    output_file << formatTime << endl;
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