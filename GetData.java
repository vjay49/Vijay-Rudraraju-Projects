import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.TreeSet;
import java.util.Vector;

import org.json.JSONObject;
import org.json.JSONArray;

public class GetData {

    static String prefix = "project3.";

    // You must use the following variable as the JDBC connection
    Connection oracleConnection = null;

    // You must refer to the following variables for the corresponding 
    // tables in your database
    String userTableName = null;
    String friendsTableName = null;
    String cityTableName = null;
    String currentCityTableName = null;
    String hometownCityTableName = null;

    // DO NOT modify this constructor
    public GetData(String u, Connection c) {
        super();
        String dataType = u;
        oracleConnection = c;
        userTableName = prefix + dataType + "_USERS";
        friendsTableName = prefix + dataType + "_FRIENDS";
        cityTableName = prefix + dataType + "_CITIES";
        currentCityTableName = prefix + dataType + "_USER_CURRENT_CITIES";
        hometownCityTableName = prefix + dataType + "_USER_HOMETOWN_CITIES";
    }

    // TODO: Implement this function
    @SuppressWarnings("unchecked")
    public JSONArray toJSON() throws SQLException {

        // This is the data structure to store all users' information
        JSONArray users_info = new JSONArray();
        
        try (Statement stmt = oracleConnection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY)) {
            // Your implementation goes here....
            ResultSet rst1 = stmt.executeQuery(
                "SELECT U.User_ID, U.First_Name, U.Last_Name, U.gender, U.year_of_birth, U.month_of_birth, U.day_of_birth, C1.city_name, C1.state_name, C1.country_name, C2.city_name, C2.state_name, C2.country_name " +
                "FROM (" + userTableName + " U INNER JOIN " + currentCityTableName + " CC ON (CC.User_ID = U.User_ID) INNER JOIN " + 
                cityTableName + " C1 ON (C1.city_id = CC.current_city_id) INNER JOIN " + hometownCityTableName + " HC ON (HC.User_ID = U.User_ID) INNER JOIN " +
                cityTableName + " C2 ON (C2.city_id = HC.hometown_city_id)) ORDER BY U.User_ID ASC"
            );
            //query inner joins everything on userid except friends and orders by user id

            int user_id = 0;
            String first_name = "";
            String last_name = "";
            String gender = "";
            int YOB = 0;
            int MOB = 0;
            int DOB = 0;
            

            while (rst1.next()) {
                user_id = rst1.getInt(1);
                first_name = rst1.getString(2);
                last_name = rst1.getString(3);
                gender = rst1.getString(4);
                YOB = rst1.getInt(5);
                MOB = rst1.getInt(6);
                DOB = rst1.getInt(7);
                JSONObject current = new JSONObject();
                current.put("country", rst1.getString(10));
                current.put("city", rst1.getString(8));
                current.put("state", rst1.getString(9));

                JSONObject hometown = new JSONObject();
                hometown.put("country", rst1.getString(13));
                hometown.put("city", rst1.getString(11));
                hometown.put("state", rst1.getString(12));

                JSONArray friends = new JSONArray();

                Statement stmt2 = oracleConnection.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
                ResultSet rst2 = stmt2.executeQuery(
                    "SELECT DISTINCT User2_ID FROM " + friendsTableName +
                    " WHERE User1_ID < User2_ID AND User1_ID = " + user_id +
                    " ORDER BY User2_ID ASC"
                ); // comes up with empty arrays
                while (rst2.next()) {
                    friends.put(rst2.getInt(1));
                }
                rst2.close();
                stmt2.close();

                JSONObject user = new JSONObject();
                user.put("user_id", user_id);
                user.put("first_name", first_name);
                user.put("last_name", last_name);
                user.put("gender", gender);
                user.put("YOB", YOB);
                user.put("MOB", MOB);
                user.put("DOB", DOB);
                user.put("current", current);
                user.put("hometown", hometown);
                user.put("friends", friends);
                users_info.put(user);
            }


            rst1.close();
            stmt.close();
        } catch (SQLException e) {
            System.err.println(e.getMessage());
        }

        return users_info;
    }

    // This outputs to a file "output.json"
    // DO NOT MODIFY this function
    public void writeJSON(JSONArray users_info) {
        try {
            FileWriter file = new FileWriter(System.getProperty("user.dir") + "/output.json");
            file.write(users_info.toString());
            file.flush();
            file.close();

        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
