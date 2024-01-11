#include "testutils.h"

#include <fstream>

bool are_files_identical(const std::string& file1_path,
                         const std::string& file2_path) {
  std::ifstream file1(file1_path, std::ifstream::binary);
  std::ifstream file2(file2_path, std::ifstream::binary);

  if (!file1.is_open() || !file2.is_open()) {
    return false;
  }

  std::istreambuf_iterator<char> file1_iter(file1), file1_end;
  std::istreambuf_iterator<char> file2_iter(file2), file2_end;

  return std::equal(file1_iter, file1_end, file2_iter, file2_end);
}

int generate_file(const std::string& path, size_t size) {
  std::ofstream out(path, std::ofstream::binary | std::ofstream::trunc);
  if (!out.is_open()) {
    return -1;
  }
  for (size_t i = 0; i < size; i++) {
    out << (char)i;
  }
  return 0;
}
