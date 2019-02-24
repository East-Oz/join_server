#include "DataBase.h"
#include <sstream>

DataBase::DataBase()
{
	table_map mapA;
	m_mapDataBase[ "A" ] = mapA;
	table_map mapB;
	m_mapDataBase[ "B" ] = mapB;
}

DataBase::~DataBase()
{}

string DataBase::ParseCommand( const string& request )
{
	string response;

	vector<string> elements;
	std::stringstream ss( request );
	std::string item;
	while( std::getline( ss, item, ' ' ) )
	{
		elements.push_back( item );
	}
	if( elements.size() > 0 )
	{
		CommandObject cmd( elements );
		response = ProcessCommand( cmd );
	}
	return response;
}

string  DataBase::ProcessCommand( CommandObject& cmd )
{
	string response = "OK";
	ECOMMAND eCommand = cmd.GetCommand();

	if( eCommand == ECOMMAND::INSERT )
	{
		pair<table_map::iterator, bool> result;
		result = m_mapDataBase[ cmd.GetTable() ].insert( table_map_pair( cmd.GetId(), cmd.GetValue() ) );
		if( result.second == false )
		{
			response = "ERR duplicate ";
			response.append( to_string( result.first->first ) );
		}
	}
	else if( eCommand == ECOMMAND::TRUNCATE )
	{
		table_map& map = m_mapDataBase[ cmd.GetTable() ];
		map.clear();
		if( map.size() != 0 )
		{
			response = "ERR truncate ";
			response.append( cmd.GetTable() );
		}
	}
	else if( eCommand == ECOMMAND::INTERSECTION )
	{
		response.clear();
		vector<string> result_vector;
		// only for 2 tables
		table_map& tableA = m_mapDataBase.at( "A" );
		table_map& tableB = m_mapDataBase.at( "B" );
		
		table_map::iterator iterB;
		for( table_map::iterator it = tableA.begin(); it != tableA.end(); ++it )
		{
			iterB = tableB.find( it->first );
			if( iterB != tableB.end() )
			{
				string _s = to_string(it->first);
				_s.append( "," );
				_s.append( it->second );
				_s.append( "," );
				_s.append( iterB->second );
				result_vector.push_back( _s );
			}
		}

		for( auto v : result_vector )
		{
			response.append( v ).append( "\n" );
		}
		response.append( "OK" );
	}
	else if( eCommand == ECOMMAND::SYMMETRIC_DIFFERENCE )
	{
		response.clear();
		vector<string> result_vector;
		// only for 2 tables
		table_map& tableA = m_mapDataBase.at( "A" );
		table_map& tableB = m_mapDataBase.at( "B" );

		table_map::iterator iterB;
		for( table_map::iterator it = tableA.begin(); it != tableA.end(); ++it )
		{
			iterB = tableB.find( it->first );
			if( iterB == tableB.end() )
			{
				string _s = to_string( it->first );
				_s.append( "," );
				_s.append( it->second );
				_s.append( "," );
				_s.append( iterB!=tableB.end()?iterB->second:string() );
				result_vector.push_back( _s );
			}
		}

		table_map::iterator iterA;
		for( table_map::iterator it = tableB.begin(); it != tableB.end(); ++it )
		{
			iterA = tableA.find( it->first );
			if( iterA == tableA.end() )
			{
				string _s = to_string( it->first );
				_s.append( "," );
				_s.append( iterA != tableA.end()?iterA->second: string() );
				_s.append( "," );
				_s.append( it->second );
				result_vector.push_back( _s );
			}
		}

		for( auto v : result_vector )
		{
			response.append( v ).append( "\n" );
		}
		response.append( "OK" );
	}

	return response;
}