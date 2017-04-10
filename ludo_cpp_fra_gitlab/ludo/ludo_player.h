#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include "floatfann.h"
#include "fann.h"

class ludo_player : public QObject {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();

    struct fann_connection *con;
    struct fann *ann;
public:
    ludo_player();
    ludo_player(struct fann *neunet);
    void set_weights(struct fann_connection* weights, int num_weights);
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
