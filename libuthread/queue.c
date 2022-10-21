#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
	void* data;
	node_t next;
};

struct queue {
	node_t front, back;
	unsigned length;
};

node_t create_new_node(void* data) {
	node_t node = (node_t) malloc(sizeof(struct node));

	if (node == NULL) {
		return NULL;
	}

	node->data = data;
	node->next = NULL;
	return node;
}

queue_t queue_create(void)
{
	queue_t queue = (queue_t) malloc(sizeof(struct queue));

	if (queue == NULL) {
		return NULL;
	}

	queue->front = queue->back = NULL;
	queue->length = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->front != NULL) {
		return -1;
	}

	node_t curr_node = queue->front;
	while (curr_node) {
		node_t next_node = curr_node->next;
		free(curr_node);
		curr_node = next_node;
	}

	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL) {
		return -1;
	}

	node_t new_node = create_new_node(data);
	if (new_node == NULL) {
		return -1;
	}

	if (queue->back == NULL) {
		queue->front = queue->back = new_node;
	} else {
		queue->back->next = new_node;
		queue->back = new_node;
	}

	queue->length++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if (queue == NULL || data == NULL || queue->front == NULL) {
		return -1;
	}

	node_t dequeued_node = queue->front;
	queue->front = queue->front->next;

	if (queue->front == NULL) {
		queue->back = NULL;
	}

	queue->length--;
	*data = dequeued_node->data;
	free(dequeued_node);
	return 0;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL || queue->front == NULL) {
		return -1;
	}

	// Node to remove is head of linked list
	if (queue->front->data == data) {
		node_t temp = queue->front;
		queue->front = queue->front->next;
		free(temp);
		queue->length--;
		return 0;
	}

	// Node to remove is is after the head of the linked list
	node_t curr_node = queue->front;
	node_t prev_node = NULL;
	while (curr_node != NULL && curr_node->data != data) {
		prev_node = curr_node;
		curr_node = curr_node->next;
	}

	// Could not find node to remove in linked list
	if (curr_node == NULL) {
		return -1;
	}

	// Remove node from linked list
	node_t temp = curr_node;
	prev_node->next = curr_node->next;

	free(temp);
	queue->length--;
	return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	unsigned processed_idx = 0;
	node_t processed[queue->length * 5]; // Making assumption that queue_func_t wont insert more than 4x current size of queue items
	node_t curr_node = queue->front;

	while (curr_node != NULL) {
		int already_processed = 0;
		for (unsigned i = 0; i < processed_idx; i++) {
			if (curr_node == processed[i]) {
				already_processed = 1;
				break;
			}
		}

		if (already_processed) {
			curr_node = curr_node->next;
			continue;
		}

		processed[processed_idx++] = curr_node;
		(*func)(queue, curr_node->data);
		curr_node = queue->front;
	}

	return 0;
}

int queue_length(queue_t queue)
{
	return (queue == NULL) ? -1 : queue->length;
}