#include "../partB/base_event.h"
#include "../partB/closed_event.h"
#include "../partB/custom_event.h"
#include "../partA/date_wrap.h"
#include "../partB/event_container.h"
#include "../partA/exceptions.h"
#include "../partB/festival.h"
#include "../partB/one_time_event.h"
#include "../partB/open_event.h"
#include "../partB/recurring_event.h"
#include "../partC/schedule.h"
#include <cstdlib>
#include <iostream>

void test1(const mtm::Schedule& schedule) { schedule.printAllEvents(); }

void test2(const mtm::Schedule& schedule) {
    schedule.printEventDetails(mtm::DateWrap(27, 12, 2020), "Publish Test");
}

void test3(const mtm::Schedule& schedule) {
    schedule.printEventDetails(mtm::DateWrap(5, 1, 2021), "Update Q&A");
}

void test4(const mtm::Schedule& schedule) { schedule.printMonthEvents(12, 2020); }

class MutatingPredicate {
    int counter = 0;

public:
    bool operator()(const mtm::BaseEvent& event) {
        ++counter;
        return true;
    }
};

void test5(const mtm::Schedule& schedule) {
    schedule.printSomeEvents(MutatingPredicate(), true);
}

void testYan1()
{
    mtm::Schedule schedule;
    //notice events should be merged lexicographically
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanB"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanR"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanZ"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanA"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanC"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanX"));
    schedule.printAllEvents();
    cout<< "====test1===="<<endl;
    /*
    ===========Excpected Output==============
    YanA 1/1/2020
    YanB 1/1/2020
    YanC 1/1/2020
    YanR 1/1/2020
    YanX 1/1/2020
    YanZ 1/1/2020
    */
}

void testYan2()
{
    mtm::Schedule schedule;
    //notice events should be merged by date
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(20, 1, 2022), "YanZ"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(30, 1, 2020), "YanR"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, 2020), "YanZ"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(2, 2, 2020), "YanA"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 2, 2020), "YanC"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 2, 2020), "YanB"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(30, 1, 2020), "YanM"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 1, -20), "YanX"));
    try{
        schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
         mtm::DateWrap(1, 1, -20), "YanX"));
    }catch(mtm::EventAlreadyExists&){ 
    }
     try{
        schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
         mtm::DateWrap(1, 2, 2020), "YanC"));
    }catch(mtm::EventAlreadyExists&){ 
    }
    schedule.printAllEvents();
    cout<< "====test2===="<<endl;
    /*
    ===========Excpected Output==============
    YanX 1/1/-20
    YanZ 1/1/2020
    YanM 30/1/2020
    YanR 30/1/2020
    YanB 1/2/2020
    YanC 1/2/2020
    YanA 2/2/2020
    YanZ 20/1/2022
    */
}

class StudentOddFilter {
    public:
    bool operator()(int student) {
        for(int i = 0; i < 100; i++){
            if(student == i*2){
                return true;
            }
        }
        return false;
    }
};


class Predicate {
    public:
    bool operator()(mtm::BaseEvent& event) {
        if(event.getName() == "YanA") return false;
        return true;
    }
};


void testYan3()
{
    mtm::Festival festival(mtm::DateWrap(21, 10, 2020));

    mtm::OpenEvent open(mtm::DateWrap(21, 10, 2020), "YanA");
    open.registerParticipant(20000);
    festival.add(open);

    mtm::ClosedEvent closed(mtm::DateWrap(21, 10, 2020), "YanC");
    for(int i = 10; i < 20; i++){
        closed.addInvitee(i);
    }
    for(int i = 1; i < 10; i++){
        closed.addInvitee(i);
    }
     for(int i = 10; i < 20; i++){
        closed.registerParticipant(i);
    }
    for(int i = 1; i < 10; i++){
        closed.registerParticipant(i);
    }
    festival.add(closed);

    mtm::CustomEvent<StudentOddFilter> custom(mtm::DateWrap(21, 10, 2020),
                                           "YanB", StudentOddFilter());
    for(int i = 10; i < 20; i++){
        custom.registerParticipant(2*i);
    }
    for(int i = 1; i < 10; i++){
        custom.registerParticipant(2*i);
    }
    festival.add(custom);

    mtm::Schedule schedule;
    schedule.addEvents(festival);
    schedule.registerToEvent(mtm::DateWrap(21, 10, 2020),"YanB",198);
    schedule.printSomeEvents(Predicate(), true);
    cout<< "====test3===="<<endl;
    /*
    students should be merged!
    new line between events!
    ===========Excpected Output==============
    YanB 21/10/2020
    2
    4
    6
    8
    10
    12
    14
    16
    18
    20
    22
    24
    26
    28
    30
    32
    34
    36
    38
    198

    YanC 21/10/2020
    1
    2
    3
    4
    5
    6
    7
    8
    9
    10
    11
    12
    13
    14
    15
    16
    17
    18
    */
}

