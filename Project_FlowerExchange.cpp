#include <iostream>           // For input-output functionalities
#include <vector>             // For using vectors (dynamic arrays)
#include <fstream>            // For file handling functionalities
#include <sstream>            // For string stream manipulation
#include <string>             // For string manipulations

using namespace std;

string Check_Validity(const vector<string>& v);

// Input and output file streams
ifstream input_file("Orders.csv");
ofstream output_file("Execution_Rep.csv");

int main() {
    // Check if the input file was opened successfully
    if (!input_file.is_open()) {
        cerr << "Error opening input file." << endl;
        return 1; // Error return code
    }

    // Check if the output file was opened successfully
    if (!output_file.is_open()) {
        cerr << "Error opening output file." << endl;
        return 1; // Error return code
    }

    // Write headers to the output file
    output_file << "execution_rep.csv\n"; // Write to A1 and move to the next line
    output_file << "Order ID, Client Order ID, Instrument, Side, Exec Status, Quantity, Price, Reason\n";

    string line;
    // Read and skip the headers from the input file
    getline(input_file, line); // Skip first line
    getline(input_file, line); // Skip second line

    while (getline(input_file, line)) {  // Read each line from the input file
        stringstream lineStream(line);
        vector<string> fields;
        string field;

        // Split the line by commas and store each field in 'fields'
        while (getline(lineStream, field, ',')) {
            fields.push_back(field);
        }

        // Check the validity of the order
        string reason = Check_Validity(fields);

    }

    // Close the files
    input_file.close();
    output_file.close();
    
    return 0;
}

string Check_Validity(const vector<string>& v) {
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
