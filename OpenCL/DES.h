#pragma once

#include <string>

std::string encrypt(const std::string &text);

std::string decrypt(const std::string &text);

void generateSubKeys(bool forEncryption);