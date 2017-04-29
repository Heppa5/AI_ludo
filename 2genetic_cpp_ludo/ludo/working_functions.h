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

void train_population(vector<individual*> *pop_to_train)
{
    vector<individual*>& population = *pop_to_train;

    for(int i=0 ; i<population.size() ; i++)
    {
        population[i]->ann.train_on_file("train_AI.data",100,0,0.1);
    }
}

void update_max_min_weights(vector<individual*> *pop, double *min_weight_value, double *max_weight_value, int resolution)
{
    vector<individual*>& population = *pop;
    double low=99;
    double high=-99;
    // search for new high / low values after training
    for(int i=0 ; i<population.size() ; i++)
    {

        vector<double> weights=population[i]->get_low_high_weight();
        cout << "Individual "<< i << "\t" << weights[0] << "\t" << weights[1] << endl;
        if(weights[0]<low)
            low=weights[0];
        if(weights[1]>high)
            high=weights[1];
    }
    *min_weight_value = low;
    *max_weight_value = high;
    cout << low << " " << high << endl;
    cout << *min_weight_value << " " << *max_weight_value << endl;
    // use the new high/low values to update genes
    for(int i=0 ; i<population.size() ; i++)
    {
        population[i]->update_min_max_res(*min_weight_value,*max_weight_value,resolution);
        /*population[i]->adjust_weights_to_resolution();
        population[i]->convert_connections_to_genes();*/
        //population[i]->update_con_matrix();
    }
}


void evaluation_function(game *g,QApplication* a, individual* tactic, ludo_player *p1,uint connum, int number_of_games_per_individual)
{

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
                tactic->set_evaluation(tactic->get_evaluation() -90);
                //int wpw=2;
            }
            else if(player_positions[h]==99)
                tactic->set_evaluation(tactic->get_evaluation() +90);
            else
                tactic->set_evaluation(tactic->get_evaluation() +player_positions[h]+30);
        }
        if(g->winner==0)
           tactic->set_wins(tactic->get_wins()+1);
        //cout << "HAd a winner" << endl;
        g->reset();
        if(g->wait()){}

    }
    //tactic->set_evaluation(tactic->get_evaluation() / number_of_games_per_individual);
    //tactic->set_evaluation(tactic->get_evaluation() + tactic->get_wins()*10);
    tactic->set_evaluation(tactic->get_wins()*1);
}

vector<int > selection_roulette_method(vector<individual*> *population,int number_of_games)
{
    random_device gen;
    bool debug =false;
    vector<individual*>& vecRef = *population; // vector is not copied here

    // create roulette wheel
    int total=0;
    for(int i=0; i < vecRef.size() ; i++)
    {
        //cout << "Wow ow" <<vecRef[i]->get_evaluation() << endl;
        total=total+vecRef[i]->get_evaluation();
    }
    total=total/vecRef.size();
    //cout << total << endl;
    vector<int> roulette;
    int more_than_one=0;
    for(int i=0; i<vecRef.size();i++)
    {
        if(vecRef[i]->get_evaluation()-total>0)
        {
            more_than_one++;
            for(int j=0; j<(vecRef[i]->get_evaluation()-total);j++)
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
            roulette.erase(roulette.begin() + i, roulette.begin() + i + vecRef[father_index_population]->get_evaluation()-total-1);
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

bool compareBySize(individual* &a, individual* &b)
{
    return a->get_evaluation() > b->get_evaluation();
}
void select_best_offspring(vector<individual*>* pop, vector<individual*>* child, int generation,uint connum,int resolution,int min_weight_value, int max_weight_value)
{
    vector<individual*>& population = *pop;
    vector<individual*>& children = *child;
    std::sort(population.begin(), population.end(), compareBySize);
    std::sort(children.begin(), children.end(), compareBySize);

    int number_of_random_each_generation=0;
    for(int i=0 ; i<number_of_random_each_generation ; i++)
    {
        individual* initialization= new individual(min_weight_value,max_weight_value,resolution,generation);
        initialization->true_random_weights();
        initialization->convert_connections_to_genes();
       // delete population[population.size()-1-i];
        population[population.size()-1-i]=initialization;

    }


    /*for(int i=0; i< population.size() ; i++)
    {
        cout << population[i]->get_evaluation() << endl;
    }*/
    int count=0;
    for(int i=0; i< population.size()-number_of_random_each_generation ; i++)
    {
        //cout <<(children[i]->get_evaluation() > population[population.size()-1-number_of_random_each_generation-i]->get_evaluation()) << endl;
        //cout << children[i]->get_evaluation() << "\t hej " << population[population.size()-1-number_of_random_each_generation-i]->get_evaluation()  << endl;
        if(children[i]->get_evaluation() > population[population.size()-1-number_of_random_each_generation-i]->get_evaluation())
        {
            cout << "Success - better child \n" << " Child evaluation was: " << children[i]->get_evaluation() << " individual evaluation was: " << population[population.size()-1-i-number_of_random_each_generation]->get_evaluation()<< endl;
            delete population[population.size()-1-number_of_random_each_generation-i];
            population[population.size()-1-number_of_random_each_generation-i]=children[i];
            count++;
        }
    }
    for(int i=0; i< children.size()-count ; i++)
    {
        delete children[children.size()-1-i];
    }
}

vector<int> rank_selection_method(vector<individual*>* pop)
{
    vector<individual*>& population = *pop;
    std::sort(population.begin(), population.end(), compareBySize);

    vector<int> roulette;

    // fill up roulette
    for(int i=0; i<population.size() ; i++)
    {
        for(int j=0; j<population.size()-i ; j++)
        {
            roulette.push_back(i);
        }
    }

    random_device gen;
    std::uniform_int_distribution<> dis(0, roulette.size()-1);

    int father_roulette_index=dis(gen);
    int father_index=roulette[father_roulette_index];

    for(int i = 0 ; i < roulette.size() ; i++)
    {
        if ( roulette[i]==father_index)
        {
           roulette.erase(roulette.begin() + i, roulette.begin() + i + (population.size()-father_index)-1);
        }
    }

    std::uniform_int_distribution<> dis2(0, roulette.size()-1);
    int mother_index_roulette = dis2(gen);
    int mother_index_population = roulette[mother_index_roulette];
    vector<int> parents;
    parents.push_back(father_index);
    parents.push_back(mother_index_population);
    return parents;
}


#endif // WORKING_FUNCTIONS_H
