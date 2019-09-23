#include "register_types.h"
#include "core/class_db.h"

#include "game.h"

/*
 * Created by Stuart Irwin on 9/20/2019.
 * Connecting relevant classes to Godot scripting
 */

void register_summator_types() {
    ClassDB::register_class<Game>();
}

void unregister_summator_types() {
   // Nothing to do here in this example.
}