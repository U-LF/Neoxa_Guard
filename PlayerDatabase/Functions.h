#pragma once
#include "Libraries.h"

using namespace std;

void MainMenu(char& Choice);
void SearchMenu(char& Choice);
void VectorDuplicationRemoval(vector<string>& InputVector);
void StackToVector(stack<string>& InputStack, vector<string>& InputVector);
void CreateCheckPoint(vector<int>& InputVector, int ValueToCreateFrom);

//This function simply compares the second vector against the first one and removes any values from the second vector if they already exist in the first one
void TwoVectorDuplicationComparison(vector<string> vec1, vector<string>& vec2);