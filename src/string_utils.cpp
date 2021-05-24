#include <vector>
#include <string>

#include "string_utils.h"

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
  return rtrim(ltrim(s));
}

std::vector<std::string> split_words(std::string text, const char separator) {
  std::vector<std::string> words{};
  text = trim(text);
  if (text.empty()) {
    return words;
  }
  std::string ss = std::string(text);

  size_t pos = 0;
  while ((pos = ss.find(separator)) != std::string::npos) {
    const std::string word = ss.substr(0, pos);
    words.push_back(word);

    ss.erase(0, pos + 1);
  }
  words.push_back(ss.c_str());

  return words;
}

std::string ascii_extended_remap(std::string *orig) {
  std::string new_str;

  for (auto it = orig->begin(); it != orig->end(); it++) {
    if (*it < 128) {
      new_str += *it;
      continue;
    }

    if (*it != 0xC2 && *it != 0xC3) {
      // Possibly extended ascii
      new_str += ASCII_EXTENDED_REMAP[*it - 128];
      continue;
    }

    // Possibly unicode, peek at the next item
    std::string::iterator peek_it = it;
    peek_it++;

    const bool we_can_remap = peek_it != orig->end() &&
                              *peek_it >= 0x80 &&
                              *peek_it <= 0xbf;

    if (we_can_remap) {
      const char *tbl;

      if (*it == 0xC2) tbl = UTF_BASIC_C2_REMAP;
      if (*it == 0xC3) tbl = UTF_BASIC_C3_REMAP;

      new_str += tbl[*peek_it - 0x80];
      it++;
    } else {
      new_str += ASCII_EXTENDED_REMAP[*it - 128];
    }
  }

  return new_str;
}
