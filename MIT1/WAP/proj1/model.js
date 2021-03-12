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

// Program argument parsing
const opts = (function(argv) {
	const err = -1;
	const argc = argv.length;
	let request;
	let file;
	let data = {};

	if (argc > 6) {
		return err;
	}

	if (argv.includes('-help') || argv.includes('-h')) {
		return err;
	}

	if (argc === 5 && argv[3] === 'add') {
		request = 'add';
		file = argv[2];
		data.text = argv[4];
	} else if (argc === 4 && argv[3] === 'sho') {
		request = 'sho';
		file = argv[2];
	} else if (argc === 5 && argv[3] === 'rem') {
		request = 'rem';
		file = argv[2];
		data.id = parseInt(argv[4]);
	} else if (argc === 6 && argv[3] === 'cng') {
		request = 'cng';
		file = argv[2];
		data.id = parseInt(argv[4]);
		data.text = argv[5];
	} else {
		return err;
	}

	return {request : request,
		file : file,
		data : data};
})(process.argv);

if (opts === -1) {
	// Print help on argument parsing error or help and exit.
	console.log("Usage:\n\tnode model.js file OP [OPARGS]\n");
	console.log("OP [OPARGS]:\n",
		"add text\t:add the text into the file as the next todo\n",
		"rem id\t\t:remove a todo item from file specified by id\n",
		"cng id text\t:change the text of a todo specified by id to text\n",
		"sho\t\t:show all todo items in file\n");
	return;
}

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
todo.sho = new Todoer('sho', opts.file);
todo.add = new Todoer('add', opts.file);
todo.rem = new Todoer('rem', opts.file);
todo.cng = new Todoer('cng', opts.file);

// TODO figure out why the chaining breaks when `sho` is set as first
// 	(the 'End of chain' exception gets thrown)...
// Set the first link in the chain
todo.setFirst(todo.add);

// Set the rest of the "next" links
todo.add.setNext(todo.rem);
todo.rem.setNext(todo.cng);
todo.cng.setNext(todo.sho);

todo.cng.completeRequest = function(request, reqArgs) {
	const id = reqArgs.id;
	const task = reqArgs.text;
	let data;
	try {
		data = this.getData();
	} catch (err) {
		console.log("File does not exist.");
		return;
	}

	let pos = data.map(function(e) { return e.id; }).indexOf(id);

	try {
		data[pos].task = task;
	} catch (err) {
		console.log("id out of range");
		return;
	}

	this.writeData(data);
}

todo.rem.completeRequest = function(request, reqArgs) {
	const id = reqArgs.id;
	let data;
	try {
		data = this.getData();
	} catch (err) {
		console.log("File does not exist.");
		return;
	}


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

todo.sho.completeRequest = function(request, reqArgs) {
	let data;
	try {
		data = this.getData();
	} catch (err) {
		// TODO in each try/catch block in completeRequest - check
		// 	if the err is thrown because of a file error or a JSON
		// 	error
		console.log("File does not exist.");
		return;
	}

	for (const item of data) {
		console.log(item);
	}
}

todo.add.completeRequest = function(request, reqArgs) {
	const task = reqArgs.text;
	let data = [];
	try {
		data = this.getData();
	} catch (err) {
		if (err.code === 'ENOENT') {
			// File not found -> create new one
			fs.writeFileSync(this.file, '', function (err) {
				if (err) throw err;});
		} else {
			throw err;
		}
	}

	data.push({id : data.length + 1, task : task });

	this.writeData(data);
}

