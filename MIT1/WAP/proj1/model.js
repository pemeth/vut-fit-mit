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

class Todoer extends lib.Link {
	file;

	constructor(request, file) {
		super(request);
		this.file = file;
	}

	load() {
		return fs.readFileSync(this.file, (err, data) => {
			if (err) throw err;});
	}
}

// The beginning of the chain 
todo = new lib.Chain();

// Create the chain link objects
// TODO change the filename from being hardcoded to user-specified
todo.add = new Todoer('add', 'todo.txt');
todo.rem = new Todoer('rem', 'todo.txt');
todo.cng = new Todoer('cng', 'todo.txt');

// Set the first link in the chain
todo.setFirst(todo.add);

// Set the rest of the "next" links
todo.add.setNext(todo.rem);
todo.rem.setNext(todo.cng);

todo.add.completeRequest = function(request, task) {
	// TODO add check if the file exists / if the JSON in it is valid
	data = JSON.parse(this.load());
	if (typeof data === "string") {
		// parse twice if 'overstringified'
		// https://stackoverflow.com/questions/42494823/
		// 	json-parse-returns-string-instead-of-object
		data = JSON.parse(data);
	}

	data.push({id : data.length + 1, task : task });
	data = JSON.stringify(data);
	fs.writeFileSync(this.file, data, (err) => {if (err) throw err;});
}

