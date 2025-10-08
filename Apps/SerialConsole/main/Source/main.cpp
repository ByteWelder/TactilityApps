#include "SerialConsole.h"
#include <TactilityCpp/App.h>

extern "C" {

int main(int argc, char* argv[]) {
    registerApp<SerialConsole>();
    return 0;
}

}
