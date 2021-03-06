// Copyright (C) 2009  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef UNITTEST_UTIL_H
#define UNITTEST_UTIL_H 1

#include <vector>
#include <string>

#include <dns/name.h>
#include <dns/message.h>

#include <gtest/gtest.h>

namespace bundy {

class UnitTestUtil {
public:
    ///
    /// read text format wire data from a file and put it to the given vector.
    ///
    static void readWireData(const char* datafile,
                             std::vector<unsigned char>& data);

    ///
    /// add a path that \c readWireData() will search for test data files.
    ///
    static void addDataPath(const std::string& directory);

    ///
    /// convert a sequence of hex strings into the corresponding list of
    /// 8-bit integers, and append them to the vector.
    ///
    static void readWireData(const std::string& datastr,
                             std::vector<unsigned char>& data);

    ///
    /// Compare two names.
    ///
    /// This check method uses \c Name::compare() for comparison, which performs
    /// deeper checks including the equality of offsets, and should be better
    /// than EXPECT_EQ, which uses operater==.  Like the \c matchWireData()
    /// method, the usage is a bit awkward; the caller should use
    /// \c EXPECT_PRED_FORMAT2.
    ///
    static ::testing::AssertionResult
    matchName(const char* nameexp1, const char* nameexp2,
              const bundy::dns::Name& name1, const bundy::dns::Name& name2);

    ///
    /// Populate a request message
    ///
    /// Create a request message in 'request_message' using the 
    /// opcode 'opcode' and the name/class/type query tuple specified in
    /// 'name', 'rrclass' and 'rrtype.
    static void
    createRequestMessage(bundy::dns::Message& request_message,
                         const bundy::dns::Opcode& opcode,
                         const uint16_t qid,
                         const bundy::dns::Name& name,
                         const bundy::dns::RRClass& rrclass,
                         const bundy::dns::RRType& rrtype);

    ///
    /// Populate a DNSSEC request message
    ///
    /// Create a request message in 'request_message' using the
    /// opcode 'opcode' and the name/class/type query tuple specified in
    /// 'name', 'rrclass' and 'rrtype.
    /// EDNS will be added with DO=1 and bufsize 4096
    static void
    createDNSSECRequestMessage(bundy::dns::Message& request_message,
                               const bundy::dns::Opcode& opcode,
                               const uint16_t qid,
                               const bundy::dns::Name& name,
                               const bundy::dns::RRClass& rrclass,
                               const bundy::dns::RRType& rrtype);

};
}
#endif // UNITTEST_UTIL_H

// Local Variables: 
// mode: c++
// End: 
