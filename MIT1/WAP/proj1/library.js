"use strict";
// Idea:
// 1 )	A todo manager. Requests will have a form of operations over the todos,
// 	such as add, remove, change, etc. These requests will be handled
// 	in a chain, such that only one link of the chain will complete the
// 	request or none at all. There is the possibility of user defined
// 	extensions, for example marking a task as completed, but not
// 	removing it, etc.

class Link {
	next;
	data;
	request;

	constructor(request) {
		this.request = request;
	}

	/**
	 * Sets the next link in the chain.
	 */
	setNext(handler) {
		this.next = handler;
	}

	/**
	 * Method checks if the passed request can be handled.
	 * If it can be, then complete the request, otherwise pass
	 * the request down the chain.
	 */
	handle(request, ...args) {
		args.unshift(request);

		if (request == this.request) {
			this.completeRequest.apply(this, args);
		} else {
			if (!this.next) {
				throw 'End of chain';
				return;
			}
			this.next.handle.apply(this.next, args);
		}
	}

	/**
	 * Method for completing specific requests. Needs to be overridden
	 * by the user-defined chain links, as this is the function, which
	 * is called to complete the requests sent down the chain
	 * of responsibility.
	 */
	completeRequest(request, ...args) {
		throw 'Method not implemented!';
	}
}

/**
 * A container/wrapper for the linked chain of objects.
 */
class Chain {
	#first;

	/**
	 * Set the first link in the chain. Should be an object of
	 * class `Link` or it's derivative.
	 */
	setFirst(first) {
		this.#first = first;
	}

	/**
	 * This starts the walk down the chain.
	 */
	handle(request, ...args) {
		args.unshift(request);
		this.#first.handle.apply(this.add, args);
	}
}

exports.Link = Link;
exports.Chain = Chain;

