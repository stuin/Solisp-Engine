#include <iostream>
using std::cout;

int main(int argc, char const *argv[]) {
	MPI_Init(&argc, &argv);

	int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(world_rank == 0)
		root();
	else
		sim(world_rank);

	MPI_Finalize();
    return 0;
}

void root() {
	//Send seed
	int seed = time(NULL);
	MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
	cout << seed << "\n";

	//Wait for game files
	MPI_Barrier(MPI_COMM_WORLD);
	cout << "All games initialized\n";
}

void sim(int world_rank) {
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Get seed
    int seed;
    MPI_Recv(&seed, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    //Setup game
    Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder("Klondike-Clean.solisp", seed);
	game.setup(builder);
	MPI_Barrier(MPI_COMM_WORLD);

}