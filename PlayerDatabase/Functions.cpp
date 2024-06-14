#include "Functions.h"

using namespace std;

void MainMenu(char& Choice)
{
    cout << "\n*******************************************************************************\n";
    cout << "\t\t\tWELCOME TO NEOXA GUARD\n";
    cout << "*******************************************************************************\n\n";
    cout << "(A) to insert data from the file in database\n";
    cout << "(B) to search for links and alt accounts\n";
    cout << "(C) to clear screen\n";
    cout << "(D) to delete database\n";
    cout << "(E) to see the total number of entries in the database\n";
    cout << "(F) to see the new links in the last 24hrs\n";
    cout << "(G) to sync / update the database\n";
    cout << "(H) to extract all data from database\n";
    cout << "(Q) to quit\n";
    //cout << "(X) for other\n";
    cout << "\nYour choice: ";

    Choice = _getch();

    cout << Choice << endl;

    Choice = toupper(Choice);
}

void SearchMenu(char& Choice)
{
    cout << "\n(A) to search for ips registered against an id with id64\n";
    cout << "(B) to search for ids registered against an ip with ip\n";
    cout << "(E) to go back to main menu\n";
    cout << "\nYour choice: ";

    Choice = _getch();

    cout << Choice << endl;

    Choice = toupper(Choice);
}

void VectorDuplicationRemoval(vector<string>& InputVector)
{
    vector<string>DuplicationRemoveId;

    DuplicationRemoveId.push_back(InputVector.at(0));

    for (auto i{ 0 }; i < InputVector.size(); i++)
    {
        bool Run = true;
        for (auto j{ 0 }; j < DuplicationRemoveId.size(); j++)
        {
            if (InputVector.at(i) == DuplicationRemoveId.at(j))
            {
                Run = false;
                break;
            }
        }
        if (Run)
        {
            DuplicationRemoveId.push_back(InputVector.at(i));
        }
    }
    InputVector.clear();
    InputVector.shrink_to_fit();

    for (auto i{ 0 }; i < DuplicationRemoveId.size(); i++)
    {
        InputVector.push_back(DuplicationRemoveId.at(i));
    }
}

void StackToVector(stack<string>& InputStack, vector<string>& InputVector)
{
    while (!InputStack.empty())
    {
        bool Run = true;
        for (auto j{ 0 }; j < InputVector.size(); j++)
        {
            if (InputStack.top() == InputVector.at(j))
            {
                Run = false;
                break;
            }
        }

        if (Run)
        {
            InputVector.push_back(InputStack.top());
            InputStack.pop();
        }
        else
        {
            InputStack.pop();
        }
    }
}

//In this function the incoming vector will always have the size of 3 and we will create 4 checkpoints at those 3 indexes that's why constants have been used
void CreateCheckPoint(vector<int>& InputVector, int ValueToCreateFrom)
{
    InputVector.at(0) = ValueToCreateFrom * 0.25;   //25% of the value
    InputVector.at(1) = ValueToCreateFrom * 0.50;   //50% of the value
    InputVector.at(2) = ValueToCreateFrom * 0.75;   //75% of the value
    InputVector.at(3) = ValueToCreateFrom * 1;      //100% of the value
}

void TwoVectorDuplicationComparison(vector<string> vec1, vector<string>& vec2)
{
    for (auto it = vec2.begin(); it != vec2.end();) {
        bool found = false;
        for (size_t i = 0; i < vec1.size(); ++i) {
            if (*it == vec1[i]) {
                found = true;
                break;
            }
        }
        if (found) {
            it = vec2.erase(it);
        }
        else {
            ++it;
        }
    }
}

bool DataInsertUpdate(string IdFileName, string IpFileName, string NameFile, Connection* con, PreparedStatement* pstmt, ResultSet* res)
{
    ifstream InputIdFile(IdFileName);
    ifstream InputIpFile(IpFileName);
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
        return false;
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

    return true;
}