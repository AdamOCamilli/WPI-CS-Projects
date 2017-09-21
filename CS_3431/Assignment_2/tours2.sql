/*
 * Written 9/08/17 by Adam Camilli (aocamilli@wpi.edu)
 * SQL program for CS 3431 Assignment 2
 *
 */

/************************************* PART 1 ***************************************/

/* Drop ReservedTours first to preserve data integrity since it uses IDs of elements in previous three tables as foreign keys. */
DROP TABLE ReservedTours;
DROP TABLE Customers;
DROP TABLE Guides;
DROP TABLE Locations;
DROP TABLE Tours; 

/************************************* PART 2 ***************************************/

CREATE TABLE Customers (
       customerID NUMBER(3),
       firstName VARCHAR2(15),
       lastName	VARCHAR2(20),
       address VARCHAR2(30),
       phone NUMBER(10),
       age NUMBER(3) NOT NULL,
       CONSTRAINT Customers_phone_U UNIQUE (phone),
       CONSTRAINT Customers_PK PRIMARY KEY (customerID)
);

INSERT INTO Customers VALUES(1,  'Michael', 'Davis', '8711 Meadow St.', 2497873464, 67 );
INSERT INTO Customers VALUES(2,  'Lisa', 'Ward', '17 Valley Drive', 9865553232, 20 );
INSERT INTO Customers VALUES(3,  'Brian', 'Gray', '1212 8th St.', 4546667821, 29 );
INSERT INTO Customers VALUES(4,  'Nicole', 'Myers', '9 Washington Court', 9864752346, 18 );
INSERT INTO Customers VALUES(5,  'Kelly', 'Ross', '98 Lake Hill Drive', 8946557732, 26 );
INSERT INTO Customers VALUES(6,  'Madison', 'Powell', '100 Main St.', 8915367188, 57 );
INSERT INTO Customers VALUES(7,  'Ashley', 'Martin', '42 Oak St.', 1233753684, 73 );
INSERT INTO Customers VALUES(8,  'Joshua', 'White', '1414 Cedar St.', 6428369619, 18 );
INSERT INTO Customers VALUES(9,  'Tyler', 'Clark', '42 Elm Place', 1946825344, 22 );
INSERT INTO Customers VALUES(10,  'Anna', 'Young', '657 Redondo Ave.', 7988641411, 25 );
INSERT INTO Customers VALUES(11,  'Justin', 'Powell', '5 Jefferson Ave.', 2324648888, 17 );
INSERT INTO Customers VALUES(12,  'Bruce', 'Allen', '143 Cambridge Ave.', 5082328798, 45 );
INSERT INTO Customers VALUES(13,  'Rachel', 'Sanchez', '77 Massachusetts Ave.', 6174153059, 68 );
INSERT INTO Customers VALUES(14,  'Dylan', 'Lee', '175 Forest St.', 2123043923, 19 );
INSERT INTO Customers VALUES(15,  'Austin', 'Garcia', '35 Tremont St.', 7818914567, 82 );

CREATE TABLE Tours (
       tourID number(2),
       tourName	varchar2(25),
       description varchar2(35),
       city varchar2(25),
       state char(2),
       vehicleType varchar2(10), 
       price number(5,2),
       CONSTRAINT Tours_vehicleType CHECK (vehicleType IN('boat', 'bus', 'car')),
       CONSTRAINT Tours_PK PRIMARY KEY (tourID)
);

INSERT INTO Tours VALUES(1,  'Alcatraz', 'Alcatraz Island', 'San Francisco',  'CA', 'boat', 75.5 );
INSERT INTO Tours VALUES(2,  'Magnificent Mile', 'Tour of Michigan Ave', 'Chicago',  'IL', 'bus', 22.75 );
INSERT INTO Tours VALUES(3,  'Duck Tour', 'Aquatic tour of the Charles River', 'Boston',  'MA', 'boat', 53.99 );
INSERT INTO Tours VALUES(4,  'Freedom Trail', 'Historic tour of Boston', 'Boston',  'MA', 'car', 34.25 );
INSERT INTO Tours VALUES(5,  'NY Museums', 'Tour of NYC Museums', 'New York',  'NY', 'bus', 160.8 );

CREATE TABLE Guides (
       guideID number(2),
       firstName varchar2(15),
       lastName varchar2(20),
       driverLicense number(8) NOT NULL,
       title varchar2(15),
       salary number(7,2),
       licenseType varchar2(10),
       CONSTRAINT Guide_licenseType CHECK (licenseType IN('land', 'sea', 'both')), --land allows driving of buses and cars
       CONSTRAINT Guide_U_driverLicense UNIQUE (driverLicense),
       CONSTRAINT Guide_PK PRIMARY KEY (guideID)
);

