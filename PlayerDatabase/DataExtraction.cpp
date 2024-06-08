#include "DataExtraction.h"

using namespace std;

void trim(string& str) {
    // Find first non-space, non-quote character (excluding digits) after potential prefix
    size_t start = str.find_first_not_of(" '\"!", 0);  // Assuming prefix starts with "!"
    if (start == string::npos) {  // String is all spaces, quotes, or non-digits
        str = "";
        return;
    }

    // Find last non-space, non-quote character (excluding digits)
    size_t end = str.find_last_not_of(" '\"!", 0);  // Search from beginning (0)

    // Extract substring without spaces, quotes, or non-digits (excluding digits)
    str = str.substr(start, end - start);
}

void createInputFile(const string& filename) {
    ofstream outfile(filename);
    if (outfile.is_open()) {
        cout << "Input file '" << filename << "' does not exist. Created a new one.\n";
        outfile << "** This is a placeholder file. Please enter your data here. **\n";
        outfile << "** Make sure to delete these two lines as well before entering data **\n";
    }
    else {
        cerr << "Error creating input file: " << filename << endl;
    }
    outfile.close();
}

string ExtractName(string InputName)
{
    size_t StartingPosition{ InputName.find('"')}, EndingPosition{ 0 };

    for (auto i{ StartingPosition+1 }; i < InputName.size(); i++)
    {
        if (InputName.at(i) == '"')
        {
            EndingPosition = i;
            break;
        }
    }

    return InputName.substr(StartingPosition + 1, EndingPosition - StartingPosition - 1);
}

vector<string> extractData(const string& filename) {
    ifstream infile(filename);
    vector<string> extractedData;

    if (!infile.is_open()) {
        cerr << "Error opening input file: " << filename << endl;
        return extractedData;
    }

    string line;
    while (getline(infile, line)) {
        stringstream lineStream(line);
        string id64, ip, name;

        // Extract id64 (assuming consistent format)
        lineStream >> id64 >> skipws;

        // Extract IP address (considering variations)
        getline(lineStream, ip);

        name = ExtractName(line);

        // Remove leading/trailing spaces, quotes, and non-digit prefixes
        trim(ip);

        // *Regular expression parsing*
        regex ipRegex(R"([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}.*)");
        smatch match;
        if (regex_search(ip, match, ipRegex)) {
            ip = match[0].str();  // Extract matched IP substring
        }

        //mini trim that makes ip extraction even more accurate by only taking it upto colon and ignoring everything else
        size_t colonPos = ip.find(':');
        if (colonPos != string::npos) {
            ip = ip.substr(0, colonPos); // Extract substring up to colon
        }

        extractedData.push_back(id64);
        extractedData.push_back(ip);
        extractedData.push_back(name);
    }

    infile.close();
    return extractedData;
}

void writeExtractedData(const vector<string>& data, const string& OutputFileName1, const string& OutputFileName2, const string& OutputFileName3) {
    ofstream idFile(OutputFileName1);
    ofstream ipFile(OutputFileName2);
    ofstream NameFile(OutputFileName3);

    if (!idFile.is_open()) {
        cerr << "Error opening output file: " << OutputFileName1 << endl;
        return;
    }
    if (!ipFile.is_open()) {
        cerr << "Error opening output file: " << OutputFileName2 << endl;
        return;
    }
    if (!NameFile.is_open()) {
        cerr << "Error opening output file: " << OutputFileName3 << endl;
        return;
    }

    for (size_t i = 0; i < data.size(); i += 3) {
        idFile << data[i] << endl;
        ipFile << data[i + 1] << endl;
        NameFile << data[i + 2] << endl;
    }

    idFile.close();
    ipFile.close();
    NameFile.close();
    cout << "Data extraction complete. Check the output file for ids: " << OutputFileName1 << endl;
    cout << "Data extraction complete. Check the output file for ips: " << OutputFileName2 << endl;
    cout << "Data extraction complete. Check the output file for names: " << OutputFileName3 << endl;
}