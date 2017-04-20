#ifndef EVALUATE_INDIVIDUAL_H
#define EVALUATE_INDIVIDUAL_H

#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"

#include "individual.h"


using namespace std;

class evaluate_individual: public QThread
{
    //Q_OBJECT
public:
    evaluate_individual();
    evaluate_individual(int num_gam, QApplication* b);



    void setup_game();
    void update_individual(individual* tac)
    {
        tactic=tac;
    }

    void run();


private:
    individual* tactic;

    int number_of_games_per_individual=0;
    QApplication* a;

    game* g;
    ludo_player* p1;
    ludo_player_random* p2;
    ludo_player_random* p3;
    ludo_player_random* p4;
    //Dialog* w;
};

#endif // EVALUATE_INDIVIDUAL_H
