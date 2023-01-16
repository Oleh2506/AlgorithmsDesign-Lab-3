#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>
#include <vector>
using namespace std;

class DenseIndexDatabase {
private:
	vector<vector<string>> indexArea_;
	vector<string> overflowArea_;
	vector<string> mainDatabase_;
	static const int numberInBlock_ = 100;
	static const int numOfBlocks_ = 100;
	int currRecordId_;

	void getIndexFromFile();
	void getMainFromFile();
	void getOverflowFromFile();
	void createMainFile(int numOfRecords);
	void createIndexFile();
	void addRecord();
	void deleteRecordByKey();
	void findRecordByKey();
	void updateRecordByKey();
	void mainUpdate();
	void indexUpdate();
	void overflowUpdate();

public:
	static const char blockDelimiter = '-';
	static const char inRecordDelimiter = ',';
	static const string indexAreaFileName;
	static const string overflowAreaFileName;
	static const string mainDatabaseFileName;

	void runUI();
	DenseIndexDatabase() {
		getIndexFromFile();
		getMainFromFile();
		getOverflowFromFile();
		currRecordId_ = mainDatabase_.size();
	}
	DenseIndexDatabase(int numOfRecords) {
		createMainFile(numOfRecords);
		getMainFromFile();
		currRecordId_ = mainDatabase_.size();
		createIndexFile();
		getOverflowFromFile();
		getIndexFromFile();
	}
};