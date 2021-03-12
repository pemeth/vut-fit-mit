"use strict";
/**
 * @author Patrik Nemeth xnemeth04
 */

/**
 * A link in the chain of responsibility.
 */
class Link {
	next;	//!< The next link in the chain
	data;	//!< Miscellaneous data
	request;	//!< The request, which this object should handle

	constructor(request) {
		this.request = request;
	}

	/**
	 * Sets the next link in the chain.
	 * @param handler the handler to be set as next in the chain.
	 */
	setNext(handler) {
		this.next = handler;
	}

	/**
	 * Method checks if the passed request can be handled.
	 * If it can be, then complete the request, otherwise pass
	 * the request down the chain.
	 * @param request the request identifier.
	 * @param ...args additional arguments to be passed to `completeRequest`.
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
	 * of responsibility. The ...args are possible additional arguments
	 * if the library user needs additional arguments for the chain links.
	 * This method must be implemented by the library user.
	 * @param request the request identifier.
	 * @param ...args additional arguments.
	 */
	completeRequest(request, ...args) {
		throw 'Method not implemented!';
	}
}

/**
 * A container/wrapper for the linked chain of objects.
 */
class Chain {
	#first;	//!< Holds the first link in the chain

	/**
	 * Set the first link in the chain. Should be an object of
	 * class `Link` or it's derivative.
	 * @param first the first link in the chain.
	 */
	setFirst(first) {
		this.#first = first;
	}

	/**
	 * This starts the walk down the chain.
	 * @param request the request identifier.
	 * @param ...args optional arguments.
	 */
	handle(request, ...args) {
		args.unshift(request);
		this.#first.handle.apply(this.add, args);
	}
}

exports.Link = Link;
exports.Chain = Chain;

