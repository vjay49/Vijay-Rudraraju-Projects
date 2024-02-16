
-- View_User_Information
CREATE VIEW View_User_Information AS
SELECT Users.user_id, Users.first_name, Users.last_name, Users.year_of_birth, Users.month_of_birth,
    Users.day_of_birth, Users.gender, c_curr.city_name AS current_city,
    c_curr.state_name AS current_state, c_curr.country_name AS current_country,
    c_home.city_name AS hometown_city, c_home.state_name AS hometown_state,
    c_home.country_name AS hometown_country, Programs.institution AS institution_name,
    Education.program_year, Programs.concentration AS program_concentration, Programs.degree AS program_degree
FROM Users INNER JOIN (User_Current_Cities INNER JOIN Cities c_curr ON User_Current_Cities.current_city_id = c_curr.city_id)
    ON Users.user_id = User_Current_Cities.user_id INNER JOIN (User_Hometown_Cities INNER JOIN Cities c_home ON
    User_Hometown_Cities.hometown_city_id = c_home.city_id) ON Users.user_id = User_Hometown_Cities.user_id
    LEFT JOIN (Programs INNER JOIN Education ON Programs.program_id = Education.program_id) ON
    Users.user_id = Education.user_id;

-- View_Are_Friends 
CREATE VIEW View_Are_Friends AS
SELECT * FROM Friends;

-- View_Photo_Information
CREATE VIEW View_Photo_Information AS
SELECT Albums.album_id, Albums.album_owner_id AS owner_id, Albums.cover_photo_id, Albums.album_name,
Albums.album_created_time, Albums.album_modified_time, Albums.album_link, Albums.album_visibility, 
Photos.photo_id, Photos.photo_caption, Photos.photo_created_time, Photos.photo_modified_time,
Photos.photo_link
FROM Albums INNER JOIN Photos ON Albums.album_id = Photos.album_id;

-- View_Event_Information
CREATE VIEW View_Event_Information AS
SELECT User_Events.event_id, User_Events.event_creator_id, User_Events.event_name, User_Events.event_tagline,
    User_Events.event_description, User_Events.event_host, User_Events.event_type, User_Events.event_subtype,
    User_Events.event_address, Cities.city_name, Cities.state_name, Cities.country_name, User_Events.event_start_time,
    User_Events.event_end_time
FROM User_Events INNER JOIN Cities ON User_Events.event_city_id = Cities.city_id;

-- View_Tag_Information
CREATE VIEW View_Tag_Information AS
SELECT * FROM Tags;