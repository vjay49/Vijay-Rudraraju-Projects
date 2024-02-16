package project2;

import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.ResultSet;
import java.util.ArrayList;

/*
    The StudentFakebookOracle class is derived from the FakebookOracle class and implements
    the abstract query functions that investigate the database provided via the <connection>
    parameter of the constructor to discover specific information.
*/
public final class StudentFakebookOracle extends FakebookOracle {
    // [Constructor]
    // REQUIRES: <connection> is a valid JDBC connection
    public StudentFakebookOracle(Connection connection) {
        oracle = connection;
    }

    @Override
    // Query 0
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the total number of users for which a birth month is listed
    //        (B) Find the birth month in which the most users were born
    //        (C) Find the birth month in which the fewest users (at least one) were born
    //        (D) Find the IDs, first names, and last names of users born in the month
    //            identified in (B)
    //        (E) Find the IDs, first names, and last name of users born in the month
    //            identified in (C)
    //
    // This query is provided to you completed for reference. Below you will find the appropriate
    // mechanisms for opening up a statement, executing a query, walking through results, extracting
    // data, and more things that you will need to do for the remaining nine queries
    public BirthMonthInfo findMonthOfBirthInfo() throws SQLException {
        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            // Step 1
            // ------------
            // * Find the total number of users with birth month info
            // * Find the month in which the most users were born
            // * Find the month in which the fewest (but at least 1) users were born
            ResultSet rst = stmt.executeQuery(
                    "SELECT COUNT(*) AS Birthed, Month_of_Birth " + // select birth months and number of uses with that birth month
                            "FROM " + UsersTable + " " + // from all users
                            "WHERE Month_of_Birth IS NOT NULL " + // for which a birth month is available
                            "GROUP BY Month_of_Birth " + // group into buckets by birth month
                            "ORDER BY Birthed DESC, Month_of_Birth ASC"); // sort by users born in that month, descending; break ties by birth month

            int mostMonth = 0;
            int leastMonth = 0;
            int total = 0;
            while (rst.next()) { // step through result rows/records one by one
                if (rst.isFirst()) { // if first record
                    mostMonth = rst.getInt(2); //   it is the month with the most
                }
                if (rst.isLast()) { // if last record
                    leastMonth = rst.getInt(2); //   it is the month with the least
                }
                total += rst.getInt(1); // get the first field's value as an integer
            }
            BirthMonthInfo info = new BirthMonthInfo(total, mostMonth, leastMonth);

            // Step 2
            // ------------
            // * Get the names of users born in the most popular birth month
            rst = stmt.executeQuery(
                    "SELECT User_ID, First_Name, Last_Name " + // select ID, first name, and last name
                            "FROM " + UsersTable + " " + // from all users
                            "WHERE Month_of_Birth = " + mostMonth + " " + // born in the most popular birth month
                            "ORDER BY User_ID"); // sort smaller IDs first

            while (rst.next()) {
                info.addMostPopularBirthMonthUser(new UserInfo(rst.getLong(1), rst.getString(2), rst.getString(3)));
            }

            // Step 3
            // ------------
            // * Get the names of users born in the least popular birth month
            rst = stmt.executeQuery(
                    "SELECT User_ID, First_Name, Last_Name " + // select ID, first name, and last name
                            "FROM " + UsersTable + " " + // from all users
                            "WHERE Month_of_Birth = " + leastMonth + " " + // born in the least popular birth month
                            "ORDER BY User_ID"); // sort smaller IDs first

            while (rst.next()) {
                info.addLeastPopularBirthMonthUser(new UserInfo(rst.getLong(1), rst.getString(2), rst.getString(3)));
            }

            // Step 4
            // ------------
            // * Close resources being used
            rst.close();
            stmt.close(); // if you close the statement first, the result set gets closed automatically

            return info;

        } catch (SQLException e) {
            System.err.println(e.getMessage());
            return new BirthMonthInfo(-1, -1, -1);
        }
    }

    @Override
    // Query 1
    // -----------------------------------------------------------------------------------
    // GOALS: (A) The first name(s) with the most letters
    //        (B) The first name(s) with the fewest letters
    //        (C) The first name held by the most users
    //        (D) The number of users whose first name is that identified in (C)
    public FirstNameInfo findNameInfo() throws SQLException {
        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            ResultSet rst = stmt.executeQuery(
                 "SELECT DISTINCT First_Name " +
                    "FROM " + UsersTable + " " +
                    "WHERE LENGTH(First_Name) IN ( " +
                    "SELECT MAX(LENGTH(First_Name)) " +
                    "FROM " + UsersTable + " " +
                    "UNION SELECT MIN(LENGTH(First_Name)) " +
                    "FROM " + UsersTable + " " +
                    ") " +
                    "ORDER BY LENGTH(First_Name) DESC, First_Name ASC");

            FirstNameInfo info = new FirstNameInfo();
            int longest_length = 0;
           // rst.absolute(1);
            String name = null;
            while (rst.next()) {
                name = rst.getString(1);
                longest_length = Math.max(longest_length, name.length());
                if ((name.length()) == longest_length) {
                    info.addLongName(name);
                }
                else {
                    info.addShortName(name);
                }
            }
            
            rst = stmt.executeQuery(
                "SELECT First_Name, COUNT(*) AS Freq " +
                    "FROM " + UsersTable + " " +
                    "WHERE First_Name IS NOT NULL " +
                    "GROUP BY First_Name " +
                    "ORDER BY Freq DESC, First_Name ASC"
            );
                    //"GROUP BY First_Name " +
                    //"HAVING (Frequency = SELECT MAX(COUNT(First_Name)) FROM " + UsersTable + " GROUP BY First_Name) " +
                    
            int max_freq = 0;
            int freq = 0;
            name = null;
            while (rst.next()) {
                name = rst.getString(1);
                freq = rst.getInt(2);
                max_freq = Math.max(max_freq, freq);
                if (max_freq == freq) {
                    info.addCommonName(name);
                    info.setCommonNameCount(freq);
                }
                else {
                    break;
                }
            }
                    /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                FirstNameInfo info = new FirstNameInfo();
                info.addLongName("Aristophanes");
                info.addLongName("Michelangelo");
                info.addLongName("Peisistratos");
                info.addShortName("Bob");
                info.addShortName("Sue");
                info.addCommonName("Harold");
                info.addCommonName("Jessica");
                info.setCommonNameCount(42);
                return info;
            */

            rst.close();
            stmt.close(); 

            return info;
        } catch (SQLException e) {
            System.err.println(e.getMessage());
            return new FirstNameInfo();
        }
    }

    @Override
    // Query 2
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the IDs, first names, and last names of users without any friends
    //
    // Be careful! Remember that if two users are friends, the Friends table only contains
    // the one entry (U1, U2) where U1 < U2.
    public FakebookArrayList<UserInfo> lonelyUsers() throws SQLException {
        FakebookArrayList<UserInfo> results = new FakebookArrayList<UserInfo>(", ");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            
            ResultSet rst = stmt.executeQuery(
                "SELECT DISTINCT U.User_ID, U.First_Name, U.Last_Name " +
                    "FROM " + UsersTable + " U " +
                    "WHERE U.User_ID NOT IN (SELECT User1_ID FROM " + FriendsTable + " " +
                    "UNION SELECT User2_ID FROM " + FriendsTable + ") " + 
                    "ORDER BY U.User_ID ASC"
            );
            FakebookArrayList<UserInfo> lonelyPeople = new FakebookArrayList<UserInfo>(", ");
            while(rst.next()){
                UserInfo info = new UserInfo(rst.getLong(1), rst.getString(2), rst.getString(3));
                lonelyPeople.add(info); 
            }
            
            rst.close();
            stmt.close(); 

            return lonelyPeople;
            
         
            
            /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo u1 = new UserInfo(15, "Abraham", "Lincoln");
                UserInfo u2 = new UserInfo(39, "Margaret", "Thatcher");
                results.add(u1);
                results.add(u2);
            */
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    @Override
    // Query 3
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the IDs, first names, and last names of users who no longer live
    //            in their hometown (i.e. their current city and their hometown are different)
    public FakebookArrayList<UserInfo> liveAwayFromHome() throws SQLException {
        FakebookArrayList<UserInfo> results = new FakebookArrayList<UserInfo>(", ");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            
                ResultSet rst = stmt.executeQuery(
                "SELECT U.User_ID, U.First_Name, U.Last_Name " +
                "FROM " + UsersTable + " U " +
                "INNER JOIN " + HometownCitiesTable + " HT ON " +
                "((U.User_ID = HT.User_ID) AND HT.hometown_city_id IS NOT NULL) " + 
                "INNER JOIN " + CurrentCitiesTable + " CC ON " +
                "((U.User_ID = CC.User_ID) AND CC.current_city_id IS NOT NULL) " +
                "WHERE CC.current_city_id != HT.hometown_city_id " +
                "ORDER BY U.User_ID ASC"
                );
            
                FakebookArrayList<UserInfo> movedAway = new FakebookArrayList<UserInfo>(", ");
                while(rst.next()){
                    UserInfo info = new UserInfo(rst.getLong(1), rst.getString(2), rst.getString(3));
                    movedAway.add(info); 
                }
            
                rst.close();
                stmt.close(); 

                return movedAway;
            
            /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo u1 = new UserInfo(9, "Meryl", "Streep");
                UserInfo u2 = new UserInfo(104, "Tom", "Hanks");
                results.add(u1);
                results.add(u2);
            */
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    @Override
    // Query 4
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the IDs, links, and IDs and names of the containing album of the top
    //            <num> photos with the most tagged users
    //        (B) For each photo identified in (A), find the IDs, first names, and last names
    //            of the users therein tagged
    public FakebookArrayList<TaggedPhotoInfo> findPhotosWithMostTags(int num) throws SQLException {
        FakebookArrayList<TaggedPhotoInfo> results = new FakebookArrayList<TaggedPhotoInfo>("\n");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            
            ResultSet rst = stmt.executeQuery(
                "SELECT P.photo_id, P.album_id, P.photo_link, A.album_name, T.tag_subject_id, U.First_Name, U.Last_Name " +
                "FROM (SELECT COUNT(tag_photo_id) AS Freq, tag_photo_id FROM " +
                TagsTable + " GROUP BY tag_photo_id) sub " + 
                "INNER JOIN " + TagsTable + " T ON T.tag_photo_id = sub.tag_photo_id " +
                "INNER JOIN " + PhotosTable + " P  ON P.photo_id = T.tag_photo_id " + 
                "INNER JOIN " + AlbumsTable + " A ON A.album_id = P.album_id " +
                "INNER JOIN " + UsersTable + " U ON U.User_ID = T.tag_subject_id " +
                "ORDER BY sub.Freq DESC, sub.tag_photo_id, T.tag_subject_id"
            );
            
            int counter = 0;
            int per_photo = 0;
            int per_user = 0;
            rst.next();
            while(counter != num){
                counter ++;
                per_photo = rst.getInt(1);
                per_user = per_photo;
                PhotoInfo p = new PhotoInfo(per_photo, rst.getInt(2), rst.getString(3), rst.getString(4));
                TaggedPhotoInfo tp = new TaggedPhotoInfo(p);
                while (per_photo == per_user) {
                    UserInfo u = new UserInfo(rst.getInt(5), rst.getString(6), rst.getString(7));
                    tp.addTaggedUser(u);
                    if (rst.next()) {
                        per_user = rst.getInt(1);
                    }
                    else {
                        break;
                    }
                }
                results.add(tp);
            }
            rst.close();
            stmt.close();
            /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                PhotoInfo p = new PhotoInfo(80, 5, "www.photolink.net", "Winterfell S1");
                UserInfo u1 = new UserInfo(3901, "Jon", "Snow");
                UserInfo u2 = new UserInfo(3902, "Arya", "Stark");
                UserInfo u3 = new UserInfo(3903, "Sansa", "Stark");
                TaggedPhotoInfo tp = new TaggedPhotoInfo(p);
                tp.addTaggedUser(u1);
                tp.addTaggedUser(u2);
                tp.addTaggedUser(u3);
                results.add(tp);
            */
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    @Override
    // Query 5
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the IDs, first names, last names, and birth years of each of the two
    //            users in the top <num> pairs of users that meet each of the following
    //            criteria:
    //              (i) same gender
    //              (ii) tagged in at least one common photo
    //              (iii) difference in birth years is no more than <yearDiff>
    //              (iv) not friends
    //        (B) For each pair identified in (A), find the IDs, links, and IDs and names of
    //            the containing album of each photo in which they are tagged together
    public FakebookArrayList<MatchPair> matchMaker(int num, int yearDiff) throws SQLException {
        FakebookArrayList<MatchPair> results = new FakebookArrayList<MatchPair>("\n");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            
                ResultSet rst = stmt.executeQuery(
                    "SELECT DISTINCT U1.User_ID, U1.First_Name, U1.Last_Name, U1.year_of_birth, U2.User_ID, U2.First_Name, U2.Last_Name, U2.year_of_birth, P.photo_id, P.photo_link, A.album_id, A.album_name, COUNT(*) as Freq " +
                    "FROM " + UsersTable + " U1 INNER JOIN " + UsersTable + " U2 ON (U1.gender = U2.gender) " +
                    "INNER JOIN " + TagsTable + " T1 ON (U1.User_ID = T1.tag_subject_id) " +
                    "INNER JOIN " + TagsTable + " T2 ON (U2.User_ID = T2.tag_subject_id AND T1.tag_photo_id = T2.tag_photo_id) " +
                    "INNER JOIN " + PhotosTable + " P ON (T1.tag_photo_id = P.photo_id) " +
                    "INNER JOIN " + AlbumsTable + " A ON (P.album_id = A.album_id) " +
                    "WHERE ABS(U1.year_of_birth - U2.year_of_birth) <= " + yearDiff + " " +
                    "AND U1.User_ID < U2.User_ID " +
                    "AND U1.year_of_birth IS NOT NULL AND U2.year_of_birth IS NOT NULL " +
                    "AND U2.User_ID NOT IN (" +
                    "SELECT F.User2_ID FROM " + FriendsTable + " F WHERE F.User1_ID = U1.User_ID) " +
                    "GROUP BY U1.User_ID, U1.First_Name, U1.Last_Name, U1.year_of_birth, U2.User_ID, U2.First_Name, U2.Last_Name, U2.year_of_birth, P.photo_id, P.photo_link, A.album_id, A.album_name " +
                    "HAVING COUNT(*) > 0 " +
                    "ORDER BY Freq, U1.User_ID, U2.User_ID"
                );
                
                int counter = 0;
                while ((counter != num) && rst.next()) {
                    counter++;
                    UserInfo u1 = new UserInfo(rst.getInt(1), rst.getString(2), rst.getString(3));
                    UserInfo u2 = new UserInfo(rst.getInt(5), rst.getString(6), rst.getString(7));
                    MatchPair mp = new MatchPair(u1, rst.getInt(4), u2, rst.getInt(8));
                    PhotoInfo p = new PhotoInfo(rst.getInt(9), rst.getInt(11), rst.getString(10), rst.getString(12));
                    mp.addSharedPhoto(p);
                    results.add(mp);
                }
                rst.close();
                stmt.close();
                    /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo u1 = new UserInfo(93103, "Romeo", "Montague");
                UserInfo u2 = new UserInfo(93113, "Juliet", "Capulet");
                MatchPair mp = new MatchPair(u1, 1597, u2, 1597);
                PhotoInfo p = new PhotoInfo(167, 309, "www.photolink.net", "Tragedy");
                mp.addSharedPhoto(p);
                results.add(mp);
            */
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    @Override
    // Query 6
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the IDs, first names, and last names of each of the two users in
    //            the top <num> pairs of users who are not friends but have a lot of
    //            common friends
    //        (B) For each pair identified in (A), find the IDs, first names, and last names
    //            of all the two users' common friends
    public FakebookArrayList<UsersPair> suggestFriends(int num) throws SQLException {
        FakebookArrayList<UsersPair> results = new FakebookArrayList<UsersPair>("\n");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
                    
                //stmt.executeUpdate("DROP VIEW Bidirectional_Friendships");
  
                stmt.executeUpdate(
                    "CREATE VIEW Bidirectional_Friendships AS " + 
                    "SELECT F1.User1_ID AS UID1, F1.User2_ID AS UID2 " + 
                    "FROM " + FriendsTable + " F1 " +
                    "UNION ALL " + 
                    "SELECT F2.User2_ID, F2.User1_ID " + 
                    "FROM " + FriendsTable + " F2"
                );

                ResultSet rst = stmt.executeQuery(
                    "SELECT * FROM( " +
                    "SELECT DISTINCT BF1.UID1 AS RID1, U1.First_Name AS UF1, U1.Last_Name AS UL1, " + 
                    "BF2.UID2 AS RID2, U2.First_Name AS UF2, U2.Last_Name AS UL2, COUNT(*) AS Freq " +
                    "FROM Bidirectional_Friendships BF1 " +
                    "JOIN Bidirectional_Friendships BF2 ON BF2.UID1 = BF1.UID2 " +
                    "JOIN " + UsersTable + " U1 ON U1.User_ID = BF1.UID1 " +
                    "JOIN " + UsersTable + " U2 ON U2.User_ID = BF2.UID2 " +
                    "WHERE BF1.UID1 < BF2.UID2 " +
                    "GROUP BY BF1.UID1, BF2.UID2, U1.First_Name, U1.Last_Name, U2.First_Name, U2.Last_Name " +
                    "ORDER BY Freq DESC, BF1.UID1 ASC, BF2.UID2 ASC) " +
                    "WHERE ROWNUM <= " + num
                );

                Statement stmtloop = oracle.createStatement(FakebookOracleConstants.AllScroll,FakebookOracleConstants.ReadOnly);
                ResultSet rstloop = null;

                while(rst.next()){
                    UserInfo friend1 = new UserInfo(rst.getInt(1), rst.getString(2), rst.getString(3));
                    UserInfo friend2 = new UserInfo(rst.getInt(4), rst.getString(5), rst.getString(6));
                    UsersPair pair = new UsersPair(friend1, friend2);
                    rstloop = stmtloop.executeQuery(
                        "SELECT DISTINCT BF1.UID2, U.First_Name, U.Last_Name " +
                        "FROM Bidirectional_Friendships BF1 " +
                        "JOIN Bidirectional_Friendships BF2 ON BF1.UID2 = BF2.UID1 " +
                        "JOIN " + UsersTable + " U on BF1.UID2 = U.User_ID " +
                        "AND BF2.UID1 = U.User_ID " +
                        "WHERE BF1.UID1 = " + rst.getInt(1) + " AND BF2.UID2 = " + rst.getInt(4) + " " +
                        "ORDER BY BF1.UID2 ASC"
                    );
                    for(int i = 1; i <= rst.getInt(7); i++){
                        rstloop.next();
                        UserInfo mutual = new UserInfo(rstloop.getInt(1), rstloop.getString(2), rstloop.getString(3));
                        pair.addSharedFriend(mutual);
                    }
                    results.add(pair);
                }

                //Make sure to drop both views
                stmt.executeUpdate("DROP VIEW Bidirectional_Friendships");
                
                rst.close();
                rstloop.close();
                stmt.close();
                stmtloop.close();
                    /* 
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo u1 = new UserInfo(16, "The", "Hacker");
                UserInfo u2 = new UserInfo(80, "Dr.", "Marbles");
                UserInfo u3 = new UserInfo(192, "Digit", "Le Boid");
                UsersPair up = new UsersPair(u1, u2);
                up.addSharedFriend(u3);
                results.add(up);
            */
        
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    @Override
    // Query 7
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the name of the state or states in which the most events are held
    //        (B) Find the number of events held in the states identified in (A)
    public EventStateInfo findEventStates() throws SQLException {
        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
                ResultSet rst = stmt.executeQuery(
                    "SELECT DISTINCT C.State_Name, COUNT(*) AS Freq " +
                    "FROM " + EventsTable + " E " +
                    "INNER JOIN " + CitiesTable + " C ON C.City_ID = E.Event_City_ID " +
                    "GROUP BY C.State_Name " +
                    "ORDER BY Freq DESC, C.State_Name ASC"
                );
                
                
                int max_freq = 0;
                int freq = 0;
                String name = "";
                while (rst.next()) {
                    if (max_freq == 0) {
                        name += rst.getString(1);
                        max_freq = rst.getInt(2);
                    }
                    else if (max_freq == rst.getInt(2)) {
                        name += ", " + rst.getString(1);
                    }
                    else {
                        break;
                    }
                }
                EventStateInfo info = new EventStateInfo(max_freq);
                info.addState(name);
                rst.close();
                stmt.close(); 
            
                    /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                EventStateInfo info = new EventStateInfo(50);
                info.addState("Kentucky");
                info.addState("Hawaii");
                info.addState("New Hampshire");
                return info;
            */
            return info;
        } catch (SQLException e) {
            System.err.println(e.getMessage());
            return new EventStateInfo(-1);
        }
    }

    @Override
    // Query 8
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find the ID, first name, and last name of the oldest friend of the user
    //            with User ID <userID>
    //        (B) Find the ID, first name, and last name of the youngest friend of the user
    //            with User ID <userID>
    public AgeInfo findAgeInfo(long userID) throws SQLException {
        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
            
            ResultSet rst = stmt.executeQuery(
                "SELECT DISTINCT U.User_ID, U.First_Name, U.Last_Name, U.year_of_birth, U.month_of_birth, U.day_of_birth " +
                "FROM " + UsersTable + " U " + 
                "WHERE U.User_ID IN ( " + 
                "SELECT f1.User1_ID " +
                "FROM " + FriendsTable + " f1 " +
                "WHERE f1.User2_ID = " + userID + " " +
                "UNION " + 
                "SELECT f2.User2_ID " + 
                "FROM " + FriendsTable + " f2 " +
                "WHERE f2.User1_ID = " + userID + ") " +
                "ORDER BY U.year_of_birth, U.month_of_birth, U.day_of_birth, U.User_ID ASC"
            );
            int old_id = 0;
            String old_fname = null;
            String old_lname = null;
            int young_id = 0;
            String young_fname = null;
            String young_lname = null;
            while(rst.next()){
                if(rst.isFirst()){
                    old_id = rst.getInt(1);
                    old_fname = rst.getString(2);
                    old_lname = rst.getString(3);
                }
                if(rst.isLast()){
                    young_id = rst.getInt(1);
                    young_fname = rst.getString(2);
                    young_lname = rst.getString(3);
                }
            }
            /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo old = new UserInfo(12000000, "Galileo", "Galilei");
                UserInfo young = new UserInfo(80000000, "Neil", "deGrasse Tyson");
                return new AgeInfo(old, young);
            */
            rst.close();
            stmt.close(); 

            return new AgeInfo(new UserInfo(old_id, old_fname, old_lname), new UserInfo(young_id, young_fname, young_lname));
        } catch (SQLException e) {
            System.err.println(e.getMessage());
            return new AgeInfo(new UserInfo(-1, "ERROR", "ERROR"), new UserInfo(-1, "ERROR", "ERROR"));
        }
    }

    @Override
    // Query 9
    // -----------------------------------------------------------------------------------
    // GOALS: (A) Find all pairs of users that meet each of the following criteria
    //              (i) same last name
    //              (ii) same hometown
    //              (iii) are friends
    //              (iv) less than 10 birth years apart
    public FakebookArrayList<SiblingInfo> findPotentialSiblings() throws SQLException {
        FakebookArrayList<SiblingInfo> results = new FakebookArrayList<SiblingInfo>("\n");

        try (Statement stmt = oracle.createStatement(FakebookOracleConstants.AllScroll,
                FakebookOracleConstants.ReadOnly)) {
                    ResultSet rst = stmt.executeQuery(
                        "SELECT DISTINCT U1.User_ID, U1.First_Name, U1.Last_Name, U2.User_ID, U2.First_Name, U2.Last_Name " +
                        "FROM " + UsersTable + " U1 " +
                        "JOIN " + UsersTable + " U2 ON U1.Last_Name = U2.Last_Name " +
                        "JOIN " + HometownCitiesTable + " HT1 ON U1.User_ID = HT1.User_ID " +
                        "JOIN " + HometownCitiesTable + " HT2 ON U2.User_ID = HT2.User_ID " +
                        "AND HT1.hometown_city_id = HT2.hometown_city_id " +
                        "JOIN " + FriendsTable + " F1 ON U1.User_ID = F1.User1_ID " +
                        "AND U2.User_ID = F1.User2_ID " +
                        "WHERE (ABS(U1.year_of_birth - U2.year_of_birth) < 10) " +
                        "AND U1.User_ID < U2.User_ID " +
                        "ORDER BY U1.User_ID ASC, U2.User_ID ASC"
                        /*"AND U1.User_ID IN ( " +
                        "SELECT HT1.User_ID " +
                        "FROM " + HometownCitiesTable + " HT1, " +
                        "WHERE ( " + HometownCitiesTable + " HT2.User_ID = U2.User_ID) SSSSS" +
                        "AND (HT1.hometown_city_id = HT2.hometown_city_id) " + 
                        "AND (HT1.User_ID != HT2.User_ID)) " +
                        "AND U2.User_ID IN ( " +
                        "SELECT F1.User2_ID FROM " + FriendsTable + " F1 " +
                        "WHERE F1.User1_ID = U1.User_ID) " +
                        "ORDER BY U1.User_ID ASC, U2.User_ID DESC" */
                        /*"INTERSECT " +
                        "SELECT HT1.User_ID, HT2.User_ID " +
                        "FROM " + HometownCitiesTable + " HT1, " + HometownCitiesTable + " HT2 " +
                        "WHERE HT1.hometown_city_id = HT2.hometown_city_id " +
                        "INTERSECT " +
                        "SELECT F.User1_ID, F.User2_ID " +
                        "FROM " + FriendsTable + " F " +
                        "ORDER BY U1.User_ID ASC, U2.User_ID DESC"*/
                    );
                    
                    while (rst.next()) {
                        UserInfo u1 = new UserInfo(rst.getLong(1), rst.getString(2), rst.getString(3));
                        UserInfo u2 = new UserInfo(rst.getLong(4), rst.getString(5), rst.getString(6));
                        SiblingInfo si = new SiblingInfo(u1, u2);
                        results.add(si);
                    }

                rst.close();
                stmt.close();
                    /*
                EXAMPLE DATA STRUCTURE USAGE
                ============================================
                UserInfo u1 = new UserInfo(81023, "Kim", "Kardashian");
                UserInfo u2 = new UserInfo(17231, "Kourtney", "Kardashian");
                SiblingInfo si = new SiblingInfo(u1, u2);
                results.add(si);
            */
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return results;
    }

    // Member Variables
    private Connection oracle;
    private final String UsersTable = FakebookOracleConstants.UsersTable;
    private final String CitiesTable = FakebookOracleConstants.CitiesTable;
    private final String FriendsTable = FakebookOracleConstants.FriendsTable;
    private final String CurrentCitiesTable = FakebookOracleConstants.CurrentCitiesTable;
    private final String HometownCitiesTable = FakebookOracleConstants.HometownCitiesTable;
    private final String ProgramsTable = FakebookOracleConstants.ProgramsTable;
    private final String EducationTable = FakebookOracleConstants.EducationTable;
    private final String EventsTable = FakebookOracleConstants.EventsTable;
    private final String AlbumsTable = FakebookOracleConstants.AlbumsTable;
    private final String PhotosTable = FakebookOracleConstants.PhotosTable;
    private final String TagsTable = FakebookOracleConstants.TagsTable;
}
