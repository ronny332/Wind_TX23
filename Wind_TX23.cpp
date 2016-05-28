#include <wiringPi.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <unordered_map>

int port = 0;

std::unordered_map<int, std::string> directions {
        {0b0000, "N"},
        {0b0001, "NNE"},
        {0b0010, "NE"},
        {0b0011, "ENE"},
        {0b0100, "E"},
        {0b0101, "ESE"},
        {0b0110, "SE"},
        {0b0111, "SSE"},
        {0b1000, "S"},
        {0b1001, "SSW"},
        {0b1010, "SW"},
        {0b1010, "SW"},
        {0b1100, "W"},
        {0b1101, "WNW"},
        {0b1110, "NW"},
        {0b1111, "NNW"}
} ;

int main(int argc, char **argv) {
    if (wiringPiSetup() == -1)
        return 1;

    bool debug = false;

    if (argc > 1) {
        std::string tmp;

        for (int i = 1; i < argc; i++) {
            tmp = argv[i];
            if (tmp == "--debug") {
                debug = true;
                break;
            }

            if (tmp == "--help") {
                std::cout << "Usage: Wind_TX23 [options]\n\nOptions:\n--help\t\tthis help page\n--debug\t\tturn on debug messages\n--format\t=[ms, kmh], ms is default\n\n";
                exit(0);
            }
        }
    }

    pinMode(port, OUTPUT);
    digitalWrite(port, LOW);
    delay(500);

    pinMode(port, INPUT);
    pullUpDnControl(port, PUD_UP);

    // ignore start bit
    while (digitalRead(port) == HIGH) {
        usleep(100);
    }

    // wind meter pulls low, ignore
    while (digitalRead(port) == LOW) {
        usleep(100);
    }

    // walk into first data field. field length = ~1200ns (~1,2ms), wait 500ns
    usleep(500);

    // data
    bool data[41];

    if (debug) {
        std::cout << "raw data:\n";
    }

    for (int i = 0; i < 41; i++) {
        if (debug && (i == 5 || i == 9 || i == 21 || i == 25 || i == 29)) {
            std::cout << "\n";
        }

        data[i] = (digitalRead(0) == LOW) ? false : true;
        if (debug) {
            std::cout << data[i];
        }
        // either digitalRead is too slow, or 1,2ms is too long. 1,15ms is working very well
        usleep(1150);
    }

    if (debug) {
        std::cout << "\n\n";
    }

    // format data
    int direction {0};
    int speed {0};

    // direction
    for (int i = 5; i < 9; i++) {
        direction = direction << 1 | data[i];
    }

    // speed
    for (int i = 20; i > 8; i--) {
        speed = speed << 1 | data[i];
    }

    double speed_human = speed;
    speed_human /= 10;

    std::string speed_format = "m/s";

    // m/s or km/h, m/s is default
    if (argc > 1) {
        std::string tmp;

        for (int i = 1; i < argc; i++) {
            tmp = argv[1];
            if (tmp == "--format=kmh") {
                speed_human *= 3.6;
                speed_format = "km/h";
            }
        }
    }

    // output
    if (debug) {
        std::cout << "direction raw: " << direction << "\n";
    }
    std::cout << "direction: " << directions[direction] << "\n";
    if (debug) {
        std::cout << "speed raw: " << speed << "\n";
    }
    std::cout << "speed: " << speed_human << " " << speed_format << "\n";
}

