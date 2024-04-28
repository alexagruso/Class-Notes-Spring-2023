#include "utils.hpp"

#include <iostream>
#include <pthread.h>

void printHelp()
{
    std::cout << "How to use:\n";
    std::cout << "  match <SEED> <FLAGS>\n";
    std::cout << "  where <SEED> is any unsigned 32 bit integer\n\n";

    std::cout << "Flags:\n";
    std::cout << "  -h | --help  print this menu\n";
}

std::vector<Card> generateDeck()
{
    std::vector<Card> deck{};

    for (int rank = 0; rank < static_cast<int>(Rank::RANK_COUNT); rank++)
    {
        for (int suite = 0; suite < static_cast<int>(Suite::SUITE_COUNT); suite++)
        {
            deck.push_back({static_cast<Rank>(rank), static_cast<Suite>(suite)});
        }
    }

    return deck;
}
