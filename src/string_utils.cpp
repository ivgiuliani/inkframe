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

std::string ascii_remap(std::string *orig) {
  // TODO: refactor this to do proper 2- and 3-bytes table lookups in a more
  // generic way
  std::string new_str;
  std::string::iterator peek_it;

  for (auto it = orig->begin(); it != orig->end(); it++) {
    if (*it < 128) {
      new_str += *it;
      continue;
    }

    // Special case utf8 dashes as they're quite common but 3 bytes wide.
    if (*it == 0xE2) {
      uint16_t dash_mask = 0;
      peek_it = it;
      peek_it++;
      for (uint8_t b = 0; b < 2 && peek_it != orig->end(); b++, peek_it++) {
        dash_mask |= (*peek_it << (b * 8));
      }

      // bytes are in reverse order because we shift left from first to last
      if (
        (dash_mask == 0x9080) || // hyphen
        (dash_mask == 0x9180) || // non-breaking hyphen
        (dash_mask == 0x9280) || // figure-dash (whatever that is)
        (dash_mask == 0x9380) || // en-dash
        (dash_mask == 0x9480) || // em-dash
        (dash_mask == 0x9580) // horizontal bar
      ) {
        new_str += "-";
        it++; it++;
        continue;
      }
    }

    if (*it != 0xC2 && *it != 0xC3) {
      // Possibly extended ascii
      new_str += ASCII_EXTENDED_REMAP[*it - 128];
      continue;
    }

    // Possibly unicode, peek at the next item
    peek_it = it;
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
