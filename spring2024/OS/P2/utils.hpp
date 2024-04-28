#ifndef MATCH_UTILS
#define MATCH_UTILS

#include <vector>

constexpr int ROUND_COUNT = 6;
constexpr int PLAYER_COUNT = 6;

enum class Rank
{
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    RANK_COUNT,
};

enum class Suite
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES,
    SUITE_COUNT,
};

struct Card
{
    Rank rank;
    Suite suite;
};

struct Player
{
    std::vector<Card> hand;
    bool isDealer;
};

// Prints command line documentation
void printHelp();

// Generates a standard deck of cards
std::vector<Card> generateDeck();

#endif // MATCH_UTILS
