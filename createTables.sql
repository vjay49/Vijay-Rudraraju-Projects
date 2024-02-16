

CREATE TABLE Users (
    user_id INTEGER NOT NULL,
    first_name VARCHAR2(100) NOT NULL,
    last_name VARCHAR2(100) NOT NULL,
    year_of_birth INTEGER,
    month_of_birth INTEGER,
    day_of_birth INTEGER,
    gender VARCHAR2(100),
    PRIMARY KEY (user_id)
);

CREATE TABLE Friends (
    user1_id INTEGER NOT NULL, 
    user2_id INTEGER NOT NULL,
    PRIMARY KEY (user1_id, user2_id),
    FOREIGN KEY (user1_id) REFERENCES Users (user_id),
    FOREIGN KEY (user2_id) REFERENCES Users (user_id),
    CHECK (user1_id <> user2_id),
    CHECK (user1_id != user2_id)
);

CREATE TRIGGER Order_Friend_Pairs
    BEFORE INSERT ON Friends
    FOR EACH ROW
        DECLARE temp INTEGER;
        BEGIN
            IF :NEW.user1_id > :NEW.user2_id THEN
                temp := :NEW.user2_id;
                :NEW.user2_id := :NEW.user1_id;
                :NEW.user1_id := temp;
            END IF;
        END;

/

CREATE TABLE Cities (
    city_id INTEGER NOT NULL,
    city_name VARCHAR2(100) NOT NULL,
    state_name VARCHAR2(100) NOT NULL,
    country_name VARCHAR2(100) NOT NULL,
    PRIMARY KEY (city_id),
    CONSTRAINT unique_city UNIQUE(city_name, state_name, country_name)
);


CREATE SEQUENCE city_ids
    START WITH 1
    INCREMENT BY 1;
CREATE TRIGGER city_increment
    BEFORE INSERT ON Cities
    FOR EACH ROW
        BEGIN
            SELECT city_ids.NEXTVAL INTO :NEW.city_id FROM DUAL;
        END;
/

CREATE TABLE User_Current_Cities (
    user_id INTEGER NOT NULL,
    current_city_id INTEGER NOT NULL,
    PRIMARY KEY (user_id),
    FOREIGN KEY (user_id) REFERENCES Users (user_id),
    FOREIGN KEY (current_city_id) REFERENCES Cities (city_id)
);


CREATE TABLE User_Hometown_Cities(
    user_id INTEGER NOT NULL,
    hometown_city_id INTEGER NOT NULL,
    PRIMARY KEY (user_id),
    FOREIGN KEY (user_id) REFERENCES Users (user_id),
    FOREIGN KEY (hometown_city_id) REFERENCES Cities (city_id)
);

CREATE TABLE Messages (
    message_id INTEGER NOT NULL,
    sender_id INTEGER NOT NULL,
    receiver_id INTEGER NOT NULL,
    message_content VARCHAR2(100) NOT NULL,
    sent_time TIMESTAMP NOT NULL,
    PRIMARY KEY(message_id),
    FOREIGN KEY (sender_id) REFERENCES Users (user_id),
    FOREIGN KEY (receiver_id) REFERENCES Users (user_id)
);

CREATE TABLE Programs (
    program_id INTEGER NOT NULL, 
    institution VARCHAR2(100) NOT NULL,
    concentration VARCHAR2(100) NOT NULL,
    degree VARCHAR2(100) NOT NULL,
    PRIMARY KEY(program_id),
    CONSTRAINT unique_program UNIQUE (institution, concentration, degree)
);

CREATE SEQUENCE program_ids
    START WITH 1
    INCREMENT BY 1;
CREATE TRIGGER program_increment
    BEFORE INSERT ON Programs
    FOR EACH ROW
        BEGIN
            SELECT program_ids.NEXTVAL INTO :NEW.program_id FROM DUAL;
        END;

/

CREATE TABLE Education (
    user_id INTEGER NOT NULL,
    program_id INTEGER NOT NULL,
    program_year INTEGER NOT NULL,
    PRIMARY KEY (user_id, program_id),
    FOREIGN KEY (user_id) REFERENCES Users (user_id),
    FOREIGN KEY (program_id) REFERENCES Programs (program_id)
);

CREATE TABLE User_Events(
    event_id INTEGER NOT NULL,
    event_creator_id INTEGER NOT NULL,
    event_name VARCHAR2(100) NOT NULL,
    event_tagline VARCHAR2(100),
    event_description VARCHAR2(100),
    event_host VARCHAR2(100),
    event_type VARCHAR2(100),
    event_subtype VARCHAR2(100),
    event_address VARCHAR2(2000),
    event_city_id INTEGER NOT NULL,
    event_start_time TIMESTAMP,
    event_end_time TIMESTAMP,
    PRIMARY KEY(event_id),
    FOREIGN KEY (event_creator_id) REFERENCES Users (user_id),
    FOREIGN KEY (event_city_id) REFERENCES Cities (city_id)
);

CREATE TABLE Participants (
    event_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    confirmation VARCHAR2(100) NOT NULL,
    PRIMARY KEY (event_id, user_id),
    FOREIGN KEY (event_id) REFERENCES User_Events (event_id),
    FOREIGN KEY (user_id) REFERENCES Users (user_id),
    CHECK (confirmation IN('Attending', 'Unsure', 'Declines', 'Not_Replied'))
);

CREATE TABLE Albums (
    album_id INTEGER NOT NULL,
    album_owner_id INTEGER NOT NULL,
    album_name VARCHAR2(100) NOT NULL,
    album_created_time TIMESTAMP NOT NULL,
    album_modified_time TIMESTAMP,
    album_link VARCHAR2(2000) NOT NULL,
    album_visibility VARCHAR(100) NOT NULL,
    cover_photo_id INTEGER NOT NULL,
    PRIMARY KEY (album_id),
    FOREIGN KEY (album_owner_id) REFERENCES Users (user_id),
    CHECK (album_visibility IN ('Everyone', 'Friends', 'Friends_Of_Friends', 'Myself'))
);

CREATE TABLE Photos (
    photo_id INTEGER NOT NULL,
    album_id INTEGER NOT NULL,
    photo_caption VARCHAR2(2000),
    photo_created_time TIMESTAMP NOT NULL,
    photo_modified_time TIMESTAMP,
    photo_link VARCHAR2(2000) NOT NULL,    
    PRIMARY KEY (photo_id),
    FOREIGN KEY (album_id) REFERENCES Albums (album_id)
        INITIALLY DEFERRED DEFERRABLE
);

ALTER TABLE Albums ADD CONSTRAINT cover_photo_add FOREIGN KEY (cover_photo_id)
    REFERENCES Photos (photo_id) INITIALLY DEFERRED DEFERRABLE;

CREATE TABLE Tags (
    tag_photo_id INTEGER NOT NULL,
    tag_subject_id INTEGER NOT NULL,
    tag_created_time TIMESTAMP NOT NULL,
    tag_x NUMBER NOT NULL,
    tag_y NUMBER NOT NULL,
    PRIMARY KEY (tag_photo_id, tag_subject_id),
    FOREIGN KEY (tag_photo_id) REFERENCES Photos (photo_id),
    FOREIGN KEY (tag_subject_id) REFERENCES Users (user_id)
);
