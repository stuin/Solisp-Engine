#include <iostream>
#include "tree.h"
#include "mpi.h"

using std::cout;

#define SEED 1638558167
#define MAX_TIME 10
#define MAX_DEPTH 20

int choose_next(node *node, int world_rank, int world_size) {
	int size = node->children.size();
	//return (world_rank + node->simulations++) % size;
	return (world_rank * (size / world_size) + node->depth + node->simulations++) % size;
}

int main(int argc, char *argv[ ]) {
	MPI_Init(&argc, &argv);

	int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Setup base vars
	int seed;
	int end;
	if(world_rank == 0) {
		seed = SEED;
		cout << "Seed: " << seed << "\n";
		end = time(NULL) + MAX_TIME;
	}

	//Share base variables
	MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&end, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Wait for game files
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder("Klondike.solisp", seed);
	game.setup(builder);

	//Create save file with starting conditions
	if(world_rank == 0)
		game.save("../saves/Klondike.seed.sav");
	MPI_Barrier(MPI_COMM_WORLD);

	if(world_rank == 0)
		cout << "All games initialized\n";

	//Setup root branch
	node root(0, game.get_remaining(), NULL, 0, 0, 0);
	for(int i = 8; i <= 14; i++)
		add_possibilities(&game, &root, i);
	if(world_rank == 0)
		cout << "Found " << root.children.size() << " starting positions\n";
	MPI_Barrier(MPI_COMM_WORLD);

	unsigned int min_depth = 2;
	unsigned int min_remaining = game.get_remaining();
	unsigned int max_depth = 0;

	//Explore some branches
	node *current = &root;
	node *best = current;
	while(time(NULL) < end && current != NULL) {
		if(current->depth > min_depth + MAX_DEPTH) {
			while(current->parent != NULL && current->depth > min_depth) {
				game.undo(1, false);
				current = current->parent;
			}
		} else {
			current = current->children[choose_next(current, world_rank, world_size)];
			if(!current->failed && simulate(&game, current)) {
				if(current->clean)
					add_possibilities(&game, current, current->from);

				if(game.get_remaining() < min_remaining) {
					min_remaining = game.get_remaining();
					min_depth = current->depth;
					best = current;
				}
				if(current->depth > max_depth)
					max_depth = current->depth;
			} else
				current = current->parent;
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank == 0)
		cout << "Starting with seed " << seed << " and " << root.remaining << " cards:\n";
	MPI_Barrier(MPI_COMM_WORLD);

	cout << "Simulator " << world_rank << " at depth " << max_depth << "/" << min_depth << 
		": reached " << min_remaining << " cards in " << best->depth << " moves using " << simulations << " simulations\n";

	MPI_Barrier(MPI_COMM_WORLD);

	int recv;
	if(world_rank > 0)
        MPI_Recv(&recv, 1, MPI_INT, world_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	cout << world_rank << ": ";
	best->print_stack();
	cout << "\n";
	
	if(world_size > world_rank + 1)
		MPI_Send(&min_remaining, 1, MPI_INT, world_rank+1, 0, MPI_COMM_WORLD);

	MPI_Finalize();
    return 0;
}