// Idea:
// 1 )	A todo manager. Requests will have a form of operations over the todos,
// 	such as add, remove, change, etc. These requests will be handled
// 	in a chain, such that only one link of the chain will complete the
// 	request or none at all. There is the possibility of user defined
// 	extensions, for example marking a task as completed, but not
// 	removing it, etc.

class ReqHandler {
	#next;
	#data;
	#request;

	constructor(request) {
		this.#request = request;
	}

	/**
	 * Sets the next link in the chain.
	 */
	setNext(handler) {
		this.#next = handler;
	}

	/**
	 * Method checks if the passed request can be handled.
	 * If it can be, then complete the request, otherwise pass
	 * the request down the chain.
	 */
	handle(request, ...args) {
		if (request == this.#request) {
			completeRequest.apply(this, args.unshift(request));
		} else {
			this.#next.handle.apply(
				this.#next, args.unshift(request));
		}
}

exports.ReqHandler = ReqHandler;

