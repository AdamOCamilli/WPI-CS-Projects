
exports.seed = function(knex, Promise) {
  // Deletes ALL existing entries
  return knex('users').del()
	.then(function () {
	    const users = [
		{
		    username: 'jsmith',
		    password: 'badhash1',
		    firstname: 'John',
		    lastname:  'Smith',
		    email: 'thejsmith@example.com',
		},
		{
		    username: 'jdoe',
		    password: 'badhash2',
		    firstname: 'John',
		    lastname:  'Doe',
		    email: 'jdoe@alternative.com',
		}
	    ];

	    return knex('users').insert(users);
	    
	});
};
