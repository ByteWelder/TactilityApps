#include "Gpio.h"

extern "C" {

int main(int argc, char* argv[]) {
    registerApp<Gpio>();
    return 0;
}

}
