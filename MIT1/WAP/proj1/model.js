"use strict";
/**
 * @author Patrik Nemeth xnemet04
 */

/**
 * Link 1:
 * 	https://stackoverflow.com/questions/42494823/json-parse-returns-string-instead-of-object
 * Link 2:
 * 	https://stackoverflow.com/questions/8668174/indexof-method-in-an-object-array
 */

let lib = require('./library.js');
let fs = require('fs');

/**
 * Program argument parsing.
 *
 * `opts` will contain either:
 * - an int of -1 on error or help
 * - an object of the following form
 *   {
 *   request : str,
 *   file : str,
 *   data : {
 *   	id : int,
 *   	text : str
 *   	}
 *   }
 */
const opts = (function(argv) {
	const err = -1;
	const argc = argv.length;
	let request;
	let file;
	let data = {};

	if (argc > 6) {
		return err;
	}

	if (argv.includes('--help') || argv.includes('-h')) {
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
	console.log("Usage:\n\tnode model.js file OP\n");
	console.log("OP can be either of:\n",
		" add {text}\tadd the text into the file as the next todo\n",
		" rem {id}\tremove a todo item from file specified by id\n",
		" cng {id} {text}\tchange the text of a todo specified by id to text\n",
		" sho\t\tshow all todo items in file\n");
	console.log("NOTE:\n",
		" Multi-word text must be given in single-quotes (i.e. 'water the dog').");
	return;
}

/**
 * A class with methods and attributes for the todo operations.
 *
 * Handles file reading and writing, JSON parsing and completing
 * the requests sent down the chain of responsibility - acts as the
 * links in the chain.
 */
class Todoer extends lib.Link {
	file;	//!< Holds the filename, with which the user wants to work

	constructor(request, file) {
		super(request);
		this.file = file;
	}

	/**
	 * Loads all of the contents of `this.file`.
	 * @returns the raw contents of `this.file`.
	 */
	load() {
		return fs.readFileSync(this.file, (err, data) => {
			if (err) throw err;});
	}

	/**
	 * Returns `JSON.parse`d data from `this.file`.
	 * @returns the parsed JSON data as an ES object.
	 */
	getData() {
		const data = JSON.parse(this.load());
		if (typeof data === "string") {
			// parse twice if 'overstringified'
			// see Link 1
			data = JSON.parse(data);
		}

		return data;
	}

	/**
	 * Takes the `data`, stringifies it and saves it to `this.file`.
	 * @param data the data object to be saved.
	 */
	writeData(data) {
		data = JSON.stringify(data);
		fs.writeFileSync(this.file, data, (err) => {if (err) throw err;});
	}

	/**
	 * Prints appropriate messages based on exception `err`.
	 * Used in a `catch` block after `try`ing `getData`. If an unknown
	 * exception is passed, it is rethrown.
	 * @param err an exception for which a message should be printed.
	 */
	getDataErrPrinter(err) {
		if (err.code === 'ENOENT') {
			console.log("File does not exist.");
		} else if (err.name === 'SyntaxError') {
			console.log("Invalid JSON syntax in: ", this.file);
		} else {
			throw err;
		}
	}
	
	/**
	 * Take todo data and reset their task IDs (for example after
	 * a task removal).
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

// Set the first link in the chain
todo.setFirst(todo.add);

// Set the rest of the "next" links
todo.add.setNext(todo.rem);
todo.rem.setNext(todo.cng);
todo.cng.setNext(todo.sho);

/**
 * Change a todo item's text. The item is identified by `reqArgs.id`
 * and the text is specified in `reqArgs.text`.
 * @param request the request id.
 * @param reqArgs arguments for the request.
 */
todo.cng.completeRequest = function(request, reqArgs) {
	const id = reqArgs.id;
	const task = reqArgs.text;
	let data;
	try {
		data = this.getData();
	} catch (err) {
		this.getDataErrPrinter(err);
		return;
	}

	let pos = data.map(function(e) { return e.id; }).indexOf(id);

	try {
		data[pos].task = task;
	} catch (err) {
		console.log("Item ID out of range");
		return;
	}

	this.writeData(data);

	console.log("Item", id, "changed to:", task);
}

/**
 * Remove a todo item identified by `reqArgs.id`.
 * @param request the request id.
 * @param reqArgs arguments for the request.
 */
todo.rem.completeRequest = function(request, reqArgs) {
	const id = reqArgs.id;
	let data;
	try {
		data = this.getData();
	} catch (err) {
		this.getDataErrPrinter(err);
		return;
	}

	// see Link 2
	let pos = data.map(function(e) { return e.id; }).indexOf(id);

	if (pos >= data.length || pos < 0) {
		console.log("Invalid item ID to be removed");
		return;
	}

	data.splice(pos, 1);
	
	data = this.resetIDs(data);
	this.writeData(data);

	console.log("Item", id, "removed");
}

/**
 * Show all todo items.
 * @param request the request id.
 * @param reqArgs arguments for the request.
 */
todo.sho.completeRequest = function(request, reqArgs) {
	let data;
	try {
		data = this.getData();
	} catch (err) {
		this.getDataErrPrinter(err);
		return;
	}

	if (data.length === 0) {
		console.log("Todo list empty");
		return;
	}

	for (const item of data) {
		console.log(item);
	}
}

/**
 * Add a new todo item with text `reqArgs.text`.
 * @param request the request id.
 * @param reqArgs arguments for the request.
 */
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
		} else if (err.name === 'SyntaxError') {
			console.log("Invalid JSON syntax in: ", this.file);
			return;
		} else {
			throw err;
		}
	}

	const id = data.length + 1;
	data.push({id : id, task : task });
	
	this.writeData(data);

	console.log("Todo item", id, "added");
}

// Run the chain
todo.handle(opts.request, opts.data);
