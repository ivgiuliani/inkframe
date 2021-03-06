#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include <Arduino.h>
#include <vector>

const char ASCII_EXTENDED_REMAP[] = {
  ' ', // 128 €
  ' ', // 129
  '"', // 130 ‚
  ' ', // 131 ƒ
  '"', // 132 „
  '.', // 133 …
  ' ', // 134 †
  ' ', // 135 ‡
  '^', // 136 ˆ
  '%', // 137 ‰
  'S', // 138 Š
  '<', // 139 ‹
  ' ', // 140 Œ
  ' ', // 141
  'Z', // 142 Ž
  ' ', // 143
  ' ', // 144
  '"', // 145 ‘
  '"', // 146 ’
  '"', // 147 “
  '"', // 148 ”
  '-', // 149 •
  '-', // 150 –
  '-', // 151 —
  '~', // 152 ˜
  ' ', // 153 ™
  's', // 154 š
  '>', // 155 ›
  ' ', // 156 œ
  ' ', // 157
  'z', // 158 ž
  'Y', // 159 Ÿ
  ' ', // 161
  '!', // 162 ¡
  'c', // 163 ¢
  ' ', // 164 £
  ' ', // 165 ¤
  ' ', // 165 ¥
  '|', // 166 ¦
  ' ', // 167 §
  ' ', // 168 ¨
  ' ', // 169 ©
  ' ', // 170 ª
  '<', // 171 «
  ' ', // 172 ¬
  ' ', // 173 ­
  ' ', // 174 ®
  ' ', // 175 ¯
  ' ', // 176 °
  ' ', // 177 ±
  ' ', // 178 ²
  ' ', // 179 ³
  '\'', // 180 ´
  ' ', // 181 µ
  ' ', // 182 ¶
  '-', // 183 ·
  ' ', // 184 ¸
  ' ', // 185 ¹
  ' ', // 186 º
  '>', // 187 »
  ' ', // 188 ¼
  ' ', // 189 ½
  ' ', // 190 ¾
  ' ', // 191 ¿
  'A', // 192 À
  'A', // 193 Á
  'A', // 194 Â -- Unicode first byte point
  'A', // 195 Ã -- Unicode first byte point
  'A', // 196 Ä
  'A', // 197 Å
  'A', // 198 Æ
  'C', // 199 Ç
  'E', // 200 È
  'E', // 201 É
  'E', // 202 Ê
  'E', // 203 Ë
  'I', // 204 Ì
  'I', // 205 Í
  'I', // 206 Î
  'I', // 207 Ï
  'D', // 208 Ð
  'N', // 209 Ñ
  'O', // 210 Ò
  'O', // 211 Ó
  'O', // 212 Ô
  'O', // 213 Õ
  'O', // 214 Ö
  'x', // 215 ×
  'O', // 216 Ø
  'U', // 217 Ù
  'U', // 218 Ú
  'U', // 219 Û
  'U', // 220 Ü
  'Y', // 221 Ý
  ' ', // 222 Þ
  'B', // 223 ß
  'a', // 224 à
  'a', // 225 á
  'a', // 226 â -- Unicode point
  'a', // 227 ã
  'a', // 228 ä
  'a', // 229 å
  'a', // 230 æ
  'c', // 231 ç
  'e', // 232 è
  'e', // 233 é
  'e', // 234 ê
  'e', // 235 ë
  'i', // 236 ì
  'i', // 237 í
  'i', // 238 î
  'i', // 239 ï
  ' ', // 240 ð
  'n', // 241 ñ
  'o', // 242 ò
  'o', // 243 ó
  'o', // 244 ô
  'o', // 245 õ
  'o', // 246 ö
  ' ', // 247 ÷
  'o', // 248 ø
  'u', // 249 ù
  'u', // 250 ú
  'u', // 251 û
  'u', // 252 ü
  'y', // 253 ý
  ' ', // 254 þ
  'y', // 255 ÿ
};

