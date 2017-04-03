
#include "DES.h"
#include <string>
#include <iostream>
#include "mpi.h"

using namespace std;

void ECB(bool encryption,string input) {
	int rank, size;
	MPI_Init(nullptr, nullptr);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	char *text = nullptr;
	int length = 0;
	char *cipher = nullptr;
	int subKeys[17][48];
	if (rank == 0) {
		FILE* fp;
		fopen_s(&fp, input.c_str(), "rb");
		int n;
		fseek(fp, 0, SEEK_END);
		n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int rem = n % 8;
		if (rem) {
			length = n + 8 - rem;
			text = new char[length];
			for (int i = n; i < length; i++) {
				text[i] = 0;
			}
		}
		else {
			length = n;
			text = new char[length];
		}
		fread(text, sizeof(char),n, fp);
		fclose(fp);
		cipher = new char[length];
		generateSubKeys(subKeys, encryption);
	}
	char textBlock[8];
	MPI_Scatter(text, 8, MPI_CHAR, textBlock, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(subKeys, 17 * 48, MPI_INT, 0, MPI_COMM_WORLD);

	char result[8];
	if (encryption) {
		encrypt(textBlock, subKeys,result);
	}
	else {
		decrypt(textBlock, subKeys,result);
	}
	MPI_Gather(result, 8, MPI_CHAR, cipher, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		if (!encryption) {
			while (cipher[length - 1] == 0) {
				length--;
			}
		}
		string filename;
		string extension = input.substr(input.find_last_of('.'));
		if (encryption) {
			filename = "files/encryption_result" + extension;
		}
		else {
			filename = "files/decryption_result" + extension;
		}
		FILE* fp;
		fopen_s(&fp, filename.c_str(), "wb");
		fwrite(cipher,sizeof(char), length, fp);
		fclose(fp);

	}

	MPI_Finalize();
}


int main(int argc, char *argv[]) {

	string result;
	string input(argv[2]);
	string type(argv[1]);

	if (type == "e") {
		ECB(true,input);
	}
	else {
		ECB(false,input);
	}

	return 0;
}