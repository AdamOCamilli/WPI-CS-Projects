DROP TABLE Guide CASCADE CONSTRAINTS;
CREATE TABLE Guide (
	DriverLicense	number(3,0) primary key,
	FirstName	varchar2(15) not null,
	LastName	varchar2(15) not null,
	Phone		number(10,0),
	VehicleType	varchar2(10),
	Title		varchar2(15),
	Salary		number(5,0),
	HireDate	date,
	Constraint Guide_vtype check (VehicleType in ('bus', 'car', 'amphibious')),
	Constraint Guide_titletype check (Title in ('Junior Guide', 'Guide', 'Senior Guide'))
);
INSERT INTO Guide VALUES(1, 'Noah', 'Smith', 5082391452, 'bus', 'Junior Guide', 22000, '18-Jan-15');
INSERT INTO Guide VALUES(2, 'Liam', 'Johnson', 7812930638, 'bus', 'Guide', 31000, '1-Feb-14');
INSERT INTO Guide VALUES(3, 'Mason', 'Williams', 6174920983, 'car', 'Senior Guide', 55000, '9-Sep-15');
INSERT INTO Guide VALUES(4, 'Olivia', 'Brown', 6672930684, 'amphibious', 'Guide', 32000, '13-Feb-16');
INSERT INTO Guide VALUES(5, 'Sofia', 'Jones', 4150973848, 'bus', 'Senior Guide', 59000, '28-Dec-15');
INSERT INTO Guide VALUES(6, 'Emily', 'Miller', 3058442323, 'car', 'Junior Guide', 25000, '17-Nov-15');
INSERT INTO Guide VALUES(7, 'Ethan', 'Davis', 4156719583, 'bus', 'Junior Guide', 24000, '7-Jun-14');
INSERT INTO Guide VALUES(8, 'Chloe', 'Garcia', 8193291234, 'amphibious', 'Guide', 30000, '3-Mar-15');
INSERT INTO Guide VALUES(9, 'Ben', 'Rodriguez', 8458799394, 'bus', 'Junior Guide', 27000, '24-Oct-14');
INSERT INTO Guide VALUES(10, 'Mia', 'Wilson', 5088314545, 'car', 'Senior Guide', 57000, '11-Nov-14');


DROP TABLE Vehicle CASCADE CONSTRAINTS;
CREATE TABLE Vehicle (
	LicensePlate	char(3) primary key,
	VehicleType	varchar2(15) not null,
	Make		varchar2(10),
	Model		varchar2(10),
	Year		number(4,0),
	MaxPassenger	number(2,0),
	Constraint Vehicle_vtype check (VehicleType in ('bus', 'car', 'amphibious'))
);
INSERT INTO Vehicle VALUES('A1K', 'bus', 'Ford', 'Tourneo', 2010, 22);
INSERT INTO Vehicle VALUES('B2C', 'amphibious', 'Hydra', 'Terra', 2014, 40);
INSERT INTO Vehicle VALUES('C3T', 'bus', 'Kia', 'Combi', 2015, 18);
INSERT INTO Vehicle VALUES('D4P', 'car', 'Honda', 'Civic', 2004, 4);
INSERT INTO Vehicle VALUES('E5R', 'bus', 'AEC', 'Cougar', 2012, 8);


DROP TABLE Tour CASCADE CONSTRAINTS;
CREATE TABLE Tour (
	TourID		number(3,0) primary key,
	TourName	varchar2(25) not null, 
	Description	varchar2(40),
	City		varchar2(10) not null,
	State		char(2) not null,
	Duration	varchar(12),
	VehicleType	varchar2(15),
	AdultCost	number(3,0),
	ChildCost	number(3,0),
	Constraint Tour_vtype check (VehicleType in ('bus', 'car', 'amphibious'))
);
INSERT INTO Tour VALUES(1, 'Duck Tour', 'Amphibious tour of the Charles River', 'Boston', 'MA', '1 hour', 'amphibious', 100, 50);
INSERT INTO Tour VALUES(2, 'Freedom Trail', 'Historic tour of Boston', 'Boston', 'MA', '4 hours', 'bus', 100, 50);
INSERT INTO Tour VALUES(3, 'Magnificent Mile Tour', 'Tour of Michigan Ave', 'Chicago', 'IL', '90 minutes', 'car', 100, 50);
INSERT INTO Tour VALUES(4, 'City Sights', 'Highlights of New York City', 'New York', 'NY', '2 hours', 'bus', 100, 50);


