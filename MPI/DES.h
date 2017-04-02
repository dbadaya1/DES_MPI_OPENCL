#pragma once

#include <string>

std::string encrypt(char plaintext[8], int subKeys[17][48]);
std::string decrypt(char cipher[8], int subKeys[17][48]);
void generateSubKeys(int subKeys[17][48], bool forEncryption);
