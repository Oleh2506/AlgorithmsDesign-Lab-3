#include "DenseIndexDatabase.h"

const string DenseIndexDatabase::indexAreaFileName = "index.txt";
const string DenseIndexDatabase::mainDatabaseFileName = "main.txt";
const string DenseIndexDatabase::overflowAreaFileName = "overflow.txt";

void DenseIndexDatabase::getIndexFromFile()
{
	vector<vector<string>> vec(numOfBlocks_);
	ifstream inFile(DenseIndexDatabase::indexAreaFileName);
	string temp;
	int block = 0;
	while (inFile >> temp) {
		if (temp[0] == blockDelimiter) {
			block++;
		}
		else {
			vec[block].push_back(temp);
		}
	}
	inFile.close();
	indexArea_ = vec;
}

void DenseIndexDatabase::getMainFromFile()
{
	vector<string> vec;
	ifstream inFile(DenseIndexDatabase::mainDatabaseFileName);
	string temp;
	while (inFile >> temp) {
		vec.push_back(temp);
	}
	inFile.close();
	mainDatabase_ = vec;
}

void DenseIndexDatabase::getOverflowFromFile()
{
	vector<string> vec;
	ifstream inFile(DenseIndexDatabase::overflowAreaFileName);
	string temp;
	while (inFile >> temp) {
		vec.push_back(temp);
	}
	inFile.close();
	overflowArea_ = vec;
}

void DenseIndexDatabase::createMainFile(int numberOfRecords)
{
	srand(time(0));
	ofstream outFile(DenseIndexDatabase::mainDatabaseFileName);
	for (int i = 0; i < numberOfRecords; i++) {
		string value = "";
		for (int j = 0; j < 10; j++) {
			value += char(65 + rand() % 25);
		}
		outFile << i << inRecordDelimiter << value << inRecordDelimiter << 1 << endl;
	}
	outFile.close();
}

void DenseIndexDatabase::createIndexFile()
{
	string temp;
	vector<vector<string>> tempIndex(numOfBlocks_);
	for (int i = 0; i < mainDatabase_.size(); ++i) {
		temp = mainDatabase_[i];
		int delimPos = temp.find(inRecordDelimiter);
		int key = stoi(temp.substr(0, delimPos));
		temp.erase(0, delimPos + 1);
		delimPos = temp.find(inRecordDelimiter);
		string value = temp.substr(0, delimPos);
		temp.erase(0, delimPos + 1);
		bool isNotDeleted = stoi(temp);
		int blockToPaste = key % numOfBlocks_;
		if (tempIndex[blockToPaste].size() < numberInBlock_) {
			tempIndex[blockToPaste].push_back(to_string(key) + inRecordDelimiter + to_string(i));
		}
		else {
			overflowArea_.push_back(to_string(key) + inRecordDelimiter + to_string(i));
		}
	}
	indexArea_ = tempIndex;
	indexUpdate();
	overflowUpdate();
}

void DenseIndexDatabase::mainUpdate() {
	ofstream outFile(DenseIndexDatabase::mainDatabaseFileName);
	for (int i = 0; i < mainDatabase_.size(); i++) {
		outFile << mainDatabase_[i] << endl;
	}
	outFile.close();
}
void DenseIndexDatabase::indexUpdate()
{
	ofstream outFile(DenseIndexDatabase::indexAreaFileName);
	for (int i = 0; i < indexArea_.size(); i++) {
		for (int j = 0; j < indexArea_[i].size(); j++) {
			outFile << indexArea_[i][j] << endl;
		}
		outFile << blockDelimiter << endl;
	}
	outFile.close();
}
void DenseIndexDatabase::overflowUpdate()
{
	ofstream outFile(DenseIndexDatabase::overflowAreaFileName);
	for (int i = 0; i < overflowArea_.size(); i++) {
		outFile << overflowArea_[i] << endl;
	}
	outFile.close();
}

int getKey(string temp) {
	int delimPos = temp.find(DenseIndexDatabase::inRecordDelimiter);
	int key = stoi(temp.substr(0, delimPos + 1));
	return key;
}

