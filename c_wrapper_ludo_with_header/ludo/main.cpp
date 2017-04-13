#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"
#include "individual.h"
#include "functions_c_edition.h"
#include <random>
#include <fstream>
Q_DECLARE_METATYPE( positions_and_dice )

#include <iostream>

//std::random_device gen;


const unsigned int num_input = 17; // placement of each piece, closest opponent for each piece,distance to globe for each piece,distance to star for each piece, dice roll = 17
const unsigned int num_output = 1; // which piece to move
const unsigned int num_layers = 5;
const unsigned int num_neurons_hidden = 9;
const int max_weight_value=5;
const int min_weight_value=-5;

const int population_size=25;
const int resolution=5; // number of bits resolution
const int number_of_games_per_individual=100;
unsigned int connum;           // connections number
int range_weights=0;
using namespace std;






int main(int argc, char *argv[]){


    ofstream myfile;
    myfile.open ("output.txt");


    range_weights=max_weight_value-min_weight_value;
    struct fann *ann = fann_create_standard(num_layers, num_input,num_neurons_hidden,num_neurons_hidden,num_neurons_hidden, num_output);
    // setting activation functions as sigmoid_symmetric functions.
    //fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 0);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_activation_function_output(ann, FANN_LINEAR_PIECE);
    fann_set_activation_function_output(ann, FANN_LINEAR_PIECE_SYMMETRIC);
    fann_randomize_weights(ann,min_weight_value,max_weight_value);

    struct fann_connection *con;   // weight matrix
    connum = fann_get_total_connections(ann);
    con = (fann_connection*)calloc(connum, sizeof(*con));
    fann_get_connection_array(ann, con);

    ludo_player p1(ann);
    // ######################## Setting up game #######################################
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    ludo_player_random  p2,p3, p4;
    game g;
    g.setGameDelay(000); //if you want to see the game, set a delay
    // Add a GUI <-- remove the '/' to uncomment block
   /* Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();*/
    // //Or don't add the GUI
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    //*/

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

    // ############################ END OF SETTING UP GAME ############################################

    // Jesper Adding code start








    vector<individual> population;
    for (int i=0; i<population_size ; i++)
    {

        //cout << "\n########## NEW INDIVIDUAL IN INITIALIZATION ####### \n";
        individual indi_initialization;
        fann_randomize_weights(ann,min_weight_value,max_weight_value);
        fann_connection *con_local = new fann_connection;
        con_local = (fann_connection*)calloc(connum, sizeof(*con_local));
        fann_get_connection_array(ann, con_local);  

        /*if(i==0)
        {
             cout << "Jesper ######################3" << endl;
            for (uint j = 0; j < connum; ++j) {
                printf("weight from %u to %u: %f\n", con_local[j].from_neuron,
                       con_local[j].to_neuron, con_local[j].weight);
            }
        }*/
        adjust_weights_to_resolution(connum,con_local,resolution,min_weight_value,range_weights);
        indi_initialization.connum=connum;
        indi_initialization.con=con_local;
        indi_initialization.generation=0;
        indi_initialization.genes=convert_connections_to_genes(connum,con_local,resolution,min_weight_value, range_weights);

        /*if(i==0)
        {
            cout << "Jesper ######################3" << endl;
            /*for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con_local[i].from_neuron,
                       con_local[i].to_neuron, con_local[i].weight);
            }

            //cout << con_local[indi_initialization.genes.size()-1].weight<<"\t"<<indi_initialization.genes[indi_initialization.genes.size()-1] << endl;
        }*/
        population.push_back(indi_initialization);

    }


    /*for (int i=0; i<population.size() ; i++)
    {
        cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

        fann_connection *con2=population[i].con;
        cout << &population[i].con << endl;
        for (uint i = 0; i < connum; ++i) {
            printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                   con2[i].to_neuron, con2[i].weight);
        }
    }*/


    // Jesper adding code
    bool done=false;
    int generation=1;
    while (done==false)
    {
        if(number_of_games_per_individual>99)
            cout << "##########################Evaluation - generation: " << generation << " ############################" << endl;

        // evaluation of population
        for (int i=0 ; i<population.size(); i++)
        {
            //cout << "New player " << i << endl;
            //p1.set_weights(population[i].con,connum);
            population[i].evaluation=0;
            population[i].wins=0;
            evaluation_function(&g,&a,&population,i,&p1,connum,number_of_games_per_individual);
            if(number_of_games_per_individual>99)
                cout << "Individual " << i << " got an evaluation of \t" << population[i].evaluation << "\t and won: " << population[i].wins << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i].generation <<  endl;
        }


        // ############ Selection ################
        vector<int> parents;
        for(int i=0; i<population.size()/2 ; i++)
        {
            vector<int> two_parents = selection_roulette_method(&population);
            parents.push_back(two_parents[0]);
            parents.push_back(two_parents[1]);
        }



        // Mutation
        vector <individual> children=generate_children(&population, generation, parents , ann, connum, resolution,min_weight_value,range_weights);
        // Just change entire population


        /*for (int i=0; i<children.size() ; i++)
        {
            cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

            fann_connection *con2=children[i].con;
            for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                       con2[i].to_neuron, con2[i].weight);
            }
        }*/

        int highest=-999;
        int highest_win=0;
        int total=0;
        for (int i= 0 ; i<population.size() ; i++)
        {
            if(population[i].evaluation>highest)
                highest=population[i].evaluation;
            if(population[i].wins>highest_win)
                highest_win=population[i].wins;
            total=total+population[i].evaluation;
        }
        double average=(double)total/(double)population.size();
        //done=true;
        myfile << generation <<"\t"<<highest <<"\t"<< highest_win <<"\t"<<average <<"\t"<< endl;
        /*for(int i= 0; i< population.size() ; i++)
        {
            if(population[i].evaluation>400)
            {
                cout << "\nWOOOOOW Jeg har noget tilnærmelsesvist ok\n\n\n" << endl;
                fann_set_weight_array(ann,population[i].con,connum);
                fann_save(ann,"hahah.net");
                done=true;
            }
        }*/

        for(int i= 0; i< children.size() ; i++)
        {
            children[i].evaluation=0;
            evaluation_function(&g,&a,&children,i,&p1,connum,number_of_games_per_individual);
        }
        //let_best_3_indi_stay(&population,&children);
        select_best_offspring(&population, &children,ann ,  generation,connum,resolution,min_weight_value,max_weight_value );
        //population=children;
       // std::sort(population.begin(), population.end(), compareBySize);
        generation++;

        /*for (int i=0; i<1 ; i++)
        {
            cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

            fann_connection *con2=population[i].con;
            cout << &population[i].con << endl;
            for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                       con2[i].to_neuron, con2[i].weight);
            }
        }*/


    }
    return 0;
}
