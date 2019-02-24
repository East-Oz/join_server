#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <utility>
#include <iosfwd>
#include <fstream>
#include <boost/bind.hpp>

#include "DataBase.h"

using namespace std;
using namespace boost::asio;
using namespace boost::posix_time;
io_service service;

class ClientConnection;
typedef boost::shared_ptr<ClientConnection> client_ptr;

typedef std::vector<client_ptr> clients_array_type;
clients_array_type clients;

int gCount = 0;
int gClient = 1;

DataBase* gDataBase = nullptr;

class ClientConnection: public boost::enable_shared_from_this<ClientConnection>
	, boost::noncopyable
{
	typedef ClientConnection self_type;
	ClientConnection( ): socket( service ), _started( false )
		
	{
		_client_id = gClient;
		gClient++;
	}
public:
	typedef boost::system::error_code error_code;
	typedef boost::shared_ptr<ClientConnection> ptr;

	void Start()
	{
		_started = true;
		clients.push_back( shared_from_this() );
		Read();
	}
	static ptr Create()
	{
		ptr new_( new ClientConnection );
		return new_;
	}
	void Stop()
	{
		if( !_started ) return;
		_started = false;
		socket.close();

		ptr self = shared_from_this();
		clients_array_type::iterator it = std::find( clients.begin(), clients.end(), self );
		clients.erase( it );
	}
	bool Started() const { return _started; }
	ip::tcp::socket & sock() { return socket; }
	
private:
	void OnRead( const error_code & err, size_t size )
	{
		if( err ) Stop();
		if( !Started() ) return;

		std::string request( read_buffer, size );
#ifdef _DEBUG
		std::cout << "cl: " << _client_id << " IN: " << request << std::endl;
#else
		std::cout << "> " << request;
#endif
		gCount++;
		std::string response = gDataBase->ParseCommand( request );

#ifdef _DEBUG
		std::cout << "cl: " << _client_id << " OUT: " << response << std::endl;
#else
		std::cout << "< " << request;
#endif
		Write( response );
	}

	void OnWrite( const error_code & err, size_t bytes )
	{
		Read();
	}
	void Read()
	{
		socket.async_receive( buffer( read_buffer ), boost::bind( &self_type::OnRead, shared_from_this( ), _1, _2 ) );
	}

	void Write( const std::string& response )
	{
		if( !Started() ) return;

		memcpy( write_buffer, &response, sizeof( response ) );

		socket.async_write_some( buffer( write_buffer, sizeof( response ) ),
			boost::bind( &self_type::OnWrite, shared_from_this(), _1, _2 )
			);

	}

private:
	ip::tcp::socket socket;
	char read_buffer[ 1000 ];
	char write_buffer[ 1000 ];
	bool _started;
	int _client_id;
};


struct SAcceptor
{
	~SAcceptor() { if( m_acceptor ) delete m_acceptor; }
	ip::tcp::acceptor* m_acceptor = nullptr;
}tagAcceptor;

void Acceptor( ClientConnection::ptr client, const boost::system::error_code & err )
{
	client->Start();
	ClientConnection::ptr new_client = ClientConnection::Create();
	tagAcceptor.m_acceptor->async_accept( new_client->sock(), boost::bind( Acceptor, new_client, _1 ) );
}


int main( int argc, char* argv[] )
{
	int port = 9000;
	if( argc > 1 )
		port = std::stoi( argv[ 1 ] );

	gDataBase = new DataBase();

	tagAcceptor.m_acceptor = new ip::tcp::acceptor( service, ip::tcp::endpoint( ip::tcp::v4(), port ) );

	ClientConnection::ptr client = ClientConnection::Create();
	tagAcceptor.m_acceptor->async_accept( client->sock(), boost::bind( Acceptor, client, _1 ) );
	service.run();
}

# for build