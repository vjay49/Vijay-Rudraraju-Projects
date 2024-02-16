
-- 1: Users
INSERT INTO Users(user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender)
SELECT DISTINCT user_id, first_name, last_name, year_of_birth, month_of_birth, day_of_birth, gender
FROM project1.Public_User_Information;

-- 2: Friends
INSERT INTO Friends(user1_id, user2_id)
SELECT DISTINCT user1_id, user2_id
FROM project1.Public_Are_Friends;

-- 3: Cities
INSERT INTO Cities(city_name, state_name, country_name)
SELECT DISTINCT current_city, current_state, current_country
FROM project1.Public_User_Information
UNION SELECT DISTINCT hometown_city, hometown_state, hometown_country
FROM project1.Public_User_Information
UNION SELECT DISTINCT event_city, event_state, event_country
FROM project1.Public_Event_Information;

-- 4: User_Current_Cities
INSERT INTO User_Current_Cities(user_id, current_city_id)
SELECT DISTINCT Users.user_id, Cities.city_id
FROM Users, Cities, project1.Public_User_Information
WHERE Users.user_id = project1.Public_User_Information.user_id
AND Cities.city_name = project1.Public_User_Information.current_city
AND Cities.state_name = project1.Public_User_Information.current_state
AND Cities.country_name = project1.Public_User_Information.current_country;

-- 5: User_Hometown_Cities
INSERT INTO User_Hometown_Cities(user_id, hometown_city_id)
SELECT DISTINCT Users.user_id, Cities.city_id
FROM Users, Cities, project1.Public_User_Information
WHERE Users.user_id = project1.Public_User_Information.user_id
AND Cities.city_name = project1.Public_User_Information.hometown_city
AND Cities.state_name = project1.Public_User_Information.hometown_state
AND Cities.country_name = project1.Public_User_Information.hometown_country;

-- 6: Messages
-- NO PUBLIC DATA

-- 7: Programs
INSERT INTO Programs(institution, concentration, degree)
SELECT DISTINCT institution_name, program_concentration, program_degree
FROM project1.Public_User_Information
WHERE program_degree IS NOT NULL;

-- 8: Education
INSERT INTO Education(user_id, program_id, program_year)
SELECT DISTINCT project1.Public_User_Information.user_id, Programs.program_id, project1.Public_User_Information.program_year
FROM project1.Public_User_Information
INNER JOIN Programs ON project1.Public_User_Information.institution_name = Programs.institution
AND project1.Public_User_Information.program_degree = Programs.degree
AND project1.Public_User_Information.program_concentration = Programs.concentration;

-- 9: User_Events
INSERT INTO User_Events(event_id, event_creator_id, event_name, event_tagline, event_description,
event_host, event_type, event_subtype, event_address, event_city_id, event_start_time, event_end_time)
SELECT DISTINCT project1.Public_Event_Information.event_id, project1.Public_Event_Information.event_creator_id,
project1.Public_Event_Information.event_name, project1.Public_Event_Information.event_tagline, 
project1.Public_Event_Information.event_description, project1.Public_Event_Information.event_host,
project1.Public_Event_Information.event_type, project1.Public_Event_Information.event_subtype,
project1.Public_Event_Information.event_address, Cities.city_id, project1.Public_Event_Information.event_start_time,
project1.Public_Event_Information.event_end_time
FROM project1.Public_Event_Information
INNER JOIN Cities ON project1.Public_Event_Information.event_city = Cities.city_name
AND project1.Public_Event_Information.event_state = Cities.state_name
AND project1.Public_Event_Information.event_country = Cities.country_name;

-- 10: Participants
-- NO DATA

-- 11: Albums
SET AUTOCOMMIT OFF;

INSERT INTO Albums(album_id, album_owner_id, cover_photo_id, album_name, album_created_time, album_modified_time, 
album_link, album_visibility)
SELECT DISTINCT album_id, owner_id, cover_photo_id, album_name, album_created_time, album_modified_time, album_link, 
album_visibility
FROM project1.Public_Photo_Information;

-- 12: Photos
INSERT INTO Photos(photo_id, album_id, photo_caption, photo_created_time, photo_modified_time, photo_link)
SELECT DISTINCT photo_id, album_id, photo_caption, photo_created_time, photo_modified_time, photo_link
FROM project1.Public_Photo_Information;

COMMIT;
SET AUTOCOMMIT ON;

-- 13: Tags
INSERT INTO Tags(tag_photo_id, tag_subject_id, tag_created_time, tag_x, tag_y)
SELECT DISTINCT Photos.photo_id, Users.user_id, tag_created_time, tag_x_coordinate, tag_y_coordinate
FROM Photos, Users, project1.Public_Tag_Information
WHERE Photos.photo_id = project1.Public_Tag_Information.photo_id
AND Users.user_id = project1.Public_Tag_Information.tag_subject_id;
