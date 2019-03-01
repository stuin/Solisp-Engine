##Data types:

* Num: A single non-card number
* Deck: A list of at least one card value
* Pattern: List of Decks with specific order
* Layout: A list of other Layouts and Slots on the screen
* Slot: A place on screen to store the Deck
* Feature: An attribute defining the workings of a slot

##Deck Constants
Returns Deck/Pattern (Unspecified suit)

* Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Joker, Any
* Numbers:
	Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten
* Faces:
	Jack, Queen, King
* Standard:
	Numbers, Faces
* Jokers:
	Joker, Joker

##Deck Functions
Returns Deck/Pattern

* Add Deck Deck ..:
	Append Decks together into one
* Remove Deck Deck:
	Remove the second Deck from the first
* Clone Num Deck:
	Append Num copies of Deck together
* Reverse Deck:
	Reverse Deck order
* Loop Deck:
	Add extra copy of the first card to the end

##Suit Functions
Returns Deck/Pattern

* Four-Suit Deck:
	Duplicate Deck for each suit
* Clubs Deck:
	Set Deck to Clubs suit
* Spades Deck:
	Set Deck to Spades suit
* Hearts Deck:
	Set Deck to Hearts suit
* Diamonds Deck:
	Set Deck to Diamonds suit
* Red Deck:
	Set Deck to Unspecified Red suit
* Black Deck:
	Set Deck to Unspecified Black suit
* Null Deck:
	Set Deck to Unspecified suit

##Pattern Functions
Returns Pattern

* Any Pattern Pattern ..:
	Accept any of these Deck orders
* Alternating Deck:
	Accept Unspecified Red and Unspecified Black alternating lists
* Same-Suit Deck:
	Duplicate for each suit as separate Deck

##Layout Functions
Returns Layout

* HLayout Layout ..:
	A left to right list of Layouts
* VLayout Layout ..:
	A top to bottom list of Layouts
* GLayout Num Layout ..:
	A VLayout of HLayouts containing Num each
* Apply Feature .. Layout:
	Copy Feature to every Slot contained in Layout
* Clone Num Layout:
	Duplicate the Layout Num times

##Slot Functions
Returns Slot/Layout

* VStack Feature:
	A vertical tableau of cards with all cards visible
* HStack Feature:
	A horizontal tableau of cards with all cards visible
* PStack Feature:
	A Pyrimid tableau of cards starting with the top and stacking downwards
* Slot Feature:
	A single stack of cards with only the top card visible
* Hidden Feature:
	An invisible Slot offscreen
* Stock Feature:
	A slot that whenever clicked sends one card to every dest slot or refils from discard

##Feature Constants
Returns Feature

* Null:
	Empty Feature
* Auto_Grab:
	Can take cards from other layouts
* Auto_Hide:
	Will hide cards after one turn
* Boolean:
	Will only accept a complete pattern
* Dest:
	Can recieve cards from stock
* Discard:
	Cards can be reclaimed for stock
* Goal:
	Game ends when all cards are in goal slots
* Input:
	Cards can not be added
* Output:
	Cards can not be removed
* Start:
	Starts with any leftover cards hidden
* Mirrored:
	Reverse the visual placement of cards

##Feature Functions
Returns Feature

* Add Feature Feature ..:
	Combine Features together into one
* Remove Feature Feature:
	Remove the second Feature from the first
* Max Num:
	Will not store more than Num cards
* Overflow Num:
	Any cards past Num will be hidden
* Filter Pattern:
	Can only store Deck in these orders/arrangements
* Filter_Open Pattern Deck:
	Filter but can start with any card
* Start Num Num:
	Delt Num hidden and Num revealed cards at the beginning

##Feature Logic Functions
Returns Feature

* ID Num:
	Designates Slot Num
* Auto_Fill Num:
	Will take from Slot Num to never be empty
* If_Empty_Lock Num:
	Locks Slot Num contents if self is empty
* Overflow Num Num:
	Any cards past Num will sent to slot Num
