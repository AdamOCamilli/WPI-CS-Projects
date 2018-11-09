
exports.up = function(knex, Promise) {
    return knex.schema.createTable('users', (table) => {
	table.increments().primary(); // Serial type, increment (UserId)
	table.text('username', 50).unique().notNullable();
	table.text('password', 50).notNullable();
	table.text('firstname', 50).notNullable();
	table.text('lastname', 50).notNullable();
	table.text('email', 200).unique().notNullable();
	table.timestamps();
    });
};

exports.down = function(knex, Promise) {
    return knex.schema.dropTable('users');
};
