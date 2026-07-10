#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
using namespace std;

class RedisLite{
    private:
    unordered_map<string,string> key_value;
    unordered_map<string, long long> expiry;

    public:
    RedisLite() = default;

    long long CurrentTime() const {
        auto now = chrono::system_clock::now();
        long long seconds = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
        return seconds;
    }

    void SET(const string& key, const string& value) {
        key_value[key] = value;
        expiry.erase(key);
    }

    bool LOAD(const string& filename) {
        ifstream file(filename);
        if(!file) {
            return false;
        }
        key_value.clear();
        expiry.clear();
        string line;
        while(getline(file,line)) {
            size_t pos = line.find(':');
            if(pos == string::npos) {
                continue;
            }
            string key = line.substr(0,pos);
            string value = line.substr(pos+1);
            key_value[key] = value;
        }
        return true;
    }

    bool RemoveIfExpired(const string& key) {
        auto it = expiry.find(key);
        if(it == expiry.end()) return false;
        long long seconds = CurrentTime();
        if(it->second > seconds) return false;
        expiry.erase(it);
        key_value.erase(key);
        return true;
    }

    void CleanupExpiredKeys() {
        vector<string> keys;
        for (const auto& it : key_value) {
            keys.push_back(it.first);
        }
        for (const auto& key : keys) {
            RemoveIfExpired(key);
        }
    }

    string GET(const string& key) {
        if(RemoveIfExpired(key)) return "Error: Key Not Found";
        if(auto it = key_value.find(key); it != key_value.end())
            return it->second;
        return "Error: Key Not Found";
    }

    bool DEL(const string& key) {
        if(RemoveIfExpired(key)) return false;
        auto it = key_value.find(key); 
        if(it == key_value.end()) return false;
        key_value.erase(it);
        expiry.erase(key);
        return true;
    }

    bool EXISTS(const string& key) {
        if(RemoveIfExpired(key)) return false;
        return key_value.find(key) != key_value.end();
    }

    vector<string> KEYS() {
        CleanupExpiredKeys();
        vector<string> keys;
        for(const auto& it : key_value) {
            keys.push_back(it.first);
        }
        sort(keys.begin(),keys.end());
        return keys;
    }

    size_t SIZE() {
        CleanupExpiredKeys();
        return key_value.size();
    }

    bool SAVE(const string& filename) {
        CleanupExpiredKeys();
        ofstream file(filename);
        if(!file) {
            return false;
        }
        for(const auto& it : key_value) {
            file<<it.first<<":"<<it.second<<'\n';
        }
        return true;
    }

    bool ChangeBy(const string& key, long long delta, long long& newValue) {
        RemoveIfExpired(key);
        auto it = key_value.find(key); 
        if(it == key_value.end()) {
            SET(key,to_string(delta));
            newValue = delta;
            return true;
        }
        try {
            size_t idx;
            const string& value = it->second;
            long long num = stoll(value, &idx);
            if (idx != value.size()) {
                return false;
            }
            num += delta;
            it->second = to_string(num);
            newValue = num;
            return true;
        }
        catch (const invalid_argument&) {
            return false;
        }
        catch (const out_of_range&) {
            return false;
        }
    }

    bool INCR(const string& key, long long& newValue) {
        return ChangeBy(key, 1, newValue);
    }

    bool DECR(const string& key, long long& newValue) {
        return ChangeBy(key, -1, newValue);
    }

    void MSET(const vector<pair<string, string>>& pairs) {
        for (const auto& [key, value] : pairs) {
            SET(key,value);
        }
    }

    vector<string> MGET(const vector<string>& keys) {
        vector<string> values;
        for (const auto& key : keys) {
            RemoveIfExpired(key);
            auto it = key_value.find(key);
            if (it != key_value.end())
                values.push_back(it->second);
            else
                values.push_back("NULL");
        }
        return values;
    }

    bool EXPIRE(const string& key, long long expireTime) {
        RemoveIfExpired(key);
        auto it = key_value.find(key);
        if(it == key_value.end()) return false;
        long long seconds = CurrentTime();
        expiry[key] = seconds + expireTime;
        return true;
    }

    long long TTL(const string& key) {
        RemoveIfExpired(key);
        auto valueIt = key_value.find(key); 
        if(valueIt == key_value.end()) return -1;
        auto expiryIt = expiry.find(key); 
        if(expiryIt == expiry.end()) return -1;
        long long seconds = CurrentTime();
        return expiryIt->second - seconds;
    }
};

