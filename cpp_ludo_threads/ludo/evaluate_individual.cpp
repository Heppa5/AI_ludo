#include "evaluate_individual.h"



evaluate_individual::evaluate_individual()
{

}

evaluate_individual::evaluate_individual(int num_gam, QApplication* b)
{
    number_of_games_per_individual=num_gam;
    a=b;
    p1 = new ludo_player();
    p2 = new ludo_player_random();
    p3 = new ludo_player_random();
    p4 = new ludo_player_random();
    //w= new Dialog();
    g=new game();



}

/*void evaluate_individual::setup_game()
{


    g.setGameDelay(000); //if you want to see the game, set a delay

    // //Or don't add the GUI
    QObject::connect(&g,SIGNAL(close()),a,SLOT(quit()));
    //

    //set up for each player
    QObject::connect(&g, SIGNAL(player1_start(positions_and_dice)),&p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p1,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player1_end(std::vector<int>)),    &p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p1,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player2_start(positions_and_dice)),&p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p2,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player2_end(std::vector<int>)),    &p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p2,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player3_start(positions_and_dice)),&p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p3,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player3_end(std::vector<int>)),    &p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p3,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player4_start(positions_and_dice)),&p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p4,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player4_end(std::vector<int>)),    &p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p4,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));
}*/

void evaluate_individual::setup_game()
{
    g->setGameDelay(000); //if you want to see the game, set a delay
    //instanciate the players here


    // Add a GUI <-- remove the '/' to uncomment block

    /*QObject::connect(g,SIGNAL(update_graphics(std::vector<int>)),w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(g,SIGNAL(set_color(int)),                   w,SLOT(get_color(int)));
    QObject::connect(g,SIGNAL(set_dice_result(int)),             w,SLOT(get_dice_result(int)));
    QObject::connect(g,SIGNAL(declare_winner(int)),              w,SLOT(get_winner()));
    QObject::connect(g,SIGNAL(close()),a,SLOT(quit()));
    w->show();*/
    // //Or don't add the GUI

    //*/

    QObject::connect(g,SIGNAL(close()),a,SLOT(quit()));
    //set up for each player
    QObject::connect(g, SIGNAL(player1_start(positions_and_dice)),p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(p1,SIGNAL(select_piece(int)),                g, SLOT(movePiece(int)));
    QObject::connect(g, SIGNAL(player1_end(std::vector<int>)),    p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(p1,SIGNAL(turn_complete(bool)),              g, SLOT(turnComplete(bool)));

    QObject::connect(g, SIGNAL(player2_start(positions_and_dice)),p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(p2,SIGNAL(select_piece(int)),               g, SLOT(movePiece(int)));
    QObject::connect(g, SIGNAL(player2_end(std::vector<int>)),    p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(p2,SIGNAL(turn_complete(bool)),             g, SLOT(turnComplete(bool)));

    QObject::connect(g, SIGNAL(player3_start(positions_and_dice)),p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(p3,SIGNAL(select_piece(int)),               g, SLOT(movePiece(int)));
    QObject::connect(g, SIGNAL(player3_end(std::vector<int>)),    p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(p3,SIGNAL(turn_complete(bool)),             g, SLOT(turnComplete(bool)));

    QObject::connect(g, SIGNAL(player4_start(positions_and_dice)),p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(p4,SIGNAL(select_piece(int)),               g, SLOT(movePiece(int)));
    QObject::connect(g, SIGNAL(player4_end(std::vector<int>)),    p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(p4,SIGNAL(turn_complete(bool)),             g, SLOT(turnComplete(bool)));
}

void evaluate_individual::run()
{
    cout << "Jeg er da startet" << endl;
    p1->change_individual(tactic);
    //p1->set_weights(population[index].con,connum);
    for(int j=0 ; j < number_of_games_per_individual ; j++)
    {

        g->start();
        a->exec();
        while (a->closingDown()){
        }
        // ## ################# evaluation function ###########################
        std::vector<int> player_positions=g->player_positions;
        for(int h=0; h< 4 ; h++)
        {
            if(player_positions[h]==-1 ){
                tactic->set_evaluation(tactic->get_evaluation() -50);
                //int wpw=2;
            }
            else if(player_positions[h]==99)
                tactic->set_evaluation(tactic->get_evaluation() +55);
            else
                tactic->set_evaluation(tactic->get_evaluation() +player_positions[h]);
        }
        if(g->winner==0)
           tactic->set_wins(tactic->get_wins()+1);

        g->reset();
        if(g->wait()){}

    }
    tactic->set_evaluation(tactic->get_evaluation() / number_of_games_per_individual);
    tactic->set_evaluation(tactic->get_evaluation() + tactic->get_wins()*10);
}
