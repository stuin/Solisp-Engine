/*
 * Created by Stuart Irwin on 27/2/2019.
 * Data enums for the solisp system.
 */

//Card face values
enum CardFace {
	null,
	ace,
	two,
	three,
	four,
	five,
	six,
	seven,
	eight,
	nine,
	ten,
	jack,
	queen,
	king,
	joker
};

//Card suit values
enum CardSuit {
	null,
	spades,
	hearts,
	clubs,
	diamonds,
	black,
	red
};

//Function to compare suits
bool operator==(const CardSuit& a, const CardSuit& b);

//Available slot types
enum SlotType {
	null,
	horizontal,
	vertical,
	slot,
	stock,
	pyramid,
	hidden
}

//Available layout types
enum LayoutType {
	null,
	horizontal,
	vertical,
	grid
}

//Available feature types
enum FeatureType {
	null,
	auto_grab,
	auto_hide,
	boolean,
	dest,
	discard,
	goal,
	input,
	output,
	start,
	mirrored,
	max_1,
	overflow_1,
	filter_0,
	filter_open_0,
	start_2,
	id_1,
	auto_fill_1,
	if_empty_lock_1,
	overflow_1_1
}