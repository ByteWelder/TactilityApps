#include "Diceware.h"
#include <TactilityCpp/App.h>

extern "C" {

int main(int argc, char* argv[]) {
    registerApp<Diceware>();
    return 0;
}

}
