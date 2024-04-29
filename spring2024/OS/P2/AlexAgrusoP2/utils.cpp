#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <pthread.h>
#include <random>
#include <string>

std::string toString(Rank rank)
{
    switch (rank)
    {
        case Rank::ONE: return "One"; break;
        case Rank::TWO: return "Two"; break;
        case Rank::THREE: return "Three"; break;
        case Rank::FOUR: return "Four"; break;
        case Rank::FIVE: return "Five"; break;
        case Rank::SIX: return "Six"; break;
        case Rank::SEVEN: return "Seven"; break;
        case Rank::EIGHT: return "Eight"; break;
        case Rank::NINE: return "Nine"; break;
        case Rank::TEN: return "Ten"; break;
        case Rank::JACK: return "Jack"; break;
        case Rank::QUEEN: return "Queen"; break;
        case Rank::KING: return "King"; break;
        default: return ""; break;
    }
}

std::string toString(Suite suite)
{
    switch (suite)
    {
        case Suite::HEARTS: return "Hearts"; break;
        case Suite::DIAMONDS: return "Diamonds"; break;
        case Suite::CLUBS: return "Clubs"; break;
        case Suite::SPADES: return "Spades"; break;
        default: return ""; break;
    }
}

bool Card::operator==(const Card& rhs)
{
    return this->rank == rhs.rank;
}

void printHelp()
{
    std::cout << "How to use:\n";
    std::cout << "  match <SEED> <FLAGS>\n";
    std::cout << "  where <SEED> is any unsigned 32 bit integer\n\n";

    std::cout << "Flags:\n";
    std::cout << "  -h | --help  print this menu\n";
}

std::queue<Card> generateShuffledDeck(std::mt19937 engine)
{
    std::vector<Card> shuffled{};

    for (int rank = 0; rank < static_cast<int>(Rank::RANK_COUNT); rank++)
    {
        for (int suite = 0; suite < static_cast<int>(Suite::SUITE_COUNT); suite++)
        {
            shuffled.push_back({static_cast<Rank>(rank), static_cast<Suite>(suite)});
        }
    }

    std::shuffle(shuffled.begin(), shuffled.end(), engine);

    std::queue<Card> deck{};

    for (const Card& card : shuffled)
    {
        deck.push(card);
    }

    return deck;
}

bool hasMatch(std::vector<Card> hand, Card target)
{
    for (const Card& card : hand)
    {
        if (card.rank == target.rank)
        {
            return true;
        }
    }

    return false;
}