INSERT INTO Guides VALUES(1, 'Emily', 'Williams', 74920983, 'Senior Guide', 24125, 'land');
INSERT INTO Guides VALUES(2, 'Ethan', 'Brown', 72930684, 'Guide', 30500 ,'sea');
INSERT INTO Guides VALUES(3, 'Chloe', 'Jones', 50973848, 'Senior Guide', 27044, 'both');
INSERT INTO Guides VALUES(4, 'Ben', 'Miller', 58442323, 'Junior Guide', 32080, 'both');
INSERT INTO Guides VALUES(5, 'Mia', 'Davis', 56719583, 'Junior Guide', 49000, 'land');
INSERT INTO Guides VALUES(6, 'Noah', 'Garcia', 93291234, 'Guide', 22000, 'land');
INSERT INTO Guides VALUES(7, 'Liam', 'Rodriguez', 58799394, 'Junior Guide', 31750, 'sea');
INSERT INTO Guides VALUES(8, 'Mason', 'Wilson', 88314545, 'Senior Guide', 45000, 'land');
INSERT INTO Guides VALUES(9, 'Olivia', 'Smith', 82391452, 'Junior Guide', 25025, 'sea');
INSERT INTO Guides VALUES(10, 'Sofia', 'Johnson', 12930638, 'Guide', 47000, 'both');

CREATE TABLE Locations (
       locationID char(3),
       locationName varchar2(40),
       locationType varchar2(15),
       address varchar2(40),
       tourID number(2),
       CONSTRAINT Locations_tourID_FK FOREIGN KEY (tourID) REFERENCES Tours (tourID) ON DELETE CASCADE,
       CONSTRAINT Locations_PK PRIMARY KEY (locationID)
);

INSERT INTO Locations values('AI1',  'San Francisco Pier 33', ' Historic', 'Pier 33 Alcatraz Landing', 1);
INSERT INTO Locations values('AI2',  'Alcatraz Ferry Terminal', ' Historic', 'Ferry Terminal', 1);
INSERT INTO Locations values('AI3',  'Agave Trail', ' Park', 'Alcatraz Agave Trail', 1);
INSERT INTO Locations values('MM1',  'Art Institute', ' Museum', '111 S Michigan Avenue', 2);
INSERT INTO Locations values('MM2',  'Chicago Tribune', ' Historic', '435 N Michigan Avenue', 2);
INSERT INTO Locations values('MM3',  'White Castle', ' Restaurant', 'S Wabash Avenue', 2);
INSERT INTO Locations values('DT1',  'Charles River', ' Historic', '10 Mass Avenue', 3);
INSERT INTO Locations values('DT2',  'Salt and Pepper Bridge', ' Historic', '100 Broadway', 3); 
INSERT INTO Locations values('FT1',  'Boston Common', ' Park', '139 Tremont Street', 4);
INSERT INTO Locations values('FT2',  'Kings Chapel', ' Historic', '58 Tremont Street', 4);
INSERT INTO Locations values('FT3',  'Omni Parker House', ' Restaurant', '60 School Street', 4);
INSERT INTO Locations values('FT4',  'Paul Revere House', ' Historic', '19 North Square', 4);
INSERT INTO Locations values('FT5',  'Bunker Hill Monument', ' Historic', 'Monument Square', 4);
INSERT INTO Locations values('NY1',  'Metropolitan Museum of Art', ' Museum', '1000 5th Ave', 5);
INSERT INTO Locations values('NY2',  'Museum of Modern Art', ' Museum', '11 W 53rd St', 5);
INSERT INTO Locations values('NY3',  'New York Botanical Garden', ' Park', '2900 Southern Boulevard', 5);
INSERT INTO Locations values('NY4',  'New Museum', ' Museum', '235 Bowery', 5);


/* Like before, make sure to delete ReservedTours first and therefore also create it last to preserve data integrity, since it uses foreign keys to references elements of other tables */
CREATE TABLE ReservedTours (
       reservedTourID number(3),
       travelDate date,
       customerID number(3),
       tourID number(2),
       guideID number(2),
       price number(6,2),
       CONSTRAINT ReservedTours_PK PRIMARY KEY (reservedTourID),
       CONSTRAINT ReservedTours_customerID_FK FOREIGN KEY (customerID) REFERENCES Customers (customerID) ON DELETE SET NULL,
       CONSTRAINT ReservedTours_tourID_FK FOREIGN KEY (tourID) REFERENCES Tours (tourID) ON DELETE SET NULL,
       CONSTRAINT ReservedTours_guideID_FK FOREIGN KEY (guideID) REFERENCES Guides (guideID) ON DELETE SET NULL
);

