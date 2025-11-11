#include "pokebot.hpp"

int main() {
    Pokebot::connect("s2", "5565");
    Pokebot::connect("mrs", "5665");

    cout << "Press Enter to exit..." << endl;
    cin.get(); // Enter キーが押されるまで待つ

    return 0;
}
