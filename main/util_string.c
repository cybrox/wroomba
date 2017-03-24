#include "util_string.h"

// String utility to check if a string contains a segment at a given position
//
// parameter haystack: The haystack to search in
// parameter needle: The needle to find
// parameter segment: The size of the segment to find
// parameter offset: The offset of the needle in the haystack
//
// returns: Boolean indicator wheter or not the needle was found
bool String_hasSegment(char* haystack, char* needle, int segment, int offset) {
  for (int i = offset; i < segment; i++) {
    if (haystack[i] != needle[i]) {
      return false;
    }
  }

  return true;
}
