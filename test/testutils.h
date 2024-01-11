#pragma once
#include <string>

bool are_files_identical(const std::string& file1_path,
                         const std::string& file2_path);
int generate_file(const std::string& path, size_t size);
