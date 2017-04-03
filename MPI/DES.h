#pragma once

void encrypt(char plaintext[8], int subKeys[17][48], char result[8]);
void decrypt(char cipher[8], int subKeys[17][48], char result[8]);
void generateSubKeys(int subKeys[17][48], bool forEncryption);