// Starts at 0xC2 0x80, finishes at 0xC2 0xBF
const char UTF_BASIC_C2_REMAP[] = {
  ' ', // 0x80 €
  ' ', // 0x81 <ctrl char>
  ' ', // 0x82 <break>
  ' ', // 0x83 <no break>
  ' ', // 0x84 <ctrl char>
  '.', // 0x85 …
  ' ', // 0x86 †
  ' ', // 0x87 ‡
  '^', // 0x88 ˆ
  '%', // 0x89 ‰
  'S', // 0x8a Š
  '<', // 0x8b ‹
  ' ', // 0x8c Œ
  ' ', // 0x8d 
  'Z', // 0x8e Ž
  ' ', // 0x8f 
  ' ', // 0x90 <control string>
  '\'', // 0x91 ‘
  '\'', // 0x92 ’
  '"', // 0x93 “
  '"', // 0x94 ”
  '-', // 0x95 •
  '-', // 0x96 –
  '-', // 0x97 —
  '~', // 0x98 ˜
  ' ', // 0x99 ™
  's', // 0x9a š
  '>', // 0x9b ›
  ' ', // 0x9c œ
  ' ', // 0x9d <operating system command>
  'z', // 0x9e ž
  'Y', // 0x9f Ÿ
  ' ', // 0xa0 <no break space>
  '!', // 0xa1 ¡
  'c', // 0xa2 ¢
  164, // 0xa3 £
  ' ', // 0xa4 ¤
  'Y', // 0xa5 ¥
  '|', // 0xa6 ¦
  ' ', // 0xa7 §
  ' ', // 0xa8 ¨
  ' ', // 0xa9 ©
  ' ', // 0xaa ª
  '<', // 0xab «
  ' ', // 0xac ¬
  '-', // 0xad <soft hyphen>
  ' ', // 0xae ®
  ' ', // 0xaf ¯
  176, // 0xb0 °
  ' ', // 0xb1 ±
  ' ', // 0xb2 ²
  ' ', // 0xb3 ³
  '\'', // 0xb4 ´
  'u', // 0xb5 µ
  ' ', // 0xb6 ¶
  '-', // 0xb7 ·
  ' ', // 0xb8 ¸
  ' ', // 0xb9 ¹
  176, // 0xba º
  '>', // 0xbb »
  ' ', // 0xbc ¼
  ' ', // 0xbd ½
  ' ', // 0xbe ¾
  '?', // 0xbf ¿
};

// Starts at 0xC3 0x80, finishes at 0xC3 0xbf
const char UTF_BASIC_C3_REMAP[] = {
  'A', // 0x80 À
  'A', // 0x81 Á
  'A', // 0x82 Â
  'A', // 0x83 Ã
  'A', // 0x84 Ä
  'A', // 0x85 Å
  'A', // 0x86 Æ
  'C', // 0x87 Ç
  'E', // 0x88 È
  'E', // 0x89 É
  'E', // 0x8a Ê
  'E', // 0x8b Ë
  'I', // 0x8c
  'I', // 0x8d
  'I', // 0x8e
  'I', // 0x8f
  'D', // 0x90
  'N', // 0x91
  'O', // 0x92
  'O', // 0x93
  'O', // 0x94
  'O', // 0x95
  'O', // 0x96
  'x', // 0x97 ×
  'O', // 0x98 Ø
  'U', // 0x99 Ù
  'U', // 0x9a Ú
  'U', // 0x9b Û
  'U', // 0x9c Ü
  'Y', // 0x9d Ý
  ' ', // 0x9e Þ
  'B', // 0x9f ß
  'a', // 0xa0 à
  'a', // 0xa1 á
  'a', // 0xa2 â
  'a', // 0xa3 ã
  'a', // 0xa4 ä
  'a', // 0xa5 å
  'a', // 0xa6 æ
  'c', // 0xa7 ç
  'e', // 0xa8 è
  'e', // 0xa9 é
  'e', // 0xaa ê
  'e', // 0xab ë
  'i', // 0xac ì
  'i', // 0xad í
  'i', // 0xae î
  'i', // 0xaf ï
  ' ', // 0xb0 ð
  'n', // 0xb1 ñ
  'o', // 0xb2 ò
  'o', // 0xb3 ó
  'o', // 0xb4 ô
  'o', // 0xb5 õ
  'o', // 0xb6 ö
  ' ', // 0xb7 ÷
  'o', // 0xb8 ø
  'u', // 0xb9 ù
  'u', // 0xba ú
  'u', // 0xbb û
  'u', // 0xbc ü
  'y', // 0xbd ý
  ' ', // 0xbe þ
  'y', // 0xbf ÿ
};

std::vector<std::string> split_words(std::string text, const char separator = ' ');
std::string ascii_remap(std::string *orig);
std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

inline std::string from_arduino_str(String *str) {
  return std::string(str->c_str());
}

#endif // __STRING_UTILS_H