DROP TABLE Location CASCADE CONSTRAINTS;
CREATE TABLE Location (
	LocationID	char(3) primary key,
	LocationName	varchar2(25) not null,
	Type		varchar2(10),
	Address		varchar2(25),
	Longitude	number(4,2),
	Latitude	number(4,2),
	Constraint LocationType check (Type in ('Historic', 'Museum', 'Restaurant'))
);
INSERT INTO Location VALUES('DT1', 'Charles River', 'Historic', '10 Mass Avenue', -71.09, 42.35);
INSERT INTO Location VALUES('DT2', 'Salt and Pepper Bridge', 'Historic', '100 Broadway', -74.01, 40.71);
INSERT INTO Location VALUES('FT1', 'Boston Common', 'Historic', '139 Tremont St', -71.07, 42.35);
INSERT INTO Location VALUES('FT2', 'Kings Chapel', 'Historic', '58 Tremont St.', -71.06, 42.36);
INSERT INTO Location VALUES('FT3', 'Omni Parker House', 'Restaurant', '60 School St', -71.05, 42.36);
INSERT INTO Location VALUES('FT4', 'Paul Revere House', 'Historic', '19 North Square', -71.05, 42.37);
INSERT INTO Location VALUES('FT5', 'Bunker Hill Monument', 'Historic', 'Monument Square', -71.06, 42.38);
INSERT INTO Location VALUES('MM1', 'Art Institute', 'Museum', '111 S Michigan Ave', -87.62, 41.88);
INSERT INTO Location VALUES('MM2', 'Chicago Tribune', 'Historic', '435 N Michigan Ave', -87.62, 41.89);
INSERT INTO Location VALUES('MM3', 'White Castle', 'Restaurant', 'S Wabash Ave', -87.62, 41.77);
INSERT INTO Location VALUES('NY1', 'Freedom Tower', 'Historic', '285 Fulton St', -74.01, 40.71);
INSERT INTO Location VALUES('NY2', 'Le Bernardin', 'Restaurant', '155 W 51 St', -73.98, 40.76);


DROP TABLE Traveling CASCADE CONSTRAINTS;
CREATE TABLE Traveling (
	TourID		number(3,0),
	LocationID	char(3),
	Constraint Traveling_PK Primary Key (TourID, LocationID),
	Constraint TravelingTour_FK Foreign Key (TourID) References Tour(TourID),
	Constraint TravelingLocation_FK Foreign Key (LocationID) References Location(LocationID)
);
INSERT INTO Traveling VALUES(1, 'DT1');
INSERT INTO Traveling VALUES(1, 'DT2');
INSERT INTO Traveling VALUES(1, 'FT3');
INSERT INTO Traveling VALUES(2, 'FT1');
INSERT INTO Traveling VALUES(2, 'FT2');
INSERT INTO Traveling VALUES(2, 'FT3');
INSERT INTO Traveling VALUES(2, 'FT4');
INSERT INTO Traveling VALUES(2, 'DT2');
INSERT INTO Traveling VALUES(2, 'FT5');
INSERT INTO Traveling VALUES(3, 'MM1');
INSERT INTO Traveling VALUES(3, 'MM2');
INSERT INTO Traveling VALUES(3, 'MM3');
INSERT INTO Traveling VALUES(4, 'NY1');
INSERT INTO Traveling VALUES(4, 'NY2');


DROP TABLE Customer CASCADE CONSTRAINTS;
CREATE TABLE Customer (
	CustomerID	number(3,0) primary key,
	FirstName	varchar2(15) not null,
	LastName	varchar2(15) not null,
	Address		varchar2(20),
	Phone		number(10,0),
	Age		number(3,0) not null
);
INSERT INTO Customer VALUES(1, 'Michael', 'Ward', '143 Cambridge St.', 5082328798, 45);
INSERT INTO Customer VALUES(2, 'Lisa', 'Gray', '77 Mass Ave.', 6174153059, 68);
INSERT INTO Customer VALUES(3, 'Brian', 'Myers', '175 Forest St.', 2123043923, 34);
INSERT INTO Customer VALUES(4, 'Nicole', 'Ross', '35 Tremont St.', 7818914567, 42);
INSERT INTO Customer VALUES(5, 'Kelly', 'Powell', '100 Main St.', 8915367188, 57);


