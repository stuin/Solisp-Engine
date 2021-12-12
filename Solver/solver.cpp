#include <iostream>
#include "tree.h"
#include "mpi.h"

using std::cout;

#define SEED time(NULL)
//1638558167

#define MAX_TIME 2
#define MAX_LOOPS 30
#define BACKUP_DEPTH 2
#define MAX_DEPTH 20
#define START_DEPTH 2

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
		if(argc > 1)
			seed = atoi(argv[1]);
		else
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
	//if(world_rank == 0)
	//	game.save("../saves/Klondike.seed.sav");
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

	std::vector<node *> wins;
	node *best;
	unsigned int max_depth = 0;
	unsigned int min_depth = START_DEPTH;

	//Explore some branches
	node *current = &root;
	best = current;

	while(time(NULL) < end && current != NULL) {
		//Return to min_depth
		if(current->depth > min_depth + MAX_DEPTH) {
			while(current->parent != NULL && current->depth > min_depth) {
				game.undo(1, true);
				current = current->parent;

				//Move up when node has been run many times
				if(current->simulations >= current->children.size() * MAX_LOOPS)
					min_depth -= BACKUP_DEPTH;
			}
		} else {
			//Choose path to follow
			current = current->children[choose_next(current, world_rank, world_size)];
			if(!current->failed && simulate(&game, current)) {
				if(game.get_remaining() == 0) {
					cout << "Win found at depth " << current->depth << "\n";
					current->failed = true;
					wins.push_back(current);

					//Reset for new round
					min_depth -= MAX_DEPTH;
					best = &root;
				} else {
					if(current->clean)
						add_possibilities(&game, current, current->from);

					if(current->remaining < best->remaining) {
						min_depth = current->depth;
						best = current;
					}

					if(current->depth > max_depth)
						max_depth = current->depth;
				}
			} else
				current = current->parent;
		}
	}
	wins.push_back(best);

	MPI_Barrier(MPI_COMM_WORLD);
	if(world_rank == 0)
		cout << "Starting with seed " << seed << " and " << root.remaining << " cards:\n";

	for(node *n : wins)
		if(n->remaining < best->remaining || (n->depth < best->depth && n->remaining == best->remaining))
			best = n;

	cout << "Simulator " << world_rank << ": reached " << best->remaining << " cards in " << best->depth << " moves"
		<< " using " << simulations << " simulations and max depth " << max_depth << "\n";

	MPI_Barrier(MPI_COMM_WORLD);

	int best_remaining = best->remaining;
	int best_rank = world_rank;
	if(world_rank == 0) {
		for(int i = 1; i < world_size; i++) {
			int recv;
			MPI_Recv(&recv, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if(recv < best_remaining) {
				best_remaining = recv;
				best_rank = i;
			}
		}
	} else
        MPI_Send(&best_remaining, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    MPI_Bcast(&best_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if(world_rank == best_rank) {
    	cout << world_rank << ": ";
		best->print_stack();
		cout << "\n";
	}

	MPI_Finalize();
    return 0;
}