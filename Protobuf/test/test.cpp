//
// Created by lixiaoqing on 2023/7/10.
//

#include <iostream>

#include "pb_helper.h"
#include "person.pb.h"

namespace PbTest {
    void testToBinary() {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        tutorial::Person person;
        person.set_name("zhangsan");
        person.set_id(1);
        person.set_email("123456@email.com");

        auto phone1 = person.add_phones();
        phone1->set_number("123456789");
        phone1->set_type(tutorial::Person::WORK);

        auto phone2 = person.add_phones();
        phone2->set_number("987654321");
        phone2->set_type(tutorial::Person::HOME);

        std::string s;
        PbHelper::ToBinary(person, s);
        std::cout << s << std::endl;

        google::protobuf::ShutdownProtobufLibrary();
    }

    void testToString() {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        tutorial::Person person;
        person.set_name("zhangsan");
        person.set_id(1);
        person.set_email("123456@email.com");

        auto phone1 = person.add_phones();
        phone1->set_number("123456789");
        phone1->set_type(tutorial::Person::WORK);

        auto phone2 = person.add_phones();
        phone2->set_number("987654321");
        phone2->set_type(tutorial::Person::HOME);

        std::string s;
        PbHelper::ToString(person, s);
        std::cout << s << std::endl;
    }

    void testToJson() {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        tutorial::Person person;
        person.set_name("zhangsan");
        person.set_id(1);
        person.set_email("123456@email.com");

        auto phone1 = person.add_phones();
        phone1->set_number("123456789");
        phone1->set_type(tutorial::Person::WORK);

        auto phone2 = person.add_phones();
        phone2->set_number("987654321");
        phone2->set_type(tutorial::Person::HOME);

        std::string json;
        PbHelper::ToJson(person, json);
        std::cout << json << std::endl;
    }

    void testParseFromString() {
        GOOGLE_PROTOBUF_VERIFY_VERSION;

        tutorial::Person person;
        person.set_name("zhangsan");
        person.set_id(1);
        person.set_email("123456@email.com");

        auto phone1 = person.add_phones();
        phone1->set_number("123456789");
        phone1->set_type(tutorial::Person::WORK);

        auto phone2 = person.add_phones();
        phone2->set_number("987654321");
        phone2->set_type(tutorial::Person::HOME);

        std::string s;
        PbHelper::ToBinary(person, s);

        tutorial::Person p2;
        PbHelper::ParseFromBinary(s, p2);

        std::cout << p2.name() << std::endl;
        std::cout << p2.email() << std::endl;
        for (auto &phone: p2.phones()) {
            std::cout << phone.number() << ", " << phone1->type() << std::endl;
        }

        // Optional:  Delete all global objects allocated by libprotobuf.
        google::protobuf::ShutdownProtobufLibrary();
    }

    void testParseFromJson() {
        std::string json = R"({"name":"zhangsan","id":1,"email":"123456@email.com","phones":[{"number":"123456789","type":2},{"number":"987654321","type":1}]})";
        tutorial::Person person;
        std::cout << std::boolalpha
                  << PbHelper::ParseFromJson(json, person) << std::endl;
        std::cout << "id: " << person.id() << std::endl;
        std::cout << "name: " << person.name() << std::endl;
        std::cout << "email: " << person.email() << std::endl;
        for (auto &phone: person.phones()) {
            std::cout << "number: " << phone.number() << ", "
                      << "type: " << phone.type() << std::endl;
        }
    }
}

int main() {
    // 经过测试，不能同时调用多个 testXXX 方法，同时只能运行一个方法，否则程序会崩，原因未知-_-
    std::cout << "===========================================" << std::endl;

//    PbTest::testToBinary();
//    PbTest::testToString();
//    PbTest::testToJson();
//    PbTest::testParseFromString();
    PbTest::testParseFromJson();

    std::cout << "===========================================" << std::endl;
    return 0;
}