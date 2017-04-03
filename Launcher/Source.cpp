
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[]) {

	if (argc < 4) {
		cout << "4 argumanets required";
		return 1;
	}
	string method(argv[1]);
	string input(argv[3]);
	string type(argv[2]);

	input = "files/" + input;

	string command;
	if (method == "MPI") {
		FILE* fp;
		fopen_s(&fp, input.c_str(), "r");
		int n;
		fseek(fp, 0, SEEK_END);
		n = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int rem = n % 8;
		if (rem) {
			n = n + 8 - rem;
		}

		command = "mpiexec -n " + to_string(n / 8) + " MPI " + type  + " " + input;
	}
	else {
		command = "OpenCL " + type  + " " + input;
	}
	cout << command<<"\n";
	system(command.c_str());

	return 0;
}