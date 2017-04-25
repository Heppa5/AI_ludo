#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"

#include "individual.h"

class ludo_player : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    individual* tactic;
    int star[8]={5,11,18,24,31,37,44,50};
public:
    ludo_player();
    void change_individual(individual* tac)
    {
        tactic = tac;
    }

signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
