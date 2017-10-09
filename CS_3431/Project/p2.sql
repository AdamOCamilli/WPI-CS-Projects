--------------------------------------------------------------------------------
------------------------------Project: Part 2-----------------------------------
--------------------------------------------------------------------------------
----------------------Group 11: Adam Camilli and Myo Thant----------------------
--------------------------------------------------------------------------------




-------------------------Queries And Procedures---------------------------------


------------------------Project 2: NoLabMgr Query-------------------------------

--Select all offices with more than one person
drop view MultiPersonOffices;

create view MultiPersonOffices as
    select distinct Offices.officeID
    from Offices, CSStaff
    where Offices.officeID = CSStaff.officeID
    group by Offices.officeID
    having count(CSStaff.officeID) > 1;
    
select * from MultiPersonOffices;

--List all professors in multi-person offices and the office they are in

drop view ListNoLabMgr;

create view ListNoLabMgr as 
    select MPO.officeID as Office, CSS.accountName as Prof
    from MultiPersonOffices MPO, CSStaff CSS, CSStaffTitles CT 
    where   (MPO.officeID = CSS.officeID)
            AND
            (CT.accountName = CSS.accountName)
            And
            (CT.acronym not in ('Lab1','Lab2'));

select * from ListNoLabMgr;

--For each multi-person office, list how many professors are in it
drop view NoLabMgr;   
    
create view NoLabMgr as
    select distinct Office, count(Office) as Amount
    from ListNoLabMgr
    group by Office;

select * from NoLabMgr;


-----------------------Project 2: NumberOfStaff procedure-----------------------

create or replace procedure NumberOfStaff (var_officeID IN varchar2) is

    total_staff NUMBER(5);

Begin
    SELECT COUNT(officeID)
    INTO total_staff
    FROM CSStaff
    WHERE officeID = var_officeID;

    dbms_output.put_line('Total Staff: ' || total_staff);

End;
/


set serveroutput on;

exec NumberOfStaff(233);

------------------------------Triggers------------------------------------------


---------------------------Project 2: TitleLimit Trigger------------------------

Create or Replace Trigger TitleLimit

Before Insert or Update on CSStaffTitles

For Each Row

Declare

    temp varchar2(15);

    cursor a1 is SELECT accountName From CSSTaffTitles 

                 Group by accountName

                 Having count(acronym) >= 3;

Begin

    open a1;

    Loop

        Fetch a1 into temp;

        

        If((a1%Found) and (temp = :new.accountName)) Then

            Raise_Application_Error(-20001, 'Professor cannot be inserted because he already has 3 titles.');

        Elsif (temp = :old.accountName) Then

            Raise_Application_Error(-20002, 'Professor has more than 3 titles.');

        End if;

        Exit When a1%NotFound;

    End Loop;

    close a1;

  

End;
/

insert into CSStaffTitles values('nth', 'GradAdmin');
insert into CSStaffTitles values('nth', 'Lab1');



----------------------------Project 2: MustBeOffice Trigger---------------------

create or replace trigger MustBeOffice
After Insert or Update on Offices
For Each Row
Declare
    temp varchar2(10);
    cursor l1 is 
        Select locationId 
        From Locations
        Where locationType <> 'Office';
    
Begin
    Open l1;
    Loop 
        Fetch l1 into temp;
        
        if (l1%found) and (temp = :new.officeID) 
            then Raise_Application_Error(-20003, 'The insert location is not an office.');
        elsif (temp = :old.officeID) 
            then Raise_Application_Error(-20004, 'Non-office location inserted in office entity.');
        end if;
    end loop;
    close l1;
End;
/

insert into Offices (OFFICEID) values ('A21');



--------------------Project 2: NoSameLocations Trigger--------------------------
Create or Replace trigger NoSameLocations 
Before Insert on Edges
For Each Row
Begin
    If (:new.startingLocationID = :new.endingLocationID)
        then Raise_Application_Error(-20005, 'The two locations are the same.');
    end if;
End;
/

insert into Edges (EDGEID, STARTINGLOCATIONID, ENDINGLOCATIONID) values ('2H1_2H1', '2H1', '2H1');



----------------------Project 2: CrossFloorEdge Trigger-------------------------
create or replace trigger CrossFloorEdge
before insert on Edges
For Each Row
Declare
    start_floor char(1);
    end_floor char(1);
    
    start_type varchar2(20);
    end_type varchar2(20);
    
Begin

    Select Locations.mapFloor
    Into start_floor
    From Locations
    Where Locations.locationID = :new.startingLocationID;
    
    Select Locations.mapFloor
    Into end_floor
    From Locations
    Where Locations.locationID = :new.endingLocationID;
    
    Select Locations.locationType
    Into start_type
    From Locations
    Where Locations.locationID = :new.startingLocationID;
    
    Select Locations.locationType
    Into end_type
    From Locations
    Where Locations.locationID = :new.endingLocationID;
    
    if (start_floor <> end_floor)
        then if (
                    (
                        (start_type <> 'Elevator') and 
                        (end_type <> 'Elevator')
                    ) 
                And (
                    (start_type <> 'Staircase') and 
                    (end_Type <> 'Staircase')
                    )
                )
            then Raise_Application_Error(-20006,'Edge crossing floors does not start and end in an either an elevator or staircase.');
        end if;
    end if;

End;
/

set serveroutput on;

delete from Edges
where EDGEID = '231_314';

insert into Edges values('231_314','231','314');

delete from Edges
where EDGEID = '2E1_3E1';

insert into Edges values('2E1_3E1', '2E1', '3E1');
                
            
