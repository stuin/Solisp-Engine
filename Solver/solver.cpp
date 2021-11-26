#include <iostream>
#include "tree.h"
#include "mpi.h"

using std::cout;

void root(int world_size) {
	//Send seed
	int seed = time(NULL);
	MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
	cout << "Seed: " << seed << "\n";

	//Wait for game files
	MPI_Barrier(MPI_COMM_WORLD);
	cout << "All games initialized\n";

	//Get root branch count
	int branches;
	MPI_Recv(&branches, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Barrier(MPI_COMM_WORLD);
	cout << branches << " branches from root found\n";

	MPI_Barrier(MPI_COMM_WORLD);
}

void sim(int world_rank, int world_size) {
    //Get seed
    int seed;
    MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Setup game
    Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder("Klondike.solisp", seed);
	game.setup(builder);
	MPI_Barrier(MPI_COMM_WORLD);

	//Setup root branch
	node root(0, game.get_remaining(), NULL, 0, 0, 0);
	for(int i = 8; i <= 14; i++)
		add_possibilities(&game, &root, i);
	cout << "Calculated positions\n";
	if(world_rank == 0) {
		int children = root.children.size();
		MPI_Send(&children, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	unsigned int min_depth = 2;
	unsigned int min_remaining = game.get_remaining();
	unsigned int max_depth = 0;

	//Pick first branch
	node *current = &root;
	while(time(NULL) < seed + 10 && current != NULL) {
		if(current->depth > min_depth + 20) {
			while(current->parent != NULL && current->depth > min_depth) {
				game.undo(1);
				current = current->parent;
			}
			if(current->simulations >= current->children.size())
				min_depth = current->depth + 1;
		} else {
			current = current->children[(world_rank + current->simulations++) % current->children.size()];
			if(simulate(&game, current)) {
				if(current->clean)
					add_possibilities(&game, current, current->from);

				if(game.get_remaining() < min_remaining) {
					min_remaining = game.get_remaining();
					min_depth = current->depth;
				}
				if(current->depth > max_depth)
					max_depth = current->depth;
			} else {
				node *parent = current->parent;
				parent->children.erase(parent->children.begin()+((world_rank + parent->simulations - 1) % parent->children.size()));
				current = current->parent;
			}
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);
	cout << "Simulator " << world_rank << " at depth " << max_depth << "/" << min_depth;
	cout << ": reached " << min_remaining << " over " << simulations << " simulated moves\n";
}

int main(int argc, char *argv[ ]) {
	MPI_Init(&argc, &argv);

	int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    cout << "Starting " << world_rank << "\n";

    if(world_rank == 0)
		root(world_size);
	else
		sim(world_rank-1, world_size);

	MPI_Finalize();
    return 0;
}