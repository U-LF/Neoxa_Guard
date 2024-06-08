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
    cout << "(X) for other\n";
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