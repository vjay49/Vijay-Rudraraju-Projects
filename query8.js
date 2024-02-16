// Query 8
// Find the city average friend count per user using MapReduce.

let city_average_friendcount_mapper = function () {
    // TODO: Implement the map function
    var value = {count: 1, friendsNum: this.friends.length};
    emit(this.hometown.city, value);
};

let city_average_friendcount_reducer = function (key, values) {
    // TODO: Implement the reduce function
    reducedVal = {count: 0, friendsNum: 0};
    for(var idx = 0; idx < values.length; idx++){
        if(values[idx]){
            reducedVal.count += values[idx].count;
            reducedVal.friendsNum += values[idx].friendsNum;
        }
    }
    return reducedVal;
};

let city_average_friendcount_finalizer = function (key, reduceVal) {
    // We've implemented a simple forwarding finalize function. This implementation
    // is naive: it just forwards the reduceVal to the output collection.
    // TODO: Feel free to change it if needed.
    const average_friends = reduceVal.friendsNum / reduceVal.count;
    return average_friends;
};
