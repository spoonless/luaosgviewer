#include <string.h>
#include <iostream>
#include "scriptengine.h"

int main (void) {
    ScriptEngine scriptEngine;

    if (! scriptEngine.exec(std::cin, "stdin"))
    {
        std::cerr << scriptEngine.lastError() << std::endl;
    }
    return 0;
}
