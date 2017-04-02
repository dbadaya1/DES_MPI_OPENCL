
#include "DES.h"
#include <string>
#include <iostream>
#include "mpi.h"

using namespace std;

string ECB(bool encryption,string input) {
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
		fopen_s(&fp, input.c_str(), "r");
		int n;
		fseek(fp, 0, SEEK_END);
		n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int rem = n % 8;
		if (rem) {
			length = n + 8 - rem;
			text = new char[length];
			for (int i = n; i < length; i++) {
				text[i] = ' ';
			}
		}
		else {
			length = n;
			text = new char[length];
		}
		fread(text, n, 1, fp);
		cipher = new char[length];
		generateSubKeys(subKeys, encryption);

	}
	char textBlock[8];
	MPI_Scatter(text, 8, MPI_CHAR, textBlock, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(subKeys, 17 * 48, MPI_INT, 0, MPI_COMM_WORLD);

	string result;
	result = encryption ?  encrypt(textBlock, subKeys) : decrypt(textBlock, subKeys);
	MPI_Gather(result.c_str(), 8, MPI_CHAR, cipher, 8, MPI_CHAR, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		string filename;
		string extension = input.substr(input.find_last_of('.'));
		if (encryption) {
			filename = "encryption_result" + extension;
		}
		else {
			filename = "decryption_result" + extension;
		}
		FILE* fp;
		fopen_s(&fp, filename.c_str(), "w");
		fwrite(cipher, length, 1, fp);

	}

	MPI_Finalize();

	return string(cipher,length);

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