#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"
#include "individual.h"


Q_DECLARE_METATYPE( positions_and_dice )

#include <iostream>

using namespace std;

void adjust_weights_to_resolution(uint connum, struct fann_connection *con, uint resolution )
{
    for (uint i = 0; i < connum; ++i)
    {
        double cur_weight=(double)con[i].weight;
        double error=99;
        int min_error_index=0;

        for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            if(abs(cur_weight-(-1+j*2/(pow(2,resolution)-1)))< error )
            {
                error=abs(cur_weight-(-1+j*2/pow(2,resolution)));
                min_error_index=j;
            }
        }
        con[i].weight=(-1+min_error_index*2/pow(2,resolution));
       // cout << "Cur_weight= " <<(double) cur_weight << " "<< con[i].weight <<endl;
    }
}


int main(int argc, char *argv[]){

    // Jesper Adding code start

    const unsigned int num_input = 5; // placement of each piece, closest opponent for each piece,distance to globe for each piece,distance to star for each piece, dice roll = 17
    const unsigned int num_output = 1; // which piece to move
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 25;
    const int max_weight_value=1;
    const int min_weight_value=-1;
    const int population_size=20;
    const int resolution=4; // number of bits resolution
    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

    // setting activation functions as sigmoid_symmetric functions.
    //fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 0);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    fann_randomize_weights(ann,min_weight_value,max_weight_value);


    struct fann_connection *con;   // weight matrix
    unsigned int connum;           // connections number


    connum = fann_get_total_connections(ann);
    con = (fann_connection*)calloc(connum, sizeof(*con));
    fann_get_connection_array(ann, con);




    //while(1){}
   /* fann_type input[num_input];
    fann_type *calc_out;
    for (int h=0 ; h < 100 ; h++){
        for (int j=0; j<num_input ; j++)
        {
            double r3 = min_weight_value + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max_weight_value-min_weight_value)));
            input[j]= r3;
        }
        input[0]=51;
        calc_out =fann_run(ann,input);
        printf ("Test (%f,%f,%f,%f,%f,%f,%f,%f,%f \n ,%f,%f,%f,%f,%f,%f,%f,%f) -> %f \n", input[0],input[1],input[2],input[3],input[4],input[5],input[6],input[7],input[8],input[9],input[10],input[11],input[12],input[13],input[14],input[15],input[16],calc_out[0]);
    }*/
    //free(con);
    //fann_print_connections(ann);
    ludo_player p1(ann);

    vector<individual> population;
    for (int i=0; i<population_size ; i++)
    {
        cout << "\n########## NEW INDIVIDUAL IN INITIALIZATION ####### \n";
        individual indi_initialization;
        fann_randomize_weights(ann,min_weight_value,max_weight_value);
        fann_get_connection_array(ann, con);
        adjust_weights_to_resolution(connum,con,resolution);
        indi_initialization.connum=connum;
        indi_initialization.con=*con;
        indi_initialization.generation=0;
        for (uint i = 0; i < connum; ++i) {
            printf("weight from %u to %u: %f\n", con[i].from_neuron,
                   con[i].to_neuron, con[i].weight);
        }
    }

    while(1){};
    p1.set_weights(con,connum);




    // Jesper Adding code end

    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    //ludo_player p1;
    ludo_player_random  p2,p3, p4;

    game g;
    g.setGameDelay(000); //if you want to see the game, set a delay

    // Add a GUI <-- remove the '/' to uncomment block
    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();
    // //Or don't add the GUI
    //QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
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


    // Jesper adding code
    //void (*foo)();
    //foo = &test_signals;
    //QObject::connect(&g,SIGNAL(declare_winner(int)), &*foo);

    for(int i = 0; i < 10000; ++i){
        g.start();
        a.exec();
        cout << "Winner is: " << g.return_winner() << endl;
        g.reset();
    }
    return 0;
}
