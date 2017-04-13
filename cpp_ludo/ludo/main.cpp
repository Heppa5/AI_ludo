#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"


#include "iostream"
#include "floatfann.h"
#include <fann_cpp.h>
#include "random"
#include "working_functions.h"
#include "individual.h"

Q_DECLARE_METATYPE( positions_and_dice )

using namespace std;
using namespace FANN;

const unsigned int num_layers=3;
const unsigned int layers[3]={17,6,1};

const int max_weight_value=5;
const int min_weight_value=-5;
const int resolution=5;
const int population_size=25;
const int number_of_games_per_individual=100;


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
                tactic->set_evaluation(tactic->get_evaluation() -40);
                //int wpw=2;
            }
            else if(player_positions[h]==99)
                tactic->set_evaluation(tactic->get_evaluation() +60);
            else
                tactic->set_evaluation(tactic->get_evaluation() +player_positions[h]);
        }
        if(g->winner==0)
           tactic->set_wins(tactic->get_wins()+1);

        g->reset();
        if(g->wait()){}

    }
    tactic->set_evaluation(tactic->get_evaluation() / number_of_games_per_individual);
    tactic->set_evaluation(tactic->get_evaluation() + tactic->get_wins()*25);
}



int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    game g;
    ludo_player p1;
    ludo_player_random p2,p3, p4;
    Dialog w;
    setup_game(&a,&g,&p1,&p2,&p3,&p4,&w);
    vector<individual*> population;
    int generation=0;
    int connum=0;
    connection *con;

    FANN::neural_net ann;
    //ann.create_standard(3, 2,2,2);
    ann.create_standard_array(num_layers,layers);
    ann.set_activation_function_hidden(SIGMOID_SYMMETRIC);
    ann.set_activation_function_output(LINEAR_PIECE_SYMMETRIC);

    connum=ann.get_total_connections();

    // initialize population
    for(int i=0 ; i<population_size ; i++)
    {
        individual* initialization= new individual(min_weight_value,max_weight_value,resolution,generation);
        initialization->true_random_weights();
        initialization->convert_connections_to_genes();
        population.push_back(initialization);
    }
    bool done=false;
    while(done==false)
    {
        cout << "hej"<< endl;
        // evaluation
        for(int i=0; i< population.size() ; i++)
        {
            population[i]->set_wins(0);
            population[i]->set_evaluation(0);
            cout << "hej"<< endl;
            evaluation_function(&g,&a,population[i],&p1,connum,number_of_games_per_individual);
            cout << "Individual " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
        }

        // selection
        vector<int> parents;
        for(int i=0; i<population.size()/2 ; i++)
        {
            /*vector<int> two_parents = selection_roulette_method(&population);
            parents.push_back(two_parents[0]);
            parents.push_back(two_parents[1]);*/
        }
        /*
        // generating children
        vector<individual*> children;
        for(int i=0; i<population.size()/2 ; i++)
        {
            vector<individual*> two_children= population[parents[i*2]]->make_two_children(population[parents[i*2+1]],generation);
            children.push_back(two_children[0]);
            children.push_back(two_children[1]);
        }

        for(int i=0; i< children.size() ; i++)
        {
            children[i]->set_wins(0);
            children[i]->set_evaluation(0);
            evaluation_function(&g,&a,population[i],&p1,connum,number_of_games_per_individual);
            cout << "children " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
        }*/
    }


    a.quit();

    for(int i=0; i< population.size() ; i++)
    {
        delete population[i];
    }
    return 0;
}
