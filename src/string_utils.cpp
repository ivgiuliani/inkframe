#include <vector>

#include "string_utils.h"

std::vector<String> split_words(String text, const char separator) {
  std::vector<String> words{};
  text.trim();
  if (text.isEmpty()) {
    return words;
  }
  std::string ss = std::string(text.c_str());

  size_t pos = 0;
  while ((pos = ss.find(separator)) != std::string::npos) {
    const std::string word = ss.substr(0, pos);
    words.push_back(word.c_str());

    ss.erase(0, pos + 1);
  }
  words.push_back(ss.c_str());

  return words;
}

void ascii_extended_remap(String *orig) {
  for (uint16_t i = 0; i < orig->length(); i++) {
    if ((*orig)[i] >= 128) {
      (*orig)[i] = ASCII_EXTENDED_REMAP[(*orig)[i] - 128];
    }
  }
}
