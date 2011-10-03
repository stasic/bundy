namespace {

const char* const DataSourceClient_doc = "\
The base class of data source clients.\n\
\n\
This is the python wrapper for the abstract base class that defines\n\
the common interface for various types of data source clients. A data\n\
source client is a top level access point to a data source, allowing \n\
various operations on the data source such as lookups, traversing or \n\
updates. The client class itself has limited focus and delegates \n\
the responsibility for these specific operations to other (c++) classes;\n\
in general methods of this class act as factories of these other classes.\n\
\n\
- InMemoryClient: A client of a conceptual data source that stores all\n\
  necessary data in memory for faster lookups\n\
- DatabaseClient: A client that uses a real database backend (such as\n\
  an SQL database). It would internally hold a connection to the\n\
  underlying database system.\n\
\n\
It is intentional that while the term these derived classes don't\n\
contain \"DataSource\" unlike their base class. It's also noteworthy\n\
that the naming of the base class is somewhat redundant because the\n\
namespace datasrc would indicate that it's related to a data source.\n\
The redundant naming comes from the observation that namespaces are\n\
often omitted with using directives, in which case \"Client\" would be\n\
too generic. On the other hand, concrete derived classes are generally\n\
not expected to be referenced directly from other modules and\n\
applications, so we'll give them more concise names such as\n\
InMemoryClient. A single DataSourceClient object is expected to handle\n\
only a single RR class even if the underlying data source contains\n\
records for multiple RR classes. Likewise, (when we support views) a\n\
DataSourceClient object is expected to handle only a single view.\n\
\n\
If the application uses multiple threads, each thread will need to\n\
create and use a separate DataSourceClient. This is because some\n\
database backend doesn't allow multiple threads to share the same\n\
connection to the database.\n\
\n\
For a client using an in memory backend, this may result in having a\n\
multiple copies of the same data in memory, increasing the memory\n\
footprint substantially. Depending on how to support multiple CPU\n\
cores for concurrent lookups on the same single data source (which is\n\
not fully fixed yet, and for which multiple threads may be used), this\n\
design may have to be revisited. This class (and therefore its derived\n\
classes) are not copyable. This is because the derived classes would\n\
generally contain attributes that are not easy to copy (such as a\n\
large size of in memory data or a network connection to a database\n\
server). In order to avoid a surprising disruption with a naive copy\n\
it's prohibited explicitly. For the expected usage of the client\n\
classes the restriction should be acceptable.\n\
\n\
Todo: This class is still not complete. It will need more factory\n\
methods, e.g. for (re)loading a zone.\n\
";

const char* const DataSourceClient_findZone_doc = "\
find_zone(name) -> (code, ZoneFinder)\n\
\n\
Returns a ZoneFinder for a zone that best matches the given name.\n\
\n\
code: The result code of the operation (integer).\n\
- DataSourceClient.SUCCESS: A zone that gives an exact match is found\n\
- DataSourceClient.PARTIALMATCH: A zone whose origin is a super domain of name\n\
  is found (but there is no exact match)\n\
- DataSourceClient.NOTFOUND: For all other cases.\n\
ZoneFinder: ZoneFinder object for the found zone if one is found;\n\
otherwise None.\n\
\n\
Any internal error will be raised as an isc.datasrc.Error exception\n\
\n\
Parameters:\n\
  name       A domain name for which the search is performed.\n\
\n\
Return Value(s): A tuple containing a result value and a ZoneFinder object or\n\
None\n\
";

const char* const DataSourceClient_getIterator_doc = "\
get_iterator(name) -> ZoneIterator\n\
\n\
Returns an iterator to the given zone.\n\
\n\
This allows for traversing the whole zone. The returned object can\n\
provide the RRsets one by one.\n\
\n\
This throws isc.datasrc.Error when the zone does not exist in the\n\
datasource, or when an internal error occurs.\n\
\n\
The default implementation throws isc.datasrc.NotImplemented. This allows for\n\
easy and fast deployment of minimal custom data sources, where the\n\
user/implementator doesn't have to care about anything else but the\n\
actual queries. Also, in some cases, it isn't possible to traverse the\n\
zone from logic point of view (eg. dynamically generated zone data).\n\
\n\
It is not fixed if a concrete implementation of this method can throw\n\
anything else.\n\
\n\
Parameters:\n\
  isc.dns.Name The name of zone apex to be traversed. It doesn't do\n\
               nearest match as find_zone.\n\
\n\
Return Value(s): Pointer to the iterator.\n\
";

const char* const DataSourceClient_getUpdater_doc = "\
get_updater(name, replace) -> ZoneUpdater\n\
\n\
Return an updater to make updates to a specific zone.\n\
\n\
The RR class of the zone is the one that the client is expected to\n\
handle (see the detailed description of this class).\n\
\n\
If the specified zone is not found via the client, a NULL pointer will\n\
be returned; in other words a completely new zone cannot be created\n\
using an updater. It must be created beforehand (even if it's an empty\n\
placeholder) in a way specific to the underlying data source.\n\
\n\
Conceptually, the updater will trigger a separate transaction for\n\
subsequent updates to the zone within the context of the updater (the\n\
actual implementation of the \"transaction\" may vary for the specific\n\
underlying data source). Until commit() is performed on the updater,\n\
the intermediate updates won't affect the results of other methods\n\
(and the result of the object's methods created by other factory\n\
methods). Likewise, if the updater is destructed without performing\n\
commit(), the intermediate updates will be effectively canceled and\n\
will never affect other methods.\n\
\n\
If the underlying data source allows concurrent updates, this method\n\
can be called multiple times while the previously returned updater(s)\n\
are still active. In this case each updater triggers a different\n\
\"transaction\". Normally it would be for different zones for such a\n\
case as handling multiple incoming AXFR streams concurrently, but this\n\
interface does not even prohibit an attempt of getting more than one\n\
updater for the same zone, as long as the underlying data source\n\
allows such an operation (and any conflict resolution is left to the\n\
specific implementation).\n\
\n\
If replace is true, any existing RRs of the zone will be deleted on\n\
successful completion of updates (after commit() on the updater); if\n\
it's false, the existing RRs will be intact unless explicitly deleted\n\
by delete_rrset() on the updater.\n\
\n\
A data source can be \"read only\" or can prohibit partial updates. In\n\
such cases this method will result in an isc.datasrc.NotImplemented exception\n\
unconditionally or when replace is false).\n\
\n\
Exceptions:\n\
  isc.datasrc. NotImplemented The underlying data source does not support\n\
               updates.\n\
  isc.datasrc.Error Internal error in the underlying data source.\n\
\n\
Parameters:\n\
  name       The zone name to be updated\n\
  replace    Whether to delete existing RRs before making updates\n\
\n\
";
} // unnamed namespace