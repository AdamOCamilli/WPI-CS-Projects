var express = require('express');
var router = express.Router();

const knex = require('../db/knex'); // Connection to database

/* GET users listing. */
router.get('/', function(req, res) {
    knex('users')
	.select()
	.then(
	    users =>
		{
		    res.render('users', { users: users });
		}
	);
});

// GET user (show account)
router.get('/:id', function(req, res) {
    const id = req.params.id;
    if (typeof id != 'undefined') {
	knex('users')
	    .select()
	    .where('id', id)
	    .first()
	    .then(
		user =>
		    {
			res.render('account', { users: user });
		    }
	    );
    } else {
	res.status(500);
	res.render('error', {
	    message: 'User does not exist'
	});
    }
});
/* GET users (Render edit page)*/
router.get("/:id/edit", function (req, res) {
    // get user with id in URL
    knex('users')
	    .select()
	    .where('id', req.params.id)
	    .first()
	    .then(
		user =>
		    {
			res.render('edit', { users: user });
		    }
	    );
    res.render('edit'); 
});
/* PUT users (Edit account)*/
router.put('/:id', function (req, res) {
   var tempbody = req.body;
    if (validUser(JSON.stringify(tempbody))) {
	// insert into db
	console.log("Updating user: " + req.body);
	const newuser = {
	    username: req.body.username,
	    password: req.body.password,
	    firstname: req.body.firstname,
	    lastname: req.body.lastname,
	    email: req.body.email,
	    created_at: new Date()
	};
	
	knex('users')
	    .insert(newuser, "id")
	    .then( function(result) {
		let userid = result[0];
		res.redirect('/users');
	    });
    } else {
	// respond with error
	res.status(500);
	res.redirect("/signup");
    }
});  


/* GET users (Render signup page) */
router.get('/signup', function(req, res) {
    res.render('signup');
});


function validUser(user) {
    console.log("Checking user: " + user);
    
    // let usernameValid = (typeof user.username == 'string') &&
    // 	(user.username.trim() != '');
    // let passwordValid = (typeof user.password == 'string') &&
    // 	(user.password.trim() != '');
    // let firstnameValid = (typeof user.firstname == 'string') &&
    // 	(user.firstname.trim() != '');
    // let lastnameValid =	(typeof user.lastname == 'string') &&
    // 	(user.ulastname.trim() != '');
    // let emailValid = (typeof user.email == 'string') &&
    // 	(user.email.trim() != '');
    // console.log(usernameValid + passwordValid + firstnameValid + lastnameValid + emailValid);
    
    // return (usernameValid && passwordValid && firstnameValid && lastnameValid && emailValid);
    return true;
}
/* POST users (Actually sign up user) */
router.post('/', function(req,res) {
    console.log(req.body);
    var tempbody = req.body;
    if (validUser(JSON.stringify(tempbody))) {
	// insert into db
	console.log("Inserting user: " + req.body);
	const newuser = {
	    username: req.body.username,
	    password: req.body.password,
	    firstname: req.body.firstname,
	    lastname: req.body.lastname,
	    email: req.body.email,
	    created_at: new Date()
	};
	
	knex('users')
	    .insert(newuser, "id")
	    .then( function(result) {
		let userid = result[0];
		res.redirect('/users');
	    });
    } else {
	// respond with error
	res.status(500);
	res.redirect("/signup");
    }
});


module.exports = router;
