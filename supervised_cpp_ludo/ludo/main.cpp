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
#include "fstream"

Q_DECLARE_METATYPE( positions_and_dice )

using namespace std;
using namespace FANN;

const unsigned int num_layers=4;
const unsigned int layers[4]={17,4,6,4};

double max_weight_value=5;
double min_weight_value=-5;
int first_max_weight_value=2;
int first_min_weight_value=-2;
int resolution=7;
int first_resolution=5;
int population_size=40;
const int number_of_games_per_individual=200;

float learning_rate=0.2;
bool supervised_learning=false;


int main(int argc, char *argv[]){

    ofstream myfile;
    myfile.open ("output.txt");
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    game g;
    ludo_player p1;
    ludo_player_random p2,p3, p4;
    Dialog w;
    setup_game(&a,&g,&p1,&p2,&p3,&p4,&w);
    vector<individual*> population;
    int generation=0;
    //int connum=0;



    // initialize population
    //population_size=20;
    for(int i=0 ; i<population_size ; i++)
    {
        individual* initialization;
        if(supervised_learning)
            initialization= new individual(first_min_weight_value,first_max_weight_value,first_resolution,generation);
        else
            initialization= new individual(min_weight_value,max_weight_value,first_resolution,generation);
        initialization->true_random_weights();
        // new
        initialization->ann.set_learning_rate(learning_rate); // standard is 0.7
        initialization->ann.set_training_algorithm(TRAIN_INCREMENTAL); // standard is: TRAIN_RPROP
        initialization->ann.set_train_error_function(ERRORFUNC_LINEAR); // Standard is: ERRORFUNC_TANH    -  TANH not recommended for incremental training
        initialization->ann.set_train_stop_function(STOPFUNC_MSE); // standard is: STOPFUNC_MSE
        // end new
        population.push_back(initialization);

    }
    train_population(&population);
    update_max_min_weights(&population,&min_weight_value,&max_weight_value,resolution);
    if(supervised_learning)
    {
        train_population(&population);
        update_max_min_weights(&population,&min_weight_value,&max_weight_value,resolution);
    }


    //while(true){};

   /* double low=99;
    double high=-99;
    // search for new high / low values after training
    for(int i=0 ; i<population_size ; i++)
    {
        vector<double> weights=population[i]->get_low_high_weight();
        if(weights[0]<low)
            low=weights[0];
        if(weights[1]>high)
            high=weights[1];
    }
    min_weight_value = low -2;
    max_weight_value = high +2;*/

    // use the new high/low values to update genes
    for(int i=0 ; i<population_size ; i++)
    {
        population[i]->update_min_max_res(min_weight_value,max_weight_value,resolution);
        population[i]->adjust_weights_to_resolution();
        population[i]->convert_connections_to_genes();
        //population[i]->update_con_matrix();
    }

    int connum=population[0]->ann.get_total_connections();

    generation++;
    bool done=false;
    int highest_win_overall=3;
    while(done==false)
    {
        cout << "################################# Generation: " << generation << " ##############################" << endl;
        // evaluation
        for(int i=0; i< population.size() ; i++)
        {
            population[i]->set_wins(0);
            population[i]->set_evaluation(0);
            evaluation_function(&g,&a,population[i],&p1,connum,number_of_games_per_individual);
            cout << "Individual " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
        }

        // selection
        vector<int> parents;
        for(int i=0; i<population.size()/2 ; i++)
        {
            vector<int> two_parents = selection_roulette_method(&population);
            parents.push_back(two_parents[0]);
            parents.push_back(two_parents[1]);
        }

        // generating children
        vector<individual*> children;
        for(int i=0; i<population.size()/2 ; i++)
        {
            vector<individual*> two_children= population[parents[i*2]]->make_two_children(population[parents[i*2+1]],generation);
            children.push_back(two_children[0]);
            children.push_back(two_children[1]);
        }

        // new
        if(supervised_learning)
        {
            for(int i=0; i<children.size() ; i++)
            {

                vector<double> hej=children[0]->get_low_high_weight();
                cout << "Children " << i << "\t" << hej[0] << "\t" << hej[1]<< endl;
            }


            for(int i=0; i<children.size() ; i++)
            {
                children[i]->ann.set_learning_rate(learning_rate); // standard is 0.7
                children[i]->ann.set_training_algorithm(TRAIN_INCREMENTAL); // standard is: TRAIN_RPROP
                children[i]->ann.set_train_error_function(ERRORFUNC_LINEAR); // Standard is: ERRORFUNC_TANH    -  TANH not recommended for incremental training
                children[i]->ann.set_train_stop_function(STOPFUNC_MSE); // standard is: STOPFUNC_MSE
            }
        }
        // end new

        int highest=-999;
        int highest_win=0;
        int total=0;
        int highest_win_index=0;
        for (int i= 0 ; i<population.size() ; i++)
        {
            if(population[i]->get_evaluation()>highest)
                highest=population[i]->get_evaluation();
            if(population[i]->get_wins()>highest_win)
            {
                highest_win=population[i]->get_wins();
                highest_win_index=i;
            }
            total=total+population[i]->get_evaluation();
        }
        double average=(double)total/(double)population.size();
        //done=true;
        myfile << generation <<"\t"<<highest <<"\t"<< highest_win <<"\t"<<average <<"\t"<< endl;
        if(population[highest_win_index]->get_wins()>highest_win_overall)
        {
            population[highest_win_index]->ann.save("The_best_net.net");
        }
        if(supervised_learning)
        {
            cout << "################### BEFORE TRAINING ######################## "<< endl;
            update_max_min_weights(&children,&min_weight_value,&max_weight_value,resolution);
            train_population(&children);
            cout << "################### AFTER TRAINING ######################## "<< endl;
            update_max_min_weights(&children,&min_weight_value,&max_weight_value,resolution);
        }
        for(int i=0; i< children.size() ; i++)
        {
            children[i]->set_wins(0);
            children[i]->set_evaluation(0);
            evaluation_function(&g,&a,children[i],&p1,connum,number_of_games_per_individual);
            //cout << "children " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
        }

        select_best_offspring(&population,&children,generation,connum,resolution,min_weight_value,max_weight_value);
        if(supervised_learning)
        {
            update_max_min_weights(&population,&min_weight_value,&max_weight_value,resolution);
        }

        generation++;
    }


    a.quit();

    for(int i=0; i< population.size() ; i++)
    {
        delete population[i];
    }
    return 0;
}
