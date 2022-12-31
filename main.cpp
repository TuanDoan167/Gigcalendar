/* COMP.CS.110 Project 2: GIGS
 * ===========================
 * ===========================
 *
 *      Acts as a simple gig calendar with n commands:
 * ARTISTS - Prints all known artist names
 * ARTIST <artist name> - Prints single artist's gigs
 * STAGES - Prints all known stage names and their locations
 * STAGE <stage name> - Prints all artists having a gig at the given stage
 * ADD_ARTIST <artist name> - Adds a new artist
 * ADD_GIG <artist name> <date> <town name> <stage name> - Adds a new gig for
 * an artist with the given date, town, and stage (the artist can't already
 * have a gig on the same date and there can't be other gigs at the same stage
 * at the same time)
 * CANCEL <artist name> <date> - Cancels artist's gigs after the given date
 *
 *      The data file's lines should be in format
 * artist_name;date;town_name;stage_name.
 * Otherwise the program execution terminates instantly (but still gracefully).
 * */

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
using namespace std;
// Farthest year for which gigs can be allocated
const std::string FARTHEST_POSSIBLE_YEAR = "2030";

// Casual split func, if delim char is between "'s, ignores it.
std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> result = {""};
    bool inside_quotation = false;
    for ( char current_char : str )
    {
        if ( current_char == '"' )
        {
            inside_quotation = not inside_quotation;
        }
        else if ( current_char == delim and not inside_quotation )
        {
            result.push_back("");
        }
        else
        {
            result.back().push_back(current_char);
        }
    }
    if ( result.back() == "" )
    {
        result.pop_back();
    }
    return result;
}

