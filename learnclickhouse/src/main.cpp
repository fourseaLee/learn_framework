#include <clickhouse/client.h>
#include <iostream>
/**
using namespace clickhouse;

   DECLARE_FIELD(host, std::string, SetHost, std::string());
    /// Service port.
    DECLARE_FIELD(port, unsigned int, SetPort, 9000);

    /// Default database.
    DECLARE_FIELD(default_database, std::string, SetDefaultDatabase, "default");
    /// User name.
    DECLARE_FIELD(user, std::string, SetUser, "default");
    /// Access password.
    DECLARE_FIELD(password, std::string, SetPassword, std::string());

    /// By default all exceptions received during query execution will be
    /// passed to OnException handler.  Set rethrow_exceptions to true to
    /// enable throwing exceptions with standard c++ exception mechanism.
    DECLARE_FIELD(rethrow_exceptions, bool, SetRethrowException, true);

    /// Ping server every time before execute any query.
    DECLARE_FIELD(ping_before_query, bool, SetPingBeforeQuery, false);
    /// Count of retry to send request to server.
    DECLARE_FIELD(send_retries, unsigned int, SetSendRetries, 1);
    /// Amount of time to wait before next retry.
    DECLARE_FIELD(retry_timeout, std::chrono::seconds, SetRetryTimeout, std::chrono::seconds(5));

    /// Compression method.
    DECLARE_FIELD(compression_method, CompressionMethod, SetCompressionMethod, CompressionMethod::None);


*/

using namespace clickhouse;


void BasicTest()
{
  /// Initialize client connection.

    ClientOptions client_opt;

    client_opt.SetUser("default");
    client_opt.SetHost("172.31.35.1");
  //  client_opt.SetPort(8123);
    client_opt.SetDefaultDatabase("default");
    client_opt.SetPassword("abcd4321");
    Client client(client_opt);



    /// Create a table.
    client.Execute("CREATE TABLE IF NOT EXISTS numbers (id UInt64, name String) ENGINE = Memory");
    /// Insert some values.
    {
        Block block;

        auto id = std::make_shared<ColumnUInt64>();
        id->Append(1);
        id->Append(7);

        auto name = std::make_shared<ColumnString>();
        name->Append("one");
        name->Append("seven");

        block.AppendColumn("id"  , id);
        block.AppendColumn("name", name);

        client.Insert("numbers", block);
    }

    /// Select values inserted in the previous step.
    client.Select("SELECT id, name FROM numbers", [] (const Block& block)
            {
            for (size_t i = 0; i < block.GetRowCount(); ++i) {
            std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
            << block[1]->As<ColumnString>()->At(i) << "\n";
            }
            }
            );

    /// Delete table.
   // client.Execute("DROP TABLE test.numbers");
}

void StandardTest()
{
	ClientOptions client_opt;

	client_opt.SetUser("default");
	client_opt.SetHost("192.168.1.27");
	//  client_opt.SetPort(8123);
	client_opt.SetDefaultDatabase("wallet_report_prod");
	client_opt.SetPassword("123456");
	Client client(client_opt);

	/// Select values inserted in the previous step.
	std::string sql = "SELECT  DISTINCT `from` from block_number_transaction;";
	std::vector<std::string> vect_address;
	client.Select(sql,[&vect_address] (const Block& block)
	{
		 for (size_t i = 0; i < block.GetRowCount(); ++i) 
		 { 
            auto address_view =  block[0]->As<ColumnString>()->At(i);
			std::string address(address_view);
			vect_address.push_back(address);
         }
	}
	);
	std::cout << "from size:  " << vect_address.size()  << std::endl;

	sql = "SELECT  DISTINCT `to` from block_number_transaction;";
	client.Select(sql,[&vect_address] (const Block& block)
	{
		 for (size_t i = 0; i < block.GetRowCount(); ++i) 
		 {
            auto address_view =  block[0]->As<ColumnString>()->At(i);
			std::string address(address_view);
			vect_address.push_back(address);
         }
	}
	);
	std::cout << "total size: "  << vect_address.size() << std::endl;

	for (size_t i = 0; i < vect_address.size(); i++)
	{
		std::cout << vect_address[i] << std::endl;
	}
	/// Delete table.
	// client.Execute("DROP TABLE test.numbers");

}
int main(int argc, char* argv[])
{
	StandardTest();
    return 0;
}