int main() {
    RedisLite db;
    cout << "Welcome to Redis Lite\n";
    cout << "Type HELP to see available commands.\n";
    bool running = true;
    while(running) {
        string input;
        cout << "> ";
        getline(cin,input);
        stringstream ss(input);
        string command, key, value;
        ss >> command;
        if(command.empty()) continue;
        transform(command.begin(), command.end(), command.begin(), ::toupper);
        if(command == "SET") {
            ss >> key;
            getline(ss, value);
            if(!value.empty() && value[0] == ' ') value.erase(0,1);
            if(key.empty() || value.empty()) {
                cout << "Usage: " << command << " <key> <value>\n";
                continue;
            }
            db.SET(key,value);
            cout << "OK" << endl;
        }
        else if(command == "GET" || command == "DEL" || command == "EXISTS" || command == "INCR" || command == "DECR") {
            ss >> key;
            if(key.empty()) {
                cout << "Usage: " << command << " <key>\n";
                continue;
            }
            if(command == "GET") {
                cout << db.GET(key) << endl;
            }
            else if(command == "EXISTS") {
                cout << boolalpha << db.EXISTS(key) << noboolalpha << endl;
            }
            else if(command == "DEL") {
                if(db.DEL(key))
                    cout << "Deleted" << endl;
                else
                    cout << "Error: Key Not Found" << endl;
            }
            else if(command == "INCR") {
                long long newValue;
                if(db.INCR(key, newValue))
                    cout << newValue << endl;
                else
                    cout << "Error: Value is not an integer" << endl;
            }
            else if(command == "DECR") {
                long long newValue;
                if(db.DECR(key, newValue))
                    cout << newValue << endl;
                else
                    cout << "Error: Value is not an integer" << endl;
            }
        }
        else if(command == "MGET") {
            vector<string> keys;
            string nextKey;
            while(ss >> nextKey) {
                keys.push_back(nextKey);
            }
            if(keys.empty()) {
                cout << "Usage: MGET <key1> <key2> ...\n";
                continue;
            }
            vector<string> values = db.MGET(keys);
            for(size_t i = 0; i < values.size(); i++) {
                cout << keys[i] << " : " << values[i] << '\n';
            }
        }
        else if(command == "MSET") {
            vector<pair<string,string>> pairs;
            while(true) {
                string nextKey, nextValue;
                if(!(ss >> nextKey)) break;
                if(!(ss >> nextValue)) {
                    cout << "Usage: MSET <key1> <value1> <key2> <value2> ...\n";
                    pairs.clear();
                    break;
                }
                pairs.push_back({nextKey, nextValue});
            }
            if(!pairs.empty()) {
                db.MSET(pairs);
                cout << "OK" << endl;
            }
        }
        else if(command == "EXPIRE") {
            long long seconds;
            if(!(ss >> key >> seconds)) {
                cout << "Usage: EXPIRE <key> <seconds>\n";
                continue;
            }
            if(seconds <= 0) {
                cout << "Error: Expiration time must be positive\n";
                continue;
            }
            if(db.EXPIRE(key, seconds))
                cout << "OK" << endl;
            else
                cout << "Error: Key Not Found" << endl;
        }
        else if(command == "TTL") {
            if(!(ss >> key)) {
                cout << "Usage: TTL <key>\n";
                continue;
            }
            long long ttl = db.TTL(key);
            if(ttl == -1)
                cout << "Error: Key Not Found or No Expiration Set" << endl;
            else
                cout << ttl << endl;
        }
        else if(command == "SAVE" || command == "LOAD") {
            ss >> key;
            if(key.empty()) {
                cout << "Usage: " << command << " <filename>\n";
                continue;
            }
            if(command == "SAVE") {
                if(db.SAVE(key)) cout<<"Database saved successfully."<<endl;
                else cout<<"Error: Unable to save database."<<endl;
            }
            else if(command == "LOAD") {
                if(db.LOAD(key)) cout<<"Database loaded successfully."<<endl;
                else cout<<"Error: Unable to load database."<<endl;
            }
        }
        else {
            if(command == "SIZE") {
                cout << db.SIZE() << endl;
            }
            else if(command == "KEYS") {
                vector<string> keys = db.KEYS();
                if(keys.empty()) {
                    cout << "No Keys Found" << endl;
                }
                else {
                    for(const auto& key : keys) {
                        cout << key << endl;
                    }
                }
            }
            else if(command == "HELP") {
                cout << "\nAvailable Commands:\n";
                cout << "  SET <key> <value>      - Store a key-value pair\n";
                cout << "  GET <key>              - Retrieve the value of a key\n";
                cout << "  DEL <key>              - Delete a key\n";
                cout << "  EXISTS <key>           - Check if a key exists\n";
                cout << "  KEYS                   - Display all keys\n";
                cout << "  SIZE                   - Display the number of keys\n";
                cout << "  HELP                   - Show this help message\n";
                cout << "  EXIT                   - Exit Redis Lite\n";
                cout << "  SAVE <filename>        - Save database to a file\n";
                cout << "  LOAD <filename>        - Load database from a file\n";
                cout << "  INCR <key>             - Increment the integer value of a key\n";
                cout << "  DECR <key>             - Decrement the integer value of a key\n";
                cout << "  MSET <k1> <v1> ...     - Store multiple key-value pairs\n";
                cout << "  MGET <k1> <k2> ...     - Retrieve multiple values\n";
                cout << "  EXPIRE <key> <seconds> - Set an expiration time (in seconds) for a key\n";
                cout << "  TTL <key>              - Show remaining time to live for a key\n";
            }
            else if(command == "EXIT") {
                running = false;
            }
            else {
                cout << "Unknown Command" << endl;
            }
        }
    }
    return 0;
}