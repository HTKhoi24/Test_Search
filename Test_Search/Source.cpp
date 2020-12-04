#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <filesystem>

using namespace std;

#define max 1000000000

double byteSize;
int totalFile = 0;

// Cau truc cho 1 dong
struct Line {
	wchar_t* val = (wchar_t*)malloc(byteSize * sizeof(wchar_t));
	double lineLen = 0;
	double countWord(wchar_t* val) {
		this->lineLen = wcslen(val);
		return lineLen;
	}
};

// Sap xep cac dong cua mang dong
void lineSort(Line runSizeArr[], int arrSize) {
	for (int i = 0; i < arrSize - 1; i++) {
		for (int j = i + 1; j < arrSize; j++) {
			bool compare = true;
			while (compare) {
				if (wcscmp(runSizeArr[i].val, runSizeArr[j].val) > 0) {
					swap(runSizeArr[i], runSizeArr[j]);
					compare = false;
				}
				else {
					compare = false;
				}
			}
		}
	}
}

// Merge 2 file 
void twoWayMerge(char* inFileName_1, char* inFileName_2, char* outFileName) {
	FILE* tmpFile_1 = fopen(strcat(inFileName_1, ".txt"), "r");
	FILE* tmpFile_2 = fopen(strcat(inFileName_2, ".txt"), "r");
	FILE* tmpFile_3 = fopen(strcat(outFileName, ".txt"), "w");
	Line* compareArr = new Line[2];
	int count = 0;
	
	//so sanh tung dong cua 2 file cho toi eof
	// neu moi bat dau co file trong thi gan toan bo file con lai vao outFile
	if (fgetws(compareArr[1].val, max, tmpFile_2) == NULL) {
		while (count != 1) {
			if (fgetws(compareArr[0].val, max, tmpFile_1) == NULL) {
				break;
			}
			fputws(compareArr[0].val, tmpFile_3);
		}
	}else
	{
		fgetws(compareArr[0].val, max, tmpFile_1);
		while (count != 2) {
			if (wcscmp(compareArr[0].val, compareArr[1].val) < 0) {
				fputws(compareArr[0].val, tmpFile_3);
				if (fgetws(compareArr[0].val, max, tmpFile_1) == NULL) {
					count++;
					while (count != 2) {
						fputws(compareArr[1].val, tmpFile_3);
						if (fgetws(compareArr[1].val, max, tmpFile_2) == NULL) {
							count++;
						}
					}
				}
			}
			if (wcscmp(compareArr[0].val, compareArr[1].val) > 0) {
				fputws(compareArr[1].val, tmpFile_3);
				if (fgetws(compareArr[1].val, max, tmpFile_2) == NULL) {
					count++;
					while (count != 2) {
						fputws(compareArr[0].val, tmpFile_3);
						if (fgetws(compareArr[0].val, max, tmpFile_1) == NULL) {
							count++;
						}
					}
				}
			}
		}
	}
	fclose(tmpFile_1);
	fclose(tmpFile_2);
	fclose(tmpFile_3);
	totalFile++;
}

void updateTotalFile() {
	totalFile++;
	stringstream updateFile;
	char upName[100];
	updateFile << totalFile - 1;
	strcpy(upName, updateFile.str().c_str());
	FILE* tmpFile = fopen(strcat(upName, ".txt"), "w");
	fclose(tmpFile);
}

void mergeFile(char* outFileName) {
	int k = 0;
	int numInFile = totalFile;	// So file dau vao
	if (totalFile % 2 == 1) {
		updateTotalFile();
		numInFile++;
	}
	int runCount = numInFile / 2;	// So lan merge cua 1 lan chay

	while (numInFile >= 1) {
		stringstream temp_1;
		stringstream temp_2;
		stringstream temp_3;
		char tempName_1[100];
		char tempName_2[100];
		char tempName_3[100];
		temp_1 << k;
		strcpy(tempName_1, temp_1.str().c_str());
		temp_2 << k + 1;
		strcpy(tempName_2, temp_2.str().c_str());
		temp_3 << totalFile;
		strcpy(tempName_3, temp_3.str().c_str());
		if (numInFile == 2) {
			twoWayMerge(tempName_1, tempName_2, outFileName);
			break;
		}
		else
			twoWayMerge(tempName_1, tempName_2, tempName_3);
		k += 2;
		runCount--;
		if (runCount == 0) {
			if (totalFile % 2 == 1) {
				updateTotalFile();
			}
			numInFile = numInFile / 2;
			if(numInFile % 2 == 1) {
				numInFile++;
			}
			runCount = numInFile / 2;
		}
	}
}


// Khoi tao cac file cho external sort
void createInitFile(char* inFileName) {
	FILE* inFile = fopen(inFileName , "r");
	FILE* tmpFile;

	// Tao mang dong vua du bo nho de sap xep
	Line* runSizeArr = new Line[100];
	bool moreInput = true;
	bool overSize = false;
	int nextOutFile = 0;
	fpos_t pointLine;
	while (moreInput) {
		int i = 0;
		double lineSize = 0;
		if (feof(inFile)) {
			moreInput = false;
			break;
		}
		if(overSize == true){
			fsetpos(inFile, &pointLine);
			overSize = false;
		}
		// Lay input vua du bo nho
		while (lineSize < byteSize) {
			Line* tmpLine = new Line;
			fgetpos(inFile, &pointLine);	// luu vi tri truoc do cua file
			fgetws(tmpLine->val, byteSize, inFile);
			if ((tmpLine->countWord(tmpLine->val) + lineSize) > byteSize)	// kiem tra dong vua lay co vuot byteSize
			{
				overSize = true;
				break;
			}
			else
			{
				runSizeArr[i].val = tmpLine->val;
				lineSize += runSizeArr[i].countWord(runSizeArr[i].val);
				i++;
			}
		}

		lineSort(runSizeArr, i);

		// tao file tam de chua phan du lieu
		stringstream temp;
		temp << totalFile;
		char tempName[10];
		strcpy(tempName, temp.str().c_str());
		tmpFile = fopen(strcat(tempName, ".txt"), "w");

		// ghi cac dong trong mang vao file tam
		for (int a = 0; a < i; a++) {
			fputws(runSizeArr[a].val, tmpFile);
		}
		fclose(tmpFile);
		totalFile++;
	}
	fclose(inFile);
}

void validateFile(char* inFileName) {
	FILE* inFile;
	inFile = fopen(inFileName, "r");
	if (inFile == NULL) {
		cout << "Can't open file \n";
		exit(1);
	}
	fseek(inFile, 0, SEEK_END);
	if (ftell(inFile) == NULL) {
		cout << "This is an empty file \n";
		exit(1);
	}
	fseek(inFile, 0, SEEK_SET);
	fclose(inFile);
}

int main() {
	char inFileName[100] = "";
	char outFileName[100] = "";

	cout << "File need to be sorted: \n";	// Nhap ten file can sap xep
	cin >> inFileName;
	strcat(inFileName, ".txt");
	validateFile(inFileName);
	
	cout << "Sorted file name: \n";	// Nhap ten file sau khi sap xep
	cin >> outFileName;

	cout << "Usable memory (byte): \n";	// Nhap so byte bo nho co the su dung
	cin >> byteSize;
	byteSize = byteSize * 8 / 10;

	createInitFile(inFileName);
	mergeFile(outFileName);

	return 0;
}