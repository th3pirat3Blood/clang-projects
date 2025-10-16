/*
 * Sample code for testing Source Code Information Getter
 */ 

#include <iostream>

using namespace std;

// Node structure
struct Node {
	int data;
	struct Node *next;

	Node() : data(0), next(nullptr) {}
	Node(int data, struct Node *next) : data(data), next(next) {}
};


// Function to display the nodes in link list
void display(struct Node *head) {
	struct Node *temp = head;
	while (temp!=nullptr) {
		cout << temp->data << " ";
		temp = temp->next;
	}
	cout << endl;
}

int main() {
	struct Node *head;

	/*
	 *	Creating the first node, then connecting all successding nodes with 
	 *	previous nodes for creating a link list 
	 */ 
	head = new Node();
	head = new Node (1, head);
	head = new Node (2, head);
	head = new Node (3, head);
	head = new Node (4, head);
	head = new Node (5, head);

	display(head);

	return 0;
}

