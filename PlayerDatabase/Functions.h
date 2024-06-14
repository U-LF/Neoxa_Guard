#pragma once
#include "Libraries.h"

using namespace std;
using namespace sql;

//other functions
void MainMenu(char& Choice);
void SearchMenu(char& Choice);
void VectorDuplicationRemoval(vector<string>& InputVector);
void StackToVector(stack<string>& InputStack, vector<string>& InputVector);
void CreateCheckPoint(vector<int>& InputVector, int ValueToCreateFrom);

//This function simply compares the second vector against the first one and removes any values from the second vector if they already exist in the first one
void TwoVectorDuplicationComparison(vector<string> vec1, vector<string>& vec2);

//functions related to database operations
bool DataInsertUpdate(string IdFileName, string IpFileName, string NameFile, Connection* con, PreparedStatement* pstmt, ResultSet* res);