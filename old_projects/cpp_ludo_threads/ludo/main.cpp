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
#include "evaluate_individual.h"
#include <atomic>


Q_DECLARE_METATYPE( positions_and_dice )

using namespace std;
using namespace FANN;

const unsigned int num_layers=6;
const unsigned int layers[6]={17,10,11,12,9,4};

const int max_weight_value=10;
const int min_weight_value=-10;
const int resolution=5;
const int population_size=50;
const int number_of_games_per_individual=100;





int main(int argc, char *argv[]){

    atomic_bool done_e
    ofstream myfile;
    myfile.open ("output.txt");
    //QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    vector<individual*> population;
    vector<evaluate_individual*> evaluate_population;
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
        QApplication* a=new QApplication(argc, argv);
        evaluate_individual* in=new evaluate_individual(number_of_games_per_individual,a);
        in->setup_game();
        evaluate_population.push_back(in);
    }
    generation++;
    bool done=false;
    int highest_win_overall=3;
    while(done==false)
    {
        cout << "################################# Generation: " << generation << " ##############################" << endl;
        // evaluation
        evaluation_function(&population,&evaluate_population );
        for(int i=0; i< population.size() ; i++)
        {
           // population[i]->set_wins(0);
            //population[i]->set_evaluation(0);
          //  evaluation_function(&g,&a,population[i],&p1,connum,number_of_games_per_individual);
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
        evaluation_function(&children,&evaluate_population );
        /*for(int i=0; i< children.size() ; i++)
        {
            //children[i]->set_wins(0);
            //children[i]->set_evaluation(0);
           // evaluation_function(&g,&a,children[i],&p1,connum,number_of_games_per_individual);
            //cout << "children " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
        }*/

        select_best_offspring(&population,&children,generation,connum,resolution,min_weight_value,max_weight_value);

        generation++;
    }


    //a.quit();

    for(int i=0; i< population.size() ; i++)
    {
        delete population[i];
    }
    return 0;
}