void testYan4()
{
    mtm::Schedule schedule;
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(1, 12, 2020), "YanZ"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(27, 12, 2020), "YanW"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(22, 12, 2020), "YanD"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(18, 12, 2020), "YanY"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(14, 12, 2020), "YanE"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(29, 12, 2020), "YanA"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(2, 12, 2020), "YanM"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(30, 12, 2020), "YanS"));
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
        mtm::DateWrap(25, 12, 2020), "YanR"));

    for(int i = 60 ; i<65; i++){
        schedule.registerToEvent(mtm::DateWrap(1, 12, 2020), "YanZ", i);
    }
    for(int i = 20 ; i<25; i++){
        schedule.registerToEvent(mtm::DateWrap(1, 12, 2020), "YanZ", i);
    }
    for(int i = 5 ; i<10; i++){
        schedule.registerToEvent(mtm::DateWrap(14, 12, 2020), "YanE", i);
    }
    schedule.printEventDetails(mtm::DateWrap(14, 12, 2020), "YanE");
    schedule.printEventDetails(mtm::DateWrap(1, 12, 2020), "YanZ");
    schedule.printEventDetails(mtm::DateWrap(25, 12, 2020), "YanR");
    cout<< "====test4===="<<endl;
    /*
    new line between events!
    ===========Excpected Output==============
    YanE 14/12/2020
    5
    6
    7
    8
    9

    YanZ 1/12/2020
    20
    21
    22
    23
    24
    60
    61
    62
    63
    64

    YanR 25/12/2020
    */
}
typedef void (*Test)(const mtm::Schedule&);
const Test tests[] = {test1, test2, test3, test4, test5, testYan1, testyan2, testYan3, testYan4};

int main(int argc, char* argv[]) {
    mtm::Schedule schedule;
    schedule.addEvents(mtm::OneTimeEvent<mtm::OpenEvent>(
            mtm::DateWrap(27, 12, 2020), "Publish Test"));

    mtm::RecurringEvent<mtm::ClosedEvent> closed(mtm::DateWrap(20, 12, 2020),
                                                 "Update Q&A", 6, 5);
    for (mtm::BaseEvent& event : closed) {
        mtm::ClosedEvent& closed_event = dynamic_cast<mtm::ClosedEvent&>(event);
        closed_event.addInvitee(1337);
        closed_event.addInvitee(850);
        closed_event.addInvitee(1500);
    }
    schedule.addEvents(closed);

    schedule.registerToEvent(mtm::DateWrap(20, 12, 2020), "Update Q&A", 850);
    schedule.registerToEvent(mtm::DateWrap(20, 12, 2020), "Update Q&A", 1500);
    schedule.registerToEvent(mtm::DateWrap(5, 1, 2021), "Update Q&A", 850);
    schedule.registerToEvent(mtm::DateWrap(5, 1, 2021), "Update Q&A", 1500);
    schedule.unregisterFromEvent(mtm::DateWrap(20, 12, 2020), "Update Q&A",
                                 1500);

    if (argc < 2) {
        test1(schedule);
        test2(schedule);
        test3(schedule);
        test4(schedule);
        test5(schedule);
        testYan1();
        testYan2();
        testYan3();
        testYan4();
    } else if (argc > 2) {
        std::cout << "invalid arguments" << std::endl;
    } else {
        int i = std::atoi(argv[1]);
        tests[i - 1](schedule);
    }
    return 0;
}
