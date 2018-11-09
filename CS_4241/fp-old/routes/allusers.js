var express = require('express');
var router = express.Router();

const knex = require('../db/knex'); // Connection to database

/* GET home page (mounted at /allusers) */
router.get('/', function(req, res) {
    knex('users')
	.select()
	.then(users =>
	      {
		  res.render('allusers', { users: users });
	      });
});

module.exports = router;
