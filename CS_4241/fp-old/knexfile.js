// Update with your config settings.

require('dotenv').config(); // Get env variables

module.exports = {

    development: {
	client: 'postgresql',
	debug: true,
	connection: process.env.DATABASE_URL + "?ssl=true",
	migrations: {
	    tableName: 'migrations'
	},
	ssl: true
    },
    production: {
	client: 'postgresql',
	debug: true,
	connection: process.env.DATABASE_URL + "?ssl=true",
	migrations: {
	    tableName: 'migrations'
	},
	ssl: true
    }
    

};
