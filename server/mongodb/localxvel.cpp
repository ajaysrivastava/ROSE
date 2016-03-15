/* Written by Cedric Blake

This script is used to push the displacement values of each encoder
to the database "rosedb"

currently, this script only simulates the encoder values by using
a random number generator. the while loop breaks after 5 iterations

*/

#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>
#include <unistd.h> /* used to sleep */

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using namespace std;

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;

int main() {
	//used to create a client connection and connect to a mongo instance
	mongocxx::instance inst{};
	//NOTE: make sure to have "mongocxx::uri{}"
	mongocxx::client conn{mongocxx::uri{}};

	auto db = conn["rosedb"];
	auto encoders = db["encoders"];

	encoders.delete_many({});

	bsoncxx::document::value initializer = 
	document{} << "encoder values" << open_array
		<< 0 << 0 
		<< 0 << 0 
		<< close_array << finalize;

	printf("Inserting first encoder value\n");

	encoders.insert_one(std::move(initializer));



	srand(time(NULL));

	int displacement[4];
	int d;

	int count = 5;

	while (count > 0){

		bsoncxx::builder::stream::document encoderVals;

		//used to update all the values in the encoderVals document
		//initialized within the while loop so that it is reset
		//and does not have multiple "$set" documents appended
		bsoncxx::builder::stream::document updateVals;

		for (int i = 0; i < 4; ++i) {
			d = rand() % 100;
			displacement[i] = d;
			//cout<<displacement[i]<<endl;
		}

		//update values here
		updateVals << "$set" << open_document
			<< "encoder values" << open_array
			<< displacement[0] << displacement[1]
			<< displacement[2] << displacement[3]
			<< close_array << close_document;

		encoderVals << "encoder values" << open_document
		<< "$exists" << true << close_document;

		encoders.update_one(encoderVals.view(), updateVals.view());

		auto cursor = encoders.find({}); //"encoders" is the collection
		//printing out for testing purposes
		for (auto&& doc : cursor ) {
			cout<<bsoncxx::to_json(doc)<<endl;
		}

		count--;
		usleep(1000000); //sleep for 1 second
	}

	return 0;
 
}