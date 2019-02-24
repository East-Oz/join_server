#pragma once

#include "CommandObject.h"
#include <vector>
#include <utility>

using namespace std;
typedef map<int, string> table_map;
typedef pair<int, string> table_map_pair;

class DataBase
{
public:
	DataBase();
	~DataBase();
	string ParseCommand( const string& request );
private:
	map<string, table_map> m_mapDataBase;
	string ProcessCommand( CommandObject& cmd );

};

