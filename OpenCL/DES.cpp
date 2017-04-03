
void generateSubKeys(int subKeys[17][48], bool forEncryption) {

	const int key[] = {
		0,0,0,1,0,0,1,1,
		0,0,1,1,0,1,0,0,
		0,1,0,1,0,1,1,1,
		0,1,1,1,1,0,0,1,
		1,0,0,1,1,0,1,1,
		1,0,1,1,1,1,0,0,
		1,1,0,1,1,1,1,1,
		1,1,1,1,0,0,0,1
	};


	const int PC1[] = {
		57,49, 41,33, 25, 17, 9,
		1,58, 50,42, 34, 26,18,
		10, 2, 59,51, 43, 35,27,
		19,11,  3,60, 52, 44,36,
		63,55, 47,39, 31, 23,15,
		7,62, 54,46, 38, 30,22,
		14, 6, 61,53, 45, 37,29,
		21,13,  5,28, 20, 12, 4
	};

	const int PC2[]{
		14, 17,11, 24,  1, 5,
		3, 28,15,  6, 21,10,
		23, 19,12,  4, 26, 8,
		16,  7,27, 20, 13, 2,
		41, 52,31, 37, 47,55,
		30, 40,51, 45, 33,48,
		44, 49,39, 56, 34,53,
		46, 42,50, 36, 29,32
	};

	const int noOfLeftShifts[] = {
		1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
	};

	int C[17][28];
	int D[17][28];
	int CD[56];

	for (int i = 0; i < 28; i++) {
		C[0][i] = key[PC1[i] - 1];
	}
	for (int i = 28, k = 0; i < 56; i++, k++) {
		D[0][k] = key[PC1[i] - 1];
	}

	for (int round = 1; round <= 16; round++) {
		for (int i = 0, j = 0; i < 28; i++) {
			C[round][i] = C[round - 1][(i + noOfLeftShifts[round - 1]) % 28];
			D[round][i] = D[round - 1][(i + noOfLeftShifts[round - 1]) % 28];
		}

		for (int i = 0; i < 28; i++) {
			CD[i] = C[round][i];
		}
		for (int i = 28, k = 0; i < 56; i++, k++) {
			CD[i] = D[round][k];
		}

		int r;
		for (int i = 0; i < 48; i++) {
			r = forEncryption ? round : 17 - round;
			subKeys[r][i] = CD[PC2[i] - 1];
		}
	}

}


