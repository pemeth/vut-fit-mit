/* 
 * TODO: recycle the ZOO project from IUS - chain of responsibility of
 *	individual employees
 */

class Employee {
	#name;
	#job;
	#nextInChain;

	constructor(name,job) {
		this.#name = name;
		this.#job = job;
	}

	setNext(next) {
		this.#nextInChain = next;
	}

	/**
	 * Check if the employee is right for the job. If yes, finish() the job,
	 * otherwise send down the chain.
	 */
	workOn(job, ...args) {
		args.unshift(job)

		if (job == this.#job) {
			this.finish.apply(this, args);
		} else {
			if (this.#nextInChain == undefined) {
				throw Error("End of chain - can't finish task");
			}
			this.#nextInChain.workOn.apply(this.#nextInChain, args)
		}
	}

	/**
	 * An "abstract" method to be implemented for each unique employee type.
	 */
	finish(job, args) {
		throw Error("Method not implemented");
	}
}

class Ceo extends Employee {
	constructor(name, job) {
		super(name, job);
	}
}

class Bureaucrat extends Employee {
	constructor(name, job) {
		super(name, job);
	}
}

class Caretaker extends Employee {
	constructor(name, job) {
		super(name, job);
	}
}

class Feeder extends Employee {
	constructor(name, job) {
		super(name, job);
	}
}

/**
 * TODO: idea for a pool of employees of same rank, which could pass tasks 
 * to each other based on whether or not they were busy (or something else)
 */
class EmployeePool {
	#pool = [];

	pushEmp(emp) {
		if (emp instanceof Employee) {
			this.#pool.push(emp);
		} else {
			throw TypeError('Expected type Employee');
		}
	}
}

