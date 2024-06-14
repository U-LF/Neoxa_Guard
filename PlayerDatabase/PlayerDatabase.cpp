#include "DataExtraction.h"
#include "Functions.h"
#include "Libraries.h"

using namespace std;
using namespace sql;

int main() {

    ifstream ServerConInfo("ServerInfo.txt");

    if (!ServerConInfo.is_open())
    {
        cout << "\nFile to get MySQL server info doesn't exist, it will now be made\n\n";

        ofstream ServerInfo("ServerInfo.txt");
        ServerInfo.close();

        ofstream DummyServerInfo("SampleServerInfo.txt");

        DummyServerInfo << "only insert address like this 'tcp://127.0.0.1:3306' in the first line" << endl;
        DummyServerInfo << "insert only server name in the second line like this 'root'" << endl;
        DummyServerInfo << "insert only the server password in the thid line like this 'password123'" << endl;

        DummyServerInfo.close();

        cout << "File for server info has been made with the name 'ServerInfo.txt' kindly enter the info to connect to server in it and start the program again\n";
        cout << "\nSee the 'SampleServerInfo.txt' to see on how to insert data in 'ServerInfo.txt'\n\n";

        system("PAUSE");

        return 0;
    }

    string Server, UserName, Password;

    ServerConInfo >> Server;
    ServerConInfo >> UserName;
    ServerConInfo >> Password;

    ServerConInfo.close();

    cout << "Server: " << Server << endl;
    cout << "UserName: " << UserName << endl;
    cout << "Password: " << Password << endl;

    Driver* driver = nullptr;
    Connection* con = nullptr;
    Statement* stmt = nullptr;
    PreparedStatement* pstmt = nullptr;
    ResultSet* res = nullptr;

    try
    {
        driver = get_driver_instance();
        con = driver->connect(Server, UserName, Password);
        cout << "\nConnection to MySQL server successful\n\n";
    }
    catch (SQLException e)
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    //checking to see if the database exists
AttemptToCreateDatabase:
    try
    {
        con->setSchema("player_db");
        cout << "\n'player_db' found and is ready to be used\n\n";
    }
    catch (SQLException& e)
    {
        cout << "Error msg: " << e.what() << endl;

        char DatabaseCreationChoice{ ' ' };

        do
        {
            cout << "Database 'player_db' not found, would you like to make one? (Y/N)\n";
            cout << "Your choice: ";

            DatabaseCreationChoice = _getch();

            cout << DatabaseCreationChoice << endl;

            DatabaseCreationChoice = toupper(DatabaseCreationChoice);

            if (DatabaseCreationChoice == 'Y')
            {
                stmt = con->createStatement();
                stmt->execute("CREATE DATABASE player_db");

                cout << "Creating database...\n";
                cout << "Database created\n";
                cout << "Creating tables...\n";

                con->setSchema("player_db");
                stmt->execute("CREATE TABLE id64 (id VARCHAR(17) PRIMARY KEY NOT NULL, id_name VARCHAR(50) NOT NULL)");
                stmt->execute("CREATE TABLE ips (ip VARCHAR(20) PRIMARY KEY NOT NULL)");
                stmt->execute("CREATE TABLE relation_id64_ip (link_date DATE NOT NULL DEFAULT (CURRENT_DATE), ips VARCHAR(20) NOT NULL, id VARCHAR(17) NOT NULL, FOREIGN KEY (ips) REFERENCES ips(ip), FOREIGN KEY (id) REFERENCES id64(id), PRIMARY KEY(ips,id))");
                
                cout << "Tables created successfully\n";
                cout << "\nDatabase setup is now complete\n\n";

                goto AttemptToCreateDatabase;
            }
            else if (DatabaseCreationChoice == 'N')
            {
                cout << "The program can't be used without the existence of database hence program will now be exited\n";
                system("PAUSE");
                return 0;
            }
            else
            {
                cout << "\nInvalid choice, Kindly select (Y) for yes and (N) for no\n\n";
            }

        } while (DatabaseCreationChoice != 'Y' && DatabaseCreationChoice != 'N');
    }

    char Choice{ ' ' };

    do
    {
        MainMenu(Choice);

        switch (Choice)
        {
            case 'A':
            {
                cout << "Before proceeding any further make sure that you have entered data in the file 'data.txt' (if it exists)\n";

                system("PAUSE");

                const string InputFilename = "data.txt";
                const string IdFile = "extracted_data_id.txt";
                const string IpFile = "extracted_data_ip.txt";
                const string NameFile = "extracted_data_name.txt";

                // Check if input file exists
                ifstream infile(InputFilename);

                if (!infile.is_open()) {
                    createInputFile(InputFilename);
                }
                infile.close();

                vector<string> extractedData = extractData(InputFilename);

                writeExtractedData(extractedData, IdFile, IpFile, NameFile);

                ifstream InputIdFile(IdFile);
                ifstream InputIpFile(IpFile);
                ifstream InputNameFile(NameFile);

                vector<string> Ids, Ips, Name;

                while (!InputIdFile.eof())
                {
                    string Line;
                    InputIdFile >> Line;
                    if (Line == "")
                    {
                        continue;
                    }
                    Ids.push_back(Line);
                }

                while (!InputIpFile.eof())
                {
                    string Line;
                    InputIpFile >> Line;
                    if (Line == "")
                    {
                        continue;
                    }
                    Ips.push_back(Line);
                }

                while (!InputNameFile.eof())
                {
                    string Line{ "" };
                    getline(InputNameFile, Line);
                    if (Line == "")
                    {
                        continue;
                    }
                    Name.push_back(Line);
                }

                InputIdFile.close();
                InputIpFile.close();
                InputNameFile.close();

                if (Ids.size() != Ips.size() || Ids.size() != Name.size())
                {
                    cout << "Something has gone wrong with the data as ip entries, id entries and name entries amount should be same\n";
                    cout << "It is not advised to proceed further as it will mess up the database\n\n";
                    cout << "Total ids: " << Ids.size() << endl;
                    cout << "Total ips: " << Ips.size() << endl;
                    cout << "Total names: " << Name.size() << endl;
                    cout << "Program will now be terminated\n";
                    system("PAUSE");
                    return 0;
                }

                //==========================================================================================================================================================
                // 
                //==========================================================================================================================================================

                vector<int>CheckPoint(4);
                int CheckPointIterator{ 0 };

                try
                {
                    PreparedStatement* PstmtInsert, * PstmtUpdateName;

                    CreateCheckPoint(CheckPoint, Ids.size());

                    //inserting ids
                    pstmt = con->prepareStatement("SELECT * FROM id64 WHERE id = ?");
                    PstmtInsert = con->prepareStatement("INSERT INTO id64 (id, id_name) VALUES (? , ?)");
                    PstmtUpdateName = con->prepareStatement("UPDATE id64 SET id_name = ? WHERE id = ?");

                    cout << "\nInserting " << Ids.size() << " ids\n";

                    for (auto i{ 0 }; i < Ids.size(); i++)
                    {
                        //cout << "accesssing index: " << i << endl;
                        pstmt->setString(1, Ids.at(i));

                        res = pstmt->executeQuery();

                        if (res->next() == false)
                        {
                            PstmtInsert->setString(1, Ids.at(i));
                            PstmtInsert->setString(2, Name.at(i));
                            PstmtInsert->execute();
                        }
                        else 
                        {
                            if (res->getString("id_name") != Name.at(i))
                            {
                                PstmtUpdateName->setString(1, Name.at(i));
                                PstmtUpdateName->setString(2, Ids.at(i));
                                PstmtUpdateName->execute();
                            }
                        }

                        if (CheckPoint.at(CheckPointIterator) == i)
                        {
                            cout << "Done with " << CheckPoint.at(CheckPointIterator) << " ids out of " << Ids.size() << " ids\n";
                            CheckPointIterator++;
                        }
                    }
                    cout << "Done with " << Ids.size() << " ids out of " << Ids.size() << " ids\n";
                    cout << "\nAll the ids have been inserted\n";

                    //inserting ips
                    pstmt = con->prepareStatement("SELECT * FROM ips WHERE ip = ?");
                    PstmtInsert = con->prepareStatement("INSERT INTO ips (ip) VALUES (?)");

                    CreateCheckPoint(CheckPoint, Ips.size());
                    CheckPointIterator = 0;

                    cout << "\nInserting " << Ips.size() << " ips\n";

                    for (auto i{ 0 }; i < Ips.size(); i++)
                    {
                        //cout << "accesssing index: " << i << endl;
                        pstmt->setString(1, Ips.at(i));

                        res = pstmt->executeQuery();

                        if (res->next() == false)
                        {
                            PstmtInsert->setString(1, Ips.at(i));
                            PstmtInsert->execute();
                        }

                        if (CheckPoint.at(CheckPointIterator) == i)
                        {
                            cout << "Done with " << CheckPoint.at(CheckPointIterator) << " ips out of " << Ips.size() << " ips\n";
                            CheckPointIterator++;
                        }
                    }
                    cout << "Done with " << Ips.size() << " ips out of " << Ips.size() << " ips\n";
                    cout << "\nAll the ips have been inserted\n";
                    delete PstmtInsert;
                    delete PstmtUpdateName;
                }
                catch (SQLException& e)
                {
                    cout << "Error msg: " << e.what() << endl;
                }

                //inserting values in the relation table
                try
                {
                    PreparedStatement* PstmtInsert;

                    pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE id = ? AND ips = ?");
                    PstmtInsert = con->prepareStatement("INSERT INTO relation_id64_ip (id, ips) VALUES (?,?)");

                    CheckPointIterator = 0;

                    cout << "\nCreating links between " << Ids.size() << " ids and " << Ips.size() << " ips\n";

                    for (auto i{ 0 }; i < Ids.size(); i++)
                    {
                        pstmt->setString(1, Ids.at(i));
                        pstmt->setString(2, Ips.at(i));

                        res = pstmt->executeQuery();

                        if (res->next() == false)
                        {
                            PstmtInsert->setString(1, Ids.at(i));
                            PstmtInsert->setString(2, Ips.at(i));
                            PstmtInsert->execute();
                        }

                        if (CheckPoint.at(CheckPointIterator) == i)
                        {
                            cout << "Done creating " << CheckPoint.at(CheckPointIterator) << " links out of " << Ips.size() << " links\n";
                            CheckPointIterator++;
                        }
                    }
                    cout << "Done creating " << Ips.size() << " links out of " << Ips.size() << " links\n";
                    cout << "\nAll the links have been created\n";
                    delete PstmtInsert;
                }
                catch (SQLException& e)
                {
                    cout << "Error msg: " << e.what() << endl;
                }
                break;
            }
            case 'B':
            {
                char S_Choice{ ' ' };
                char SearchType{ ' ' };
                char DisplayChoice{ ' ' };
                bool DisplayIp{ true };
            
            SearchTypeSelection:

                cout << "\n(N) for normal search\n";
                cout << "(W) for web search\n";
                cout << "Your choice: ";

                SearchType = _getch();

                cout << SearchType << endl;

                SearchType = toupper(SearchType);

                if (SearchType != 'N' && SearchType != 'W')
                {
                    cout << "\nInvalid option selected, Try again\n\n";
                    goto SearchTypeSelection;
                }

            DisplayChoiceSelection:
                cout << "\nDo you want to see all the ip links as well (Y/N)\n";
                cout << "Your choice: ";

                DisplayChoice = _getch();

                cout << DisplayChoice << endl;

                DisplayChoice = toupper(DisplayChoice);

                if (DisplayChoice == 'N')
                {
                    DisplayIp = false;
                }
                else
                {
                    if (DisplayChoice != 'Y' && DisplayChoice != 'N')
                    {
                        cout << "\nInvalid option selected, Try again\n\n";
                        goto DisplayChoiceSelection;
                    }
                }

                do
                {
                    bool ResultFound{ false };

                    SearchMenu(S_Choice);
                    
                    switch (S_Choice)
                    {
                        case 'A':
                        {
                            string S_Id{ "" };

                            if (SearchType == 'W')
                            {
                                stack<string>IdDone, IpDone;
                                vector<string>AltIp, AltId, AltIdDone, AltIpDone;

                                cout << "Enter the id that you want to search with: ";
                                cin >> S_Id;

                                pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE id = ?");
                                pstmt->setString(1, S_Id);

                                res = pstmt->executeQuery();

                                cout << "\nThe results against your searched id " << S_Id << " are:\n\n";

                                while (res->next())
                                {
                                    cout << "IP: " << res->getString("ips") << endl;
                                    AltIp.push_back(res->getString("ips"));
                                    ResultFound = true;
                                }

                                if (ResultFound)
                                {
                                    PreparedStatement* pstmt1;

                                    pstmt1 = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE ips = ?");

                                SearchAgain:

                                    for (auto i{ 0 }; i < AltIp.size(); i++)
                                    {
                                        pstmt1->setString(1, AltIp.at(i));

                                        res = pstmt1->executeQuery();

                                        while (res->next())
                                        {
                                            if (res->getString("id") != S_Id)
                                            {
                                                AltId.push_back(res->getString("id"));
                                            }
                                        }
                                        IpDone.push(AltIp.at(i));
                                    }

                                    if (!AltId.empty())
                                    {
                                        VectorDuplicationRemoval(AltId);
                                    }

                                    if (!AltIdDone.empty())
                                    {
                                        TwoVectorDuplicationComparison(AltIdDone, AltId);
                                    }

                                    //simply adds the already checked ips to the done list and avoids duplicate entries in the done list at the same time as well
                                    if (AltIpDone.empty())
                                    {
                                        AltIpDone.push_back(IpDone.top());
                                        IpDone.pop();

                                        if (!IpDone.empty())
                                        {
                                            StackToVector(IpDone, AltIpDone);
                                        }
                                    }
                                    else
                                    {
                                        if (!IpDone.empty())
                                        {
                                            StackToVector(IpDone, AltIpDone);
                                        }
                                    }

                                    if (!AltId.empty())
                                    {
                                        AltIp.clear();
                                        AltIp.shrink_to_fit();

                                        for (auto i{ 0 }; i < AltId.size(); i++)
                                        {
                                            pstmt->setString(1, AltId.at(i));

                                            res = pstmt->executeQuery();

                                            while (res->next())
                                            {
                                                AltIp.push_back(res->getString("ips"));
                                            }

                                            IdDone.push(AltId.at(i));
                                        }

                                        if (!AltIp.empty())
                                        {
                                            VectorDuplicationRemoval(AltIp);
                                        }

                                        if (!AltIpDone.empty())
                                        {
                                            TwoVectorDuplicationComparison(AltIpDone, AltIp);
                                        }

                                        //simply adds the already checked ids to the done list and avoids duplicate entries in the done list at the same time as well
                                        if (AltIdDone.empty())
                                        {
                                            AltIdDone.push_back(IdDone.top());
                                            IdDone.pop();

                                            if (!IdDone.empty())
                                            {
                                                StackToVector(IdDone, AltIdDone);
                                            }
                                        }
                                        else
                                        {
                                            if (!IdDone.empty())
                                            {
                                                StackToVector(IdDone, AltIdDone);
                                            }
                                        }

                                        if (!AltIp.empty())
                                        {
                                            AltId.clear();
                                            AltId.shrink_to_fit();
                                            goto SearchAgain;
                                        }
                                    }

                                    PreparedStatement* PstmtGetName;

                                    PstmtGetName = con->prepareStatement("SELECT id_name FROM id64 WHERE id = ?");

                                    cout << "\nPossible alts:\n\n";
                                    for (auto i{ 0 }; i < AltIdDone.size(); i++)
                                    {
                                        PstmtGetName->setString(1, AltIdDone.at(i));
                                        res = PstmtGetName->executeQuery();
                                        res->next();
                                        cout << "Alt id: " << AltIdDone.at(i) << "\tname: "<<res->getString("id_name")<<"\n";
                                    }

                                    if (DisplayIp)
                                    {
                                        cout << "\nIps found via alt ids:\n\n";
                                        for (auto i{ 0 }; i < AltIpDone.size(); i++)
                                        {
                                            cout << "IP: " << AltIpDone.at(i) << endl;
                                        }
                                    }

                                    delete pstmt1;
                                    delete PstmtGetName;
                                }
                                else
                                {
                                    cout << "\nNo result found against your entered ip: " << S_Id << "\n";
                                }

                            }
                            else
                            {
                                vector<string>IpReceived;

                                cout << "Enter the id that you want to search with: ";
                                cin >> S_Id;

                                pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE id = ?");
                                pstmt->setString(1, S_Id);

                                res = pstmt->executeQuery();

                                cout << "\nThe ips registered against your searched id " << S_Id << " are:\n\n";

                                while (res->next())
                                {
                                    cout << "IP: " << res->getString("ips") << endl;
                                    IpReceived.push_back(res->getString("ips"));
                                }

                                pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE ips = ?");

                                cout << "\nIds linked with registered ips against your search id " << S_Id << " are:\n\n";

                                for (auto i{ 0 }; i < IpReceived.size(); i++)
                                {
                                    pstmt->setString(1, IpReceived.at(i));
                                    res = pstmt->executeQuery();

                                    while (res->next())
                                    {
                                        if (res->getString("id") != S_Id)
                                        {
                                            cout << "Alt id: " << res->getString("id") << endl;
                                        }
                                    }
                                }
                            }

                            break;
                        }
                        case 'B':
                        {
                            string S_Ip{ "" };

                            if (SearchType == 'W')
                            {
                                stack<string>IdDone, IpDone;
                                vector<string>AltIp, AltId, AltIdDone, AltIpDone;

                                cout << "Enter the ip that you want to search with: ";
                                cin >> S_Ip;

                                pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE ips = ?");
                                pstmt->setString(1, S_Ip);

                                res = pstmt->executeQuery();

                                cout << "\nThe results against your searched ip " << S_Ip << " are:\n\n";

                                while (res->next()) {
                                    cout << "ID: " << res->getString("id") << endl;
                                    AltId.push_back(res->getString("id"));
                                    ResultFound = true;
                                }

                                if (ResultFound)
                                {
                                    PreparedStatement* pstmt1;

                                    pstmt1 = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE id = ?");

                                SearchAgainIP:

                                    for (auto i{ 0 }; i < AltId.size(); i++)
                                    {
                                        pstmt1->setString(1, AltId.at(i));

                                        res = pstmt1->executeQuery();

                                        while (res->next())
                                        {
                                            if (res->getString("ips") != S_Ip)
                                            {
                                                AltIp.push_back(res->getString("ips"));
                                            }
                                        }
                                        IdDone.push(AltId.at(i));
                                    }

                                    if (!AltIp.empty())
                                    {
                                        VectorDuplicationRemoval(AltIp);
                                    }

                                    if (!AltIpDone.empty())
                                    {
                                        TwoVectorDuplicationComparison(AltIpDone, AltIp);
                                    }

                                    //simply adds the already checked ids to the done list and avoids duplicate entries in the done list at the same time as well
                                    if (AltIdDone.empty())
                                    {
                                        AltIdDone.push_back(IdDone.top());
                                        IdDone.pop();

                                        if (!IdDone.empty())
                                        {
                                            StackToVector(IdDone, AltIdDone);
                                        }
                                    }
                                    else
                                    {
                                        if (!IdDone.empty())
                                        {
                                            StackToVector(IdDone, AltIdDone);
                                        }
                                    }

                                    if (!AltIp.empty())
                                    {
                                        AltId.clear();
                                        AltId.shrink_to_fit();

                                        for (auto i{ 0 }; i < AltIp.size(); i++)
                                        {
                                            pstmt->setString(1, AltIp.at(i));

                                            res = pstmt->executeQuery();

                                            while (res->next())
                                            {
                                                AltId.push_back(res->getString("id"));
                                            }

                                            IpDone.push(AltIp.at(i));
                                        }

                                        if (!AltId.empty())
                                        {
                                            VectorDuplicationRemoval(AltId);
                                        }

                                        if (!AltIdDone.empty())
                                        {
                                            TwoVectorDuplicationComparison(AltIdDone, AltId);
                                        }

                                        //simply adds the already checked ips to the done list and avoids duplicate entries in the done list at the same time as well
                                        if (AltIpDone.empty())
                                        {
                                            AltIpDone.push_back(IpDone.top());
                                            IpDone.pop();

                                            if (!IpDone.empty())
                                            {
                                                StackToVector(IpDone, AltIpDone);
                                            }
                                        }
                                        else
                                        {
                                            if (!IpDone.empty())
                                            {
                                                StackToVector(IpDone, AltIpDone);
                                            }
                                        }

                                        if (!AltId.empty())
                                        {
                                            AltIp.clear();
                                            AltIp.shrink_to_fit();
                                            goto SearchAgainIP;
                                        }
                                    }

                                    PreparedStatement* PstmtGetName;

                                    PstmtGetName = con->prepareStatement("SELECT id_name FROM id64 WHERE id = ?");

                                    cout << "\nPossible alts:\n\n";
                                    for (auto i{ 0 }; i < AltIdDone.size(); i++)
                                    {
                                        PstmtGetName->setString(1, AltIdDone.at(i));
                                        res = PstmtGetName->executeQuery();
                                        res->next();
                                        cout << "Alt id: " << AltIdDone.at(i) << "\tname: " << res->getString("id_name") << "\n";
                                        
                                    }

                                    if (DisplayIp)
                                    {
                                        cout << "\nIps found via alt ids:\n\n";
                                        for (auto i{ 0 }; i < AltIpDone.size(); i++)
                                        {
                                            cout << "IP: " << AltIpDone.at(i) << endl;
                                        }
                                    }

                                    delete pstmt1;
                                    delete PstmtGetName;
                                }
                                else
                                {
                                    cout << "\nNo result found against your entered ip: " << S_Ip << "\n";
                                }

                            }
                            else
                            {
                                cout << "Enter the ip that you want to search with: ";
                                cin >> S_Ip;

                                pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE ips = ?");
                                pstmt->setString(1, S_Ip);

                                res = pstmt->executeQuery();

                                cout << "\nThe results against your searched ip " << S_Ip << " are:\n\n";

                                while (res->next()) {
                                    cout << "ID: " << res->getString("id") << endl;
                                }
                            }

                            break;
                        }
                        case 'E':
                        {
                            cout << "Hope you caught that scumbag :)\n";

                            break;
                        }
                        default:
                        {
                            cout << "\nInvalid choice, Try again\n\n";

                            break;
                        }
                    }
                } while (S_Choice != 'E');

                break;
            }
            case 'C':
            {
                system("CLS");

                break;
            }
            case 'D':
            {
                char DeleteChoice{ ' ' };

                cout << "\nCAUTION\n" << "This will delete the entire database along with all the data in it and this action cannot be undone\n";
                cout << "Do you wish to continue (Y/N): ";
                DeleteChoice = _getch();

                cout << "\nYour choice: " << DeleteChoice << "\n";

                DeleteChoice = toupper(DeleteChoice);

                if (DeleteChoice == 'Y')
                {
                    try
                    {
                        stmt = con->createStatement();
                        stmt->execute("DROP DATABASE player_db");
                        cout << "\nDatabase has been successfully deleted\n\n";

                        cout << "Would you like to restart the program (Y/N): ";

                        DeleteChoice = _getch();

                        cout << "\nYour choice: " << DeleteChoice << "\n";

                        DeleteChoice = toupper(DeleteChoice);

                        if (DeleteChoice == 'Y')
                        {
                            goto AttemptToCreateDatabase;
                        }
                        else
                        {
                            cout << "Goodbye\n";

                            return 0;
                        }
                    }
                    catch (SQLException&e)
                    {
                        cout << e.what() << "\n";
                        system("PAUSE");
                    }
                    
                }

                break;
            }
            case 'E':
            {
                try
                {
                    //for ids
                    pstmt = con->prepareStatement("SELECT COUNT(*) FROM id64");

                    res = pstmt->executeQuery();

                    if (res->next())
                    {
                        cout << "\nThe total number of ids in the database are: " << res->getInt(1) << "\n";
                    }

                    //for ips
                    pstmt = con->prepareStatement("SELECT COUNT(*) FROM ips");

                    res = pstmt->executeQuery();

                    if (res->next())
                    {
                        cout << "\nThe total number of ips in the database are: " << res->getInt(1) << "\n";
                    }
                    
                    //for links
                    pstmt = con->prepareStatement("SELECT COUNT(*) FROM relation_id64_ip");

                    res = pstmt->executeQuery();

                    if (res->next())
                    {
                        cout << "\nThe total number of links between ids and ips in the database are: " << res->getInt(1) << "\n";
                    }
                    
                }
                catch (SQLException& e)
                {
                    cout << e.what() << "\n";
                }

                break;
            }
            case 'F':
            {
                pstmt = con->prepareStatement("SELECT id, ips FROM relation_id64_ip WHERE link_date = CURRENT_DATE()-1");
                res = pstmt->executeQuery();

                while (res->next())
                {
                    cout << "Id: " << res->getString("id") << "\n";
                    cout << "Ip: " << res->getString("ips") << "\n";
                }
                
                //cout << "\nTo be implemented :)\n";

                break;
            }
            case 'G':
            {
                cout << "\nTo be implemented :)\n";

                break;
            }
            case 'H':
            {
                cout << "\nTo be implemented :)\n";

                break;
            }
            case 'Q':
            {
                cout << "Goodbye\n";

                break;
            }
            //case only for testing purposes
            case 'X':
            {
                try
                {
                    pstmt = con->prepareStatement("SELECT * FROM relation_id64_ip WHERE id = ? AND ips = ?");
                    pstmt->setString(1, "76561198017310413");
                    pstmt->setString(2, "178.220.181.18");

                    res = pstmt->executeQuery();

                    //cout << res->next() << endl;

                    if (res->next())                                            //correct way to check if something exists this also makes the res->next() go to the next position which can lead to incorrect results so be careful
                        //if (res->getString("id") == "" && res->getString("ips") == "")    //incorrect way to check if something exists
                    {
                        cout << "hehe\n";
                    }
                    while (res->next()) {
                        cout << "ID: " << res->getString("id") << ", IP: " << res->getString("ips") << endl;
                    }
                }
                catch (SQLException& e)
                {
                    cout << "Error msg: " << e.what() << endl;
                }

                break;
            }
            default:
            {
                cout << "\nInvalid option, Try again\n\n";

                break;
            }
        }
    } while (Choice != 'Q');


    system("PAUSE");

    delete con;
    delete stmt;
    delete pstmt;
    delete res;

    return 0;
}