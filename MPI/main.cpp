
#include "DES.h"
#include <string>
#include <iostream>
#include "mpi.h"

using namespace std;

void ECB(bool encryption,string inputFile) {
	int rank, size;
	MPI_Init(nullptr, nullptr);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	char *input = nullptr;
	int length = 0;
	char *output = nullptr;
	int subKeys[17][48];
	if (rank == 0) {
		FILE* fp;
		fopen_s(&fp, inputFile.c_str(), "rb");
		int n;
		fseek(fp, 0, SEEK_END);
		n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int rem = n % 8;
		if (rem) {
			length = n + 8 - rem;
			input = new char[length];
			for (int i = n; i < length; i++) {
				input[i] = 0;
			}
		}
		else {
			length = n;
			input = new char[length];
		}
		fread(input, sizeof(char),n, fp);
		fclose(fp);
		output = new char[length];
		generateSubKeys(subKeys, encryption);
	}
	char inputBlock[8];
	MPI_Scatter(input, 8, MPI_CHAR, inputBlock, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(subKeys, 17 * 48, MPI_INT, 0, MPI_COMM_WORLD);

	char result[8];
	if (encryption) {
		encrypt(inputBlock, subKeys,result);
	}
	else {
		decrypt(inputBlock, subKeys,result);
	}
	MPI_Gather(result, 8, MPI_CHAR, output, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		if (!encryption) {
			for (int i = 0; i < 7; i++) {
				if (output[length - 1] == 0) {
					length--;
				}
			}
		}
		string filename;
		string extension = inputFile.substr(inputFile.find_last_of('.'));
		if (encryption) {
			filename = "files/encryption_result" + extension;
		}
		else {
			filename = "files/decryption_result" + extension;
		}
		FILE* fp;
		fopen_s(&fp, filename.c_str(), "wb");
		fwrite(output,sizeof(char), length, fp);
		fclose(fp);

	}

	MPI_Finalize();
}


int main(int argc, char *argv[]) {

	string result;
	string inputFile(argv[2]);
	string type(argv[1]);

	if (type == "e") {
		ECB(true,inputFile);
	}
	else {
		ECB(false,inputFile);
	}

	return 0;
}