#pragma once
#include "Libraries.h"

using namespace std;

void trim(string& str);
string ExtractName(string InputName);
void createInputFile(const string& filename);
vector<string> extractData(const string& filename);
void writeExtractedData(const vector<string>& data, const string& OutputFileName1, const string& OutputFileName2, const string& OutputFileName3);