#ifndef WORKING_FUNCTIONS_H
#define WORKING_FUNCTIONS_H


void setup_game(QApplication *a, game *g, ludo_player *p1, ludo_player_random *p2, ludo_player_random *p3, ludo_player_random *p4,Dialog *w)
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
    QObject::connect(g,SIGNAL(close()),a,SLOT(quit()));
    //*/

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




vector<int > selection_roulette_method(vector<individual*> *population)
{
    random_device gen;
    bool debug =false;
    vector<individual*>& vecRef = *population; // vector is not copied here

    // create roulette wheel

    vector<int> roulette;
    int more_than_one=0;
    for(int i=0; i<vecRef.size();i++)
    {
        if(vecRef[i]->get_evaluation()>0)
        {
            more_than_one++;
            for(int j=0; j<vecRef[i]->get_evaluation();j++)
            {
                roulette.push_back(i);
            }
        }
    }
    if (more_than_one<2)
    {
        for(int i=0; i<vecRef.size();i++)
        {
            roulette.push_back(i);
        }
    }

    if (debug == true)
    {
        for( int i = 0; i< roulette.size() ; i++)
        {
            //cout << roulette[i] << endl;
        }
        //cout << "Size of roulette is: " << roulette.size() << endl;
    }
    std::uniform_int_distribution<> dis(0, roulette.size()-1);

    int father_index_roulette = dis(gen);

    int father_index_population = roulette[father_index_roulette];
    // remove father from roulette
    for(int i = 0 ; i < roulette.size() ; i++)
    {
        if ( roulette[i]==father_index_population)
        {
            if(debug == true)
            {
                cout << "Deleting father index: " << father_index_population << " and random number was: " << father_index_roulette << "\n So we're deleting from " << i << " to " << i + vecRef[father_index_population]->get_evaluation() << endl;
                //cout << roulette[i + vecRef[father_index_population].evaluation-1] << endl;
            }
            roulette.erase(roulette.begin() + i, roulette.begin() + i + vecRef[father_index_population]->get_evaluation()-1);
            break;
        }
    }
    std::uniform_int_distribution<> dis2(0, roulette.size()-1);
    int mother_index_roulette = dis2(gen);
    int mother_index_population = roulette[mother_index_roulette];
    vector<int> parents;
    parents.push_back(father_index_population);
    parents.push_back(mother_index_population);
    return parents;
}

#endif // WORKING_FUNCTIONS_H
