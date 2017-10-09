drop table Customer cascade constraints;
create table Customer (
	CustomerID NUMBER(4,0) PRIMARY KEY,
	FirstName VARCHAR2(15),
	LastName VARCHAR2(15),
	Address VARCHAR2(25),
	Phone NUMBER(10,0),
	Age NUMBER(3,0) NOT NULL
);

INSERT INTO Customer VALUES(1, 'Michael', 'Ward', '143 Cambridge Ave.', 5082328798, 45);
INSERT INTO Customer VALUES(2, 'Lisa', 'Gray', '77 Massachusetts Ave.', 6174153059, 68);
INSERT INTO Customer VALUES(3, 'Brian', 'Myers', '175 Forest St.', 2123043923, 19);
INSERT INTO Customer VALUES(4, 'Nicole', 'Ross', '35 Tremont St.', 7818914567, 42);
INSERT INTO Customer VALUES(5, 'Kelly', 'Powell', '100 Main St.', 8915367188, 57);
INSERT INTO Customer VALUES(6, 'Madison', 'Martin', '42 Oak St.', 1233753684, 16);
INSERT INTO Customer VALUES(7, 'Ashley', 'White', '1414 Cedar St.', 6428369619, 18);
INSERT INTO Customer VALUES(8, 'Joshua', 'Clark', '42 Elm Place', 1946825344, 22);
INSERT INTO Customer VALUES(9, 'Tyler', 'Young', '9 Washington Court', 9864752346, 25);
INSERT INTO Customer VALUES(10, 'Anna', 'Powell', '98 Lake Hill Drive', 8946557732, 17);
INSERT INTO Customer VALUES(11, 'Justin', 'Allen', '657 Redondo Ave.', 7988641411, 18);
INSERT INTO Customer VALUES(12, 'Bruce', 'Sanchez', '5 Jefferson Ave.', 2324648888, 26);
INSERT INTO Customer VALUES(13, 'Rachel', 'Lee', '8711 Meadow St.', 2497873464, 67);
INSERT INTO Customer VALUES(14, 'Dylan', 'Garcia', '17 Valley Drive', 9865553232, 20);
INSERT INTO Customer VALUES(15, 'Austin', 'Davis', '1212 8th St.', 4546667821, 29);