int getMainIndex(string temp) {
	int delimPos = temp.find(DenseIndexDatabase::inRecordDelimiter);
	int index = stoi(temp.substr(delimPos + 1, temp.length() - delimPos));
	return index;
}

void DenseIndexDatabase::addRecord() {
	cout << "Please, enter the value of the record you want to add: ";
	string value;
	cin >> value;
	if (indexArea_[currRecordId_ % numOfBlocks_].size() < numberInBlock_) {
		indexArea_[currRecordId_ % numOfBlocks_].push_back(to_string(currRecordId_) + ',' + to_string(mainDatabase_.size()));
		indexUpdate();
	}
	else {
		overflowArea_.push_back(to_string(currRecordId_) + inRecordDelimiter + to_string(mainDatabase_.size()));
		overflowUpdate();
	}

	mainDatabase_.push_back(to_string(currRecordId_) + inRecordDelimiter + value + inRecordDelimiter + "1");
	mainUpdate();
	cout << "The record has been successfully added to the database!\n";
	currRecordId_++;
}

int doBinarySearch(vector<string> block, int key, int& comparisonNum) {
	if (block.size() == 0) {
		return -1;
	}
	else {
		int lo = 0, hi = block.size() - 1;

		comparisonNum++;
		while (hi - lo > 1) {
			int mid = (hi + lo) / 2;
			comparisonNum++;
			if (getKey(block[mid]) < key) {
				lo = mid + 1;
			}
			else {
				hi = mid;
			}

			comparisonNum++;
		}

		comparisonNum++;
		if (getKey(block[lo]) == key) {
			return lo;
		}
		else if (getKey(block[hi]) == key) {
			comparisonNum++;
			return hi;
		}
		else {
			return -1;
		}
	}
}

void DenseIndexDatabase::deleteRecordByKey() {
	cout << "Please, enter the key of the record you want to delete: ";
	int key;
	cin >> key;
	if (key < 0) {
		cout << "Unfortunately, the database does not contain the record with entered key\n";
	}
	else {
		vector<string> currIndexBlock = indexArea_[key % numOfBlocks_];
		int indexComparisonsNumber = 0;
		int inBlockIndex = doBinarySearch(currIndexBlock, key, indexComparisonsNumber);
		if (inBlockIndex == -1) {
			int overflowComparisonsNumber = 0;
			int indexInOverflow = doBinarySearch(overflowArea_, key, overflowComparisonsNumber);
			if (indexInOverflow == -1) {
				cout << "Unfortunately, the database does not contain the record with entered key\n";
			}
			else {
				string temp = mainDatabase_[getMainIndex(overflowArea_[indexInOverflow])];
				temp[temp.length() - 1] = '0';
				mainDatabase_[getMainIndex(overflowArea_[indexInOverflow])] = temp;
				mainUpdate();
				vector<string> newOverflow;
				for (int i = 0; i < overflowArea_.size(); i++) {
					if (getKey(overflowArea_[i]) != key) {
						newOverflow.push_back(overflowArea_[i]);
					}
				}
				overflowArea_ = newOverflow;
				overflowUpdate();
				cout << "The record has been successfully deleted!\n";
				cout << "The number of comparisons is: " << indexComparisonsNumber + overflowComparisonsNumber << "\n";
			}
		}
		else {
			vector<vector<string>> newIndex(numOfBlocks_);
			for (int i = 0; i < indexArea_.size(); i++) {
				for (int j = 0; j < indexArea_[i].size(); j++) {
					if (getKey(indexArea_[i][j]) != key) {
						newIndex[i].push_back(indexArea_[i][j]);
					}
				}
			}
			indexArea_ = newIndex;
			indexUpdate();
			string temp = mainDatabase_[getMainIndex(currIndexBlock[inBlockIndex])];
			temp[temp.length() - 1] = '0';
			mainDatabase_[getMainIndex(currIndexBlock[inBlockIndex])] = temp;
			mainUpdate();
			cout << "The record has been successfully deleted!\n";
			cout << "The number of comparisons is: " << indexComparisonsNumber << "\n";
		}
	}
}

