#pragma once
#include <string>
#include <map>
#include <vector>

using namespace std;

enum class ECOMMAND
{
	INSERT,
	TRUNCATE,
	INTERSECTION,
	SYMMETRIC_DIFFERENCE,
	NOT_IMPLEMENTED
};

static map<string, ECOMMAND> COMMANDS =
{
	{ "INSERT", ECOMMAND::INSERT }
	, { "TRUNCATE", ECOMMAND::TRUNCATE }
	, { "INTERSECTION", ECOMMAND::INTERSECTION }
	, { "SYMMETRIC_DIFFERENCE", ECOMMAND::SYMMETRIC_DIFFERENCE }
};

class CommandObject
{
public:
	CommandObject(  ): m_eCommand( ECOMMAND::NOT_IMPLEMENTED ), m_nId(-1) {}
	CommandObject( string& command, string& table, int id, string& value ): 
		m_sTable( table )
		, m_nId( id )
		, m_sValue( value )
	{
		SetCommand( command );
	}
	CommandObject( vector<string>& elements ): m_eCommand( ECOMMAND::NOT_IMPLEMENTED ), m_nId( -1 )
	{
		if( elements.size() > 0 )
		{
			try
			{
				SetCommand( elements.at( 0 ) );
			}
			catch( ... ) { }
		}
		if( elements.size() > 1 )
		{
			try
			{
				 m_sTable = elements.at( 1 );
			}
			catch( ... ) { }
		}
		if( elements.size() > 2 )
		{
			try
			{
				m_nId = stoi( elements.at( 2 ) );
			}
			catch( ... ) {}
		}
		if( elements.size() > 3 )
		{
			try
			{
				m_sValue = elements.at( 3 );
			}
			catch( ... ) {}
		}
	}
	virtual ~CommandObject() = default;
	virtual ECOMMAND GetCommand() { return m_eCommand; }
	virtual string GetTable() { return m_sTable; }
	virtual int GetId() { return m_nId; }
	virtual string GetValue() { return m_sValue; }
	virtual void SetCommand( string command )
	{
		try
		{
			m_eCommand = COMMANDS.at( command );
		}
		catch( ... ) { m_eCommand = ECOMMAND::NOT_IMPLEMENTED; }
	}

private:
	ECOMMAND m_eCommand;
	string m_sTable;
	int m_nId;
	string m_sValue;

};
