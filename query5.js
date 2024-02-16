// Query 5
// Find the oldest friend for each user who has a friend. For simplicity,
// use only year of birth to determine age, if there is a tie, use the
// one with smallest user_id. You may find query 2 and query 3 helpful.
// You can create selections if you want. Do not modify users collection.
// Return a javascript object : key is the user_id and the value is the oldest_friend id.
// You should return something like this (order does not matter):
// {user1:userx1, user2:userx2, user3:userx3,...}

function oldest_friend(dbname) {
    db = db.getSiblingDB(dbname);

    let results = {};
    // TODO: implement oldest friends
    db.createCollection("selectedView");

    db.users.find().forEach(function(U) {
        db.selectedView.insertOne({"user_id": U.user_id, "friends": U.friends, "YOB": U.YOB});
    });

    db.selectedView.aggregate([{$unwind: "$friends"}]).forEach(function(U) {
        db.selectedView.update({user_id: U.friends}, {$push: {friends: U.user_id}})
    });

    db.selectedView.find({friends: {$ne: []}}).forEach(function(U) {
        results[U.user_id] = db.selectedView.find({user_id: {$in: U.friends}}).sort({
            YOB: 1, user_id: 1}).limit(1).next().user_id
    });
    return results;
}
