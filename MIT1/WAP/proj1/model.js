/**
 * TODO JSON format:
 * 	{"todo": [
 *		{"id": 1,
 *		 "date": "12.12.2021",
 *		 "task": "walk the plants"},
 *		{"id": 2,
 *		 "date": "13.12.2021",
 *		 "task": "water the dog"}
 * 	]}
 * 	i.e. an array of objects
 */

let lib = require('./library.js');
let fs = require('fs');

class Todoer extends lib.ReqHandler {
	#file;

	constructor(file) {
		super();
		this.#file = file;
	}

	load() {
		return fs.readFileSync(this.#file, (err, data) => {
			if (err) throw err;});
	}
}

todo = new Todoer('todo.txt');
add = new Todoer('todo.txt');
rem = new Todoer('todo.txt');

todo.setNext(add);
add.setNext(rem);

add.completeRequest = function(op, task) {
	if (op == 'add') {
		data = JSON.parse(this.load());
		if (typeof data === "string") {
			// parse twice if 'overstringified'
			// https://stackoverflow.com/questions/42494823/
			// 	json-parse-returns-string-instead-of-object
			data = JSON.parse(data);
		}

		console.log(data);
	}
}

