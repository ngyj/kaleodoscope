#include <sstream>
#include <iostream>

void repl() {
    while (1) {
        if (std::cin.eof()) {
            std::cout << std::endl << "Leaving." << std::endl;
            std::cout.flush();
            return;
        }
        std::cout << "> ";
        while(isspace(std::cin.peek()))
            std::cin.get();

        std::string str;
        std::getline(std::cin, str, ';');
        auto ss = std::istringstream(str);
    }
}

int main() {
    repl();
}
