"use strict";
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

/**
 * Link 1:
 * 	https://stackoverflow.com/questions/42494823/json-parse-returns-string-instead-of-object
 * Link 2:
 * 	https://stackoverflow.com/questions/8668174/indexof-method-in-an-object-array
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

	getData() {
		// TODO add check if the file exists / if the JSON in it is valid
		const data = JSON.parse(this.load());
		if (typeof data === "string") {
			// parse twice if 'overstringified'
			// see Link 1
			data = JSON.parse(data);
		}

		return data;
	}

	writeData(data) {
		data = JSON.stringify(data);
		fs.writeFileSync(this.file, data, (err) => {if (err) throw err;});
	}
	
	/**
	 * @brief Take todo data and reset their task IDs (for example after
	 * a task removal).
	 *
	 * @param data an array of todo item objects.
	 * @returns the same data array, only with reset IDs.
	 */
	resetIDs(data) {
		// TODO maybe try to define this only for items from a certain
		// 	ID, as, for example after removing, only the higher
		// 	ID numbers will be out of order - possible timesaver
		let i = 1;
		for (let item of data) {
			item.id = i;
			i++;
		}

		return data;
	}
}

// The beginning of the chain 
let todo = new lib.Chain();

// Create the chain link objects
// TODO change the filename from being hardcoded to user-specified
todo.sho = new Todoer('sho', 'todo.txt');
todo.add = new Todoer('add', 'todo.txt');
todo.rem = new Todoer('rem', 'todo.txt');
todo.cng = new Todoer('cng', 'todo.txt');

// TODO figure out why the chaining breaks when `sho` is set as first
// 	(the 'End of chain' exception gets thrown)...
// Set the first link in the chain
todo.setFirst(todo.add);

// Set the rest of the "next" links
todo.add.setNext(todo.rem);
todo.rem.setNext(todo.cng);
todo.cng.setNext(todo.sho);

todo.cng.completeRequest = function(request, id, task) {
	let data = this.getData();

	let pos = data.map(function(e) { return e.id; }).indexOf(id);

	try {
		data[pos].task = task;
	} catch (err) {
		console.log("id out of range");
		return;
	}

	this.writeData(data);
}

todo.rem.completeRequest = function(request, id) {
	let data = this.getData();

	// see Link 2
	let pos = data.map(function(e) { return e.id; }).indexOf(id);

	if (pos >= data.length || pos < 0) {
		console.log("invalid id to be removed");
		return;
	}

	data.splice(pos, 1);
	
	data = this.resetIDs(data);
	this.writeData(data);
}

todo.sho.completeRequest = function() {
	const data = this.getData();

	for (const item of data) {
		console.log(item);
	}
}

todo.add.completeRequest = function(request, task) {
	let data = this.getData();

	data.push({id : data.length + 1, task : task });

	this.writeData(data);
}

