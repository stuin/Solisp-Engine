#include "structures.h"

/*
 * Created by Stuart Irwin on 26/2/2019.
 * Basic list structure.
 */

//Copy whole list
ListNode::ListNode(const ListNode &source) {
	//Copy next in list
	if(source.next != NULL) {
		ListNode node = *(source.next);
		next = &node;
	}

	//Copy node values
	copy(source);
}

//Function to copy values and be ovewritten
void ListNode::copy(const ListNode &source) {
	
}

//Delete whole list
void ListNode::remove() {
	if(next != NULL) {
		*next.remove();
	}
	delete *this;
}

//Copy whole list from head
List::List(const List &source) {
	if(source.head != NULL) {
		ListNode node = *(source.head);
		head = &node;
	}
}

//Append lists together
void List::operator+=(const List& second) {
	ListNode **node = &(second->head);

	while(*node != NULL) {
		node = &(node->next);
	}

	*node = head;
	head = second->head;
}

//Add single object to list
void List::operator+=(const ListNode& node) {
	node->next = head;
	head = node;
}	