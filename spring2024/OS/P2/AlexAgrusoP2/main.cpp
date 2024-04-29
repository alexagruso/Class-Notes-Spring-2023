#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <random>
#include <vector>

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

    int dealer = 0;
    int deckIndex = 0;
    int currentTurn = 1; // dealer + 1
    Card target;

    std::mt19937 randomEngine{seed};
    std::queue<Card> deck;
    std::vector<Player> players{PLAYER_COUNT};

    for (int round = 0; round < ROUND_COUNT; round++)
    {
        deck = generateShuffledDeck(randomEngine);

        target = deck.front();
        std::cout << "Dealer picks " << toString(deck.front().rank) << " of " << toString(deck.front().suite) << '\n';
        deck.pop();

        for (int player = 0; player < PLAYER_COUNT; player++)
        {
            if (player != dealer)
            {
                players[player].hand.push_back(deck.front());
                deck.pop();
            }
        }

        while (true)
        {
            if (currentTurn == dealer)
            {
                currentTurn = (currentTurn + 1) % PLAYER_COUNT;
                continue;
            }

            players[currentTurn].hand.push_back(deck.front());
            std::cout << "Player " << currentTurn + 1 << " draws " << toString(deck.front().rank) << " of "
                      << toString(deck.front().suite) << '\n';
            deck.pop();

            if (hasMatch(players[currentTurn].hand, target))
            {
                int winner = currentTurn;

                for (int player = 0; player < PLAYER_COUNT; player++)
                {
                    if (player != dealer)
                    {
                        if (player == winner)
                        {
                            std::cout << "Player " << player + 1 << " won round " << round + 1 << '\n';
                        }
                        else
                        {
                            std::cout << "Player " << player + 1 << " lost round " << round + 1 << '\n';
                            ;
                        }
                    }
                }

                break;
            }

            std::uniform_int_distribution<int> discard{0, 1};
            int discardPosition = discard(randomEngine);

            deck.push(players[currentTurn].hand.at(discardPosition));
            std::cout << "Player " << currentTurn + 1 << " discards " << toString(deck.back().rank) << " of "
                      << toString(deck.back().suite) << '\n';
            players[currentTurn].hand.erase(players[currentTurn].hand.begin() + discardPosition);

            currentTurn = (currentTurn + 1) % PLAYER_COUNT;
        }

        // Reset state
        dealer = (dealer + 1) % PLAYER_COUNT;
        currentTurn = (dealer + 1) % PLAYER_COUNT;
        deckIndex = 0;
        players = std::vector<Player>{PLAYER_COUNT};

        std::cout << "\n================================================================\n\n";
    }

    return EXIT_SUCCESS;
}
