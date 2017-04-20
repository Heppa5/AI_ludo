/*
Fast Artificial Neural Network Library (fann)
Copyright (C) 2003-2012 Steffen Nissen (sn@leenissen.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <iostream>
//#include "/usr/include/floatfann.h"
//#include "/usr/include/fann.h"

#include "floatfann.h"
//#include <fann.h>
#include <fann_cpp.h>
//#include "floatfann.h"
//#include "fann.h"

using namespace std;
using namespace FANN;

int main()
{
    FANN::neural_net net;
    net.create_standard(3, 2,2,2);
    net.randomize_weights(-2,2);
    net.set_activation_function_hidden(SIGMOID_SYMMETRIC);
    net.set_activation_function_output(LINEAR_PIECE_SYMMETRIC);
    
    connection* con;
    int connum = net.get_total_connections();
    con=(connection*)calloc(connum, sizeof(*con));
    
    net.get_connection_array(con);
    for (uint j = 0; j < connum; ++j) {
        printf("weight from %u to %u: %f\n", con[j].from_neuron,
                con[j].to_neuron, con[j].weight);
    }
    
    net.print_connections();

    return 0;
}
/*#include "floatfann.h"
#include "fann.h"

using namespace std;

int main()
{
    const unsigned int num_input = 2;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 3;
    struct fann *ann = fann_create_standard(3, 2, 2, 3);
    //fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_train_on_file(ann, "xor.data", max_epochs, epochs_between_reports, desired_error);
    
    fann_randomize_weights(ann,-1,1);
    
    struct fann_connection *con;   
    unsigned int connum;           
    size_t i;

    fann_print_connections(ann);
    
    
    
    
    //fann_print_parameters(ann);
    //fann_save(ann, "xor_float.net");
    fann_destroy(ann);	
    return 0;
}*/