void DenseIndexDatabase::findRecordByKey() {
	cout << "Please, enter the key of the record you want to find: ";
	int key;
	cin >> key;
	if (key < 0) {
		cout << "Unfortunately, the database does not contain the record with entered key\n";
	}
	else {
		vector<string> currIndexBlock = indexArea_[key % numOfBlocks_];
		int indexComparisonsNumber = 0;
		int inBlockIndex = doBinarySearch(currIndexBlock, key, indexComparisonsNumber);
		if (inBlockIndex == -1) {
			int overflowComparisonsNumber = 0;
			int inOverflowIndex = doBinarySearch(overflowArea_, key, overflowComparisonsNumber);
			if (inOverflowIndex == -1) {
				cout << "Unfortunately, the database does not contain the record with entered key\n";
			}
			else {
				string temp = mainDatabase_[getMainIndex(overflowArea_[inOverflowIndex])];
				temp.erase(0, temp.find(inRecordDelimiter) + 1);
				temp.erase(temp.find(inRecordDelimiter), temp.length() - temp.find(inRecordDelimiter));
				cout << "The record has been successfully found, its value is: " << temp << "\n";
				cout << "The number of comparisons is: " << indexComparisonsNumber + overflowComparisonsNumber << "\n";
			}
		}
		else {
			string temp = mainDatabase_[getMainIndex(currIndexBlock[inBlockIndex])];
			temp.erase(0, temp.find(inRecordDelimiter) + 1);
			temp.erase(temp.find(inRecordDelimiter), temp.length() - temp.find(inRecordDelimiter));
			cout << "The record has been successfully found, its value is: " << temp << "\n";
			cout << "The number of comparisons is: " << indexComparisonsNumber << "\n";
		}
	}
}

void DenseIndexDatabase::updateRecordByKey()
{
	cout << "Please, enter the key of the record you want to update: ";
	int key;
	cin >> key;
	cout << "Enter the new value for the selected record: ";
	string val = "";
	cin >> val;
	if (key < 0) {
		cout << "Unfortunately, the database does not contain the record with entered key\n";
	}
	else {
		vector<string> blockToFind = indexArea_[key % numOfBlocks_];
		int indexComparisonsNumber = 0;
		int inBlockIndex = doBinarySearch(blockToFind, key, indexComparisonsNumber);
		if (inBlockIndex == -1) {
			int overflowComparisonsNumber = 0;
			int inOverflowIndex = doBinarySearch(overflowArea_, key, overflowComparisonsNumber);
			if (inOverflowIndex == -1) {
				cout << "Unfortunately, the database does not contain the record with entered key\n";
			}
			else {
				string newStr = to_string(getMainIndex(overflowArea_[inOverflowIndex])) + inRecordDelimiter + val + inRecordDelimiter + "1";
				mainDatabase_[getMainIndex(overflowArea_[inOverflowIndex])] = newStr;
				mainUpdate();
				cout << "The record has been successfully updated!\n";
				cout << "The number of comparisons is: " << indexComparisonsNumber + overflowComparisonsNumber << "\n";
			}
		}
		else {
			string newStr = to_string(getMainIndex(blockToFind[inBlockIndex])) + inRecordDelimiter + val + inRecordDelimiter + "1";
			mainDatabase_[getMainIndex(blockToFind[inBlockIndex])] = newStr;
			mainUpdate();
			cout << "The record has been successfully updated!\n";
			cout << "The number of comparisons is: " << indexComparisonsNumber << "\n";
		}
	}
}

void DenseIndexDatabase::runUI()
{
	int commandId;
	int continueCommandId = 1;
	while (continueCommandId) {

		cout << "Please, select command:\t1 - Add record;\n\t\t\t2 - Delete record by key;\n\t\t\t3 - Find record by key;\n\t\t\t4 - Update record by key.\nYour input: ";
		cin >> commandId;
		switch (commandId) {
		case 1:
			addRecord();
			break;
		case 2:
			deleteRecordByKey();
			break;
		case 3:
			findRecordByKey();
			break;
		case 4:
			updateRecordByKey();
			break;
		default:
			cout << "Error: Unknown command!\n";
		}

		cout << "\nContinue?\t\t1 - Yes;\n\t\t\t0 - No.\nYour input: ";
		cin >> continueCommandId;
		cout << "\n";
	}
}
