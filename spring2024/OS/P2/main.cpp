#include "utils.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <random>
#include <vector>

int deckIndex = 0;
std::vector<Card> deck;
Card target;

void* playerTurn(void* arg)
{
    Player const* currentPlayer = static_cast<Player*>(arg);

    std::cout << currentPlayer->isDealer << '\n';

    pthread_exit(nullptr);
}

int main(int argc, char* args[])
{
    // Process command line arguments

    if (argc < 2 || argc > 3)
    {
        printHelp();
        return EXIT_FAILURE;
    }

    uint seed;

    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(args[i], "-h") == 0 || std::strcmp(args[i], "--help") == 0)
        {
            printHelp();
            return EXIT_FAILURE;
        }
        else if (i == 1)
        {
            seed = atoi(args[i]);
        }
    }

    // Generate RNG engine and deck

    std::mt19937 engine{seed};
    std::uniform_int_distribution card{1, 52};

    deck = generateDeck();

    int dealer = 0;
    pthread_t playerThreads[PLAYER_COUNT];
    std::vector<Player> players{PLAYER_COUNT};

    for (int round = 0; round < ROUND_COUNT; round++)
    {
        players[dealer].isDealer = true;

        // Create threads
        for (int player = 0; player < PLAYER_COUNT; player++)
        {
            pthread_create(&playerThreads[(player + dealer) % PLAYER_COUNT], nullptr, playerTurn, &players[player]);
        }

        // Join threads
        for (int player = 0; player < PLAYER_COUNT; player++)
        {
            pthread_join(playerThreads[(player + dealer) % PLAYER_COUNT], nullptr);
        }

        // Iterate dealer modulo player count and reset dealer status
        // Modulus is necessary if the number of rounds exceeds the number of players
        dealer = (dealer + 1) % PLAYER_COUNT;
        players[dealer].isDealer = false;
    }

    return EXIT_SUCCESS;
}
