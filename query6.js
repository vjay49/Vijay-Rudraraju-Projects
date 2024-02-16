// Query 6
// Find the average friend count per user.
// Return a decimal value as the average user friend count of all users in the users collection.

function find_average_friendcount(dbname) {
    db = db.getSiblingDB(dbname);

    // TODO: calculate the average friend count
    const result = db.users.aggregate(
        {$addFields: {"friendCount": {$size: "$friends"}}},
        {$group: {_id: null, friendAverage: {$avg: "$friendCount"}}}).toArray();

    if (result.length > 0){
        return result[0].friendAverage;
    }
    else{
        return 0;
    }
}