// Checks if the given date_str is a valid date, i.e. if it has the format
// dd-mm-yyyy and if it is also otherwise possible date.
bool is_valid_date(const std::string& date_str)
{
    std::vector<std::string> date_vec = split(date_str, '-');
    if ( date_vec.size() != 3 )
    {
        return false;
    }

    std::string year = date_vec.at(0);
    std::string month = date_vec.at(1);
    std::string day = date_vec.at(2);
    std::vector<std::string> month_sizes
            = { "31", "28", "31", "30", "31", "30",
                "31", "31", "30", "31", "30", "31" };

    if ( month < "01" or month > "12" )
    {
        return false;
    }
    if ( year < "0001" or year > FARTHEST_POSSIBLE_YEAR )
    {
        return false;
    }
    unsigned int year_int = stoi(year);
    bool is_leap_year = (year_int % 400 == 0) or
            (year_int % 100 != 0 and year_int % 4 == 0);
    if ( day >= "01" and
         (day <= month_sizes.at(stoi(month) - 1) or
          (month == "02" and is_leap_year and day <= "29")) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    cout<<"Give a name for input file: ";
    string filename;
    getline(cin, filename);
    ifstream file_object(filename);
    if ( not file_object ) {
        cout << "Error: File could not be read."<<endl;
        return EXIT_FAILURE;
    }

    else {
        typedef std::pair<std::string, std::string> pairofstring;
        // I create two map to save the content of file.
        // The first map have a pair of artist and date as search key and pair of town and stage as mapped value.
        map<pairofstring,pairofstring> MapofArtist;
        // The second map has pair of town stage date as search key and artist as mapped value.
        map<pairofstring,string> MapofStage;
        set<string> Artist;
        string line;
        while (getline(file_object,line)){
            vector<string> data=split(line,';');
            string artist=data.at(0);
            string date=data.at(1);
            string town=data.at(2);
            string stage=data.at(3);

            // The if condition is to ensure that the program read only line that correspond to the specification.
            if(data.size()!=4 or artist.empty() or date.empty() or town.empty() or stage.empty() ){
                cout<<"Error: Invalid format in file."<<endl;
                return EXIT_FAILURE;
            }
            // Check if the date is valid
            else if(not is_valid_date(date)){
                cout<<"Error: Invalid date."<<endl;
                return EXIT_FAILURE;}
            else{
                Artist.insert(artist);
                // this pair is used as search key for the MapofArtist map.
                pairofstring artistDate = make_pair(artist, date);
                // this pair is used as mapped value for the MapofArtist map.
                pairofstring townStage = make_pair(town,stage);
                // this pair is used as search key for the MapofStage map.
                pairofstring StageDate = make_pair(stage, date);
                if ( MapofArtist.find(artistDate) != MapofArtist.end() or MapofStage.find(StageDate) != MapofStage.end()) {
                    // check double bookings for artist and stage.
                    cout<<"Error: Already exists."<<endl;
                    return EXIT_FAILURE;
                }
                else {
                    MapofArtist.insert(pair<pairofstring,pairofstring>(artistDate,townStage));
                    MapofStage.insert(pair<pairofstring,string>(StageDate,data.at(0)));
                }
            }
        }

        while(true)
        {
            std::string commandLine;
            std::cout << "gigs> ";
            getline(std::cin, commandLine);
            std::vector<std::string> commandParts = split(commandLine, ' ');

            if (commandParts.empty()){
                cout<<"Error: Invalid input."<<endl;
                continue;
            }

            std::string command = commandParts.at(0);

            if(command == "ARTISTS" or command=="artists"){
                cout<<"All artists in alphabetical order:"<<endl;
                for (const auto &entry: Artist)
                {
                    std::cout <<entry<<endl;
                }
            }

            else if(command == "ARTIST" or command=="artist")
            {
                if (commandParts.size()<2){
                    cout<<"Error: Invalid input."<<endl;
                    continue;
                }

                string artist=commandParts.at(1);

                if(Artist.find(artist)==Artist.end()){
                    // If the artist in not found in the set of artist
                    cout<<"Error: Not found."<<endl;
                    continue;
                }
                else {
                    cout<<"Artist "<<artist<<" has the following gigs in the order they are listed: "<<endl;
                    for (const auto &entry: MapofArtist){
                        if(entry.first.first==artist){
                            cout<<" - "<<entry.first.second<<" : "<<entry.second.first<<", "<<entry.second.second<<endl;
                        }
                    }
                }
            }

            else if(command == "STAGES" or command=="stages")
            {   set<pairofstring> TownStage;
                for (const auto& entry:MapofArtist){
                    pairofstring townstage=make_pair(entry.second.first,entry.second.second);
                    TownStage.insert(townstage);
                }
                cout<<"All gig places in alphabetical order:"<<endl;
                for (const auto &entry : TownStage){
                    cout<<entry.first<<", "<<entry.second<<endl;
                }
            }

            else if(command == "STAGE" or command=="stage")
            {  if (commandParts.size()<2){
                    cout<<"Error: Invalid input."<<endl;
                    continue;
                }
                set<string>Stages;
                for (const auto& entry:MapofArtist){
                    string data=entry.second.second;
                    Stages.insert(data);
                }
                string stage=commandParts.at(1);
                if(Stages.find(stage)==Stages.end()){
                    // If the stage in not found in the set of stage
                    cout<<"Error: Not found."<<endl;
                    continue;
                }
                else{
                    cout<<"Stage "<< stage<<" has gigs of the following artists:"<<endl;
                    for(const auto&entry : MapofArtist){
                        if(entry.second.second==stage){
                            cout<<" - "<<entry.first.first<<endl;
                        }
                    }
                }
            }

            else if(command == "ADD_ARTIST" or command=="add_artist"){
                if (commandParts.size()<2){
                    cout<<"Error: Invalid input."<<endl;
                    continue;
                }
                string newartist=commandParts.at(1);
                if( Artist.find(newartist)==Artist.end()){
                    Artist.insert(newartist);
                    cout<<"Artist added."<<endl;
                }
                else {
                    cout<<"Error: Already exists."<<endl;
                }
            }

            else if(command == "ADD_GIG" or command=="add_gig"){
                if (commandParts.size()<5){
                    cout<<"Error: Invalid input."<<endl;
                    continue;
                }
                string artist=commandParts.at(1);
                string date=commandParts.at(2);
                string town=commandParts.at(3);
                string stage=commandParts.at(4);
                if(Artist.find(artist)==Artist.end()){
                    cout<<"Error: Not found."<<endl;
                    continue;
                }
                if(not is_valid_date(date)){
                    cout<<"Error: Invalid date."<<endl;
                    continue;
                }
                pairofstring townStage = make_pair(town,stage);
                pairofstring artistDate = make_pair(artist, date);
                pairofstring StageDate = make_pair(stage, date);
                if ( MapofArtist.find(artistDate) != MapofArtist.end() or MapofStage.find(StageDate) != MapofStage.end()) {
                    cout<<"Error: Already exists."<<endl;
                }
                else {
                    MapofArtist.insert(pair<pairofstring,pairofstring>(artistDate,townStage));
                    MapofStage.insert(pair<pairofstring,string>(StageDate,artist));
                    cout<<"Gig added."<<endl;
                }
            }

            else if(command == "CANCEL" or command=="cancel"){
                if(commandParts.size()<3){
                    cout<<"Error: Invalid input."<<endl;
                    continue;
                }
                string artist = commandParts.at(1);
                if(Artist.find(artist)==Artist.end()){
                    cout<<"Error: Not found."<<endl;
                    continue;
                }
                string date = commandParts.at(2);
                if(not is_valid_date(date)){
                    cout<<"Error: Invalid date."<<endl;
                    continue;
                }

                pairofstring artistDate = make_pair(artist, date);

                using Iter = map<pairofstring,pairofstring>::iterator;
                Iter it  = MapofArtist.begin();
                Iter end = MapofArtist.end();
                int n=0;
                // Traverse through the map by using iterator and erase the element meets the condition, the code is borrowed from here https://stackoverflow.com/questions/4600567/how-can-i-delete-elements-of-a-stdmap-with-an-iterator
                while (it != end) {
                    if (it->first.first==artist and it->first.second>date) {
                        MapofArtist.erase(it++);
                        ++n;
                    } else {
                        ++it;
                    }
                }

                using Iter1 = map<pairofstring,string>::iterator;
                Iter1 it1  = MapofStage.begin();
                Iter1 end1 = MapofStage.end();
                while (it1 != end1) {
                    if (it1->second==artist and it1->first.second>date) {
                        MapofStage.erase(it1++);
                    } else {
                        ++it1;
                    }
                }

                if(n==0){
                    cout<<"Error: No gigs after the given date."<<endl;
                    continue;
                }
                else {
                    cout<<"Artist's gigs after the given date cancelled."<<endl;
                }
            }

            else if(command == "QUIT" or command=="quit" )
            {
                return EXIT_SUCCESS;
            }

            else
            {
                cout << "Error: Invalid input." <<endl;
            }
        }
    }
    return EXIT_SUCCESS;
}
