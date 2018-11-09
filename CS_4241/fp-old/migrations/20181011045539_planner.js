
exports.up = function(knex, Promise) {
  return knex.schema.createTable('planners', (table) => {
      table.increments().primary(); // Serial type, increment (UserId)
      table.integer('UserId').unsigned().index().references('id').inTable('users'); // User 
      table.integer('Hours').unsigned().notNullable();
      table.text('password', 50).notNullable();
      table.text('task', 1000).notNullable();
      table.text('incentive',1000).notNullable();
      table.text('notes', 1000);
      table.timestamps();
  });
};

exports.down = function(knex, Promise) {
    return knex.schema.dropTable('planners');
};