DROP TABLE TravelingWith CASCADE CONSTRAINTS;
CREATE TABLE TravelingWith (
	TravelingWithID	char(3),
	CustomerID	number(3,0),
	FirstName	varchar2(15) not null,
	LastName	varchar2(15) not null,
	Age		number(3,0) not null,
	Constraint TravelingWith_PK Primary Key (TravelingWithID, CustomerID),
	Constraint TravelingWithCustomerID_FK Foreign Key (CustomerID) References Customer(CustomerID)
);
INSERT INTO TravelingWith VALUES('KP1', 5, 'Jacob', 'Powell', 5);
INSERT INTO TravelingWith VALUES('BM1', 3, 'Madison', 'Martin', 16);
INSERT INTO TravelingWith VALUES('KP2', 5, 'Ashley', 'White', 18);
INSERT INTO TravelingWith VALUES('BM2', 3, 'Joshua', 'Clark', 22);
INSERT INTO TravelingWith VALUES('MW1', 1, 'Tyler', 'Young', 25);
INSERT INTO TravelingWith VALUES('KP3', 5, 'Anna', 'Powell', 13);
INSERT INTO TravelingWith VALUES('MW2', 1, 'Justin', 'Allen', 18);
INSERT INTO TravelingWith VALUES('KP4', 5, 'Bruce', 'Sanchez', 45);
INSERT INTO TravelingWith VALUES('MW3', 1, 'Rachel', 'Lee', 67);
INSERT INTO TravelingWith VALUES('NR1', 4, 'Dylan', 'Garcia', 78);
INSERT INTO TravelingWith VALUES('LG1', 2, 'Austin', 'Davis', 13);
INSERT INTO TravelingWith VALUES('KP5', 5, 'Sydney', 'Powell', 34);
INSERT INTO TravelingWith VALUES('LG2', 2, 'Haley', 'Miller', 9);
INSERT INTO TravelingWith VALUES('NR2', 4, 'Jose', 'Allen', 14);


DROP TABLE BookedTour CASCADE CONSTRAINTS;
CREATE TABLE BookedTour (
	BookedTourID	number(3,0) primary key,
	PurchaseDate	date not null,
	TravelDate	date not null,
	TotalPrice	number(3,0),
	TourID		number(3,0) not null,
	DriverLicense	number(3,0) not null,
	LicensePlate	char(3) not null,
	CustomerID	number(3,0) not null,
	Constraint BookedTourTourID_FK Foreign Key (TourID) References Tour(TourID),
	Constraint BookedTourDriverLicense_FK Foreign Key (DriverLicense) References Guide (DriverLicense),
	Constraint BookedTourLicensePlate_FK Foreign Key (LicensePlate) References Vehicle(LicensePlate),
	Constraint BookedTourCustomerID_FK Foreign Key (CustomerID) References Customer(CustomerID)
);
INSERT INTO BookedTour VALUES(1, '5-Jan-16', '15-Mar-16', 0, 2, 5, 'A1K', 5);
INSERT INTO BookedTour VALUES(2, '23-Dec-14', '28-Feb-16', 0, 4, 1, 'C3T', 1);
INSERT INTO BookedTour VALUES(3, '19-Feb-16', '3-Jun-16', 0, 1, 4, 'B2C', 5);
INSERT INTO BookedTour VALUES(4, '30-Nov-15', '17-May-16', 0, 4, 2, 'C3T', 4);
INSERT INTO BookedTour VALUES(5, '2-Feb-16', '11-Apr-16', 0, 2, 9, 'E5R', 3);
INSERT INTO BookedTour VALUES(6, '14-Jan-16', '24-Nov-16', 0, 1, 8, 'B2C', 2);
INSERT INTO BookedTour VALUES(7, '8-Dec-15', '3-Aug-16', 0, 3, 3, 'D4P', 4);
