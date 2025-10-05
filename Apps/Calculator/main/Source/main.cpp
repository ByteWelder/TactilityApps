#include "Calculator.h"
#include <TactilityCpp/App.h>

extern "C" {

int main(int argc, char* argv[]) {
    registerApp<Calculator>();
    return 0;
}

}
