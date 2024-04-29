#ifndef MATCH_UTILS
#define MATCH_UTILS

#include <queue>
#include <random>
#include <string>
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

std::string toString(Rank rank);

enum class Suite
{
    HEARTS,
    DIAMONDS,
    CLUBS,
    SPADES,
    SUITE_COUNT,
};

std::string toString(Suite suite);

struct Card
{
    Rank rank;
    Suite suite;

    // for std::find()
    bool operator==(const Card& rhs);
};

struct Player
{
    std::vector<Card> hand;
};

// Prints command line documentation
void printHelp();

// Generates a standard deck of cards
std::queue<Card> generateShuffledDeck(std::mt19937 engine);

// Finds card of same rank
bool hasMatch(std::vector<Card> hand, Card target);

#endif // MATCH_UTILS
