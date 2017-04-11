#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}
ludo_player::ludo_player(struct fann *neunet):
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    ann=neunet;
}

int ludo_player::make_decision(){
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}

/*int ludo_player::make_decision(){
    int num_input=fann_get_num_input(ann);
    fann_type input[5];
    fann_type *calc_out;
    bool debug=false;
    // Fill up input
    if(debug)
        cout << "The dice is: " << dice_roll << endl;
    for(int i=0; i<4 ;i++) // 4 opponents
    {
        if(debug)
            cout << "The position of piece " << i << " is: " << pos_start_of_turn[i] << endl;
        if(pos_start_of_turn[i]==-1)
            input[i]=pos_start_of_turn[i];
        else if(pos_start_of_turn[i]>0 && pos_start_of_turn[i] != 99)
            input[i]=pos_start_of_turn[i]/55;
        else // if in goal then set it to zero
            input[i]=0;
    }
    input[num_input-1]=dice_roll;
    // Calculate output
    calc_out =fann_run(ann,input);

    if(calc_out[0] < -0.5 )
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 0" << std::endl;
        return 0;
    }
    else if(calc_out[0] > -0.5 && calc_out[0]<0)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 1" << std::endl;
        return 1;
    }
    else if(calc_out[0]>0 && calc_out[0]<0.5)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 2" << std::endl;
        return 2;
    }
    else if(calc_out[0]>0.5)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 3" << std::endl;
        return 3;
    }
    return -1;
}*/
void ludo_player::set_weights(struct fann_connection* weights, int num_weights)
{
    con=weights;
    fann_set_weight_array(ann,con,num_weights);
}

void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}
