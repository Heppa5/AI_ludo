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
#include "fann.h"

using namespace std;

int main()
{
    const unsigned int num_input = 2;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 3;
    const unsigned int num_neurons_hidden = 3;
    const float desired_error = (const float) 0.001;
    const unsigned int max_epochs = 500000;
    const unsigned int epochs_between_reports = 1000;
    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
    //fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_train_on_file(ann, "xor.data", max_epochs, epochs_between_reports, desired_error);
    
    fann_randomize_weights(ann,-1,1);
    
    struct fann_connection *con;   /* weight matrix */
    unsigned int connum;           /* connections number */
    size_t i;

    /* Insert your net allocation and training code here */


    connum = fann_get_total_connections(ann);
    if (connum == 0) {
        fprintf(stderr, "Error: connections count is 0\n");
        return EXIT_FAILURE;
    }

    con = (fann_connection*)calloc(connum, sizeof(*con));
    if (con == NULL) {
        fprintf(stderr, "Error: unable to allocate memory\n");
        return EXIT_FAILURE;
    }

    /* Get weight matrix */
    fann_get_connection_array(ann, con);

    /* Print weight matrix */
    for (i = 0; i < connum; ++i) {
        printf("weight from %u to %u: %f\n", con[i].from_neuron,
               con[i].to_neuron, con[i].weight);
    }

    free(con);
    
    
    
    
    //fann_print_parameters(ann);
    //fann_save(ann, "xor_float.net");
    fann_destroy(ann);	;
    return 0;
}

/*int main()
{
	fann_type *calc_out;
	fann_type input[2];

	struct fann *ann = fann_create_from_file("xor_float.net");

	input[0] = -1;
	input[1] = 1;
	calc_out = fann_run(ann, input);

	printf("xor test (%f,%f) -> %f\n", input[0], input[1], calc_out[0]);

	fann_destroy(ann);
	return 0;
}*/
