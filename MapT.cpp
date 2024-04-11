//
// Created by we7289 on 4/19/2021.
//

#include "MapT.h"
#include <unordered_map>
//using namespace std;

const int DEFAULT_BUCKETS = 10;
const double DEFAULT_LOAD = 1.0;

template<class K, class T>
MapT<K, T>::MapT() {
    numBuckets = DEFAULT_BUCKETS;
    numKeys = 0;
    maxLoad = DEFAULT_LOAD;

    buckets = new forward_list<pair<K, T>>[numBuckets];
}

template<class K, class T>
MapT<K, T> &MapT<K, T>::operator=(const MapT &other) {
    delete []buckets; // remove all items in the map

    buckets = new forward_list<pair<K, T>>[other.numBuckets];
    numBuckets = other.numBuckets;
    numKeys = 0;

    for (int b = 0; b < other.numBuckets; b++){
        for (auto it = other.buckets[b].begin(); it != other.buckets[b].end(); ++it){
            Add(it->first, it->second);
        }
    }

    return *this;
}

template<class K, class T>
void MapT<K, T>::Add(K key, T value) {

    // Find the appropriate bucket that key lives in
    int bucket = GetHashIndex(key);

    // Remove any instance of key
    Remove(key);

    //make key value pair to be added to the bucket
    pair<K, T> keyValue;
    keyValue.first = key;
    keyValue.second = value;

    // Add the new key value to the bucket
    buckets[bucket].push_front(keyValue);

    ++numKeys;

    if (LoadFactor() > maxLoad){
        Rehash(numBuckets * 2);
    }
}

template<class K, class T>
void MapT<K, T>::Remove(K key) {
    // Find the appropriate bucket that key lives in
    int bucket = GetHashIndex(key);

    //look at all items in the bucket
    for (auto it = buckets[bucket].begin(); it != buckets[bucket].end(); ++it){ //must prefix increment
        if (key == it->first){
            buckets[bucket].remove(*it);
            --numKeys;
        }
    }
}

template<class K, class T>
bool MapT<K, T>::Contains(K key) {
    // Find the appropriate bucket that key lives in
    int bucket = GetHashIndex(key);

    //look at all items in the bucket
    for (auto it = buckets[bucket].begin(); it != buckets[bucket].end(); ++it){ //must prefix increment
        if (key == it->first){
            return true;
        }
    }

    return false;
}

template<class K, class T>
T MapT<K, T>::operator[](K key) {
    int bucket = GetHashIndex(key);


    //look at all items in the bucket
    for (auto it = buckets[bucket].begin(); it != buckets[bucket].end(); ++it){ //must prefix increment
        if (key == it->first){
            return it->second;
        }
    }


    throw KeyDoesNotExist();
}

template<class K, class T>
double MapT<K, T>::LoadFactor() {
    return static_cast<double>(numKeys)/numBuckets;
}

template<class K, class T>
void MapT<K, T>::SetMaxLoad(double maxLoad) {
    this->maxLoad = maxLoad;
}

template<class K, class T>
void MapT<K, T>::Rehash(int numBuckets) {
    MapT<K, T> newMap(numBuckets);  // Need to copy over all elements to newMap

    //for every bucket...
    for (int b = 0; b < this->numBuckets; b++){
        //look at all items in the bucket
        for (auto it = buckets[b].begin(); it != buckets[b].end(); ++it){ //must prefix increment
            newMap.Add(it->first, it->second); //add old items to new map
        }
    }

    //copy over values from newMap to current instance
    *this = newMap;
}

template<class K, class T>
void MapT<K, T>::ResetIterator() {
    mapIter = buckets[0].begin();
    currBucket = 0;
}

template<class K, class T>
pair<K,T> MapT<K, T>::GetNextPair() {
    pair<K,T> currVal;


    // Try to find first bucket with items
    while (mapIter == buckets[currBucket].end()){
        currBucket++;
        mapIter = buckets[currBucket].begin();
        if (currBucket == numBuckets){
            throw "Iterator out of bounds";
        }
    }
    currVal = *mapIter;
    ++mapIter;

    return currVal;
}

template<class K, class T>
int MapT<K, T>::GetHashIndex(const K &key) {
    unordered_map<K,T> mapper;
    typename unordered_map<K,T>::hasher hashFunction = mapper.hash_function();
    return static_cast<int>(hashFunction(key) % numBuckets);
}



