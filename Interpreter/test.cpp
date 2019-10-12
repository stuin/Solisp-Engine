#include "game.h"

#include <iostream>
using std::cout;

char cardFace(Solisp::Card *card) {
	switch(card->getData().value) {
		case 1:
			return 'A';
		case 10:
			return 'T';
		case 11:
			return 'J';
		case 12:
			return 'Q';
		case 13:
			return 'K';
	}
	return '0' + card->getData().value;
}

void printStack(Solisp::Card *card) {
	cout << "{" << cardFace(card) << "," << card->getData().suit << "}, ";
	if(card->getNext() != NULL)
		printStack(card->getNext());
}

int main() {
	cout << "Initializing variables\n";
	Solisp::Game game;
	Solisp::Builder *builder = new Solisp::Builder();

	cout << "Counting slots: ";
	int i = 0;
	while(game.getStack(i) != NULL)
		i++;
	cout << i << " found\n";

	cout << "Setting up game\n";
	Solisp::Card *card = game.setup(builder);
	printStack(card);
	game.update();

	cout << " Listing slots\n";
	i = 0;
	while(game.getStack(i) != NULL)	{
		cout << "Stack " << i << ": ";
		if(game.getStack(i)->getCard() != NULL)
			printStack(game.getStack(i)->getCard());
		cout << "\n";
		i++;
	}
}