INSERT INTO ReservedTours VALUES(1, '6-Feb-18', 6, 1, 2, null);
INSERT INTO ReservedTours VALUES(2, '31-Aug-18', 14, 3, 5, null);
INSERT INTO ReservedTours VALUES(3, '10-Apr-19', 11, 4, 1, null);
INSERT INTO ReservedTours VALUES(4, '29-Jul-18', 7, 2, 4, null);
INSERT INTO ReservedTours VALUES(5, '15-Mar-18', 14, 3, 2, null);
INSERT INTO ReservedTours VALUES(6, '28-Feb-19', 12, 4, 6, null);
INSERT INTO ReservedTours VALUES(7, '3-Jun-18', 14, 4, 2, null);
INSERT INTO ReservedTours VALUES(8, '17-May-18', 5, 1, 10, null);
INSERT INTO ReservedTours VALUES(9, '11-Apr-19', 9, 5, 3, null);
INSERT INTO ReservedTours VALUES(10, '24-Nov-18', 13, 4, 9, null);
INSERT INTO ReservedTours VALUES(11, '3-Aug-18', 3, 5, 7, null);
INSERT INTO ReservedTours VALUES(12, '13-Dec-17', 2, 1, 7, null);
INSERT INTO ReservedTours VALUES(13, '9-Nov-17', 4, 5, 1, null);
INSERT INTO ReservedTours VALUES(14, '21-Jan-19', 10, 2, 10, null);
INSERT INTO ReservedTours VALUES(15, '11-Dec-17', 5, 1, 7, null);
INSERT INTO ReservedTours VALUES(16, '12-Aug-18', 1, 3, 5, null);
INSERT INTO ReservedTours VALUES(17, '22-Jun-18', 5, 3, 8, null);
INSERT INTO ReservedTours VALUES(18, '1-Feb-19', 8, 2, 9, null);
INSERT INTO ReservedTours VALUES(19, '15-Oct-17', 15, 4, 8, null);
INSERT INTO ReservedTours VALUES(20, '8-Mar-18', 4, 1, 3, null);

/************************************* PART 3 ***************************************/

/***********************HIGH LEVEL SUMMARY OF THE DATABASE SCHEMA********************

CUSTOMERS(customerID PK, firstName, lastName, address, phone, age)
Tours(tourID PK, tourName, description, city, state, vehicleType, price)
Guides(guideID PK, firstName, lastName, driverLicense, title, salary, licenseType)
Locations(locationID PK, locationName, locationType, address, tourID FK)
ReservedTours(reservedTourID PK, customerID FK, tourID FK, guideID FK, price)

*/

/* Update the ReservedTours prices based on the prices in Tours */

UPDATE ReservedTours
SET ReservedTours.price=(SELECT Tours.price
		 	 FROM Tours
		 	 WHERE (ReservedTours.tourID=Tours.tourID));

/* List for each level of guide - junior guide, guide or senior guide - how many mismatches there are between the required tour’s vehicle type and the guide's license type. 
Sort by the title. */

SELECT Guides.title, COUNT(*) AS mismatches
FROM Guides,Tours,ReservedTours
WHERE ReservedTours.guideID=Guides.guideID
      AND 
      ReservedTours.tourID=Tours.tourID
      AND (  (Tours.vehicleType='boat' AND Guides.licenseType NOT IN ('sea','both'))
             OR
             (Tours.vehicleType IN ('car','bus') AND Guides.licenseType NOT IN ('land','both'))
	  )
GROUP BY Guides.title
ORDER BY Guides.title;

/* For each customer, list the first name, last name, and total amount being spent for land-based tours. Format the price so it is displayed with two decimals and with the heading TotalLandPrice. Sort by 
lastname, and then first name. */

SELECT Customers.firstName, Customers.lastName, to_char(SUM(Tours.price),'999.99') AS TotalLandPrice
FROM Customers,Tours,ReservedTours
WHERE Customers.customerID=ReservedTours.customerID
      AND
      Tours.tourID=ReservedTours.tourID
      AND
      Tours.vehicleType!='boat'
GROUP BY Customers.firstName,Customers.lastName
ORDER BY Customers.lastName,Customers.firstName;

/* Determine the customer who will make the most number of visits to tour locations. List the firstName, lastName, and the number of location visits (use the heading Visits) */

SELECT *
FROM 
     (SELECT Customers.firstName, Customers.lastName, COUNT(Locations.locationID) AS Visits
     FROM Customers,Tours,Locations,ReservedTours
     WHERE (Customers.customerID=ReservedTours.customerID
     	   AND
      	   Tours.tourID=ReservedTours.tourID
      	   AND
      	   Locations.tourID=Tours.tourID)
	   GROUP BY Customers.firstName,Customers.lastName
	   ORDER BY Visits DESC
     )
WHERE ROWNUM = 1;




             
             

