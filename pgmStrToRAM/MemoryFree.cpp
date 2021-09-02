//#include <Arduino.h>
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
//Serial.println("__arm__ defined");
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
//Serial.println("__arm__ undefined");
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
